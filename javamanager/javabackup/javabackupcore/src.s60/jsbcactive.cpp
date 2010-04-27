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
* Description:  Implementation of Java Secure Backup Core's JsbcActive class
*
*/


#include "jsbcactive.h"
#include "jsbcdataowner.h"
#include "logger.h"

using namespace conn;
using namespace java::backup;

// ======== MEMBER FUNCTIONS ========

CJsbcActive::CJsbcActive():CActive(EPriorityNormal), iError(KErrNone)
{
    ILOG(EBackup, "CJsbcActive constructor");
}

void CJsbcActive::ConstructL()
{
    JELOG2(EBackup);

    // Set up the property to catch the BUR flag.
    TInt backupStateValue = 0;
    iProperty.Attach(KUidSystemCategory, KUidBackupRestoreKey);
    //LOG( EBackup, EInfo, "CJsbcActive::ConstructL, BackupRestoreKey attached" );
    ILOG(EBackup, "CJsbcActive::ConstructL, BackupRestoreKey attached");
    // Backup client and callback objects are constructed
    iCallbackImpl = CJsbcDataOwner::NewL(this);

    //LOG( EBackup, EInfo, "CJsbcActive::ConstructL, CJsbcDataOwner object created" );
    ILOG(EBackup, "CJsbcActive::ConstructL, CJsbcDataOwner object created");
    iActiveBackupClient = CActiveBackupClient::NewL(iCallbackImpl);
    //LOG( EBackup, EInfo,
    //    "CJsbcActive::ConstructL, CActiveBackupClient object created" );
    ILOG(EBackup,"CJsbcActive::ConstructL, CActiveBackupClient object created");

    // Check current state to see if we were started for backup purposes
    iProperty.Get(backupStateValue);

    if ((backupStateValue & KBURPartTypeMask) == EBURNormal)
    {
        ELOG(EBackup, "Backup flag not set");
        User::Leave(KErrCancel);
    }

    ProcessBackupStateL(backupStateValue);

    // Add to active scheduler
    CActiveScheduler::Add(this);

    // Subscribe to the P&S flag to catch transitions
    SubscribeToPSFlag();
}


CJsbcActive* CJsbcActive::NewL()
{
    JELOG2(EBackup);

    CJsbcActive* self = new(ELeave) CJsbcActive();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
}


CJsbcActive::~CJsbcActive()
{
    JELOG2(EBackup);
    Cancel();
    delete iActiveBackupClient;
    delete iCallbackImpl;
    iProperty.Close();
}


void CJsbcActive::SubscribeToPSFlag()
{
    JELOG2(EBackup);
    iStatus = KRequestPending;
    iProperty.Subscribe(iStatus);
    SetActive();
}


void CJsbcActive::ProcessBackupStateL(TInt aBackupStateValue)
{
    /*LOG1( EBackup, EInfo,
    "CJsbcActive::ProcessBackupStateL, BackupStateValue: %d",
    aBackupStateValue );*/
    JELOG2(EBackup);

    TInt type = aBackupStateValue & KBURPartTypeMask;
    ILOG1(EBackup, "CJsbcActive::ProcessBackupStateL, type: %d", type);
    // Test if the device is going into backup or restore mode, and we are
    // required to participate.
    if ((type == EBURBackupFull || type == EBURRestoreFull)
            || ((type == EBURBackupPartial || type == EBURRestorePartial)
                && iActiveBackupClient->DoesPartialBURAffectMeL()))
    {
        // The callback implementation is called to be able to prepare for the
        // backup or restore operation (e.g. to uninstall MIDlets before restore)
        iCallbackImpl->PrepareForBURL(type);

        ILOG(EBackup,
             "CJsbcActive::ProcessBackupStateL, iActiveBackupClient->ConfirmReadyForBURL");
        iActiveBackupClient->ConfirmReadyForBURL(KErrNone);
    }
    else
    {
        // Operation has finished or partial operation does not affect us
        // - ActiveScheduler should be stopped.
        if (type != EBURUnset)
        {
            ILOG(EBackup, "CJsbcActive::ProcessBackupStateL, CActiveScheduler::Stop()");
            CActiveScheduler::Stop();
        }
    }
}

TInt CJsbcActive::Error()
{
    WLOG(EBackup, "CJsbcActive::Error");
    return iError;
}

void CJsbcActive::DoCancel()
{
    WLOG(EBackup, "CJsbcActive::DoCancel");
    iProperty.Cancel();
}

void CJsbcActive::RunL()
{
    ILOG1(EBackup,
          "CJsbcActive::RunL, Status: %d", iStatus.Int());

    if (iStatus == KErrNone)
    {
        /* The P&S flag has changed state, which could mean that the device is
           either entering a backup, restore or normal state. */
        // To store the P&S value
        TInt backupStateValue = 0;

        iProperty.Get(backupStateValue);

        // Process the mode change accordingly
        ProcessBackupStateL(backupStateValue);

        // re-subscribe to the flag to monitor future changes
        SubscribeToPSFlag();
    }
    else
    {
        CActiveScheduler::Stop();
    }
}

TInt CJsbcActive::RunError(TInt aError)
{
    LOG1(EBackup, EInfo, "CJsbcActive::RunError, Error: %d", aError);

    // if a leave occurs in RunL(), we stop the scheduler
    CActiveScheduler::Stop();
    iError = aError;

    return KErrNone;
}

// ======== GLOBAL FUNCTIONS ========

GLDEF_C TInt E32MainL()
{
    CActiveScheduler* sched = new(ELeave) CActiveScheduler();

    CleanupStack::PushL(sched);

    CActiveScheduler::Install(sched);

    CJsbcActive* cjsbc = CJsbcActive::NewL();

    CActiveScheduler::Start();

    TInt error = cjsbc->Error();

    delete cjsbc;

    CleanupStack::PopAndDestroy(sched);

    return error;
}


GLDEF_C TInt E32Main()
{
    CTrapCleanup* cleanup = CTrapCleanup::New();

    TRAPD(error, E32MainL());

    delete cleanup;

    return error;
}
