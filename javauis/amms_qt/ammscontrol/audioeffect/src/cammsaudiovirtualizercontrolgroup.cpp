/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Group for reverb controls
*
*/


// INCLUDE FILES
#include <StereoWideningUtility.h>
#include <StereoWideningUtilityData.h>
#include <logger.h>
#include "cammsaudiovirtualizercontrolgroup.h"
#include "cammsaudiovirtualizercontrol.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSAudioVirtualizerControlGroup::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSAudioVirtualizerControlGroup* CAMMSAudioVirtualizerControlGroup::NewLC()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSAudioVirtualizerControlGroup::NewLC +");

    CAMMSAudioVirtualizerControlGroup* self =
        new(ELeave) CAMMSAudioVirtualizerControlGroup;

    CleanupStack::PushL(self);
    self->BaseConstructL();

    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSAudioVirtualizerControlGroup::NewLC -");

    return self;
}

// Destructor
CAMMSAudioVirtualizerControlGroup::~CAMMSAudioVirtualizerControlGroup()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSAudioVirtualizerControlGroup::~ +");

    delete iEmptyStereoWideningUtility;

    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSAudioVirtualizerControlGroup::~ -");
}

// -----------------------------------------------------------------------------
// CAMMSAudioVirtualizerControlGroup::ClassName
// Returns class name that identifies this control group.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
const TDesC16& CAMMSAudioVirtualizerControlGroup::ClassName()
{
    return KAMMSAudioVirtualizerControlClassName;
}

// -----------------------------------------------------------------------------
// CAMMSAudioVirtualizerControlGroup::NotifyPlayerAddedL
// Called by when a new player is added
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSAudioVirtualizerControlGroup::NotifyPlayerAddedL(
    CMMAPlayer *aPlayer,
    CMMAControl* aControl)
{
    CAMMSEffectControlGroup::NotifyPlayerAddedL(aPlayer, aControl);
}

// -----------------------------------------------------------------------------
// CAMMSAudioVirtualizerControlGroup::PresetChangedL
// Called when the current preset changes
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSAudioVirtualizerControlGroup::PresetChangedL()
{
}

// -----------------------------------------------------------------------------
// CAMMSAudioVirtualizerControlGroup::InitializeL
// Finish initialization (after the 1st player is added)
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSAudioVirtualizerControlGroup::InitializeL()
{
}

// -----------------------------------------------------------------------------
// CAMMSAudioVirtualizerControlGroup::PrepareEmptyGroupUtilitiesL
// Creates utilities that can be used to obtain preset names and preset data.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSAudioVirtualizerControlGroup::PrepareEmptyGroupUtilitiesL()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSAudioVirtualizerControlGroup::PrepareEmptyGroupUtilitiesL +");

    if (!iEmptyStereoWideningUtility)
    {
        CAMMSEffectControlGroup::PrepareEmptyGroupUtilitiesL();

        iEmptyStereoWideningUtility =
            CStereoWideningUtility::NewL(*iEmptyPlayerUtility);
    }

    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSAudioVirtualizerControlGroup::PrepareEmptyGroupUtilitiesL -");
}

// -----------------------------------------------------------------------------
// CAMMSAudioVirtualizerControlGroup::DeleteEmptyGroupUtilities
// Deletes utilities that are used to obtain preset names and preset data.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSAudioVirtualizerControlGroup::DeleteEmptyGroupUtilities()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSAudioVirtualizerControlGroup::DeleteEmptyGroupUtilities +");

    if (iEmptyPlayerUtility)
    {
        delete iEmptyStereoWideningUtility;
        iEmptyStereoWideningUtility = NULL;

        CAMMSEffectControlGroup::DeleteEmptyGroupUtilities();
    }

    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSAudioVirtualizerControlGroup::DeleteEmptyGroupUtilities -");
}

// -----------------------------------------------------------------------------
// CAMMSAudioVirtualizerControlGroup::GetPresetNamesL
// Gets list of preset names available.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSAudioVirtualizerControlGroup::GetPresetNamesL(
    CDesCArray& aPresetNames)
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSAudioVirtualizerControlGroup::GetPresetNamesL +");

    // Create empty group utilities for getting preset names.
    PrepareEmptyGroupUtilitiesL();

    TArray< TEfStereoWideningUtilityPreset > presetNames =
        iEmptyStereoWideningUtility->Presets();


    TInt presetCount = presetNames.Count();

    for (TInt i = 0; i < presetCount; i++)
    {
        aPresetNames.AppendL(presetNames[ i ].iPresetName);
    }


    // Delete empty group utilities in order to save memory.
    DeleteEmptyGroupUtilities();

    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSAudioVirtualizerControlGroup::GetPresetNamesL -");
}

// -----------------------------------------------------------------------------
// CAMMSAudioVirtualizerControlGroup::CAMMSAudioVirtualizerControlGroup
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
CAMMSAudioVirtualizerControlGroup::CAMMSAudioVirtualizerControlGroup()
        : CAMMSEffectControlGroup(KAMMSAudioVirtualizerControl)
{
}

// -----------------------------------------------------------------------------
// CAMMSAudioVirtualizerControlGroup::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CAMMSAudioVirtualizerControlGroup::ConstructL()
{
    CAMMSEffectControlGroup::BaseConstructL();
}

//  End of File
