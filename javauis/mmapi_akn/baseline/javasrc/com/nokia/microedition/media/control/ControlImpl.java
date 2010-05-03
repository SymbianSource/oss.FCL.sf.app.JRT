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
* Description:  This class is an implementation of control
*
*/

package com.nokia.microedition.media.control;

import javax.microedition.media.Player;

public class ControlImpl implements javax.microedition.media.Control
{
    protected Player iPlayer;
    protected int iControlHandle;
    protected int iEventSource;

    /**
     * Constructor
     */
    public ControlImpl()
    {
    }

    /**
     * Called when the Player is disposed
     */
    public void notifyDispose()
    {
    }

    /**
     * Set handles
     * @param aPlayer Player instance
     * @param aEventSource Handle to event source
     * @param aControlHandle Handle Handle to control
     */
    public void setHandles(Player aPlayer,
                           int aEventSource,
                           int aControlHandle)
    {
        iPlayer = aPlayer;
        iEventSource = aEventSource;
        iControlHandle = aControlHandle;

        _setHandle(aEventSource, aControlHandle, this);
    }

    /**
     * Throws IllegalStateException if player is closed and control
     * cannot be used anymore.
     * @throws IllegalStateException if Player is closed.
     */
    protected void checkState()
    {
        if (iPlayer.getState() == Player.CLOSED)
        {
            throw new IllegalStateException("Player is CLOSED.");
        }
    }

    private static native void _setHandle(int aEventSourceHandle,
                                          int aControlHandle,
                                          Object aControl);
}
