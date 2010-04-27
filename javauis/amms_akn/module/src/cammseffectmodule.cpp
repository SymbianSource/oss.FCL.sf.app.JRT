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
* Description:  Module for holding control groups belonging to EffectModule.
*
*/


// INCLUDE FILES
#include "cammseffectmodule.h"
#include "cammsaudiovirtualizercontrolgroup.h"
#include "cammsglobalmanager.h"
#include "jdebug.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSEffectModule::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSEffectModule* CAMMSEffectModule::NewLC(
    CAMMSGlobalManager* aGlobalManager)
{
    DEBUG("AMMS:CAMMSEffectModule::NewLC +");

    CAMMSEffectModule* self = new(ELeave) CAMMSEffectModule(aGlobalManager);

    CleanupStack::PushL(self);
    self->ConstructL();

    DEBUG("AMMS:CAMMSEffectModule::NewLC -");

    return self;
}

// -----------------------------------------------------------------------------
// CAMMSEffectModule::~CAMMSEffectModule()
// Destructor.
// -----------------------------------------------------------------------------
//
CAMMSEffectModule::~CAMMSEffectModule()
{
    DEBUG("AMMS:CAMMSEffectModule::~");
}

// -----------------------------------------------------------------------------
// CAMMSEffectModule::AddPlayerNoStateCheckL
// Adds player without checking its state.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAMMSEffectModule::AddPlayerNoStateCheckL(CMMAPlayer* aPlayer)
{
    DEBUG("AMMS:CAMMSEffectModule::AddPlayerNoStateCheckL +");

    // Check that the player can be added.
    User::LeaveIfError(iGlobalManager->PlayerAddingAllowed(aPlayer));

    // Add the player to this module.
    CAMMSModule::AddPlayerNoStateCheckL(aPlayer);

    DEBUG("AMMS:CAMMSEffectModule::AddPlayerNoStateCheckL -");
}


// -----------------------------------------------------------------------------
// CAMMSSoundSource3D::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CAMMSEffectModule::ConstructL()
{
    DEBUG("AMMS:CAMMSEffectModule::ConstructL +");

    CAMMSModule::ConstructL();

    // Add required controls to the EffectModule.

    AddControlGroupAndPopL(CAMMSAudioVirtualizerControlGroup::NewLC());

    DEBUG("AMMS:CAMMSEffectModule::ConstructL -");
}

// -----------------------------------------------------------------------------
// CAMMSEffectModule::CAMMSEffectModule
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSEffectModule::CAMMSEffectModule(CAMMSGlobalManager* aGlobalManager)
        : iGlobalManager(aGlobalManager)
{
}

//  End of File
