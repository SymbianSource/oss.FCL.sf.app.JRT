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
* Description:  Manipulates the virtual orientation of an object.
*
*/



// INCLUDE FILES
#include <e32math.h>
#include <logger.h>
#include "cammsorientationcontrol.h"


// ============================ MEMBER FUNCTIONS ===============================

// Destructor
CAMMSOrientationControl::~CAMMSOrientationControl()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSOrientationControl::~CAMMSOrientationControl");
}

// -----------------------------------------------------------------------------
// CAMMSOrientationControl::SetOrientationL
// Turns the object to the new orientation.
// -----------------------------------------------------------------------------
void CAMMSOrientationControl::SetOrientationL(
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

    LOG3( EJavaAMMS, EInfo, "AMMS::CAMMSOrientationControl::SetOrientationL: %d, %d, %d",
               aHeading, aPitch, aRoll);

    TInt32 convertedHeading = (TInt32)(aHeading * KDegToRad *
                                       1000);   // CSI: 47 Effect API uses thousands of radians #

    TInt32 convertedPitch = (TInt32)(aPitch * KDegToRad *
                                     1000);   // CSI: 47 Effect API uses thousands of radians #

    TInt32 convertedRoll = (TInt32)(aRoll * KDegToRad *
                                    1000);   // CSI: 47 Effect API uses thousands of radians #

    LOG3( EJavaAMMS, EInfo, "AMMS::CAMMSOrientationControl::SetOrientationL: %d, %d, %d",
               convertedHeading, convertedPitch, convertedRoll);

    iOrientationEffect->SetOrientationL(
        convertedHeading, convertedPitch, convertedRoll);

    // Apply updated settings to Effect API.
    iOrientationEffect->ApplyL();
}

// -----------------------------------------------------------------------------
// CAMMSOrientationControl::SetOrientationL
// Turns the object to the new orientation.
// -----------------------------------------------------------------------------
void CAMMSOrientationControl::SetOrientationL(
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

    LOG3( EJavaAMMS, EInfo, "AMMS::CAMMSOrientationControl::SetOrientationL: Front %d, %d, %d",
               aFrontX, aFrontY, aFrontZ);

    LOG3( EJavaAMMS, EInfo, "AMMS::CAMMSOrientationControl::SetOrientationL: Above %d, %d, %d",
               aAboveX, aAboveY, aAboveZ);

    iOrientationEffect->SetOrientationVectorsL(
        (TInt32)aFrontX, (TInt32)aFrontY, (TInt32)aFrontZ,
        (TInt32)aAboveX, (TInt32)aAboveY, (TInt32)aAboveZ);

    // Apply updated settings to Effect API.
    iOrientationEffect->ApplyL();
}

// -----------------------------------------------------------------------------
// CAMMSOrientationControl::OrientationVectors
// Gets the orientation of the object using two vectors.
// -----------------------------------------------------------------------------
void CAMMSOrientationControl::OrientationVectors(
    TInt& aFrontX, TInt& aFrontY, TInt& aFrontZ,
    TInt& aAboveX, TInt& aAboveY, TInt& aAboveZ)
{
    // Gets the orientation of the source.

    iOrientationEffect->OrientationVectors(
        (TInt32&)aFrontX, (TInt32&)aFrontY, (TInt32&)aFrontZ,
        (TInt32&)aAboveX, (TInt32&)aAboveY, (TInt32&)aAboveZ);
}

// -----------------------------------------------------------------------------
// CAMMSOrientationControl::CAMMSOrientationControl
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSOrientationControl::CAMMSOrientationControl(CMMAPlayer* aPlayer)
        : CAMMSControl(aPlayer)
{
}

//  End of File
