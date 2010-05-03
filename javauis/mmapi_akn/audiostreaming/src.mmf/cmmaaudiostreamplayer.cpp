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
* Description:  This class is used for streaming audio.
*
*/


//  INCLUDE FILES
#include <mmf/server/mmfdes.h>
#include <audiopreference.h>
#include <jdebug.h>

#include "CMMAAudioStreamPlayer.h"
#include "CMMADataSourceStream.h"
#include "CMMAEventSource.h"
#include "CMMAStreamHandler.h"

const TInt KPlayerPriority = KAudioPriorityRecording;
_LIT(KMMAStreamErrorMessage, "Internal error: %d");

// Uid must be same as in S60StreamingSourceUIDs.hrh which is not exported from
// EnhancedAudioPlayerUtility/AudioStreaming/AudioStreamingSource.
const TUid KMMAMmfS60StreamingSourceUid = { 0x10207AF3 };


CMMAAudioStreamPlayer* CMMAAudioStreamPlayer::NewLC(
    CMMAMMFResolver* aResolver)
{
    CMMAAudioStreamPlayer* self = new(ELeave) CMMAAudioStreamPlayer(aResolver);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

CMMAAudioStreamPlayer::~CMMAAudioStreamPlayer()
{
    delete iStreamHandler;
}

CMMAAudioStreamPlayer::CMMAAudioStreamPlayer(
    CMMAMMFResolver* aResolver):
        CMMAAudioPlayer(aResolver)
{
}

void CMMAAudioStreamPlayer::ConstructL()
{
    CMMAAudioPlayer::ConstructL();
    iStreamHandler = CMMAStreamHandler::NewL(*this, iController);
}

TInt CMMAAudioStreamPlayer::DoOpen(TUid aSourceUid,
                                   const TDesC8& aSourceData,
                                   TUid aSinkUid,
                                   const TDesC8& aSinkData,
                                   TMMFPrioritySettings aPrioritySettings)
{
    // Make sure any existing controller is closed.
    iEventMonitor->Cancel();
    iController.Close();

    // Try opening and configuring each controller in turn
    TInt error = KErrNotSupported;
    TInt index = 0;

    // Try controllers until found a good controller or out of list
    while ((error != KErrNone) &&
            (index < iControllerInfos->Count()))
    {
        // Open the controller
        error = iController.Open((*iControllerInfos)[ index ]->Uid(),
                                 aPrioritySettings);

        // If the controller was opened without error, start receiving events from it.
        if (!error)
        {
            iEventMonitor->Start();

            // Add the data source to the controller.
            error = iController.AddDataSource(aSourceUid,
                                              aSourceData,
                                              iStreamHandler->MessageDestination());
        }

        // Add the data sink
        if (!error)
        {
            error = iController.AddDataSink(aSinkUid, aSinkData);
        }

        // If an error occurred in any of the above, close the controller.
        if (error)
        {
            iEventMonitor->Cancel();
            iController.Close();
        }

        index++;
    }

    return error;
}

CMMASourceStream* CMMAAudioStreamPlayer::AddSourceStreamL(JNIEnv* aJNIEnv,
        CMMAEventSource* aEventSource,
        jobject aReader)
{
    CMMADataSourceStream* sourceStream = CMMADataSourceStream::NewLC(aJNIEnv,
                                         aEventSource,
                                         aReader,
                                         this);
    User::LeaveIfError(iSourceStreams.Append(sourceStream));
    CleanupStack::Pop(sourceStream);
    iStreamHandler->SetSourceStream(sourceStream);
    return sourceStream;
}

void CMMAAudioStreamPlayer::PrefetchL()
{
    __ASSERT_DEBUG(iSourceStreams.Count() > 0, User::Invariant());

    // player priority settings
    TMMFPrioritySettings prioritySettings;
    prioritySettings.iPriority = KPlayerPriority;
    prioritySettings.iPref = EMdaPriorityPreferenceTimeAndQuality;
    prioritySettings.iState = EMMFStatePlaying;

    User::LeaveIfError(DoOpen(KMMAMmfS60StreamingSourceUid,
                              KNullDesC8,
                              KUidMmfAudioOutput,
                              KNullDesC8,
                              prioritySettings));
    iStreamHandler->PrepareL();
}

void CMMAAudioStreamPlayer::StartL()
{
    // If the player is in Prefetched state then it is implied that it has read "KMMAStreamRequestBufferSize"
    // and it can be played
    if (iState == EPrefetched)
    {

        TInt64 time;
        GetMediaTime(&time);

        // inform java side
        PostLongEvent(CMMAPlayerEvent::EStarted, time);

        // go to started state
        ChangeState(EStarted);

        PostActionCompleted(KErrNone);   // java start return
    }
    iStreamHandler->StartL();
}

void CMMAAudioStreamPlayer::StopL(TBool aPostEvent)
{
    DEBUG_INT("CMMAAudioStreamPlayer::Stop state %d", iState);
    if (iState == EStarted)
    {
        User::LeaveIfError(Pause());

        // go back to prefetched state
        ChangeState(EPrefetched);
        if (aPostEvent)
        {
            TInt64 time;
            GetMediaTime(&time);
            PostLongEvent(CMMAPlayerEvent::EStopped, time);
        }

    }
    DEBUG("CMMAAudioStreamPlayer::Stop OK");
}

TInt CMMAAudioStreamPlayer::Pause()
{
    iStreamHandler->Pause();
    return iController.Pause();
}

void CMMAAudioStreamPlayer::PlayCompleteL(TInt aError)
{
    DEBUG_INT("MMA::CMMAAudioStreamPlayer::PlayCompleteL error %d",
              aError);

    // Before controller is started it must be primed
    iControllerPrimed = EFalse;

    TInt64 time;
    GetDuration(&time);

    // Send 'Stopped' only when stop() is called.
    PostLongEvent(CMMAPlayerEvent::EEndOfMedia, time);

    ChangeState(EPrefetched);   // ready to play again

    if (aError == KErrNone)
    {
        iRepeatCount++;

        if (iRepeatForever || iRepeatCount < iRepeatNumberOfTimes)
        {
            StartL();
        }
        else
        {
            iRepeatCount = 0;
        }
    }
    else
    {
        // error has occured, setting correct number of
        // repeats for next start
        SetLoopCount(iRepeatNumberOfTimes);
    }
}

void CMMAAudioStreamPlayer::GetDuration(TInt64* aDuration)
{
    CMMAPlayer::GetDuration(aDuration);
}

void CMMAAudioStreamPlayer::PrepareComplete(TInt aError)
{
    DEBUG_INT("MMA::CMMAAudioStreamPlayer::PrepareComplete error %d",
              aError);
    if (aError == KErrNone)
    {
        ChangeState(EPrefetched);
    }
    PostActionCompleted(aError);   // java prefetch return
}

void CMMAAudioStreamPlayer::StartComplete(TInt aError)
{
    DEBUG_INT("MMA::CMMAAudioStreamPlayer::StartComplete error %d",
              aError);

    // do not start if player is deallocated or closed
    // RateControl start can start controller in started state
    if ((iState != EStarted) &&
            (iState != EPrefetched))
    {
        return;
    }

    TInt err = aError;
    if (!iControllerPrimed)
    {
        // Prime must be called when player is started first time or restarted
        DEBUG("MMA::CMMAAudioStreamPlayer::StartComplete PRIME");
        err = iController.Prime();
        iControllerPrimed = ETrue;
        DEBUG_INT("MMA::CMMAAudioStreamPlayer::StartComplete prime error %d",
                  err);
    }
    else
    {
        err = KErrNone;
    }

    TInt64 time;
    if (err == KErrNone)
    {
        // must be primed before media time can be get
        GetMediaTime(&time);
        err = iController.Play();
        DEBUG_INT("MMA::CMMAAudioStreamPlayer::StartComplete play error %d",
                  err);
    }

    // RateControl can start controller in started state, then Java event is
    // not sent
    if (err == KErrNone && iState != EStarted)
    { // move to started state and post started event
        PostLongEvent(CMMAPlayerEvent::EStarted, time);
        ChangeState(EStarted);
    }
    else
    { // post error event
        HandleError(aError);
    }
}

void CMMAAudioStreamPlayer::HandleError(TInt aError)
{
    DEBUG_INT("MMA::CMMAAudioStreamPlayer::HandleError error %d",
              aError);

    TName errorMessage;
    errorMessage.Format(KMMAStreamErrorMessage, aError);
    PostStringEvent(CMMAPlayerEvent::EError, errorMessage);
}

//  END OF FILE
