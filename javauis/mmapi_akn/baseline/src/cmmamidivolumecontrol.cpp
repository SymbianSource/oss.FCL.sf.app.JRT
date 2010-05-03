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
* Description:  This class is used for setting volume to audio player
*
*/


#include <jdebug.h>
#include "cmmamidivolumecontrol.h"
#include "cmmamidiplayer.h"

CMMAMIDIVolumeControl::CMMAMIDIVolumeControl(CMMAMIDIPlayer* aPlayer)
        : CMMAVolumeControl(aPlayer)
{
    iPlayer = aPlayer;
}

void CMMAMIDIVolumeControl::ConstructL()
{
    ConstructBaseL();
}

CMMAMIDIVolumeControl* CMMAMIDIVolumeControl::NewL(CMMAMIDIPlayer* aPlayer)
{
    CMMAMIDIVolumeControl* self = new(ELeave)CMMAMIDIVolumeControl(aPlayer);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

void CMMAMIDIVolumeControl::DoSetLevelL(TInt aLevel)
{
    CMidiClientUtility* midi = iPlayer->MidiClient();
    TInt maxVolume = midi->MaxVolumeL();
    midi->SetVolumeL(aLevel * maxVolume / KMMAVolumeMaxLevel);
}

TInt CMMAMIDIVolumeControl::DoGetLevelL()
{
    CMidiClientUtility* midi = iPlayer->MidiClient();
    // result is in range 0..100
    return (midi->VolumeL() * KMMAVolumeMaxLevel / midi->MaxVolumeL());
}


//  END OF FILE

