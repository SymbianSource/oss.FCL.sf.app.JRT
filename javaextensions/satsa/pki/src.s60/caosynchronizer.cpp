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
* Description:
*
*/


#include "caosynchronizer.h"



EXPORT_C CAOSynchronizer* CAOSynchronizer::NewL()
{
    CAOSynchronizer* self = new(ELeave) CAOSynchronizer();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

CAOSynchronizer::~CAOSynchronizer()
{
    Cancel();
    delete iTimer;
    delete iCas;
    delete iWait;
}

void CAOSynchronizer::ConstructL()
{
    if (CActiveScheduler::Current() == NULL)
    {
        // Need to create our own ActiveScheduler
        iCas = new(ELeave) CActiveScheduler;
        CActiveScheduler::Install(iCas);
    }

    // Register as an active object
    CActiveScheduler::Add(this);
    iWait = new(ELeave) CActiveSchedulerWait();
}

EXPORT_C void CAOSynchronizer::ExecuteL(TTimeIntervalMicroSeconds32 aTimeout)
{
    delete iTimer;
    iTimer = NULL;
    if (aTimeout > (TTimeIntervalMicroSeconds32)0)
    {
        iTimer = CTimeOutTimer::NewL(CActive::EPriorityStandard, *this);
        iTimer->After(aTimeout);
    }

    SetActive();

    if (iCas)
    {
        iCas->Start();
    }
    else
    {
        if (!iWait->IsStarted())
        {
            iWait->Start();
        }
    }
}

void CAOSynchronizer::Complete()
{
    if (iTimer)
    {
        iTimer->Cancel();
    }

    if (iCas)
    {
        iCas->Stop();
    }
    else
    {
        if (iWait->IsStarted())
        {
            iWait->AsyncStop();
        }
    }
}

// From CActive
void CAOSynchronizer::DoCancel()
{
    Complete();
}

void CAOSynchronizer::RunL()
{
    Complete();
}

// From CTimeoutTimer
void CAOSynchronizer::TimerExpired()
{
    Complete();
}


