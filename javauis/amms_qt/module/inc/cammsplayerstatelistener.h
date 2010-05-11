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


#ifndef CAMMSPLAYERSTATELISTENER_H
#define CAMMSPLAYERSTATELISTENER_H

//  INCLUDES
#include <e32base.h>
#include <mmmaplayerstatelistener.h>

// FORWARD DECLARATIONS
class CMMAPlayer;
class CAMMSControlGroup;

// CLASS DECLARATION
/**
 *  Implementation for player state listener.
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CAMMSPlayerStateListener)
        : public CBase,
        public MMMAPlayerStateListener
{
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     *
     * @param iPlayer Player to be listened by this listener.
     * @param iControlGroup Control group to be informed when
     *        player state changes.
     */
    static CAMMSPlayerStateListener* NewLC(
        CMMAPlayer* aPlayer,
        CAMMSControlGroup* aControlGroup);
    /**
     * Destructor.
     */
    ~CAMMSPlayerStateListener();

private:  // Constructors and destructor

    /**
     * 2nd phase constructor.
     */
    void ConstructL();

    /**
     * C++ constructor.
     * @param iPlayer Player to be listened by this listener.
     * @param iControlGroup Control group to be informed when
     *        player state changes.
     */
    CAMMSPlayerStateListener(CMMAPlayer* aPlayer,
                             CAMMSControlGroup* aControlGroup);


public:  // New methods

    /**
     * Returns the player listened by this listener.
     *
     * @return Player.
     */
    CMMAPlayer* Player();

    /**
     * Returns the array index of the listener that listens
     * to the given player.
     */
    /*        static TInt FindPlayerInArray(
              CArrayFix< CAMMSPlayerStateListener >* aArray, CMMAPlayer* aPlayer );
    */

public:  // From MMMAPlayerStateListener

    void StateChanged(TInt aState);

private:    // Data

    // Player to be listened, not owned.
    CMMAPlayer* iPlayer;

    // Control group to be informed when player state changes, not owned.
    CAMMSControlGroup* iControlGroup;

};

#endif // CAMMSPLAYERSTATELISTENER_H


