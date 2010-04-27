/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is used for playing sounds
*
*/


//  INCLUDE FILES
#include <mmf/server/mmfdes.h>
#include <mmf/server/mmffile.h>
#include <AudioPreference.h>
#include <jdebug.h>
#include "cmmaaudioplayer.h"

const TInt KPlayerPriority = KAudioPriorityRecording;
const TInt KErrorMessageSize = 32;

const TUid KSourceUid = { KMmfUidDescriptorSource };
const TUid KFileSourceUid = { KMmfUidFileSource };
_LIT(KErrDefaultError, "Symbian OS Error: %d");

const TInt KMinIntervalBeforePrime = 0;

CPlaybackCompletedCallback* CPlaybackCompletedCallback::NewL(MPlaybackCompletedCallback& aObs)
{
    CPlaybackCompletedCallback* self = new(ELeave)CPlaybackCompletedCallback(aObs);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

CPlaybackCompletedCallback::~CPlaybackCompletedCallback()
{
    Cancel();
}

void CPlaybackCompletedCallback::RunL()
{
    iObs.HandlePlaybackCompleteL();
}

TInt CPlaybackCompletedCallback::RunError(TInt aError)
{
    iObs.ErrorPlaybackComplete(aError);
    return KErrNone;
}

CPlaybackCompletedCallback::CPlaybackCompletedCallback(MPlaybackCompletedCallback& aObs)
        : CTimer(EPriorityStandard), iObs(aObs)
{
    CActiveScheduler::Add(this);
}

void CPlaybackCompletedCallback::Callback()
{
    if (!IsActive())
        After(KMinIntervalBeforePrime);
}

CMMAAudioPlayer* CMMAAudioPlayer::NewLC(
    CMMAMMFResolver* aResolver)
{
    CMMAAudioPlayer* self = new(ELeave) CMMAAudioPlayer(aResolver);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}


CMMAAudioPlayer::~CMMAAudioPlayer()
{

    delete iPlaybackCompleted;
}


CMMAAudioPlayer::CMMAAudioPlayer(
    CMMAMMFResolver* aResolver):
        CMMAMMFPlayerBase(aResolver)
{
}


void CMMAAudioPlayer::ConstructL()
{
    CMMAMMFPlayerBase::ConstructL();

    iPlaybackCompleted = CPlaybackCompletedCallback::NewL(*this);
}


EXPORT_C void CMMAAudioPlayer::PrefetchDataL(const TDesC8& aData)
{
    DEBUG_INT("MMA::CMMAAudioPlayer::PrefetchDataL aData size %d",
              aData.Size());

    // player priority settings
    TMMFPrioritySettings prioritySettings;
    prioritySettings.iPriority = KPlayerPriority;
    prioritySettings.iPref = EMdaPriorityPreferenceTimeAndQuality;
    prioritySettings.iState = EMMFStatePlaying;

    TMMFDescriptorConfig srcConfig;
    srcConfig().iDes = (TAny*)&aData; // wants pointer to TPtr8
    srcConfig().iDesThreadId = RThread().Id();

    User::LeaveIfError(DoOpen(KSourceUid,
                              srcConfig,
                              KUidMmfAudioOutput,
                              KNullDesC8,
                              prioritySettings));

    User::LeaveIfError(iController.Prime());
}

EXPORT_C void CMMAAudioPlayer::PrefetchFileL()
{
    DEBUG("MMA::CMMAAudioPlayer::Prefetching from file");

    // player priority settings
    TMMFPrioritySettings prioritySettings;
    prioritySettings.iPriority = KPlayerPriority;
    prioritySettings.iPref = EMdaPriorityPreferenceTimeAndQuality;
    prioritySettings.iState = EMMFStatePlaying;

    TMMFFileConfig srcConfig;
    srcConfig().iPath = iFileName->Des();

    User::LeaveIfError(DoOpen(KFileSourceUid,
                              srcConfig,
                              KUidMmfAudioOutput,
                              KNullDesC8,
                              prioritySettings));

    User::LeaveIfError(iController.Prime());
}

EXPORT_C void CMMAAudioPlayer::PlayCompleteL(TInt aError)
{
    TInt64 time;
    GetDuration(&time);
    iMediaTime = time;
    iStartedEventTime = 0;

    ChangeState(EPrefetched);   // ready to play again

    // Send 'Stopped' only when stop() is called.
    PostLongEvent(CMMAPlayerEvent::EEndOfMedia, time);

    if (aError == KErrNone)
    {
        iRepeatCount++;

        // priming again for allowing e.g. mediatime setting
        User::LeaveIfError(iController.Prime());

        if (iRepeatForever || iRepeatCount < iRepeatNumberOfTimes)
        {
            StartL();
        }
        else
        {
            iRepeatCount = 0;

            TTimeIntervalMicroSeconds position(0);
            User::LeaveIfError(iController.SetPosition(position));
        }
    }
    else
    {
        // error has occured, setting correct number of
        // repeats for next start
        SetLoopCount(iRepeatNumberOfTimes);
    }
}

void CMMAAudioPlayer::RealizeL()
{
    DEBUG("CMMAAudioPlayer::RealizeL");
    // DataSource must have at least 1 stream or
    // we must have file to play
    if ((iSourceStreams.Count() == 0) && !iFileName)
    {
        User::Leave(KErrNotEnoughStreams);
    }
    CMMAPlayer::RealizeL();
}


void CMMAAudioPlayer::PrefetchL()
{
    DEBUG("CMMAAudioPlayer::PrefetchL");
    __ASSERT_DEBUG((iSourceStreams.Count() > 0) || iFileName, User::Invariant());

    if (iFileName)
    {
        // prefetching the file
        PrefetchFileL();
        // we can go to prefetched state immediately
        ChangeState(EPrefetched);
        PostActionCompleted(KErrNone);
    }
    else
    {
        // Using TDes -- load the whole sound
        iSourceStreams[ 0 ]->ReadAllL();
    }
    // CMMASourceStream will notify with ReadCompleted
}

const TDesC& CMMAAudioPlayer::Type()
{
    return KMMAAudioPlayer;
}

//
// CMMASourceStreamReader finished read operation
// This is called when ReadL is completed in Prefetch()
//
void CMMAAudioPlayer::ReadCompletedL(TInt aStatus, const TDesC8& aData)
{
    DEBUG_INT("CMMAAudioPlayer::ReadCompletedL: status = %d", aStatus);
    if (aStatus < KErrNone)
    {
        PostActionCompleted(aStatus);
    }
    else
    {
        TRAPD(err, PrefetchDataL(aData));
        if (err == KErrNone)
        {
            ChangeState(EPrefetched);
        }
        PostActionCompleted(err);
    }
}


void CMMAAudioPlayer::HandleEvent(const TMMFEvent& aEvent)
{
    DEBUG("MID::CMMAAudioPlayer::HandleEvent");
    TInt err = aEvent.iErrorCode;
    if (iState == EStarted)
    {
        // normal situation; will loop in PlayCompleteL if looping is set
        if ((err == KErrEof || err == KErrUnderflow || err == KErrNone)
                && aEvent.iEventType == KMMFEventCategoryPlaybackComplete)
        {
            iPlaybackCompleted->Callback();
        }
    }

    if (err ==  KErrDied && aEvent.iEventType == KMMFEventCategoryPlaybackComplete)
    {

        // basically pausing the playback
        //1. when the phone call is received/answered , the player will be pushed to pause state and phone call is given high priority.
        //2. when the call ends the player will still be in pause state , In this case the user should resume the player.
        err = iController.Pause();

        if (iState == EStarted)
        {
            TInt64 time;
            GetMediaTime(&time);
            iStartedEventTime = time;
            DEBUG("MID::CMMAAudioPlayer::Going to ChangeState( EPrefetched );");

            PostLongEvent(CMMAPlayerEvent::EStopped, time);
            ChangeState(EPrefetched);
        }
    }

    if (err != KErrNone && err != KErrDied)
    {
        ErrorPlaybackComplete(err);
    }
}

EXPORT_C void CMMAAudioPlayer::HandlePlaybackCompleteL()
{
    PlayCompleteL(KErrNone);
}

EXPORT_C void CMMAAudioPlayer::ErrorPlaybackComplete(TInt aError)
{
    DEBUG_INT("MID::CMMAAudioPlayer::ErrorPlaybackComplete: aError = %d", aError);
    TBuf<KErrorMessageSize> errorMessage;
    errorMessage.Format(KErrDefaultError, aError);
    PostStringEvent(CMMAPlayerEvent::EError, errorMessage);

    // Preparing controller for next try
    TInt err = iController.Prime();
    if (err != KErrNone)
    {
        // Prime failed
        errorMessage.Format(KErrDefaultError, err);
        PostStringEvent(CMMAPlayerEvent::EError, errorMessage);
        // we cannot recover, going back to unrealized state
        ChangeState(EUnrealized);
        return;
    }

    // If player was in started state, then error will change state to
    // EPrefetched. In other cases the old state is retained.
    if (iState == EStarted)
    {
        ChangeState(EPrefetched);
    }
}

//  END OF FILE
