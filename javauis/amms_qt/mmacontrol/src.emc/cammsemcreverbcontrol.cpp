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
* Description:  Manipulates the settings of an audio effect called reverb.
*
*/


// INCLUDE FILES
#include "cammsemcreverbcontrol.h"
#include <RoomLevelBase.h>
#include <logger.h>
#ifdef _DEBUG
// CONSTANTS
const TInt KAMMSMaxReverbLevel = 0;
const TInt KAMMSMinReverbTime = 0;
#endif // _DEBUG

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSEMCReverbControl::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSEMCReverbControl* CAMMSEMCReverbControl::NewLC(CMMAPlayer* aPlayer)
{
    CAMMSEMCReverbControl* self = new(ELeave) CAMMSEMCReverbControl(aPlayer);

    CleanupStack::PushL(self);
    self->ConstructL();

    return self;
}

// Destructor
CAMMSEMCReverbControl::~CAMMSEMCReverbControl()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCReverbControl::~CAMMSEMCReverbControl");

    // Perform DeallocateControl, if the state change has not yet performed it.
    DeallocateControl();
    if (iAMMSEMCReverbEnvironmentalPresetUtility)
        delete iAMMSEMCReverbEnvironmentalPresetUtility;
    delete iPresetNames;
}

// -----------------------------------------------------------------------------
// CAMMSEMCReverbControl::SetReverbLevelL
// Sets the gain level of the reverberation.
// -----------------------------------------------------------------------------
//
TInt CAMMSEMCReverbControl::SetReverbLevelL(TInt aLevel)
{
    LOG1( EJavaAMMS, EInfo, "AMMS::CAMMSEMCReverbControl::SetReverbLevelL: %d", aLevel);
    __ASSERT_DEBUG(aLevel <= KAMMSMaxReverbLevel, User::Invariant());
    TInt roomLevel = 0;
    iMReverbControl->RoomLevel(roomLevel);
    TInt reflectionLevel =0;
    iMReverbControl->ReflectionsLevel(reflectionLevel);
    TInt tempChange = aLevel - reflectionLevel - roomLevel;

    // Sets the reverb reflections level in mB
    iMReverbControl->SetReflectionsLevel(aLevel - roomLevel);

    // Calculate native reverb level.
    TInt reverbLevel;
    // iMReverbControl->ReverbLevel(reverbLevel);
    reverbLevel = iEnvReverbLevel + tempChange;

    // Ensure that the level is within the limits.

    TInt minLevel = 0;
    TInt maxLevel = 0;
    iMReverbControl->ReverbLevelRange(minLevel, maxLevel);

    LOG1( EJavaAMMS, EInfo, "AMMS::CAMMSEMCReverbControl::SetReverbLevelL: orig %d",
              reverbLevel);

    reverbLevel = Min(reverbLevel, maxLevel);
    reverbLevel = Max(reverbLevel, minLevel);

    LOG1( EJavaAMMS, EInfo, "AMMS::CAMMSEMCReverbControl::SetReverbLevelL: set %d",
              reverbLevel);
    // Sets the reverb level in mB
    iMReverbControl->SetReverbLevel(reverbLevel);
    // Apply can be called for Reverb, as it does not internally include EnableL
    // unlike in case of Equalizer or StereoWidening
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCReverbControl::SetReverbLevelL calling ApplyL");
    iMReverbControl->Apply();

    // Return the value that was used in setting the reverb
    return aLevel;
}

// -----------------------------------------------------------------------------
// CAMMSEMCReverbControl::SetReverbTimeL
// Sets the reverberation time of the reverb.
// -----------------------------------------------------------------------------
//
void CAMMSEMCReverbControl::SetReverbTimeL(TInt aTime)
{
    LOG1( EJavaAMMS, EInfo, "AMMS::CAMMSEMCReverbControl::SetReverbTimeL: %d", aTime);
    // Check in debug build that aTime is within valid range.
    __ASSERT_DEBUG(aTime >= KAMMSMinReverbTime, User::Invariant());
    iMReverbControl->SetDecayTime(aTime);
    iMReverbControl->Apply();
}

// -----------------------------------------------------------------------------
// CAMMSEMCReverbControl::ReverbLevel
// Gets the gain level of the reverberation.
// -----------------------------------------------------------------------------
//
TInt CAMMSEMCReverbControl::ReverbLevel()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCReverbControl::ReverbLevel called");
    TInt iReverbLevel;
    iMReverbControl->ReverbLevel(iReverbLevel);
    return iReverbLevel;
}

// -----------------------------------------------------------------------------
// CAMMSEMCReverbControl::ReverbTime
// Gets the reverberation time.
// -----------------------------------------------------------------------------
//
TInt CAMMSEMCReverbControl::ReverbTime()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCReverbControl::ReverbTime called");

    TUint iReverbTime;
    iMReverbControl->DecayTime(iReverbTime);
    return iReverbTime;
}

// -----------------------------------------------------------------------------
// CAMMSEMCReverbControl::MinReverbLevel
// Gets the minimum level of the reverberation.
// -----------------------------------------------------------------------------
//
TInt CAMMSEMCReverbControl::MinReverbLevel()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCReverbControl::MinReverbLevel called");

    TInt minLevel;
    TInt maxLevel;
    iMReverbControl->ReverbLevelRange(
        minLevel, maxLevel);

    return minLevel;
}

// -----------------------------------------------------------------------------
// CAMMSEMCReverbControl::MaxReverbLevel
// Gets the maximum level of the reverberation.
// -----------------------------------------------------------------------------
//
TInt CAMMSEMCReverbControl::MaxReverbLevel()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCReverbControl::MaxReverbLevel called");
    TInt minLevel;
    TInt maxLevel;
    iMReverbControl->ReverbLevelRange(minLevel, maxLevel);
    return maxLevel;
}

// -----------------------------------------------------------------------------
// CAMMSEMCReverbControl::SetPresetL
// Sets the effect according to the given preset.
// -----------------------------------------------------------------------------
//
void CAMMSEMCReverbControl::SetPresetL(const TDesC& aPreset)
{
    LOG1( EJavaAMMS, EInfo, "AMMS::CAMMSEMCReverbControl::SetPresetL \"%S\"", aPreset.Ptr());
    CEnvironmentalReverb *iEnvironmentalReverb = NULL;
    User::LeaveIfError(iAMMSEMCReverbEnvironmentalPresetUtility->PrepareEmptyReverbUtilitiesL());
    iAMMSEMCReverbEnvironmentalPresetUtility->GetPresetNamesL(*iPresetNames);
    TInt presetPosition = 0;
    TInt findPreset = iPresetNames->Find(aPreset, presetPosition);
    if (findPreset == 0)  // Find returns zero, if a matching element is found.
    {
        iEnvironmentalReverb = iAMMSEMCReverbEnvironmentalPresetUtility->GetEnvironmentalReverbAtPresetIndexL(presetPosition);
        iCurrentPreset = presetPosition;
    }
    else
    {
        iAMMSEMCReverbEnvironmentalPresetUtility->DeleteEmptyReverbUtilities();
        User::Leave(KErrArgument);
    }

    if (!iEnvironmentalReverb)
    {
        iAMMSEMCReverbEnvironmentalPresetUtility->DeleteEmptyReverbUtilities();
        return;
    }
    //Get all preset data here

    TInt iReverbTime = iEnvironmentalReverb-> DecayTime();
    TInt iReverbLevel = iEnvironmentalReverb->ReflectionsLevel() + iEnvironmentalReverb->RoomLevel();
    iEnvReverbLevel = iEnvironmentalReverb->ReverbLevel();
    TInt roomLevel = iEnvironmentalReverb->RoomLevel();
    TInt reflectionLevel = iEnvironmentalReverb->ReflectionsLevel();
    TUint decayHFRatio = iEnvironmentalReverb->DecayHFRatio();
    TUint density = iEnvironmentalReverb->Density();
    TUint diffusion = iEnvironmentalReverb->Diffusion();
    TUint reflectionsDelay = iEnvironmentalReverb->ReflectionsDelay();
    TUint reverbDelay = iEnvironmentalReverb->ReverbDelay();
    TUint roomHFLevel = iEnvironmentalReverb->RoomHFLevel();

    // Set all preset data to EMC
    TInt err = iMReverbControl->SetDecayTime(iReverbTime);
    err = iMReverbControl->SetReverbLevel(iReverbLevel);
    err = iMReverbControl->SetRoomLevel(roomLevel);
    err = iMReverbControl->SetReflectionsLevel(reflectionLevel);
    err = iMReverbControl->SetDecayHFRatio(decayHFRatio);
    err = iMReverbControl->SetDensity(density);
    err = iMReverbControl->SetDiffusion(diffusion);
    err = iMReverbControl->SetReflectionsDelay(reflectionsDelay);
    err = iMReverbControl->SetReverbDelay(reverbDelay);
    err = iMReverbControl->SetRoomHFLevel(roomHFLevel);
    err = iMReverbControl->Apply();
    ELOG1( EJavaAMMS, "AMMS::CAMMSEMCReverbControl::SetPresetL iMReverbControl setter func. err = %d",err);
    iAMMSEMCReverbEnvironmentalPresetUtility->DeleteEmptyReverbUtilities();
}

// -----------------------------------------------------------------------------
// CAMMSEMCReverbControl::PresetNamesL
// Gets the available preset names.
// -----------------------------------------------------------------------------
//
const CDesCArray& CAMMSEMCReverbControl::PresetNamesL()
{

    //CEnvironmentalReverb *iEnvironmentalReverb = NULL;
    iAMMSEMCReverbEnvironmentalPresetUtility->PrepareEmptyReverbUtilitiesL();
    iAMMSEMCReverbEnvironmentalPresetUtility->GetPresetNamesL(*iPresetNames);
    iAMMSEMCReverbEnvironmentalPresetUtility->DeleteEmptyReverbUtilities();
    return *iPresetNames;
}

// -----------------------------------------------------------------------------
// CAMMSEMCReverbControl::PresetL
// Gets the current preset.
// -----------------------------------------------------------------------------
//
const TDesC& CAMMSEMCReverbControl::PresetL()
{
    //if no preset is set, return null
    if (iCurrentPreset < 0)
    {
        return KNullDesC;
    }

    else
    {
        // Retrieves a Preset with the given index from the utility class
        iAMMSEMCReverbEnvironmentalPresetUtility->PrepareEmptyReverbUtilitiesL();
        iAMMSEMCReverbEnvironmentalPresetUtility->GetPresetAtIndexL(iPresetName , iCurrentPreset);
        iAMMSEMCReverbEnvironmentalPresetUtility->DeleteEmptyReverbUtilities();
        return iPresetName;
    }


}


// -----------------------------------------------------------------------------
// CAMMSEMCReverbControl::SetEnabledL
// Enables/disables the effect.
// -----------------------------------------------------------------------------
//
void CAMMSEMCReverbControl::SetEnabledL(TBool aEnable)
{
    if (aEnable)
    {
        // Enable the effect
        LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCReverbControl::SetEnabledL(true), calling Enable");
        iMReverbControl->Enable();
        iMReverbControl->Apply();
    }
    else
    {
        LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCReverbControl::SetEnabledL(false), calling Disable");
        iMReverbControl->Disable();
    }
}

// -----------------------------------------------------------------------------
// CAMMSEMCReverbControl::PrepareControlL
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSEMCReverbControl::PrepareControlL()
{
    // Perform the action only for the first time, skip if called afterwards
    if (!iMReverbControl)
    {
        LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCReverbControl::PrepareControlL");
        //Create Reverb Effect Control
        iStreamControl = (static_cast<CMMAEMCAudioPlayer*>(iMMAPlayer))->StreamControl();
        iFactory = (static_cast<CMMAEMCAudioPlayer*>(iMMAPlayer))->MMFactory();

        MEffectControl* temp(NULL);
        User::LeaveIfError(iFactory->CreateEffectControl(KReverbEffectControl, temp));
        iMReverbControl  = static_cast<MReverbControl*>(temp);
        User::LeaveIfError(iStreamControl->AddEffect(*iMReverbControl));
        iMReverbControl->Enable();
        SetPresetL(KAMMSEMCDefaultReverbPreset);

    }
}

// -----------------------------------------------------------------------------
// CAMMSEMCReverbControl::DeallocateControl
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSEMCReverbControl::DeallocateControl()
{

    if (iMReverbControl)
    {
        LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCReverbControl::DeallocateControl");

        // Reverb for Audio can be disabled or enabled
        TRAPD(err,iMReverbControl->Disable());
        if (err != KErrNone)
        {
            //Some EMC Error
            ELOG1( EJavaAMMS, "AMMS::CAMMSEMCReverbControl::DeallocateControl err = %d",err);
        }
        //return the control to factory
        MEffectControl* temp = iMReverbControl;
        iStreamControl->RemoveEffect(*temp);
        // Delete the Effect
        MEffectControl* tempCtrl = iMReverbControl;
        iFactory->DeleteEffectControl(tempCtrl);
        iMReverbControl = NULL;

        iCurrentPreset = -1;
    }
}


// -----------------------------------------------------------------------------
// CAMMSEMCReverbControl::GetReverbControl
// Returns the  reverb control. It will be used by CAMMSEMCReverbSourceControl
// -----------------------------------------------------------------------------
MReverbControl * CAMMSEMCReverbControl::GetReverbControlL()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCReverbControl::GetReverbControl");

    if (! iMReverbControl)
    {
        PrepareControlL(); // creates the control
    }

    return iMReverbControl;
}

// -----------------------------------------------------------------------------
// CAMMSEMCReverbControl::CurrentPresetIndex
// Returns the  Current Preset Index. It will be used by CAMMSEMCReverbSourceControl
// -----------------------------------------------------------------------------
TInt CAMMSEMCReverbControl::CurrentPresetIndex()
{
    return iCurrentPreset;
}

// -----------------------------------------------------------------------------
// Overriding the base class CAMMSEffectControl function here
//------------------------------------------------------------------------------

void CAMMSEMCReverbControl::SetEnforcedL(TBool aEnforced)
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCReverbControl::SetEnforcedL");
    // Indicate the effect is to be enforced or not. ETrue = Enforced.
    iMReverbControl->Enforce(aEnforced);

    ApplySettingsL();

}
TBool CAMMSEMCReverbControl::Enforced()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCReverbControl::Enforced");
    // Returns ETrue if the effect is enforced, EFalse if not enforced.
    TBool enforced;
    iMReverbControl->IsEnforced(enforced);
    return enforced;

}
void CAMMSEMCReverbControl::SetScopeL(TInt aScope)
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCReverbControl::SetScopeL");
    __ASSERT_DEBUG(
        (aScope == CAMMSEffectControlGroup::EScopeLiveOnly),
        User::Invariant());
    // Just to suppress warning in release build
    (void)aScope;
}
TInt CAMMSEMCReverbControl::Scope()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCReverbControl::Scope");
    // For now only the (LIVE_ONLY) scope is supported.
    return CAMMSEffectControlGroup::EScopeLiveOnly;

}
void CAMMSEMCReverbControl::ApplySettingsL()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCReverbControl::ApplySettingsL");
    TBool enabled;
    iMReverbControl->IsEnabled(enabled);
    if (enabled)
    {
        LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCReverbControl:ApplySettingsL calling ApplyL");
        iMReverbControl->Apply();
    }
}

const TDesC& CAMMSEMCReverbControl::ClassName() const
{
    return KAMMSEMCReverbControl;
}

// -----------------------------------------------------------------------------
// CAMMSEMCReverbControl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CAMMSEMCReverbControl::ConstructL()
{
    // Create array for preset names
    iPresetNames = new(ELeave) CDesCArrayFlat(1);

    // Set current preset to a negative value as it is not set yet
    iCurrentPreset = -1;

    CAMMSEffectControl::ConstructL();
    iAMMSEMCReverbEnvironmentalPresetUtility = new(ELeave) CAMMSEMCReverbEnvironmentalPresetUtility;
    iAMMSEMCReverbEnvironmentalPresetUtility->ConstructL();

}

// -----------------------------------------------------------------------------
// CAMMSEMCReverbControl::CAMMSEMCReverbControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSEMCReverbControl::CAMMSEMCReverbControl(CMMAPlayer* aPlayer)
        : CAMMSBaseReverbControl(aPlayer)
{
    iMMAPlayer = aPlayer;
}

//  End of File
