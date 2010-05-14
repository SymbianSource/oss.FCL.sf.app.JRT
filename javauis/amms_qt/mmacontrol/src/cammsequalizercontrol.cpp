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
#include <logger.h>
#include "cammsequalizercontrol.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSEqualizerControl::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSEqualizerControl* CAMMSEqualizerControl::NewLC(CMMAPlayer* aPlayer)
{
    CAMMSEqualizerControl* self =
        new(ELeave) CAMMSEqualizerControl(aPlayer);

    CleanupStack::PushL(self);
    self->ConstructL();

    return self;
}

// Destructor
CAMMSEqualizerControl::~CAMMSEqualizerControl()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEqualizerControl::~CAMMSEqualizerControl");

    // Perform DeallocateControl, if the state change has not yet performed it.
    DeallocateControl();
    delete iEqualizerUtility;
    delete iPresetNames;
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControl::BandLevelL
// Gets the gain set for the given equalizer band.
// -----------------------------------------------------------------------------
//
TInt CAMMSEqualizerControl::BandLevelL(TInt aBand)
{
    // if aBand is out of range the method must leave with KErrArgument.
    if (aBand < 0 || aBand > (NumberOfBands() - 1))
    {
        User::Leave(KErrArgument);
    }

    // Returns the band level in mB for the specified band
    // AudioEqualizerBase: TInt32 BandLevel( TUint8 aBand ) const;
    return ((CAudioEqualizer*)iAudioEffect)->BandLevel(
               (TUint8)(aBand + KAMMSBandOffset));
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControl::MaxBandLevel
// Returns the maximum band level supported.
// -----------------------------------------------------------------------------
//
TInt CAMMSEqualizerControl::MaxBandLevel()
{
    // Get the dB range in mB for the equalizer.

    TInt32 minLevel = 0;
    TInt32 maxLevel = 0;
    ((CAudioEqualizer*)iAudioEffect)->DbLevelLimits(minLevel, maxLevel);

    return maxLevel;
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControl::MinBandLevel
// Returns the minimum band level supported.
// -----------------------------------------------------------------------------
//
TInt CAMMSEqualizerControl::MinBandLevel()
{
    // Get the dB range in mB for the equalizer.

    TInt32 minLevel = 0;
    TInt32 maxLevel = 0;
    ((CAudioEqualizer*)iAudioEffect)->DbLevelLimits(minLevel, maxLevel);

    return minLevel;
}


// -----------------------------------------------------------------------------
// CAMMSEqualizerControl::BandWidth
// Returns the band width in Hz for the specified band.
// -----------------------------------------------------------------------------
//
TInt CAMMSEqualizerControl::BandWidth(TInt aBand)
{
    return ((CAudioEqualizer*)iAudioEffect)->BandWidth(
               (TUint8)(aBand + KAMMSBandOffset));
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControl::CenterFrequency
// Returns the center frequency in Hz for a given band.
// -----------------------------------------------------------------------------
//
TInt CAMMSEqualizerControl::CenterFrequency(TInt aBand)
{
    return ((CAudioEqualizer*)iAudioEffect)->CenterFrequency(
               (TUint8)(aBand + KAMMSBandOffset));
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControl::CrossoverFrequency
// Returns the cross-over frequency between the given frequency.
// -----------------------------------------------------------------------------
//
TInt CAMMSEqualizerControl::CrossoverFrequency(TInt aBand)
{
    return ((CAudioEqualizer*)iAudioEffect)->CrossoverFrequency(
               (TUint8)(aBand + KAMMSBandOffset));
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControl::NumberOfBands
// Gets the number of frequency bands that the equalizer supports.
// -----------------------------------------------------------------------------
//
TInt CAMMSEqualizerControl::NumberOfBands()
{
    // Returns the number of equalizer bands.
    // AudioEqualizerBase: TUint8 NumberOfBands() const;
    return ((CAudioEqualizer*)iAudioEffect)->NumberOfBands();
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControl::SetBandLevelL
// Sets the given equalizer band to the given gain value.
// -----------------------------------------------------------------------------
//
void CAMMSEqualizerControl::SetBandLevelL(
    TInt aLevel,
    TInt aBand)
{
    LOG2( EJavaAMMS, EInfo, "AMMS::CAMMSEqualizerControl::SetBandLevelL: level=%d, band=%d",
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

    // Sets the equalizer band level value in mB, ranging from Min to Max
    ((CAudioEqualizer*)iAudioEffect)->SetBandLevelL(
        (TInt8)(aBand + KAMMSBandOffset), aLevel);

    ApplySettingsL();
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControl::SetPresetL
// Sets the effect according to the given preset.
// -----------------------------------------------------------------------------
//
void CAMMSEqualizerControl::SetPresetL(const TDesC& aPreset)
{
    LOG1( EJavaAMMS, EInfo, "AMMS::CAMMSEqualizerControl::SetPresetL \"%S\"", aPreset.Ptr());

    const CDesCArray& presetNames = PresetNamesL();

    TInt presetPosition = 0;
    TInt findPreset = presetNames.Find(aPreset, presetPosition);
    if (findPreset == 0)  // Find returns zero, if a matching element is found.
    {
        // This supposes that the indexing of the presets starts at zero.
        iEqualizerUtility->GetPresetL(presetPosition);

        // Set the base class audio effect as the new CAudioEqualizer
        // that is set with the previous GetPresetL method.
        iAudioEffect = &(iEqualizerUtility->Equalizer());
        iCurrentPreset = presetPosition;

        LOG1( EJavaAMMS, EInfo, "AMMS::CAMMSEqualizerControl::SetPresetL \"%S\" GetPresetL OK",
                  aPreset.Ptr());
    }
    else
    {
        User::Leave(KErrArgument);
    }
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControl::PresetNamesL
// Gets the available preset names.
// -----------------------------------------------------------------------------
//
const CDesCArray& CAMMSEqualizerControl::PresetNamesL()
{
    // Returns an array of all preset names (pre-defined and user-defined).
    // The pre-defined presets are in the beginning of the list.

    TArray< TEfAudioEqualizerUtilityPreset > presetNames =
        iEqualizerUtility->Presets();

    // Before appending the preset names, reset the member array
    iPresetNames->Reset();
    for (TInt i = 0; i < presetNames.Count(); i++)
    {
        iPresetNames->AppendL(presetNames[ i ].iPresetName);
    }

    return *iPresetNames;
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControl::PresetL
// Gets the current preset.
// -----------------------------------------------------------------------------
//
const TDesC& CAMMSEqualizerControl::PresetL()
{
    //if no preset is set, return null
    if (iCurrentPreset < 0)
    {
        return KNullDesC;
    }
    else
    {
        // Retrieves a Preset with the given index from the Central Repository
        // AudioEqualizerUtility.h:
        //      const TDesC& GetPresetL(TInt aPresetIndex)
        return iEqualizerUtility->GetPresetL(iCurrentPreset);
    }
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControl::SetEnabledL
// Enables/disables the effect.
// -----------------------------------------------------------------------------
//
void CAMMSEqualizerControl::SetEnabledL(TBool aEnable)
{
    if (aEnable)
    {
        // Enable the effect
        // Instead of using CAudioEffectBase: virtual void EnableL(),
        // use the derived effect's (=preset's) ApplyL which calls EnableL.
        LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEqualizerControl::SetEnabledL(true), calling ApplyL");
        iAudioEffect->ApplyL();
    }
    else
    {
        // Disable the effect
        // Instead of using CAudioEffectBase: virtual void DisableL(),
        // use the utility class DisableEqualizerL,
        // which calls DisableL for the correct preset
        LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEqualizerControl::SetEnabledL(false), calling DisableEqualizerL");
        iEqualizerUtility->DisableEqualizerL();
    }
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControl::PrepareControlL
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSEqualizerControl::PrepareControlL()
{
    // Perform the action only for the first time, skip if called afterwards
    if (!iEqualizerUtility)
    {
        LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEqualizerControl::PrepareControlL");

        CCustomCommandUtility* customCommandUtility =
            CreateCustomCommandUtilityL();

        // Effect API takes the ownership of customCommandUtility.
        iEqualizerUtility = CAudioEqualizerUtility::NewL(
                                customCommandUtility);

        // Set the base class audio effect as CAudioEqualizer,
        // even the native AudioEqualizerUtility has an empty CAudioEqualizer
        iAudioEffect = &(iEqualizerUtility->Equalizer());
    }
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControl::DeallocateControl
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSEqualizerControl::DeallocateControl()
{
    if (iEqualizerUtility)
    {
        LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEqualizerControl::DeallocateControlL");

        // Delete the Effect API class.
        TRAPD(err, iEqualizerUtility->DisableEqualizerL());
        if (err != KErrNone)
        {
            // The only even theoritically possible error code here would be
            // KErrAccessDenied which is a result from Effect API calling ApplyL
            // method without having update rights, but since the Utility
            // class is already created, that situation can be discarded here.
        }

        delete iEqualizerUtility;
        iEqualizerUtility = NULL;
    }
}

const TDesC& CAMMSEqualizerControl::ClassName() const
{
    return KAMMSEqualizerControl;
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CAMMSEqualizerControl::ConstructL()
{
    // Create array for preset names
    iPresetNames = new(ELeave) CDesCArrayFlat(1);

    // Set current preset to a negative value as it is not set yet
    iCurrentPreset = -1;

    CAMMSEffectControl::ConstructL();
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControl::CAMMSEqualizerControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSEqualizerControl::CAMMSEqualizerControl(CMMAPlayer* aPlayer)
        : CAMMSBaseEqualizerControl(aPlayer)
{
}

//  End of File



