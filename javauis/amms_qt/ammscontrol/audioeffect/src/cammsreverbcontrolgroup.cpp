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
* Description:  Group for reverb controls
*
*/


// INCLUDE FILES
#include <EnvironmentalReverbUtility.h>
#include <EnvironmentalReverbUtilityData.h>
#include <logger.h>

#include "cammsreverbcontrolgroup.h"
#include "cammsbasereverbcontrol.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSReverbControlGroup::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSReverbControlGroup* CAMMSReverbControlGroup::NewLC()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSReverbControlGroup::NewLC +");

    CAMMSReverbControlGroup* self = new(ELeave) CAMMSReverbControlGroup;

    CleanupStack::PushL(self);
    self->ConstructL();

    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSReverbControlGroup::NewLC -");

    return self;
}

// Destructor
CAMMSReverbControlGroup::~CAMMSReverbControlGroup()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSReverbControlGroup::~");

    delete iEmptyEnvironmentalReverbUtility;
}

// -----------------------------------------------------------------------------
// CAMMSReverbControlGroup::ReverbLevel
// Gets the gain level of the reverberation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TInt CAMMSReverbControlGroup::ReverbLevel()
{
    return iReverbLevel;
}

// -----------------------------------------------------------------------------
// CAMMSReverbControlGroup::ReverbTime
// Gets the reverberation time, as set either explicitly via
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TInt CAMMSReverbControlGroup::ReverbTime()
{
    return iReverbTime;
}

// -----------------------------------------------------------------------------
// CAMMSReverbControlGroup::SetReverbLevelL
// Sets the gain level of the reverberation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSReverbControlGroup::SetReverbLevelL(TInt aLevel)
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSReverbControlGroup::SetReverbLevelL");

    __ASSERT_DEBUG(aLevel <= 0, User::Invariant());

    // Set the level between the accepted limits [iReverbMinLevel,
    // iReverbMaxLevel].
    TInt reverbLevel = Min(aLevel, iReverbMaxLevel);
    reverbLevel = Max(reverbLevel, iReverbMinLevel);

    // Set reverb level to the controls.
    TInt count = ControlCount();
    for (TInt i = 0; i < count; i++)
    {
        TypeSafeControl(i)->SetReverbLevelL(reverbLevel);
    }

    // Invalidate current preset and save the new reverb level
    iPresetIndex = -1;
    iReverbLevel = reverbLevel;

    LOG4( EJavaMMAPI, EInfo, "CAMMSReverbControlGroup::SetReverbLevelL, levels: %d, %d, %d %d",
        aLevel, iReverbMinLevel, iReverbMaxLevel, reverbLevel);
}

// -----------------------------------------------------------------------------
// CAMMSReverbControlGroup::SetReverbTimeL
// Sets the reverberation time of the reverb
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSReverbControlGroup::SetReverbTimeL(TInt aTime)
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSReverbControlGroup::SetReverbTimeL +");

    __ASSERT_DEBUG(aTime >= 0, User::Invariant());

    // Set reverb time to controls.
    TInt count = ControlCount();
    for (TInt i = 0; i < count; i++)
    {
        TypeSafeControl(i)->SetReverbTimeL(aTime);
    }

    // Invalidate current preset and save new reverb time
    iPresetIndex = -1;
    iReverbTime = aTime;

    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSReverbControlGroup::SetReverbTimeL -");
}

// -----------------------------------------------------------------------------
// CAMMSReverbControlGroup::TypeSafeControl
// Gets control. Ownership is not tranferred.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CAMMSBaseReverbControl*
CAMMSReverbControlGroup::TypeSafeControl(TInt aIndex) const
{
    return static_cast<CAMMSBaseReverbControl*>(Control(aIndex));
}

// -----------------------------------------------------------------------------
// CAMMSReverbControlGroup::ClassName
// Returns class name that identifies this control group.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
const TDesC16& CAMMSReverbControlGroup::ClassName()
{
    return KAMMSReverbControlClassName;
}

// -----------------------------------------------------------------------------
// CAMMSReverbControlGroup::NotifyPlayerAddedL
// Called by when a new player is added
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSReverbControlGroup::NotifyPlayerAddedL(
    CMMAPlayer *aPlayer,
    CMMAControl* aControl)
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSReverbControlGroup::NotifyPlayerAddedL +");

    CAMMSEffectControlGroup::NotifyPlayerAddedL(aPlayer, aControl);

    CAMMSBaseReverbControl* control =
        static_cast<CAMMSBaseReverbControl*>(aControl);

    // if the preset is not valid, set reverb level and time to the added
    // control
    if (iPresetIndex < 0)
    {
        control->SetReverbLevelL(iReverbLevel);
        control->SetReverbTimeL(iReverbTime);
    }

    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSReverbControlGroup::NotifyPlayerAddedL -");
}

// -----------------------------------------------------------------------------
// CAMMSReverbControlGroup::PresetChangedL
// Called when the current preset changes
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSReverbControlGroup::PresetChangedL()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSReverbControlGroup::PresetChangedL +");

    TInt count = ControlCount();

    LOG1( EJavaAMMS, EInfo, "AMMS::CAMMSReverbControlGroup::PresetChangedL, controls=%d",
              count);

    // Ask preset data from the first control if exists. If not, ask the
    // data from the empty group utility.
    if (count > 0)
    {
        CAMMSBaseReverbControl* control = TypeSafeControl(0);

        iReverbTime = control->ReverbTime();
        iReverbLevel = control->ReverbLevel();
        iReverbMinLevel = control->MinReverbLevel();
        iReverbMaxLevel = control->MaxReverbLevel();
    }
    else
    {
        LOG1( EJavaAMMS, EInfo, "AMMS::CAMMSReverbControlGroup::PresetChangedL, index=%d",
                  iPresetIndex);

        __ASSERT_DEBUG(iPresetIndex >= 0, User::Invariant());

        // Create empty group utilities for getting preset data.
        PrepareEmptyGroupUtilitiesL();

        // Set the preset to the empty group utility.
        iEmptyEnvironmentalReverbUtility->GetPresetL(iPresetIndex);

        CEnvironmentalReverb& audioEffect =
            iEmptyEnvironmentalReverbUtility->EnvironmentalReverb();


        iReverbTime = audioEffect.DecayTime();
        iReverbLevel =
            audioEffect.ReflectionsLevel() + audioEffect.RoomLevel();

        TInt32 minLevel;
        TInt32 maxLevel;
        audioEffect.ReverbLevelRange(minLevel, maxLevel);

        iReverbMinLevel = minLevel;
        iReverbMaxLevel = maxLevel;

        // Delete empty group utilities in order to save memory.
        DeleteEmptyGroupUtilities();
    }

    LOG4( EJavaAMMS, EInfo, "CAMMSReverbControlGroup::PresetChangedL, values: %d, %d, %d %d",
        iReverbTime, iReverbLevel, iReverbMinLevel, iReverbMaxLevel);

    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSReverbControlGroup::PresetChangedL -");
}

// -----------------------------------------------------------------------------
// CAMMSReverbControlGroup::InitializeL
// Finish initialization (after the 1st player is added)
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSReverbControlGroup::InitializeL()
{
}

// -----------------------------------------------------------------------------
// CAMMSReverbControlGroup::PrepareEmptyGroupUtilitiesL
// Creates utilities that can be used to obtain preset names and preset data.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSReverbControlGroup::PrepareEmptyGroupUtilitiesL()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSReverbControlGroup::PrepareEmptyGroupUtilitiesL +");

    if (!iEmptyEnvironmentalReverbUtility)
    {
        CAMMSEffectControlGroup::PrepareEmptyGroupUtilitiesL();

        iEmptyEnvironmentalReverbUtility =
            CEnvironmentalReverbUtility::NewL(*iEmptyPlayerUtility);
    }

    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSReverbControlGroup::PrepareEmptyGroupUtilitiesL -");
}

// -----------------------------------------------------------------------------
// CAMMSReverbControlGroup::DeleteEmptyGroupUtilities
// Deletes utilities that are used to obtain preset names and preset data.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSReverbControlGroup::DeleteEmptyGroupUtilities()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSReverbControlGroup::DeleteEmptyGroupUtilities +");

    if (iEmptyPlayerUtility)
    {
        delete iEmptyEnvironmentalReverbUtility;
        iEmptyEnvironmentalReverbUtility = NULL;

        CAMMSEffectControlGroup::DeleteEmptyGroupUtilities();
    }

    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSReverbControlGroup::DeleteEmptyGroupUtilities -");
}

// -----------------------------------------------------------------------------
// CAMMSReverbControlGroup::GetPresetNamesL
// Gets list of preset names available.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSReverbControlGroup::GetPresetNamesL(
    CDesCArray& aPresetNames)
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSReverbControlGroup::GetPresetNamesL +");

    // Create empty group utilities for getting preset names.
    PrepareEmptyGroupUtilitiesL();

    TArray< TEfEnvironmentalReverbUtilityPreset > presetNames =
        iEmptyEnvironmentalReverbUtility->Presets();


    TInt presetCount = presetNames.Count();

    for (TInt i = 0; i < presetCount; i++)
    {
        aPresetNames.AppendL(presetNames[ i ].iPresetName);
    }

    // Delete empty group utilities in order to save memory.
    DeleteEmptyGroupUtilities();

    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSReverbControlGroup::GetPresetNamesL -");
}

// -----------------------------------------------------------------------------
// CAMMSReverbControlGroup::CAMMSReverbControlGroup
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
CAMMSReverbControlGroup::CAMMSReverbControlGroup()
        : CAMMSEffectControlGroup(KAMMSBaseReverbControl)
{
}

// -----------------------------------------------------------------------------
// CAMMSReverbControlGroup::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CAMMSReverbControlGroup::ConstructL()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSReverbControlGroup::ConstructL +");

    CAMMSEffectControlGroup::BaseConstructL();

    TRAPD(err, SetPresetL(KAMMSBaseDefaultReverbPreset));

    // The following code prevents build warning.
    if (err != KErrNone)
    {
        ELOG1( EJavaAMMS, "AMMS::CAMMSReverbControlGroup::ConstructL, err %d", err);
    }

    // Ignore the error so that using MMA and AMMS is possible.
    // Error is visible so that "smallroom" preset is not activated.

    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSReverbControlGroup::ConstructL -");
}

//  End of File
