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
* Description:  Module for holding control groups belonging to SoundSource3D.
*
*/


// INCLUDE FILES
#include "cammssoundsource3d.h"
#include "cammsdopplercontrolgroup.h"
#include "cammslocationcontrolgroup.h"
#include "cammsreverbsourcecontrolgroup.h"
#include "cammsdistanceattenuationcontrolgroup.h"
#include "cammspancontrolgroup.h"
#include "cammsglobalmanager.h"
#include "jdebug.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSSoundSource3D::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSSoundSource3D* CAMMSSoundSource3D::NewLC(CAMMSModule* aSpectator,
        CAMMSGlobalManager* aGlobalManager)
{
    DEBUG("AMMS:CAMMSSoundSource3D::NewLC +");

    CAMMSSoundSource3D* self = new(ELeave) CAMMSSoundSource3D(aSpectator,
            aGlobalManager);

    CleanupStack::PushL(self);
    self->ConstructL();

    DEBUG("AMMS:CAMMSSoundSource3D::NewLC -");

    return self;
}

// -----------------------------------------------------------------------------
// CAMMSSoundSource3D::~CAMMSSoundSource3D()
// Destructor.
// -----------------------------------------------------------------------------
//
CAMMSSoundSource3D::~CAMMSSoundSource3D()
{
    DEBUG("AMMS:CAMMSSoundSource3D::~");
}

// -----------------------------------------------------------------------------
// CAMMSSoundSource3D::AddPlayerNoStateCheckL
// Adds player without checking its state.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAMMSSoundSource3D::AddPlayerNoStateCheckL(CMMAPlayer* aPlayer)
{
    DEBUG("AMMS:CAMMSSoundSource3D::AddPlayerNoStateCheckL +");

    // Check that the player can be added.
    User::LeaveIfError(iGlobalManager->PlayerAddingAllowed(aPlayer));

    // Remove the pan control from the global manager because the pan control
    // cannot be used when 3D sound is in use.
    CAMMSPanControlGroup* panControlGroup =
        (CAMMSPanControlGroup*)iGlobalManager->Find(
            KAMMSPanControlClassName);

    // PanControl is not supported in WINSCW builds.
    if (panControlGroup)
    {
        panControlGroup->PlayerRemoved(aPlayer);
    }


    // Add the player to the spectator.
    iSpectator->AddPlayerNoStateCheckL(aPlayer);

    // Add the player to this module.
    CAMMSModule::AddPlayerNoStateCheckL(aPlayer);

    DEBUG("AMMS:CAMMSSoundSource3D::AddPlayerNoStateCheckL -");
}

// -----------------------------------------------------------------------------
// CAMMSSoundSource3D::RemovePlayerNoStateCheck
// Removes player without checking its state
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CAMMSSoundSource3D::RemovePlayerNoStateCheck(CMMAPlayer* aPlayer)
{
    DEBUG("AMMS:CAMMSSoundSource3D::RemovePlayerNoStateCheck +");

    // Remove the player from the module. The method returns the index
    // of the Player or an error code.
    TInt error = CAMMSModule::RemovePlayerNoStateCheck(aPlayer);

    DEBUG_INT("AMMS:CAMMSSoundSource3D::RemovePlayerNoStateCheck err %d",
              error);

    // If the player was found in this module, remove the player also from the
    // spectator. The method returns the index of the Player or an error code.
    if (error >= KErrNone)
    {
        error = iSpectator->RemovePlayerNoStateCheck(aPlayer);

        DEBUG_INT("AMMS:CAMMSSoundSource3D::RemovePlayerNoStateCheck err2 %d",
                  error);
    }

    // If the player is deleted from the module, it is required to add the
    // pan control to the global manager.
    if (error >= KErrNone)
    {
        CAMMSPanControlGroup* panControlGroup =
            (CAMMSPanControlGroup*)iGlobalManager->Find(
                KAMMSPanControlClassName);

        // PanControl is not supported in WINSCW builds.
        if (panControlGroup)
        {
            TRAP(error, panControlGroup->PlayerAddedL(aPlayer));
        }

        DEBUG_INT("AMMS:CAMMSSoundSource3D::RemovePlayerNoStateCheck err3 %d",
                  error);
    }

    DEBUG("AMMS:CAMMSSoundSource3D::RemovePlayerNoStateCheck -");

    return error;
}


// -----------------------------------------------------------------------------
// CAMMSSoundSource3D::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CAMMSSoundSource3D::ConstructL()
{
    DEBUG("AMMS:CAMMSSoundSource3D::ConstructL +");

    CAMMSModule::ConstructL();

    // Add required controls to the SoundSource3D.

    AddControlGroupAndPopL(CAMMSDopplerControlGroup::NewLC(
                               EAMMSSoundSource3DControl));

    AddControlGroupAndPopL(CAMMSLocationControlGroup::NewLC(
                               EAMMSSoundSource3DControl));

    AddControlGroupAndPopL(CAMMSDistanceAttenuationControlGroup::NewLC());

    AddControlGroupAndPopL(CAMMSReverbSourceControlGroup::NewLC());

    DEBUG("AMMS:CAMMSSoundSource3D::ConstructL -");
}

// -----------------------------------------------------------------------------
// CAMMSSoundSource3D::CAMMSSoundSource3D
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSSoundSource3D::CAMMSSoundSource3D(CAMMSModule* aSpectator,
                                       CAMMSGlobalManager* aGlobalManager)
        : iSpectator(aSpectator),
        iGlobalManager(aGlobalManager)
{
}

//  End of File
