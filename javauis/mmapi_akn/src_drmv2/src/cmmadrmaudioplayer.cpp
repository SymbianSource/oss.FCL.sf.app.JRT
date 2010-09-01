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
* Description:  This class is used for MIDI.
*
*/


//  INCLUDE FILES
#include <jdebug.h>
#include <e32base.h>
#include <AudioPreference.h>

#include "cmmadrmaudioplayer.h"
#include "mmmadisplay.h"

const TInt KErrorMessageSize = 32;
const TInt64 KDurationUnknown = -1;
const TInt KMediaStartTime = 0;
const TInt KMediaTimeDurationTreshold = 100000;

_LIT(KErrDefaultError, "Symbian OS Error: %d");

CMMADRMAudioPlayer* CMMADRMAudioPlayer::NewLC(const TDesC& aContentType,
        TFileName aFileName)
{
    CMMADRMAudioPlayer* self = new(ELeave) CMMADRMAudioPlayer(aFileName);
    CleanupStack::PushL(self);
    self->ConstructL(aContentType);
    return self;
}

CMMADRMAudioPlayer::~CMMADRMAudioPlayer()
{
    CloseClientUtility();
    delete iUtility;

    DEBUG("MMA::CMMADRMAudioPlayer::~CMMADRMAudioPlayer ");
}

void CMMADRMAudioPlayer::ConstructL(const TDesC& aContentType)
{
    DEBUG("CMMADRMAudioPlayer::ConstructL");
    // Only file based sources are supported with DRM
    if (iFileName == KNullDesC)
    {
        User::Leave(KErrNotSupported);
    }
    iContentType = aContentType.AllocL();
    iUtility = CDrmPlayerUtility::NewL(*this, KAudioPriorityRealOnePlayer,
                                       (TMdaPriorityPreference) KAudioPrefRealOneLocalPlayback);

    CMMAPlayer::ConstructL();
}

CMMADRMAudioPlayer::CMMADRMAudioPlayer(TFileName aFileName):
        CMMAPlayer(),
        iFileName(aFileName)
{
}

CDrmPlayerUtility* CMMADRMAudioPlayer::DRMUtility() const
{
    return iUtility;
}


void CMMADRMAudioPlayer::RealizeL()
{
    DEBUG("CMMADRMAudioPlayer::RealizeL");
    CMMAPlayer::RealizeL();
}

void CMMADRMAudioPlayer::PrefetchL()
{
    DEBUG("CMMADRMAudioPlayer::PrefetchL");
    // Prefetch will be completed in init callback
    iUtility->OpenFileL(iFileName);
}

void CMMADRMAudioPlayer::DeallocateL()
{
    DEBUG("MMA: CMMAMidiPlayer: DeallocateL +");
    if (iState == EPrefetched)
    {
        CloseClientUtility();
        ChangeState(ERealized);
    }
    DEBUG("MMA: CMMAMidiPlayer: DeallocateL -");
}

void CMMADRMAudioPlayer::StartL(TBool aPostEvent)
{
    // start can't be called to not ready player
    ASSERT(iState == EPrefetched);

    TInt64 time = TInt64(KMediaStartTime);

    // Media time is not fetched using GetMediaTime
    // Because it adjusts the result if it is close
    // to duration.

    TTimeIntervalMicroSeconds position(KMediaStartTime);
    User::LeaveIfError(iUtility->GetPosition(position));

    time = position.Int64();

    TInt64 duration;
    GetDuration(&duration);
    if ((duration > 0) && (time > duration - KMediaTimeDurationTreshold))
    {
        time = KMediaStartTime;
        iUtility->SetPosition(time);
    }

    iUtility->Play();

    PostActionCompleted(KErrNone);   // java start return
    if (aPostEvent)
    {
        // inform java side
        PostLongEvent(CMMAPlayerEvent::EStarted, time);
    }
    ChangeState(EStarted);
}

void CMMADRMAudioPlayer::StopL(TBool aPostEvent)
{
    if (iState == EStarted)
    {
        User::LeaveIfError(iUtility->Pause());

        if (aPostEvent)
        {
            TInt64 time;
            GetMediaTime(&time);
            PostLongEvent(CMMAPlayerEvent::EStopped, time);
        }
        // go back to prefetched state
        ChangeState(EPrefetched);
    }
}

void CMMADRMAudioPlayer::GetDuration(TInt64* aDuration)
{
    if (iState < EPrefetched)
    {
        *aDuration = KDurationUnknown;
    }
    else
    {
        *aDuration = iUtility->Duration().Int64();
    }
}

void CMMADRMAudioPlayer::SetMediaTimeL(TInt64* aTime)
{
    DEBUG_INT("MMA::CMMADRMAudioPlayer::SetMediaTimeL + aTime: %d", *aTime);
    // Duration is needed so we do not try to set media time
    // too far away. If duration is not known, setting media time
    // is not allowed.

    TInt64 duration = TInt64(KMediaStartTime);
    GetDuration(&duration);
    if (duration < KErrNone)
    {
        // If duration is not known, we will leave with suitable code.
        User::Leave(KErrNotSupported);
    }

    // Check if desired media time is past media duration and
    // set it to duration in that case. Negative values are not
    // checked here because it's done already in Java side.

    // If media time is attempted to set to duration, then
    // it is set close to duration. This way
    // when utility is then started, end of media event
    // will come soon after. This is not indicated to Java
    // side but instead returned that the media time was
    // set to duration.

    TTimeIntervalMicroSeconds position;
    if (*aTime >= duration)
    {
        *aTime = duration;
        position = duration - KMediaTimeDurationTreshold;
    }
    else
    {
        position = *aTime;
    }

    iUtility->SetPosition(position);

    // Inform about the position change to the StateListeners
    ChangeState(iState);

    // Get the actual media time
    GetMediaTime(aTime);
}

void CMMADRMAudioPlayer::GetMediaTime(TInt64* aMediaTime)
{
    DEBUG_INT("MMA::CMMADRMAudioPlayer::GetMediaTime + aMediaTime: %d", *aMediaTime);
    TTimeIntervalMicroSeconds position;
    TInt64 duration = TInt64(KMediaStartTime);

    // Error code discarded on purpose
    GetDuration(&duration);

    TInt error(iUtility->GetPosition(position));
    if (error == KErrNone)
    {
        // set return value
        *aMediaTime = position.Int64();
        //if duration is unknown and position 0, player is realized and
        //we can not know media time
        if ((duration == KDurationUnknown) && (position == 0))
        {
            *aMediaTime = KTimeUnknown;
        }
    }
    else
    {
        // media time cannot be get
        *aMediaTime = KTimeUnknown;
    }

    // Second part of the set media time workaround.
    // If position is close to duration, then media
    // time of duration is returned instead.

    // If the duration is zero or not known
    // then this adjustment is not done.
    if ((duration > KErrNone) &&
            (*aMediaTime >= duration - KMediaTimeDurationTreshold))
    {
        *aMediaTime = duration;
    }
    DEBUG_INT("MMA::CMMADRMAudioPlayer::GetMediaTime - aMediaTime: %d", *aMediaTime);
}

void CMMADRMAudioPlayer::CloseL()
{
    CMMAPlayer::CloseL();
    CloseClientUtility();
}

const TDesC& CMMADRMAudioPlayer::Type()
{
    return KMMADRMPlayer;
}

void CMMADRMAudioPlayer::PlayCompleteL(TInt aError)
{
    DEBUG_INT("MMA: CMMADRMAudioPlayer: PlayCompleteL: Error=%d", aError);
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
            StartL(ETrue);
        }
        else
        {
            iRepeatCount = 0;
            TTimeIntervalMicroSeconds position(time);
            iUtility->SetPosition(position);
        }
    }
    else
    {
        // error has occured, setting correct number of
        // repeats for next start
        SetLoopCount(iRepeatNumberOfTimes);
    }
    DEBUG("MMA: CMMADRMAudioPlayer: PlayCompleteL -");
}

void CMMADRMAudioPlayer::MdapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
{
    DEBUG_INT("MMA::CMMADRMAudioPlayer::MdapcInitComplete: aError = %d", aError);
    if (!aError)
    {
        ChangeState(EPrefetched);
    }
    PostActionCompleted(aError);
}

void CMMADRMAudioPlayer::MdapcPlayComplete(TInt aError)
{
    if (aError)
    {
        ErrorPlaybackComplete(aError);
    }
    else
    {
        TRAPD(error, PlayCompleteL(KErrNone));
        if (error)
        {
            ErrorPlaybackComplete(error);
        }
    }
}

void CMMADRMAudioPlayer::CloseClientUtility()
{
    DEBUG("MMA: CMMADRMAudioPlayer: CloseClientUtility +");
    if (iUtility)
    {
        iUtility->Close();
    }
    DEBUG("MMA: CMMADRMAudioPlayer: CloseClientUtility -");
}

void CMMADRMAudioPlayer::ErrorPlaybackComplete(TInt aError)
{
    DEBUG_INT("MMA::CMMADRMAudioPlayer::ErrorPlaybackComplete: aError = %d", aError);
    TBuf<KErrorMessageSize> errorMessage;
    errorMessage.Format(KErrDefaultError, aError);
    PostStringEvent(CMMAPlayerEvent::EError, errorMessage);

    CloseClientUtility();
    ChangeState(ERealized);
}
//  END OF FILE
