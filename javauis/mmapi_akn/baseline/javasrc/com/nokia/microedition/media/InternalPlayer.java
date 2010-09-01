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
* Description:  InternalPlayer
*
*/

package com.nokia.microedition.media;

import javax.microedition.media.Control;
import javax.microedition.media.Player;
import javax.microedition.media.MediaException;

/**
 * Internal inteface for MMA players. All players used in MMA must implement
 * this interface.
 */
public interface InternalPlayer extends Player
{
    /**
     * Adds new <code>Control</code> to player.
     * @param aControl New <code>Control</code> to add.
     * @param aControlType Fully-qualified name of the control class.
     * @exception MediaException Thrown if new Control cannot be added to
     * <code>InternalPlayer</code>.
     */
    void addControl(Control aControl, String aControlType)
    throws MediaException;

}
