/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  S60FileSystemUtilities
 *
*/


#include <f32file.h>
#include "logger.h"
#include "jstringutils.h"
#include "s60filesystemutilities.h"
#include "fileutilities.h"

using namespace java::fileutils;

/*
 * Implementation of S60FileSystemUtilities.h
 *
 */
using namespace std;

void S60FileSystemUtilities::GetAvailableSizeL(TInt64& aRetSize,
        TChar& aDriveChar)
{
    JELOG2(EJavaFile);
    //Note: Uri type check is not needed here.
    TVolumeInfo volumeInfo;
    RFs iRFs;
    User::LeaveIfError(iRFs.Connect());
    TInt drive = EDriveC;
    RFs::CharToDrive(aDriveChar, drive);

    // If it is not a valid root, return immediately
    int error = 0;
    if (RFs::IsValidDrive(drive))
    {
        error = iRFs.Volume(volumeInfo, drive);
    }
    else
    {
        iRFs.Close();
        User::Leave(KErrArgument);
    }

    if (KErrNone != error)
    {
        iRFs.Close();
        User::Leave(error);
    }

    aRetSize = volumeInfo.iFree;
    iRFs.Close();
}

/**
 *
 */
void S60FileSystemUtilities::GetUsedSizeL(TInt64& aRetSize, TChar& aDriveChar)
{
    JELOG2(EJavaFile);
    //Note: Uri type check is not needed here.
    TVolumeInfo volumeInfo;
    RFs iRFs;
    User::LeaveIfError(iRFs.Connect());
    TInt drive = EDriveC;
    RFs::CharToDrive(aDriveChar, drive);

    // If it is not a valid root, return immediately
    int error = 0;
    if (RFs::IsValidDrive(drive))
    {
        error = iRFs.Volume(volumeInfo, drive);
    }
    else
    {
        iRFs.Close();
        User::Leave(KErrArgument);
    }

    if (KErrNone != error)
    {
        iRFs.Close();
        User::Leave(error);
    }

    aRetSize = volumeInfo.iSize - volumeInfo.iFree;
    iRFs.Close();
}

void S60FileSystemUtilities::IsHiddenL(TBool& aIsHidden, const TDesC &aFile)
{
    JELOG2(EJavaFile);
    RFs iRFs;
    User::LeaveIfError(iRFs.Connect());
    TEntry entry;

    TInt length = aFile.Length();

    HBufC* fileUrl = HBufC::NewL(length);
    *fileUrl = aFile;

    TPtr uriBufPtr(fileUrl->Des());
    ReplaceCharacters(uriBufPtr, '/', '\\');

    User::LeaveIfError(iRFs.Entry(uriBufPtr, entry));
    aIsHidden = entry.IsHidden();
    iRFs.Close();

    delete fileUrl; //allocated on heap, so deleting it
}

void S60FileSystemUtilities::GetTotalSizeL(TInt64& aRetSize, TChar& aDriveChar)
{
    JELOG2(EJavaFile);
    //Note: Uri type check is not needed here.
    TVolumeInfo volumeInfo;
    RFs iRFs;
    User::LeaveIfError(iRFs.Connect());
    TInt drive = EDriveC;
    RFs::CharToDrive(aDriveChar, drive);

    // If it is not a valid root, return immediately
    int error = 0;
    if (RFs::IsValidDrive(drive))
    {
        error = iRFs.Volume(volumeInfo, drive);
    }
    else
    {
        iRFs.Close();
        User::Leave(KErrArgument);
    }

    if (KErrNone != error)
    {
        iRFs.Close();
        User::Leave(error);
    }
    aRetSize = volumeInfo.iSize;
    iRFs.Close();
}

EXPORT_C void S60FileSystemUtilities::SetHiddenL(TDesC& aFile, TBool aHide)
{
    JELOG2(EJavaFile);
    //Might need to close the file before attempting to change att.
    //need to verify it
    RFs iRFs;
    TInt err = KErrNone;
    User::LeaveIfError(iRFs.Connect());

    HBufC* fileUrl = HBufC::NewL(aFile.Length());
    *fileUrl = aFile;

    TPtr uriBufPtr(fileUrl->Des());
    ReplaceCharacters(uriBufPtr, '/', '\\');

    if (aHide)
        err = iRFs.SetAtt(uriBufPtr, KEntryAttHidden, 0);
    else
        err = iRFs.SetAtt(uriBufPtr, 0, KEntryAttHidden);

    iRFs.Close();
    delete fileUrl;
    User::LeaveIfError(err);
}

void S60FileSystemUtilities::ReplaceCharacters(TPtr& aPtr,
        TUint8 aReplacedChar, TUint8 aNewChar)
{
    ReplaceCharacters(aPtr, aReplacedChar, aNewChar, EFalse);
}

/**
 *
 */
void S60FileSystemUtilities::ReplaceCharacters(TPtr& aPtr,
        TUint8 aReplacedChar, TUint8 aNewChar, TBool aOnlyFirstMatch)
{
    for (TInt i = 0; i < aPtr.Length(); ++i)
    {
        if (aReplacedChar == aPtr[i])
        {
            aPtr[i] = aNewChar;
            if (aOnlyFirstMatch)
                return;
        }
    }
}

TDesC16& S60FileSystemUtilities::GetSupportedDrives()
{
    JELOG2(EJavaFile);
    //TBuf16<80> driveString;
    HBufC* driveString = HBufC::NewMax(80);
    TDriveList driveList;
    TInt driveNumber = EDriveA;
    TChar driveChar;
    TChar colon = ':';
    TChar separator = '*';

    RFs session;
    int error = session.Connect();
    if (KErrNone == error)
    {
        error = session.DriveList(driveList);
        if (KErrNone == error)
        {
            for (; driveNumber <= EDriveZ; driveNumber++)
            {
                if (driveList[driveNumber])
                {
                    session.DriveToChar(driveNumber, driveChar);
                    (driveString->Des()).Append(driveChar);
                    (driveString->Des()).Append(colon);
                    (driveString->Des()).Append(separator);
                }
            }
        }
    }
    session.Close();
    return *driveString;
}
