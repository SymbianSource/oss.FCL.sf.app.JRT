/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is a tuner player
*
*/



// INCLUDE FILES
#include    "CAMMSTunerPlayer.h"
#include    "CAMMSTunerControl.h"

#include    <tuner.h>
#include <logger.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSTunerPlayer::CAMMSTunerPlayer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAMMSTunerPlayer::CAMMSTunerPlayer()
{
}

// -----------------------------------------------------------------------------
// CAMMSTunerPlayer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAMMSTunerPlayer* CAMMSTunerPlayer::NewL(CAMMSTunerControl* aTunerControl)
{
    CAMMSTunerPlayer* self = new(ELeave) CAMMSTunerPlayer;

    CleanupStack::PushL(self);
    self->ConstructL(aTunerControl);
    CleanupStack::Pop();

    return self;
}


// -----------------------------------------------------------------------------
// CAMMSTunerPlayer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAMMSTunerPlayer::ConstructL(CAMMSTunerControl* aTunerControl)
{
    LOG( EJavaAMMS, EInfo, "CAMMSTunerPlayer::ConstructL +");
    iTunerControl = aTunerControl;
    iTunerPlayerUtility = iTunerControl->TunerUtility()->TunerPlayerUtilityL(*this);
    iActiveSchedulerWait = new(ELeave) CActiveSchedulerWait();
    iIsInitialized = EFalse;

    LOG( EJavaAMMS, EInfo, "CAMMSTunerPlayer::ConstructL -");
}

// Destructor
CAMMSTunerPlayer::~CAMMSTunerPlayer()
{
    LOG( EJavaAMMS, EInfo, "CAMMSTunerPlayer::~CAMMSTunerPlayer +");
    delete iTunerPlayerUtility;
    delete iActiveSchedulerWait;
    LOG( EJavaAMMS, EInfo, "CAMMSTunerPlayer::~CAMMSTunerPlayer -");
}

// -----------------------------------------------------------------------------
// CAMMSTunerPlayer::PrefetchL()
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSTunerPlayer::PrefetchL()
{
    LOG( EJavaAMMS, EInfo, "CAMMSTunerPlayer::PrefetchL +");
    //this must be called before play, stop, mute etc...
    iTunerPlayerUtility->InitializeL(EMdaPriorityNormal,
                                     EMdaPriorityPreferenceTimeAndQuality);
    if (!iActiveSchedulerWait->IsStarted())
    {
        iActiveSchedulerWait->Start();  // CSI: 10 Active object state already checked. #
    }

    LOG( EJavaAMMS, EInfo, "CAMMSTunerPlayer::PrefetchL -");
}

// -----------------------------------------------------------------------------
// CAMMSTunerPlayer::StartL()
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSTunerPlayer::StartL()
{
    LOG( EJavaAMMS, EInfo, "CAMMSTunerPlayer::StartL +");
    if (iIsInitialized)
    {
        // inform java side
        ChangeState(EStarted);
        iTunerPlayerUtility->Play();
        // set time when started
        iStartTime = CurrentTime();

        TInt64 time;
        GetMediaTime(&time);
        PostLongEvent(CMMAPlayerEvent::EStarted, time);
    }
    else
    {
        User::Leave(KErrNotReady);
    }
    LOG( EJavaAMMS, EInfo, "CAMMSTunerPlayer::StartL -");
}

// -----------------------------------------------------------------------------
// CAMMSTunerPlayer::StopL()
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSTunerPlayer::StopL(TBool /*aPostEvent*/)
{
    LOG( EJavaAMMS, EInfo, "CAMMSTunerPlayer::StopL +");
    if (iIsInitialized)
    {
        ChangeState(EPrefetched);

        iTunerPlayerUtility->Stop();

        TInt64 time;
        GetMediaTime(&time);
        iStartTime = KErrNotFound;
        PostLongEvent(CMMAPlayerEvent::EStopped, time);
    }
    else
    {
        User::Leave(KErrNotReady);
    }

    LOG( EJavaAMMS, EInfo, "CAMMSTunerPlayer::StopL -");
}

// -----------------------------------------------------------------------------
// CAMMSTunerPlayer::DeallocateL()
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSTunerPlayer::DeallocateL()
{

}

// -----------------------------------------------------------------------------
// CAMMSTunerPlayer::GetMediaTime()
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSTunerPlayer::GetMediaTime(TInt64* aMediaTime)
{
    if (iState == EStarted)
    {
        // add play time to media time
        iMediaTime += CurrentTime() - iStartTime;
        // set new start time
        iStartTime = CurrentTime();
    }

    // set value to parameter
    (*aMediaTime) = iMediaTime;
}

// -----------------------------------------------------------------------------
// CAMMSTunerPlayer::MTapoInitializeComplete
// Initialize complete event. This event is asynchronous and is received after
//  a call to CMMTunerAudioPlayerUtility::InitializeL().
// -----------------------------------------------------------------------------
void CAMMSTunerPlayer::MTapoInitializeComplete(TInt aError)
{
    ELOG1( EJavaAMMS, "CAMMSTunerPlayer::MTapoInitializeComplete aError = %d", aError);
    if (iActiveSchedulerWait->IsStarted())
    {
        LOG( EJavaAMMS, EInfo, "CAMMSTunerControl::MToTuneComplete AsyncStop");
        iActiveSchedulerWait->AsyncStop();
    }
    if (aError == KErrNone)
    {
        iIsInitialized = ETrue;
        ChangeState(EPrefetched);
        // Inform Java side.
        PostActionCompleted(KErrNone);
    }
    else
    {
        ELOG1( EJavaAMMS, "CAMMSTunerPlayer::MTapoInitializeComplete aError = %d", aError);
        PostLongEvent(CMMAPlayerEvent::EError, aError);
        //User::Leave( aError );
    }
}

// -----------------------------------------------------------------------------
// CAMMSTunerPlayer::MTapoPlayEvent
// Passes an asychronous event to the tuner client.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSTunerPlayer::MTapoPlayEvent(
    MMMTunerAudioPlayerObserver::TEventType /*aEvent*/,
    TInt /*aError*/, TAny* /*aAdditionalInfo*/)
{
    LOG( EJavaAMMS, EInfo, "CAMMSTunerPlayer::MTapoPlayEvent +");
}

// -----------------------------------------------------------------------------
// CAMMSTunerPlayer::CurrentTime()
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TInt64 CAMMSTunerPlayer::CurrentTime()
{
    TTime time;
    time.HomeTime();
    return time.Int64();
}

//  End of File
