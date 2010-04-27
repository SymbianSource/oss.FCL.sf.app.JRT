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
* Description:  Group for effect controls
*
*/


// INCLUDE FILES
#include <jdebug.h>
#include "cammsemcreverbenvironmentalpresetutility.h"

const TInt KAMMSEMCPresetGranularity = 5;



// ============================ MEMBER FUNCTIONS ===============================
// Constructors

CAMMSEMCReverbEnvironmentalPresetUtility::CAMMSEMCReverbEnvironmentalPresetUtility()
{
    DEBUG("AMMS::CAMMSEMCReverbEnvironmentalPresetUtility:: CAMMSEMCReverbEnvironmentalPresetUtility()");
}

// Destructor
CAMMSEMCReverbEnvironmentalPresetUtility::~CAMMSEMCReverbEnvironmentalPresetUtility()
{
    DEBUG("AMMS::CAMMSEMCReverbEnvironmentalPresetUtility::~ +");
    delete iPresetNames;

    if (iEmptyEnvironmentalReverbUtility)
    {
        delete iEmptyEnvironmentalReverbUtility;
        iEmptyEnvironmentalReverbUtility = NULL;
    }
    DEBUG("AMMS::CAMMSEMCReverbEnvironmentalPresetUtility::~ -");
}

// -----------------------------------------------------------------------------
// CAMMSEMCReverbEnvironmentalPresetUtility::GetPresetAtIndexL
// Gets the current preset.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEMCReverbEnvironmentalPresetUtility::GetPresetAtIndexL(TDes& aPreset ,TInt iIndex)
{
    DEBUG("AMMS::CAMMSEMCReverbEnvironmentalPresetUtility::GetPresetAtIndexL +");
    // Return KNullDesC if no preset is set.
    if (iIndex >= 0)
    {

        TArray< TEfEnvironmentalReverbUtilityPreset > presetNames =
            iEmptyEnvironmentalReverbUtility->Presets();
        aPreset = presetNames[ iIndex ].iPresetName ;

    }
    else
    {
        aPreset = KNullDesC;
    }

    DEBUG("AMMS::CAMMSEMCReverbEnvironmentalPresetUtility::GetPresetAtIndexL -");
}

// -----------------------------------------------------------------------------
// CAMMSEMCReverbEnvironmentalPresetUtility::PrepareEmptyReverbUtilitiesL
// Creates utilities that can be used to obtain preset names and preset data.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TInt CAMMSEMCReverbEnvironmentalPresetUtility::PrepareEmptyReverbUtilitiesL()
{
    DEBUG("AMMS::CAMMSEMCReverbEnvironmentalPresetUtility::PrepareEmptyReverbUtilitiesL +");
    //Calling base class function to create and initialize the CMMFDevSound
    TInt err = CreateAndInitializeDevSoundL();
    DEBUG_INT("AMMS::CAMMSEMCReverbEnvironmentalPresetUtility::PrepareEmptyReverbUtilitiesL  err = %d",err);
    if (!iEmptyEnvironmentalReverbUtility && (err == KErrNone))
    {
        iEmptyEnvironmentalReverbUtility =
            CEnvironmentalReverbUtility::NewL(*iMMFDevSound);
    }

    DEBUG("AMMS::CAMMSEMCReverbEnvironmentalPresetUtility::PrepareEmptyReverbUtilitiesL -");
    return err;
}

// -----------------------------------------------------------------------------
// CAMMSEMCReverbEnvironmentalPresetUtility::DeleteEmptyReverbUtilities
// Deletes utilities that are used to obtain preset names and preset data.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEMCReverbEnvironmentalPresetUtility::DeleteEmptyReverbUtilities()
{
    DEBUG("AMMS::CAMMSEMCReverbEnvironmentalPresetUtility::DeleteEmptyReverbUtilities +");

    if (iEmptyEnvironmentalReverbUtility)
    {
        delete iEmptyEnvironmentalReverbUtility;
        iEmptyEnvironmentalReverbUtility = NULL;
    }
    //Calling base class function to delete CMMFDevSound object to free memory
    DeleteDevSound();
    DEBUG("AMMS::CAMMSEMCReverbEnvironmentalPresetUtility::DeleteEmptyReverbUtilities -");
}


// -----------------------------------------------------------------------------
// CAMMSEMCReverbEnvironmentalPresetUtility::MMFDevSound
// Deletes utilities that are used to obtain preset names and preset data.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CMMFDevSound* CAMMSEMCReverbEnvironmentalPresetUtility::MMFDevSound()
{
    DEBUG("AMMS::CAMMSEMCReverbEnvironmentalPresetUtility::MMFDevSound ");
    return iMMFDevSound;
}

// -----------------------------------------------------------------------------
// CAMMSEMCReverbEnvironmentalPresetUtility::GetPresetNamesL
// Gets list of preset names available.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEMCReverbEnvironmentalPresetUtility::GetPresetNamesL(
    CDesCArray& aPresetNames)
{
    DEBUG("AMMS::CAMMSEMCReverbEnvironmentalPresetUtility::GetPresetNamesL +");
    TArray< TEfEnvironmentalReverbUtilityPreset > presetNames =
        iEmptyEnvironmentalReverbUtility->Presets();
    TInt presetCount = presetNames.Count();
    for (TInt i = 0; i < presetCount; i++)
    {
        aPresetNames.AppendL(presetNames[ i ].iPresetName);
    }
    DEBUG("AMMS::CAMMSEMCReverbEnvironmentalPresetUtility::GetPresetNamesL -");
}


// -----------------------------------------------------------------------------
// CAMMSEMCReverbEnvironmentalPresetUtility::GetEnvironmentalReverbAtPresetIndexL
// Gets list of preset names available.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CEnvironmentalReverb * CAMMSEMCReverbEnvironmentalPresetUtility::GetEnvironmentalReverbAtPresetIndexL(TInt iIndex)
{
    DEBUG_INT("AMMS::CAMMSEMCReverbEnvironmentalPresetUtility::GetEnvironmentalReverbAtPresetIndexL, index=%d",
              iIndex);

    __ASSERT_DEBUG(iIndex >= 0, User::Invariant());

    // Set the preset to the empty Environmental Reverb utility.
    iEmptyEnvironmentalReverbUtility->GetPresetL(iIndex);
    return &(iEmptyEnvironmentalReverbUtility->EnvironmentalReverb());

}


// -----------------------------------------------------------------------------
// CAMMSEMCReverbEnvironmentalPresetUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEMCReverbEnvironmentalPresetUtility::ConstructL()
{
    DEBUG("AMMS::CAMMSEMCReverbEnvironmentalPresetUtility::ConstructL ");
    iPresetNames = new(ELeave)CDesCArrayFlat(KAMMSEMCPresetGranularity);
    CAMMSEMCBaseMMFDevSound ::ConstructL();
}

//  End of File
