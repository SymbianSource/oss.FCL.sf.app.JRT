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
* Description:  This class provides GlobalManager functionality.
*
*/


// INCLUDE FILES
#include <logger.h>

#include "cammsglobalmanager.h"
#include "cammsmodulecontainer.h"
#include "cammssoundsource3d.h"
#include "cammseffectmodule.h"

// Audio 3d includes.
#include "cammsdopplercontrolgroup.h"
#include "cammslocationcontrolgroup.h"

// Global manager
#include "cammsreverbcontrolgroup.h"
#include "cammsequalizercontrolgroup.h"
#include "cammsvolumecontrolgroup.h"
#include "cammspancontrolgroup.h"
#include "cammscommitcontrolgroup.h"
#include "cammsorientationcontrolgroup.h"

#include "cammsplayerbuildergroup.h"
#include "cammsaudioplayerbuilder.h"
#include "cammsvolumecontrolbuilder.h"
#include "ammsconstants.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSGlobalManager::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSGlobalManager* CAMMSGlobalManager::NewLC()
{
    CAMMSGlobalManager* self = new(ELeave) CAMMSGlobalManager;

    CleanupStack::PushL(self);
    self->ConstructL();

    return self;
}

//   Destructor
CAMMSGlobalManager::~CAMMSGlobalManager()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSGlobalManager::~CAMMSGlobalManager");
    delete iSpectator;
    delete iEffectModules;
    delete iSoundSource3Ds;
    delete iPlayerBuilder;
}

// -----------------------------------------------------------------------------
// CAMMSGlobalManager::AddPlayerNotifyL
// Adds new player
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAMMSGlobalManager::AddPlayerNotifyL(CMMAPlayer* aPlayer)
{
    LOG1( EJavaAMMS, EInfo, "AMMS::CAMMSGlobalManager::AddPlayerNotifyL player type %S",
              aPlayer->Type().Ptr());

    // Add amms controls to player.
    iPlayerBuilder->PreparePlayerL(aPlayer);

    // AddPlayerNoStateCheckL is used to add players to global manager and
    // global controls because mma player may be started or prefetched before
    // amms global manager is created.

    // Add player to global manager. All global control groups will be informed.
    AddPlayerNoStateCheckL(aPlayer);
}

// -----------------------------------------------------------------------------
// CAMMSGlobalManager::RemovePlayerNotify
// Removes player
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAMMSGlobalManager::RemovePlayerNotify(CMMAPlayer* aPlayer)
{
    LOG1( EJavaAMMS, EInfo, "AMMS::CAMMSGlobalManager::RemovePlayerNotify player type %S",
              aPlayer->Type().Ptr());

    // RemovePlayer can return only KErrNotFound or KErrNone, and in this case
    // we do not need to handle KErrNotFound
    RemovePlayerNoStateCheck(aPlayer);

    // Removing player from other modules
    iEffectModules->RemovePlayer(aPlayer);
    iSoundSource3Ds->RemovePlayer(aPlayer);
}

// -----------------------------------------------------------------------------
// CAMMSGlobalManager::InitL
// Initializes GlobalManager.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAMMSGlobalManager::InitL(RPointerArray< CMMAPlayer >& aPlayers)
{
    TInt playerCount = aPlayers.Count();

    // Add all players to the global controls and to the spectator.
    for (TInt i = 0; i < playerCount; i++)
    {
        LOG1( EJavaAMMS, EInfo, "AMMS::CAMMSGlobalManager::InitL adding controls to %S",
                  aPlayers[ i ]->Type().Ptr());
        AddPlayerNotifyL(aPlayers[ i ]);
    }
}


// -----------------------------------------------------------------------------
// CAMMSGlobalManager::CreateSoundSource3DL
// Creates a new sound source 3D module.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CAMMSModule* CAMMSGlobalManager::CreateSoundSource3DL()
{
    // InitL must have been called before call to this methdod.
    __ASSERT_DEBUG(iSpectator, User::Invariant());

    CAMMSSoundSource3D* soundSource3D =
        CAMMSSoundSource3D::NewLC(iSpectator, this);

    iSoundSource3Ds->AppendL(soundSource3D);

    CleanupStack::Pop(soundSource3D);

    return soundSource3D;
}

// -----------------------------------------------------------------------------
// CAMMSGlobalManager::CreateEffectModuleL
// Creates a new effect module.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CAMMSModule* CAMMSGlobalManager::CreateEffectModuleL()
{
    CAMMSEffectModule* module = CAMMSEffectModule::NewLC(this);

    iEffectModules->AppendL(module);

    CleanupStack::Pop(module);

    return module;
}

// -----------------------------------------------------------------------------
// CAMMSGlobalManager::Spectator
// Returns the spectator.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CAMMSModule* CAMMSGlobalManager::Spectator()
{
    return iSpectator;
}

// -----------------------------------------------------------------------------
// CAMMSGlobalManager::DisposeModule
// Disposes module.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSGlobalManager::DisposeModule(CAMMSModule* aModule)
{
    // Remove the module from the module containers.
    iSoundSource3Ds->RemoveModule(aModule);
    iEffectModules->RemoveModule(aModule);

    // Delete the module itself.
    delete aModule;
}

// -----------------------------------------------------------------------------
// CAMMSGlobalManager::PlayerAddingAllowed
// Checks whether the given player can be added to a module.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TInt CAMMSGlobalManager::PlayerAddingAllowed(CMMAPlayer* aPlayer)
{
    LOG1( EJavaAMMS, EInfo, "AMMS::CAMMSGlobalManager::PlayerAddingAllowed %d +",
              (TInt)aPlayer);

    TInt result = KErrNone;

    // Check whether the player can be added according to SoundSource3Ds.
    TInt modules = iSoundSource3Ds->Count();

    for (TInt i = 0; (result == KErrNone) && (i < modules); i++)
    {
        CAMMSModule* module = iSoundSource3Ds->At(i);

        result = PlayerAddingAllowed(aPlayer, module);
    }


    // Check whether the player can be added according to EffectModules.
    modules = iEffectModules->Count();

    for (TInt i = 0; (result == KErrNone) && (i < modules); i++)
    {
        CAMMSModule* module = iEffectModules->At(i);

        result = PlayerAddingAllowed(aPlayer, module);
    }


    LOG1( EJavaAMMS, EInfo, "AMMS::CAMMSGlobalManager::PlayerAddingAllowed %d -", result);

    return result;
}

// -----------------------------------------------------------------------------
// CAMMSGlobalManager::CAMMSGlobalManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSGlobalManager::CAMMSGlobalManager()
{
}

// -----------------------------------------------------------------------------
// CAMMSGlobalManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CAMMSGlobalManager::ConstructL()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSGlobalManager::ConstructL +");

    CAMMSModule::ConstructL();

    iSoundSource3Ds = new(ELeave) CAMMSModuleContainer();
    iEffectModules = new(ELeave) CAMMSModuleContainer();
    iPlayerBuilder = CAMMSPlayerBuilderGroup::NewL();
    iPlayerBuilder->AddBuilderAndPopL(CAMMSAudioPlayerBuilder::NewLC());
    iPlayerBuilder->AddBuilderAndPopL(CAMMSVolumeControlBuilder::NewLC());

    // Add control groups to global manager
    AddControlGroupAndPopL(CAMMSReverbControlGroup::NewLC());
    AddControlGroupAndPopL(CAMMSEqualizerControlGroup::NewLC());
    AddControlGroupAndPopL(CAMMSVolumeControlGroup::NewLC(
                               KAMMSGlobalVolume));

#ifndef __WINS__
    // PanControl is not supported in WINSCW builds.
    // This is because of limited pan support in DirectX.
    AddControlGroupAndPopL(CAMMSPanControlGroup::NewLC());
#endif // __WINS__

    // Create spectator and add controls.
    iSpectator = CAMMSModule::NewL();
    iSpectator->AddControlGroupAndPopL(CAMMSLocationControlGroup::NewLC(
                                           EAMMSSpectatorControl));
    iSpectator->AddControlGroupAndPopL(CAMMSOrientationControlGroup::NewLC());
    iSpectator->AddControlGroupAndPopL(CAMMSDopplerControlGroup::NewLC(
                                           EAMMSSpectatorControl));

    // Add Commit control group to global manager, and pass required references
    AddControlGroupAndPopL(CAMMSCommitControlGroup::NewLC(
                               *iSpectator,
                               *iSoundSource3Ds));

    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSGlobalManager::ConstructL -");
}

// -----------------------------------------------------------------------------
// CAMMSGlobalManager::PlayerAddingAllowed
// Checks whether the given player can be added to a module.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TInt CAMMSGlobalManager::PlayerAddingAllowed(CMMAPlayer* aPlayer,
        CAMMSModule* aModule)
{
    LOG2( EJavaAMMS, EInfo, "AMMS::CAMMSGlobalManager::PlayerAddingAllowed %d %d +",
               (TInt)aPlayer, (TInt)aModule);

    TBool playerExists = aModule->HasPlayer(aPlayer);

    TInt result = (playerExists ? KAMMSPlayerAlreadyInModuleError : KErrNone);

// WINSCW build does not support audio mixing.
// Thus, check in WINSCW build that there is not any non-closed player
// in the module. If there is any non-closed player in the module,
// adding a new player is not allowed to ANY module.
// However, allow a player to be in CLOSED state, because it cannot be started
// anymore and cannot thus reserve any resources anymore.

#ifdef __WINS__

    if (result == KErrNone)
    {
        TInt players = aModule->PlayerCount(CMMAPlayer::EUnrealized,
                                            CMMAPlayer::EStarted);

        LOG1( EJavaAMMS, EInfo, "AMMS::CAMMSGlobalManager::PlayerAddingAllowed players=%d",
                  players);

        result = (players == 0 ? KErrNone : KAMMSMixingNotSupported);
    }

#endif  // __WINS__


    LOG1( EJavaAMMS, EInfo, "AMMS::CAMMSGlobalManager::PlayerAddingAllowed %d -", result);

    return result;
}


//  End of File
