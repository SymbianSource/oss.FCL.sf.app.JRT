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
* Description:  Implementation for player state listener.
*
*/


// INCLUDE FILES
#include "cammsplayerstatelistener.h"
#include "cammscontrolgroup.h"
#include <cmmaplayer.h>
#include <cmmaplayerevent.h>

// CONSTANTS
const TInt KErrorMessageSize = 32;
_LIT(KErrDefaultError, "AMMS error: %d");


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSPlayerStateListener::NewLC
// Two-phased constructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CAMMSPlayerStateListener* CAMMSPlayerStateListener::NewLC(
    CMMAPlayer* aPlayer,
    CAMMSControlGroup* aControlGroup)
{
    CAMMSPlayerStateListener* self =
        new(ELeave) CAMMSPlayerStateListener(aPlayer,
                                             aControlGroup);
    CleanupStack::PushL(self);

    self->ConstructL();

    return self;
}

// -----------------------------------------------------------------------------
// CAMMSPlayerStateListener::~CAMMSPlayerStateListener
// Destructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CAMMSPlayerStateListener::~CAMMSPlayerStateListener()
{
    if (iPlayer)
    {
        iPlayer->RemoveStateListener(this);
    }
}

// -----------------------------------------------------------------------------
// CAMMSPlayerStateListener::ConstructL
// 2nd phase constructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSPlayerStateListener::ConstructL()
{
    __ASSERT_DEBUG(iPlayer && iControlGroup, User::Invariant());

    iPlayer->AddStateListenerL(this);
}

// -----------------------------------------------------------------------------
// CAMMSPlayerStateListener::CAMMSPlayerStateListener
// C++ constructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CAMMSPlayerStateListener::CAMMSPlayerStateListener(
    CMMAPlayer* aPlayer,
    CAMMSControlGroup* aControlGroup)
        : iPlayer(aPlayer),
        iControlGroup(aControlGroup)
{
}


// -----------------------------------------------------------------------------
// CAMMSPlayerStateListener::Player
// Returns the player listened by this listener.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CMMAPlayer* CAMMSPlayerStateListener::Player()
{
    return iPlayer;
}

// -----------------------------------------------------------------------------
// CAMMSPlayerStateListener::StateChanged
// Called when player state is changed.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSPlayerStateListener::StateChanged(TInt aState)
{
    TRAPD(err, iControlGroup->PlayerStateChangedL(iPlayer, aState));

    if (err != KErrNone)
    {
        TBuf<KErrorMessageSize> errorMessage;
        errorMessage.Format(KErrDefaultError, err);
        iPlayer->PostStringEvent(CMMAPlayerEvent::EError, errorMessage);
    }
}

//  End of File
