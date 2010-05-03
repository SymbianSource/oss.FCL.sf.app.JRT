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
* Description:  This class implements general wait object for calling a desired
 *                callback function after asynchronous notification
 *
*/


#include "asyncwaitcallback.h"
#include "logger.h"

using namespace Java::Installer;

// ============================ MEMBER FUNCTIONS ===============================

CAsyncWaitCallBack* CAsyncWaitCallBack::NewL(TCallBack aCallBack)
{
    CAsyncWaitCallBack* self = new(ELeave) CAsyncWaitCallBack();
    CleanupStack::PushL(self);
    self->ConstructL(aCallBack);
    CleanupStack::Pop(self);
    return self;
}


CAsyncWaitCallBack::CAsyncWaitCallBack():
        CActive(CActive::EPriorityStandard), iActivatingReasonCode(0)
{
}


CAsyncWaitCallBack::~CAsyncWaitCallBack()
{
}


void CAsyncWaitCallBack::ConstructL(TCallBack aCallBack)
{
    iCallBack = aCallBack;
    CActiveScheduler::Add(this);
}


void CAsyncWaitCallBack::RunL()
{
    // Execute callback only if the request completed with correct
    // status code
    if (iStatus == iActivatingReasonCode)
    {
        iCallBack.CallBack();
    }
    else
    {
        WLOG1(EJavaInstaller,
              "CAsyncWaitCallBack was called with unexpected notif code %d, reactivate",
              iStatus.Int());
        // Reactivate wait
        iProcessToListen.Rendezvous(iStatus);
        SetActive();
    }
}


void CAsyncWaitCallBack::DoCancel()
{
    iProcessToListen.RendezvousCancel(iStatus);
}


void CAsyncWaitCallBack::Wait(RProcess aProcessToListen, TInt aActivatingReasonCode)
{
    iActivatingReasonCode = aActivatingReasonCode;
    iProcessToListen = aProcessToListen;
    iProcessToListen.Rendezvous(iStatus);
    SetActive();
}

