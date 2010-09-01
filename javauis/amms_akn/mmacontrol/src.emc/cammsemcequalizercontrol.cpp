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
* Description:  Manipulates the equalization settings of a Player.
*
*/


// INCLUDE FILES
#include <jdebug.h>
#include "cammsemcequalizercontrol.h"




// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSEMCEqualizerControl::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSEMCEqualizerControl* CAMMSEMCEqualizerControl::NewLC(CMMAPlayer* aPlayer)
{
    CAMMSEMCEqualizerControl* self =
        new(ELeave) CAMMSEMCEqualizerControl(aPlayer);

    CleanupStack::PushL(self);
    self->ConstructL();

    return self;
}

// Destructor
CAMMSEMCEqualizerControl::~CAMMSEMCEqualizerControl()
{
    DEBUG("AMMS::CAMMSEMCEqualizerControl::~CAMMSEMCEqualizerControl");

    // Perform DeallocateControl, if the state change has not yet performed it.
    DeallocateControl();
    if (iAMMSEMCEqualizerEnvironmentalPresetUtility)
        delete iAMMSEMCEqualizerEnvironmentalPresetUtility;
    delete iPresetNames;
}

// -----------------------------------------------------------------------------
// CAMMSEMCEqualizerControl::BandLevelL
// Gets the gain set for the given equalizer band.
// -----------------------------------------------------------------------------
//
TInt CAMMSEMCEqualizerControl::BandLevelL(TInt aBand)
{
    // if aBand is out of range the method must leave with KErrArgument.

    if (aBand < 0 || aBand > (NumberOfBands() - 1))
    {
        User::Leave(KErrArgument);
    }
    // Returns the band level in mB for the specified band
    TInt bandlevel;
    TInt aNativeBand = aBand + KAMMSBandOffset;
    iMEqualizerControl->BandLevel(aNativeBand,bandlevel);
    return bandlevel;

}

// -----------------------------------------------------------------------------
// CAMMSEMCEqualizerControl::MaxBandLevel
// Returns the maximum band level supported.
// -----------------------------------------------------------------------------
//
TInt CAMMSEMCEqualizerControl::MaxBandLevel()
{
    // Get the dB range in mB for the equalizer.
    TInt min,max;
    iMEqualizerControl->DbLevelLimits(min,max);
    return max;
}

// -----------------------------------------------------------------------------
// CAMMSEMCEqualizerControl::MinBandLevel
// Returns the minimum band level supported.
// -----------------------------------------------------------------------------
//
TInt CAMMSEMCEqualizerControl::MinBandLevel()
{
    // Get the dB range in mB for the equalizer.
    TInt min,max;
    iMEqualizerControl->DbLevelLimits(min,max);
    return min;
}


// -----------------------------------------------------------------------------
// CAMMSEMCEqualizerControl::BandWidth
// Returns the band width in Hz for the specified band.
// -----------------------------------------------------------------------------
//
TInt CAMMSEMCEqualizerControl::BandWidth(TInt aBand)
{
    TInt bndwdth;
    TInt aNativeBand = aBand + KAMMSBandOffset;
    iMEqualizerControl->BandWidth(aNativeBand,bndwdth);
    return bndwdth;
}

// -----------------------------------------------------------------------------
// CAMMSEMCEqualizerControl::CenterFrequency
// Returns the center frequency in Hz for a given band.
// -----------------------------------------------------------------------------
//
TInt CAMMSEMCEqualizerControl::CenterFrequency(TInt aBand)
{
    TInt cntrfreq;
    TInt aNativeBand = aBand + KAMMSBandOffset;
    iMEqualizerControl->CenterFrequency(aNativeBand,cntrfreq);
    return cntrfreq;
}

// -----------------------------------------------------------------------------
// CAMMSEMCEqualizerControl::CrossoverFrequency
// Returns the cross-over frequency between the given frequency.
// -----------------------------------------------------------------------------
//
TInt CAMMSEMCEqualizerControl::CrossoverFrequency(TInt aBand)
{
    TInt aCrossOverFreq;
    TInt aNativeBand = aBand + KAMMSBandOffset;
    iMEqualizerControl->CrossoverFrequency(aNativeBand, aCrossOverFreq);
    return aCrossOverFreq;
}

// -----------------------------------------------------------------------------
// CAMMSEMCEqualizerControl::NumberOfBands
// Gets the number of frequency bands that the equalizer supports.
// -----------------------------------------------------------------------------
//
TInt CAMMSEMCEqualizerControl::NumberOfBands()
{
    // Returns the number of equalizer bands.
    // AudioEqualizerBase: TUint8 NumberOfBands() const;

    TInt aNumOfBands;
    iMEqualizerControl->NumberOfBands(aNumOfBands);
    return aNumOfBands;

}

// -----------------------------------------------------------------------------
// CAMMSEMCEqualizerControl::SetBandLevelL
// Sets the given equalizer band to the given gain value.
// -----------------------------------------------------------------------------
//
void CAMMSEMCEqualizerControl::SetBandLevelL(
    TInt aLevel,
    TInt aBand)
{
    DEBUG_INT2("AMMS::CAMMSEMCEqualizerControl::SetBandLevelL: level=%d, band=%d",
               aLevel, aBand);
    // If aBand or aLevel is out of valid range the method must leave
    // with KErrArgument.
    if (aBand < 0 ||
            aBand > (NumberOfBands() - 1) ||
            aLevel < MinBandLevel() ||
            aLevel > MaxBandLevel())
    {
        User::Leave(KErrArgument);
    }
    TInt aNativeBand = aBand + KAMMSBandOffset;
    // Sets the equalizer band level value in mB, ranging from Min to Max
    iMEqualizerControl->SetBandLevel(aNativeBand, aLevel);

    ApplySettingsL();
}

// -----------------------------------------------------------------------------
// CAMMSEMCEqualizerControl::SetPresetL
// Sets the effect according to the given preset.
// -----------------------------------------------------------------------------
//
void CAMMSEMCEqualizerControl::SetPresetL(const TDesC& aPreset)
{
    DEBUG_STR("AMMS::CAMMSEMCEqualizerControl::SetPresetL \"%S\"", aPreset);
    CAudioEqualizer *iAudioEqualizer = NULL;
    iAMMSEMCEqualizerEnvironmentalPresetUtility->PrepareEmptyEqualizerUtilitiesL();
    iAMMSEMCEqualizerEnvironmentalPresetUtility->GetPresetNamesL(*iPresetNames);
    TInt presetPosition = 0;
    TInt findPreset = iPresetNames->Find(aPreset, presetPosition);
    if (findPreset == 0)  // Find returns zero, if a matching element is found.
    {
        iAudioEqualizer = iAMMSEMCEqualizerEnvironmentalPresetUtility->GetEqualizerAtPresetIndexL(presetPosition);
        iCurrentPreset = presetPosition;
    }
    else
    {
        iAMMSEMCEqualizerEnvironmentalPresetUtility->DeleteEmptyEqualizerUtilities();
        User::Leave(KErrArgument);
    }

    if (!iAudioEqualizer)
    {
        iAMMSEMCEqualizerEnvironmentalPresetUtility->DeleteEmptyEqualizerUtilities();
        return;
    }
    //Get all preset data here
    // For each band of AudioEqualizer get the band level and set it to iMEqualizerControl

    TInt noOfbands = iAudioEqualizer->NumberOfBands();

    for (TInt i = 0; i < noOfbands; i++)
    {
        // Band 0 in JSR-234 equals Band 1 in Effect API
        TInt aNativeBand =  i + KAMMSBandOffset;
        TInt iBandLevel = iAudioEqualizer->BandLevel(aNativeBand);
        iMEqualizerControl->SetBandLevel(aNativeBand, iBandLevel);
    }

    iMEqualizerControl->Apply();
    iAMMSEMCEqualizerEnvironmentalPresetUtility->DeleteEmptyEqualizerUtilities();
}

// -----------------------------------------------------------------------------
// CAMMSEMCEqualizerControl::PresetNamesL
// Gets the available preset names.
// -----------------------------------------------------------------------------
//
const CDesCArray& CAMMSEMCEqualizerControl::PresetNamesL()
{
    // Returns an array of all preset names (pre-defined and user-defined).
    // The pre-defined presets are in the beginning of the list.
    iAMMSEMCEqualizerEnvironmentalPresetUtility->PrepareEmptyEqualizerUtilitiesL();
    iAMMSEMCEqualizerEnvironmentalPresetUtility->GetPresetNamesL(*iPresetNames);
    iAMMSEMCEqualizerEnvironmentalPresetUtility->DeleteEmptyEqualizerUtilities();
    return *iPresetNames;
}

// -----------------------------------------------------------------------------
// CAMMSEMCEqualizerControl::PresetL
// Gets the current preset.
// -----------------------------------------------------------------------------
//
const TDesC& CAMMSEMCEqualizerControl::PresetL()
{//if no preset is set, return null
    if (iCurrentPreset < 0)
    {
        return KNullDesC;
    }

    else
    {
        // Retrieves a Preset with the given index from the utility class
        iAMMSEMCEqualizerEnvironmentalPresetUtility->PrepareEmptyEqualizerUtilitiesL();  // codescanner::leave
        iAMMSEMCEqualizerEnvironmentalPresetUtility->GetPresetAtIndexL(iPresetName , iCurrentPreset);  // codescanner::leave
        iAMMSEMCEqualizerEnvironmentalPresetUtility->DeleteEmptyEqualizerUtilities();
        return iPresetName;
    }
}

// -----------------------------------------------------------------------------
// CAMMSEMCEqualizerControl::SetEnabledL
// Enables/disables the effect.
// -----------------------------------------------------------------------------
//
void CAMMSEMCEqualizerControl::SetEnabledL(TBool aEnable)
{
    if (aEnable)
    {
        // Enable the effect.

        iMEqualizerControl->Enable();
        DEBUG("AMMS::CAMMSEMCEqualizerControl::SetEnabledL(true), calling Apply");
        iMEqualizerControl->Apply();
    }
    else
    {
        // Disable the effect

        DEBUG("AMMS::CAMMSEMCEqualizerControl::SetEnabledL(false), calling Disable");
        iMEqualizerControl->Disable();
        iMEqualizerControl->Apply();
    }
}

// -----------------------------------------------------------------------------
// CAMMSEMCEqualizerControl::PrepareControlL
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSEMCEqualizerControl::PrepareControlL()
{
    // Perform the action only for the first time, skip if called afterwards
    if (!iMEqualizerControl)
    {
        DEBUG("AMMS::CAMMSEMCEqualizerControl::PrepareControlL");
        //Create Equalizer Effect Control
        iStreamControl = (static_cast<CMMAEMCAudioPlayer*>(iMMAPlayer))->StreamControl();
        iFactory = (static_cast<CMMAEMCAudioPlayer*>(iMMAPlayer))->MMFactory();

        MEffectControl* temp(NULL);
        User::LeaveIfError(iFactory->CreateEffectControl(KEqualizerEffectControl, temp));
        iMEqualizerControl  = static_cast<MEqualizerControl*>(temp);
        User::LeaveIfError(iStreamControl->AddEffect(*iMEqualizerControl));
        iMEqualizerControl->Enable();
    }


}

// -----------------------------------------------------------------------------
// CAMMSEMCEqualizerControl::DeallocateControl
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSEMCEqualizerControl::DeallocateControl()
{
    if (iMEqualizerControl)
    {
        DEBUG("AMMS::CAMMSEMCEqualizerControl::DeallocateControl");

        // Equalizer for Audio can be disabled or enabled
        TRAPD(err,iMEqualizerControl->Disable());
        if (err != KErrNone)
        {
            //Some EMC Error
            DEBUG_INT("AMMS::CAMMSEMCEqualizerControl::DeallocateControl err = %d",err);
        }
        //return the control to factory
        MEffectControl* temp = iMEqualizerControl;
        iStreamControl->RemoveEffect(*temp);
        // Delete the Effect
        MEffectControl* tempCtrl = iMEqualizerControl;
        iFactory->DeleteEffectControl(tempCtrl);
        iMEqualizerControl = NULL;
    }
}


const TDesC& CAMMSEMCEqualizerControl::ClassName() const
{
    return KAMMSEMCEqualizerControl;
}

// -----------------------------------------------------------------------------
// CAMMSEMCEqualizerControl::SetEnforcedL
// Enforces the effect to be in use.
// -----------------------------------------------------------------------------
//
void CAMMSEMCEqualizerControl::SetEnforcedL(TBool aEnforced)
{
    DEBUG("AMMS::CAMMSEMCEqualizerControl:SetEnforcedL");
    // Indicate the effect is to be enforced or not. ETrue = Enforced.

    iMEqualizerControl->Enforce(aEnforced);

    ApplySettingsL();

}

// -----------------------------------------------------------------------------
// CAMMSEMCEqualizerControl::Enforced
// Returns the current enforced setting of the effect.
// -----------------------------------------------------------------------------
//
TBool CAMMSEMCEqualizerControl::Enforced()
{
    // Returns ETrue if the effect is enforced, EFalse if not enforced.
    TBool enforced;
    iMEqualizerControl->IsEnforced(enforced);
    return enforced;

}

// -----------------------------------------------------------------------------
// CAMMSEMCEqualizerControl::SetScopeL
// Sets the scope of the effect.
// -----------------------------------------------------------------------------
//
void CAMMSEMCEqualizerControl::SetScopeL(TInt aScope)
{
    DEBUG("AMMS::CAMMSEMCEqualizerControl:SetScopeL");
    __ASSERT_DEBUG(
        (aScope == CAMMSEffectControlGroup::EScopeLiveOnly),
        User::Invariant());
    // Just to suppress warning in release build
    (void)aScope;
}

// -----------------------------------------------------------------------------
// CAMMSEMCEqualizerControl::Scope
// Returns the scope in which the effect is present.
// -----------------------------------------------------------------------------
//
TInt CAMMSEMCEqualizerControl::Scope()
{
    // For now only the (LIVE_ONLY) scope is supported.
    return CAMMSEffectControlGroup::EScopeLiveOnly;
}

// -----------------------------------------------------------------------------
// CAMMSEMCAudioVirtualizerControl::ApplySettingsL
// Apply changed settings if Effect is in enabled state.
// -----------------------------------------------------------------------------
void CAMMSEMCEqualizerControl::ApplySettingsL()
{
    DEBUG("AMMS::CAMMSEMCEqualizerControl:ApplySettingsL called, checking state");
    TBool enabled;
    iMEqualizerControl->IsEnabled(enabled);
    if (enabled)
    {
        DEBUG("AMMS::CAMMSEMCEqualizerControl:ApplySettingsL calling ApplyL");
        iMEqualizerControl->Apply();
    }
}


// -----------------------------------------------------------------------------
// CAMMSEMCEqualizerControl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CAMMSEMCEqualizerControl::ConstructL()
{
    // Create array for preset names
    iPresetNames = new(ELeave) CDesCArrayFlat(1);

    // Set current preset to a negative value as it is not set yet
    iCurrentPreset = -1;

    CAMMSEffectControl::ConstructL();
    iAMMSEMCEqualizerEnvironmentalPresetUtility =  new(ELeave)CAMMSEMCEqualizerEnvironmentalPresetUtility;
    iAMMSEMCEqualizerEnvironmentalPresetUtility->ConstructL();
}

// -----------------------------------------------------------------------------
// CAMMSEMCEqualizerControl::CAMMSEMCEqualizerControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSEMCEqualizerControl::CAMMSEMCEqualizerControl(CMMAPlayer* aPlayer)
        : CAMMSBaseEqualizerControl(aPlayer)
{
    iMMAPlayer = aPlayer;
}

//  End of File



