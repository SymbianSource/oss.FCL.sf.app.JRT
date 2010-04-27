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
* Description:  Manipulates the doppler effect of the spectator.
*
*/



// INCLUDE FILES
#include <jdebug.h>
#include <e32math.h>
#include "cammsemcspectatordopplercontrol.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSEMCSpectatorDopplerControl::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSEMCSpectatorDopplerControl* CAMMSEMCSpectatorDopplerControl::NewLC(
    CMMAPlayer* aPlayer)
{
    CAMMSEMCSpectatorDopplerControl* self =
        new(ELeave)CAMMSEMCSpectatorDopplerControl(aPlayer);

    CleanupStack::PushL(self);

    return self;
}

// Destructor
CAMMSEMCSpectatorDopplerControl::~CAMMSEMCSpectatorDopplerControl()
{
    DEBUG("AMMS::CAMMSEMCSpectatorDopplerControl::~CAMMSEMCSpectatorDopplerControl");

    // Perform DeallocateControl, if the state change has not yet performed it.
    DeallocateControl();
}

// -----------------------------------------------------------------------------
// CAMMSEMCSpectatorDopplerControl::PrepareControlL
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSEMCSpectatorDopplerControl::PrepareControlL()
{
    // Perform the action only for the first time, skip if called afterwards
    if (!iMListenerDopplerControl)
    {
        DEBUG("AMMS::CAMMSEMCSpectatorDopplerControl::PrepareControlL");
        iStreamControl = (static_cast<CMMAEMCAudioPlayer*>(iMMAPlayer))->StreamControl();
        iFactory = (static_cast<CMMAEMCAudioPlayer*>(iMMAPlayer))->MMFactory();
        MEffectControl* temp(NULL);
        User::LeaveIfError(iFactory->CreateEffectControl(KListenerDopplerEffectControl, temp));
        iMListenerDopplerControl  = static_cast<MListenerDopplerControl*>(temp);
        User::LeaveIfError(iStreamControl->AddEffect(*iMListenerDopplerControl));
        TInt err = iMListenerDopplerControl->Enable();
        DEBUG_INT("AMMS::CAMMSEMCSpectatorDopplerControl::PrepareControlL: EnableErr = %d", err);
    }
}

// -----------------------------------------------------------------------------
// CAMMSEMCSpectatorDopplerControl::DeallocateControl
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSEMCSpectatorDopplerControl::DeallocateControl()
{
    if (iMListenerDopplerControl)
    {
        DEBUG("AMMS::CAMMSEMCSpectatorDopplerControl::DeallocateControl");

        // Doppler for Spectator is always enabled (JSR-234 mandates it)
        // so the disabling is needed here
        iMListenerDopplerControl->Disable();
        //return the control to factory
        MEffectControl* temp = iMListenerDopplerControl;
        iStreamControl->RemoveEffect(*temp);
        // Delete the Effect
        MEffectControl* tempCtrl = iMListenerDopplerControl;
        iFactory->DeleteEffectControl(tempCtrl);
        iMListenerDopplerControl = NULL;
    }
}

const TDesC& CAMMSEMCSpectatorDopplerControl::ClassName() const
{
    return KAMMSEMCListenerDopplerControl;
}


void CAMMSEMCSpectatorDopplerControl::SetEnabledL(TBool aDopplerEnabled)
{
    if (aDopplerEnabled)
    {
        TInt err = iMListenerDopplerControl->Enable();
        DEBUG_INT("AMMS::CAMMSEMCSpectatorDopplerControl::SetEnabledL: Enable err = %d", err);
    }
    else
    {
        TInt err = iMListenerDopplerControl->Disable();
        DEBUG_INT("AMMS::CAMMSEMCSpectatorDopplerControl::SetEnabledL: Disable err = %d", err);
    }
}


// -----------------------------------------------------------------------------
// CAMMSEMCSpectatorDopplerControl::Enabled
// Returns whether this Doppler effect is currently active.
// -----------------------------------------------------------------------------
//
TBool CAMMSEMCSpectatorDopplerControl::Enabled()
{
    TBool temp;
    TInt err = iMListenerDopplerControl->IsEnabled(temp);
    DEBUG_INT("AMMS::CAMMSEMCSpectatorDopplerControl::Enabled: err = %d", err);
    return temp;
}

// -----------------------------------------------------------------------------
// CAMMSEMCSpectatorDopplerControl::SetVelocityCartesianL
// Sets the velocity, used in calculations for the Doppler effect.
// -----------------------------------------------------------------------------
//
void CAMMSEMCSpectatorDopplerControl::SetVelocityCartesianL(
    TInt aX,
    TInt aY,
    TInt aZ)
{
    DEBUG_INT3("AMMS::CAMMSEMCSpectatorDopplerControl::SetVelocityCartesianL: X=%d, Y=%d, Z=%d",
               aX, aY, aZ);
    TInt err = iMListenerDopplerControl->SetCartesianVelocity(aX, aY, aZ);

    DEBUG_INT("AMMS::CAMMSEMCSpectatorDopplerControl::SetVelocityCartesianL: err = %d", err);
    // Apply updated settings
    iMListenerDopplerControl->Apply();

}

// -----------------------------------------------------------------------------
// CAMMSEMCSpectatorDopplerControl::VelocityCartesian
// Returns the current velocity, used in calculations for the Doppler effect.
// -----------------------------------------------------------------------------
//
void CAMMSEMCSpectatorDopplerControl::VelocityCartesian(
    TInt& aX, TInt& aY, TInt& aZ)
{
    // Get the velocity's cartesian settings
    // aX, aY and aZ are velocities in format mm/s

    TInt err = iMListenerDopplerControl->CartesianVelocity(aX,aY,aZ);
    DEBUG_INT("AMMS::CAMMSEMCSpectatorDopplerControl::VelocityCartesian: err = %d", err);
}

// -----------------------------------------------------------------------------
// CAMMSEMCSpectatorDopplerControl::SetVelocitySphericalL
// Sets the velocity, used in calculations for the Doppler effect.
// -----------------------------------------------------------------------------
//
void CAMMSEMCSpectatorDopplerControl::SetVelocitySphericalL(
    TInt aAzimuth,
    TInt aElevation,
    TInt aRadius)
{
    // Parameters are thousandths of radians

    DEBUG_INT3("AMMS::CAMMSEMCSpectatorDopplerControl::SetVelocitySphericalL %d, %d, %d",
               aAzimuth, aElevation, aRadius);

    TInt32 convertedAzimuth = (TInt32)(aAzimuth * KDegToRad *
                                       1000);   // CSI: 47 Effect API uses thousands of radians #

    TInt32 convertedElevation = (TInt32)(aElevation * KDegToRad *
                                         1000);   // CSI: 47 Effect API uses thousands of radians #

    DEBUG_INT2("AMMS::CAMMSEMCSpectatorDopplerControl::SetVelocitySphericalL %d, %d",
               convertedAzimuth, convertedElevation);

    TInt err = iMListenerDopplerControl->SetSphericalVelocity(
                   convertedAzimuth, convertedElevation, aRadius);
    DEBUG_INT("AMMS::CAMMSEMCSpectatorDopplerControl::SetVelocitySphericalL: err = %d", err);
    // Apply updated settings to EMC API.
    iMListenerDopplerControl->Apply();
}


// -----------------------------------------------------------------------------
// CAMMSEMCSpectatorDopplerControl::CAMMSEMCSpectatorDopplerControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSEMCSpectatorDopplerControl::CAMMSEMCSpectatorDopplerControl(
    CMMAPlayer* aPlayer)
        : CAMMSDopplerControl(aPlayer)
{
    iMMAPlayer = aPlayer;
}


//  End of File
