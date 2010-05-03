/*******************************************************************************
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/

#include <org_eclipse_swt_internal_qt_OS.h>
#include <hwrmdomainpskeys.h>

#include "swtflipwatch.h"
#include "swtapplication.h"
#include "swts60.h"

namespace Java { namespace eSWT {

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
CSwtFlipWatch* CSwtFlipWatch::NewL()
{
    CSwtFlipWatch* self = new(ELeave) CSwtFlipWatch();
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
CSwtFlipWatch::CSwtFlipWatch()
        : CActive(KSwtFlipWatchPriority)
{
  
}

// ---------------------------------------------------------------------------
// CSwtFlipWatch::ConstructL
// ---------------------------------------------------------------------------
//
void CSwtFlipWatch::ConstructL()
{
    isInitializationState = ETrue;
    User::LeaveIfError(iProperty.Attach(KPSUidHWRM, KHWRMFlipStatus));
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
    // For performance reasons while in fast sequence of swaps the observers
    // will get screen deactivated event only once.
    if (!iFlipChanging)
        {
        SymbianUtils::eventFilter( swtApp, -1, org_eclipse_swt_internal_qt_OS_QSWTEVENT_SCREENDEACTIVATED);
        }
    iFlipChanging = ETrue;
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
    TInt err = iProperty.Get(flipStatus);
    if ( !isInitializationState && err == KErrNone)
        {
        iFlipChanging = EFalse;
        // Report flip change
        if (flipStatus ==  EPSHWRMFlipClosed)
            {
            SymbianUtils::eventFilter( swtApp, -1, org_eclipse_swt_internal_qt_OS_QSWTEVENT_MOBILEDEVICECLOSED);
            } else{
            SymbianUtils::eventFilter( swtApp, -1, org_eclipse_swt_internal_qt_OS_QSWTEVENT_MOBILEDEVICEOPENED);
            }
        // For performance reasons while in fast sequence of swaps the observers
        // will get screen deactivated event only once.
        // ASSUMPTION: The screen changes if the flip changes.
        SymbianUtils::eventFilter( swtApp, -1, org_eclipse_swt_internal_qt_OS_QSWTEVENT_SCREENACTIVATED);
        }
    isInitializationState = EFalse;
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

}}
