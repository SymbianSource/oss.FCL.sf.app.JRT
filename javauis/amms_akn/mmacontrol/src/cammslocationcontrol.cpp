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
* Description:  Manipulates the virtual location of an object.
*
*/



// INCLUDE FILES
#include <e32math.h>
#include <jdebug.h>
#include "cammslocationcontrol.h"

#ifdef _DEBUG
// CONSTANTS
const TInt KAMMSMinRadius = 0;
#endif // _DEBUG

// ============================ MEMBER FUNCTIONS ===============================

// Destructor
CAMMSLocationControl::~CAMMSLocationControl()
{
    DEBUG("AMMS::CAMMSLocationControl::~CAMMSLocationControl");
}

// -----------------------------------------------------------------------------
// CAMMSLocationControl::SetLocationCartesianL
// Moves the object to the new location.
// -----------------------------------------------------------------------------
void CAMMSLocationControl::SetLocationCartesianL(
    TInt& aX,
    TInt& aY,
    TInt& aZ)
{
    // Sets the cartesian coordinates for the listener/source location.

    DEBUG_INT3("AMMS::CAMMSLocationControl::SetLocationCartesianL: %d, %d, %d",
               aX, aY, aZ);

    iLocationEffect->SetLocationCartesianL(
        (TInt32&)aX, (TInt32&)aY, (TInt32&)aZ);

    // Apply updated settings to Effect API.
    iLocationEffect->ApplyL();
}

// -----------------------------------------------------------------------------
// CAMMSLocationControl::LocationCartesian
// Gets the coordinates of the current location.
// -----------------------------------------------------------------------------
void CAMMSLocationControl::LocationCartesian(
    TInt& aX, TInt& aY, TInt& aZ)
{
    // Gets the cartesian coordinates for the location of the listener position.
    // The coordinates of the positions are in millimeters.

    iLocationEffect->LocationCartesian(
        (TInt32&)aX, (TInt32&)aY, (TInt32&)aZ);
}


// -----------------------------------------------------------------------------
// CAMMSLocationControl::SetLocationSphericalL
// Moves the object to the new location.
// -----------------------------------------------------------------------------
void CAMMSLocationControl::SetLocationSphericalL(
    TInt& aAzimuth,
    TInt& aElevation,
    TInt& aRadius)
{
    // Check in debug build that aRadius is within valid range.
    __ASSERT_DEBUG(aRadius >= KAMMSMinRadius, User::Invariant());

    // Sets the spherical coordinates for the location of the listener position.
    // The parameters are thousandths of radians
    //
    // NOTE: Effect API uses thousandths of radians for aAzimuth and aElevation,
    // but JSR234 uses degrees.

    DEBUG_INT3("AMMS::CAMMSLocationControl::SetLocationSphericalL: %d, %d, %d",
               aAzimuth, aElevation, aRadius);

    TInt32 convertedAzimuth = (TInt32)(aAzimuth *
                                       KDegToRad * 1000);   // CSI: 47 Effect API uses thousands of radians #

    TInt32 convertedElevation = (TInt32)(aElevation *
                                         KDegToRad * 1000);   // CSI: 47 Effect API uses thousands of radians #

    DEBUG_INT2("AMMS::CAMMSLocationControl::SetLocationSphericalL: %d, %d",
               convertedAzimuth, convertedElevation);

    iLocationEffect->SetLocationSphericalL(
        convertedAzimuth, convertedElevation, (TInt32&)aRadius);

    // Apply updated settings to Effect API.
    iLocationEffect->ApplyL();
}

// -----------------------------------------------------------------------------
// CAMMSLocationControl::CAMMSLocationControl
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSLocationControl::CAMMSLocationControl(CMMAPlayer* aPlayer)
        : CAMMSControl(aPlayer)
{
}

//  End of File
