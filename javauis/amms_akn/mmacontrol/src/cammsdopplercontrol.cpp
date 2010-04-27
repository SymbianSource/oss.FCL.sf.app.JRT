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
* Description:  Manipulates the settings of an effect called Doppler.
*
*/



// INCLUDE FILES
#include <e32math.h>
#include <jdebug.h>
#include "cammsdopplercontrol.h"


// ============================ MEMBER FUNCTIONS ===============================

// Destructor
CAMMSDopplerControl::~CAMMSDopplerControl()
{
    DEBUG("AMMS::CAMMSDopplerControl::~CAMMSDopplerControl");
}

// -----------------------------------------------------------------------------
// CAMMSDopplerControl::SetEnabled
// Specifies if this Doppler effect is active or ignored.
// -----------------------------------------------------------------------------
//
void CAMMSDopplerControl::SetEnabledL(TBool aDopplerEnabled)
{
    if (aDopplerEnabled)
    {
        DEBUG("AMMS::CAMMSDopplerControl::SetEnabledL(true)");
        iDopplerEffect->EnableL();
    }
    else
    {
        DEBUG("AMMS::CAMMSDopplerControl::SetEnabledL(false)");
        iDopplerEffect->DisableL();
    }
}


// -----------------------------------------------------------------------------
// CAMMSDopplerControl::Enabled
// Returns whether this Doppler effect is currently active.
// -----------------------------------------------------------------------------
//
TBool CAMMSDopplerControl::Enabled()
{
    return iDopplerEffect->IsEnabled();
}

// -----------------------------------------------------------------------------
// CAMMSDopplerControl::SetVelocityCartesianL
// Sets the velocity, used in calculations for the Doppler effect.
// -----------------------------------------------------------------------------
//
void CAMMSDopplerControl::SetVelocityCartesianL(
    TInt aX,
    TInt aY,
    TInt aZ)
{
    DEBUG_INT3("AMMS::CAMMSDopplerControl::SetVelocityCartesianL: X=%d, Y=%d, Z=%d",
               aX, aY, aZ);

    iDopplerEffect->SetCartesianVelocityL(aX, aY, aZ);

    // Apply updated settings to Effect API.
    iDopplerEffect->ApplyL();
}

// -----------------------------------------------------------------------------
// CAMMSDopplerControl::VelocityCartesian
// Returns the current velocity, used in calculations for the Doppler effect.
// -----------------------------------------------------------------------------
//
void CAMMSDopplerControl::VelocityCartesian(
    TInt& aX, TInt& aY, TInt& aZ)
{
    // Get the velocity's cartesian settings
    // aX, aY and aZ are velocities in format mm/s

    iDopplerEffect->CartesianVelocity(
        (TInt32&)aX, (TInt32&)aY, (TInt32&)aZ);
}

// -----------------------------------------------------------------------------
// CAMMSDopplerControl::SetVelocitySphericalL
// Sets the velocity, used in calculations for the Doppler effect.
// -----------------------------------------------------------------------------
//
void CAMMSDopplerControl::SetVelocitySphericalL(
    TInt aAzimuth,
    TInt aElevation,
    TInt aRadius)
{
    // Parameters are thousandths of radians

    DEBUG_INT3("AMMS::CAMMSDopplerControl::SetVelocitySphericalL %d, %d, %d",
               aAzimuth, aElevation, aRadius);

    TInt32 convertedAzimuth = (TInt32)(aAzimuth * KDegToRad *
                                       1000);   // CSI: 47 Effect API uses thousands of radians #

    TInt32 convertedElevation = (TInt32)(aElevation * KDegToRad *
                                         1000);   // CSI: 47 Effect API uses thousands of radians #

    DEBUG_INT2("AMMS::CAMMSDopplerControl::SetVelocitySphericalL %d, %d",
               convertedAzimuth, convertedElevation);

    iDopplerEffect->SetSphericalVelocityL(
        convertedAzimuth, convertedElevation, aRadius);

    // Apply updated settings to Effect API.
    iDopplerEffect->ApplyL();
}

// -----------------------------------------------------------------------------
// CAMMSDopplerControl::CAMMSDopplerControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSDopplerControl::CAMMSDopplerControl(CMMAPlayer* aPlayer):
        CAMMSControl(aPlayer)
{
}


//  End of File
