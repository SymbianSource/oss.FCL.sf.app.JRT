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
#include <logger.h>
#include "cammsdistanceattenuationcontrol.h"

// CONSTANTS
namespace
{
const TInt KAMMSRoomRollOffFactor = 0; // The logical default value set to 0
// Rolloff factor 1000 in AMMS is 100 in Effect API
const TInt KAMMSRolloffDivider = 10;

#ifdef _DEBUG
const TInt KAMMSMinDistance = 1;
const TInt KAMMSMinRollofFactor = 0;
#endif // _DEBUG
}

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSDistanceAttenuationControl::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSDistanceAttenuationControl* CAMMSDistanceAttenuationControl::NewLC(
    CMMAPlayer* aPlayer)
{
    CAMMSDistanceAttenuationControl* self =
        new(ELeave)CAMMSDistanceAttenuationControl(aPlayer);

    CleanupStack::PushL(self);

    return self;
}

// Destructor
CAMMSDistanceAttenuationControl::~CAMMSDistanceAttenuationControl()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSDistanceAttenuationControl::~CAMMSDistanceAttenuationControl");

    // Perform DeallocateControl, if the state change has not yet performed it.
    DeallocateControl();
    delete iDistanceAttenuation;
}

// -----------------------------------------------------------------------------
// CAMMSDistanceAttenuationControl::SetParametersL
// Sets all the 3D audio distance attenuation parameters simultaneously.
// -----------------------------------------------------------------------------
//
void CAMMSDistanceAttenuationControl::SetParametersL(
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
    LOG4( EJavaMMAPI, EInfo, "AMMS::CAMMSDistanceAttenuationControl::SetParametersL %d, %d, %d, %d",
               aMinDistance, aMaxDistance, aMuteAfterMax, aRolloffFactor);

    TInt convertedRolloffFactor = aRolloffFactor / KAMMSRolloffDivider;

    iDistanceAttenuation->SetDistanceAttenuationL(aMinDistance, aMaxDistance,
            aMuteAfterMax, convertedRolloffFactor, KAMMSRoomRollOffFactor);

    // Apply updated settings to Effect API.
    iDistanceAttenuation->ApplyL();
}

// -----------------------------------------------------------------------------
// CAMMSDistanceAttenuationControl::PrepareControlL
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSDistanceAttenuationControl::PrepareControlL()
{
    // Perform the action only for the first time, skip if called afterwards
    if (!iDistanceAttenuation)
    {
        LOG( EJavaAMMS, EInfo, "AMMS::CAMMSDistanceAttenuationControl::PrepareControlL");

        CCustomCommandUtility* customCommandUtility =
            CreateCustomCommandUtilityL();

        // Effect API takes the ownership of customCommandUtility.
        iDistanceAttenuation = CDistanceAttenuation::NewL(
                                   customCommandUtility);

        // Enable the Effect API Control
        iDistanceAttenuation->EnableL();
    }
}

// -----------------------------------------------------------------------------
// CAMMSDistanceAttenuationControl::DeallocateControl
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSDistanceAttenuationControl::DeallocateControl()
{
    if (iDistanceAttenuation)
    {
        LOG( EJavaAMMS, EInfo, "AMMS::CAMMSDistanceAttenuationControl::DeallocateControl");

        // Disable the Effect API Control
        TRAPD(err, iDistanceAttenuation->DisableL());
        if (err != KErrNone)
        {
            // The only even theoritically possible error code here would be
            // KErrAccessDenied which is a result from Effect API calling ApplyL
            // method without having update rights, but since the Effect
            // is already created, that situation can be discarded here.
        }

        // Delete the Effect API class (it deletes CustomCommandUtility)
        delete iDistanceAttenuation;
        iDistanceAttenuation = NULL;
    }
}

const TDesC& CAMMSDistanceAttenuationControl::ClassName() const
{
    return KAMMSDistanceAttenuationControl;
}

// -----------------------------------------------------------------------------
// CAMMSDistanceAttenuationControl::CAMMSDistanceAttenuationControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSDistanceAttenuationControl::CAMMSDistanceAttenuationControl(
    CMMAPlayer* aPlayer)
        : CAMMSBaseDistanceAttenuationControl(aPlayer)
{
}

//  End of File
