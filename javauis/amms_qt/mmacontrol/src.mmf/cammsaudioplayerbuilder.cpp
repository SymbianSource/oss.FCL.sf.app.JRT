/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  AMMS player builder for adding AMMS audio player controls.
*
*/


// INCLUDE FILES
#include <CMMAMIDIPlayer.h>
#include <CMMAAudioPlayer.h>
#include <logger.h>

#include "CAMMSAudioPlayerBuilder.h"
#include "CAMMSCustomCommandUtility.h"

#include "CAMMSEqualizerControl.h"
#include "CAMMSReverbControl.h"
#include "CAMMSReverbSourceControl.h"
#include "CAMMSAudioVirtualizerControl.h"
#include "CAMMSPanControl.h"
#include "CAMMSDistanceAttenuationControl.h"
#include "CAMMSAudioDopplerControl.h"
#include "CAMMSSpectatorDopplerControl.h"
#include "CAMMSAudioLocationControl.h"
#include "CAMMSSpectatorLocationControl.h"
#include "CAMMSSpectatorOrientationControl.h"
#include "CAMMSVolumeControl.h"
#include "AMMSConstants.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSAudioPlayerBuilder::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSAudioPlayerBuilder* CAMMSAudioPlayerBuilder::NewLC()
{
    CAMMSAudioPlayerBuilder* self = new(ELeave) CAMMSAudioPlayerBuilder();

    CleanupStack::PushL(self);

    return self;
}

// Destructor
CAMMSAudioPlayerBuilder::~CAMMSAudioPlayerBuilder()
{
}

// -----------------------------------------------------------------------------
// CAMMSAudioPlayerBuilder::PreparePlayerL
// Adds the AMMS audio controls to the player.
// -----------------------------------------------------------------------------
//
void CAMMSAudioPlayerBuilder::PreparePlayerL(CMMAPlayer* aPlayer)
{
    const TDesC& playerType = aPlayer->Type();

    LOG1( EJavaAMMS, EInfo, "AMMS::CAMMSAudioPlayerBuilder::PreparePlayerL type %S",
              playerType);

    if ((playerType != KMMAAudioPlayer) &&
            (playerType != KMMAMIDIPlayer))
    {
        LOG( EJavaAMMS, EInfo, "AMMS::CAMMSAudioPlayerBuilder::PreparePlayerL, not supported");
        return;
    }


    // Default type for a Control is base Control (=not a derived Control).
    // DopplerControl and LocationControl have different Effect API
    // implementations and also different corresponding AMMS Controls for
    // SoundSource3D and for Spectator, so these Control types need to be
    // set accordingly.
    //
    // CMMAPlayer::AddControlL( CMMAControl* aControl ) adds a new control.
    // Ownership of the control is transferred to CMMAPlayer.
    //

#ifndef __WINS__
    // PanControl is not supported in WINSCW builds.
    // This is because of limited pan support in DirectX.
    CAMMSPanControl* panControl = CAMMSPanControl::NewLC(aPlayer);
    aPlayer->AddControlL(panControl);
    CleanupStack::Pop(panControl);
#endif // __WINS__

    CAMMSEqualizerControl* equalizerControl =
        CAMMSEqualizerControl::NewLC(aPlayer);
    aPlayer->AddControlL(equalizerControl);
    CleanupStack::Pop(equalizerControl);

    CAMMSReverbControl* reverbControl =
        CAMMSReverbControl::NewLC(aPlayer);
    aPlayer->AddControlL(reverbControl);
    CleanupStack::Pop(reverbControl);

    CAMMSAudioVirtualizerControl* audioVirtualizerControl =
        CAMMSAudioVirtualizerControl::NewLC(aPlayer);
    aPlayer->AddControlL(audioVirtualizerControl);
    CleanupStack::Pop(audioVirtualizerControl);

    CAMMSReverbSourceControl* reverbSourceControl =
        CAMMSReverbSourceControl::NewLC(aPlayer, reverbControl);
    aPlayer->AddControlL(reverbSourceControl);
    CleanupStack::Pop(reverbSourceControl);

    CAMMSDistanceAttenuationControl* distanceAttenuationControl =
        CAMMSDistanceAttenuationControl::NewLC(aPlayer);
    aPlayer->AddControlL(distanceAttenuationControl);
    CleanupStack::Pop(distanceAttenuationControl);

    CAMMSAudioDopplerControl* audioDopplerControl =
        CAMMSAudioDopplerControl::NewLC(aPlayer);
    audioDopplerControl->iControlType = EAMMSSoundSource3DControl;
    aPlayer->AddControlL(audioDopplerControl);
    CleanupStack::Pop(audioDopplerControl);

    CAMMSSpectatorDopplerControl* spectatorDopplerControl =
        CAMMSSpectatorDopplerControl::NewLC(aPlayer);
    spectatorDopplerControl->iControlType = EAMMSSpectatorControl;
    aPlayer->AddControlL(spectatorDopplerControl);
    CleanupStack::Pop(spectatorDopplerControl);

    CAMMSAudioLocationControl* audioLocationControl =
        CAMMSAudioLocationControl::NewLC(aPlayer);
    audioLocationControl->iControlType = EAMMSSoundSource3DControl;
    aPlayer->AddControlL(audioLocationControl);
    CleanupStack::Pop(audioLocationControl);

    CAMMSSpectatorLocationControl* spectatorLocationControl =
        CAMMSSpectatorLocationControl::NewLC(aPlayer);
    spectatorLocationControl->iControlType = EAMMSSpectatorControl;
    aPlayer->AddControlL(spectatorLocationControl);
    CleanupStack::Pop(spectatorLocationControl);

    CAMMSSpectatorOrientationControl* spectatorOrientationControl =
        CAMMSSpectatorOrientationControl::NewLC(aPlayer);
    aPlayer->AddControlL(spectatorOrientationControl);
    CleanupStack::Pop(spectatorOrientationControl);

    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSAudioPlayerBuilder::PreparePlayerL add OK");
}

// -----------------------------------------------------------------------------
// CAMMSAudioPlayerBuilder::CAMMSAudioPlayerBuilder
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSAudioPlayerBuilder::CAMMSAudioPlayerBuilder()
{
}

//  End of File
