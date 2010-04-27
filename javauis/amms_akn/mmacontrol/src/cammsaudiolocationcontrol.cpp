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
#include <jdebug.h>
#include "cammsaudiolocationcontrol.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSAudioLocationControl::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSAudioLocationControl* CAMMSAudioLocationControl::NewLC(
    CMMAPlayer* aPlayer)
{
    CAMMSAudioLocationControl* self =
        new(ELeave) CAMMSAudioLocationControl(aPlayer);

    CleanupStack::PushL(self);

    return self;
}

// Destructor
CAMMSAudioLocationControl::~CAMMSAudioLocationControl()
{
    DEBUG("AMMS::CAMMSAudioLocationControl::~CAMMSAudioLocationControl");

    // Perform DeallocateControl, if the state change has not yet performed it.
    DeallocateControl();
    delete(CAudioEffect*)iLocationEffect;
}

// -----------------------------------------------------------------------------
// CAMMSAudioLocationControl::PrepareControlL
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSAudioLocationControl::PrepareControlL()
{
    // Perform the action only for the first time, skip if called afterwards
    if (!iLocationEffect)
    {
        DEBUG("AMMS::CAMMSAudioLocationControl::PrepareControlL");

        CCustomCommandUtility* customCommandUtility =
            CreateCustomCommandUtilityL();

        // Set the base class location effect as CSourceLocation
        // Effect API takes the ownership of customCommandUtility.
        iLocationEffect = CSourceLocation::NewL(customCommandUtility);

        // Enable the Effect API Control
        iLocationEffect->EnableL();
    }
}

// -----------------------------------------------------------------------------
// CAMMSAudioLocationControl::DeallocateControl
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSAudioLocationControl::DeallocateControl()
{
    if (iLocationEffect)
    {
        DEBUG("AMMS::CAMMSAudioLocationControl::DeallocateControl");

        // Disable the Effect API Control
        TRAPD(err, iLocationEffect->DisableL());
        if (err != KErrNone)
        {
            // The only even theoritically possible error code here would be
            // KErrAccessDenied which is a result from Effect API calling ApplyL
            // method without having update rights, but since the Effect
            // is already created, that situation can be discarded here.
        }

        // Delete the Effect API class.
        // The class that is derived from CLocation is casted here to the
        // base class CAudioEffect, since CLocation has the destructor
        // defined as protected, but the derived class and the base class
        // have it defined as public.
        delete(CAudioEffect*)iLocationEffect;
        iLocationEffect = NULL;
    }
}

const TDesC& CAMMSAudioLocationControl::ClassName() const
{
    return KAMMSAudioLocationControl;
}

// -----------------------------------------------------------------------------
// CAMMSAudioLocationControl::CAMMSAudioLocationControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSAudioLocationControl::CAMMSAudioLocationControl(CMMAPlayer* aPlayer)
        : CAMMSLocationControl(aPlayer)
{
}

//  End of File
