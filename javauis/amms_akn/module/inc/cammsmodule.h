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


#ifndef CAMMSMODULE_H
#define CAMMSMODULE_H

//  INCLUDES
#include <e32base.h>

#include <cmmaplayer.h>

#include "mammscontrollable.h"

// CONSTANTS
const TInt KAMMSPlayerAlreadyInModuleError = -123;

#ifdef __WINS__
const TInt KAMMSMixingNotSupported = -1234;
#endif  // __WINS__


// FORWARD DECLARATIONS
class CAMMSControlGroup;

// CLASS DECLARATION

/**
*
*  This class is a container for MAMMSControlGroup objects.
*
*
*  @since 3.0
*/
NONSHARABLE_CLASS(CAMMSModule): public CBase, public MAMMSControllable
{
public:
    /**
    * Two-phased constructor.
    */
    static CAMMSModule* NewL();

    /**
    * Two-phased constructor.
    */
    static CAMMSModule* NewLC();

    /**
    * Destructor.
    */
    ~CAMMSModule();

public: // New functions
    /**
    * Adds new control group to module. Ownership is transferred to
    * the module and aGroup is removed from the cleanupstack
    * if this method does not leave.
    * @since 3.0
    * @param aGroup New control group. Must be the previous item in the
    * cleanupstack.
    */
    void AddControlGroupAndPopL(CAMMSControlGroup* aGroup);

    /**
     * Adds new player to this module.
     * @param aPlayer New player to add
     */
    virtual void AddPlayerL(CMMAPlayer* aPlayer);

    /**
     * Removes player from module.
     * @param aPlayer player to remove
     * @return KErrNotFound if player was not in the module.
     */
    virtual TInt RemovePlayer(CMMAPlayer* aPlayer);

    /**
     * Adds new player to this module without checking player states.
     * @param aPlayer New player to add
     */
    virtual void AddPlayerNoStateCheckL(CMMAPlayer* aPlayer);

    /**
     * Removes player from module without checking player states.
     * @param aPlayer player to remove
     * @return KErrNotFound if player was not in the module.
     */
    virtual TInt RemovePlayerNoStateCheck(CMMAPlayer* aPlayer);

    /**
     * Checks whether the given player is in this module.
     * @param aPlayer Player to be checked.
     * @return ETrue if the specified player exists in this
     * module; EFalse otherwise.
     */
    TBool HasPlayer(CMMAPlayer* aPlayer);

#ifdef __WINS__

    /**
     * Returns the count of players whose state is between the given
     * limits.
     * @param aMinState Minimum player state to be searched.
     * @param aMaxState Maximum player state to be searched.
     * @return The count of players having state between the given
     * limits.
     */
    TInt PlayerCount(TInt aMinState, TInt aMaxState);

#endif  // __WINS__


private: // Private new functions
    /**
     * Checks that none of the players in this module is not in PREFETCHED
     * or STARTED state.
     * @return KErrNotReady if some player is in PREFETCHED or STARTED
     * state, otherwise KErrNone.
     */
    TInt CheckAllPlayersState();

    /**
     * Checks that player state is not in PREFETCHED or STARTED state.
     * @param aPlayer player to check
     * @return KErrNotReady if player is in PREFETCHED or STARTED state,
     * otherwise KErrNone.
     */
    TInt CheckPlayerState(CMMAPlayer* aPlayer);

    /**
     * Static function to be used with TCleanupItem in
     * AddPlayerNoStateCheckL method. This method removes last added
     * player from module and control groups.
     * @param aModule Module to clean.
     */
    static void CleanupAddPlayer(TAny* aModule);

public: // Functions from base classes
    MAMMSControlGroup* Find(const TDesC& aClassName);
    MAMMSControlGroup* At(TInt aIndex);
    TInt Count();

protected:
    /**
    * C++ default constructor.
    */
    CAMMSModule();

protected:
    /**
    * Protected because derived classes must call ConstructL.
    */
    void ConstructL();

private: // Data
    // Owned array containing control groups, which are owned.
    // Elements are added using AddControlGroupL method and deleted
    // in the destructor.
    CArrayPtrSeg< CAMMSControlGroup >* iControlGroups;

    // Array containing players.
    // Elements are added using AddPlayerL method and removes
    // with RemovePlayer
    RPointerArray< CMMAPlayer > iPlayers;
};

#endif // CAMMSMODULE_H


