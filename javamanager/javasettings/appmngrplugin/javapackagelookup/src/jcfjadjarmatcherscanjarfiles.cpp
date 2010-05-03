/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  This class is used to find matching JAR file for JAD file
*
*/


#include <apmstd.h>
#include "appmngr2midletmanifestreader.h"
#include "javacommonutils.h"
#include "jcfjadjarmatcherscanjarfiles.h"
#include "logger.h"

_LIT(KSearchMask, "*.*");
_LIT(KJarExt, ".jar");

// ---------------------------------------------------------------------------
//    To construct new CJcfJadJarMatcherScanJarFiles.
// -----------------------------------------------------------------------------
CJcfJadJarMatcherScanJarFiles::CJcfJadJarMatcherScanJarFiles(
    MJcfJadJarMatcherObserver* aObs, RFs& aFs) :
        CJcfJadJarMatcherState(aObs),
        iFf(aFs),
        iFs(aFs)
{
}


// ---------------------------------------------------------------------------
//    To destruct CJcfJadJarMatcherScanJarFiles. Cancel any
//    outstanding request. Delete all C-objects owned by this object.
// -----------------------------------------------------------------------------
CJcfJadJarMatcherScanJarFiles::~CJcfJadJarMatcherScanJarFiles()
{
    Cancel();
    Cleanup();
}


// ---------------------------------------------------------------------------
//    To execute this state. This method starts long running task,
//    which seeks JAR files and does JAD/JAR matching for them.
// -----------------------------------------------------------------------------
void CJcfJadJarMatcherScanJarFiles::ExecuteL(const TDesC& aJadName,
        const TDesC& aDirName,
        TDes* aJarNamePtr)
{
    JELOG2(EJavaStorage);

    Cleanup();

    std::auto_ptr<AppMngr2MidletManifestReader> reader(
        new(ELeave) AppMngr2MidletManifestReader(iFs));

    reader->ReadManifestL(aJadName, iAttributes);

    // Take only name without path and postfix.
    TInt postfixOffset = aJadName.LocateReverse('.');
    TInt slashOffset = aJadName.LocateReverse('\\');
    ++slashOffset;

    TPtrC jadName = aJadName.Mid(slashOffset, (postfixOffset-slashOffset));

    iFullJadName.reset(jadName.Alloc());

    iDir = aDirName.AllocL();
    iJarFileNamePtr = aJarNamePtr;

    iState = EOpenJadFile;
    SetObjectActive();
}


// ---------------------------------------------------------------------------
//    To match JAR and JAD files. This method seeks JAR files from
//    given directory. If matching JAR file is found, then notification is sent
//    to the observer of this object and execution of this active object is
//    stopped. Otherwise if matching JAR file is not found and relative path
//    (without driver) is defined, method starts to seek the file from same
//    directory in different driver. All available drivers searched in
//    descenting order: Y: -> A: and finally Z:. The JAD file 'jad' and the JAR
//    file 'jar' is said to be matching iff Match('jad', 'jar') == true
//    Otherwise files are said to be unmatching. This method is assumed to be
//    long running. When all directories are scanned, complete notification
//    (KErrNotFound) is sent.
// -----------------------------------------------------------------------------
void CJcfJadJarMatcherScanJarFiles::RunL()
{
    switch (iState)
    {
        // EOpenJadFile Preconditions. - Successfull call of Cleanup().
        // Postconditions. - iFl is initialized.
    case EOpenJadFile:
    {
        TParsePtrC parse(iDir->Des());
        TPtrC drive = parse.Drive();

        // Check whether drive is specified (absolute path) or not
        // (relative path).
        iIsAbsolute = (0 != drive.Length());

        if (iIsAbsolute)
        {
            User::LeaveIfError(iFf.FindWildByDir(
                                   KSearchMask, parse.DriveAndPath(), iFl));
        }
        else
        {
            User::LeaveIfError(iFf.FindWildByDir(
                                   KSearchMask, parse.Path(), iFl));
        }

        // Start scanning loop.
        iIndex = 0;
        iAdHocGuess = ETrue;
        iState = EScanJarFile;
        SetObjectActive();
        break;
    }

    // EScanJarFile Preconditions. - iFl is initialized. Postconditions. -
    // One file is checked whether it matches or not.
    case EScanJarFile:
        if (iIndex < iFl->Count())
        {
            // Generate a name for a file to be opened next. An ad hoc
            // guess is that it's likely that the matching file has same
            // name as the argument. If the guess is right, the whole
            // matching is very fast. On the other hand if the guess is
            // wrong, the only penalty is that one file is matched twice.
            TBool  foundMatchingJar = EFalse;
            std::auto_ptr<HBufC>nameBuf(HBufC::NewL(KMaxFileName));
            TPtr namePtr(nameBuf->Des());
            TParsePtrC driveAndPath(iFf.File());
            namePtr.Append(driveAndPath.Drive());
            namePtr.Append(driveAndPath.Path());

            if (iAdHocGuess)
            {
                iAdHocGuess = EFalse;
                TInt index = 0;

                // Check (case-sensitive) the names in the file list.
                for (; index < iFl->Count(); index++)
                {
                    TInt offset = (*iFl)[index].iName.LocateReverse('.');
                    if (KErrNotFound != offset)
                    {
                        TPtrC name((*iFl)[index].iName.Left(offset));
                        TPtrC jadName(iFullJadName->Des());

                        if (0 == jadName.Compare(name))
                        {
                            // The beginning of the file name matches,
                            // but is the suffix ".jar" ?
                            TPtrC suffix((*iFl)[index].iName.Mid(offset));
                            if (0 == suffix.CompareF(KJarExt))
                            {
                                foundMatchingJar = ETrue;
                                break;
                            }
                        }
                    }
                }

                if (foundMatchingJar)
                {
                    namePtr.Append((*iFl)[index].iName);
                }
                else
                {
                    namePtr.Append((*iFl)[iIndex++].iName);
                }
            }
            else
            {
                namePtr.Append((*iFl)[iIndex++].iName);
            }

            TParsePtrC absName(*nameBuf);

            // Parse JAD File attributes for the matching.
            std::auto_ptr<AppMngr2MidletManifestReader> reader(
                new(ELeave) AppMngr2MidletManifestReader(iFs));

            RPointerArray<Java::MJavaAttribute>jarAttributes;

            TRAPD(parseErr, reader->ReadManifestL(namePtr, jarAttributes));

            if (KErrNone == parseErr)
            {
                if (Match(iAttributes, jarAttributes))
                {
                    LOG(EJavaStorage, EInfo, "JAR and JAD match");
                    iJarFileNamePtr->SetLength(0);
                    iJarFileNamePtr->Append(namePtr);

                    iValue = KErrNone;
                    iState = EEnd;
                }
            }
            jarAttributes.ResetAndDestroy();
            SetObjectActive();
        }
        else
        {
            delete iFl;
            iFl = NULL;

            if (iIsAbsolute || KErrNotFound == iFf.FindWild(iFl))
            {
                // End of loop.
                iValue = KErrNotFound;
                iState = EEnd;
            }

            iIndex = 0;
            iAdHocGuess = ETrue;
            SetObjectActive();
        }
        break;

        // EEnd Preconditions. - None. Postconditions. - Matching round is
        // completed. The ExecuteL() activates a new one.
    case EEnd:
        Cleanup();
        NotifyObserver(iValue);
    }
}


// ---------------------------------------------------------------------------
//    To cancel this object activity. This method cancels the JAR
//    file scanning.
// -----------------------------------------------------------------------------
void CJcfJadJarMatcherScanJarFiles::DoCancel()
{
    NotifyObserver(KErrCancel);
    Cleanup();
}


// ---------------------------------------------------------------------------
//    To complete pending request and set this object active.
// -----------------------------------------------------------------------------
void CJcfJadJarMatcherScanJarFiles::SetObjectActive()
{
    TRequestStatus *status = &iStatus;
    User::RequestComplete(status, KErrNone);
    if (!IsActive())
    {
        SetActive();
    }
}

// ---------------------------------------------------------------------------
//    To do clean-up for next round.
// -----------------------------------------------------------------------------
void CJcfJadJarMatcherScanJarFiles::Cleanup()
{
    delete iDir;
    iDir = NULL;
    delete iFl;
    iFl = NULL;
    iFullJadName.reset(0);
    iAttributes.ResetAndDestroy();

    iState = EBegin;
}

