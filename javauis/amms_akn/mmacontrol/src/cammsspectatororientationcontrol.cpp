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
#include "cammsspectatororientationcontrol.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSSpectatorOrientationControl::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSSpectatorOrientationControl* CAMMSSpectatorOrientationControl::NewLC(
    CMMAPlayer* aPlayer)
{
    CAMMSSpectatorOrientationControl* self =
        new(ELeave)CAMMSSpectatorOrientationControl(aPlayer);

    CleanupStack::PushL(self);

    return self;
}

// Destructor
CAMMSSpectatorOrientationControl::~CAMMSSpectatorOrientationControl()
{
    DEBUG("AMMS::CAMMSSpectatorOrientationControl::~CAMMSSpectatorOrientationControl");

    // Perform DeallocateControl, if the state change has not yet performed it.
    DeallocateControl();
    delete(CAudioEffect*)iOrientationEffect;
}

// -----------------------------------------------------------------------------
// CAMMSSpectatorOrientationControl::PrepareControlL
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSSpectatorOrientationControl::PrepareControlL()
{
    // Perform the action only for the first time, skip if called afterwards
    if (!iOrientationEffect)
    {
        DEBUG("AMMS::CAMMSSpectatorOrientationControl::PrepareControlL");

        CCustomCommandUtility* customCommandUtility =
            CreateCustomCommandUtilityL();

        // Set the base class orientation effect as CListenerOrientation
        // Effect API takes the ownership of customCommandUtility.
        iOrientationEffect = CListenerOrientation::NewL(
                                 customCommandUtility);

        // Enable the Effect API Control
        iOrientationEffect->EnableL();
    }
}

// -----------------------------------------------------------------------------
// CAMMSSpectatorOrientationControl::DeallocateControl
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSSpectatorOrientationControl::DeallocateControl()
{
    if (iOrientationEffect)
    {
        DEBUG("AMMS::CAMMSSpectatorOrientationControl::DeallocateControl");

        // Disable the Effect API Control
        TRAPD(err, iOrientationEffect->DisableL());
        if (err != KErrNone)
        {
            // The only even theoritically possible error code here would be
            // KErrAccessDenied which is a result from Effect API calling ApplyL
            // method without having update rights, but since the Effect
            // is already created, that situation can be discarded here.
        }

        // Delete the Effect API class.
        // The class that is derived from COrientation is casted here to the
        // base class CAudioEffect, since COrientation has the destructor
        // defined as protected, but the derived class and the base class
        // have it defined as public.
        delete(CAudioEffect*)iOrientationEffect;
        iOrientationEffect = NULL;
    }
}

const TDesC& CAMMSSpectatorOrientationControl::ClassName() const
{
    return KAMMSSpectatorOrientationControl;
}

// -----------------------------------------------------------------------------
// CAMMSSpectatorOrientationControl::CAMMSSpectatorOrientationControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSSpectatorOrientationControl::CAMMSSpectatorOrientationControl(
    CMMAPlayer* aPlayer)
        : CAMMSOrientationControl(aPlayer)
{
}

//  End of File
