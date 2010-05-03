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
* Description:  This class implements beep playing and OTA ringingtone playing.
*
*/


#include <e32std.h>
#include <e32svr.h>

#include "com_nokia_mid_sound_Sound.h"
#include "CMIDTone.h"
#include "MMIDEventSender.h"

namespace
{
const TInt KMIDMaxJavaVolume = 255;
const TInt KMIDMinNativeVolume = 1;
const TInt KMIDStopSleepTime = 10000;
const TInt KMIDMicrosInMilli = 1000;
const TInt KMIDMinDataLength = 4;
const TInt KMIDLoopForever = 999999;
}

CMIDTone* CMIDTone::NewL(TInt aFreq, TInt64 aDuration,
                         MMIDEventSender* aEventSender)
{
    CMIDTone* self = new(ELeave) CMIDTone(aEventSender);
    CleanupStack::PushL(self);
    self->ConstructL(aFreq, aDuration);
    CleanupStack::Pop(self);
    return self;
}

CMIDTone* CMIDTone::NewL(const TDesC8& aData, MMIDEventSender* aEventSender)
{
    CMIDTone* self = new(ELeave) CMIDTone(aEventSender);
    CleanupStack::PushL(self);
    self->ConstructL(aData);
    CleanupStack::Pop(self);
    return self;
}


CMIDTone::~CMIDTone()
{
    if (iMdaAudioToneUtility)
    {
        if (iMdaAudioToneUtility->State() == EMdaAudioToneUtilityNotReady)
        {
            iMdaAudioToneUtility->CancelPrepare();
        }
        else if (iMdaAudioToneUtility->State()
                 == EMdaAudioToneUtilityPlaying)
        {
            iMdaAudioToneUtility->CancelPlay();
        }
    }
    delete iMdaAudioToneUtility;
    delete iSchedulerWait;
}

CMIDTone::CMIDTone(MMIDEventSender* aEventSender)
        : CMIDSoundImpl(aEventSender)
{
}

void CMIDTone::ConstructL(TInt aFreq, TInt64 aDuration)
{
    CMIDSoundImpl::ConstructL();
    iSchedulerWait = new(ELeave) CActiveSchedulerWait;
    iFreq = aFreq;
    iDuration = aDuration;
    iState = EInitialising;
    iMdaAudioToneUtility = CMdaAudioToneUtility::NewL(*this);
    iMdaAudioToneUtility->PrepareToPlayTone(iFreq,
                                            TTimeIntervalMicroSeconds(iDuration * KMIDMicrosInMilli));
    iSchedulerWait->Start();
}

void CMIDTone::ConstructL(const TDesC8& aData)
{
    CMIDSoundImpl::ConstructL();
    iSchedulerWait = new(ELeave) CActiveSchedulerWait;
    // first byte is the number of command parts
    iFreq = 0;
    iDuration = 0;
    iState = EInitialising;

    if (aData.Length() < KMIDMinDataLength)
    {
        iState = ENotSupported;
        User::Leave(KErrArgument);
    }

    if (aData[0x000] == 0x02 && aData[0x001] == 0x4a && aData[0x002] == 0x3a)
    {
        iState = EInitialising;
    }
    else if (aData[0x000] == 0x03 && aData[0x001] == 0x4a && aData[0x002] ==
             0x44 && aData[0x003] == 0x3a)
    {
        iState = EInitialising;
    }
    else if (aData[0] == 0x00 && aData[1] == 0x11)
    {
        iState = EInitialising;
    }
    else
    {
        iState = ENotSupported;
        User::Leave(KErrArgument);
    }
    iMdaAudioToneUtility = CMdaAudioToneUtility::NewL(*this);
    iMdaAudioToneUtility->PrepareToPlayDesSequence(aData);
    iSchedulerWait->Start();

}

TInt CMIDTone::Play(TInt aLoop)
{
    __ASSERT_DEBUG(iState == EReadyToPlay, User::Invariant());

    if (aLoop == 0)
    {
        // There is no known working method to play tone forever
        aLoop = KMIDLoopForever;
    }

    // Setting repeats to 1 causes sound played twice
    if (aLoop == 1)
    {
        iMdaAudioToneUtility->SetRepeats(0, TTimeIntervalMicroSeconds(0));
    }
    else if (aLoop > 1)
    {
        iMdaAudioToneUtility->SetRepeats(aLoop, TTimeIntervalMicroSeconds(0));
    }

    iMdaAudioToneUtility->Play();

    iEventSender->SendEvent(com_nokia_mid_sound_Sound_SOUND_PLAYING);
    iState = EPlaying;
    iPlayed = ETrue;

    return KErrNone;
}

void CMIDTone::Stop()
{
    if (iState == EPlaying)
    {
        iMdaAudioToneUtility->CancelPlay();
        // sleep to make sure the device has time to stop
        User::After(TTimeIntervalMicroSeconds32(KMIDStopSleepTime));      // CSI: 92 MdaAudioToneUtility does not send event when stopping has finished #
        iEventSender->SendEvent(com_nokia_mid_sound_Sound_SOUND_STOPPED);
        iState = EReadyToPlay;
    }
}

void CMIDTone::SetVolume(TInt aVolume)
{
    TInt maxVolume = iMdaAudioToneUtility->MaxVolume();
    TInt volume = 0;
    if (aVolume)
    {
        volume = (((maxVolume
                    - KMIDMinNativeVolume + 1)
                   * aVolume)
                  / (KMIDMaxJavaVolume + 1)) + KMIDMinNativeVolume;
    }
    iMdaAudioToneUtility->SetVolume(volume);
}

TInt CMIDTone::Volume()
{
    TInt maxVolume = iMdaAudioToneUtility->MaxVolume();
    TInt volume = (iMdaAudioToneUtility->Volume() *
                   KMIDMaxJavaVolume) / maxVolume;

    return volume;
}

void CMIDTone::MatoPrepareComplete(TInt aError)
{
    if (aError == KErrNone)
    {
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

void CMIDTone::MatoPlayComplete(TInt aError)
{
    if (KErrNone == aError)
    {
        iEventSender->SendEvent(com_nokia_mid_sound_Sound_SOUND_STOPPED);
        iState = EReadyToPlay;
    }
}

void CMIDTone::Release()
{
    iState = ENotReady;
    if (iMdaAudioToneUtility)
    {
        if (iMdaAudioToneUtility->State() == EMdaAudioToneUtilityNotReady)
        {
            iMdaAudioToneUtility->CancelPrepare();
        }
        else if (iMdaAudioToneUtility->State() == EMdaAudioToneUtilityPlaying
                 || iState == EPlaying)
        {
            iMdaAudioToneUtility->CancelPlay();
        }
    }
    delete iMdaAudioToneUtility;
    iMdaAudioToneUtility = NULL;
    iState = ENotReady;
}

//  End of File
