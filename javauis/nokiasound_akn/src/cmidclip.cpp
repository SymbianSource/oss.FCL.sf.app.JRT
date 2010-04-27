/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class implements wav playing.
*
*/


#include <e32std.h>
#include <AudioPreference.h>

#include "com_nokia_mid_sound_Sound.h"
#include "CMIDClip.h"
#include "MMIDEventSender.h"

namespace
{
const TInt KMIDMaxJavaVolume = 255;
const TInt KMIDMinNativeVolume = 1;
const TInt KMIDHeaderLength = 4;
_LIT8(KMIDHeaderWAV, "RIFF");
_LIT8(KMIDHeaderAMR, "#!AM");
const TInt KMIDClipForever = 999999;
const TInt KMIDMinDataSize = 4;
}

CMIDClip* CMIDClip::NewL(const TDesC8* aData, MMIDEventSender* aEventSender)
{
    CMIDClip* self = new(ELeave) CMIDClip(aEventSender);
    CleanupStack::PushL(self);
    self->ConstructL(aData);
    CleanupStack::Pop(self);
    return self;
}

CMIDClip::~CMIDClip()
{
    if (iMdaAudioPlayerUtility)
    {
        if (iState == EPlaying)
        {
            iMdaAudioPlayerUtility->Stop();
        }
    }
    delete iMdaAudioPlayerUtility;
    delete iClip;
    delete iSchedulerWait;
}

CMIDClip::CMIDClip(MMIDEventSender* aEventSender)
        : CMIDSoundImpl(aEventSender)
{
}

void CMIDClip::ConstructL(const TDesC8* aData)
{
    CMIDSoundImpl::ConstructL();
    iSchedulerWait = new(ELeave) CActiveSchedulerWait;

    if (aData->Length() < KMIDMinDataSize)
    {
        iState = ENotSupported;
        User::Leave(KErrArgument);
    }

    TPtrC8 header = aData->Left(KMIDHeaderLength);

    if ((header != KMIDHeaderWAV()) && (header != KMIDHeaderAMR))
    {
        iState = ENotSupported;
        User::Leave(KErrArgument);
    }

    iState = EInitialising;
    iClip = aData->AllocL();
    iMdaAudioPlayerUtility =
        CMdaAudioPlayerUtility::
        NewDesPlayerReadOnlyL(*iClip, *this,
                              KAudioPriorityRecording);
    iMdaAudioPlayerUtility->UseSharedHeap();
    iSchedulerWait->Start();
}

TInt CMIDClip::Play(TInt aLoop)
{
    __ASSERT_DEBUG(iState == EReadyToPlay, User::Invariant());

    if (aLoop == 0)
    {
        // There is no known working method to play clip forever, so we play
        // it for a long time instead.
        aLoop = KMIDClipForever;
    }

    --aLoop;

    // Setting repeats to 1 causes sound played twice
    if (aLoop > 0)
    {
        iMdaAudioPlayerUtility->SetRepeats(aLoop, TTimeIntervalMicroSeconds(0));
    }

    iEventSender->SendEvent(com_nokia_mid_sound_Sound_SOUND_PLAYING);

    iMdaAudioPlayerUtility->Play();
    iState = EPlaying;
    iPlayed = ETrue;
    return KErrNone;
}

void CMIDClip::Stop()
{
    if (iState == EPlaying)
    {
        iEventSender->SendEvent(com_nokia_mid_sound_Sound_SOUND_STOPPED);
        iMdaAudioPlayerUtility->Stop();
        iState = EReadyToPlay;
    }
}

void CMIDClip::SetVolume(TInt aVolume)
{
    iVolume = aVolume;
    TInt volume = 0;
    if (aVolume)
    {
        volume = (((iMdaAudioPlayerUtility->MaxVolume()
                    - KMIDMinNativeVolume + 1)
                   * aVolume)
                  / (KMIDMaxJavaVolume + 1)) + KMIDMinNativeVolume;
    }
    iMdaAudioPlayerUtility->SetVolume(volume);
}

TInt CMIDClip::Volume()
{
    return iVolume;
}

void CMIDClip::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
{
    if (aError == KErrNone)
    {
        //Setting iVolume to max because
        //CMdaAudioPlayerUtility doesn't have Volume()
        iVolume = KMIDMaxJavaVolume;
        iState = EReadyToPlay;
    }
    else
    {
        if (aError == KErrNotSupported)
        {
            iState = ENotSupported;
        }
        else
        {
            iState = ENotReady;
        }
    }
    iSchedulerWait->AsyncStop();
}

void CMIDClip::MapcPlayComplete(TInt /*aError*/)
{
    iEventSender->SendEvent(com_nokia_mid_sound_Sound_SOUND_STOPPED);
    iState = EReadyToPlay;
}

void CMIDClip::Release()
{
    if (iMdaAudioPlayerUtility)
    {
        if (iState == EPlaying)
        {
            iMdaAudioPlayerUtility->Stop();
        }
    }
    delete iMdaAudioPlayerUtility;
    iMdaAudioPlayerUtility = NULL;
    delete iClip;
    iClip = NULL;
    iState = ENotReady;
}

//End of File

