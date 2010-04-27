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


#include <commdbconnpref.h>
#include "swtbrowserconnector.h"


// ======== MEMBER FUNCTIONS ========


CSwtBrowserConnector::CSwtBrowserConnector(RConnection& aConnection,
        TInt aPriority)
        : CActive(aPriority)
        , iConnection(aConnection)
{
    CActiveScheduler::Add(this);  //inserting this into the queue
}

CSwtBrowserConnector::~CSwtBrowserConnector()
{
    Cancel();//The standard way of destroying an Active object
}

void CSwtBrowserConnector::StartConnection(TCommDbConnPref* aSettings,
        TRequestStatus& aStatus)
{
    iExternalRequestStatus = &aStatus;

    if (aSettings)
    {
        iConnection.Start(*aSettings, iStatus);
    }
    else
    {
        iConnection.Start(iStatus);
    }

    SetActive();

    *iExternalRequestStatus = KRequestPending;
}

void CSwtBrowserConnector::DoCancel()
{
    iConnection.Close();
    User::RequestComplete(iExternalRequestStatus, KErrCancel);  //completing user req
}

void CSwtBrowserConnector::RunL()
{
    User::RequestComplete(iExternalRequestStatus, iStatus.Int());
}


// CSwtBrowserConnectorSyncWrapper

CSwtBrowserConnectorSyncWrapper::CSwtBrowserConnectorSyncWrapper(TInt aPriority)
        : CActive(aPriority)
{
    CActiveScheduler::Add(this);
}

CSwtBrowserConnectorSyncWrapper::~CSwtBrowserConnectorSyncWrapper()
{
    Cancel();
    delete iActiveConnector;
}

CSwtBrowserConnectorSyncWrapper* CSwtBrowserConnectorSyncWrapper::NewL(
    RConnection& aConnection, TInt aPriority)
{
    CSwtBrowserConnectorSyncWrapper * self
    = CSwtBrowserConnectorSyncWrapper::NewLC(aConnection, aPriority);
    CleanupStack::Pop(self);
    return self;
}

CSwtBrowserConnectorSyncWrapper * CSwtBrowserConnectorSyncWrapper::NewLC(
    RConnection& aConnection, TInt aPriority)
{
    CSwtBrowserConnectorSyncWrapper * self
    = new(ELeave)CSwtBrowserConnectorSyncWrapper(aPriority);
    CleanupStack::PushL(self);
    self->ConstructL(aConnection);
    return self;
}

void CSwtBrowserConnectorSyncWrapper::ConstructL(RConnection& aConnection)
{
    iActiveConnector = new(ELeave) CSwtBrowserConnector(aConnection, Priority());
}

// ---------------------------------------------------------
// The function will return on error or when the connection has been created
// The agent that was passed must be opened before calling this function
// and must not be closed before it completes
// ---------------------------------------------------------
//
TInt CSwtBrowserConnectorSyncWrapper::Connect(TCommDbConnPref* aSettings)
{
    iActiveConnector->StartConnection(aSettings, iStatus);
    SetActive();
    iWait.Start();
    return iStatus.Int();
}

void CSwtBrowserConnectorSyncWrapper::RunL()
{
    iWait.AsyncStop();
}

void CSwtBrowserConnectorSyncWrapper::DoCancel()
{
    iActiveConnector->Cancel();
    iWait.AsyncStop();
}
