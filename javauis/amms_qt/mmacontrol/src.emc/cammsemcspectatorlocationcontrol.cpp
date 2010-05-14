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
#include "cammsemcspectatorlocationcontrol.h"

#ifdef _DEBUG
// CONSTANTS
const TInt KAMMSMinRadius = 0;
#endif // _DEBUG


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSEMCSpectatorLocationControl::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSEMCSpectatorLocationControl* CAMMSEMCSpectatorLocationControl::NewLC(
    CMMAPlayer* aPlayer)
{
    CAMMSEMCSpectatorLocationControl* self =
        new(ELeave) CAMMSEMCSpectatorLocationControl(aPlayer);

    CleanupStack::PushL(self);

    return self;
}

// Destructor
CAMMSEMCSpectatorLocationControl::~CAMMSEMCSpectatorLocationControl()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCSpectatorLocationControl::CAMMSEMCSpectatorLocationControl");
    // Perform DeallocateControl, if the state change has not yet performed it.
    DeallocateControl();
}

// -----------------------------------------------------------------------------
// CAMMSEMCSpectatorLocationControl::PrepareControlL
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSEMCSpectatorLocationControl::PrepareControlL()
{
    // Perform the action only for the first time, skip if called afterwards
    if (!iMListenerLocationControl)
    {
        LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCSpectatorLocationControl::PrepareControlL");
        //Create Location Effect Control
        iStreamControl = (static_cast<CMMAEMCAudioPlayer*>(iMMAPlayer))->StreamControl();
        iFactory = (static_cast<CMMAEMCAudioPlayer*>(iMMAPlayer))->MMFactory();

        MEffectControl* temp(NULL);
        User::LeaveIfError(iFactory->CreateEffectControl(KListenerLocationEffectControl, temp));
        iMListenerLocationControl  = static_cast<MListenerLocationControl*>(temp);
        User::LeaveIfError(iStreamControl->AddEffect(*iMListenerLocationControl));
        iMListenerLocationControl->Enable();
    }
}

// -----------------------------------------------------------------------------
// CAMMSEMCSpectatorLocationControl::DeallocateControl
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSEMCSpectatorLocationControl::DeallocateControl()
{
    if (iMListenerLocationControl)
    {
        LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCSpectatorLocationControl::DeallocateControl");

        // Location for Audio can be disabled or enabled
        iMListenerLocationControl->Disable();
        //return the control to factory
        MEffectControl* temp = iMListenerLocationControl;
        iStreamControl->RemoveEffect(*temp);
        // Delete the Effect
        MEffectControl* tempCtrl = iMListenerLocationControl;
        iFactory->DeleteEffectControl(tempCtrl);
        iMListenerLocationControl = NULL;
    }
}

const TDesC& CAMMSEMCSpectatorLocationControl::ClassName() const
{
    return KAMMSEMCSpectatorLocationControl;
}


// -----------------------------------------------------------------------------
// CAMMSEMCSpectatorLocationControl::SetLocationCartesianL
// Moves the object to the new location.
// -----------------------------------------------------------------------------
void CAMMSEMCSpectatorLocationControl::SetLocationCartesianL(
    TInt& aX,
    TInt& aY,
    TInt& aZ)
{
    // Sets the cartesian coordinates for the source location.

    LOG3( EJavaAMMS, EInfo, "AMMS::CAMMSEMCSpectatorLocationControl::SetLocationCartesianL: %d, %d, %d",
               aX, aY, aZ);

    iMListenerLocationControl->SetLocationCartesian(aX, aY, aZ);

    // Apply updated settings to EMC API.
    iMListenerLocationControl->Apply();
}

// -----------------------------------------------------------------------------
// CAMMSEMCSpectatorLocationControl::LocationCartesian
// Gets the coordinates of the current location.
// -----------------------------------------------------------------------------
void CAMMSEMCSpectatorLocationControl::LocationCartesian(
    TInt& aX, TInt& aY, TInt& aZ)
{
    // Gets the cartesian coordinates for the location of the Listener position.
    // The coordinates of the positions are in millimeters.
    iMListenerLocationControl->LocationCartesian(aX, aY, aZ);
}

// -----------------------------------------------------------------------------
// CAMMSEMCSpectatorLocationControl::SetLocationSphericalL
// Moves the object to the new location.
// -----------------------------------------------------------------------------
void CAMMSEMCSpectatorLocationControl::SetLocationSphericalL(
    TInt& aAzimuth,
    TInt& aElevation,
    TInt& aRadius)
{
    // Check in debug build that aRadius is within valid range.
    __ASSERT_DEBUG(aRadius >= KAMMSMinRadius, User::Invariant());

    // Sets the spherical coordinates for the location of the source position.
    // The parameters are thousandths of radians

    LOG3( EJavaAMMS, EInfo, "AMMS::CAMMSEMCSpectatorLocationControl::SetLocationSphericalL: %d, %d, %d",
               aAzimuth, aElevation, aRadius);

    TInt32 convertedAzimuth = (TInt32)(aAzimuth *
                                       KDegToRad * 1000);

    TInt32 convertedElevation = (TInt32)(aElevation *
                                         KDegToRad * 1000);

    LOG2( EJavaAMMS, EInfo, "AMMS::CAMMSEMCSpectatorLocationControl::SetLocationSphericalL: %d, %d",
               convertedAzimuth, convertedElevation);

    iMListenerLocationControl->SetLocationSpherical(
        convertedAzimuth, convertedElevation, (TInt32&)aRadius);

    // Apply updated settings to EMC API.
    iMListenerLocationControl->Apply();
}

// -----------------------------------------------------------------------------
// CAMMSEMCSpectatorLocationControl::CAMMSEMCSpectatorLocationControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSEMCSpectatorLocationControl::CAMMSEMCSpectatorLocationControl(CMMAPlayer* aPlayer)
        : CAMMSLocationControl(aPlayer)
{
    iMMAPlayer = aPlayer;
}

//  End of File
