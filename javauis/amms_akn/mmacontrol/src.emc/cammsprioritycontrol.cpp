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
* Description:  Manipulates the priority of a Player.
*
*/



// INCLUDE FILES
#include <AudioPreference.h>
#include <jdebug.h>
#include <cmmamidiplayer.h>
#include "cammsprioritycontrol.h"
#include <cmmaplayerevent.h>
#include <cmmaaudioplayer.h>
#include <cmmaemcaudioplayer.h>
#include <mmf/common/mmfcontroller.h>
#include <midiclientutility.h>



const TInt KErrorMessageSize = 32;
_LIT(KErrPriorityError, "AMMS PC error: %d");

// Default AMMS priority.
const TInt KAMMSDefaultPriority = 50;

// Reasonable MMF priorities.
const TInt KAMMSMinMMFPriority = 71;
const TInt KAMMSMaxMMFPriority = 89;

#ifdef _DEBUG
// CONSTANTS
const TInt KAMMSMaxPriority = 100;
const TInt KAMMSMinPriority = 0;
#endif // _DEBUG


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSPriorityControl::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSPriorityControl* CAMMSPriorityControl::NewLC(CMMAPlayer* aPlayer)
{
    CAMMSPriorityControl* self = new(ELeave) CAMMSPriorityControl(aPlayer);

    CleanupStack::PushL(self);

    self->ConstructL();

    return self;
}

// Destructor
CAMMSPriorityControl::~CAMMSPriorityControl()
{
    DEBUG("AMMS::CAMMSPriorityControl::~");
}

// -----------------------------------------------------------------------------
// CAMMSPriorityControl::SetPriorityL
// Sets the priority using a linear point scale between 0 and 100.
// -----------------------------------------------------------------------------
//
void CAMMSPriorityControl::SetPriorityL(TInt aPriority)
{
    DEBUG_INT("AMMS::CAMMSPriorityControl::SetPriorityL %d", aPriority);

    // Check in debug build that aPriority is within valid range.
    __ASSERT_DEBUG(
        (aPriority <= KAMMSMaxPriority) &&
        (aPriority >= KAMMSMinPriority),
        User::Invariant());

    // Set a new priority only if it differs from the previous one.
    if (aPriority != iVisiblePriority)
    {
        // Set the new priority to MMF only if the player is PREFETCHED
        // (otherwise setting will leave). In other states, the new priority
        // will be set when the player state changes to PREFETCHED.
        if (iPlayer->State() == CMMAPlayer::EPrefetched)
        {
            SetPriorityToMmfL(aPriority);
        }

        iVisiblePriority = aPriority;
    }

    DEBUG("AMMS::CAMMSPriorityControl::SetPriorityL -");
}

// -----------------------------------------------------------------------------
// CAMMSPriorityControl::Priority
// Gets the priority.
// -----------------------------------------------------------------------------
TInt CAMMSPriorityControl::Priority()
{
    return iVisiblePriority;
}

// -----------------------------------------------------------------------------
// CAMMSPriorityControl::StateChanged
// Called when player state is changed.
// -----------------------------------------------------------------------------
void CAMMSPriorityControl::StateChanged(TInt aState)
{
    DEBUG_INT("AMMS::CAMMSPriorityControl::StateChanged +, state = %d",
              aState);

    // If the state was changed to PREFETCHED, set the buffered priority to
    // MMF. Set a new priority only if it differs from the previous one.

    if ((aState == CMMAPlayer::EPrefetched) &&
            (iActualPriority != iVisiblePriority))
    {
        TRAPD(err, SetPriorityToMmfL(iVisiblePriority));

        DEBUG_INT("AMMS::CAMMSPriorityControl::StateChanged, err = %d", err);

        if (err != KErrNone)
        {
            TBuf<KErrorMessageSize> errorMessage;
            errorMessage.Format(KErrPriorityError, err);
            iPlayer->PostStringEvent(CMMAPlayerEvent::EError, errorMessage);
        }
    }

    DEBUG("AMMS::CAMMSPriorityControl::StateChanged -");
}

const TDesC& CAMMSPriorityControl::ClassName() const
{
    DEBUG("CAMMSPriorityControl::ClassName");

    return KAMMSPriorityControl;
}

// -----------------------------------------------------------------------------
// CAMMSPriorityControl::SetPriorityToMmfL
// Scales the given AMMS priority to MMF priority and sets it to MMF.
// -----------------------------------------------------------------------------
void CAMMSPriorityControl::SetPriorityToMmfL(TInt aAmmsPriority)
{
    DEBUG_INT("AMMS::CAMMSPriorityControl::SetPriorityToMmfL %d",
              aAmmsPriority);

    // Scale the AMMS priority value to MMF priority value before setting it.
    // The default priority used by MMA is 80. MMF priority can be between
    // -100 and 100, but values between 71 and 89 are reasonable ones.
    TInt newPriority = KAMMSMinMMFPriority + aAmmsPriority *
                       (KAMMSMaxMMFPriority - KAMMSMinMMFPriority) / 100;   // CSI: 47 100% for scaled value #

    DEBUG_INT("AMMS::CAMMSPriorityControl::SetPriorityToMmfL, newPriority = %d",
              newPriority);

    // In case of MIDIPlayer, use CMidiClientUtility, otherwise RMMFController.
    if (iPlayer->Type() == KMMAMIDIPlayer)
    {
        // In case of CMMAMIDIPlayer use CMidiClientUtility
        CMMAMIDIPlayer* mmaMIDIPlayer =
            reinterpret_cast< CMMAMIDIPlayer* >(iPlayer);

        CMidiClientUtility* midiClientUtility = mmaMIDIPlayer->MidiClient();

        midiClientUtility->SetPriorityL(newPriority,
                                        KMMAMIDIPriorityPreference);
    }
    else if (iPlayer->Type() == KMMAAudioPlayer)
    {
        CMMAAudioPlayer* mmaAudioPlayer =
            reinterpret_cast< CMMAAudioPlayer* >(iPlayer);

        RMMFController& rmmfController = mmaAudioPlayer->Controller();

        TMMFPrioritySettings prioritySettings;
        prioritySettings.iPriority = newPriority;

        User::LeaveIfError(
            rmmfController.SetPrioritySettings(prioritySettings));
    }
    else if (iPlayer->Type() == KMMAEMCAudioPlayer)
    {
        MStreamControl*  streamControl = (static_cast<CMMAEMCAudioPlayer*>(iPlayer))->StreamControl();
        User::LeaveIfError(streamControl->SetPriority(newPriority, KMMAEMCPriorityPreference));
    }

    iActualPriority = aAmmsPriority;

    DEBUG("AMMS::CAMMSPriorityControl::SetPriorityToMmfL -");
}

// -----------------------------------------------------------------------------
// CAMMSPriorityControl::CAMMSPriorityControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSPriorityControl::CAMMSPriorityControl(CMMAPlayer* aPlayer)
        : CAMMSControl(aPlayer),
        iVisiblePriority(KAMMSDefaultPriority),
        iActualPriority(KAMMSDefaultPriority)
{
}

// -----------------------------------------------------------------------------
// CAMMSPlayerStateListener::ConstructL
// 2nd phase constructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSPriorityControl::ConstructL()
{
    DEBUG("AMMS::CAMMSPriorityControl::ConstructL +");

    iPlayer->AddStateListenerL(this);

    DEBUG("AMMS::CAMMSPriorityControl::ConstructL -");
}


//  End of File
