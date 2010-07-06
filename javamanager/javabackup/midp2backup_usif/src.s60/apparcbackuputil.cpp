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
* Description:  Implementation of CAppArcBackupUtil
*
*/


#include "apparcbackuputil.h"
#include "javaapparcutil.h"

#include "javasymbianoslayer.h"
#include "javauid.h"
#include "logger.h"
#include "javacommonutils.h"
#include "s60commonutils.h"

#include <apgcli.h>
#include <wchar.h>
#include <s32mem.h>
#include <sysutil.h>
#include <f32file.h>

#include <memory>
#include <string>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <apgicnfl.h>
#else
#include <apgicnflpartner.h>
#endif


using namespace std;
using namespace java::storage;
using namespace java::backup;

const TUid  KMidletApplicationTypeUid = { 0x10210E26 };
_LIT(KDefaultFolder, "");

// ======== MEMBER FUNCTIONS ========

CAppArcBackupUtil::CAppArcBackupUtil()
{
    LOG(EBackup, EInfo, "CAppArcBackupUtil constructor");
}

void CAppArcBackupUtil::ConstructL(RFs& aFs)
{
    LOG(EBackup, EInfo, "CAppArcBackupUtil::ConstructL");

    User::LeaveIfError(iApparcServer.Connect());

    iNumberOfIcons = 0;
    iFs = aFs;
}


CAppArcBackupUtil* CAppArcBackupUtil::NewL(RFs& aFs)
{
    LOG(EBackup, EInfo, "CAppArcBackupUtil::NewL");

    CAppArcBackupUtil* self = new(ELeave) CAppArcBackupUtil();
    CleanupStack::PushL(self);
    self->ConstructL(aFs);
    CleanupStack::Pop();

    return self;
}


CAppArcBackupUtil::~CAppArcBackupUtil()
{
    LOG(EBackup, EInfo, "CAppArcBackupUtil destructor");

    iApparcServer.Close();
}


void CAppArcBackupUtil::RegisterAppL(TPtr8& aRestoreIconPtr, TDriveNumber& aDrive)
{
    LOG(EBackup, EInfo, "CAppArcBackupUtil::RegisterApp");

    RDesReadStream stream(aRestoreIconPtr);
    CleanupClosePushL(stream);

    // Read Uid
    int midletUid = stream.ReadUint32L();

    if (!CheckIfAlreadyRegisteredL(midletUid))
    {
        // Read filename
        TInt32 fileNameSize = stream.ReadInt32L();
        HBufC* fileName = HBufC::NewL(fileNameSize);
        CleanupStack::PushL(fileName);
        TPtr16 fileNamePtr = fileName->Des();
        stream.ReadL(fileNamePtr, fileNameSize / 2);   // half of size

        // Read number of icons
        TInt numberOfIcons = stream.ReadInt32L();
//        numberOfIcons = 1;
        //LOG1(EBackup, EInfo, "Number of Icons = %d", numberOfIcons);

        TInt groupNameSize = stream.ReadInt32L();
        TBuf<KApaMaxAppGroupName> groupName;

        if (groupNameSize != 0)
        {
            stream.ReadL(groupName, groupNameSize / 2);
        }

        // check disk space
        TInt index = sizeof(TUint32)    // midletUid
                     + sizeof(TInt32)                // size of fileNameSize
                     + fileNameSize                  // size of filename
                     + sizeof(TInt32)                // size of numberOfIcons
                     + sizeof(TInt32)                // group name size
                     + groupNameSize;                // groupname

        if (SysUtil::DiskSpaceBelowCriticalLevelL(&iFs, aRestoreIconPtr.Size() - index, aDrive))
        {
            ELOG(EBackup, "Disk space below critical level. Leaving with KErrDiskFull.");
            User::Leave(KErrDiskFull);
        }

        // Make directory if necessary
        iFs.CreatePrivatePath(aDrive);

        // Make full path of temporary icon file
        TPath tempPath;
        TPath tempPrivatePath;
        iFs.PrivatePath(tempPrivatePath);

        TDriveUnit* driveUnit = new(ELeave) TDriveUnit(aDrive);
        CleanupStack::PushL(driveUnit);

        tempPath.Append(driveUnit->Name().Left(1));
        tempPath.Append(KDriveDelimiter);
        tempPath.Append(tempPrivatePath);
        tempPath.Append(fileNamePtr);

        // Set private session path to point same drive where restoring
        iFs.SetSessionToPrivate(aDrive);


        RFile iconFile;
        User::LeaveIfError(iconFile.Create(iFs, fileNamePtr, EFileWrite));

        CleanupStack::PopAndDestroy(fileName);
        CleanupClosePushL(iconFile);

        iconFile.Write(aRestoreIconPtr.Mid(index));

        // Register application
        LOG1(EBackup,EInfo,"Registering app %d", midletUid);
        TUid uid = TUid::Uid(midletUid);
        TBuf<128>   appName;
        CJavaApparcUtil::AppName(uid, appName);

        CApaRegistrationResourceFileWriter* writer =
            CApaRegistrationResourceFileWriter::NewL(uid,
                    appName,
                    TApaAppCapability::ENonNative);
        CleanupStack::PushL(writer);

        const TInt  KOpaqueDataLength = 4;
        TBuf8<KOpaqueDataLength>    opaqueData;
        RDesWriteStream             writeStream(opaqueData);
        CleanupClosePushL(writeStream);

        LOG(EBackup, EInfo, "Getting midlet entry");

        JavaStorageApplicationEntry_t appEntries = StorageEntryL(uid);

        if (appEntries.size() == 0)
        {
            ELOG(EBackup, "No midlet entry found in Java Storage; Mismatch between AppArc and Storage data. Abort backup");
            User::Leave(KErrNotFound);
        }

        JavaStorageEntry attribute;
        attribute.setEntry(NAME, L"");

        // Get Name attribute from read attributes.
        JavaStorageApplicationEntry_t::const_iterator findIterator = appEntries.find(attribute);
        wstring midName;

        if (findIterator != appEntries.end())
        {
            midName = (*findIterator).entryValue();

        }
        else
        {
            User::Leave(KErrBadHandle);
        }

        appEntries.clear();

        HBufC* tempstr = java::util::S60CommonUtils::wstringToDes(midName.c_str());
        TPtrC midletName = tempstr->Des();

        if (groupNameSize != 0)
        {
            writer->SetGroupNameL(groupName);
        }
        else
        {
            writer->SetGroupNameL(KDefaultFolder);
        }

        writeStream.WriteUint32L(midletUid);
        writeStream.CommitL();

        LOG(EBackup, EInfo, "Setting opaque data");
        writer->SetOpaqueDataL(opaqueData);

        CApaLocalisableResourceFileWriter*  lWriter =
            CApaLocalisableResourceFileWriter::NewL(KNullDesC,
                                                    midletName,
                                                    numberOfIcons,
                                                    KNullDesC);
        CleanupStack::PushL(lWriter);

        // Prepare AppArc for deregistration
        LOG(EBackup, EInfo, "PrepareNonNativeApplicationsUpdatesL");
        iApparcServer.PrepareNonNativeApplicationsUpdatesL();

        // Register application
        LOG(EBackup, EInfo, "RegisterNonNativeApplicationL");
        iApparcServer.RegisterNonNativeApplicationL(KMidletApplicationTypeUid,
                aDrive,
                *writer,
                lWriter,
                &iconFile);

        // Commit registration changes
        LOG(EBackup, EInfo, "CommitNonNativeApplicationsUpdatesL");
        iApparcServer.CommitNonNativeApplicationsUpdatesL();

        delete tempstr;
        CleanupStack::PopAndDestroy(lWriter);
        CleanupStack::PopAndDestroy(&writeStream);
        CleanupStack::PopAndDestroy(writer);
        iconFile.Close();
        CleanupStack::PopAndDestroy(&iconFile);

        TInt err = iFs.Delete(tempPath);
        LOG1(EBackup, EInfo, "iFs.Delete err = %d", err);
        CleanupStack::PopAndDestroy(driveUnit);
    }

    CleanupStack::PopAndDestroy(&stream);
}


void CAppArcBackupUtil::DeregisterAppsL(TDriveNumber aDrive)
{
    LOG1(EBackup, EInfo, "CAppArcBackupUtil::DeregisterApps, drive: %d", aDrive);

    // Initialise the process of getting apps
    iApparcServer.GetFilteredApps(TApaAppCapability::ENonNative,
                                  TApaAppCapability::ENonNative);

    // Getting the apps one-by-one
    TApaAppInfo* info = new(ELeave) TApaAppInfo();
    CleanupStack::PushL(info);

    TUid typeUid;

    // Prepare AppArc for deregistration
    LOG(EBackup, EInfo, "PrepareNonNativeApplicationsUpdatesL");
    iApparcServer.PrepareNonNativeApplicationsUpdatesL();

    while (!iApparcServer.GetNextApp((*info)))
    {
        LOG1(EBackup, EInfo, "Getting Next App from AppArc, UID = %d", info->iUid);

        // Getting the type of the application
        iApparcServer.GetAppType(typeUid, info->iUid);

        // If it's a MIDlet, check if it has an entry in Java Registry
        TInt drive = 0;
        GetDriveOfApp(info->iFullName, drive);

        if (typeUid == KMidletApplicationTypeUid && drive == aDrive)
        {
            LOG1(EBackup, EInfo, "Checking registry, midlet id = %d", info->iUid);
            wstring PackageID;
            // If there's no entry on the drive, deregister it from AppArc
            JavaStorageApplicationEntry_t appEntries = StorageEntryL(info->iUid);

            if (appEntries.size() != 0)
            {
                JavaStorageEntry attribute;
                attribute.setEntry(PACKAGE_ID, L"");

                JavaStorageApplicationEntry_t::const_iterator findIterator = appEntries.find(attribute);

                if (findIterator != appEntries.end())
                {
                    PackageID = (*findIterator).entryValue();
                }
                else
                {
                    User::Leave(KErrBadHandle);
                }

                if (GetDrivefromStorageL(PackageID) != aDrive)
                {
                    iApparcServer.DeregisterNonNativeApplicationL(info->iUid);
                    LOG1(EBackup, EInfo, "Deregistering midlet id = %d", info->iUid);
                }
            }
            else
            {
                LOG(EBackup, EInfo, "Midlet entry not found in Java Storage; Deregister the Application");
                iApparcServer.DeregisterNonNativeApplicationL(info->iUid);
                LOG1(EBackup, EInfo, "Deregistering midlet id = %d", info->iUid);
            }
            appEntries.clear();
        }
    }

    // Commit registration changes
    LOG(EBackup, EInfo, "CommitNonNativeApplicationsUpdatesL");
    iApparcServer.CommitNonNativeApplicationsUpdatesL();
    CleanupStack::PopAndDestroy(info);
}


void CAppArcBackupUtil::GetMidletsFromAppArcL(RArray<TUid>& aUidArray, TDriveNumber aDrive)
{
    LOG(EBackup, EInfo, "CAppArcBackupUtil::GetMidletsFromAppArc");
    CleanupClosePushL(aUidArray);

    // Initialise the process of getting apps
    iApparcServer.GetFilteredApps(TApaAppCapability::ENonNative,
                                  TApaAppCapability::ENonNative);

    // Getting the apps one-by-one
    TApaAppInfo* info = new(ELeave) TApaAppInfo();
    CleanupStack::PushL(info);

    TUid typeUid;
    LOG1(EBackup, EInfo, "Drive Number aDrive = %d", aDrive);

    while (!iApparcServer.GetNextApp((*info)))
    {
        // Getting the type of the application
        iApparcServer.GetAppType(typeUid, info->iUid);

        // If it's a MIDlet, check if it's registered to this drive
        if (typeUid == KMidletApplicationTypeUid)
        {
            JavaStorageApplicationEntry_t appEntries = StorageEntryL(info->iUid);

            if (appEntries.size() != 0)
            {
                JavaStorageEntry attribute;
                attribute.setEntry(PACKAGE_ID, L"");

                JavaStorageApplicationEntry_t::const_iterator findIterator = appEntries.find(attribute);
                wstring PackageID;

                if (findIterator != appEntries.end())
                {
                    PackageID = (*findIterator).entryValue();

                }
                else
                {
                    User::Leave(KErrBadHandle);
                }
                appEntries.clear();

                if (GetDrivefromStorageL(PackageID) == aDrive)
                {
                    aUidArray.AppendL(info->iUid);
                    iNumberOfIcons++;
                }
            }
            else
            {
                ELOG(EBackup, "No entries from registry; midlet does not exist");
            }
        }
    }
    CleanupStack::PopAndDestroy(info);
    CleanupStack::Pop();

}


void CAppArcBackupUtil::GetIconFilename(TUid aUid, HBufC*& aFullFileName)
{
    LOG(EBackup, EInfo, "CAppArcBackupUtil::GetIconFilename");

    iApparcServer.GetAppIcon(aUid, aFullFileName);
}

TInt CAppArcBackupUtil::NumberOfIcons()
{
    LOG(EBackup, EInfo, "CAppArcBackupUtil::NumberOfIcons");

    return iNumberOfIcons;
}

void CAppArcBackupUtil::NumberOfOwnDefinedIcons(TUid aUid, TInt& aIconCount)
{
    LOG(EBackup, EInfo, "CAppArcBackupUtil::NumberOfOwnDefinedIcons");

    iApparcServer.NumberOfOwnDefinedIcons(aUid, aIconCount);
}

TBool CAppArcBackupUtil::CheckIfAlreadyRegisteredL(TInt& aUid)
{
    LOG(EBackup, EInfo, "CAppArcBackupUtil::CheckIfAlreadyRegisteredL");

    TApaAppInfo* info = new(ELeave) TApaAppInfo();

    TInt err = iApparcServer.GetAppInfo(*info, TUid::Uid(aUid));
    delete info;
    if (err == KErrNone)
    {
        return ETrue;
    }
    else
    {
        return EFalse;
    }


}

void CAppArcBackupUtil::GetDriveOfApp(TFileName aPath, TInt& aDrive)
{
    LOG(EBackup, EInfo, "CAppArcBackupUtil::DriveOfApp");

    iFs.CharToDrive(aPath[0], aDrive);
}


TApaAppGroupName CAppArcBackupUtil::GetMidletGroupName(TUid aUid)
{
    TApaAppCapabilityBuf cap;

    TInt err = iApparcServer.GetAppCapability(cap, aUid);
    return cap().iGroupName;
}


JavaStorageApplicationEntry_t CAppArcBackupUtil::StorageEntryL(const TUid& aMidletUID)
{
    auto_ptr<JavaStorage> js(JavaStorage::createInstance());

    try
    {
        js->open(JAVA_DATABASE_NAME);
        LOG(EBackup, EInfo, "Java database opened");
    }
    catch (JavaStorageException jse)
    {
        ELOG(EBackup, "Opening database connection failed");
        User::Leave(KErrNotFound);
    }

    JavaStorageApplicationEntry_t entries;

    java::util::Uid appUid;
    TUidToUid(aMidletUID, appUid);

    try
    {
        js->read(APPLICATION_TABLE, appUid, entries);
    }
    catch (JavaStorageException jse)
    {
        ELOG(EBackup, "Read in Java Storage failed");
        js->close();
        User::Leave(KErrGeneral);
    }
    js->close();

    return entries;
}

TDriveNumber CAppArcBackupUtil::GetDrivefromStorageL(wstring& aPackageID)
{
    wchar_t Drive;
    auto_ptr<JavaStorage> js(JavaStorage::createInstance());

    try
    {
        js->open(JAVA_DATABASE_NAME);
    }
    catch (JavaStorageException jse)
    {
        ELOG(EBackup, "Opening database connection failed");
        User::Leave(KErrNotFound);
    }

    JavaStorageApplicationEntry_t entries;
    java::util::Uid PackageID(aPackageID);

    try
    {
        js->read(APPLICATION_PACKAGE_TABLE, PackageID, entries);
    }
    catch (JavaStorageException jse)
    {
        ELOG(EBackup, "Read in Java Storage failed");
        js->close();
        User::Leave(KErrGeneral);
    }

    js->close();

    JavaStorageEntry attribute;
    attribute.setEntry(ROOT_PATH, L"");

    JavaStorageApplicationEntry_t::const_iterator findIterator = entries.find(attribute);
    wstring RootPath;

    if (findIterator != entries.end())
    {
        RootPath = (*findIterator).entryValue();

    }
    else
    {
        User::Leave(KErrBadHandle);
    }

    Drive = (wchar_t)RootPath[0];
    LOG1(EBackup, EInfo, "Drive = %c", Drive);
    TInt i = Drive - (wchar_t)'a';
    LOG1(EBackup, EInfo, "Drive = %d", (TDriveNumber)i);
    return (TDriveNumber)i;
}
