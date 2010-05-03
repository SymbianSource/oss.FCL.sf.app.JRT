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
* Description:  Virtualizes audio channels.
*
*/



// INCLUDE FILES
#include <jdebug.h>
#include "cammsemcaudiovirtualizercontrol.h"


// CONSTANTS
const TInt KAMMSEMCDefaultStereoWideningLevel = 100;

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CAMMSEMCAudioVirtualizerControl::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSEMCAudioVirtualizerControl* CAMMSEMCAudioVirtualizerControl::NewLC(
    CMMAPlayer* aPlayer)
{
    CAMMSEMCAudioVirtualizerControl* self =
        new(ELeave) CAMMSEMCAudioVirtualizerControl(aPlayer);

    CleanupStack::PushL(self);
    self->ConstructL();

    return self;
}

// Destructor
CAMMSEMCAudioVirtualizerControl::~CAMMSEMCAudioVirtualizerControl()
{
    DEBUG("AMMS::CAMMSEMCAudioVirtualizerControl::~CAMMSEMCAudioVirtualizerControl");

    // Perform DeallocateControl, if the state change has not yet performed it.
    DeallocateControl();
    if (iAMMSEMCAudioVirtualizerEnvironmentalPresetUtility)
        delete iAMMSEMCAudioVirtualizerEnvironmentalPresetUtility;
    delete iPresetNames;

}

// -----------------------------------------------------------------------------
// CAMMSEMCAudioVirtualizerControl::SetPresetL
// Sets the effect according to the given preset.
// -----------------------------------------------------------------------------
//
void CAMMSEMCAudioVirtualizerControl::SetPresetL(const TDesC& aPreset)
{

    DEBUG_STR("AMMS::CAMMSEMCAudioVirtualizerControl::SetPresetL \"%S\"",
              aPreset);
    CStereoWidening *iStereoWidening = NULL;
    iAMMSEMCAudioVirtualizerEnvironmentalPresetUtility->PrepareEmptyStereoWideningUtilitiesL();
    iAMMSEMCAudioVirtualizerEnvironmentalPresetUtility->GetPresetNamesL(*iPresetNames);
    TInt presetPosition = 0;
    TInt findPreset = iPresetNames->Find(aPreset, presetPosition);
    if (findPreset == 0)  // Find returns zero, if a matching element is found.
    {
        iStereoWidening = iAMMSEMCAudioVirtualizerEnvironmentalPresetUtility->GetStereoWideningAtPresetIndexL(presetPosition);
        iCurrentPreset = presetPosition;
    }
    else
    {
        iAMMSEMCAudioVirtualizerEnvironmentalPresetUtility->DeleteEmptyStereoWideningUtilities();
        User::Leave(KErrArgument);
    }

    if (!iStereoWidening)
    {
        iAMMSEMCAudioVirtualizerEnvironmentalPresetUtility->DeleteEmptyStereoWideningUtilities();
        return;
    }
    //Get all preset data here
    TUint8 iStereoWideningLevel = iStereoWidening->StereoWideningLevel();
    // Set all preset data to EMC
    iMStereoWideningControl->SetStereoWideningLevel(iStereoWideningLevel);
    iMStereoWideningControl->Apply();
    iAMMSEMCAudioVirtualizerEnvironmentalPresetUtility->DeleteEmptyStereoWideningUtilities();
    DEBUG_STR("AMMS::CAMMSEMCAudioVirtualizerControl::SetPresetL \"%S\" GetPresetL OK",
              aPreset);
}

// -----------------------------------------------------------------------------
// CAMMSEMCAudioVirtualizerControl::PresetNamesL
// Gets the available preset names.
// -----------------------------------------------------------------------------
//
const CDesCArray& CAMMSEMCAudioVirtualizerControl::PresetNamesL()
{
    // Returns an array of all preset names (pre-defined and user-defined).
    // The pre-defined presets are in the beginning of the list.
    iAMMSEMCAudioVirtualizerEnvironmentalPresetUtility->PrepareEmptyStereoWideningUtilitiesL();
    iAMMSEMCAudioVirtualizerEnvironmentalPresetUtility->GetPresetNamesL(*iPresetNames);
    iAMMSEMCAudioVirtualizerEnvironmentalPresetUtility->DeleteEmptyStereoWideningUtilities();
    return *iPresetNames;
}

// -----------------------------------------------------------------------------
// CAMMSEMCAudioVirtualizerControl::PresetL
// Gets the current preset.
// -----------------------------------------------------------------------------
//
const TDesC& CAMMSEMCAudioVirtualizerControl::PresetL()
{
    //if no preset is set, return null
    if (iCurrentPreset < 0)
    {
        return KNullDesC;
    }

    else
    {
        // Retrieves a Preset with the given index from the utility class
        iAMMSEMCAudioVirtualizerEnvironmentalPresetUtility->PrepareEmptyStereoWideningUtilitiesL();
        iAMMSEMCAudioVirtualizerEnvironmentalPresetUtility->GetPresetAtIndexL(iPresetName , iCurrentPreset);
        iAMMSEMCAudioVirtualizerEnvironmentalPresetUtility->DeleteEmptyStereoWideningUtilities();
        return iPresetName;
    }
}

// -----------------------------------------------------------------------------
// CAMMSEMCAudioVirtualizerControl::SetEnabledL
// Enables/disables the effect.
// -----------------------------------------------------------------------------
//
void CAMMSEMCAudioVirtualizerControl::SetEnabledL(TBool aEnable)
{
    if (aEnable)
    {
        // Enable the effect.
        DEBUG("AMMS::CAMMSEMCAudioVirtualizerControl::SetEnabledL(true).");
        iMStereoWideningControl->Enable();
        iMStereoWideningControl->SetStereoWideningLevel(KAMMSEMCDefaultStereoWideningLevel);
        iMStereoWideningControl->Apply();
    }
    else
    {
        // Disable the effect

        DEBUG("AMMS::CAMMSEMCAudioVirtualizerControl::SetEnabledL(false).");
        iMStereoWideningControl->Disable();
    }

}

// -----------------------------------------------------------------------------
// CAMMSEMCAudioVirtualizerControl::PrepareControlL
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSEMCAudioVirtualizerControl::PrepareControlL()
{
    // Perform the action only for the first time, skip if called afterwards
    if (!iMStereoWideningControl)
    {
        DEBUG("AMMS::CAMMSEMCAudioVirtualizerControl::PrepareControlL");
        //Create AudioVirtualizer Effect Control
        iStreamControl = (static_cast<CMMAEMCAudioPlayer*>(iMMAPlayer))->StreamControl();
        iFactory = (static_cast<CMMAEMCAudioPlayer*>(iMMAPlayer))->MMFactory();

        MEffectControl* temp(NULL);
        User::LeaveIfError(iFactory->CreateEffectControl(KStereoWideningEffectControl, temp));
        iMStereoWideningControl  = static_cast<MStereoWideningControl*>(temp);
        User::LeaveIfError(iStreamControl->AddEffect(*iMStereoWideningControl));
    }
}

// -----------------------------------------------------------------------------
// CAMMSEMCAudioVirtualizerControl::DeallocateControl
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSEMCAudioVirtualizerControl::DeallocateControl()
{

    if (iMStereoWideningControl)
    {
        DEBUG("AMMS::CAMMSEMCAudioVirtualizerControl::DeallocateControl");

        // StereoWidening for Audio can be disabled or enabled
        TRAPD(err,iMStereoWideningControl->Disable());
        if (err != KErrNone)
        {
            //Some EMC Error
            DEBUG_INT("AMMS::CAMMSEMCAudioVirtualizerControl::DeallocateControl err = %d",err);
        }
        //return the control to factory
        MEffectControl* temp = iMStereoWideningControl;
        iStreamControl->RemoveEffect(*temp);
        // Delete the Effect
        MEffectControl* tempCtrl = iMStereoWideningControl;
        iFactory->DeleteEffectControl(tempCtrl);
        iMStereoWideningControl = NULL;
    }
}

const TDesC& CAMMSEMCAudioVirtualizerControl::ClassName() const
{
    return KAMMSEMCAudioVirtualizerControl;
}


// -----------------------------------------------------------------------------
// CAMMSEMCAudioVirtualizerControl::SetEnforcedL
// Enforces the effect to be in use.
// -----------------------------------------------------------------------------
//
void CAMMSEMCAudioVirtualizerControl::SetEnforcedL(TBool aEnforced)
{
    DEBUG("AMMS::CAMMSEMCAudioVirtualizerControl:SetEnforcedL");
    // Indicate the effect is to be enforced or not. ETrue = Enforced.

    iMStereoWideningControl->Enforce(aEnforced);

    ApplySettingsL();

}

// -----------------------------------------------------------------------------
// CAMMSEMCAudioVirtualizerControl::Enforced
// Returns the current enforced setting of the effect.
// -----------------------------------------------------------------------------
//
TBool CAMMSEMCAudioVirtualizerControl::Enforced()
{
    // Returns ETrue if the effect is enforced, EFalse if not enforced.
    TBool enforced;
    iMStereoWideningControl->IsEnforced(enforced);
    return enforced;

}

// -----------------------------------------------------------------------------
// CAMMSEMCAudioVirtualizerControl::SetScopeL
// Sets the scope of the effect.
// -----------------------------------------------------------------------------
//
void CAMMSEMCAudioVirtualizerControl::SetScopeL(TInt aScope)
{
    DEBUG("AMMS::CAMMSEMCAudioVirtualizerControl:SetScopeL");
    __ASSERT_DEBUG(
        (aScope == CAMMSEffectControlGroup::EScopeLiveOnly),
        User::Invariant());
    // Just to suppress warning in release build
    (void)aScope;
}

// -----------------------------------------------------------------------------
// CAMMSEMCAudioVirtualizerControl::Scope
// Returns the scope in which the effect is present.
// -----------------------------------------------------------------------------
//
TInt CAMMSEMCAudioVirtualizerControl::Scope()
{
    // For now only the (LIVE_ONLY) scope is supported.
    return CAMMSEffectControlGroup::EScopeLiveOnly;
}

// -----------------------------------------------------------------------------
// CAMMSEMCAudioVirtualizerControl::ApplySettingsL
// Apply changed settings if Effect is in enabled state.
// -----------------------------------------------------------------------------
void CAMMSEMCAudioVirtualizerControl::ApplySettingsL()
{
    DEBUG("AMMS::CAMMSEMCAudioVirtualizerControl:ApplySettingsL called, checking state");
    TBool enabled;
    iMStereoWideningControl->IsEnabled(enabled);
    if (enabled)
    {
        DEBUG("AMMS::CAMMSEMCAudioVirtualizerControl:ApplySettingsL calling ApplyL");
        iMStereoWideningControl->Apply();
    }
}


// -----------------------------------------------------------------------------
// CAMMSEMCAudioVirtualizerControl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CAMMSEMCAudioVirtualizerControl::ConstructL()
{
    // Create array for preset names
    iPresetNames = new(ELeave) CDesCArrayFlat(1);

    // Set current preset to a negative value as it is not set yet
    iCurrentPreset = -1;

    CAMMSEffectControl::ConstructL();
    iAMMSEMCAudioVirtualizerEnvironmentalPresetUtility =
        new(ELeave)CAMMSEMCAudioVirtualizerEnvironmentalPresetUtility;
    iAMMSEMCAudioVirtualizerEnvironmentalPresetUtility->ConstructL();
}

// -----------------------------------------------------------------------------
// CAMMSEMCAudioVirtualizerControl::CAMMSEMCAudioVirtualizerControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSEMCAudioVirtualizerControl::CAMMSEMCAudioVirtualizerControl(
    CMMAPlayer* aPlayer)
        : CAMMSEffectControl(aPlayer)
{
    iMMAPlayer = aPlayer;
}

//  End of File
