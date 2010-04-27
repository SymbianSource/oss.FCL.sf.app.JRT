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
* Description:  Group for equalizer controls
*
*/


// INCLUDE FILES
#include <AudioEqualizerUtility.h>
#include <AudioEqualizerUtilityData.h>
#include <jdebug.h>

#include "cammsequalizercontrolgroup.h"
#include "cammsbaseequalizercontrol.h"

// CONSTANTS
namespace
{
const TInt KAMMSDefaultEqualizerLevel = 0;
const TInt KAMMSDefaultEqualizerMaxLevel = 1200;
const TInt KAMMSDefaultEqualizerMinLevel = -1200;
const TInt KAMMSDefaultEqualizerTrebleAndBassLevel = 50;
const TInt KAMMSUndefinedTrebleOrBass = -1004;
const TInt KAMMSKilo = 1000;  // e.g. 1Hz = 1000 mHz
}

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSEqualizerControlGroup::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSEqualizerControlGroup* CAMMSEqualizerControlGroup::NewLC()
{
    DEBUG("AMMS::CAMMSEqualizerControlGroup::NewLC +");

    CAMMSEqualizerControlGroup* self =
        new(ELeave) CAMMSEqualizerControlGroup;

    CleanupStack::PushL(self);
    self->ConstructL();

    DEBUG("AMMS::CAMMSEqualizerControlGroup::NewLC -");

    return self;
}

// Destructor
CAMMSEqualizerControlGroup::~CAMMSEqualizerControlGroup()
{
    DEBUG("AMMS::CAMMSEqualizerControlGroup::~ +");

    delete iEmptyAudioEqualizerUtility;

    iBands.Close();

    DEBUG("AMMS::CAMMSEqualizerControlGroup::~ -");
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControlGroup::Band
// Gets the band that has the most effect on the given frequency
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TInt CAMMSEqualizerControlGroup::Band(TInt aFrequency)
{
    DEBUG("AMMS::CAMMSEqualizerControlGroup::Band +");

    // Check whether bands have effect on the given frequency.
    if ((aFrequency <= 0) ||
            (aFrequency > KAMMSHalfOfSamplingFrequency))    // 24000000 milliHertz
    {
        return -1;  // no effect on the frequency. Frequency 0 goes here too.
    }

    TInt bandCount = iBands.Count();
    if (bandCount == 0)
    {
        return 0;  // if the number of the bands is zero, return band zero.
    }

    // Effect API uses hertzes whereas AMMS uses millihertzes.
    TInt frequencyInHertzes = aFrequency / KAMMSKilo;

    // Find the first band whose cross-over frequency is greater than the
    // given frequency (the band has effect on the given frequency).
    TInt i = 0;
    for (i = 0; i < bandCount; i++)
    {
        if (iBands[ i ].iCrossoverFrequency >= frequencyInHertzes)
        {
            return i;
        }
    }

    DEBUG("AMMS::CAMMSEqualizerControlGroup::Band -");

    // If the given frequency if bigger than any cross-over frequency,
    // return the last band.
    return (bandCount - 1);
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControlGroup::GetBandLevelL
// Gets the gain set for the given equalizer band.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEqualizerControlGroup::GetBandLevelL(TInt aBand, TInt& aBandLevel)
{
    DEBUG("AMMS::CAMMSEqualizerControlGroup::GetBandLevelL +");

    CheckBandIndexL(aBand);

    aBandLevel = iBands[ aBand ].iBandLevel;  // CSI: 2 Index checked above #

    DEBUG("AMMS::CAMMSEqualizerControlGroup::GetBandLevelL -");
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControlGroup::Bass
// Gets the bass level.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TInt CAMMSEqualizerControlGroup::Bass()
{
    return iBass;
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControlGroup::GetCenterFreqL
// Gets the center frequency of the given band
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEqualizerControlGroup::GetCenterFreqL(TInt aBand, TInt& aCenterFreq)
{
    DEBUG("AMMS::CAMMSEqualizerControlGroup::GetCenterFreqL +");

    CheckBandIndexL(aBand);

    // Effect API uses hertzes whereas AMMS uses millihertzes.
    aCenterFreq = iBands[ aBand ].iCenterFrequency * KAMMSKilo; // CSI: 2 Index checked above #

    DEBUG("AMMS::CAMMSEqualizerControlGroup::GetCenterFreqL -");
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControlGroup::MaxBandLevel
// Returns the maximum band level supported
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TInt CAMMSEqualizerControlGroup::MaxBandLevel()
{
    return iMaxBandLevel;
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControlGroup::MinBandLevel
// Returns the minimum band level supported.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TInt CAMMSEqualizerControlGroup::MinBandLevel()
{
    return iMinBandLevel;
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControlGroup::NumberOfBands
// Gets the number of frequency bands that the equalizer supports.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TInt CAMMSEqualizerControlGroup::NumberOfBands()
{
    return iBands.Count();
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControlGroup::Treble
// Gets the treble level.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TInt CAMMSEqualizerControlGroup::Treble()
{
    return iTreble;
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControlGroup::SetBandLevelL
// Sets the given equalizer band to the given gain value.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEqualizerControlGroup::SetBandLevelL(TInt aLevel, TInt aBand)
{
    DEBUG("AMMS::CAMMSEqualizerControlGroup::SetBandLevelL +");

    // check given parameters
    CheckBandIndexL(aBand);

    if (aLevel < iMinBandLevel || aLevel > iMaxBandLevel)
    {
        User::Leave(KErrArgument);
    }

    // Set new level to controls.
    TInt count = ControlCount();
    for (TInt i = 0; i < count; i++)
    {
        TypeSafeControl(i)->SetBandLevelL(aLevel, aBand);
    }

    iBands[ aBand ].iBandLevel = aLevel;  // CSI: 2 Index checked above #

    // remove current preset since it is not valid any more
    iPresetIndex = -1;

    iTreble = KAMMSUndefinedTrebleOrBass;
    iBass = KAMMSUndefinedTrebleOrBass;

    DEBUG("AMMS::CAMMSEqualizerControlGroup::SetBandLevelL -");
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControlGroup::SetBassL
// Sets the bass level
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEqualizerControlGroup::SetBassL(TInt aLevel, TInt& aSetLevel)
{
    DEBUG("AMMS::CAMMSEqualizerControlGroup::SetBassL +");

    __ASSERT_DEBUG((aLevel >= iMinBandLevel) &&
                   (aLevel <= iMaxBandLevel), User::Invariant());

    // Set the bass band (first band) to the given percentage of the valid range
    // between MinBandLevel and MaxBandLevel.
    TInt newBandLevel = iMinBandLevel + aLevel *
                        (iMaxBandLevel - iMinBandLevel) / 100;   // CSI: 47 Value 100 means 100% here #

    // Set new value to bass band (band 0) if band count > 0
    if (iBands.Count() > 0)
    {
        // Set new bass (band 0) to controls.
        TInt count = ControlCount();
        for (TInt i = 0; i < count; i++)
        {
            TypeSafeControl(i)->SetBandLevelL(newBandLevel, 0);
        }

        iBands[ 0 ].iBandLevel = newBandLevel;
    }

    // remove the current preset since it is not valid any more
    iPresetIndex = -1;

    iBass = aLevel;

    aSetLevel = iBass;

    DEBUG("AMMS::CAMMSEqualizerControlGroup::SetBassL -");
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControlGroup::SetTrebleL
// Sets the treble level
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEqualizerControlGroup::SetTrebleL(TInt aLevel, TInt& aSetLevel)
{
    DEBUG("AMMS::CAMMSEqualizerControlGroup::SetTrebleL +");

    __ASSERT_DEBUG((aLevel >= iMinBandLevel) &&
                   (aLevel <= iMaxBandLevel), User::Invariant());

    TInt bands = iBands.Count();
    // Set new values to treble bands if there is at least two bands.
    if (bands > 1)
    {
        // Treble affects to two bands.
        TInt trebleIndex1 = bands - 1;  // CSI: 47 Last band #
        TInt trebleIndex2 = bands - 2;  // CSI: 47 Second last band #

        // Set the highest band with 100% weight of the given percentage
        // of the valid range between MinBandLevel and MaxBandLevel.
        TInt newBandLevel1 = iMinBandLevel +
                             aLevel * (iMaxBandLevel - iMinBandLevel) / 100;   // CSI: 47 Value 100 means 100% #

        // The treble affects 50% to the next highest band.
        TInt newBandLevel2 = KAMMSDefaultEqualizerLevel +
                             (newBandLevel1 - KAMMSDefaultEqualizerLevel) / 2;   // CSI: 47 Divided by 2 is the same as 50% #

        // Set new treble level to controls.
        TInt count = ControlCount();
        for (TInt i = 0; i < count; i++)
        {
            TypeSafeControl(i)->SetBandLevelL(newBandLevel1, trebleIndex1);
            TypeSafeControl(i)->SetBandLevelL(newBandLevel2, trebleIndex2);
        }

        iBands[ trebleIndex1 ].iBandLevel = newBandLevel1;
        iBands[ trebleIndex2 ].iBandLevel = newBandLevel2;
    }

    // remove the current preset since it is not valid any more
    iPresetIndex = -1;

    iTreble = aLevel;

    aSetLevel = iTreble;

    DEBUG("AMMS::CAMMSEqualizerControlGroup::SetTrebleL -");
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControlGroup::TypeSafeControl
// Gets control. Ownership is not tranferred.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CAMMSBaseEqualizerControl*
CAMMSEqualizerControlGroup::TypeSafeControl(TInt aIndex) const
{
    return static_cast<CAMMSBaseEqualizerControl*>(Control(aIndex));
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControlGroup::CheckBandIndexL
// Checks whether the band index is between 0 and number of bands -1.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEqualizerControlGroup::CheckBandIndexL(TInt aBand) const
{
    if (aBand < 0 || aBand >= iBands.Count())
    {
        User::Leave(KErrArgument);
    }
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControlGroup::GetPresetBandsL
// Gets bands corresponding to the given preset index.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEqualizerControlGroup::GetPresetBandsL(TUint /*aPresetIndex*/,
        RArray< TEfAudioEqualizerBand >& aBands)
{
    aBands.Reset();
    CleanupClosePushL(aBands);
    TInt count = ControlCount();

    DEBUG_INT("AMMS::CAMMSEqualizerControlGroup::GetPresetBandsL, controls=%d",
              count);

    // Get data of each band from a control. If there is no controls in the
    // group, get the data from the empty group utility.
    if (count > 0)
    {
        CAMMSBaseEqualizerControl* control =
            CAMMSEqualizerControlGroup::TypeSafeControl(0);

        TInt bands = control->NumberOfBands();

        for (TInt i = 0; i < bands; i++)
        {
            TEfAudioEqualizerBand band;

            band.iBandLevel = control->BandLevelL(i);
            band.iBandWidth = control->BandWidth(i);
            band.iCenterFrequency = control->CenterFrequency(i);
            band.iCrossoverFrequency = control->CrossoverFrequency(i);

            aBands.AppendL(band);
        }
    }
    else
    {
        // Create empty group utilities for getting preset data.
        PrepareEmptyGroupUtilitiesL();

        CAudioEqualizer& audioEffect = iEmptyAudioEqualizerUtility->Equalizer();
        TInt bands = audioEffect.NumberOfBands();

        for (TInt i = 0; i < bands; i++)
        {
            TEfAudioEqualizerBand band;

            // Band 0 in JSR-234 equals Band 1 in Effect API
            TUint8 bandId = (TUint8)(i + KAMMSBandOffset);

            band.iBandLevel = audioEffect.BandLevel(bandId);
            band.iBandWidth = audioEffect.BandWidth(bandId);
            band.iCenterFrequency = audioEffect.CenterFrequency(bandId);
            band.iCrossoverFrequency = audioEffect.CrossoverFrequency(bandId);

            aBands.AppendL(band);
        }

        // Delete empty group utilities in order to save memory.
        DeleteEmptyGroupUtilities();
    }
    CleanupStack::Pop();
    DEBUG("AMMS::CAMMSEqualizerControlGroup::GetPresetBandsL -");
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControlGroup::ClassName
// Returns class name that identifies this control group.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
const TDesC16& CAMMSEqualizerControlGroup::ClassName()
{
    return KAMMSEqualizerControlClassName;
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControlGroup::NotifyPlayerAddedL
// Called by when a new player is added
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEqualizerControlGroup::NotifyPlayerAddedL(
    CMMAPlayer *aPlayer,
    CMMAControl* aControl)
{
    DEBUG("AMMS::CAMMSEqualizerControlGroup::NotifyPlayerAddedL +");

    CAMMSEffectControlGroup::NotifyPlayerAddedL(aPlayer, aControl);

    CAMMSBaseEqualizerControl* control =
        static_cast<CAMMSBaseEqualizerControl*>(aControl);

    // if preset was not set into the new control by the main class,
    // set bands, treble, and bass manually
    if (iPresetIndex < 0)
    {
        TInt bands = iBands.Count();

        // set current band levels
        for (TInt i = 0; i < bands; i++)
        {
            control->SetBandLevelL(iBands[ i ].iBandLevel, i);
        }
    }

    DEBUG("AMMS::CAMMSEqualizerControlGroup::NotifyPlayerAddedL -");
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControlGroup::PresetChangedL
// Called when the current preset changes
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEqualizerControlGroup::PresetChangedL()
{
    DEBUG("AMMS::CAMMSEqualizerControlGroup::PresetChangedL +");

    // Invalidate bass and treble levels
    iBass = KAMMSUndefinedTrebleOrBass;
    iTreble = KAMMSUndefinedTrebleOrBass;

    // Get band data from controls (or from empty group utility, if the
    // group has no controls).
    GetPresetBandsL(iPresetIndex, iBands);   // index limits already checked

    DEBUG("AMMS::CAMMSEqualizerControlGroup::PresetChangedL -");
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControlGroup::InitializeL
// Finish initialization (after the 1st player is added)
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEqualizerControlGroup::InitializeL()
{
    DEBUG("AMMS::CAMMSEqualizerControlGroup::InitializeL +");

    CAMMSBaseEqualizerControl* control = TypeSafeControl(0);

    iMaxBandLevel = control->MaxBandLevel();
    iMinBandLevel = control->MinBandLevel();

    DEBUG("AMMS::CAMMSEqualizerControlGroup::InitializeL -");
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControlGroup::PrepareEmptyGroupUtilitiesL
// Creates utilities that can be used to obtain preset names and preset data.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEqualizerControlGroup::PrepareEmptyGroupUtilitiesL()
{
    DEBUG("AMMS::CAMMSEqualizerControlGroup::PrepareEmptyGroupUtilitiesL +");

    if (!iEmptyAudioEqualizerUtility)
    {
        CAMMSEffectControlGroup::PrepareEmptyGroupUtilitiesL();

        iEmptyAudioEqualizerUtility =
            CAudioEqualizerUtility::NewL(*iEmptyPlayerUtility);
    }

    DEBUG("AMMS::CAMMSEqualizerControlGroup::PrepareEmptyGroupUtilitiesL -");
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControlGroup::DeleteEmptyGroupUtilities
// Deletes utilities that are used to obtain preset names and preset data.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEqualizerControlGroup::DeleteEmptyGroupUtilities()
{
    DEBUG("AMMS::CAMMSEqualizerControlGroup::DeleteEmptyGroupUtilities +");

    if (iEmptyPlayerUtility)
    {
        delete iEmptyAudioEqualizerUtility;
        iEmptyAudioEqualizerUtility = NULL;

        CAMMSEffectControlGroup::DeleteEmptyGroupUtilities();
    }

    DEBUG("AMMS::CAMMSEqualizerControlGroup::DeleteEmptyGroupUtilities -");
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControlGroup::GetPresetNamesL
// Gets list of preset names available.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEqualizerControlGroup::GetPresetNamesL(
    CDesCArray& aPresetNames)
{
    DEBUG("AMMS::CAMMSEqualizerControlGroup::GetPresetNamesL +");

    // Create empty group utilities for getting preset names.
    PrepareEmptyGroupUtilitiesL();

    TArray< TEfAudioEqualizerUtilityPreset > presetNames =
        iEmptyAudioEqualizerUtility->Presets();


    TInt presetCount = presetNames.Count();

    for (TInt i = 0; i < presetCount; i++)
    {
        aPresetNames.AppendL(presetNames[ i ].iPresetName);
    }

    // Delete empty group utilities in order to save memory.
    DeleteEmptyGroupUtilities();

    DEBUG("AMMS::CAMMSEqualizerControlGroup::GetPresetNamesL -");
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControlGroup::CAMMSEqualizerControlGroup
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
CAMMSEqualizerControlGroup::CAMMSEqualizerControlGroup()
        : CAMMSEffectControlGroup(KAMMSBaseEqualizerControl)
{
    iMaxBandLevel = KAMMSDefaultEqualizerMaxLevel;
    iMinBandLevel = KAMMSDefaultEqualizerMinLevel;
}

// -----------------------------------------------------------------------------
// CAMMSEqualizerControlGroup::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CAMMSEqualizerControlGroup::ConstructL()
{
    DEBUG("AMMS::CAMMSEqualizerControlGroup::ConstructL +");

    CAMMSEffectControlGroup::BaseConstructL();

    // Get band data from empty group utility.
    // If the Effect API implementation does not support the effect,
    // the function leaves with KErrNotSupported. The leaving can be ignored
    // in this case, the result is that the list of supported presets
    // remain empty.
    TRAPD(err, GetPresetBandsL(0, iBands));

    DEBUG_INT("AMMS::CAMMSEqualizerControlGroup::ConstructL, err %d", err);

    // In case of an error, delete possible utilities to save memory.
    if (err != KErrNone)
    {
        DeleteEmptyGroupUtilities();
    }

    // Ignore the error so that using MMA and AMMS is possible.
    // Only successfully read bands are visible for the user.


    TInt bands = iBands.Count();

    // Set default band levels.
    for (TInt i = 0; i < bands; i++)
    {
        iBands[ i ].iBandLevel = KAMMSDefaultEqualizerLevel;
    }

    // Set default values for Bass and Treble. Value 50 means flat equalization.
    iBass = KAMMSDefaultEqualizerTrebleAndBassLevel;
    iTreble = KAMMSDefaultEqualizerTrebleAndBassLevel;

    DEBUG("AMMS::CAMMSEqualizerControlGroup::ConstructL -");
}

//  End of File
