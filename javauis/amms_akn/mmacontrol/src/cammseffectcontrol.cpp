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
* Description:  Controls an abstract filter with various preset settings.
*
*/



// INCLUDE FILES
#include <jdebug.h>
#include "cammseffectcontrol.h"
#include "cammseffectcontrolgroup.h"

// ============================ MEMBER FUNCTIONS ===============================

// Destructor
CAMMSEffectControl::~CAMMSEffectControl()
{
    DEBUG("AMMS::CAMMSEffectControl::~CAMMSEffectControl");
}

// -----------------------------------------------------------------------------
// CAMMSEffectControl::SetEnforcedL
// Enforces the effect to be in use.
// -----------------------------------------------------------------------------
//
void CAMMSEffectControl::SetEnforcedL(TBool aEnforced)
{
    DEBUG("AMMS::CAMMSEffectControl:SetEnforcedL");
    // Indicate the effect is to be enforced or not. ETrue = Enforced.

    iAudioEffect->EnforceL(aEnforced);

    ApplySettingsL();
}

// -----------------------------------------------------------------------------
// CAMMSEffectControl::Enforced
// Returns the current enforced setting of the effect.
// -----------------------------------------------------------------------------
//
TBool CAMMSEffectControl::Enforced()
{
    // Returns ETrue if the effect is enforced, EFalse if not enforced.
    return iAudioEffect->IsEnforced();
}

// -----------------------------------------------------------------------------
// CAMMSEffectControl::SetScopeL
// Sets the scope of the effect.
// -----------------------------------------------------------------------------
//
void CAMMSEffectControl::SetScopeL(TInt aScope)
{
    DEBUG("AMMS::CAMMSEffectControl:SetScopeL");
    // Check in debug build that scope is the only supported scope (LIVE_ONLY).
    __ASSERT_DEBUG(
        (aScope == CAMMSEffectControlGroup::EScopeLiveOnly),
        User::Invariant());
    // Just to suppress warning in release build
    (void)aScope;
}

// -----------------------------------------------------------------------------
// CAMMSEffectControl::Scope
// Returns the scope in which the effect is present.
// -----------------------------------------------------------------------------
//
TInt CAMMSEffectControl::Scope()
{
    // For now only the (LIVE_ONLY) scope is supported.
    return CAMMSEffectControlGroup::EScopeLiveOnly;
}

// -----------------------------------------------------------------------------
// CAMMSEffectControl::ApplySettingsL
// Apply changed settings if Effect is in enabled state.
// -----------------------------------------------------------------------------
void CAMMSEffectControl::ApplySettingsL()
{
    DEBUG("AMMS::CAMMSEffectControl:ApplySettingsL called, checking state");
    if (iAudioEffect->IsEnabled())
    {
        DEBUG("AMMS::CAMMSEffectControl:ApplySettingsL calling ApplyL");
        iAudioEffect->ApplyL();
    }
}

// -----------------------------------------------------------------------------
// CAMMSEffectControl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CAMMSEffectControl::ConstructL()
{
}

// -----------------------------------------------------------------------------
// CAMMSEffectControl::CAMMSEffectControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSEffectControl::CAMMSEffectControl(CMMAPlayer* aPlayer):
        CAMMSControl(aPlayer)
{
}

//  End of File
