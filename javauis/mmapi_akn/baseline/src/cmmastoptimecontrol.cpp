/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is used for stoptime controlling
*
*/


//  INCLUDE FILES
#include <jdebug.h>
#include <e32std.h>

#include "cmmastoptimecontrol.h"
#include "cmmaplayer.h"


_LIT(KControlName, "StopTimeControl");

_LIT(KMMAStopTimeControlError, "StopTimeControl Symbian OS error: %d");
const TInt KMMAStopTimeControlErrorLength = 50;

CMMAStopTimeControl::CStopTimer* CMMAStopTimeControl::CStopTimer::NewL(
    CMMAStopTimeControl* aControl)
{
    CStopTimer* self = new(ELeave) CStopTimer(aControl);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}


CMMAStopTimeControl::CStopTimer::CStopTimer(CMMAStopTimeControl* aControl)
        : CTimer(CActive::EPriorityStandard)
{
    iControl = aControl;
}


void CMMAStopTimeControl::CStopTimer::ConstructL()
{
    CTimer::ConstructL();
    CActiveScheduler::Add(this);
}


void CMMAStopTimeControl::CStopTimer::RunL()
{
    DEBUG("MMA:CMMAStopTimeControl:CStopTimer:RunL timer triggered");
    iControl->StopAtTimeL();
}

const TDesC& CMMAStopTimeControl::ClassName() const
{
    return KControlName;
}


CMMAStopTimeControl* CMMAStopTimeControl::NewL(CMMAPlayer* aPlayer)
{
    CMMAStopTimeControl* control =
        new(ELeave) CMMAStopTimeControl(aPlayer);
    CleanupStack::PushL(control);
    control->ConstructL();
    CleanupStack::Pop(); // control
    return control;
}


CMMAStopTimeControl::~CMMAStopTimeControl()
{
    delete iTimer;
}


CMMAStopTimeControl::CMMAStopTimeControl(CMMAPlayer* aPlayer)
        : iPlayer(aPlayer), iNoTimer((MAKE_TINT64(KMaxTInt, KMaxTUint)))
{
    iStopTime = iNoTimer;
}


void CMMAStopTimeControl::ConstructL()
{
    iTimer = CStopTimer::NewL(this);
    iPlayer->AddStateListenerL(this);
}


void CMMAStopTimeControl::StaticGetStopTime(CMMAStopTimeControl* aControl,
        TInt64* aTime)
{
    aControl->GetStopTime(*aTime);
}


void CMMAStopTimeControl::StaticSetStopTimeL(CMMAStopTimeControl* aControl,
        TInt64* aTime)
{
    aControl->SetStopTimeL(*aTime);
}


void CMMAStopTimeControl::StopAtTimeL()
{
    DEBUG("MMA:CMMAStopTimeControl::StopAtTime");

    // Stop the player only when it's playing
    if (iPlayer->State() == CMMAPlayer::EStarted)
    {
        TInt64 time;
        iPlayer->GetMediaTime(&time);
        if (time >= 0 && time < iStopTime)
        {
            DEBUG_INT("MMA:CMMAStopTimeControl::StopAtTime - Called %dms too early",
                      I64INT((time - iStopTime)/1000));
            StartTimer(time);
            return;
        }

        TInt64 stopTime;

        // MediaTime is known
        if (time >= 0)
        {
            DEBUG_INT("MMA:CMMAStopTimeControl::StopAtTime - called %dms late", I64INT((time - iStopTime)/1000));
            stopTime = time;
        }
        else
        {
            // Use the requested time
            stopTime = iStopTime;
        }

        iPlayer->StopL(EFalse);

        // Inform the player that it's "stopped at time"
        iPlayer->PostLongEvent(CMMAPlayerEvent::EStoppedAtTime, stopTime);

        iStopTime = iNoTimer; // Timer is reseted
    }
}


/**
 * Get stop time, in microseconds
 */
void CMMAStopTimeControl::GetStopTime(TInt64& aTime)
{
    aTime = iStopTime;
}


/**
 * Set stop time, in microseconds
 * @param aTime is iNoTimer if the timer should be reseted
 */
void CMMAStopTimeControl::SetStopTimeL(const TInt64& aTime)
{
    iStopTime = aTime;

    if (aTime != iNoTimer)
    {
        DEBUG_INT("MMA:CMMAStopTimeControl:SetStopTime(%dms)",
                  I64INT(aTime / 1000));

        if (iPlayer->State() == CMMAPlayer::EStarted)
        {
            TInt64 currentTime(0);
            iPlayer->GetMediaTime(&currentTime);

            StartTimer(currentTime);
        }
    }
    else
    {
        DEBUG("MMA:CMMAStopTimeControl:SetStopTime(RESET)");

        iTimer->Cancel();
    }
}


/*
 * Start timer
 * @param aCurrentTime current position of the player
 */
void CMMAStopTimeControl::StartTimer(const TInt64& aCurrentTime)
{
    // StopTime is defined
    TInt64 time = iStopTime - aCurrentTime;
    DEBUG_INT("MMA:CMMAStopTimeControl:StartTimer timer started; time=%dms",
              I64INT(time / 1000));
    iTimer->Cancel();

    if (time >= 0)
    {
        // Value is too large to represent with TInt
        // use the biggest possible value instead
        if (I64HIGH(time) != 0 || I64INT(time) < 0)
        {
            time = KMaxTInt;
        }

        iTimer->After(TTimeIntervalMicroSeconds32(I64INT(time)));
    }
    else
    {
        // Stop the player immediatelly
        TRAPD(err,  StopAtTimeL());
        if (err != KErrNone)
        {
            TBuf< KMMAStopTimeControlErrorLength > errorMsg;
            errorMsg.Format(KMMAStopTimeControlError, err);
            iPlayer->PostStringEvent(CMMAPlayerEvent::EError,
                                     errorMsg);
        }
    }
}


void CMMAStopTimeControl::StateChanged(TInt aState)
{
    switch ((CMMAPlayer::TPlayerState) aState)
    {
    case CMMAPlayer::EStarted:
    {
        if (iStopTime != iNoTimer)
        {
            // (Re)start the timer
            TInt64 time;
            iPlayer->GetMediaTime(&time);

            StartTimer(time);
        }
        break;
    }
    case CMMAPlayer::ERealized:
    case CMMAPlayer::EPrefetched:
    case CMMAPlayer::EClosed:
    {
        // Player is not running anymore
        iTimer->Cancel();
        break;
    }
    default:
    {
        // other states are ignored
    }
    }
}

//  END OF FILE
