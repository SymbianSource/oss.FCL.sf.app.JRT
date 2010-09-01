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


#include <swtlaffacade.h>
#include "swtflipwatch.h"


// Priority of the flip watch CActive. If for some reason the flip watch
// CActive gets blocked, the watch timer should wait also. Therefore the
// priority of the watch timer should be lower than this.
const TInt KSwtFlipWatchPriority = 0;

// Using a short delay on the flip watch will avoid ui unresponsiveness when
// fast swaping the flip back and forth.
const TInt KSwtFlipWatchTimeout = 250000;


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtFlipWatch::NewL
// ---------------------------------------------------------------------------
//
CSwtFlipWatch* CSwtFlipWatch::NewL(MSwtFlipObserver* aObserver)
{
    CSwtFlipWatch* self = new(ELeave) CSwtFlipWatch(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtFlipWatch::~CSwtFlipWatch
// ---------------------------------------------------------------------------
//
CSwtFlipWatch::~CSwtFlipWatch()
{
    Cancel();
    iProperty.Close();

    if (iTimer)
    {
        iTimer->Cancel();
        delete iTimer;
    }
}


// ---------------------------------------------------------------------------
// CSwtFlipWatch::CSwtFlipWatch
// ---------------------------------------------------------------------------
//
CSwtFlipWatch::CSwtFlipWatch(MSwtFlipObserver* aObserver)
        : CActive(KSwtFlipWatchPriority), iObserver(aObserver)
        , iFlipOpen(ETrue)
{
    ASSERT(iObserver);
}

// ---------------------------------------------------------------------------
// CSwtFlipWatch::ConstructL
// ---------------------------------------------------------------------------
//
void CSwtFlipWatch::ConstructL()
{
    User::LeaveIfError(iProperty.Attach(CSwtLafFacade::GetFlipWatchUid(), CSwtLafFacade::GetFlipWatchStatus()));
    iTimer = CSwtFlipWatchTimer::NewL(this);
    CActiveScheduler::Add(this);
    RunL();
}

// ---------------------------------------------------------------------------
// CSwtFlipWatch::DoCancel
// ---------------------------------------------------------------------------
//
void CSwtFlipWatch::DoCancel()
{
    iProperty.Cancel();
}

// ---------------------------------------------------------------------------
// CSwtFlipWatch::RunL
// ---------------------------------------------------------------------------
//
void CSwtFlipWatch::RunL()
{
    iTimer->Cancel();
    iProperty.Subscribe(iStatus);
    iObserver->FlipChangingL();
    iTimer->SetTimer(KSwtFlipWatchTimeout);
    SetActive();
}

// ---------------------------------------------------------------------------
// CSwtFlipWatch::HandleTimeoutL
// ---------------------------------------------------------------------------
//
void CSwtFlipWatch::HandleTimeoutL()
{
    TInt flipStatus = 0;
    TBool flipOpen = ETrue;
    TInt err = iProperty.Get(flipStatus);
    if (err == KErrNone)
    {
        if (flipStatus == CSwtLafFacade::GetFlipWatchEnumerationStatus())
        {
            flipOpen = EFalse;
        }
        if (iFlipOpen != flipOpen)
        {
            iObserver->FlipChangedL(flipOpen);
            iFlipOpen = flipOpen;
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtFlipWatchTimer::NewL
// ---------------------------------------------------------------------------
//
CSwtFlipWatchTimer* CSwtFlipWatchTimer::NewL(MSwtFlipWatchTimerHandler* aHandler)
{
    CSwtFlipWatchTimer* self = new(ELeave) CSwtFlipWatchTimer(aHandler);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtFlipWatchTimer::CSwtFlipWatchTimer
// ---------------------------------------------------------------------------
//
CSwtFlipWatchTimer::CSwtFlipWatchTimer(MSwtFlipWatchTimerHandler* aHandler)
        : CActive(KSwtFlipWatchPriority - 1)
        , iHandler(aHandler)
{
    ASSERT(iHandler);
}

// ---------------------------------------------------------------------------
// CSwtFlipWatchTimer::ConstructL
// ---------------------------------------------------------------------------
//
void CSwtFlipWatchTimer::ConstructL()
{
    TInt err = iTimer.CreateLocal();
    User::LeaveIfError(err);
    CActiveScheduler::Add(this);
}

// ---------------------------------------------------------------------------
// CSwtFlipWatchTimer::~CSwtFlipWatchTimer
// ---------------------------------------------------------------------------
//
CSwtFlipWatchTimer::~CSwtFlipWatchTimer()
{
    Cancel();
    iTimer.Close();
}

// ---------------------------------------------------------------------------
// CSwtFlipWatchTimer::RunL
// ---------------------------------------------------------------------------
//
void CSwtFlipWatchTimer::RunL()
{
    iHandler->HandleTimeoutL();
}

// ---------------------------------------------------------------------------
// CSwtFlipWatchTimer::DoCancel
// ---------------------------------------------------------------------------
//
void CSwtFlipWatchTimer::DoCancel()
{
    iTimer.Cancel();
}

// ---------------------------------------------------------------------------
// CSwtFlipWatchTimer::SetTimer
// ---------------------------------------------------------------------------
//
void CSwtFlipWatchTimer::SetTimer(TTimeIntervalMicroSeconds32 aDelay)
{
    Cancel();
    iTimer.After(iStatus, aDelay);
    SetActive();
}
