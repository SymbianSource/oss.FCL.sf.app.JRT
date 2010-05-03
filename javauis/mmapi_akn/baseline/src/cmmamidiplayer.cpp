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
* Description:  This class is used for MIDI.
*
*/


//  INCLUDE FILES
#include <mmf/server/mmffile.h>
#include <jdebug.h>
#include <e32base.h>
#include <AudioPreference.h>

#include "cmmamidiplayer.h"
#include "mmmadisplay.h"

const TInt KErrorMessageSize = 32;
_LIT(KErrDefaultError, "Symbian OS Error: %d");
const TInt KMidiDefaultBank = 0x3c80;
const TInt KMidiDrumBank = 0x3c7c;
const TInt KMidiDrumChannel = 9;
const TInt KMidiChannels = 16;
const TInt KMidiDefaultInstrument = 1;

CMMAMIDIPlayer* CMMAMIDIPlayer::NewLC(const TDesC& aContentType,
                                      TFileName aFileName)
{
    CMMAMIDIPlayer* self = new(ELeave) CMMAMIDIPlayer(aFileName);
    CleanupStack::PushL(self);
    self->ConstructL(aContentType);
    return self;
}

CMMAMIDIPlayer::~CMMAMIDIPlayer()
{
    CloseClientUtility();
    // new function added to CMMAPlayer delete the controls before the destruction of iMidi.
    DeleteControls();
    delete iMidi;
    delete iActiveSchedulerWait;
    iObservers.Close();

    DEBUG("MMA::CMMAMIDIPlayer::~CMMAMIDIPlayer");
}

void CMMAMIDIPlayer::ConstructL(const TDesC& aContentType)
{
    DEBUG("CMMAMIDIPlayer::ConstructL");
    iContentType = aContentType.AllocL();
    iActiveSchedulerWait = new(ELeave)CActiveSchedulerWait;
    iMidi = CMidiClientUtility::NewL(*this, KAudioPriorityRecording,
                                     KMMAMIDIPriorityPreference, ETrue);

    CMMAPlayer::ConstructL();
}

CMMAMIDIPlayer::CMMAMIDIPlayer(TFileName aFileName):
        iFileName(aFileName),
        iMediaTime(KTimeUnknown), iStartedEventTime(0)
{
}

EXPORT_C CMidiClientUtility* CMMAMIDIPlayer::MidiClient() const
{
    return iMidi;
}


void CMMAMIDIPlayer::RealizeL()
{
    DEBUG("CMMAMIDIPlayer::RealizeL");
    CMMAPlayer::RealizeL();
}

void CMMAMIDIPlayer::PrefetchL()
{
    DEBUG_INT("CMMAMIDIPlayer::PrefetchL stream count %d", iSourceStreams.Count());
    if (iFileName != KNullDesC)
    {
        iMidi->OpenFile(iFileName);
    }
    else if (iSourceStreams.Count() == 0)
    {
        // We have no data, but need to initialize the player
        // Preparing all channels
        for (TInt i = 0; i < KMidiChannels; i++)
        {
            iMidi->SetInstrumentL(i, KMidiDefaultBank,
                                  KMidiDefaultInstrument);
        }

        // Setting drums to channel 10
        iMidi->SetInstrumentL(KMidiDrumChannel, KMidiDrumBank,
                              KMidiDefaultInstrument);

        // Start it immediately in order to get MIDIControl work without
        // calling the start. This is how reference implementation works.
        iMidi->Play();
    }
    else
    {
        // Created with content, audio player will initialize controller
        // and data source.
        iSourceStreams[ 0 ]->ReadAllL();
    }
}

//
// This method is called when CMMASourceStreamReader finished reading
// initiated in PrefetchL
//
void CMMAMIDIPlayer::ReadCompletedL(TInt aStatus, const TDesC8& aData)
{
    if (aStatus < KErrNone)
    {
        PostActionCompleted(aStatus);
    }
    else
    {
        // we're not finished yet
        iMidi->OpenDes(aData);      //wait for MMidiClientUtilityObserver::MmcuoStateChanged
    }
}

void CMMAMIDIPlayer::DeallocateL()
{
    DEBUG("MMA: CMMAMidiPlayer: DeallocateL +");
    if (iState == EPrefetched)
    {
        CloseClientUtility();
        ResetSourceStreams();
        ChangeState(ERealized);
    }
    DEBUG("MMA: CMMAMidiPlayer: DeallocateL -");
}

void CMMAMIDIPlayer::StartL()
{
    iMediaTime = KTimeUnknown;

    // Player is already started if this player is constructed with
    // device://midi locator.
    TBool isDeviceMidi = (iSourceStreams.Count() == 0 &&
                          iFileName == KNullDesC);
    if (!isDeviceMidi)
    {
        iMidi->Play();
    }

    // inform java side
    PostLongEvent(CMMAPlayerEvent::EStarted, iStartedEventTime);
    ChangeState(EStarted);

    // To achieve similar functionality as reference implementation,
    // END_OF_MEDIA must be sent right after Start on device://midi.
    if (isDeviceMidi)
    {
        PostLongEvent(CMMAPlayerEvent::EEndOfMedia, iStartedEventTime);
        ChangeState(EPrefetched);
    }
    PostActionCompleted(KErrNone);   // java start return
}

void CMMAMIDIPlayer::StopL(TBool aPostEvent)
{
    DEBUG("MMA: CMMAMidiPlayer::StopL");
    if (iState == EStarted)
    {
        TInt64 time;
        GetMediaTime(&time);
        iStartedEventTime = time;

        // do the stop only if we are playing some content
        if ((iSourceStreams.Count() > 0) ||
                (iFileName != KNullDesC))
        {
            // should actually pause!!!
            iMidi->Stop(TTimeIntervalMicroSeconds(0));
        }

        if (aPostEvent)
        {
            PostLongEvent(CMMAPlayerEvent::EStopped, time);
        }
        // go back to prefetched state
        ChangeState(EPrefetched);
    }
}

void CMMAMIDIPlayer::GetDuration(TInt64* aDuration)
{
    // Special case for device://midi
    if ((iSourceStreams.Count() == 0) &&
            (iFileName == KNullDesC))
    {
        iDuration = KErrNone;
    }
    else if (iDuration == KTimeUnknown)
    {
        TRAPD(err, iDuration = iMidi->DurationMicroSecondsL().Int64());
        if (err != KErrNone)
        {
            // Duration was not available.
            iDuration = KTimeUnknown;
        }
    }

    *aDuration = iDuration;
}

void CMMAMIDIPlayer::SetMediaTimeL(TInt64* aTime)
{
    TTimeIntervalMicroSeconds position(*aTime);
    iMidi->SetPositionMicroSecondsL(position);

    // Reset cached media time, because actual set position may be
    // something else than aTime.
    iMediaTime = KTimeUnknown;

    // Inform about the position change to the StateListeners
    ChangeState(iState);

    // Get the actual media time
    GetMediaTime(aTime);

    iStartedEventTime = iMediaTime;
}

void CMMAMIDIPlayer::GetMediaTime(TInt64* aMediaTime)
{
    // Special case for device://midi
    if ((iSourceStreams.Count() == 0) &&
            (iFileName == KNullDesC))
    {
        iMediaTime = KErrNone;
    }
    else if (iMediaTime == KTimeUnknown || iState == EStarted)
    {
        TTimeIntervalMicroSeconds position(0);
        TRAPD(error, position = iMidi->PositionMicroSecondsL());

        if (error == KErrNone)
        {
            TInt64 newTime = position.Int64();

            // Sanity check for media time going backwards or beyond the
            // duration.
            // Some native controls may return zero media time for
            // a few moments just before playback will complete.
            if (newTime < iMediaTime ||
                    (iDuration > 0 && newTime > iDuration))
            {
                GetDuration(&iMediaTime);
            }
            else
            {
                // set return value
                iMediaTime = newTime;
            }
        }
        else
        {
            // media time cannot be get
            iMediaTime = KTimeUnknown;
        }
    }
    *aMediaTime = iMediaTime;
}

void CMMAMIDIPlayer::ReInitializeMidiEngineL(const TDesC8* aMidiSequence)
{
    DEBUG("MMA: CMMAMIDIPlayer: ReInitializeMidiEngineL: + ");

    CloseClientUtility();

    DEBUG("MMA: CMMAMidiPlayer: ReInitializeMidiEngineL: Opening descriptor");

    iMidi->OpenDes(*aMidiSequence);
    // Wait until asynchronous OpenDes call has completed
    if (!iActiveSchedulerWait->IsStarted())
    {
        iActiveSchedulerWait->Start();
    }
    ChangeState(EPrefetched);
    DEBUG("MMA: CMMAMIDIPlayer: ReInitializeMidiEngineL: - ");
}

void CMMAMIDIPlayer::addObserverL(MMidiClientUtilityObserver* aObserver)
{
    iObservers.AppendL(aObserver);
}

void CMMAMIDIPlayer::CloseL()
{
    CMMAPlayer::CloseL();
    CloseClientUtility();
}

const TDesC& CMMAMIDIPlayer::Type()
{
    return KMMAMIDIPlayer;
}

void CMMAMIDIPlayer::PlayCompleteL(TInt aError)
{
    DEBUG("MMA: CMMAMidiPlayer: PlayCompleteL +");
    DEBUG_INT("MMA: CMMAMidiPlayer: PlayCompleteL: Error=%d", aError);
    TInt64 duration;
    GetDuration(&duration);
    iMediaTime = duration;
    iStartedEventTime = 0;

    iMidi->Stop(TTimeIntervalMicroSeconds(0));

    // go back to prefetched state
    ChangeState(EPrefetched);   // ready to play again

    // Send 'Stopped' only when stop() is called.
    PostLongEvent(CMMAPlayerEvent::EEndOfMedia, duration);

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
    DEBUG("MMA: CMMAMidiPlayer: PlayCompleteL -");
}

void CMMAMIDIPlayer::MmcuoStateChanged(TMidiState aOldState,
                                       TMidiState aNewState,
                                       const TTimeIntervalMicroSeconds& aTime,
                                       TInt aError)
{
    TInt err = aError;

    DEBUG_INT3("MMA: CMMAMIDIPlayer: MmcuoStateChanged: Old=%d, New=%d, Error=%d",
               aOldState,
               aNewState,
               err);
    // Closing the utility or reinitialising
#ifdef RD_JAVA_TMIDISTATECHANGE
    if (iActiveSchedulerWait->IsStarted() &&
            ((aNewState == EMidiStateClosedDisengaged) ||
             (aNewState == EMidiStateOpenDisengaged) ||
             (aNewState == EMidiStateClosedEngaged)))
#else
    if (iActiveSchedulerWait->IsStarted() &&
            ((aNewState == EClosedDisengaged) ||
             (aNewState == EOpenDisengaged) ||
             (aNewState == EClosedEngaged)))
#endif
    {
        iActiveSchedulerWait->AsyncStop();
    }
    // changing from realized to prefetched state
#ifdef RD_JAVA_TMIDISTATECHANGE
    else if ((iState == ERealized) &&
             (aOldState == EMidiStateClosedDisengaged) &&
             ((aNewState == EMidiStateOpenDisengaged) ||
              (aNewState == EMidiStateClosedEngaged) ||
              (aNewState == EMidiStateClosedDisengaged)))
#else
    else if ((iState == ERealized) &&
             (aOldState == EClosed) &&
             ((aNewState == EOpen) || (aNewState == EClosedEngaged) ||
              (aNewState == EClosed)))     // EClosed is EClosedDisengaged
#endif
    {
        if (aError == KErrNone)
        {
            // prefetch succeed
            ChangeState(EPrefetched);
        }
        // else state remains realized

        // inform java
        PostActionCompleted(aError);
        err = KErrNone; // don't report the error again
    }
#ifdef RD_JAVA_TMIDISTATECHANGE
    else if ((aOldState == EMidiStateOpenPlaying) &&
             (aNewState == EMidiStateOpenEngaged) &&
             (iState == EStarted))
#else
    else if ((aOldState == EPlaying) &&
             (aNewState == EOpenEngaged) &&
             (iState == EStarted))
#endif
    {
        // If iState is not EStarted PlayCompleteL may not be called because
        // player may be already stopped.

        // playing completed
        TRAPD(playErr, PlayCompleteL(aError));
        if (playErr != KErrNone)
        {
            err = playErr;
        }
    }

    if (err != KErrNone)
    {
        TBuf<KErrorMessageSize> errorMessage;
        errorMessage.Format(KErrDefaultError, err);
        PostStringEvent(CMMAPlayerEvent::EError, errorMessage);
        if (iActiveSchedulerWait->IsStarted())
        {
            iActiveSchedulerWait->AsyncStop();
        }
    }

    // notify observers
    TInt count = iObservers.Count();
    for (TInt i = 0; i < count; i++)
    {
        iObservers[ i ]->MmcuoStateChanged(aOldState, aNewState, aTime, aError);
    }

    DEBUG_INT("MMA: CMMAMIDIPlayer: MmcuoStateChanged: midi state %d",
              iMidi->State());
}

void CMMAMIDIPlayer::MmcuoTempoChanged(TInt aMicroBeatsPerMinute)
{
    // notify observers
    TInt count = iObservers.Count();
    for (TInt i = 0; i < count; i++)
    {
        iObservers[ i ]->MmcuoTempoChanged(aMicroBeatsPerMinute);
    }
}

void CMMAMIDIPlayer::MmcuoVolumeChanged(TInt aChannel,TReal32 aVolumeInDecibels)
{
    // notify observers
    TInt count = iObservers.Count();
    for (TInt i = 0; i < count; i++)
    {
        iObservers[ i ]->MmcuoVolumeChanged(aChannel, aVolumeInDecibels);
    }
}

void CMMAMIDIPlayer::MmcuoMuteChanged(TInt aChannel,TBool aMuted)
{
    // notify observers
    TInt count = iObservers.Count();
    for (TInt i = 0; i < count; i++)
    {
        iObservers[ i ]->MmcuoMuteChanged(aChannel, aMuted);
    }
}

void CMMAMIDIPlayer::MmcuoSyncUpdate(const TTimeIntervalMicroSeconds& aMicroSeconds,TInt64 aMicroBeats)
{
    // notify observers
    TInt count = iObservers.Count();
    for (TInt i = 0; i < count; i++)
    {
        iObservers[ i ]->MmcuoSyncUpdate(aMicroSeconds, aMicroBeats);
    }
}

void CMMAMIDIPlayer::MmcuoMetaDataEntryFound(const TInt aMetaDataEntryId,const TTimeIntervalMicroSeconds& aPosition)
{
    // notify observers
    TInt count = iObservers.Count();
    for (TInt i = 0; i < count; i++)
    {
        iObservers[ i ]->MmcuoMetaDataEntryFound(aMetaDataEntryId, aPosition);
    }
}

void CMMAMIDIPlayer::MmcuoMipMessageReceived(const RArray<TMipMessageEntry>& aMessage)
{
    // notify observers
    TInt count = iObservers.Count();
    for (TInt i = 0; i < count; i++)
    {
        iObservers[ i ]->MmcuoMipMessageReceived(aMessage);
    }
}

void CMMAMIDIPlayer::MmcuoPolyphonyChanged(TInt aNewPolyphony)
{
    // notify observers
    TInt count = iObservers.Count();
    for (TInt i = 0; i < count; i++)
    {
        iObservers[ i ]->MmcuoPolyphonyChanged(aNewPolyphony);
    }
}

void CMMAMIDIPlayer::MmcuoInstrumentChanged(TInt aChannel,TInt aBankId,TInt aInstrumentId)
{
    // notify observers
    TInt count = iObservers.Count();
    for (TInt i = 0; i < count; i++)
    {
        iObservers[ i ]->MmcuoInstrumentChanged(aChannel, aBankId, aInstrumentId);
    }
}

void CMMAMIDIPlayer::CloseClientUtility()
{
    DEBUG("MMA: CMMAMidiPlayer: CloseClientUtility +");
    if (iMidi &&
            iActiveSchedulerWait &&
#ifdef RD_JAVA_TMIDISTATECHANGE
            (iMidi->State() != EMidiStateClosedDisengaged))
#else
            (iMidi->State() != EClosed))
#endif
    {
        // Have to stop midi before closing,
        // this case is for device://midi
#ifdef RD_JAVA_TMIDISTATECHANGE
        if (iMidi->State() == EMidiStateClosedEngaged)
#else
        if (iMidi->State() == EClosedEngaged)
#endif
        {
            iMidi->Stop(TTimeIntervalMicroSeconds(0));
            if (!iActiveSchedulerWait->IsStarted())
            {
                iActiveSchedulerWait->Start();
            }
        }
        else
        {
            // Calling Close and Stop or Stop and Close should
            // always lead to EClosed state.

            // From EOpenEngaged or EOpenPlaying to EClosedEngaged
            // or from EOpenDisengaged to EClosedDisengaged
            iMidi->Close();
            if (!iActiveSchedulerWait->IsStarted())
            {
                iActiveSchedulerWait->Start();
            }

            DEBUG_INT("State after Close: %d", iMidi->State());

            // If not in EClosedDisengaged yet
#ifdef RD_JAVA_TMIDISTATECHANGE
            if (iMidi->State() != EMidiStateClosedDisengaged)
#else
            if (iMidi->State() != EClosed)
#endif
            {
                // From EClosedEngaged to EClosedDisengaged
                iMidi->Stop(TTimeIntervalMicroSeconds(0));
                if (!iActiveSchedulerWait->IsStarted())
                {
                    iActiveSchedulerWait->Start();
                }
                DEBUG_INT("State after Stop: %d", iMidi->State());
            }
        }
        DEBUG_INT("MMA: CMMAMidiPlayer: CloseClientUtility: State after close: %d", iMidi->State());
    }
    DEBUG("MMA: CMMAMidiPlayer: CloseClientUtility -");
}

//  END OF FILE
