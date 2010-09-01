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
* Description:  This class is used to find matching JAD file for given JAR
*                file.
*
*/


#include "appmngr2midletmanifestreader.h"
#include "javacommonutils.h"
#include "jcfjadjarmatcherscanjadfiles.h"
#include "logger.h"

_LIT(KJadMask, "*.jad");

// ---------------------------------------------------------------------------
//    To construct new CJcfJadJarMatcherScanJadFiles.
// -----------------------------------------------------------------------------
CJcfJadJarMatcherScanJadFiles::CJcfJadJarMatcherScanJadFiles(
    MJcfJadJarMatcherObserver*  aObs, RFs&  aFs) :
        CJcfJadJarMatcherState(aObs),
        iFf(aFs),
        iFs(aFs)
{
}


// ---------------------------------------------------------------------------
//    To destruct CJcfJadJarMatcherScanJadFiles. Cancel any
//    outstanding request. Delete all C-objects owned by this object.
// -----------------------------------------------------------------------------
CJcfJadJarMatcherScanJadFiles::~CJcfJadJarMatcherScanJadFiles()
{
    Cancel();
    Cleanup();
}


// ---------------------------------------------------------------------------
//    To execute this state. This method starts long running task,
//    which seeks JAD files and does JAD/JAR matching for them. Make clean-up.
//    Open JAR file asynchronously. Set new state.
// -----------------------------------------------------------------------------
void CJcfJadJarMatcherScanJadFiles::ExecuteL(const TDesC& aJarName,
        const TDesC& aDirName,
        TDes* aJadNamePtr)
{
    JELOG2(EJavaStorage);
    Cleanup();

    std::auto_ptr<AppMngr2MidletManifestReader> reader(
        new(ELeave) AppMngr2MidletManifestReader(iFs));

    reader->ReadManifestL(aJarName, iAttributes);

    iIndex = 0;
    iDir = aDirName.AllocL();

    // Take only name without path and postfix.
    TInt postfixOffset = aJarName.LocateReverse('.');
    TInt slashOffset = aJarName.LocateReverse('\\');
    ++slashOffset;

    TPtrC jarName = aJarName.Mid(slashOffset, (postfixOffset-slashOffset));

    iFullJarName.reset(jarName.Alloc());
    iJadFileNamePtr = aJadNamePtr;

    iState = EOpenJarFile;
    SetObjectActive();
}


// ---------------------------------------------------------------------------
//    To match JAR and JAD files. This method seeks JAD files from
//    given directory. If matching JAD file is found, then notification is sent
//    to the observer of this object and execution of this active object is
//    stopped. Otherwise if matching JAD file is not found and relative path
//    (without driver) is defined, method starts to seek the file from same
//    directory in different driver. All available drivers searched in
//    descenting order: Y: -> A: and finally Z:. The JAD file 'jad' and the JAR
//    file 'jar' is said to be matching if Match('jad', 'jar') == true
//    Otherwise files are said to be unmatching. This method is assumed to be
//    long running. When all directories are scanned, complete notification
//    (KErrNotFound) is sent.
// -----------------------------------------------------------------------------
void CJcfJadJarMatcherScanJadFiles::RunL()
{
    switch (iState)
    {
        // EOpenJarFile Preconditions. - Successfull call of Cleanup().
        // Postconditions. - iFl is initialized.
    case EOpenJarFile:
        if (KErrNone == iStatus.Int())
        {
            TParsePtrC parse(iDir->Des());
            TPtrC drive = parse.Drive();
            iIsAbsolute = (0 != drive.Length());

            // Check whether a drive is specified (absolute path) or not
            // (relative path).
            if (iIsAbsolute)
            {
                TPtrC path = parse.DriveAndPath();
                User::LeaveIfError(iFf.FindWildByDir(KJadMask, path, iFl));
            }
            else
            {
                TPtrC path = parse.Path();
                User::LeaveIfError(iFf.FindWildByDir(KJadMask, path, iFl));
            }

            // Start scanning loop.
            iIndex = 0;
            iAdHocGuess = ETrue;
            iState = EScanJadFile;
        }
        else
        {
            iValue = KErrArgument;
            iState = EEnd;
        }

        SetObjectActive();
        break;

        // EScanJadFile Preconditions. - iFl is initialized. Postconditions. -
        // One file is checked whether it matches or not.
    case EScanJadFile:
        if (iIndex < iFl->Count())
        {
            // Generate a name for a file to be opened next. An ad hoc
            // guess is that it's likely that the matching file has same
            // name as the argument. If the guess is right, the whole
            // matching is very fast. On the other hand if the guess is
            // wrong, the only penalty is that one file is matched twice.
            std::auto_ptr<HBufC>nameBuf(HBufC::NewL(KMaxFileName));
            TPtr namePtr(nameBuf->Des());
            TParsePtrC driveAndPath(iFf.File());
            namePtr.Append(driveAndPath.Drive());
            namePtr.Append(driveAndPath.Path());

            if (iAdHocGuess)
            {
                iAdHocGuess = EFalse;
                TBool find = EFalse;
                TInt index = 0;
                TParsePtrC argName(*iFullJarName);

                // Check (case-sensitive) the names in the file list.
                for (; index < iFl->Count(); index++)
                {
                    TInt offset = (*iFl)[index].iName.LocateReverse('.');
                    if (KErrNotFound != offset)
                    {
                        TPtrC name((*iFl)[index].iName.Left(offset));
                        if (0 == argName.Name().Compare(name))
                        {
                            find = ETrue;
                            break;
                        }
                    }
                }

                if (find)
                {
                    namePtr.Append((*iFl)[index].iName);
                }
                else
                {
                    namePtr.Append((*iFl)[iIndex++].iName);
                }
            }
            else    // !iAdHocGuess
            {
                namePtr.Append((*iFl)[iIndex++].iName);
            }

            LOG1WSTR(EJavaStorage, EInfo, "Matching to jad file: %s",
                     (const wchar_t*)namePtr.PtrZ());

            // Parse JAD File attributes for the matching.
            std::auto_ptr<AppMngr2MidletManifestReader> reader(
                new(ELeave) AppMngr2MidletManifestReader(iFs));

            RPointerArray<Java::MJavaAttribute>jadAttributes;
            reader->ReadManifestL(namePtr, jadAttributes);

            if (Match(iAttributes, jadAttributes))
            {
                LOG(EJavaStorage, EInfo, "JAR and JAD match");
                iJadFileNamePtr->SetLength(0);
                iJadFileNamePtr->Append(namePtr);

                iValue = KErrNone;
                iState = EEnd;
            }

            jadAttributes.ResetAndDestroy();
            SetObjectActive();
        }
        else    // all entries in iF1 have scanned
        {
            delete iFl;
            iFl = NULL;

            // If path was absolute, there wasn't matching JAD file.
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
        // completed. The ExecuteL() activates a new one
    case EEnd:
        Cleanup();
        NotifyObserver(iValue);
    }
}


// ---------------------------------------------------------------------------
//    To cancel this object activity. This method cancels the JAD
//    file scanning.
// -----------------------------------------------------------------------------
void CJcfJadJarMatcherScanJadFiles::DoCancel()
{
    NotifyObserver(KErrCancel);
    Cleanup();
}


// ---------------------------------------------------------------------------
//    To complete pending request and set this object active.
// -----------------------------------------------------------------------------
void CJcfJadJarMatcherScanJadFiles::SetObjectActive()
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
void CJcfJadJarMatcherScanJadFiles::Cleanup()
{
    delete iDir;
    iDir = NULL;
    delete iFl;
    iFl = NULL;
    iFullJarName.reset(0);
    iAttributes.ResetAndDestroy();

    iState = EBegin;
}

