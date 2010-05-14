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
#include "cammsemcequalizerenvironmentalpresetutility.h"

const TInt KAMMSEMCPresetGranularity = 5;



// ============================ MEMBER FUNCTIONS ===============================
// Constructors

CAMMSEMCEqualizerEnvironmentalPresetUtility::CAMMSEMCEqualizerEnvironmentalPresetUtility()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCEqualizerEnvironmentalPresetUtility:: CAMMSEMCEqualizerEnvironmentalPresetUtility()");
}

// Destructor
CAMMSEMCEqualizerEnvironmentalPresetUtility::~CAMMSEMCEqualizerEnvironmentalPresetUtility()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCEqualizerEnvironmentalPresetUtility::~ +");
    delete iPresetNames;
    if (iEmptyEqualizerUtility)
    {
        TRAPD(err,iEmptyEqualizerUtility ->DisableEqualizerL());
        if (err)
        {
            // The only even theoritically possible error code here would be
            // KErrAccessDenied which is a result from Effect API calling ApplyL
            // method without having update rights, but since the Utility
            // class is already created, that situation can be discarded here.
        }
        delete iEmptyEqualizerUtility;
        iEmptyEqualizerUtility = NULL;
    }
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCEqualizerEnvironmentalPresetUtility::~ -");
}

// -----------------------------------------------------------------------------
// CAMMSEMCEqualizerEnvironmentalPresetUtility::GetPresetAtIndexL
// Gets the current preset.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEMCEqualizerEnvironmentalPresetUtility::GetPresetAtIndexL(TDes& aPreset ,TInt iIndex)
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCEqualizerEnvironmentalPresetUtility::GetPresetAtIndexL +");
    // Return KNullDesC if no preset is set.
    if (iIndex >= 0)
    {

        TArray< TEfAudioEqualizerUtilityPreset > presetNames =
            iEmptyEqualizerUtility->Presets();
        aPreset = presetNames[ iIndex ].iPresetName ;

    }
    else
    {
        aPreset = KNullDesC;
    }

    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCEqualizerEnvironmentalPresetUtility::GetPresetAtIndexL -");
}

// -----------------------------------------------------------------------------
// CAMMSEMCEqualizerEnvironmentalPresetUtility::PrepareEmptyEqualizerUtilitiesL
// Creates utilities that can be used to obtain preset names and preset data.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEMCEqualizerEnvironmentalPresetUtility::PrepareEmptyEqualizerUtilitiesL()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCEqualizerEnvironmentalPresetUtility::PrepareEmptyEqualizerUtilitiesL +");
    //Calling base class function to create and initialize the CMMFDevSound
    CreateAndInitializeDevSoundL();
    if (!iEmptyEqualizerUtility)
    {
        iEmptyEqualizerUtility =
            CAudioEqualizerUtility::NewL(*iMMFDevSound);
    }

    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCEqualizerEnvironmentalPresetUtility::PrepareEmptyEqualizerUtilitiesL -");
}

// -----------------------------------------------------------------------------
// CAMMSEMCEqualizerEnvironmentalPresetUtility::DeleteEmptyEqualizerUtilities
// Deletes utilities that are used to obtain preset names and preset data.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEMCEqualizerEnvironmentalPresetUtility::DeleteEmptyEqualizerUtilities()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCEqualizerEnvironmentalPresetUtility::DeleteEmptyEqualizerUtilities +");

    if (iEmptyEqualizerUtility)
    {
        TRAPD(err,iEmptyEqualizerUtility ->DisableEqualizerL());
        if (err)
        {
            // The only even theoritically possible error code here would be
            // KErrAccessDenied which is a result from Effect API calling ApplyL
            // method without having update rights, but since the Utility
            // class is already created, that situation can be discarded here.
        }
        delete iEmptyEqualizerUtility;
        iEmptyEqualizerUtility = NULL;
    }

    //Calling base class function to delete CMMFDevSound object to free memory
    DeleteDevSound();
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCEqualizerEnvironmentalPresetUtility::DeleteEmptyEqualizerUtilities -");
}


// -----------------------------------------------------------------------------
// CAMMSEMCEqualizerEnvironmentalPresetUtility::GetPresetNamesL
// Gets list of preset names available.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEMCEqualizerEnvironmentalPresetUtility::GetPresetNamesL(
    CDesCArray& aPresetNames)
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCEqualizerEnvironmentalPresetUtility::GetPresetNamesL +");
    TArray< TEfAudioEqualizerUtilityPreset > presetNames =
        iEmptyEqualizerUtility->Presets();
    TInt presetCount = presetNames.Count();
    for (TInt i = 0; i < presetCount; i++)
    {
        aPresetNames.AppendL(presetNames[ i ].iPresetName);
    }
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCEqualizerEnvironmentalPresetUtility::GetPresetNamesL -");
}


// -----------------------------------------------------------------------------
// CAMMSEMCEqualizerEnvironmentalPresetUtility::GetEqualizerAtPresetIndexL
// Gets list of preset names available.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CAudioEqualizer * CAMMSEMCEqualizerEnvironmentalPresetUtility::GetEqualizerAtPresetIndexL(TInt iIndex)
{
    LOG1( EJavaAMMS, EInfo, "AMMS::CAMMSEMCEqualizerEnvironmentalPresetUtility::GetEqualizerAtPresetIndexL, index=%d",
              iIndex);

    __ASSERT_DEBUG(iIndex >= 0, User::Invariant());

    // Set the preset to the empty Environmental Equalizer utility.
    iEmptyEqualizerUtility->GetPresetL(iIndex);
    return &(iEmptyEqualizerUtility->Equalizer());

}


// -----------------------------------------------------------------------------
// CAMMSEMCEqualizerEnvironmentalPresetUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEMCEqualizerEnvironmentalPresetUtility::ConstructL()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCEqualizerEnvironmentalPresetUtility::ConstructL");
    iPresetNames = new(ELeave)CDesCArrayFlat(KAMMSEMCPresetGranularity);
    CAMMSEMCBaseMMFDevSound ::ConstructL();
}

//  End of File
