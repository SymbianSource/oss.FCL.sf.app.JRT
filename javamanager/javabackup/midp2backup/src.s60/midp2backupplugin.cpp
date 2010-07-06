/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of Midp2BackupPlugin
*
*/


#include "midp2backupplugin.h"
#include "midp2backupdataids.h"
#include "apparcbackuputil.h"
#include "javastoragebackuputil.h"
#include "javaversionbackuputil.h"

#include "javastoragenames.h"
#include "javastorageentry.h"
#include "javastorage.h"

#include "javacommonutils.h"
#include "logger.h"

#include <connect/sbdefs.h>
#include <s32mem.h>
#include <s32file.h>
#include <f32file.h>
#include <apgcli.h>


using namespace std;
using namespace java::storage;
using namespace java::backup;

// ======== MEMBER FUNCTIONS ========

CMidp2BackupPlugin::CMidp2BackupPlugin()
{
    LOG(EBackup, EInfo, "CMidp2BackupPlugin constructor");
}

void CMidp2BackupPlugin::ConstructL()
{
    LOG(EBackup, EInfo, "CMidp2BackupPlugin::ConstructL");

    iIconDescIndex = 0;
    iBufferSpaceLeft = 0;
    iFirstCallToGetBackupDataSection = ETrue;
    iStorageDataBackup = ETrue;
    iStorageDataRestore = ETrue;
    iJavaVersionInfoWritten = EFalse;
    iRestoreState = EVersion;

    User::LeaveIfError(iFs.Connect());
    // to share file handles with AppArc
    User::LeaveIfError(iFs.ShareProtected());

    iStorageBackupUtil = CStorageBackupUtil::NewL();
    iAppArcUtil = CAppArcBackupUtil::NewL(iFs);
}

CMidp2BackupPlugin* CMidp2BackupPlugin::NewL()
{
    LOG(EBackup, EInfo, "CMidp2BackupPlugin::NewL");

    CMidp2BackupPlugin* self = new(ELeave) CMidp2BackupPlugin();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
}

CMidp2BackupPlugin::~CMidp2BackupPlugin()
{
    LOG(EBackup, EInfo, "CMidp2BackupPlugin destructor");

    iFs.Close();
    iIconUidArray.Close();
    delete iAppArcUtil;
    delete iIconDesc;
    delete iRestoreIconDesc;
    delete iStorageBackupUtil;
}

// from base class CBackupPlugin

void CMidp2BackupPlugin::PrepareForBURL(TInt aBackupStateValue)
{
    (void)aBackupStateValue;  // Just to suppress a warning in release builds
    LOG1(EBackup, EInfo, "CMidp2BackupPlugin::PrepareForBURL, BUR state: %d", aBackupStateValue);
}

void CMidp2BackupPlugin::ReceiveSnapshotDataL(TDriveNumber /* aDrive */, TDesC8& /* aBuffer */, TBool /* aLastSection */)
{
    JELOG2(EBackup);
    User::Leave(KErrNotSupported);
}

TUint CMidp2BackupPlugin::GetExpectedDataSize(TDriveNumber /* aDrive */)
{
    JELOG2(EBackup);
    return 0;
}

void CMidp2BackupPlugin::GetSnapshotDataL(TDriveNumber /* aDrive */, TPtr8& /* aBuffer */, TBool& /* aFinished */)
{
    JELOG2(EBackup);
    User::Leave(KErrNotSupported);
}

void CMidp2BackupPlugin::InitialiseGetBackupDataL(TDriveNumber aDrive)
{
    LOG(EBackup, EInfo, "CMidp2BackupPlugin::InitialiseGetBackupDataL");
    iDrive = aDrive;
}

void CMidp2BackupPlugin::GetBackupDataSectionL(TPtr8& aBuffer, TBool& aFinished)
{
    LOG(EBackup, EInfo, "CMidp2BackupPlugin::GetBackupDataSectionL");

    iBufferSpaceLeft = aBuffer.MaxLength();
    RDesWriteStream stream(aBuffer);
    CleanupClosePushL(stream);

    if (!iJavaVersionInfoWritten)
    {
        JavaVersionBackupUtil::WriteJavaVersionL(stream);
        iJavaVersionInfoWritten = ETrue;
        iBufferSpaceLeft -= 8;
    }

    if (iStorageDataBackup)
    {
        iStorageBackupUtil->BackupStorageDataL(stream, iStorageDataBackup, iBufferSpaceLeft);
        aFinished = EFalse;
    }

    else
    {
        if (iFirstCallToGetBackupDataSection)
        {
            LOG(EBackup, EInfo, "First Call To GetBackupDataSection");
            // Get the uids of midlets whose icons are to be backed up
            iIconFilePointer = 0;
            ResetIconArray();
            iAppArcUtil->GetMidletsFromAppArcL(iIconUidArray, iDrive);

            /* Streaming MMC's unique Id, if current drive is MMC
               and there's at least one midlet on the drive*/
            if (iIconUidArray.Count() > 0 && iDrive >= EDriveE && iDrive <= EDriveZ)
            {
                TUint32 mmcId = MmcIdL();

                if (mmcId != 0)
                {
                    LOG1(EBackup,EInfo, "Write MMC id: %u", mmcId);
                    stream.WriteUint32L(mmcId);
                    iBufferSpaceLeft -= sizeof(mmcId);
                }
            }
            // Setting flag to EFalse
            iFirstCallToGetBackupDataSection = EFalse;
        }

        // Next icon uid and filename
        TUid currentUid;
        HBufC* fullFileName = NULL;

        // Streaming leftover part of icon file
        if (iIconDescIndex)
        {
            LOG(EBackup, EInfo, "Streaming leftover part of icon file");

            /* if the icon file is bigger than the space in the buffer, write only
             part of it and fill iLeftover with remaining part. */
            if (iIconDesc->Size() - iIconDescIndex > iBufferSpaceLeft)
            {
                stream.WriteL(iIconDesc->Mid(iIconDescIndex), iBufferSpaceLeft);
                iIconDescIndex += iBufferSpaceLeft;
            }
            else
            {
                stream.WriteL(iIconDesc->Mid(iIconDescIndex));
                iIconDescIndex = 0;
            }

            aFinished = EFalse;
        }

        // Get new icon file
        else if (NextIcon(currentUid, fullFileName))
        {
            LOG1(EBackup, EInfo, "New icon file, id: %d", currentUid.iUid);

            // Readstream for icon file
            RFileReadStream fileStream;

            User::LeaveIfError(fileStream.Open(iFs, *fullFileName, EFileRead));
            CleanupClosePushL(fileStream);

            TEntry* entry = new(ELeave) TEntry();
            CleanupStack::PushL(entry);

            iFs.Entry(*fullFileName, *entry);
            TInt32 size = entry->iSize;

            // Write icon file content into iIconDesc
            LOG(EBackup, EInfo, "Write icon file content into iIconDesc");
            delete iIconDesc;
            iIconDesc = NULL;
            iIconDesc = HBufC8::NewL(size);

            TPtr8 buf = iIconDesc->Des();
            fileStream.ReadL(buf, size);

            // get the group name of the midlet from AppArc
            TBuf< KApaMaxAppGroupName >groupName(iAppArcUtil->GetMidletGroupName(currentUid));
            TInt groupNameSize = groupName.Size();

            // Write the size of the data
            LOG(EBackup,EInfo, "Write the size of the data");

            TParsePtrC parse(*fullFileName);
            TPtrC fileName = parse.NameAndExt();
            TInt32 fileNameSize = fileName.Size();

            stream.WriteInt32L(sizeof(TUid)    // Uid
                               + sizeof(TInt32)   // fileNameSize
                               + fileNameSize // filename
                               + sizeof(TInt32)   // number of icons
                               + sizeof(TInt32)   // groupnamesize
                               + groupNameSize
                               + size // icon file
                              );
            iBufferSpaceLeft -= sizeof(TInt32);

            // Write the Uid of the midlet
            LOG(EBackup, EInfo, "Write the Uid of the midlet");
            stream.WriteInt32L(currentUid.iUid);
            iBufferSpaceLeft -= sizeof(TUid);

            // Write the size of the icon filename
            LOG(EBackup, EInfo, "Write the size of the icon filename");
            stream.WriteInt32L(fileNameSize);
            iBufferSpaceLeft -= sizeof(TInt32);

            // Write icon filename
            LOG(EBackup, EInfo, "Write icon filename");
            stream.WriteL(fileName);
            iBufferSpaceLeft -= fileNameSize;

            // Write number of icons in midlet
            LOG(EBackup, EInfo, "Write number of icons in midlet");
            TInt iconCount;
            iAppArcUtil->NumberOfOwnDefinedIcons(TUid::Uid(currentUid.iUid), iconCount);
            stream.WriteInt32L(iconCount);
            iBufferSpaceLeft -= sizeof(TInt32);

            // TPtr8 groupName(iAppArcUtil->GetMidletGroupName(currentUid));
            LOG(EBackup, EInfo, "Write the size of the Group name of the midlet");
            stream.WriteInt32L(groupNameSize);
            iBufferSpaceLeft -= sizeof(TInt32);

            if (groupNameSize != 0)
            {
                LOG(EBackup, EInfo, "Write the Group name of the midlet");
                stream.WriteL(groupName);
                iBufferSpaceLeft -= groupNameSize;
            }

            /* if the icon file is bigger than the space in the buffer,
               write only part of it and fill iLeftover with remaining part */
            LOG(EBackup, EInfo, "Write icon");
            if (size > iBufferSpaceLeft)
            {
                stream.WriteL(buf, iBufferSpaceLeft);
                iIconDescIndex = iBufferSpaceLeft;
            }
            else
            {
                stream.WriteL(buf, size);
                iIconDescIndex = 0;
            }

            aFinished = EFalse;

            CleanupStack::PopAndDestroy(&fileStream);
            CleanupStack::PopAndDestroy(entry);
        }

        else
        {
            aFinished = ETrue;
            iFirstCallToGetBackupDataSection = ETrue;
            iStorageDataBackup = ETrue;
        }

        delete fullFileName;
    }
    CleanupStack::PopAndDestroy(&stream);
}


void CMidp2BackupPlugin::InitialiseRestoreBaseDataL(TDriveNumber aDrive)
{
    LOG(EBackup, EInfo, "CMidp2BackupPlugin::InitialiseRestoreBaseDataL");
    iDrive = aDrive;
}

void CMidp2BackupPlugin::RestoreBaseDataSectionL(TDesC8& aBuffer, TBool aFinished)
{
    LOG(EBackup, EInfo, "CMidp2BackupPlugin::RestoreBaseDataSectionL");

    iBufferSpaceLeft = aBuffer.Size();
    RDesReadStream stream(aBuffer);
    CleanupClosePushL(stream);
    TBool versionCheck = ETrue;

    if (((iRestoreState == EVersion) || (iRestoreState == EInstall)) && iBufferSpaceLeft >= 8)
    {
        if (iRestoreState == EInstall)
        {
            versionCheck = EFalse;
        }
        TUint ret = JavaVersionBackupUtil::CheckJavaVersionL(stream,iDrive,versionCheck);
        if (1==ret)
        {
            // Here set the satus to restore finished, so that control goes back to SBE & SBE calls B&R for another drive
            iRestoreState = EInstall;
        }
        else
        {
            /* version information has been written to stream.
             So change restore state to restore storage data  */
            iRestoreState = EStorage;
        }
        // version information is of length 8 bytes.
        iBufferSpaceLeft -= 8;
    }

    if (iRestoreState == EStorage)
    {
        iStorageBackupUtil -> RestoreStorageDataL(stream, iRestoreState, iBufferSpaceLeft);
    }

    // Only at first call
    if ((iRestoreState != EStorage && iRestoreState != EVersion) && (iRestoreState != EInstall))
    {
        if (iRestoreState == EAppArc)
        {
            // Making AppArc deregistrations
            TRAPD(err, iAppArcUtil->DeregisterAppsL(iDrive));

            if (err)
            {
                ELOG(EBackup, "Leave in deregistering apps");
            }
        }

        // Process the data buffer for restore
        ProcessBufferL(stream);

        if (aFinished)
        {
            // Set state to EStorage
            iRestoreState = EStorage;
        }
    }

    aFinished = ETrue;
    CleanupStack::PopAndDestroy(&stream);
}

void CMidp2BackupPlugin::InitialiseRestoreIncrementDataL(TDriveNumber /* aDrive */)
{
    JELOG2(EBackup);
    User::Leave(KErrNotSupported);
}

void CMidp2BackupPlugin::RestoreIncrementDataSectionL(TDesC8& /* aBuffer */, TBool /* aFinished */)
{
    JELOG2(EBackup);
    User::Leave(KErrNotSupported);
}

void CMidp2BackupPlugin::RestoreComplete(TDriveNumber /* aDrive */)
{
    LOG(EBackup, EInfo, "CMidp2BackupPlugin::RestoreComplete");
}

TUint CMidp2BackupPlugin::MmcIdL()
{
    LOG(EBackup, EInfo, "CMidp2BackupPlugin::MmcIdL()");

    TVolumeInfo* volumeInfo = new(ELeave) TVolumeInfo();

    TInt err = iFs.Volume(*volumeInfo, iDrive);

    if (!err)
    {
        TUint mmcId = volumeInfo->iUniqueID;
        return mmcId;
    }
    else
    {
        return 0;
    }


}

void CMidp2BackupPlugin::ProcessBufferL(RDesReadStream& stream)
{
    LOG(EBackup, EInfo, "CMidp2BackupPlugin::ProcessBuffer()");

    // Cycle until there's data in the buffer
    while (iBufferSpaceLeft > 0)
    {
        switch (iRestoreState)
        {
        case EAppArc:
            ProcessFirstBufferL(stream, iBufferSpaceLeft);
            break;

        case EInIcon:
            ProcessInIconL(stream, iBufferSpaceLeft);
            break;

        case EInSize:
            ProcessInSizeL(stream, iBufferSpaceLeft);
            break;

        default:
            CleanupStack::PopAndDestroy(&stream);
            User::Leave(KErrNotSupported);
        }
    }
}


_LIT(KStarWildCard, "*.*");

void CMidp2BackupPlugin::RestoreMmcRegistryL()
{
    LOG(EBackup, EInfo, "CMidp2BackupPlugin::RestoreMmcRegistry()");

    /* If backed up MMC is not the same as the current MMC, copy the entries
       from the directory of the backed up MMC to the directory
       of the current MMC */
    TUint32 mmc = MmcIdL();

    if (iBackupMmc != mmc)
    {
        // Create path to old place
        TPath path_old;
        CreateMmcPath(path_old, iDrive, iBackupMmc);

        // Create path to new place
        TPath path_new;
        CreateMmcPath(path_new, iDrive, mmc);

        // Get old files list
        CFileMan* fileMan = CFileMan::NewL(iFs);

        CleanupStack::PushL(fileMan);

        // Copy to new place
        TInt err = iFs.MkDir(path_new);
        ILOG2(EBackup, "MkDir, status: %d, path=%S", err, path_new.PtrZ());

        path_old.Append(KStarWildCard);
        err = fileMan->Copy(path_old, path_new);

        ILOG1(EBackup, "File copy, status: %d", err);

        CleanupStack::PopAndDestroy();
    }
}

_LIT(KPrivatePath, "C:\\private\\");

void CMidp2BackupPlugin::CreateMmcPath(TDes& aPathName, TInt aDrive, TUint aUniqueId)
{
    LOG(EBackup, EInfo, "CMidp2BackupPlugin::CreateMmcPath()");

    aPathName.Zero();
    aPathName.Append(KPrivatePath);
    aPathName.AppendNum(KRegistryServerUid, EHex);
    aPathName.Append(KPathDelimiter);

    // append drive name
    TDriveUnit driveUnit(aDrive);
    aPathName.Append(driveUnit.Name().Left(1));
    aPathName.Append(KPathDelimiter);

    TUint64 id = aUniqueId;
    aPathName.AppendNum(id, EHex);
    aPathName.Append(KPathDelimiter);
}

TBool CMidp2BackupPlugin::NextIcon(TUid& aCurrentUid, HBufC*& aFullFileName)
{
    LOG(EBackup, EInfo, "CMidp2BackupPlugin::NextIcon()");

    if (iIconUidArray.Count() > iIconFilePointer)
    {
        aCurrentUid = iIconUidArray[iIconFilePointer];
        iAppArcUtil->GetIconFilename(iIconUidArray[iIconFilePointer], aFullFileName);
        iIconFilePointer++;
        return ETrue;
    }
    else
    {
        // There are no more icon files
        return EFalse;
    }
}

void CMidp2BackupPlugin::ProcessFirstBufferL(RDesReadStream& aStream, TInt& aBufferSpaceLeft)
{
    LOG(EBackup, EInfo, "CMidp2BackupPlugin::ProcessFirstBuffer()");

    if (iDrive >= EDriveE && iDrive <= EDriveZ)
    {
        // Read MMC id and restore MMC entries if needed
        iBackupMmc = aStream.ReadUint32L();
        aBufferSpaceLeft -= sizeof(iBackupMmc);
        LOG1(EBackup, EInfo, "Read MMC Id: %u", iBackupMmc);
        RestoreMmcRegistryL();
    }
    if (aBufferSpaceLeft >= sizeof(TInt32))
    {
        iIconFileSizeLeft = aStream.ReadInt32L();
        aBufferSpaceLeft -= sizeof(iIconFileSizeLeft);
        iRestoreIconDesc = HBufC8::NewL(iIconFileSizeLeft);
        iRestoreState = EInIcon;
    }
    else
    {
        // No more data to read
        aBufferSpaceLeft = 0;
    }
}

void CMidp2BackupPlugin::ProcessInIconL(RDesReadStream& aStream, TInt& aBufferSpaceLeft)
{
    LOG(EBackup, EInfo, "CMidp2BackupPlugin::ProcessInIcon()");

    TPtr8 restoreIconPtr = iRestoreIconDesc->Des();
    HBufC8* tempDesc = HBufC8::NewL(iIconFileSizeLeft);
    CleanupStack::PushL(tempDesc);
    TPtr8 tempPtr = tempDesc->Des();

    // Icon cannot be read fully
    if (iIconFileSizeLeft > aBufferSpaceLeft)
    {
        aStream.ReadL(tempPtr, aBufferSpaceLeft);
        restoreIconPtr.Append(tempPtr);
        iIconFileSizeLeft -= aBufferSpaceLeft;
        aBufferSpaceLeft = 0;
        iRestoreState = EInIcon;
    }
    // Icon can be read fully
    else
    {
        aStream.ReadL(tempPtr, iIconFileSizeLeft);
        restoreIconPtr.Append(tempPtr);
        aBufferSpaceLeft -= iIconFileSizeLeft;
        iRestoreState = EInSize;

        TRAPD(err, iAppArcUtil->RegisterAppL(restoreIconPtr, iDrive));

        if (err)
        {
            ELOG(EBackup, "Registerin application to AppArc failed.");
        }
    }

    CleanupStack::PopAndDestroy(tempDesc);
}

void CMidp2BackupPlugin::ProcessInSizeL(RDesReadStream& aStream, TInt& aBufferSpaceLeft)
{
    LOG(EBackup, EInfo, "CMidp2BackupPlugin::ProcessInSize()");

    if (aBufferSpaceLeft >= sizeof(TInt32))
    {
        if (iSizeBuffer.Size() != 0)
        {
            // Read leftover part of size
            TInt readData = sizeof(TInt32) - iSizeBuffer.Size();
            aStream.ReadL(iSizeBuffer, readData);
            RMemReadStream sizeStream(iSizeBuffer.Ptr(), 4);
            CleanupClosePushL(sizeStream);
            iIconFileSizeLeft = sizeStream.ReadInt32L();
            CleanupStack::PopAndDestroy(&sizeStream);
            aBufferSpaceLeft -= sizeof(readData);
        }
        else
        {
            // Read size in full
            iIconFileSizeLeft = aStream.ReadInt32L();
            aBufferSpaceLeft -= sizeof(iIconFileSizeLeft);
        }

        delete iRestoreIconDesc;
        iRestoreIconDesc = NULL;
        iRestoreIconDesc = HBufC8::NewL(iIconFileSizeLeft);
        iRestoreState = EInIcon;
    }
    else
    {
        // Read first byte(s) of size
        aStream.ReadL(iSizeBuffer, aBufferSpaceLeft);
        aBufferSpaceLeft = 0;
    }
}

void CMidp2BackupPlugin::ResetIconArray()
{
    LOG(EBackup, EInfo, "CMidp2BackupPlugin::ResetIconArray()");

    // Remove all entries
    while (iIconUidArray.Count())
    {
        iIconUidArray.Remove(0);
    }
}
