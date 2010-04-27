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
* Description:  Manipulates the virtual orientation of the spectator.
*
*/



// INCLUDE FILES
#include <jdebug.h>
#include <e32math.h>
#include "cammsemcspectatororientationcontrol.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSEMCSpectatorOrientationControl::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSEMCSpectatorOrientationControl* CAMMSEMCSpectatorOrientationControl::NewLC(
    CMMAPlayer* aPlayer)
{
    CAMMSEMCSpectatorOrientationControl* self =
        new(ELeave)CAMMSEMCSpectatorOrientationControl(aPlayer);

    CleanupStack::PushL(self);

    return self;
}

// Destructor
CAMMSEMCSpectatorOrientationControl::~CAMMSEMCSpectatorOrientationControl()
{
    DEBUG("AMMS::CAMMSEMCSpectatorOrientationControl::~CAMMSEMCSpectatorOrientationControl");

    // Perform DeallocateControl, if the state change has not yet performed it.
    DeallocateControl();
}

// -----------------------------------------------------------------------------
// CAMMSEMCSpectatorOrientationControl::PrepareControlL
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSEMCSpectatorOrientationControl::PrepareControlL()
{
    // Perform the action only for the first time, skip if called afterwards
    if (!iMListenerOrientationControl)
    {
        DEBUG("AMMS::CAMMSEMCSpectatorOrientationControl::PrepareControlL");
        //Create Orientation Effect Control
        iStreamControl = (static_cast<CMMAEMCAudioPlayer*>(iMMAPlayer))->StreamControl();
        iFactory = (static_cast<CMMAEMCAudioPlayer*>(iMMAPlayer))->MMFactory();

        MEffectControl* temp(NULL);
        User::LeaveIfError(iFactory->CreateEffectControl(KListenerOrientationEffectControl, temp));
        iMListenerOrientationControl  = static_cast<MListenerOrientationControl*>(temp);
        User::LeaveIfError(iStreamControl->AddEffect(*iMListenerOrientationControl));
        iMListenerOrientationControl->Enable();
    }
}

// -----------------------------------------------------------------------------
// CAMMSEMCSpectatorOrientationControl::DeallocateControl
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSEMCSpectatorOrientationControl::DeallocateControl()
{
    if (iMListenerOrientationControl)
    {
        DEBUG("AMMS::CAMMSEMCSpectatorOrientationControl::DeallocateControl");

        // Orientation for Listener can be disabled or enabled
        iMListenerOrientationControl->Disable();
        //return the control to factory
        MEffectControl* temp = iMListenerOrientationControl;
        iStreamControl->RemoveEffect(*temp);
        // Delete the Effect
        MEffectControl* tempCtrl = iMListenerOrientationControl;
        iFactory->DeleteEffectControl(tempCtrl);
        iMListenerOrientationControl = NULL;
    }
}

const TDesC& CAMMSEMCSpectatorOrientationControl::ClassName() const
{
    return KAMMSEMCSpectatorOrientationControl;
}


// -----------------------------------------------------------------------------
// CAMMSEMCSpectatorOrientationControl::SetOrientationL
// Turns the object to the new orientation.
// -----------------------------------------------------------------------------
void CAMMSEMCSpectatorOrientationControl::SetOrientationL(
    TInt aHeading,
    TInt aPitch,
    TInt aRoll)
{
    // Sets the Heading, Pitch, Roll values for the source/listener orientation.
    // Parameters are given in thousandths of radians.
    // SourceOrientationBase/ListenerLocationBase :
    //
    // NOTE: Effect API uses thousandths of radians for all three parameters,
    // but JSR234 uses degrees.
    //
    // From e32Math.h: The multiplying factor to convert degrees to radians.

    DEBUG_INT3("AMMS::CAMMSEMCSpectatorOrientationControl::SetOrientationL: %d, %d, %d",
               aHeading, aPitch, aRoll);

    TInt32 convertedHeading = (TInt32)(aHeading * KDegToRad *
                                       1000);   // CSI: 47 Effect API uses thousands of radians #

    TInt32 convertedPitch = (TInt32)(aPitch * KDegToRad *
                                     1000);   // CSI: 47 Effect API uses thousands of radians #

    TInt32 convertedRoll = (TInt32)(aRoll * KDegToRad *
                                    1000);   // CSI: 47 Effect API uses thousands of radians #

    DEBUG_INT3("AMMS::CAMMSEMCSpectatorOrientationControl::SetOrientationL: %d, %d, %d",
               convertedHeading, convertedPitch, convertedRoll);

    TInt err = iMListenerOrientationControl->SetOrientation(
                   convertedHeading, convertedPitch, convertedRoll);
    DEBUG_INT("AMMS::CAMMSEMCSpectatorOrientationControl::SetOrientationL(3 arg.) err = %d",err);


    // Apply updated settings to EMC API.
    iMListenerOrientationControl->Apply();
}

// -----------------------------------------------------------------------------
// CAMMSEMCSpectatorOrientationControl::SetOrientationL
// Turns the object to the new orientation.
// -----------------------------------------------------------------------------
void CAMMSEMCSpectatorOrientationControl::SetOrientationL(
    TInt aFrontX, TInt aFrontY, TInt aFrontZ,
    TInt aAboveX, TInt aAboveY, TInt aAboveZ)
{
    // Check in debug build that parameters are not zero vectors.
    __ASSERT_DEBUG(((aFrontX != 0) ||
                    (aFrontY != 0) ||
                    (aFrontZ != 0)) &&
                   ((aAboveX != 0) ||
                    (aAboveY != 0) ||
                    (aAboveZ != 0)),
                   User::Invariant());

    // Check in debug build that vectors are not parallel.
    // Two vectors are parallel if their cross product is zero vector.
    // Cross product of vectors a1*i + a2*j + a3*k and b1*i + b2*j + b3*k :
    // (a2*b3 - a3*b2)i + (a3*b1 - a1*b3)j + (a1*b2 - a2*b1)k
    __ASSERT_DEBUG(
        ((aFrontY * aAboveZ) - (aFrontZ * aAboveY) != 0) ||
        ((aFrontZ * aAboveX) - (aFrontX * aAboveZ) != 0) ||
        ((aFrontX * aAboveY) - (aFrontY * aAboveX) != 0),
        User::Invariant());

    // Sets the Front and Above vectors for the orientation of the source/listener.

    DEBUG_INT3("AMMS::CAMMSEMCSpectatorOrientationControl::SetOrientationL: Front %d, %d, %d",
               aFrontX, aFrontY, aFrontZ);

    DEBUG_INT3("AMMS::CAMMSEMCSpectatorOrientationControl::SetOrientationL: Above %d, %d, %d",
               aAboveX, aAboveY, aAboveZ);

    TInt err = iMListenerOrientationControl->SetOrientationVectors(
                   (TInt32)aFrontX, (TInt32)aFrontY, (TInt32)aFrontZ,
                   (TInt32)aAboveX, (TInt32)aAboveY, (TInt32)aAboveZ);
    DEBUG_INT("AMMS::CAMMSEMCSpectatorOrientationControl::SetOrientationL(6 arg.) err = %d",err);
    // Apply updated settings to EMC API.
    iMListenerOrientationControl->Apply();
}

// -----------------------------------------------------------------------------
// CAMMSEMCSpectatorOrientationControl::OrientationVectors
// Gets the orientation of the object using two vectors.
// -----------------------------------------------------------------------------
void CAMMSEMCSpectatorOrientationControl::OrientationVectors(
    TInt& aFrontX, TInt& aFrontY, TInt& aFrontZ,
    TInt& aAboveX, TInt& aAboveY, TInt& aAboveZ)
{
    // Gets the orientation of the source.

    iMListenerOrientationControl->OrientationVectors(
        aFrontX, aFrontY, aFrontZ, aAboveX, aAboveY, aAboveZ);
}

// -----------------------------------------------------------------------------
// CAMMSEMCSpectatorOrientationControl::CAMMSEMCSpectatorOrientationControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSEMCSpectatorOrientationControl::CAMMSEMCSpectatorOrientationControl(
    CMMAPlayer* aPlayer)
        : CAMMSOrientationControl(aPlayer)
{
    iMMAPlayer = aPlayer;
}

//  End of File
