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
#include <cmmamidiplayer.h>
#include <cmmaaudioplayer.h>

#include <logger.h>
#include "cammsaudioplayerbuilder.h"
#include "cammscustomcommandutility.h"

//MMF
#include "cammsequalizercontrol.h"
#include "cammsreverbcontrol.h"
#include "cammsreverbsourcecontrol.h"
#include "cammsaudiovirtualizercontrol.h"
#include "cammspancontrol.h"                  // same header will be used for EMC Control too
#include "cammsdistanceattenuationcontrol.h"
#include "cammsaudiodopplercontrol.h"
#include "cammsspectatordopplercontrol.h"
#include "cammsaudiolocationcontrol.h"
#include "cammsspectatorlocationcontrol.h"
#include "cammsspectatororientationcontrol.h"
#include "cammsvolumecontrol.h"
#include "ammsconstants.h"


//EMC
#include <cmmaemcaudioplayer.h>
#include "cammsemcaudiodopplercontrol.h"
#include "cammsemcspectatordopplercontrol.h"
#include "cammsemcaudiolocationcontrol.h"
#include "cammsemcspectatorlocationcontrol.h"
#include "cammsemcspectatororientationcontrol.h"
#include "cammsemcdistanceattenuationcontrol.h"
#include "cammsemcreverbcontrol.h"
#include "cammsemcreverbsourcecontrol.h"
#include "cammsemcaudiovirtualizercontrol.h"
#include "cammsemcequalizercontrol.h"


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
              playerType.Ptr());

    if ((playerType != KMMAAudioPlayer) &&
            (playerType != KMMAMIDIPlayer) && (playerType != KMMAEMCAudioPlayer))
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

    // CAMMSPanControl is common class to create control for MMF,MIDI and EMC type of Player,however implementation is different

#ifndef __WINS__
    // PanControl is not supported in WINSCW builds.
    // This is because of limited pan support in DirectX.
    CAMMSPanControl* panControl = CAMMSPanControl::NewLC(aPlayer);
    aPlayer->AddControlL(panControl);
    CleanupStack::Pop(panControl);
#endif // __WINS__


    if (playerType != KMMAEMCAudioPlayer)
    {
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

        CAMMSDistanceAttenuationControl* distanceAttenuationControl =
            CAMMSDistanceAttenuationControl::NewLC(aPlayer);
        aPlayer->AddControlL(distanceAttenuationControl);
        CleanupStack::Pop(distanceAttenuationControl);

        CAMMSReverbControl* reverbControl =
            CAMMSReverbControl::NewLC(aPlayer);
        aPlayer->AddControlL(reverbControl);
        CleanupStack::Pop(reverbControl);

        CAMMSReverbSourceControl* reverbSourceControl =
            CAMMSReverbSourceControl::NewLC(aPlayer, reverbControl);
        aPlayer->AddControlL(reverbSourceControl);
        CleanupStack::Pop(reverbSourceControl);

        CAMMSAudioVirtualizerControl* audioVirtualizerControl =
            CAMMSAudioVirtualizerControl::NewLC(aPlayer);
        aPlayer->AddControlL(audioVirtualizerControl);
        CleanupStack::Pop(audioVirtualizerControl);

        CAMMSEqualizerControl* equalizerControl =
            CAMMSEqualizerControl::NewLC(aPlayer);
        aPlayer->AddControlL(equalizerControl);
        CleanupStack::Pop(equalizerControl);

    }
    else  //EMC Player
    {

        CAMMSEMCAudioDopplerControl* emcaudioDopplerControl =
            CAMMSEMCAudioDopplerControl::NewLC(aPlayer);
        emcaudioDopplerControl->iControlType = EAMMSSoundSource3DControl;
        aPlayer->AddControlL(emcaudioDopplerControl);
        CleanupStack::Pop(emcaudioDopplerControl);

        CAMMSEMCSpectatorDopplerControl* emcspectatorDopplerControl =
            CAMMSEMCSpectatorDopplerControl::NewLC(aPlayer);
        emcspectatorDopplerControl->iControlType = EAMMSSpectatorControl;
        aPlayer->AddControlL(emcspectatorDopplerControl);
        CleanupStack::Pop(emcspectatorDopplerControl);

        CAMMSEMCAudioLocationControl* emcaudioLocationControl =
            CAMMSEMCAudioLocationControl::NewLC(aPlayer);
        emcaudioLocationControl->iControlType = EAMMSSoundSource3DControl;
        aPlayer->AddControlL(emcaudioLocationControl);
        CleanupStack::Pop(emcaudioLocationControl);

        CAMMSEMCSpectatorLocationControl* emcspectatorLocationControl =
            CAMMSEMCSpectatorLocationControl::NewLC(aPlayer);
        emcspectatorLocationControl->iControlType = EAMMSSpectatorControl;
        aPlayer->AddControlL(emcspectatorLocationControl);
        CleanupStack::Pop(emcspectatorLocationControl);

        CAMMSEMCSpectatorOrientationControl* emcspectatorOrientationControl =
            CAMMSEMCSpectatorOrientationControl::NewLC(aPlayer);
        aPlayer->AddControlL(emcspectatorOrientationControl);
        CleanupStack::Pop(emcspectatorOrientationControl);

        CAMMSEMCDistanceAttenuationControl* emcdistanceAttenuationControl =
            CAMMSEMCDistanceAttenuationControl::NewLC(aPlayer);
        aPlayer->AddControlL(emcdistanceAttenuationControl);
        CleanupStack::Pop(emcdistanceAttenuationControl);

        CAMMSEMCReverbControl* emcreverbControl =
            CAMMSEMCReverbControl::NewLC(aPlayer);
        aPlayer->AddControlL(emcreverbControl);
        CleanupStack::Pop(emcreverbControl);

        CAMMSEMCReverbSourceControl* emcreverbSourceControl =
            CAMMSEMCReverbSourceControl::NewLC(aPlayer, emcreverbControl);
        aPlayer->AddControlL(emcreverbSourceControl);
        CleanupStack::Pop(emcreverbSourceControl);

        CAMMSEMCAudioVirtualizerControl* emcaudioVirtualizerControl =
            CAMMSEMCAudioVirtualizerControl::NewLC(aPlayer);
        aPlayer->AddControlL(emcaudioVirtualizerControl);
        CleanupStack::Pop(emcaudioVirtualizerControl);

        CAMMSEMCEqualizerControl* emcequalizerControl =
            CAMMSEMCEqualizerControl::NewLC(aPlayer);
        aPlayer->AddControlL(emcequalizerControl);
        CleanupStack::Pop(emcequalizerControl);

    }

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
