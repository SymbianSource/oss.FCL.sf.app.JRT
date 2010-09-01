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
* Description:  Manipulates the panning of a Player in the stereo output mix.
*
*/



// INCLUDE FILES
#include <jdebug.h>
#include <CMMAAudioPlayer.h>
#include <CMMAMidiPlayer.h>
#include "CAMMSPanControl.h"


#ifdef _DEBUG
// CONSTANTS
const TInt KAMMSMaxPanning = 100;
const TInt KAMMSMinPanning = -100;
#endif // _DEBUG

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSPanControl::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSPanControl* CAMMSPanControl::NewLC(CMMAPlayer* aPlayer)
{
    CAMMSPanControl* self = new(ELeave) CAMMSPanControl(aPlayer);

    CleanupStack::PushL(self);

    self->ConstructL();

    return self;
}

// Destructor
CAMMSPanControl::~CAMMSPanControl()
{
    delete iRMMFAudioPlayDeviceCustomCommands;
}

// -----------------------------------------------------------------------------
// CAMMSPanControl::PanL
// Gets the current panning set.
// -----------------------------------------------------------------------------
//
TInt CAMMSPanControl::PanL()
{
    TInt balance = KMMFBalanceCenter;

    // In case of MIDIPlayer, use CMidiClientUtility, otherwise RMMFController.
    if (iMidiClientUtility)
    {
        // Get the current stereo balance value.

        balance = iMidiClientUtility->GetBalanceL();
    }
    else
    {
        // Gets the balance between the left and right stereo audio channels.

        User::LeaveIfError(
            iRMMFAudioPlayDeviceCustomCommands->GetBalance(balance));
    }
    DEBUG_INT("AMMS::CAMMSPanControl::PanL called, native GetBalance = %d",
              balance);

    return balance;
}

// -----------------------------------------------------------------------------
// CAMMSPanControl::SetPanL
// Sets the panning using a linear point scale (-100 - +100).
// -----------------------------------------------------------------------------
//
TInt CAMMSPanControl::SetPanL(TInt aPan)
{
    DEBUG_INT("AMMS::CAMMSPanControl::SetPanL: Pan = %d", aPan);

    // Check in debug build that aPan is within valid range.
    __ASSERT_DEBUG(
        (aPan <= KAMMSMaxPanning) &&
        (aPan >= KAMMSMinPanning),
        User::Invariant());

    // In case of MIDIPlayer, use CMidiClientUtility, otherwise RMMFController.
    if (iMidiClientUtility)
    {
        // Set the current stereo balance value.
        // Defaults to KMMFBalanceCenter to restore equal left-right balance.

        iMidiClientUtility->SetBalanceL(aPan);
    }
    else
    {
        // Sets the balance between the left and right stereo audio channels
        // Use a value between KMMFBalanceMaxLeft and KMMFBalanceMaxRight.
        // Centre balance can be restored by using KMMFBalanceCenter.

        User::LeaveIfError(
            iRMMFAudioPlayDeviceCustomCommands->SetBalance(aPan));
    }

    return PanL();
}


const TDesC& CAMMSPanControl::ClassName() const
{
    return KAMMSPanControl;
}

// -----------------------------------------------------------------------------
// CAMMSPanControl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CAMMSPanControl::ConstructL()
{
    DEBUG("AMMS::CAMMSPanControl::ConstructL");

    if (iPlayer->Type() == KMMAMIDIPlayer)
    {
        // In case of CMMAMIDIPlayer use CMidiClientUtility
        CMMAMIDIPlayer* mmaMIDIPlayer =
            reinterpret_cast< CMMAMIDIPlayer* >(iPlayer);

        iMidiClientUtility = mmaMIDIPlayer->MidiClient();
    }
    else
    {
        CMMAAudioPlayer* mmaAudioPlayer =
            reinterpret_cast< CMMAAudioPlayer* >(iPlayer);

        RMMFController& mmfController = mmaAudioPlayer->Controller();

        iRMMFAudioPlayDeviceCustomCommands =
            new(ELeave) RMMFAudioPlayDeviceCustomCommands(mmfController);
    }
}

// -----------------------------------------------------------------------------
// CAMMSPanControl::CAMMSPanControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSPanControl::CAMMSPanControl(CMMAPlayer* aPlayer)
        : CAMMSControl(aPlayer)
{
}
//  End of File
