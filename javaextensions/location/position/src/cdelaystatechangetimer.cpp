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
* Description:  Timer used to delay state changes
 *
*/


// INCLUDE FILES
#include "cdelaystatechangetimer.h"
#include "clocationprovider.h"

using namespace java::location;

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDelayStateChangeTimer::CDelayStateChangeTimer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDelayStateChangeTimer::CDelayStateChangeTimer(
    CLocationProvider* aLocationProvider) :
        CTimer(EPriorityNormal), iLocationProvider(aLocationProvider)
{
    CActiveScheduler::Add(this);
}

// -----------------------------------------------------------------------------
// CDelayStateChangeTimer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDelayStateChangeTimer* CDelayStateChangeTimer::NewL(
    CLocationProvider* aLocationProvider)
{
    CDelayStateChangeTimer* self =
        new(ELeave) CDelayStateChangeTimer(aLocationProvider);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// Destructor
CDelayStateChangeTimer::~CDelayStateChangeTimer()
{
    Cancel();
}

// -----------------------------------------------------------------------------
// CDelayStateChangeTimer::ChangeStateAfter
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CDelayStateChangeTimer::ChangeStateAfter(
    const TTimeIntervalMicroSeconds32& aInterval, TInt aNewState)
{
    iNewState = aNewState;
    CTimer::After(aInterval);
}

// -----------------------------------------------------------------------------
// CDelayStateChangeTimer::RunL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CDelayStateChangeTimer::RunL()
{
    iLocationProvider->ChangeState(iNewState);
}

//  End of File
