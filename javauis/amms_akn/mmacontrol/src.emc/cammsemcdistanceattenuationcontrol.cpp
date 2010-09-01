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
* Description:  Controls how the sound is attenuated with its distance.
*
*/



// INCLUDE FILES
#include <jdebug.h>
#include "cammsemcdistanceattenuationcontrol.h"

// CONSTANTS
namespace
{
const TInt KAMMSRoomRollOffFactor = 0; // The logical default value set to 0
// Rolloff factor 1000 in AMMS is 100 in EMC API
const TInt KAMMSRolloffDivider = 10;

#ifdef _DEBUG
const TInt KAMMSMinDistance = 1;
const TInt KAMMSMinRollofFactor = 0;
#endif // _DEBUG
}

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSEMCDistanceAttenuationControl::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSEMCDistanceAttenuationControl* CAMMSEMCDistanceAttenuationControl::NewLC(
    CMMAPlayer* aPlayer)
{
    CAMMSEMCDistanceAttenuationControl* self =
        new(ELeave)CAMMSEMCDistanceAttenuationControl(aPlayer);

    CleanupStack::PushL(self);

    return self;
}

// Destructor
CAMMSEMCDistanceAttenuationControl::~CAMMSEMCDistanceAttenuationControl()
{
    DEBUG("AMMS::CAMMSEMCDistanceAttenuationControl::~CAMMSEMCDistanceAttenuationControl");

    // Perform DeallocateControl, if the state change has not yet performed it.
    DeallocateControl();

}

// -----------------------------------------------------------------------------
// CAMMSEMCDistanceAttenuationControl::SetParametersL
// Sets all the 3D audio distance attenuation parameters simultaneously.
// -----------------------------------------------------------------------------
//
void CAMMSEMCDistanceAttenuationControl::SetParametersL(
    TInt aMinDistance,
    TInt aMaxDistance,
    TBool aMuteAfterMax,
    TInt aRolloffFactor)
{
    // Check in debug build that parameters are within valid range.
    __ASSERT_DEBUG(
        (aMaxDistance > aMinDistance) &&
        (aMinDistance >= KAMMSMinDistance) &&
        (aMaxDistance >= KAMMSMinDistance) &&
        (aRolloffFactor >= KAMMSMinRollofFactor),
        User::Invariant());

    // NOTE: Effect API uses hundreths (100 corresponds to 1.00),
    // but JSR234 uses thousandths (1000 represents a rolloff factor of 1.0)
    //
    DEBUG_INT4("AMMS::CAMMSEMCDistanceAttenuationControl::SetParametersL %d, %d, %d, %d",
               aMinDistance, aMaxDistance, aMuteAfterMax, aRolloffFactor);

    TInt convertedRolloffFactor = aRolloffFactor / KAMMSRolloffDivider;

    iMDistanceAttenuationControl->SetDistanceAttenuation(aMinDistance, aMaxDistance,
            aMuteAfterMax, convertedRolloffFactor, KAMMSRoomRollOffFactor);

    // Apply updated settings to EMC API.
    iMDistanceAttenuationControl->Apply();
}

// -----------------------------------------------------------------------------
// CAMMSEMCDistanceAttenuationControl::PrepareControlL
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSEMCDistanceAttenuationControl::PrepareControlL()
{
    // Perform the action only for the first time, skip if called afterwards
    if (!iMDistanceAttenuationControl)
    {
        DEBUG("AMMS::CAMMSEMCDistanceAttenuationControl::PrepareControlL");
        //Create Orientation Effect Control
        iStreamControl = (static_cast<CMMAEMCAudioPlayer*>(iMMAPlayer))->StreamControl();
        iFactory = (static_cast<CMMAEMCAudioPlayer*>(iMMAPlayer))->MMFactory();

        MEffectControl* temp(NULL);
        User::LeaveIfError(iFactory->CreateEffectControl(KDistanceAttenuationEffectControl, temp));
        iMDistanceAttenuationControl  = static_cast<MDistanceAttenuationControl*>(temp);
        User::LeaveIfError(iStreamControl->AddEffect(*iMDistanceAttenuationControl));
        iMDistanceAttenuationControl->Enable();

    }

}

// -----------------------------------------------------------------------------
// CAMMSEMCDistanceAttenuationControl::DeallocateControl
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSEMCDistanceAttenuationControl::DeallocateControl()
{
    if (iMDistanceAttenuationControl)
    {
        DEBUG("AMMS::CAMMSEMCDistanceAttenuationControl::DeallocateControl");

        // Location for Audio can be disabled or enabled
        TRAPD(err,iMDistanceAttenuationControl->Disable());
        if (err != KErrNone)
        {
            //Some EMC Error
            DEBUG_INT("AMMS::CAMMSEMCDistanceAttenuationControl::DeallocateControl err = %d",err);
        }
        //return the control to factory
        MEffectControl* temp = iMDistanceAttenuationControl;
        iStreamControl->RemoveEffect(*temp);
        // Delete the Effect
        MEffectControl* tempCtrl = iMDistanceAttenuationControl;
        iFactory->DeleteEffectControl(tempCtrl);
        iMDistanceAttenuationControl = NULL;
    }
}

const TDesC& CAMMSEMCDistanceAttenuationControl::ClassName() const
{
    return KAMMSEMCDistanceAttenuationControl;
}

// -----------------------------------------------------------------------------
// CAMMSEMCDistanceAttenuationControl::CAMMSEMCDistanceAttenuationControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSEMCDistanceAttenuationControl::CAMMSEMCDistanceAttenuationControl(
    CMMAPlayer* aPlayer)
        : CAMMSBaseDistanceAttenuationControl(aPlayer)
{
    iMMAPlayer = aPlayer;
}

//  End of File
