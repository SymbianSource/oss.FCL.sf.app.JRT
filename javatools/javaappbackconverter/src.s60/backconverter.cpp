/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Java platform 2.0 javaappbackconverter process.
*              Reregisters the old S60 MIDlets back to AppArc
*
*/


#include <e32base.h>
#include <apgicnfl.h>
#include <s32mem.h>
#include <e32property.h>

#include "javacommonutils.h"
#include "javauids.h"
#include "backconverter.h"
#include "noarmlogs.h"


const TInt KDelayWhenWaitingAppArc = 500000;

// This is in the private data cage of javaappbackconverter
_LIT(KMidletImportDirectory, "C:\\private\\20022D90\\data\\");

// Symbian file path separator
_LIT(KPathSeperator, "\\");

// Postfix for the fake application name generated only to make AppArc happy
_LIT(KAppPostfix, ".fakeapp");

// The application type Uid for MIDlets in S60
const TUid KUidMidletApplicationType = { 0x10210E26 };

/**
 * To create new instance of this class.
 *
 * @param aFs - A reference to the file server.
 * @return Reference to the object of this class.
 * @exception If construction fails.
 *
 */
CBackConverter* CBackConverter::NewLC(RFs& aFs)
{
    CBackConverter* self = new(ELeave) CBackConverter(aFs);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

/**
 * To do 1st phase construction for this object.
 *
 * Adds this active object to the scheduler.
 *
 * @param param aFs - A reference to the file server.
 * @return Reference to the object of this class.
 */
CBackConverter::CBackConverter(RFs& aFs) :
        CActive(EPriorityStandard), iFs(aFs)
{
    CActiveScheduler::Add(this);
}

/**
 * To do 2nd phase construction for this object.
 *
 * @exception If the method is not able to allocate necessary buffers.
 */
void CBackConverter::ConstructL()
{
    JELOG2(EJavaConverters);

    iMidlets = new(ELeave) CMidletList();
    iState = EInitialize;
}

/**
 * Deletes this object.
 * All allocated resources are released.
 */
CBackConverter::~CBackConverter()
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
 * To start back conversion
 */
void CBackConverter::Start()
{
    JELOG2(EJavaConverters);
    iState = EInitialize;
    CompleteRequest();
}

/**
 * To stop whole back conversion.
 * Stops the active scheduler.
 */
void CBackConverter::Exit()
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
void CBackConverter::CompleteRequest()
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
 * EInitialize:
 *   - if midlet export data file does not exist, exit
 *
 * EReadConversionData:
 *   - read data from data file to iMidlets
 *
 * EReregisterMidlets:
 *   - register each midlet in iMidlets to AppArc
 *
 * EExit:
 *   - free resources and exit
 *
 */
void CBackConverter::RunL()
{
    JELOG2(EJavaConverters);

    switch (iState)
    {

    case EInitialize:
    {
        LOG(EJavaConverters, EInfo,
            "CBackConverter::RunL EInitialize");

        TFileName dataFile(KMidletImportDirectory);
        dataFile.Append(KMidletExportDataFileName);
        TUint attributes;
        TInt  err = iFs.Att(dataFile, attributes);
        if (KErrNone != err)
        {
            WLOG1(EJavaConverters,
                  "CBackConverter::RunL EInitialize "
                  "Data file does not exist (%d). Cannot do back conversion.", err);
            LOG1WSTR(EJavaConverters, EInfo,
                     "Data file name is %s", (wchar_t *)(dataFile.PtrZ()));
            iState = EExit;
        }
        else
        {
            iState = EReadConversionData;
        }
        CompleteRequest();
    }
    break;

    case EReadConversionData:
    {
        LOG(EJavaConverters, EInfo,
            "CBackConverter::RunL EReadConversionData");

        // Read all midlet info from data file to iMidlets
        TFileName importDirectory(KMidletImportDirectory);
        iMidlets->ImportListL(iFs, importDirectory);

        iState = EUnregisterOldMidletData;
        CompleteRequest();
    }
    break;

    case EUnregisterOldMidletData:
    {
        LOG(EJavaConverters, EInfo,
            "CBackConverter::RunL EUnregisterOldMidletData");

        // If executing backconverter after user cancel,
        // AppArc may contain Java 2.0 specific data
        // for converted midlets. Remove it or reregistration
        // will not update the data.
        UnregisterOldMidletData();

        iState = EReregisterMidlets;
        CompleteRequest();
    }
    break;

    case EReregisterMidlets:
    {
        LOG(EJavaConverters, EInfo,
            "CBackConverter::RunL EReregisterMidlets");

        // Reregister old midlet info back to AppArc
        RegisterMidletsL();

        // If this line is executed, registering old midlets
        // back to AppArc succeeded. So the data file can be
        // removed.
        RemoveDataFile();

        iState = EExit;
        CompleteRequest();
    }
    break;

    case EExit:
    {
        LOG(EJavaConverters, EInfo, "CBackConverter::RunL EExit");

        FullCleanup();

        // The whole javaappbackconverter process is stopped.
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
TInt CBackConverter::RunError(TInt aError)
{
    ELOG2(EJavaConverters,
          "CBackConverter::RunError(%d) from state %d", aError, iState);

    Cancel();

    iState = EExit;
    CompleteRequest();

    return KErrNone;
}

/**
 * To do cancelling for this object.
 *
 */
void CBackConverter::DoCancel()
{
    ELOG1(EJavaConverters,
          "CBackConverter::DoCancel from state %d", iState);

}

/**
 * To cleanup member variables.
 */
void CBackConverter::FullCleanup()
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
 * Register all midlets in iMidlets to AppArc.
 */
void CBackConverter::RegisterMidletsL()
{
    RApaLsSession apaSession;

    // open AppArc session
    TInt err = apaSession.Connect();
    if (KErrNone != err)
    {
        // Cannot connect to AppArc server
        ELOG(EJavaConverters,
             "CBackConverter::RegisterMidletsL: Cannot connect to AppArc server");
        User::Leave(err);
    }
    CleanupClosePushL(apaSession);

    // Delete any pending (un)registrations (possible if
    // e.g. device rebooted before commit).
    // This call does nothing if there is no pending registrations.
    // Ignore errors.
    (void)apaSession.RollbackNonNativeApplicationsUpdates();

    // Prepare for Java application registrations / unregistrations
    TRAP(err, apaSession.PrepareNonNativeApplicationsUpdatesL());
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "CBackConverter::RegisterMidletsL: "
              "PrepareNonNativeApplicationsUpdatesL leaved with err %d",
              err);
        User::Leave(err);
    }

    // register midlets one by one
    CMidletInfo *midlet = iMidlets->GetFirst();
    TBuf8<512>   midletDesc;

    while (NULL != midlet)
    {
        midlet->ToString8(midletDesc);
        WLOG(EJavaConverters,
             "CBackConverter::RegisterMidletsL Going to reregister this midlet:");
        midletDesc.ZeroTerminate();
        WLOG(EJavaPreinstaller, (const char *)(midletDesc.Ptr()));
        midletDesc.Zero();

        RegisterOneMidletL(&apaSession, midlet);
        midlet = iMidlets->GetNext();
    }

    // commit registrations
    TRAP(err, apaSession.CommitNonNativeApplicationsUpdatesL());
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "CBackConverter::RegisterMidletsL: "
              "CommitNonNativeApplicationsUpdatesL leaved with err %d",
              err);
        User::Leave(err);
    }

    CleanupStack::PopAndDestroy(); // apaSession
}

/**
 * Register one midlet to AppArc
 *
 * @param aSession AppArc session
 * @param aMidlet  the midlet to be registered
 * @exception If registering application fails
 */
void CBackConverter::RegisterOneMidletL(RApaLsSession *aSession, CMidletInfo *aMidlet)
{
    JELOG2(EJavaPreinstaller);

    RFile appArcIcon;
    TInt  numberOfIcons = 1;

    // open the icon in the shared mode required by AppArc
    TInt err = appArcIcon.Open(
                   iFs,
                   aMidlet->GetIconFileName(),
                   (EFileShareReadersOrWriters | EFileWrite));
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "CBackConverter::RegisterOneMidletL: Cannot open icon, err code is %d ",
              err);
        User::Leave(err);
    }

    // Now icon is open
    CleanupClosePushL(appArcIcon);

    // Generate the executable name name using the same
    // algorithm as used earlier in S60 platform
    // in case some external S60 application
    // needs to parse the Uid from the executable name.
    TFileName appName;
    appName.Copy(KPathSeperator);
    appName.AppendNum(aMidlet->GetMidletUid().iUid);
    appName.Append(KAppPostfix);

    // Create writer needed for actual registration
    CApaRegistrationResourceFileWriter *writer = NULL;
    writer =
        CApaRegistrationResourceFileWriter::NewL(
            aMidlet->GetMidletUid(),
            appName,
            TApaAppCapability::ENonNative);
    CleanupStack::PushL(writer);

    writer->SetGroupNameL(aMidlet->GetGroupName());

    // Write MIDlet suite id and MIDlet id to opaque data  (needed by S60 MIDlet launcher)
    TBuf8<8>         opaqueData;     // opaque data will contain two signed 32-bit int
    RDesWriteStream  writeStream(opaqueData);
    writeStream.WriteInt32L(aMidlet->GetSuiteId());
    writeStream.WriteInt32L(aMidlet->GetMidletId());
    writeStream.CommitL();
    writer->SetOpaqueDataL(opaqueData);

    CApaLocalisableResourceFileWriter*  lWriter = NULL;
    lWriter =
        CApaLocalisableResourceFileWriter::NewL(
            KNullDesC,  // short caption
            aMidlet->GetMidletName(),  // caption
            numberOfIcons,
            KNullDesC);
    CleanupStack::PushL(lWriter);

    TDriveUnit targetDrive(aMidlet->GetDrive());

    // Register application to AppArc
    aSession->RegisterNonNativeApplicationL(
        KUidMidletApplicationType,
        targetDrive,
        *writer,
        lWriter,
        &appArcIcon);

    CleanupStack::PopAndDestroy(lWriter);
    CleanupStack::PopAndDestroy(writer);
    // close icon file handle
    CleanupStack::PopAndDestroy();  // appArcIcon
}

/**
 * Unregisters the applications to be back converted from AppArc
 * so that they can be registered again with the same Uid but with
 * new opaque data (needed for old S60 Java).
 * This function needs to called only if making back conversion after
 * the user has canceled the installation of Java 2.0.
 * If back conversion is done while uninstalling Java 2.0, all Java applications
 * have been uninstalled and there are no Uids to be unregistered.
 *
 * Leaves with error code if AppArc cannot be connected or if
 * unregistrations cannot be committed.
 */
void CBackConverter::UnregisterOldMidletData()
{
    // connect to AppArc
    RApaLsSession apaSession;

    TInt err = apaSession.Connect();
    if (KErrNone != err)
    {
        // Fatal error, try to connect again. The midlets cannot be back converted
        // using the same uids as earlier if the unregistration cannot be done.
        TInt retryCount = 0;
        do
        {
            ELOG1(EJavaConverters,
                  "CBackConverter::UnregisterCurrentJavaAppsL Cannot connect to "
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
            return;
        }
    }

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
              "CBackConverter::UnregisterCurrentJavaAppsL: "
              "PrepareNonNativeApplicationsUpdatesL leaved with err %d",
              err);
        apaSession.Close();
        return;
    }

    // Unregister all apps
    CMidletInfo *midlet = iMidlets->GetFirst();

    while (NULL != midlet)
    {
        TRAP(err, apaSession.DeregisterNonNativeApplicationL(midlet->GetMidletUid()));
        if (KErrNone != err)
        {
            WLOG2(EJavaConverters,
                  "CBackConverter::UnregisterCurrentJavaAppsL: "
                  "DeregisterNonNativeApplicationL leaved with err %d for uid %d",
                  err, midlet->GetMidletUid().iUid);
            // Ignore error, this particular application cannot be back converted.
        }

        midlet = iMidlets->GetNext();
    }

    // Commit unregistrations
    TRAP(err, apaSession.CommitNonNativeApplicationsUpdatesL())
    {
        if (KErrNone != err)
        {
            ELOG1(EJavaConverters,
                  "CBackConverter::UnregisterCurrentJavaAppsL: "
                  "CommitNonNativeApplicationsUpdatesL leaved with err %d",
                  err);
            apaSession.Close();
            return;
        }
    }

    apaSession.Close();
    return;
}

/**
 * Remove data file that contains info about midlets to be back converted.
 */
void CBackConverter::RemoveDataFile()
{
    TFileName dataFile(KMidletImportDirectory);
    dataFile.Append(KMidletExportDataFileName);

    TInt err = iFs.Delete(dataFile);
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "CBackConverter::RemoveDataFile: Cannot delete data file, err %d ",
              err);
    }
}
