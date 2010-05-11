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
* Description:
*
*/


package com.nokia.microedition.media.protocol;

import com.nokia.microedition.media.InternalPlayer;
import com.nokia.microedition.media.Locator;
import javax.microedition.media.MediaException;

/**
 * Interface for player creation from locator
 */
public interface Protocol
{
    /**
     * Creates a new instance of the player from given locator
     * @param aLocator Locator for creating Player.
     * @return Fully constructed new Player
     */
    abstract InternalPlayer createPlayer(Locator aLocator)
    throws java.io.IOException,
                MediaException,
                java.lang.SecurityException;
}
