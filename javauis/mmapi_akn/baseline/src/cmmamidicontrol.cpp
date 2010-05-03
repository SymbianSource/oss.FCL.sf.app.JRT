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
* Description:  This class is a MIDIControl.
*
*/


//  INCLUDE FILES
#include <jdebug.h>
#include <e32math.h>

#include "cmmamidicontrol.h"
#include "cmmamidiplayer.h"

namespace
{
const TReal KMMAMIDIVolumeConversionConstant = 40;
const TReal KMMADecibelToMidiVolumeConversionPowerBase = 10;
//const TTimeIntervalMicroSeconds32 KMMAMIDIVolumeChangeTimeout = 2000000;
const TInt KMMAMIDIVolumeChangeTimeout = 2000000;
const TReal KMMAMIDIMinimumVolumeDecibels = -130;
// For channel volume change message:
const TUint8 KMIDIControlChangeEvent = 0xB0;
const TUint8 KMIDIControlMainVolume = 0x07;
}

CMMAMIDIControl* CMMAMIDIControl::NewL(CMMAMIDIPlayer* aPlayer)
{
    CMMAMIDIControl* self = new(ELeave)CMMAMIDIControl(aPlayer);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

CMMAMIDIControl::CMMAMIDIControl(CMMAMIDIPlayer* aPlayer)
{
    DEBUG("MMA:CMMAMIDIControl::CMMAMIDIControl");
    iPlayer = aPlayer;
}

CMMAMIDIControl::~CMMAMIDIControl()
{
    DEBUG("MMA:CMMAMIDIControl::~CMMAMIDIControl +");
    delete iVolumeEventWait;
    DEBUG("MMA:CMMAMIDIControl::~CMMAMIDIControl -");
}

void CMMAMIDIControl::ConstructL()
{
    DEBUG("MMA:CMMAMIDIControl::ConstructL +");
    iVolumeEventWait = CChannelVolumeEventWait::NewL();
    iPlayer->addObserverL(this);
    DEBUG("MMA:CMMAMIDIControl::ConstructL -");
}

const TDesC& CMMAMIDIControl::ClassName() const
{
    DEBUG("MMA:CMMAMIDIControl::ClassName");
    return KMIDIControlName;
}

TInt CMMAMIDIControl::ChannelVolumeL(TInt aChannel)
{
    DEBUG_INT("MMA: CMMAMIDIControl: ChannelVolumeL + aChannel=%d", aChannel);

    CMidiClientUtility* midi = iPlayer->MidiClient();

    // If engine is not processing events, current
    // volume cannot be known for sure.
    TMidiState engineState = midi->State();
#ifdef RD_JAVA_TMIDISTATECHANGE
    if ((engineState == EMidiStateClosedDisengaged) ||
            (engineState == EMidiStateOpenDisengaged))
#else
    if ((engineState == EClosedDisengaged) ||
            (engineState == EOpenDisengaged))
#endif
    {
        return KErrNotFound;
    }

    // This calculation might be slow on hardware, since
    // following equation must be calculated using TReals:
    //
    // p1 = 10^(dB/40) * p0, where:
    // p1 is target midi volume,
    // p0 is reference value (maximum midi volume, 127)
    // dB is volume value in Decibels from MidiClientUtility

    TReal32 volume = midi->ChannelVolumeL(aChannel);
    if (volume < KMMAMIDIMinimumVolumeDecibels)
    {
        volume = KMMAMIDIMinimumVolumeDecibels;
    }
    TReal exponent = volume / KMMAMIDIVolumeConversionConstant;
    TReal targetVal = 0;

    // calculate midi volume value
    User::LeaveIfError(
        Math::Pow(
            targetVal,
            KMMADecibelToMidiVolumeConversionPowerBase,
            exponent));

    // multiply by reference value
    targetVal *= KMAXVolume;

    // round value to zero decimals
    User::LeaveIfError(Math::Round(targetVal, targetVal, 0));

    // clamp to bounds
    TInt retVal = (int) targetVal;
    if (retVal < 0)
    {
        retVal  = 0;
    }
    else if (retVal > KMAXVolume)
    {
        retVal = KMAXVolume;
    }

    DEBUG_INT("MMA: CMMAMIDIControl: ChannelVolumeL - retVal=%d", retVal);
    return retVal;
}

void CMMAMIDIControl::SetChannelVolumeL(TInt aChannel, TInt aVolume)
{
    DEBUG_INT2("MMA: CMMAMIDIControl: SetChannelVolumeL + aChannel=%d aVolume=%d", aChannel, aVolume);

    CMidiClientUtility* midi = iPlayer->MidiClient();

    // Change is done with shortMidiEvent so midi volume -> decibel
    // calculation is avoided (would be needed for midi client api
    // SetChannelVolumeL method)

    TBuf8<3> volumeChangeMessage;
    // range is checked before
    TUint8 channel = (KMIDIControlChangeEvent | (TUint8)aChannel);
    volumeChangeMessage.Append(channel);
    volumeChangeMessage.Append(KMIDIControlMainVolume);
    volumeChangeMessage.Append((TUint8)aVolume);
    SendMIDIEventL(&volumeChangeMessage);

    TMidiState engineState = midi->State();

    // If engine is not processing, we do not have to
    // wait until change is done.
#ifdef RD_JAVA_TMIDISTATECHANGE
    if (!((engineState == EMidiStateClosedDisengaged) ||
            (engineState == EMidiStateOpenDisengaged)))
#else
    if (!((engineState == EClosedDisengaged) ||
            (engineState == EOpenDisengaged)))
#endif
    {
        DEBUG("MMA: CMMAMIDIControl: SetChannelVolumeL: ExecuteL ->");
        iVolumeEventWait->StartWait(aChannel);
        DEBUG("MMA: CMMAMIDIControl: SetChannelVolumeL: ExecuteL <-");
    }
    DEBUG("MMA: CMMAMIDIControl: SetChannelVolumeL -");
}

void CMMAMIDIControl::SetProgramL(TInt aChannel,
                                  TInt aBank,
                                  TInt aProgram)
{
    CMidiClientUtility* midi = iPlayer->MidiClient();

    // Program means instrument.
    midi->SetInstrumentL(aChannel, aBank, aProgram);
}

TInt CMMAMIDIControl::SendMIDIEventL(const TDesC8* aData)
{
    DEBUG("MMA: CMMAMIDIControl: SendMIDIEventL +");
    CMidiClientUtility* midi = iPlayer->MidiClient();

    // SendMessageL only processes first message in the descriptor,
    // so we need to send blocks of data as many times as needed.

    TInt dataLength = aData->Length();
    TInt dataSent = 0;
    while (dataSent < dataLength)
    {
        // Delegate event directly to the native implementation
        // which checks the validity.
        TPtrC8 nextBlock = aData->Right(dataLength - dataSent);
        dataSent += midi->SendMessageL(nextBlock);
    }
    DEBUG_INT("MMA: CMMAMIDIControl: SendMIDIEventL: sent %d bytes", dataSent);
    DEBUG("MMA: CMMAMIDIControl: SendMIDIEventL -");
    return dataSent;
}

TInt CMMAMIDIControl::ReInitializeMidiL(const TDesC8* aData)
{
    DEBUG("MMA: CMMAMIDIControl: ReInitializeMidiL + ");
    iPlayer->ReInitializeMidiEngineL(aData);
    DEBUG("MMA: CMMAMIDIControl: ReInitializeMidiL - ");
    return KErrNone;
}

void CMMAMIDIControl::MmcuoStateChanged(TMidiState /*aOldState*/,
                                        TMidiState /*aNewState*/,
                                        const TTimeIntervalMicroSeconds& /*aTime*/,
                                        TInt /*aError*/)
{
}

void CMMAMIDIControl::MmcuoTempoChanged(TInt /*aMicroBeatsPerMinute*/)
{
}

void CMMAMIDIControl::MmcuoVolumeChanged(TInt aChannel, TReal32 /*aVolumeInDecibels*/)
{
    DEBUG_INT("CMMAMIDIControl:: MmcuoVolumeChanged + aChannel=%d", aChannel);
    if (iVolumeEventWait)
    {
        iVolumeEventWait->HandleVolumeChangedEvent(aChannel);
    }
    DEBUG("CMMAMIDIControl:: MmcuoVolumeChanged -");
}

void CMMAMIDIControl::MmcuoMuteChanged(TInt /*aChannel*/,TBool /*aMuted*/)
{
}

void CMMAMIDIControl::MmcuoSyncUpdate(const TTimeIntervalMicroSeconds& /*aMicroSeconds*/,TInt64 /*aMicroBeats*/)
{
}

void CMMAMIDIControl::MmcuoMetaDataEntryFound(const TInt /*aMetaDataEntryId*/,const TTimeIntervalMicroSeconds& /*aPosition*/)
{
}

void CMMAMIDIControl::MmcuoMipMessageReceived(const RArray<TMipMessageEntry>& /*aMessage*/)
{
}

void CMMAMIDIControl::MmcuoPolyphonyChanged(TInt /*aNewPolyphony*/)
{
}

void CMMAMIDIControl::MmcuoInstrumentChanged(TInt /*aChannel*/,TInt /*aBankId*/,TInt /*aInstrumentId*/)
{
}

CMMAMIDIControl::CChannelVolumeEventWait* CMMAMIDIControl::CChannelVolumeEventWait::NewL()
{
    DEBUG("CMMAMIDIControl::CChannelVolumeEventWait::NewL");
    CChannelVolumeEventWait* self = new(ELeave) CChannelVolumeEventWait();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

void CMMAMIDIControl::CChannelVolumeEventWait::ConstructL()
{
    iWait = new(ELeave) CActiveSchedulerWait();
    iTimer = CTimeOutTimer::NewL(CActive::EPriorityStandard, *this);
}

CMMAMIDIControl::CChannelVolumeEventWait::CChannelVolumeEventWait()
{
}

CMMAMIDIControl::CChannelVolumeEventWait::~CChannelVolumeEventWait()
{
    delete iTimer;
    delete iWait;
}

void CMMAMIDIControl::CChannelVolumeEventWait::TimerExpired()
{
    DEBUG("CMMAMIDIControl::CChannelVolumeEventWait::TimerExpired +");
    if (iWait->IsStarted())
    {
        DEBUG("CMMAMIDIControl::CChannelVolumeEventWait::TimerExpired - cancelling wait");
        iWait->AsyncStop();
    }
    DEBUG("CMMAMIDIControl::CChannelVolumeEventWait::TimerExpired -");
}

void CMMAMIDIControl::CChannelVolumeEventWait::StartWait(TInt aChannel)
{
    DEBUG_INT("CMMAMIDIControl::CChannelVolumeEventWait::StartWait aChannel=%d", aChannel);
    iChannel = aChannel;

    if (!iWait->IsStarted())
    {
        iTimer->After(KMMAMIDIVolumeChangeTimeout);
        iWait->Start();
    }
}

void CMMAMIDIControl::CChannelVolumeEventWait::StopWait()
{
    DEBUG("CMMAMIDIControl::CChannelVolumeEventWait::StopWait");
    if (iWait->IsStarted())
    {
        iTimer->Cancel();
        iWait->AsyncStop();
    }
}

void CMMAMIDIControl::CChannelVolumeEventWait::HandleVolumeChangedEvent(TInt aChannel)
{
    DEBUG_INT("CMMAMIDIControl::CChannelVolumeEventWait::HandleVolumeChangedEvent aChannel=%d", aChannel);
    if (iChannel == aChannel)
    {
        StopWait();
    }
}

//  END OF FILE
