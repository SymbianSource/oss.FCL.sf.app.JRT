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
* Description:  Wrapper for CMIDClip and CMIDTone class initialisations.
*
*/


#include <e32std.h>
#include <centralrepository.h>
#include <ProfileEngineSDKCRKeys.h>

#include "CMIDSound.h"
#include "CMIDTone.h"
#include "CMIDClip.h"
#include "logger.h"
#include "fs_methodcall.h"
#include "com_nokia_mid_sound_Sound.h"


CMIDSound* CMIDSound::NewL(JNIEnv& aJni, jobject aPeer)
{
    JELOG2(EJavaUI);

    CMIDSound* self = new CMIDSound();
    if (self != NULL)
    {
        self->attachToVm(aJni, aPeer);
    }
    return self;
}

CMIDSound::~CMIDSound()
{
    delete iMIDSound;

    // CENREP
    if (iProfileListener)
    {
        iProfileListener->StopListening();
        delete iProfileListener;
    }
    delete iRepository;
}

void CMIDSound::HandleNotifyInt(TUint32 aId, TInt aNewValue)
{
    JELOG2(EJavaUI);
    if (aId == KProEngActiveWarningTones)
    {
        if (iMIDSound)
        {
            iMIDSound->SetVolume(aNewValue == 0 ? 0 : iRealVolume);
        }
    }
}

void CMIDSound::SendEvent(TInt aEventType)
{
    /**
     *Callback to Java
     */
    JELOG2(EJavaUI);

    jclass javaPeerClass = mJniEnv->FindClass("com/nokia/mid/sound/Sound");

    jmethodID soundStateChangeCallback = mJniEnv->GetMethodID(
                                             javaPeerClass /*mJavaPeerClass */, "soundStateChanged", "(I)V");


    mJniEnv->CallVoidMethod(mJavaPeerObject, soundStateChangeCallback,
                            aEventType);
}

CMIDSound::CMIDSound()
        : java::util::FunctionServer("CMIDSoundFunctionServer"),
        iMIDSound(0)
{
    JELOG2(EJavaUI);
    createServerToNewThread();
}

int CMIDSound::InitProfileListener()
{
    TRAPD(err, CallMethodL(this, &CMIDSound::InitProfileListenerL, this));
    return err;
}

void CMIDSound::InitProfileListenerL()
{
    JELOG2(EJavaUI);
    // repository for profile
    iRepository = CRepository::NewL(KCRUidProfileEngine);

    // get notifies about changes in KProEngActiveWarningTones
    iProfileListener = CCenRepNotifyHandler::NewL(*this,
                       *iRepository,
                       CCenRepNotifyHandler::EIntKey,
                       KProEngActiveWarningTones);

    // Changes will be informed to HandleNotifyInt
    iProfileListener->StartListeningL();
}

TBool CMIDSound::IsAudioEnabled(/*CMIDSound* aSelf*/)
{
    JELOG2(EJavaUI);
    TInt isProfileOn = 0;
    TInt error = iRepository->Get(KProEngActiveWarningTones, isProfileOn);
    TBool retVal = EFalse;

    // If getting profile status fails do not play any tones.
    if ((isProfileOn == 1) &&
            (error == KErrNone))
    {
        retVal = ETrue;
    }
    return retVal;
}

void CMIDSound::SetBeep(TInt aFreq, TInt64 aDuration)
{
    JELOG2(EJavaUI);
    iFreq = aFreq;
    iDur = aDuration;
}

TInt CMIDSound::Init(TInt aType, const TDesC8* aData)
{
    JELOG2(EJavaUI);
    TRAPD(err, CallMethodL(this, &CMIDSound::InitL, aType, aData, this));
    return err;
}

void CMIDSound::InitL(TInt aType, const TDesC8* aData)
{
    JELOG2(EJavaUI);

    if (iMIDSound)
    {
        if (iMIDSound->State() == CMIDSoundImpl::EInitialising)
        {
            return;
        }
        if (iMIDSound->State() == CMIDSoundImpl::EReadyToPlay ||
                iMIDSound->State() == CMIDSoundImpl::EPlaying)
        {
            iMIDSound->Release();
        }
    }

    CMIDSoundImpl* soundImpl = NULL;
    switch (aType)
    {
    case com_nokia_mid_sound_Sound_FORMAT_TONE: //Tone
    {
        soundImpl = CMIDTone::NewL(*aData, this);
        break;
    }
    case com_nokia_mid_sound_Sound_FORMAT_BEEP: //Beep
    {
        soundImpl = CMIDTone::NewL(iFreq, iDur, this);
        break;
    }
    case com_nokia_mid_sound_Sound_FORMAT_WAV:  //Wav
    {
        soundImpl = CMIDClip::NewL(aData, this);
        break;
    }
    default:
    {
        break;
    }
    }
    if (soundImpl->State() != CMIDSoundImpl::EReadyToPlay)
    {
        TInt state = soundImpl->State();
        delete soundImpl;
        User::Leave(KErrNotReady);
    }
    iRealVolume = soundImpl->Volume();

    delete iMIDSound;
    iMIDSound = soundImpl;
}

TInt CMIDSound::Play(TInt aLoop)
{
    JELOG2(EJavaUI);
    TInt err;
    CallMethodL(err, this, &CMIDSound::DoPlay, aLoop, this);
    return err;
}

TInt CMIDSound::DoPlay(TInt aLoop)
{
    JELOG2(EJavaUI);
    TInt state = iMIDSound->State();
    if (state != CMIDSoundImpl::EReadyToPlay)
    {
        return state;
    }
    if (!IsAudioEnabled())
    {
        iMIDSound->SetVolume(0);
    }
    return iMIDSound->Play(aLoop);
}

void CMIDSound::Resume()
{
    JELOG2(EJavaUI);
    CallMethod(this, &CMIDSound::DoResume, this);
}

void CMIDSound::DoResume()
{
    JELOG2(EJavaUI);
    iMIDSound->Resume();
}

void CMIDSound::Stop()
{
    JELOG2(EJavaUI);
    CallMethod(this, &CMIDSound::DoStop, this);
}

void CMIDSound::DoStop()
{
    JELOG2(EJavaUI);
    iMIDSound->Stop();
}

TInt CMIDSound::SoundVolume()
{
    JELOG2(EJavaUI);
    TInt result;
    CallMethodL(result, this, &CMIDSound::Volume, this);
    return result;
}

TInt CMIDSound::Volume()
{
    JELOG2(EJavaUI);
    return iMIDSound->Volume();
}

void CMIDSound::SetVolume(TInt aVolume)
{
    JELOG2(EJavaUI);
    CallMethod(this, &CMIDSound::DoSetVolume, aVolume, this);
}

void CMIDSound::DoSetVolume(TInt aVolume)
{
    JELOG2(EJavaUI);
    iRealVolume = aVolume;
    TInt currentState = iMIDSound->State();
    if ((currentState == CMIDSoundImpl::EReadyToPlay) ||
            (currentState == CMIDSoundImpl::EPlaying))
    {
        iMIDSound->SetVolume(IsAudioEnabled() ? aVolume : 0);
    }
}


TInt CMIDSound::PlayerState()
{
    JELOG2(EJavaUI);
    TInt result;
    CallMethodL(result, this, &CMIDSound::State, this);
    return result;
}

TInt CMIDSound::State()
{
    JELOG2(EJavaUI);
    return iMIDSound->State();
}

void CMIDSound::Release()
{
    JELOG2(EJavaUI);
    CallMethod(this, &CMIDSound::DoRelease, this);
}

void CMIDSound::DoRelease()
{
    JELOG2(EJavaUI);
    iMIDSound->Release();
}
