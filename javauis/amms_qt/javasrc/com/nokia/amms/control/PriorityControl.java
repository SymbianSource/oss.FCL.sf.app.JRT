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
* Description:  Control for adjusting player priorities.
*
*/

package com.nokia.amms.control;

import com.nokia.microedition.media.NativeError;
import javax.microedition.media.Player;

/**
 * Control for adjusting player priorities.
 */
public class PriorityControl
        extends ControlImpl
        implements javax.microedition.amms.control.PriorityControl
{
    private static final int MIN_PRIORITY = 0;
    private static final int MAX_PRIORITY = 100;

    // The player owning this control.
    private Player iPlayer;


    /**
     * Constructor
     */
    public PriorityControl(Player aPlayer)
    {
        iPlayer = aPlayer;
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public void setPriority(int aPriority)
    {
        checkValid();

        int priority = aPriority;
        // Panning value that is out of range is set to the valid range.
        if (aPriority < MIN_PRIORITY ||
                aPriority > MAX_PRIORITY)
        {
            throw new IllegalArgumentException(
                "Priority must be between 0 and 100");
        }

        // Priority setting in native side returns a system wide error code.
        int error = _setPriority(iEventSource, iControlHandle, aPriority);
        NativeError.check(error);
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getPriority()
    {
        checkValid();

        int value = _getPriority(iEventSource, iControlHandle);

        return value;
    }

    public void checkValid()
    {
        super.checkValid();

        if (iPlayer.getState() == Player.CLOSED)
        {
            throw new IllegalStateException("Player is closed.");
        }
    }

    // native methods
    private static native int _setPriority(
        int aEventSource,
        int aControlHandle,
        int aPriority);
    private static native int _getPriority(
        int aEventSource,
        int aControlHandle);
}
