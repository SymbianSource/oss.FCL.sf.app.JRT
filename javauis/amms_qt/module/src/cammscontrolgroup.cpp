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
* Description:  Base class for control groups
*
*/


// INCLUDE FILES
#include <logger.h>
#include "cammscontrolgroup.h"
#include <cmmaplayer.h>
#include "cammscontrol.h"
#include "ammsutil.h"
#include "cammsplayerstatelistener.h"

#ifdef _DEBUG
_LIT(KAMMSNoGroupNameError, "No group name");
#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSControlGroup::~CAMMSControlGroup
// Destructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CAMMSControlGroup::~CAMMSControlGroup()
{
    if (iPlayerStateListeners)
    {
        iPlayerStateListeners->ResetAndDestroy();
        delete iPlayerStateListeners;
    }

    iControls.Close();
}

// -----------------------------------------------------------------------------
// CAMMSControlGroup::PlayerStateChangedL
// This function is called when state of a player has changed.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSControlGroup::PlayerStateChangedL(
    CMMAPlayer* aPlayer,
    TInt aNewState)
{
    CMMAControl* findControl = AMMSUtil::FindControl(
                                   aPlayer, iName, iControlType);

    // It is safe to cast CMMAControl to the subclass CAMMSControl, as only
    // AMMS Controls are returned from FindControl method
    CAMMSControl* control = static_cast< CAMMSControl* >(findControl);

    // The player has a control belonging to this group if this function
    // is called.
    __ASSERT_DEBUG(control, User::Invariant());

    TInt controlIndex = iControls.Find(control);

    // Add the control of the player to the group if the player is prefetched
    // or started.
    if (controlIndex == KErrNotFound)
    {
        if ((aNewState == CMMAPlayer::EPrefetched) ||
                (aNewState == CMMAPlayer::EStarted))
        {
            // Now it is safe to call PrepareControlL to initialize the Control
            LOG1( EJavaAMMS, EInfo, "AMMS::CAMMSControlGroup::PlayerStateChangedL calling PrepareControl for type: %d",
                      control->iControlType);
            control->PrepareControlL();

            // Add control to controls array
            User::LeaveIfError(iControls.Append(control));

            // Notify derived classes about new player control
            NotifyPlayerAddedL(aPlayer, control);
        }
    }

    // Remove the control of the player from the group if the player is
    // deallocated or closed.
    else
    {
        if ((aNewState == CMMAPlayer::ERealized) ||
                (aNewState == CMMAPlayer::EClosed))
        {
            // Call DeallocateControl to delete the Control's Effect API class
            LOG1( EJavaAMMS, EInfo, "AMMS::CAMMSControlGroup::PlayerStateChangedL calling DeallocateControl for type: %d",
                      control->iControlType);
            control->DeallocateControl();

            NotifyPlayerRemoved(aPlayer, control);
            iControls.Remove(controlIndex);
        }
    }
}

// -----------------------------------------------------------------------------
// CAMMSControlGroup::ConstructL
// 2nd phase constructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSControlGroup::ConstructL()
{
    iPlayerStateListeners =
        new(ELeave) CArrayPtrSeg< CAMMSPlayerStateListener >(1);
}

// -----------------------------------------------------------------------------
// CAMMSControlGroup::ControlCount
// Returns the total count of controls in this group.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TInt CAMMSControlGroup::ControlCount() const
{
    return iControls.Count();
}

// -----------------------------------------------------------------------------
// CAMMSControlGroup::Control
// Gets control. Ownership is not tranferred.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CMMAControl* CAMMSControlGroup::Control(TInt aIndex) const
{
    // aIndex must not be negative and must not be greater than the number
    // of objects currently in the array,
    // otherwise the operator raises a USER-130 panic.
    return iControls[ aIndex ];
}


// -----------------------------------------------------------------------------
// CAMMSControlGroup::NotifyPlayerRemoved
// Called by PlayerAddedL when new player is added
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSControlGroup::NotifyPlayerAddedL(
    CMMAPlayer* /*aPlayer*/,
    CMMAControl* /*aControl*/)
{
}

// -----------------------------------------------------------------------------
// CAMMSControlGroup::NotifyPlayerRemoved
// Called by PlayerRemoved when a player is removed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSControlGroup::NotifyPlayerRemoved(
    CMMAPlayer* /*aPlayer*/,
    CMMAControl* /*aControl*/)
{
}

// -----------------------------------------------------------------------------
// CAMMSControlGroup::PlayerAddedL
// Called by the owning module when a player is added
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSControlGroup::PlayerAddedL(CMMAPlayer *aPlayer)
{
    // All derived classes must define name for the group
    __ASSERT_DEBUG(iName != KNullDesC,
                   User::Panic(KAMMSNoGroupNameError,
                               KErrUnknown));

    // Do nothing if the player does not have a control belonging to this group.
    CMMAControl* findControl = AMMSUtil::FindControl(
                                   aPlayer, iName, iControlType);
    if (!findControl)
    {
        return;
    }

    // It is safe to cast CMMAControl to the subclass CAMMSControl, as only
    // AMMS Controls are returned from FindControl method
    CAMMSControl* control = static_cast< CAMMSControl* >(findControl);

    CAMMSPlayerStateListener* playerListener =
        CAMMSPlayerStateListener::NewLC(aPlayer, this);

    iPlayerStateListeners->AppendL(playerListener);

    CleanupStack::Pop(playerListener);

    // Add the control of the player directly to the group
    // if the player is in STARTED or PREFETCHED state.
    // If not, the control is added later when the player changes
    // its state to PREFETCHED.
    TInt playerState = aPlayer->State();

    if ((playerState == CMMAPlayer::EPrefetched) ||
            (playerState == CMMAPlayer::EStarted))
    {
        // Now it is safe to call PrepareControlL to initialize the Control
        LOG1( EJavaAMMS, EInfo, "AMMS::CAMMSControlGroup::PlayerAddedL calling PrepareControl for type: %d",
                  control->iControlType);
        control->PrepareControlL();

        // Add control to controls array
        User::LeaveIfError(iControls.Append(control));

        // Notify derived classes about new player control
        NotifyPlayerAddedL(aPlayer, control);
    }
}

// -----------------------------------------------------------------------------
// CAMMSControlGroup::PlayerRemoved
// Called by the owning module when a player is removed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSControlGroup::PlayerRemoved(CMMAPlayer *aPlayer)
{
    CMMAControl* control = AMMSUtil::FindControl(
                               aPlayer, iName, iControlType);

    // Do nothing if the player does not have a control belonging to this group.
    if (!control)
    {
        return;
    }

    // Remove the control of the player if the control is in the group.
    TInt controlIndex = iControls.Find(control);
    if (controlIndex != KErrNotFound)
    {
        // It is safe to cast CMMAControl to the subclass CAMMSControl, as only
        // AMMS Controls are returned from FindControl method
        CAMMSControl* ammsControl = static_cast< CAMMSControl* >(control);

        // Call DeallocateControl to delete the Control's Effect API class
        LOG1( EJavaAMMS, EInfo, "AMMS::CAMMSControlGroup::PlayerRemoved calling DeallocateControl for type: %d",
                  ammsControl->iControlType);
        ammsControl->DeallocateControl();

        NotifyPlayerRemoved(aPlayer, ammsControl);
        iControls.Remove(controlIndex);
    }

    // Remove the state listener belonging to the given player.

    TInt listenerCount = iPlayerStateListeners->Count();
    TInt listenerIndex = KErrNotFound;

    for (TInt i = 0; i < listenerCount; i++)
    {
        // Elements in the listener array are never null.
        if (iPlayerStateListeners->At(i)->Player() == aPlayer)
        {
            listenerIndex = i;
            break;
        }
    }

    if (listenerIndex != KErrNotFound)
    {
        delete iPlayerStateListeners->At(listenerIndex);
        iPlayerStateListeners->Delete(listenerIndex);
    }
}

// -----------------------------------------------------------------------------
// CAMMSControlGroup::CAMMSControlGroup
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
CAMMSControlGroup::CAMMSControlGroup(
    const TDesC& aName,
    TAMMSControlTypes aControlType) :
        iName(aName),
        iControlType(aControlType)
{
}

//  End of File
