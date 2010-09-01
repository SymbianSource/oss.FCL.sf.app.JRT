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
* Description:  AMMS player builder for adding AMMS audio player controls.
*
*/



// INCLUDE FILES
#include <cmmavolumecontrol.h>
#include <cmmaaudioplayer.h>
#include <jdebug.h>

#include "cammsvolumecontrolbuilder.h"
#include "cammsvolumecontrol.h"
#include "ammsconstants.h"
#include "ammsutil.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSVolumeControlBuilder::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSVolumeControlBuilder* CAMMSVolumeControlBuilder::NewLC()
{
    CAMMSVolumeControlBuilder* self = new(ELeave) CAMMSVolumeControlBuilder();
    CleanupStack::PushL(self);
    return self;
}


// Destructor
CAMMSVolumeControlBuilder::~CAMMSVolumeControlBuilder()
{
}

// -----------------------------------------------------------------------------
// CAMMSVolumeControlBuilder::PreparePlayerL
// -----------------------------------------------------------------------------
//
void CAMMSVolumeControlBuilder::PreparePlayerL(CMMAPlayer* aPlayer)
{
    DEBUG_STR("AMMS::CAMMSVolumeControlBuilder::PreparePlayerL type %S",
              aPlayer->Type());

    CMMAControl* control = AMMSUtil::FindControl(aPlayer,
                           KMMAVolumeControlName);
    if (control)
    {
        CMMAVolumeControl* mmaControl =
            reinterpret_cast< CMMAVolumeControl* >(control);

        CAMMSVolumeControl* ammsControl = CAMMSVolumeControl::NewLC(
                                              KAMMSGlobalVolume,
                                              mmaControl,
                                              aPlayer);
        aPlayer->AddControlL(ammsControl);
        CleanupStack::Pop(ammsControl);

        DEBUG("AMMS::CAMMSVolumeControlBuilder::PreparePlayerL add OK");
    }
    // else volume control need not to be added, for example camera player.
}

// -----------------------------------------------------------------------------
// CAMMSVolumeControlBuilder::CAMMSVolumeControlBuilder
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSVolumeControlBuilder::CAMMSVolumeControlBuilder()
{
}

//  End of File
