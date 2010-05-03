/*
* Copyright (c) 2008 - 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Java platform 2.0 javaapppreconverter process.
*              This process needs AllFiles capability to be able to read
*              the icon files of all midlets and to store midlet data
*              to the data cage of the javaappbackconverter process.
*
*/


#include <e32base.h>
#include <pathinfo.h>
#include <driveinfo.h>
#include <apgcli.h>
#include <d32dbms.h>

#include <mdatabasev2.h>
#include <mmidlettablev2.h>
#include <mmidlettableiterator.h>

#include "javacommonutils.h"
#include "javauids.h"
#include "preconverter.h"
#include "noarmlogs.h"

const TInt    KDelayWhenWaitingAppArc = 500000;
const TUint32 KDBVersion = 0x0200;

// Name for the system AMS DB policy
_LIT(KUidSSystemAMSDbPolicy, "secure[102045FE]");

// Constant needed to access SystemAMS static database
_LIT(KStaticDbFileName, "C:\\private\\100012A5\\DBS_102045FE_MIDP2SystemAMSStaticV2");

// This is in the private data cage of javaappbackconverter.
// javaappbackconverter will destroy the files created to this directory
// after it has done the back conversion.
_LIT(KMidletExportDirectory, "C:\\private\\20022D90\\data\\");

// This is in the private data cage of usersettingsconfigurator.
_LIT(KMidletExportDirectoryForUserSettings, "C:\\private\\20022E7A\\data\\");

// This is in the private data cage of javappconverter
// javaappconverter will destroy the 'uids' file created to this directory
// after it has made the conversion.
_LIT(KUidsExportDirectory, "C:\\private\\2002121C\\data\\");
_LIT(KUidsExportDataFileName, "uids");

// These literals are used for logging midlet information found from db.
_LIT8(KMLName, "\nname: ");
_LIT8(KMLClassName, "\nclassName: ");
_LIT8(KMLIdString, "\nidString: ");


/**
 * To create new instance of this class.
 *
 * @param aFs - A reference to the file server.
 * @return Reference to the object of this class.
 * @exception If construction fails.
 */
CPreConverter* CPreConverter::NewLC(RFs& aFs)
{
    CPreConverter* self = new(ELeave) CPreConverter(aFs);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

/**
 * To do 1st phase construction for this object.
 *
 * Adds this active object to the scheduler.
 */
CPreConverter::CPreConverter(RFs& aFs) :
        CActive(EPriorityStandard), iFs(aFs)
{
    CActiveScheduler::Add(this);
}

/**
 * To do 2nd phase construction for this object.
 *
 * @exception If the method is not able to allocate necessary buffers.
 */
void CPreConverter::ConstructL()
{
    JELOG2(EJavaConverters);

    iMidlets = new(ELeave) CMidletList();
    iState = EConversionDataAlreadyGathered;
}

/**
 * Deletes this object.
 * All allocated resources are released.
 */
CPreConverter::~CPreConverter()
{
    JELOG2(EJavaConverters);
    Cancel();
    if (iMidlets)
    {
        iMidlets->ResetAndDestroy();
        delete iMidlets;
        iMidlets = NULL;
    }
}

/**
 * To start preconversion
 */
void CPreConverter::Start()
{
    JELOG2(EJavaConverters);
    iState = EConversionDataAlreadyGathered;
    CompleteRequest();
}

/**
 * To stop whole preconversion.
 * Stops the active scheduler.
 */
void CPreConverter::Exit()
{
    Deque();
    CActiveScheduler::Stop();
}

/**
 * To complete the request for this object.
 *
 * @Postconditions The following conditions are true immediately after
 * returning from this method.
 * - iStatus == KErrNone
 * - IsActive() == ETrue
 */
void CPreConverter::CompleteRequest()
{
    JELOG2(EJavaConverters);

    TRequestStatus *status = &iStatus;
    User::RequestComplete(status, KErrNone);
    if (!IsActive())
    {
        SetActive();
    }
}

/**
 * To run this active object.
 *
 * The state logic is:
 *
 * EConversionDataAlreadyGathered:
 *    if midlet export data file already exists,
 *       active object exits from this state
 *
 * EFindOutInstalledMidlets:
 *    iterate through all apps in AppArc,
 *      if java app,
 *        store all info available from AppArc to iMidlets
 *
 * EFillDataFromSystemAmsDb:
 *    add info from SystemAMS DB to iMidlets
 *
 * EStoreData:
 *    store data in iMidlets to a file
 *
 * EStopMidlets:
 *    stop all running midlets
 *
 * EUnregisterMidlets
 *    unregister all midlets in iMidlets from AppArc
 *
 * EExit:
 *    free resources and exit
 *
 */
void CPreConverter::RunL()
{
    JELOG2(EJavaConverters);

    switch (iState)
    {

    case EConversionDataAlreadyGathered:
    {
        LOG(EJavaConverters, EInfo,
            "CPreConverter::RunL EConversionDataAlreadyGathered");

        TFileName exportDataFile(KMidletExportDirectory);
        exportDataFile.Append(KMidletExportDataFileName);
        TUint attributes;
        TInt  err = iFs.Att(exportDataFile, attributes);
        if (KErrNone == err)
        {
            LOG(EJavaConverters, EInfo,
                "CPreConverter::RunL EConversionDataAlreadyGathered "
                "data file already exists. Can exit.");
            iState = EExit;
        }
        else
        {
            if ((KErrNotFound == err) || (KErrPathNotFound == err))
            {
                // We come here only when OMJ is not currently installed
                // to the device. However, the device may still have
                // old OMJ database files. Clean them now to make sure
                // that e.g. old db schemas do not cause problems.
                LOG(EJavaConverters, EInfo,
                    "CPreConverter::RunL EConversionDataAlreadyGathered "
                    "Trying to destroy OMJ database files");

                _LIT(KJavaOtaStorageDbPath, "C:\\private\\10281E17\\[200211dc]JavaOtaStorage.db");
                err = iFs.Delete(KJavaOtaStorageDbPath);
                LOG1(EJavaConverters, EInfo,
                     "CPreConverter::RunL EConversionDataAlreadyGathered "
                     "Destroy C:\\private\\10281E17\\[200211dc]JavaOtaStorage.db, err %d", err);

                _LIT(KJavaOtaStorageJournalPath, "C:\\private\\10281E17\\[200211dc]JavaOtaStorage.db-journal");
                err = iFs.Delete(KJavaOtaStorageJournalPath);
                LOG1(EJavaConverters, EInfo,
                     "CPreConverter::RunL EConversionDataAlreadyGathered "
                     "Destroy C:\\private\\10281E17\\[200211dc]JavaOtaStorage.db-journal, err %d", err);

                _LIT(KJavaStorageDbPath, "C:\\private\\10281E17\\[200211dc]JavaStorage.db");
                err = iFs.Delete(KJavaStorageDbPath);
                LOG1(EJavaConverters, EInfo,
                     "CPreConverter::RunL EConversionDataAlreadyGathered "
                     "Destroy C:\\private\\10281E17\\[200211dc]JavaStorage.db, err %d", err);

                _LIT(KJavaStorageJournalPath, "C:\\private\\10281E17\\[200211dc]JavaStorage.db-journal");
                err = iFs.Delete(KJavaStorageJournalPath);
                LOG1(EJavaConverters, EInfo,
                     "CPreConverter::RunL EConversionDataAlreadyGathered "
                     "Destroy C:\\private\\10281E17\\[200211dc]JavaStorage.db-journal, err %d", err);

                LOG(EJavaConverters, EInfo,
                    "CPreConverter::RunL EConversionDataAlreadyGathered "
                    "Data file does not exist yet. OK");
            }
            else
            {
                LOG1(EJavaConverters, EInfo,
                     "CPreConverter::RunL EConversionDataAlreadyGathered "
                     "Checking data file attributes caused error %d", err);
            }
            iState = EFindOutInstalledMidlets;
        }
        CompleteRequest();
    }
    break;

    case EFindOutInstalledMidlets:
    {
        LOG(EJavaConverters, EInfo,
            "CPreConverter::RunL EFindOutInstalledMidlets");

        // Read all installed midlets from AppArc,
        // store info to iMidlets
        ReadMidletsFromAppArcL();

        iState = EFillDataFromSystemAmsDb;
        CompleteRequest();
    }
    break;

    case EFillDataFromSystemAmsDb:
    {
        LOG(EJavaConverters, EInfo,
            "CPreConverter::RunL EFillDataFromSystemAmsDb");

        // Some of the necessary information is in SystemAMS DB,
        // read it now for each midlet in iMidlets
        AddDataFromSystemAmsDbL();

        // Especially in R&D environment it is possible to that
        // AppArc contains some midlets that have not really
        // been installed to old S60 java environment.
        // SystemAMS DB does not contain any information of
        // these midlets. They must be removed now.
        RemoveInvalidMidlets();

        iState = EStoreData;
        CompleteRequest();
    }
    break;

    case EStoreData:
    {
        LOG(EJavaConverters, EInfo,
            "CPreConverter::RunL EStoreData");

        if (iMidlets->Count() == 0)
        {
            // No midlets to convert
            WLOG(EJavaConverters, "CPreConverter::RunL EStoreData No midlets to convert.");
        }
        else
        {
            // Store the data directly to the data cage of
            // javaappbackconverter.exe
            TFileName exportDirectory(KMidletExportDirectory);
            iMidlets->ExportListL(iFs, exportDirectory);

            // Store the data also to the data cage of
            // usersettingsconfigurator.exe
            TFileName exportDirectoryForUserSettings(KMidletExportDirectoryForUserSettings);
            iMidlets->ExportListL(iFs, exportDirectoryForUserSettings);

            // Store the uids of the midlets to be converted
            // to the data cage of javaappconverter.exe
            StoreUidsL();
        }

        iState = EStopMidlets;
        CompleteRequest();
    }
    break;

    case EStopMidlets:
    {
        LOG(EJavaConverters, EInfo,
            "CPreConverter::RunL EStopMidlets");

        CMidletInfo* pMidlet = iMidlets->GetFirst();

        TFullName foundName;
        TFullName searchName;

        while (pMidlet != NULL)
        {
            searchName = pMidlet->GetMidletName();
            _LIT(KMidletProcessIdString, "[102033e6]*");
            searchName.Append(KMidletProcessIdString);

            LOG1WSTR(EJavaConverters, EInfo, "Searching midlet process named %s",
                     (wchar_t *)(searchName.PtrZ()));

            TFindProcess find(searchName);
            TInt err = find.Next(foundName);
            if (KErrNone == err)
            {
                LOG1WSTR(EJavaConverters, EInfo, "The midlet process %s was found",
                         (wchar_t *)(foundName.PtrZ()));
                RProcess process;
                err = process.Open(find);
                if (KErrNone == err)
                {
                    LOG(EJavaConverters, EInfo, "Killing the midlet process");
                    process.Kill(KErrNone);
                    process.Close();
                }
            }

            pMidlet = iMidlets->GetNext();
        }

        iState = EUnregisterMidlets;
        CompleteRequest();
    }
    break;

    case EUnregisterMidlets:
    {
        LOG(EJavaConverters, EInfo, "CPreConverter::RunL EUnregisterMidlets");

        // Unregister all midlets in iMidlets from AppArc
        UnregisterOldJavaAppsL();

        iState = EExit;
        CompleteRequest();
    }
    break;

    case EExit:
    {
        LOG(EJavaConverters, EInfo, "CPreConverter::RunL EExit");

        FullCleanup();

        // The whole javaapppreconverter process is stopped.
        Exit();
    }
    break;

    }
}

/**
 * To handle leave from RunL.
 * This method exits this active object using normal state machine
 * After calling this method this active object will exit.
 *
 * @param aError - A reason of error.
 * @return KErrNone.
 */
TInt CPreConverter::RunError(TInt aError)
{
    ELOG2(EJavaConverters,
          "CPreConverter::RunError(%d) from state %d", aError, iState);

    Cancel();

    iState = EExit;
    CompleteRequest();

    return KErrNone;
}

/**
 * To do cancelling for this object.
 *
 */
void CPreConverter::DoCancel()
{
    ELOG1(EJavaConverters,
          "CPreConverter::DoCancel from state %d", iState);

}

/**
 * To cleanup member variables.
 *
 */
void CPreConverter::FullCleanup()
{
    JELOG2(EJavaPreinstaller);

    if (iMidlets)
    {
        iMidlets->ResetAndDestroy();
        delete iMidlets;
        iMidlets = NULL;
    }
}

/**
 * Read all midlets from AppArc.
 * Store midlet Uid, Midlet name, installation drive,
 * group name and icon file path name to iMidlets.
 */
void CPreConverter::ReadMidletsFromAppArcL()
{
    JELOG2(EJavaConverters);

    TInt   retryCounter = 10;
    TUid   appTypeUid;
    HBufC *pIconFilePathName = NULL;
    TApaAppInfo   info;
    RApaLsSession apaSession;
    TApaAppCapabilityBuf cbuf;
    TApaAppCapability    capability;

    TBuf8<512> midletDesc;


    TInt err = apaSession.Connect();
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "CPreConverter::ReadMidletsFromAppArcL RApaLsSession Connect error %d", err);
        User::Leave(err);
    }
    CleanupClosePushL(apaSession);

    // Get the process of getting the list of all non native applications
    // (mostly java applications)
    err =
        apaSession.GetFilteredApps(
            TApaAppCapability::ENonNative, TApaAppCapability::ENonNative);

    do
    {
        err = apaSession.GetNextApp(info);
        if (RApaLsSession::EAppListInvalid == err)
        {
            // Application list has not yet been populated,
            // try again after a short delay
            retryCounter--;
            if (retryCounter > 0)
            {
                User::After(KDelayWhenWaitingAppArc);
                continue;
            }
            else
            {
                ELOG(EJavaConverters,
                     "CPreConverter::ReadMidletsFromAppArcL RApaLsSession "
                     "GetNext returned EAppListInvalid for 10 times, error");
                User::Leave(err);
            }
        }
        else if (KErrNone == err)
        {
            // Info contains valid app info. Now check whether it is java app
            err = apaSession.GetAppType(appTypeUid, info.iUid);
            if (KErrNone != err)
            {
                ELOG1(EJavaConverters,
                      "CPreConverter::ReadMidletsFromAppArcL RApaLsSession "
                      "GetAppType returned error %d", err);
                User::Leave(err);
            }
            if (appTypeUid.iUid == KMidletApplicationTypeUid)
            {
                // This is java application. Store info to list.
                CMidletInfo *midletInfo = new(ELeave) CMidletInfo();
                midletInfo->SetMidletUid(info.iUid);
                midletInfo->SetMidletName(info.iCaption);
                midletInfo->SetDrive(ExtractDriveNumberFromPathL(info.iFullName));

                // Get group name
                err = apaSession.GetAppCapability(cbuf, info.iUid);
                if (KErrNone != err)
                {
                    ELOG1(EJavaConverters,
                          "CPreConverter::ReadMidletsFromAppArcL RApaLsSession "
                          "GetAppCapability returned error %d", err);
                    User::Leave(err);
                }
                capability = cbuf();
                midletInfo->SetGroupName(capability.iGroupName);

                // Get icon file path name
                err = apaSession.GetAppIcon(info.iUid, pIconFilePathName);
                if ((KErrNone != err) || (NULL == pIconFilePathName))
                {
                    ELOG1(EJavaConverters,
                          "CPreConverter::ReadMidletsFromAppArcL RApaLsSession "
                          "GetAppIcon returned error %d", err);
                    User::Leave(err);
                }
                midletInfo->SetIconFileName(pIconFilePathName);
                delete pIconFilePathName;
                pIconFilePathName = NULL;

                err = iMidlets->Append(midletInfo);
                if (KErrNone != err)
                {
                    ELOG1(EJavaConverters,
                          "CPreConverter::ReadMidletsFromAppArcL CMidletList "
                          "Append returned error %d", err);
                    User::Leave(err);
                }
                else
                {
                    midletInfo->ToString8(midletDesc);
                    LOG(EJavaConverters, EInfo,
                        "CPreConverter::ReadMidletsFromAppArcL Added this midlet:");
                    midletDesc.ZeroTerminate();
                    LOG(EJavaPreinstaller, EInfo, (const char *)(midletDesc.Ptr()));
                    midletDesc.Zero();
                }
            }
        }

    }
    while (KErrNone == err);

    if (RApaLsSession::ENoMoreAppsInList != err)
    {
        ELOG1(EJavaConverters,
              "CPreConverter::ReadMidletsFromAppArcL RApaLsSession GetNext returned error %d", err);
        User::Leave(err);
    }

    CleanupStack::PopAndDestroy(); // apaSession
}

/**
 * Parse the drive letter from path and return it as TDriveNumber.
 *
 * @param aPathName full path name. Contains always the drive.
 * @return the drive number
 * @exception if the drive cannot parsed from path
 */
TDriveNumber CPreConverter::ExtractDriveNumberFromPathL(TFileName &aPathName)
{
    TDriveNumber drive = EDriveE;
    TUint driveLetter = aPathName[0];

    if ((driveLetter >= 'a') && (driveLetter <= 'z'))
    {
        drive = (TDriveNumber)(driveLetter - 'a');
    }
    else if ((driveLetter >= 'A') && (driveLetter <= 'Z'))
    {
        drive = (TDriveNumber)(driveLetter - 'A');
    }
    else
    {
        ELOG1(EJavaConverters,
              "CPreConverter::ExtractDriveNumberFromPathL path starts "
              "with illegal char, value %d", drive);
        User::Leave(KErrArgument);
    }

    return drive;
}

/**
 * Read the midlet id and suite id for each midlet in iMidlets from
 * SystemAMS DB and store them to iMidlets
 */
void CPreConverter::AddDataFromSystemAmsDbL()
{
    MIDP::DBv2::MDatabase* database = NULL;
    MIDP::DBv2::MMIDletTable* midletTable = NULL;
    RDbNamedDatabase dbNamed;
    RDbs dbs;

    // Connect to db
    TInt err = dbs.Connect();
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "CPreConverter::AddDataFromSystemAmsDbL Cannot connect to RDbs, "
              "error %d", err);
        User::Leave(err);
    }

    // Open db
    err = dbs.ShareAuto();
    if (KErrNone != err)
    {
        dbs.Close();
        ELOG1(EJavaConverters,
              "CPreConverter::AddDataFromSystemAmsDbL Cannot share RDbs session, "
              "error %d", err);
        User::Leave(err);
    }
    err = dbNamed.Open(
              iFs, KStaticDbFileName(), KUidSSystemAMSDbPolicy, RDbNamedDatabase::EReadOnly);
    if (KErrNone != err)
    {
        dbs.Close();
        ELOG1(EJavaConverters,
              "CPreConverter::AddDataFromSystemAmsDbL Cannot open named db, "
              "error %d", err);
        User::Leave(err);
    }

    // Get db
    TRAP(err, database = MIDP::DBv2::GetDatabaseL(KDBVersion));
    if (KErrNone != err)
    {
        dbNamed.Close();
        dbs.Close();
        ELOG1(EJavaConverters,
              "CPreConverter::AddDataFromSystemAmsDbL Cannot get database, "
              "error %d", err);
        User::Leave(err);
    }

    // Get midlet table
    TRAP(err, midletTable = database->MIDletTableL());
    if (KErrNone != err)
    {
        delete database;
        dbNamed.Close();
        dbs.Close();
        ELOG1(EJavaConverters,
              "CPreConverter::AddDataFromSystemAmsDbL Cannot get midlet table from db, "
              "error %d", err);
        User::Leave(err);
    }

    // Open midlet table
    TRAP(err, midletTable->OpenL(dbNamed));
    if (KErrNone != err)
    {
        delete database;
        dbNamed.Close();
        dbs.Close();
        ELOG1(EJavaConverters,
              "CPreConverter::AddDataFromSystemAmsDbL Cannot open midlet table in db, "
              "error %d", err);
        User::Leave(err);
    }

    // get iterator to access the data in midlet table
    MIDP::DBv2::MMIDletTableIterator* tableIterator = NULL;
    // No items must be left in cleanup stack when exiting TRAP macro
    TRAP(err,
         tableIterator = midletTable->IteratorLC();
         if (KErrNone == err) CleanupStack::Pop();
        );
    if (KErrNone != err)
    {
        midletTable->Close();
        delete database;
        dbNamed.Close();
        dbs.Close();
        ELOG1(EJavaConverters,
              "CPreConverter::AddDataFromSystemAmsDbL Cannot get iterator for midlet table in db, "
              "error %d", err);
        User::Leave(err);
    }


    TBuf8<512> midletDesc;
    TInt32  count = 0;
    TUint32 msId;
    TUint32 id;
    TUint32 uid;
    TPtrC   name;
    TPtrC   className;
    TPtrC   idString;
    while (tableIterator->HasNext())
    {
        TRAP(err, tableIterator->NextL(msId, id, uid, name, className, idString));
        if (KErrNone != err)
        {
            delete tableIterator;
            midletTable->Close();
            delete database;
            dbNamed.Close();
            dbs.Close();
            ELOG1(EJavaConverters,
                  "CPreConverter::AddDataFromSystemAmsDbL Midlet table iterator NextL failed, "
                  "error %d", err);
            User::Leave(err);
        }
        LOG3(EJavaConverters, EInfo,
             "CPreConverter::AddDataFromSystemAmsDbL Found midlet uid %x, suite id %d, midlet id %d",
             uid, msId, id);

        midletDesc.Append(KMLName);
        midletDesc.Append(name);
        midletDesc.Append(KMLClassName);
        midletDesc.Append(className);
        midletDesc.Append(KMLIdString);
        midletDesc.Append(idString);
        LOG(EJavaPreinstaller, EInfo, (const char *)midletDesc.PtrZ());
        midletDesc.Zero();

        iMidlets->SetIds(name, msId, id);
        count++;
    }

    LOG1(EJavaConverters, EInfo,
         "CPreConverter::AddDataFromSystemAmsDbL midlet table had %d midlets.", count);

    // close and free everything
    delete tableIterator;
    midletTable->Close();
    delete database;
    dbNamed.Close();
    dbs.Close();
}

/**
 * Check all midlets in iMidlets and remove those midlets that do not
 * have midlet id and suite id.
 */
void CPreConverter::RemoveInvalidMidlets()
{
    // If midlet id or suite id is KMaxTUint32, it has not been set ->
    // remove the midlet

    CMidletInfo *midlet = iMidlets->GetFirst();
    TBuf8<512> midletDesc;

    while (NULL != midlet)
    {
        if ((midlet->GetMidletId() == KMaxTUint32) ||
                (midlet->GetSuiteId() == KMaxTUint32))
        {
            midlet->ToString8(midletDesc);
            WLOG(EJavaConverters,
                 "CPreConverter::RemoveInvalidMidlets Not going to convert this midlet:");
            midletDesc.ZeroTerminate();
            WLOG(EJavaPreinstaller, (const char *)(midletDesc.Ptr()));
            midletDesc.Zero();
            iMidlets->Remove(midlet);
        }

        midlet = iMidlets->GetNext();
    }
}

/**
 * Store the uids of the midlets to be converted
 * to file 'uids' in the data cage of javaappconverter.exe
 */
void CPreConverter::StoreUidsL()
{
    TFileName exportUidsPath(KUidsExportDirectory);

    // Create directory if it does not exist
    TInt err= iFs.MkDirAll(exportUidsPath);
    if ((KErrNone != err) && (KErrAlreadyExists != err))
    {
        User::Leave(err);
    }

    // Store all uids to file 'uids' in the directory
    exportUidsPath.Append(KUidsExportDataFileName);

    // Construct write stream so that possibly existing old data file is replaced.
    RFileWriteStream writeStream;
    User::LeaveIfError(writeStream.Replace(iFs, exportUidsPath, EFileWrite));
    CleanupClosePushL(writeStream);

    // Write the number of midlets
    TUint32 nMidlets = iMidlets->Count();
    writeStream.WriteUint32L(nMidlets);

    // Write Uids of the midlets one by one
    CMidletInfo *midlet = iMidlets->GetFirst();

    while (NULL != midlet)
    {
        writeStream.WriteUint32L(midlet->GetMidletUid().iUid);
        midlet = iMidlets->GetNext();
    }

    // Closes writeStream
    CleanupStack::PopAndDestroy();
}

/**
 * Unregisters the applications to be converted from AppArc
 * so that Java Installer can make conversion installation
 * using the same Uids as the applications now have.
 *
 * Leaves with error code if AppArc cannot be connected or if
 * unregistrations cannot be committed.
 */
void CPreConverter::UnregisterOldJavaAppsL()
{
    // connect to AppArc
    RApaLsSession apaSession;

    TInt err = apaSession.Connect();
    if (KErrNone != err)
    {
        // Fatal error, try to connect again. The midlets cannot be converted
        // using the same uids as earlier if the unregistration cannot be done.
        TInt retryCount = 0;
        do
        {
            ELOG1(EJavaConverters,
                  "CPreConverter::UnregisterOldJavaAppsL Cannot connect to "
                  "AppArc server, err %d", err);

            // wait
            User::After(KDelayWhenWaitingAppArc);

            err = apaSession.Connect();
            if (KErrNone == err)
            {
                break;
            }

            retryCount++;
        }
        while (retryCount < 10);

        if (KErrNone != err)
        {
            User::Leave(err);
        }
    }
    CleanupClosePushL(apaSession);

    // Delete any pending (un)registrations (possible if
    // e.g. device rebooted before commit).
    // This call does nothing if there is no pending registrations.
    // Ignore errors.
    (void)apaSession.RollbackNonNativeApplicationsUpdates();

    // Prepare for Java application unregistrations
    TRAP(err, apaSession.PrepareNonNativeApplicationsUpdatesL());
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "CPreConverter::UnregisterOldJavaAppsL: "
              "PrepareNonNativeApplicationsUpdatesL leaved with err %d",
              err);
        User::Leave(err);
    }

    // Unregister all apps
    CMidletInfo *midlet = iMidlets->GetFirst();

    while (NULL != midlet)
    {
        TRAP(err, apaSession.DeregisterNonNativeApplicationL(midlet->GetMidletUid()));
        if (KErrNone != err)
        {
            WLOG2(EJavaConverters,
                  "CPreConverter::UnregisterOldJavaAppsL: "
                  "DeregisterNonNativeApplicationL leaved with err %d for uid %d",
                  err, midlet->GetMidletUid().iUid);
            // Ignore error, this particular application cannot be converted.
        }

        midlet = iMidlets->GetNext();
    }

    // Commit unregistrations
    TRAP(err, apaSession.CommitNonNativeApplicationsUpdatesL())
    {
        if (KErrNone != err)
        {
            ELOG1(EJavaConverters,
                  "CPreConverter::UnregisterOldJavaAppsL: "
                  "CommitNonNativeApplicationsUpdatesL leaved with err %d",
                  err);
            User::Leave(err);
        }
    }

    // Closes apaSession
    CleanupStack::PopAndDestroy();
}

