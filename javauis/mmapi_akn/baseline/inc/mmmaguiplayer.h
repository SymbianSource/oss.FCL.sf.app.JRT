/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface for players which shows picture.
*
*/


#ifndef MMMAGUIPLAYER_H
#define MMMAGUIPLAYER_H

#include "cmmaplayerevent.h"

//  FORWARD DECLARATIONS
class MMMADisplay;
class CFbsBitmap;
class MMMASnapshot;
class CMMAEventSource;
class CMIDToolkit;

//  CLASS DEFINITION
/**
    Interface for players which shows picture. Player will be notified
    throw this interface when display state is changed.
*/
NONSHARABLE_CLASS(MMMAGuiPlayer)
{
protected:  // Constructors and destructors
    /**
     * Do not allow delete through this interface
     */
    virtual ~MMMAGuiPlayer() {};

public:     // Abstract methods
    /**
     * Sets display to the player. Player must likely will create window
     * for the display. Called when display is available for the player.
     *
     * @param aDisplay Display which can be used.
     */
    virtual void SetDisplayL(MMMADisplay* aDisplay) = 0;

    /**
     * Returns size of the content. Content size may not available
     * before player is initialized.
     *
     * @return Size of the source content.
     */
    virtual TSize SourceSize() = 0;

    /**
     * Returns inferface that is used for taking snapshots from the player.
     * Method can leave if player creates a snapshoter object when this
     * method is called.
     *
     * @return Interface for taking snaphots.
     */
    virtual MMMASnapshot* SnapshoterL() = 0;

    /**
     * Method sends event to the Java. This can be used to notify errors
     * to the Java side.
     *
     * @param aEventType Type of the event.
     * @param aStringEventData Data of the event.
     */
    virtual void NotifyWithStringEvent(CMMAPlayerEvent::TEventType aEventType,
                                       const TDesC& aStringEventData) = 0;

    // No data in MClass!!

};

#endif // MMMAGUIPLAYER_H
