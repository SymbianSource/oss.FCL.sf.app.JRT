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
* Description:  Handles timeouts for CTrackingPositioner
 *
*/


// INCLUDE FILES
#include "ctimeouttimer.h"

using namespace java::location;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTimeoutTimer::CTimeoutTimer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTimeoutTimer::CTimeoutTimer(const TCallBack& aCallBack) :
        CTimer(EPriorityNormal), iCallBack(aCallBack)
{
    CActiveScheduler::Add(this);
}

// -----------------------------------------------------------------------------
// CTimeoutTimer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTimeoutTimer* CTimeoutTimer::NewL(const TCallBack& aCallBack)
{
    CTimeoutTimer* self = new(ELeave) CTimeoutTimer(aCallBack);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// Destructor
CTimeoutTimer::~CTimeoutTimer()
{
    Cancel();
}

// -----------------------------------------------------------------------------
// CTimeoutTimer::TimeoutAfter
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTimeoutTimer::TimeoutAfter(const TTimeIntervalMicroSeconds& aDelay)
{
    if (aDelay < MAKE_TINT64(0, KMaxTInt))
    {
        CTimer::After(I64INT(aDelay.Int64()));
    }
    else
    {
        TTime timeoutTime;
        timeoutTime.HomeTime();
        timeoutTime += aDelay;
        CTimer::At(timeoutTime);
    }
}

// -----------------------------------------------------------------------------
// CTimeoutTimer::RunL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTimeoutTimer::RunL()
{
    if (iStatus == KErrNone)
    {
        iCallBack.CallBack();
    }
}

//  End of File
