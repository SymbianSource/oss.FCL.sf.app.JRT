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
#include "cammsreverbcontrol.h"
#include <RoomLevelBase.h>
#include <jdebug.h>

#ifdef _DEBUG
// CONSTANTS
const TInt KAMMSMaxReverbLevel = 0;
const TInt KAMMSMinReverbTime = 0;
#endif // _DEBUG

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSReverbControl::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSReverbControl* CAMMSReverbControl::NewLC(CMMAPlayer* aPlayer)
{
    CAMMSReverbControl* self = new(ELeave) CAMMSReverbControl(aPlayer);

    CleanupStack::PushL(self);
    self->ConstructL();

    return self;
}

// Destructor
CAMMSReverbControl::~CAMMSReverbControl()
{
    DEBUG("AMMS::CAMMSReverbControl::~CAMMSReverbControl");

    // Perform DeallocateControl, if the state change has not yet performed it.
    DeallocateControl();
    delete iReverbUtility;
    delete iPresetNames;
}

// -----------------------------------------------------------------------------
// CAMMSReverbControl::SetReverbLevelL
// Sets the gain level of the reverberation.
// -----------------------------------------------------------------------------
//
TInt CAMMSReverbControl::SetReverbLevelL(TInt aLevel)
{
    DEBUG_INT("AMMS::CAMMSReverbControl::SetReverbLevelL: %d", aLevel);

    // Check in debug build that aLevel is within valid range.
    __ASSERT_DEBUG(aLevel <= KAMMSMaxReverbLevel, User::Invariant());

    CEnvironmentalReverb* reverbEffect = (CEnvironmentalReverb*)iAudioEffect;

    TInt roomLevel = reverbEffect->RoomLevel();
    TInt tempChange = aLevel - reverbEffect->ReflectionsLevel() - roomLevel;

    // Sets the reverb reflections level in mB
    reverbEffect->SetReflectionsLevelL(aLevel - roomLevel);


    // Calculate native reverb level.
    TInt reverbLevel = reverbEffect->ReverbLevel() + tempChange;

    // Ensure that the level is within the limits.

    TInt32 minLevel = 0;
    TInt32 maxLevel = 0;
    reverbEffect->ReverbLevelRange(minLevel, maxLevel);

    DEBUG_INT("AMMS::CAMMSReverbControl::SetReverbLevelL: orig %d",
              reverbLevel);

    reverbLevel = Min(reverbLevel, maxLevel);
    reverbLevel = Max(reverbLevel, minLevel);

    DEBUG_INT("AMMS::CAMMSReverbControl::SetReverbLevelL: set %d",
              reverbLevel);

    // Sets the reverb level in mB
    reverbEffect->SetReverbLevelL(reverbLevel);

    // Apply can be called for Reverb, as it does not internally include EnableL
    // unlike in case of Equalizer or StereoWidening
    DEBUG("AMMS::CAMMSReverbControl::SetReverbLevelL calling ApplyL");
    reverbEffect->ApplyL();

    // Return the value that was used in setting the reverb
    return aLevel;
}

// -----------------------------------------------------------------------------
// CAMMSReverbControl::SetReverbTimeL
// Sets the reverberation time of the reverb.
// -----------------------------------------------------------------------------
//
void CAMMSReverbControl::SetReverbTimeL(TInt aTime)
{
    DEBUG_INT("AMMS::CAMMSReverbControl::SetReverbTimeL: %d", aTime);

    // Check in debug build that aTime is within valid range.
    __ASSERT_DEBUG(aTime >= KAMMSMinReverbTime, User::Invariant());

    // Sets the decay time in milliseconds
    ((CEnvironmentalReverb*)iAudioEffect)->SetDecayTimeL(aTime);

    // Apply can be called for Reverb, as it does not internally include EnableL
    // unlike in case of Equalizer or StereoWidening
    DEBUG("AMMS::CAMMSReverbControl::SetReverbTimeL calling ApplyL");
    iAudioEffect->ApplyL();

}

// -----------------------------------------------------------------------------
// CAMMSReverbControl::ReverbLevel
// Gets the gain level of the reverberation.
// -----------------------------------------------------------------------------
//
TInt CAMMSReverbControl::ReverbLevel()
{
    DEBUG("AMMS::CAMMSReverbControl::ReverbLevel called");
    // Return here (reflections level + room level)
    return (((CEnvironmentalReverb*)iAudioEffect)->ReflectionsLevel() +
            ((CEnvironmentalReverb*)iAudioEffect)->RoomLevel());
}

// -----------------------------------------------------------------------------
// CAMMSReverbControl::ReverbTime
// Gets the reverberation time.
// -----------------------------------------------------------------------------
//
TInt CAMMSReverbControl::ReverbTime()
{
    DEBUG("AMMS::CAMMSReverbControl::ReverbTime called");

    return (((CEnvironmentalReverb*)iAudioEffect)->DecayTime());
}

// -----------------------------------------------------------------------------
// CAMMSReverbControl::MinReverbLevel
// Gets the minimum level of the reverberation.
// -----------------------------------------------------------------------------
//
TInt CAMMSReverbControl::MinReverbLevel()
{
    DEBUG("AMMS::CAMMSReverbControl::MinReverbLevel called");

    TInt32 minLevel;
    TInt32 maxLevel;
    ((CEnvironmentalReverb*)iAudioEffect)->ReverbLevelRange(
        minLevel, maxLevel);

    return minLevel;
}

// -----------------------------------------------------------------------------
// CAMMSReverbControl::MaxReverbLevel
// Gets the maximum level of the reverberation.
// -----------------------------------------------------------------------------
//
TInt CAMMSReverbControl::MaxReverbLevel()
{
    DEBUG("AMMS::CAMMSReverbControl::MaxReverbLevel called");

    TInt32 minLevel;
    TInt32 maxLevel;
    ((CEnvironmentalReverb*)iAudioEffect)->ReverbLevelRange(
        minLevel, maxLevel);

    return maxLevel;
}

// -----------------------------------------------------------------------------
// CAMMSReverbControl::CurrentPresetIndex
// Gets current preset index.
// -----------------------------------------------------------------------------
//
TInt CAMMSReverbControl::CurrentPresetIndex()
{
    return iCurrentPreset;
}
// -----------------------------------------------------------------------------
// CAMMSReverbControl::SetPresetL
// Sets the effect according to the given preset.
// -----------------------------------------------------------------------------
//
void CAMMSReverbControl::SetPresetL(const TDesC& aPreset)
{
    DEBUG_STR("AMMS::CAMMSReverbControl::SetPresetL \"%S\"", aPreset);

    const CDesCArray& presetNames = PresetNamesL();

    TInt presetPosition = 0;
    TInt findPreset = presetNames.Find(aPreset, presetPosition);
    if (findPreset == 0)  // Find returns zero, if a matching element is found.
    {
        // This supposes that the indexing of the presets starts at zero.
        iReverbUtility->GetPresetL(presetPosition);

        // Set the base class audio effect as the new CEnvironmentalReverb
        // that is set with the previous GetPresetL method.
        iAudioEffect = &(iReverbUtility->EnvironmentalReverb());
        iCurrentPreset = presetPosition;

        DEBUG_STR("AMMS::CAMMSReverbControl::SetPresetL \"%S\" GetPresetL OK",
                  aPreset);
    }
    else
    {
        User::Leave(KErrArgument);
    }
}

// -----------------------------------------------------------------------------
// CAMMSReverbControl::PresetNamesL
// Gets the available preset names.
// -----------------------------------------------------------------------------
//
const CDesCArray& CAMMSReverbControl::PresetNamesL()
{
    // Returns an array of all preset names (pre-defined and user-defined).
    // The pre-defined presets are in the beginning of the list.
    TArray< TEfEnvironmentalReverbUtilityPreset > presetNames =
        iReverbUtility->Presets();

    // Before appending the preset names, reset the member array
    iPresetNames->Reset();
    for (TInt i = 0; i < presetNames.Count(); i++)
    {
        iPresetNames->AppendL(presetNames[ i ].iPresetName);
    }

    return *iPresetNames;
}

// -----------------------------------------------------------------------------
// CAMMSReverbControl::PresetL
// Gets the current preset.
// -----------------------------------------------------------------------------
//
const TDesC& CAMMSReverbControl::PresetL()
{
    //if no preset is set, return null
    if (iCurrentPreset < 0)
    {
        return KNullDesC;
    }
    else
    {
        // Retrieves a Preset with the given index from the Central Repository
        return iReverbUtility->GetPresetL(iCurrentPreset);
    }
}


// -----------------------------------------------------------------------------
// CAMMSReverbControl::SetEnabledL
// Enables/disables the effect.
// -----------------------------------------------------------------------------
//
void CAMMSReverbControl::SetEnabledL(TBool aEnable)
{
    if (aEnable)
    {
        // Enable the effect
        DEBUG("AMMS::CAMMSReverbControl::SetEnabledL(true), calling EnableL");
        iAudioEffect->EnableL();
    }
    else
    {
        // Disable the effect
        // Instead of using CAudioEffectBase: virtual void DisableL(),
        // use the utility class DisableEnvironmentalReverbL,
        // which calls DisableL for the correct preset
        DEBUG("AMMS::CAMMSReverbControl::SetEnabledL(false), calling DisableEnvironmentalReverbL");
        iReverbUtility->DisableEnvironmentalReverbL();
    }
}

// -----------------------------------------------------------------------------
// CAMMSReverbControl::PrepareControlL
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSReverbControl::PrepareControlL()
{
    // Perform the action only for the first time, skip if called afterwards
    if (!iReverbUtility)
    {
        DEBUG("AMMS::CAMMSReverbControl::PrepareControlL");

        CCustomCommandUtility* customCommandUtility =
            CreateCustomCommandUtilityL();

        // Effect API takes the ownership of customCommandUtility.
        iReverbUtility = CEnvironmentalReverbUtility::NewL(
                             customCommandUtility);

        SetPresetL(KAMMSDefaultReverbPreset);
    }
}

// -----------------------------------------------------------------------------
// CAMMSReverbControl::DeallocateControl
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSReverbControl::DeallocateControl()
{
    if (iReverbUtility)
    {
        DEBUG("AMMS::CAMMSReverbControl::DeallocateControlL");

        // Delete the Effect API class.
        TRAPD(err, iReverbUtility->DisableEnvironmentalReverbL());
        if (err != KErrNone)
        {
            // The only even theoritically possible error code here would be
            // KErrAccessDenied which is a result from Effect API calling ApplyL
            // method without having update rights, but since the Utility
            // class is already created, that situation can be discarded here.
        }

        delete iReverbUtility;
        iReverbUtility = NULL;

        // Set current preset to a negative value to state it has not been set
        iCurrentPreset = -1;
    }
}

// -----------------------------------------------------------------------------
// CAMMSReverbControl::GetEnvironmentalReverbUtilityL
// Returns the environmental reverb utility.
// -----------------------------------------------------------------------------
void CAMMSReverbControl::GetEnvironmentalReverbUtilityL(
    CEnvironmentalReverbUtility** aEnvironmentalReverbUtility)
{
    DEBUG("AMMS::CAMMSReverbControl::GetEnvironmentalReverbUtilityL");
    DEBUG_INT("AMMS::CAMMSReverbControl::GetEnvironmentalReverbUtilityL CurrentPreset = %d",CurrentPresetIndex());
    *aEnvironmentalReverbUtility = iReverbUtility;
}

const TDesC& CAMMSReverbControl::ClassName() const
{
    return KAMMSReverbControl;
}

// -----------------------------------------------------------------------------
// CAMMSReverbControl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CAMMSReverbControl::ConstructL()
{
    // Create array for preset names
    iPresetNames = new(ELeave) CDesCArrayFlat(1);

    // Set current preset to a negative value as it is not set yet
    iCurrentPreset = -1;

    CAMMSEffectControl::ConstructL();
}

// -----------------------------------------------------------------------------
// CAMMSReverbControl::CAMMSReverbControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSReverbControl::CAMMSReverbControl(CMMAPlayer* aPlayer)
        : CAMMSBaseReverbControl(aPlayer)
{
}

//  End of File
