/*******************************************************************************
 * Copyright (c) 2007, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#include <nifman.h>
#include <in_sock.h>
#include <es_enum.h>
#include <cdbcols.h>
#include "swtbrowserconnectionobserver.h"


// ======== MEMBER FUNCTIONS ========


CSwtBrowserConnectionObserver::CSwtBrowserConnectionObserver()
        : CActive(EPriorityStandard)
{
    CActiveScheduler::Add(this);
}

CSwtBrowserConnectionObserver::~CSwtBrowserConnectionObserver()
{
    Cancel();
    DoCloseAgent();
}

void CSwtBrowserConnectionObserver::DoCancel()
{
    iConnection.CancelProgressNotification();
    DoCloseAgent();
}

void CSwtBrowserConnectionObserver::RunL()
{
    if (iProgressBuf().iStage == iStageToObserve)
    {
        DoCloseAgent();
        iObserver->ConnectionStageAchievedL();
    }
    else//reissuing request
    {
        iConnection.ProgressNotification(iProgressBuf, iStatus);  //reissuing request
        SetActive();
    }
}

void CSwtBrowserConnectionObserver::StartNotificationL(TName* aConnName, TInt aStageToObserve,
        MConnectionStageObserver* aObserver, TBool aCompleteIfAlreadyAtStage)
{
    __ASSERT_DEBUG(aObserver, User::Panic(KNullDesC, KErrArgument));

    iObserver = aObserver;
    iStageToObserve = aStageToObserve;

    DoOpenAgentL(aConnName);

    TNifProgress progress;
    iConnection.Progress(progress);
    if (progress.iStage == aStageToObserve && aCompleteIfAlreadyAtStage)
    {
        DoCloseAgent();
        TRAP_IGNORE(iObserver->ConnectionStageAchievedL());
    }
    else
    {
        iConnection.ProgressNotification(iProgressBuf, iStatus);  //issuing request
        SetActive();
    }
}

void CSwtBrowserConnectionObserver::DoOpenAgentL(TName* aConnName)
{
    TInt err(KErrNone);
    err = iServer.Connect();
    if (err == KErrNone)
    {
        err = iConnection.Open(iServer, *aConnName);
        if (!err)
        {
            const TInt KBufLen(64);
            TBuf< KBufLen > query;
            TUint32 apId;
            TUint32 netId;
            _LIT(KFormatStr, "%s\\%s");

            query.Format(KFormatStr, IAP, COMMDB_ID);
            if (iConnection.GetIntSetting(query, apId) == KErrNone)
            {
                query.Format(KFormatStr, IAP, IAP_NETWORK);
                if (iConnection.GetIntSetting(query, netId) == KErrNone)
                {
                    TPckgBuf<TConnectionInfo> connInfo;

                    connInfo().iIapId = apId;
                    connInfo().iNetId = netId;

                    iConnection.Close();
                    if (iConnection.Open(iServer, KAfInet) == KErrNone)
                    {
                        err = iConnection.Attach(connInfo, RConnection::EAttachTypeMonitor);
                    }
                }
            }
        }
    }

    if (err)
    {
        iConnection.Close();
        iServer.Close();
        User::Leave(err);
    }
}

void CSwtBrowserConnectionObserver::DoCloseAgent()
{
    iConnection.Close();
    iServer.Close();
}
