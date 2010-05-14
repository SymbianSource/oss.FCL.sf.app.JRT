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
#include <logger.h>
#include "cammsemcaudiovirtualizerenvironmentalpresetutility.h"

const TInt KAMMSEMCPresetGranularity = 5;



// ============================ MEMBER FUNCTIONS ===============================
// Constructors

CAMMSEMCAudioVirtualizerEnvironmentalPresetUtility::CAMMSEMCAudioVirtualizerEnvironmentalPresetUtility()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCAudioVirtualizerEnvironmentalPresetUtility:: constructor");
}

// Destructor
CAMMSEMCAudioVirtualizerEnvironmentalPresetUtility::~CAMMSEMCAudioVirtualizerEnvironmentalPresetUtility()
{
    delete iPresetNames;
    if (iEmptyStereoWideningUtility)
    {
        delete iEmptyStereoWideningUtility;
        iEmptyStereoWideningUtility = NULL;
    }
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCAudioVirtualizerEnvironmentalPresetUtility::~ ");
}

// -----------------------------------------------------------------------------
// CAMMSEMCAudioVirtualizerEnvironmentalPresetUtility::GetPresetAtIndexL
// Gets the current preset.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEMCAudioVirtualizerEnvironmentalPresetUtility::GetPresetAtIndexL(TDes& aPreset ,TInt iIndex)
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCAudioVirtualizerEnvironmentalPresetUtility::GetPresetAtIndexL +");
    // Return KNullDesC if no preset is set.
    if (iIndex >= 0)
    {

        TArray< TEfStereoWideningUtilityPreset > presetNames =
            iEmptyStereoWideningUtility->Presets();
        aPreset = presetNames[ iIndex ].iPresetName ;

    }
    else
    {
        aPreset = KNullDesC;
    }

    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCAudioVirtualizerEnvironmentalPresetUtility::GetPresetAtIndexL -");
}

// -----------------------------------------------------------------------------
// CAMMSEMCAudioVirtualizerEnvironmentalPresetUtility::PrepareEmptyStereoWideningUtilitiesL
// Creates utilities that can be used to obtain preset names and preset data.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEMCAudioVirtualizerEnvironmentalPresetUtility::PrepareEmptyStereoWideningUtilitiesL()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCAudioVirtualizerEnvironmentalPresetUtility::PrepareEmptyStereoWideningUtilitiesL +");
    //Calling base class function to create and initialize the CMMFDevSound
    CreateAndInitializeDevSoundL();
    if (!iEmptyStereoWideningUtility)
    {
        iEmptyStereoWideningUtility =
            CStereoWideningUtility::NewL(*iMMFDevSound);
    }

    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCAudioVirtualizerEnvironmentalPresetUtility::PrepareEmptyStereoWideningUtilitiesL -");
}

// -----------------------------------------------------------------------------
// CAMMSEMCAudioVirtualizerEnvironmentalPresetUtility::DeleteEmptyStereoWideningUtilities
// Deletes utilities that are used to obtain preset names and preset data.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEMCAudioVirtualizerEnvironmentalPresetUtility::DeleteEmptyStereoWideningUtilities()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCAudioVirtualizerEnvironmentalPresetUtility::DeleteEmptyStereoWideningUtilities +");

    if (iEmptyStereoWideningUtility)
    {
        delete iEmptyStereoWideningUtility;
        iEmptyStereoWideningUtility = NULL;
    }
    //Calling base class function to delete CMMFDevSound object to free memory
    DeleteDevSound();
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCAudioVirtualizerEnvironmentalPresetUtility::DeleteEmptyStereoWideningUtilities -");
}


// -----------------------------------------------------------------------------
// CAMMSEMCAudioVirtualizerEnvironmentalPresetUtility::GetPresetNamesL
// Gets list of preset names available.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEMCAudioVirtualizerEnvironmentalPresetUtility::GetPresetNamesL(
    CDesCArray& aPresetNames)
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCAudioVirtualizerEnvironmentalPresetUtility::GetPresetNamesL +");
    TArray< TEfStereoWideningUtilityPreset > presetNames =
        iEmptyStereoWideningUtility->Presets();
    TInt presetCount = presetNames.Count();
    for (TInt i = 0; i < presetCount; i++)
    {
        aPresetNames.AppendL(presetNames[ i ].iPresetName);
    }
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCAudioVirtualizerEnvironmentalPresetUtility::GetPresetNamesL -");
}


// -----------------------------------------------------------------------------
// CAMMSEMCAudioVirtualizerEnvironmentalPresetUtility::GetEnvironmentalStereoWideningAtPresetIndexL
// Gets list of preset names available.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CStereoWidening * CAMMSEMCAudioVirtualizerEnvironmentalPresetUtility::GetStereoWideningAtPresetIndexL(TInt iIndex)
{
    LOG1( EJavaAMMS, EInfo, "AMMS::CAMMSEMCAudioVirtualizerEnvironmentalPresetUtility::GetStereoWideningAtPresetIndexL, index=%d",
              iIndex);

    __ASSERT_DEBUG(iIndex >= 0, User::Invariant());

    // Set the preset to the empty Environmental Reverb utility.
    iEmptyStereoWideningUtility->GetPresetL(iIndex);
    return &(iEmptyStereoWideningUtility->StereoWidening());
}


// -----------------------------------------------------------------------------
// CAMMSEMCAudioVirtualizerEnvironmentalPresetUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEMCAudioVirtualizerEnvironmentalPresetUtility::ConstructL()
{
    iPresetNames = new(ELeave)CDesCArrayFlat(KAMMSEMCPresetGranularity);
    CAMMSEMCBaseMMFDevSound ::ConstructL();
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCAudioVirtualizerEnvironmentalPresetUtility::ConstructL");
}

//  End of File
