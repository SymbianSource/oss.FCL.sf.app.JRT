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
#include "cammsaudiovirtualizercontrol.h"

// CONSTANTS
const TInt KAMMSDefaultStereoWideningLevel = 100;

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CAMMSAudioVirtualizerControl::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSAudioVirtualizerControl* CAMMSAudioVirtualizerControl::NewLC(
    CMMAPlayer* aPlayer)
{
    CAMMSAudioVirtualizerControl* self =
        new(ELeave) CAMMSAudioVirtualizerControl(aPlayer);

    CleanupStack::PushL(self);
    self->ConstructL();

    return self;
}

// Destructor
CAMMSAudioVirtualizerControl::~CAMMSAudioVirtualizerControl()
{
    DEBUG("AMMS::CAMMSAudioVirtualizerControl::~CAMMSAudioVirtualizerControl");

    // Perform DeallocateControl, if the state change has not yet performed it.
    DeallocateControl();
    delete iStereoWideningUtility;
    delete iPresetNames;
}

// -----------------------------------------------------------------------------
// CAMMSAudioVirtualizerControl::SetPresetL
// Sets the effect according to the given preset.
// -----------------------------------------------------------------------------
//
void CAMMSAudioVirtualizerControl::SetPresetL(const TDesC& aPreset)
{
    DEBUG_STR("AMMS::CAMMSAudioVirtualizerControl::SetPresetL \"%S\"",
              aPreset);

    const CDesCArray& presetNames = PresetNamesL();

    TInt presetPosition = 0;
    TInt findPreset = presetNames.Find(aPreset, presetPosition);
    if (findPreset == 0)  // Find returns zero, if a matching element is found.
    {
        // This supposes that the indexing of the presets starts at zero.
        iStereoWideningUtility->GetPresetL(presetPosition);

        // Set the base class audio effect as the new CStereoWidening
        // that is set with the previous GetPresetL method.
        iAudioEffect = &(iStereoWideningUtility->StereoWidening());
        iCurrentPreset = presetPosition;

        DEBUG_STR("AMMS::CAMMSAudioVirtualizerControl::SetPresetL \"%S\" GetPresetL OK",
                  aPreset);
    }
    else
    {
        User::Leave(KErrArgument);
    }
}

// -----------------------------------------------------------------------------
// CAMMSAudioVirtualizerControl::PresetNamesL
// Gets the available preset names.
// -----------------------------------------------------------------------------
//
const CDesCArray& CAMMSAudioVirtualizerControl::PresetNamesL()
{
    // Returns an array of all preset names (pre-defined and user-defined).
    // The pre-defined presets are in the beginning of the list.

    TArray< TEfStereoWideningUtilityPreset > presetNames =
        iStereoWideningUtility->Presets();

    // Before appending the preset names, reset the member array
    iPresetNames->Reset();
    for (TInt i = 0; i < presetNames.Count(); i++)
    {
        iPresetNames->AppendL(presetNames[ i ].iPresetName);
    }

    return *iPresetNames;
}

// -----------------------------------------------------------------------------
// CAMMSAudioVirtualizerControl::PresetL
// Gets the current preset.
// -----------------------------------------------------------------------------
//
const TDesC& CAMMSAudioVirtualizerControl::PresetL()
{
    //if no preset is set, return null
    if (iCurrentPreset < 0)
    {
        return KNullDesC;
    }
    else
    {
        // Retrieves a Preset with the given index from the Central Repository
        return iStereoWideningUtility->GetPresetL(iCurrentPreset);
    }
}

// -----------------------------------------------------------------------------
// CAMMSAudioVirtualizerControl::SetEnabledL
// Enables/disables the effect.
// -----------------------------------------------------------------------------
//
void CAMMSAudioVirtualizerControl::SetEnabledL(TBool aEnable)
{
    if (aEnable)
    {
        // Enable the effect.

        ((CStereoWidening*)iAudioEffect)->SetStereoWideningLevelL(
            KAMMSDefaultStereoWideningLevel);

        // Instead of using CAudioEffectBase: virtual void EnableL(),
        // use the derived effect's (=preset's) ApplyL which calls EnableL.
        DEBUG("AMMS::CAMMSAudioVirtualizerControl::SetEnabledL(true), calling ApplyL");
        iAudioEffect->ApplyL();
    }
    else
    {
        // Disable the effect
        // Instead of using CAudioEffectBase: virtual void DisableL(),
        // use the utility class DisableStereoWideningL,
        // which calls DisableL for the correct preset
        DEBUG("AMMS::CAMMSAudioVirtualizerControl::SetEnabledL(false), calling DisableStereoWideningL");
        iStereoWideningUtility->DisableStereoWideningL();
    }
}

// -----------------------------------------------------------------------------
// CAMMSAudioVirtualizerControl::PrepareControlL
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSAudioVirtualizerControl::PrepareControlL()
{
    // Perform the action only for the first time, skip if called afterwards
    if (!iStereoWideningUtility)
    {
        DEBUG("AMMS::CAMMSAudioVirtualizerControl::PrepareControlL");

        CCustomCommandUtility* customCommandUtility =
            CreateCustomCommandUtilityL();

        // Effect API takes the ownership of customCommandUtility.
        iStereoWideningUtility = CStereoWideningUtility::NewL(
                                     customCommandUtility);

        // Set the base class audio effect as CStereoWidening
        // even the native CStereoWideningUtility has an empty CStereoWidening
        iAudioEffect = &(iStereoWideningUtility->StereoWidening());
    }
}

// -----------------------------------------------------------------------------
// CAMMSAudioVirtualizerControl::DeallocateControl
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSAudioVirtualizerControl::DeallocateControl()
{
    if (iStereoWideningUtility)
    {
        DEBUG("AMMS::CAMMSAudioVirtualizerControl::DeallocateControl");

        // Delete the Effect API class.
        TRAPD(err, iStereoWideningUtility->DisableStereoWideningL());
        if (err != KErrNone)
        {
            // The only even theoritically possible error code here would be
            // KErrAccessDenied which is a result from Effect API calling ApplyL
            // method without having update rights, but since the Utility
            // class is already created, that situation can be discarded here.
        }

        delete iStereoWideningUtility;
        iStereoWideningUtility = NULL;
    }
}

const TDesC& CAMMSAudioVirtualizerControl::ClassName() const
{
    return KAMMSAudioVirtualizerControl;
}

// -----------------------------------------------------------------------------
// CAMMSAudioVirtualizerControl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CAMMSAudioVirtualizerControl::ConstructL()
{
    // Create array for preset names
    iPresetNames = new(ELeave) CDesCArrayFlat(1);

    // Set current preset to a negative value as it is not set yet
    iCurrentPreset = -1;

    CAMMSEffectControl::ConstructL();
}

// -----------------------------------------------------------------------------
// CAMMSAudioVirtualizerControl::CAMMSAudioVirtualizerControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSAudioVirtualizerControl::CAMMSAudioVirtualizerControl(
    CMMAPlayer* aPlayer)
        : CAMMSEffectControl(aPlayer)
{
}

//  End of File
