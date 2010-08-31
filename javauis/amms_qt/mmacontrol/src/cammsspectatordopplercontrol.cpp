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
#include <logger.h>
#include "cammsspectatordopplercontrol.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSSpectatorDopplerControl::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSSpectatorDopplerControl* CAMMSSpectatorDopplerControl::NewLC(
    CMMAPlayer* aPlayer)
{
    CAMMSSpectatorDopplerControl* self =
        new(ELeave)CAMMSSpectatorDopplerControl(aPlayer);

    CleanupStack::PushL(self);

    return self;
}

// Destructor
CAMMSSpectatorDopplerControl::~CAMMSSpectatorDopplerControl()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSSpectatorDopplerControl::~CAMMSSpectatorDopplerControl");

    // Perform DeallocateControl, if the state change has not yet performed it.
    DeallocateControl();
    delete(CAudioEffect*)iDopplerEffect;
}

// -----------------------------------------------------------------------------
// CAMMSSpectatorDopplerControl::PrepareControlL
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSSpectatorDopplerControl::PrepareControlL()
{
    // Perform the action only for the first time, skip if called afterwards
    if (!iDopplerEffect)
    {
        LOG( EJavaAMMS, EInfo, "AMMS::CAMMSSpectatorDopplerControl::PrepareControlL");

        CCustomCommandUtility* customCommandUtility =
            CreateCustomCommandUtilityL();

        // Set the base class doppler effect as CListenerDoppler
        // Effect API takes the ownership of customCommandUtility.
        iDopplerEffect = CListenerDoppler::NewL(customCommandUtility);
    }
}

// -----------------------------------------------------------------------------
// CAMMSSpectatorDopplerControl::DeallocateControl
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSSpectatorDopplerControl::DeallocateControl()
{
    if (iDopplerEffect)
    {
        LOG( EJavaAMMS, EInfo, "AMMS::CAMMSSpectatorDopplerControl::DeallocateControl");

        // Doppler for Spectator is always enabled (JSR-234 mandates it)
        // so the disabling is needed here
        TRAPD(err, iDopplerEffect->DisableL());
        if (err != KErrNone)
        {
            // The only even theoritically possible error code here would be
            // KErrAccessDenied which is a result from Effect API calling ApplyL
            // method without having update rights, but since the Effect
            // is already created, that situation can be discarded here.
        }

        // Delete the Effect API class (it deletes CustomCommandUtility)
        // The class that is derived from CDoppler is casted here to the
        // base class CAudioEffect, since CDoppler has the destructor
        // defined as protected, but the derived class and the base class
        // have it defined as public.
        delete(CAudioEffect*)iDopplerEffect;
        iDopplerEffect = NULL;
    }
}

const TDesC& CAMMSSpectatorDopplerControl::ClassName() const
{
    return KAMMSSpectatorDopplerControl;
}

// -----------------------------------------------------------------------------
// CAMMSSpectatorDopplerControl::CAMMSSpectatorDopplerControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSSpectatorDopplerControl::CAMMSSpectatorDopplerControl(
    CMMAPlayer* aPlayer)
        : CAMMSDopplerControl(aPlayer)
{
}


//  End of File
