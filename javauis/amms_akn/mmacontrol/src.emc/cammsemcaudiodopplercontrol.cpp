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
* Description:  Manipulates the doppler effect of the SoundSource3D.
*
*/


// INCLUDE FILES
#include <jdebug.h>
#include <e32math.h>
#include "cammsemcaudiodopplercontrol.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSEMCAudioDopplerControl::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSEMCAudioDopplerControl* CAMMSEMCAudioDopplerControl::NewLC(CMMAPlayer* aPlayer)
{
    CAMMSEMCAudioDopplerControl* self =
        new(ELeave) CAMMSEMCAudioDopplerControl(aPlayer);

    CleanupStack::PushL(self);

    return self;
}

// Destructor
CAMMSEMCAudioDopplerControl::~CAMMSEMCAudioDopplerControl()
{
    DEBUG("AMMS::CAMMSEMCAudioDopplerControl::~CAMMSEMCAudioDopplerControl");
    // Perform DeallocateControl, if the state change has not yet performed it.
    DeallocateControl();
}

// -----------------------------------------------------------------------------
// CAMMSEMCAudioDopplerControl::PrepareControlL
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSEMCAudioDopplerControl::PrepareControlL()
{
    // Perform the action only for the first time, skip if called afterwards
    if (!iMSourceDopplerControl)
    {
        DEBUG("AMMS::CAMMSEMCAudioDopplerControl::PrepareControlL");
        //Create Doppler Effect Control
        iStreamControl = (static_cast<CMMAEMCAudioPlayer*>(iMMAPlayer))->StreamControl();
        iFactory = (static_cast<CMMAEMCAudioPlayer*>(iMMAPlayer))->MMFactory();
        //Check the state of stream control ,it must be INITIALIZED
        TInt state = iStreamControl->GetState();
        DEBUG_INT("AMMS::CAMMSEMCAudioDopplerControl::PrepareControlL: Stream state = %d,",state);

        MEffectControl* temp(NULL);
        User::LeaveIfError(iFactory->CreateEffectControl(KSourceDopplerEffectControl, temp));
        iMSourceDopplerControl  = static_cast<MSourceDopplerControl*>(temp);
        User::LeaveIfError(iStreamControl->AddEffect(*iMSourceDopplerControl));
        iMSourceDopplerControl->Enable();
    }
}

// -----------------------------------------------------------------------------
// CAMMSEMCAudioDopplerControl::DeallocateControl
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSEMCAudioDopplerControl::DeallocateControl()
{
    if (iMSourceDopplerControl)
    {

        DEBUG("AMMS::CAMMSEMCAudioDopplerControl::DeallocateControl");

        // Doppler for Audio can be disabled or enabled
        if (Enabled())
        {
            iMSourceDopplerControl->Disable();
        }
        //return the control to factory
        MEffectControl* temp = iMSourceDopplerControl;
        iStreamControl->RemoveEffect(*temp);
        // Delete the Effect
        MEffectControl* tempCtrl = iMSourceDopplerControl;
        iFactory->DeleteEffectControl(tempCtrl);
        iMSourceDopplerControl = NULL;

    }

}

const TDesC& CAMMSEMCAudioDopplerControl::ClassName() const
{
    return KAMMSEMCAudioDopplerControl;
}



void CAMMSEMCAudioDopplerControl::SetEnabledL(TBool aDopplerEnabled)
{

    if (aDopplerEnabled)
    {
        DEBUG("AMMS::CAMMSEMCAudioDopplerControl::SetEnabledL(true)");
        iMSourceDopplerControl->Enable();
    }
    else
    {
        DEBUG("AMMS::CAMMSEMCAudioDopplerControl::SetEnabledL(false)");
        iMSourceDopplerControl->Disable();
    }

}


// -----------------------------------------------------------------------------
// CAMMSEMCDopplerControl::Enabled
// Returns whether this Doppler effect is currently active.
// -----------------------------------------------------------------------------
//
TBool CAMMSEMCAudioDopplerControl::Enabled()
{
    TBool temp;
    iMSourceDopplerControl->IsEnabled(temp);
    return temp;
}

// -----------------------------------------------------------------------------
// CAMMSEMCDopplerControl::SetVelocityCartesianL
// Sets the velocity, used in calculations for the Doppler effect.
// -----------------------------------------------------------------------------
//
void CAMMSEMCAudioDopplerControl::SetVelocityCartesianL(
    TInt aX,
    TInt aY,
    TInt aZ)
{

    DEBUG_INT3("AMMS::CAMMSEMCAudioDopplerControl::SetVelocityCartesianL: X=%d, Y=%d, Z=%d",
               aX, aY, aZ);
    //Check the state of stream control ,it must be INITIALIZED
    TInt state = iStreamControl->GetState();

    DEBUG_INT("AMMS::CAMMSEMCAudioDopplerControl::SetVelocityCartesianL: Stream state = %d,",state);
    TInt err = iMSourceDopplerControl->SetCartesianVelocity(aX, aY, aZ);

    DEBUG_INT("AMMS::CAMMSEMCAudioDopplerControl::SetVelocityCartesianL: err = %d,",err);


    // Apply updated settings to Effect API.
    iMSourceDopplerControl->Apply();
}

// -----------------------------------------------------------------------------
// CAMMSEMCDopplerControl::VelocityCartesian
// Returns the current velocity, used in calculations for the Doppler effect.
// -----------------------------------------------------------------------------
//
void CAMMSEMCAudioDopplerControl::VelocityCartesian(
    TInt& aX, TInt& aY, TInt& aZ)
{
    // Get the velocity's cartesian settings
    // aX, aY and aZ are velocities in format mm/s
    TInt err = iMSourceDopplerControl->CartesianVelocity(aX,aY,aZ);
    DEBUG_INT("AMMS::CAMMSEMCAudioDopplerControl::VelocityCartesian: err = %d,",err);
}

// -----------------------------------------------------------------------------
// CAMMSEMCDopplerControl::SetVelocitySphericalL
// Sets the velocity, used in calculations for the Doppler effect.
// -----------------------------------------------------------------------------
//
void CAMMSEMCAudioDopplerControl::SetVelocitySphericalL(
    TInt aAzimuth,
    TInt aElevation,
    TInt aRadius)
{
    // Parameters are thousandths of radians

    DEBUG_INT3("AMMS::CAMMSEMCAudioDopplerControl::SetVelocitySphericalL %d, %d, %d",
               aAzimuth, aElevation, aRadius);

    TInt32 convertedAzimuth = (TInt32)(aAzimuth * KDegToRad *
                                       1000);   // CSI: 47 Effect API uses thousands of radians #

    TInt32 convertedElevation = (TInt32)(aElevation * KDegToRad *
                                         1000);   // CSI: 47 Effect API uses thousands of radians #

    DEBUG_INT2("AMMS::CAMMSEMCDopplerControl::SetVelocitySphericalL %d, %d",
               convertedAzimuth, convertedElevation);

    TInt err = iMSourceDopplerControl->SetSphericalVelocity(
                   convertedAzimuth, convertedElevation, aRadius);
    DEBUG_INT("AMMS::CAMMSEMCAudioDopplerControl::SetVelocitySphericalL: err = %d,",err);
    // Apply updated settings to EMC API.
    iMSourceDopplerControl->Apply();

}


// -----------------------------------------------------------------------------
// CAMMSEMCAudioDopplerControl::CAMMSEMCAudioDopplerControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSEMCAudioDopplerControl::CAMMSEMCAudioDopplerControl(
    CMMAPlayer* aPlayer)
        : CAMMSDopplerControl(aPlayer)
{
    iMMAPlayer = aPlayer;
}
//  End of File
