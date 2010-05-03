/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#include "globalmsgobserver.h"

using namespace java::runtimeui;

EXPORT_C GlobalMsgObserver* GlobalMsgObserver::NewL()
{
    GlobalMsgObserver* self = new(ELeave) GlobalMsgObserver();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

EXPORT_C GlobalMsgObserver* GlobalMsgObserver::NewLC()
{
    GlobalMsgObserver* self = new(ELeave) GlobalMsgObserver();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

EXPORT_C int GlobalMsgObserver::getAnswer()
{
    // set the active object as active
    SetActive();
    // enter scheduler loop
    iActiveSchedulerWait->Start();
    return iStatus.Int();
}

void GlobalMsgObserver::ConstructL()
{
    if (!CActiveScheduler::Current())
    {
        // create own ActiveScheduler since one doesn't exist yet
        iActiveScheduler = new(ELeave) CActiveScheduler;
        CActiveScheduler::Install(iActiveScheduler);
    }
    // register myself as an active object
    CActiveScheduler::Add(this);
    // create wait object for nested loop control
    iActiveSchedulerWait = new(ELeave) CActiveSchedulerWait;
}

GlobalMsgObserver::~GlobalMsgObserver()
{
    // cancel any outstanding requests
    Cancel();
    // delete own active scheduler if it was created
    delete iActiveScheduler;
    // delete wait object that is always created
    delete iActiveSchedulerWait;
}

void GlobalMsgObserver::Complete()
{
    // stop the active scheduler loop
    iActiveSchedulerWait->AsyncStop();
}

void GlobalMsgObserver::DoCancel()
{
    // This implementation is not correct. It should call the cancel provided
    // by the asynchronous service provider. If something leaves while in the
    // scheduler loop in getAnswer() then now it will try to stop the scheduler
    // which isn't running anymore because of the leave and there will be
    // TooManyStops panic.
    Complete();
}

void GlobalMsgObserver::RunL()
{
    Complete();
}
