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
* Description:  Manipulates the virtual location of the SoundSource3D.
*
*/


// INCLUDE FILES
#include <logger.h>
#include <e32math.h>
#include "cammsemcaudiolocationcontrol.h"

#ifdef _DEBUG
// CONSTANTS
const TInt KAMMSMinRadius = 0;
#endif // _DEBUG


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSEMCAudioLocationControl::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSEMCAudioLocationControl* CAMMSEMCAudioLocationControl::NewLC(
    CMMAPlayer* aPlayer)
{
    CAMMSEMCAudioLocationControl* self =
        new(ELeave) CAMMSEMCAudioLocationControl(aPlayer);

    CleanupStack::PushL(self);

    return self;
}

// Destructor
CAMMSEMCAudioLocationControl::~CAMMSEMCAudioLocationControl()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCAudioLocationControl::~CAMMSEMCAudioLocationControl");
    // Perform DeallocateControl, if the state change has not yet performed it.
    DeallocateControl();
}

// -----------------------------------------------------------------------------
// CAMMSEMCAudioLocationControl::PrepareControlL
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSEMCAudioLocationControl::PrepareControlL()
{
    // Perform the action only for the first time, skip if called afterwards
    if (!iMSourceLocationControl)
    {
        LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCAudioLocationControl::PrepareControlL");
        //Create Location Effect Control
        iStreamControl = (static_cast<CMMAEMCAudioPlayer*>(iMMAPlayer))->StreamControl();
        iFactory = (static_cast<CMMAEMCAudioPlayer*>(iMMAPlayer))->MMFactory();
        MEffectControl* temp(NULL);
        User::LeaveIfError(iFactory->CreateEffectControl(KSourceLocationEffectControl, temp));
        iMSourceLocationControl  = static_cast<MSourceLocationControl*>(temp);
        User::LeaveIfError(iStreamControl->AddEffect(*iMSourceLocationControl));
        iMSourceLocationControl->Enable();
    }
}

// -----------------------------------------------------------------------------
// CAMMSEMCAudioLocationControl::DeallocateControl
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSEMCAudioLocationControl::DeallocateControl()
{
    if (iMSourceLocationControl)
    {
        LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCAudioLocationControl::DeallocateControl");

        // Location for Audio can be disabled or enabled
        TRAPD(err,iMSourceLocationControl->Disable());
        if (err != KErrNone)
        {
            // some EMC Error
            ELOG1( EJavaAMMS, "AMMS::CAMMSEMCAudioLocationControl::DeallocateControl err = %d",err);
        }
        //return the control to factory
        MEffectControl* temp = iMSourceLocationControl;
        iStreamControl->RemoveEffect(*temp);
        // Delete the Effect
        MEffectControl* tempCtrl = iMSourceLocationControl;
        iFactory->DeleteEffectControl(tempCtrl);
        iMSourceLocationControl = NULL;
    }
}

const TDesC& CAMMSEMCAudioLocationControl::ClassName() const
{
    return KAMMSEMCAudioLocationControl;
}


// -----------------------------------------------------------------------------
// CAMMSEMCAudioLocationControl::SetLocationCartesianL
// Moves the object to the new location.
// -----------------------------------------------------------------------------
void CAMMSEMCAudioLocationControl::SetLocationCartesianL(
    TInt& aX,
    TInt& aY,
    TInt& aZ)
{
    // Sets the cartesian coordinates for the source location.

    LOG3( EJavaAMMS, EInfo, "AMMS::CAMMSEMCAudioLocationControl::SetLocationCartesianL: %d, %d, %d",
               aX, aY, aZ);

    iMSourceLocationControl->SetLocationCartesian(aX, aY, aZ);

    // Apply updated settings to EMC API.
    iMSourceLocationControl->Apply();
}

// -----------------------------------------------------------------------------
// CAMMSEMCAudioLocationControl::LocationCartesian
// Gets the coordinates of the current location.
// -----------------------------------------------------------------------------
void CAMMSEMCAudioLocationControl::LocationCartesian(
    TInt& aX, TInt& aY, TInt& aZ)
{
    // Gets the cartesian coordinates for the location of the source position.
    // The coordinates of the positions are in millimeters.
    iMSourceLocationControl->LocationCartesian(aX, aY, aZ);
}

// -----------------------------------------------------------------------------
// CAMMSEMCAudioLocationControl::SetLocationSphericalL
// Moves the object to the new location.
// -----------------------------------------------------------------------------
void CAMMSEMCAudioLocationControl::SetLocationSphericalL(
    TInt& aAzimuth,
    TInt& aElevation,
    TInt& aRadius)
{
    // Check in debug build that aRadius is within valid range.
    __ASSERT_DEBUG(aRadius >= KAMMSMinRadius, User::Invariant());

    // Sets the spherical coordinates for the location of the source position.
    // The parameters are thousandths of radians

    LOG3( EJavaAMMS, EInfo, "AMMS::CAMMSEMCAudioLocationControl::SetLocationSphericalL: %d, %d, %d",
               aAzimuth, aElevation, aRadius);

    TInt32 convertedAzimuth = (TInt32)(aAzimuth *
                                       KDegToRad * 1000);

    TInt32 convertedElevation = (TInt32)(aElevation *
                                         KDegToRad * 1000);

    LOG2( EJavaAMMS, EInfo, "AMMS::CAMMSEMCAudioLocationControl::SetLocationSphericalL: %d, %d",
               convertedAzimuth, convertedElevation);

    iMSourceLocationControl->SetLocationSpherical(
        convertedAzimuth, convertedElevation, (TInt32&)aRadius);

    // Apply updated settings to EMC API.
    iMSourceLocationControl->Apply();
}

// -----------------------------------------------------------------------------
// CAMMSEMCAudioLocationControl::CAMMSEMCAudioLocationControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSEMCAudioLocationControl::CAMMSEMCAudioLocationControl(CMMAPlayer* aPlayer)
        : CAMMSLocationControl(aPlayer)
{
    iMMAPlayer = aPlayer;
}

//  End of File
