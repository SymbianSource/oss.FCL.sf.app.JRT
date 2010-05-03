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


#ifndef CAMMSCONTROLGROUP_H
#define CAMMSCONTROLGROUP_H

//  INCLUDES
#include <e32base.h>
#include "mammscontrolgroup.h"
#include "ammsconstants.h"

// FORWARD DECLARATIONS
class MAMMSControllable;
class CMMAControl;
class CMMAPlayer;
class CAMMSPlayerStateListener;

// CLASS DECLARATION
/**
 *  Base class for control groups
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CAMMSControlGroup): public CBase, public MAMMSControlGroup
{
public:  // Constructors and destructor

    /**
     * Destructor.
     */
    virtual ~CAMMSControlGroup();

public:  // New functions

    /**
     * This function is called when state of a player has changed.
     *
     * @param aPlayer Player whose state has changed.
     * @param aNewState State that the player has now.
     */
    void PlayerStateChangedL(CMMAPlayer* aPlayer,
                             TInt aNewState);

protected:  // New functions

    /**
     * 2nd phase constructor.
     */
    void ConstructL();

    /**
     * Returns the total count of controls in this group.
     *
     * @return Count of the controls.
     */
    TInt ControlCount() const;

    /**
     * Gets control. Ownership is not tranferred.
     *
     * @param aIndex    Control index.
     *
     * @return the control at given index
     */
    CMMAControl* Control(TInt aIndex) const;

    /**
     * Called by PlayerAddedL when new player is added
     *
     * @param aPlayer   The player being added
     * @param aControl  The player's control relevant to this group
     */
    virtual void NotifyPlayerAddedL(CMMAPlayer* aPlayer, CMMAControl* aControl);

    /**
     * Called by PlayerRemoved when new player is removed
     *
     * @param aPlayer   The player being removed
     * @param aControl  The player's control relevant to this group
     */
    virtual void NotifyPlayerRemoved(CMMAPlayer* aPlayer, CMMAControl* aControl);

public:  // From MAMMSControlGroup

    /**
     * Called by the owning module when a player is added
     *
     * @param aPlayer   The player being added
     */
    void PlayerAddedL(CMMAPlayer *aPlayer);

    /**
     * Called by the owning module when a player is removed
     *
     * @param aPlayer   The player being removed
     */
    void PlayerRemoved(CMMAPlayer *aPlayer);

protected:
    /**
     * C++ default constructor.
     *
     * @param aName The name of the corresponding amms control
     *        (doesn't take a copy!)
     * @param aControlType Special AMMS type of the Control
     */
    CAMMSControlGroup(
        const TDesC& aName,
        TAMMSControlTypes aControlType = EAMMSBaseControl);

protected:    // Data

    // Listeners for player state changes, owned.
    CArrayPtrSeg< CAMMSPlayerStateListener >* iPlayerStateListeners;

    // Grouped controls, not owned
    RArray< CMMAControl* > iControls;

    // The name of contained controls, not owned
    const TDesC& iName;

    // The type of the actual Control
    TAMMSControlTypes iControlType;

    // Controllable object. Not owned. (Global manager)
    MAMMSControllable* iAMMSControllable;

};

#endif // CAMMSCONTROLGROUP_H


