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
* Description:  Base class for AMMS controls.
*
*/


// INCLUDE FILES
#include <cmmaaudioplayer.h>
#include <cmmamidiplayer.h>
#include "cammscontrol.h"
#include "cammscustomcommandutility.h"

// ============================ MEMBER FUNCTIONS ===============================

// Destructor
CAMMSControl::~CAMMSControl()
{
}

// -----------------------------------------------------------------------------
// CAMMSControl::CreateCustomCommandUtilityL
// Creates a custom command utility.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCustomCommandUtility* CAMMSControl::CreateCustomCommandUtilityL()
{
    CCustomCommandUtility* customCommandUtility = NULL;

    if (iPlayer->Type() == KMMAMIDIPlayer)
    {
        CMMAMIDIPlayer* mmaMIDIPlayer =
            reinterpret_cast< CMMAMIDIPlayer* >(iPlayer);

        CMidiClientUtility* midiClientUtility = mmaMIDIPlayer->MidiClient();
        customCommandUtility =
            CCustomCommandUtility::NewL(*midiClientUtility);
    }
    else if (iPlayer->Type() == KMMAAudioPlayer)
    {
        CMMAAudioPlayer* mmaAudioPlayer =
            reinterpret_cast< CMMAAudioPlayer* >(iPlayer);

        RMMFController& mmfController = mmaAudioPlayer->Controller();
        customCommandUtility =
            CAMMSCustomCommandUtility::NewL(mmfController);
    }
    else
    {
        // Player is KMMAEMCAudioPlayer or of other type
        // Do nothing
    }

    return customCommandUtility;
}

// -----------------------------------------------------------------------------
// CAMMSControl::PublicClassName
// Returns public class name.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC& CAMMSControl::PublicClassName() const
{
    // Return null descriptor so that the control name is not visible in MMA.
    return KNullDesC;
}

// -----------------------------------------------------------------------------
// CAMMSControl::PrepareControlL
// Virtual function to prepare Controls.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAMMSControl::PrepareControlL()
{
    // Empty implementation, the Controls that need preparing overwrite this
}

// -----------------------------------------------------------------------------
// CAMMSControl::DeallocateControl
// Virtual function to deallocate Controls.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAMMSControl::DeallocateControl()
{
    // Empty implementation, the Controls that need deallocating overwrite this
}

// -----------------------------------------------------------------------------
// CAMMSControl::CAMMSControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSControl::CAMMSControl(CMMAPlayer* aPlayer)
        : iPlayer(aPlayer)
{
}

//  End of File

