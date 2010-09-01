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
* Description:  This class is a TempoControl.
*
*/


//  INCLUDE FILES
#include <jdebug.h>
#include <e32base.h>

#include "cmmamiditempocontrol.h"
#include "cmmamidiplayer.h"

static const TInt KMicroBeatsInMiliBeat = 1000;
static const TInt KMMAMIDIMinimumTempo = 10000;
static const TInt KMMAMIDIMaximumTempo = 300000;
static const TInt KMMAMIDIDefaultTempoInMicroBeats = 120000000;
_LIT(KMMAMidiTempoStateChangedError, "Tempo error in state change");

CMMAMIDITempoControl* CMMAMIDITempoControl::NewL(CMMAMIDIPlayer* aPlayer)
{
    CMMAMIDITempoControl* self = new(ELeave) CMMAMIDITempoControl(aPlayer);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

CMMAMIDITempoControl::CMMAMIDITempoControl(CMMAMIDIPlayer* aPlayer)
{
    iPlayer = aPlayer;
    iTempo = KMMAMIDIDefaultTempoInMicroBeats;

    DEBUG("MMA:CMMAMIDITempoControl::CMMAMIDITempoControl");
}

CMMAMIDITempoControl::~CMMAMIDITempoControl()
{
    DEBUG("MMA:CMMAMIDITempoControl::~CMMAMIDITempoControl");
}

inline void CMMAMIDITempoControl::ConstructL()
{
    DEBUG("MMA:CMMAMIDITempoControl::ConstructL");

    iPlayer->addObserverL(this);
    iPlayer->AddStateListenerL(this);
}

const TDesC& CMMAMIDITempoControl::ClassName() const
{
    DEBUG("MMA:CMMAMIDITempoControl::ClassName");
    return KMIDITempoControlName;
}


TInt CMMAMIDITempoControl::TempoL()
{
    DEBUG("MMA: CMMAMIDITempoControl: TempoL");

    return iTempo / KMicroBeatsInMiliBeat;
}

TInt CMMAMIDITempoControl::SetTempoL(TInt aTempo)
{
    DEBUG("MMA: CMMAMIDITempoControl: SetTempoL");
    TInt tempo = aTempo;
    if (tempo < KMMAMIDIMinimumTempo)
    {
        tempo = KMMAMIDIMinimumTempo;
    }
    if (tempo > KMMAMIDIMaximumTempo)
    {
        tempo = KMMAMIDIMaximumTempo;
    }

    DEBUG_INT("MMA: CMMAMIDITempoControl: SetTempoL: setting tempo %d",
              tempo);

    // Convert tempo to micro-beats.
    tempo *= KMicroBeatsInMiliBeat;

    // Set the tempo to the midi client if the player state is not REALIZED.
    // (Tempo cannot be set to the midi client if the player
    // has not been prefetched).
    if (iPlayer->State() != CMMAPlayer::ERealized)
    {
        CMidiClientUtility* midi = iPlayer->MidiClient();
        midi->SetTempoL(tempo);
        tempo = iPlayer->MidiClient()->TempoMicroBeatsPerMinuteL();
    }

    iTempo = tempo;

    DEBUG_INT("MMA: CMMAMIDITempoControl: SetTempoL: Tempo set: %d",
              tempo / KMicroBeatsInMiliBeat);

    return tempo / KMicroBeatsInMiliBeat;
}

TInt CMMAMIDITempoControl::SetRateL(TInt aRate)
{
    TInt rate = aRate;
    TInt maxRate = MaxRateL();
    TInt minRate = MinRateL();

    if (rate > maxRate)
    {
        rate = maxRate;
    }
    else if (rate < minRate)
    {
        rate = minRate;
    }
    iPlayer->MidiClient()->SetPlaybackRateL(rate);
    return RateL();         // return the actual tempo
}

TInt CMMAMIDITempoControl::RateL()
{
    return iPlayer->MidiClient()->PlaybackRateL();
}

TInt CMMAMIDITempoControl::MaxRateL()
{
    return iPlayer->MidiClient()->MaxPlaybackRateL();
}

TInt CMMAMIDITempoControl::MinRateL()
{
    return iPlayer->MidiClient()->MinPlaybackRateL();
}

void CMMAMIDITempoControl::StateChanged(TInt aState)
{
    DEBUG_INT("CMMAMIDITempoControl:: StateChanged, state = %d", aState);

    // If tempo was set when the player was in REALIZED state, set the tempo
    // now when the player has been prefetched.
    if ((aState == CMMAPlayer::EPrefetched) &&
            (iTempo != KMMAMIDIDefaultTempoInMicroBeats))
    {
        CMidiClientUtility* midi = iPlayer->MidiClient();
        TRAPD(err, midi->SetTempoL(iTempo));

        DEBUG_INT("CMMAMIDITempoControl:: StateChanged, err = %d", err);

        if (err != KErrNone)
        {
            iPlayer->PostStringEvent(CMMAPlayerEvent::EError,
                                     KMMAMidiTempoStateChangedError);
        }
    }
    else if (aState == CMMAPlayer::ERealized)
    {
        // If the player was deallocated, set the tempo to the default value.
        iTempo = KMMAMIDIDefaultTempoInMicroBeats;
    }
}
void CMMAMIDITempoControl::MmcuoStateChanged(TMidiState /*aOldState*/,
        TMidiState /*aNewState*/,
        const TTimeIntervalMicroSeconds& /*aTime*/,
        TInt /*aError*/)
{
}

void CMMAMIDITempoControl::MmcuoTempoChanged(TInt aMicroBeatsPerMinute)
{
    DEBUG("CMMAMIDITempoControl:: MmcuoTempoChanged");

    iTempo = aMicroBeatsPerMinute;
}

void CMMAMIDITempoControl::MmcuoVolumeChanged(TInt /*aChannel*/,TReal32 /*aVolumeInDecibels*/)
{
}

void CMMAMIDITempoControl::MmcuoMuteChanged(TInt /*aChannel*/,TBool /*aMuted*/)
{
}

void CMMAMIDITempoControl::MmcuoSyncUpdate(const TTimeIntervalMicroSeconds& /*aMicroSeconds*/,TInt64 /*aMicroBeats*/)
{
}

void CMMAMIDITempoControl::MmcuoMetaDataEntryFound(const TInt /*aMetaDataEntryId*/,const TTimeIntervalMicroSeconds& /*aPosition*/)
{
}

void CMMAMIDITempoControl::MmcuoMipMessageReceived(const RArray<TMipMessageEntry>& /*aMessage*/)
{
}

void CMMAMIDITempoControl::MmcuoPolyphonyChanged(TInt /*aNewPolyphony*/)
{
}

void CMMAMIDITempoControl::MmcuoInstrumentChanged(TInt /*aChannel*/,TInt /*aBankId*/,TInt /*aInstrumentId*/)
{
}
//  END OF FILE
