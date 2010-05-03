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
#include "cammsaudiodopplercontrol.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSAudioDopplerControl::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSAudioDopplerControl* CAMMSAudioDopplerControl::NewLC(CMMAPlayer* aPlayer)
{
    CAMMSAudioDopplerControl* self =
        new(ELeave) CAMMSAudioDopplerControl(aPlayer);

    CleanupStack::PushL(self);

    return self;
}

// Destructor
CAMMSAudioDopplerControl::~CAMMSAudioDopplerControl()
{
    DEBUG("AMMS::CAMMSAudioDopplerControl::~CAMMSAudioDopplerControl");

    // Perform DeallocateControl, if the state change has not yet performed it.
    DeallocateControl();
    delete(CAudioEffect*)iDopplerEffect;
}

// -----------------------------------------------------------------------------
// CAMMSAudioDopplerControl::PrepareControlL
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSAudioDopplerControl::PrepareControlL()
{
    // Perform the action only for the first time, skip if called afterwards
    if (!iDopplerEffect)
    {
        DEBUG("AMMS::CAMMSAudioDopplerControl::PrepareControlL");

        CCustomCommandUtility* customCommandUtility =
            CreateCustomCommandUtilityL();

        // Set the base class location effect as CSourceDoppler
        // Effect API takes the ownership of customCommandUtility.
        iDopplerEffect = CSourceDoppler::NewL(customCommandUtility);
    }
}

// -----------------------------------------------------------------------------
// CAMMSAudioDopplerControl::DeallocateControl
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSAudioDopplerControl::DeallocateControl()
{
    if (iDopplerEffect)
    {
        DEBUG("AMMS::CAMMSAudioDopplerControl::DeallocateControl");

        // Doppler for Audio can be disabled or enabled
        if (Enabled())
        {
            TRAPD(err, iDopplerEffect->DisableL());
            if (err != KErrNone)
            {
                // The only even theoritically possible error code here would be
                // KErrAccessDenied which is a result from Effect API calling
                // ApplyL method without having update rights, but since the
                // Effect is already created, that situation can be discarded.
            }
        }

        // Delete the Effect API class.
        delete(CAudioEffect*)iDopplerEffect;
        iDopplerEffect = NULL;
    }
}

const TDesC& CAMMSAudioDopplerControl::ClassName() const
{
    return KAMMSAudioDopplerControl;
}


// -----------------------------------------------------------------------------
// CAMMSAudioDopplerControl::CAMMSAudioDopplerControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSAudioDopplerControl::CAMMSAudioDopplerControl(
    CMMAPlayer* aPlayer)
        : CAMMSDopplerControl(aPlayer)
{
}
//  End of File
