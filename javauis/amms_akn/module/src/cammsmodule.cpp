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
* Description:  This class is a container for MAMMSControlGroup objects.
*
*/


// INCLUDE FILES
#include "cammsmodule.h"
#include "cammscontrolgroup.h"
#include <jdebug.h>


// CONSTANTS
// Before further testing is done, 4 is sufficient average value to be used here.
const TInt KAMMSModuleDefaultGranularity = 4;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSModule::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSModule* CAMMSModule::NewL()
{
    CAMMSModule* self = NewLC();
    CleanupStack::Pop(self);

    return self;
}

// -----------------------------------------------------------------------------
// CAMMSModule::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSModule* CAMMSModule::NewLC()
{
    CAMMSModule* self = new(ELeave) CAMMSModule;

    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// Destructor
CAMMSModule::~CAMMSModule()
{
    // Check that construction fully succeed.
    if (iControlGroups)
    {
        // control groups are owned and they must be deleted.
        iControlGroups->ResetAndDestroy();
        delete iControlGroups;
    }

    // Players are not owned.
    iPlayers.Close();
}

// -----------------------------------------------------------------------------
// CAMMSModule::AddControlGroupL
// Add control to iControlGroups array.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAMMSModule::AddControlGroupAndPopL(CAMMSControlGroup* aGroup)
{
    // Check in debug build that group is not null.
    __ASSERT_DEBUG(aGroup, User::Invariant());

    iControlGroups->AppendL(aGroup);

    // aGroup must be the previous item in the cleanupstack
    CleanupStack::Pop(aGroup);
}

// -----------------------------------------------------------------------------
// CAMMSModule::AddPlayerL
// Adds new player to this module.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAMMSModule::AddPlayerL(CMMAPlayer* aPlayer)
{
    // Check in debug build that player is not null.
    __ASSERT_DEBUG(aPlayer, User::Invariant());

    // Must leave if the player is already in the module
    if (HasPlayer(aPlayer))
    {
        User::Leave(KErrArgument);
    }

    // Must leave if player is in PREFETCHED or STARTED state
    User::LeaveIfError(CheckPlayerState(aPlayer));

    // Check that none of the players is not in PREFETCHED or STARTED state
    User::LeaveIfError(CheckAllPlayersState());

    AddPlayerNoStateCheckL(aPlayer);
}

// -----------------------------------------------------------------------------
// CAMMSModule::RemovePlayer
// Removes player from module..
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CAMMSModule::RemovePlayer(CMMAPlayer* aPlayer)
{
    // call RemovePlayerNoStateCheck only if aPlayer and all players in the
    // module are in the right state.
    TInt error = CheckPlayerState(aPlayer);
    if (error == KErrNone)
    {
        error = CheckAllPlayersState();
        if (error == KErrNone)
        {
            error = RemovePlayerNoStateCheck(aPlayer);
        }
    }
    return error;
}

// -----------------------------------------------------------------------------
// CAMMSModule::AddPlayerNoStateCheckL
// Adds player without checking its state.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAMMSModule::AddPlayerNoStateCheckL(CMMAPlayer* aPlayer)
{
    iPlayers.AppendL(aPlayer);

    // If adding players leaves CAMMSModule::CleanupAddPlayer method will be
    // called and player removed from module and from groups.
    CleanupStack::PushL(TCleanupItem(&CAMMSModule::CleanupAddPlayer, this));

    // Inform all control groups
    TInt count = iControlGroups->Count();
    for (TInt i = 0; i < count; i++)
    {
        iControlGroups->At(i)->PlayerAddedL(aPlayer);
    }

    CleanupStack::Pop(); // CSI: 12 Parameter TCleanupItem cannot be used as a parameter in Pop #
}

// -----------------------------------------------------------------------------
// CAMMSModule::RemovePlayerNoStateCheck
// Removes player without checking its state
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CAMMSModule::RemovePlayerNoStateCheck(CMMAPlayer* aPlayer)
{
    TInt index = iPlayers.Find(aPlayer);
    if (index != KErrNotFound)
    {
        // player was in the array, remove it
        iPlayers.Remove(index);

        // inform all groups
        TInt count = iControlGroups->Count();
        for (TInt i = 0; i < count; i++)
        {
            iControlGroups->At(i)->PlayerRemoved(aPlayer);
        }
    }

    // Find returned KErrNotFound if player was not in the module
    return index;
}

// -----------------------------------------------------------------------------
// CAMMSModule::HasPlayer
// Checks whether the given player is in this module.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CAMMSModule::HasPlayer(CMMAPlayer* aPlayer)
{
    DEBUG_INT("AMMS:CAMMSModule::HasPlayer %d +", (TInt)aPlayer);

    TInt index = iPlayers.Find(aPlayer);

    DEBUG_INT("AMMS:CAMMSModule::HasPlayer index=%d -", index);

    return (index != KErrNotFound);
}


#ifdef __WINS__

// -----------------------------------------------------------------------------
// CAMMSModule::PlayerCount
// Returns the count of players whose state is between the given limits.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CAMMSModule::PlayerCount(TInt aMinState, TInt aMaxState)
{
    DEBUG_INT2("AMMS:CAMMSModule::PlayerCount %d %d +", aMinState, aMaxState);

    TInt result = 0;

    TInt playerCount = iPlayers.Count();

    DEBUG_INT("AMMS:CAMMSModule::HasPlayer, players=%d", playerCount);

    for (TInt i = 0; i < playerCount; i++)
    {
        TInt playerState = iPlayers[ i ]->State();

        DEBUG_INT2("AMMS:CAMMSModule::HasPlayer %d, state=%d", i,
                   playerState);

        if ((playerState >= aMinState) &&
                (playerState <= aMaxState))
        {
            result++;
        }
    }

    DEBUG_INT("AMMS:CAMMSModule::PlayerCount %d -", result);

    return result;
}

#endif  // __WINS__

// -----------------------------------------------------------------------------
// CAMMSModule::CheckPlayerState
// Checks all players state.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CAMMSModule::CheckAllPlayersState()
{
    TInt error = KErrNone;

    TInt playerCount = iPlayers.Count();

    // Loop until all players are checked or one of the players is in
    // PREFETCHED or STARTED state
    for (TInt i = 0; (i < playerCount) &&
            (error == KErrNone); i++)
    {
        error = CheckPlayerState(iPlayers[ i ]);
    }
    return error;
}

// -----------------------------------------------------------------------------
// CAMMSModule::CheckPlayerState
// Checks player state.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CAMMSModule::CheckPlayerState(CMMAPlayer* aPlayer)
{
    // Player may not be in PREFETCHED or STARTED state
    TInt retVal = KErrNone;
    TInt playerState = aPlayer->State();
    if (playerState == CMMAPlayer::EStarted ||
            playerState == CMMAPlayer::EPrefetched)
    {
        retVal = KErrNotReady;
    }
    return retVal;
}

// -----------------------------------------------------------------------------
// CAMMSModule::CleanupAddPlayer
// Static function to be used with TCleanupItem in AddPlayerNoStateCheckL
// method. This method removes last added player from module and control
// groups.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAMMSModule::CleanupAddPlayer(TAny* aModule)
{
    CAMMSModule* module = static_cast< CAMMSModule* >(aModule);

    // This method is called from AddPlayerNoStateCheckL and there is always
    // at least one player.
    __ASSERT_DEBUG(module->iPlayers.Count() > 0, User::Invariant());

    // Remove last added player from module and from control groups
    module->RemovePlayerNoStateCheck(
        module->iPlayers[ module->iPlayers.Count() - 1 ]);
}

// -----------------------------------------------------------------------------
// CAMMSModule::Find
// Find control group with specified class name.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MAMMSControlGroup* CAMMSModule::Find(const TDesC& aClassName)
{
    MAMMSControlGroup* group = NULL;
    TInt groupIndex = 0;
    TInt groupCount = iControlGroups->Count();

    // Loop until group is found or all group are checked
    while (!group && (groupIndex < groupCount))
    {
        MAMMSControlGroup* tmpGroup = iControlGroups->At(groupIndex);

        if (tmpGroup->ClassName() == aClassName)
        {
            // found the group, set return value which will stop while loop
            group = tmpGroup;
        }

        groupIndex++;
    }

    return group;
}

// -----------------------------------------------------------------------------
// CAMMSModule::At
// Index must be non-negative and less than the number of objects currently
// within the array otherwise the functions raise an E32USER-CBase 21 panic.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MAMMSControlGroup* CAMMSModule::At(TInt aIndex)
{
    return iControlGroups->At(aIndex);    // CSI: 1 Array range panic allowed according to function description #
}

// -----------------------------------------------------------------------------
// CAMMSModule::Count
// return iControlGroup count.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CAMMSModule::Count()
{
    return iControlGroups->Count();
}

// -----------------------------------------------------------------------------
// CAMMSModule::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CAMMSModule::ConstructL()
{
    // Create array with default granularity, all derived classes must call
    // this method.
    iControlGroups = new(ELeave)CArrayPtrSeg< CAMMSControlGroup >(
        KAMMSModuleDefaultGranularity);

}

// -----------------------------------------------------------------------------
// CAMMSModule::CAMMSModule
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSModule::CAMMSModule()
{
}

//  End of File
