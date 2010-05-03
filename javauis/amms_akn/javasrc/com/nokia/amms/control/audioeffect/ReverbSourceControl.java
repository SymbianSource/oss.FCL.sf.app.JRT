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

package com.nokia.amms.control.audioeffect;

import javax.microedition.media.MediaException;
import com.nokia.mj.impl.rt.legacy.NativeError;

public class ReverbSourceControl
        extends com.nokia.amms.control.ControlImpl
        implements javax.microedition.amms.control.audioeffect.ReverbSourceControl
{

    /**
     * Constructor
     */
    public ReverbSourceControl()
    {
    }

    /**
      * Please refer to JSR 234 for more details.
      */
    public void setRoomLevel(int aLevel)
    throws IllegalArgumentException, MediaException
    {
        checkValid();

        if ((aLevel > 0) &&
                (aLevel != DISCONNECT))
        {
            throw new IllegalArgumentException("Room level must be less than zero");
        }

        int level = aLevel;
        int err = _setRoomLevel(iEventSource, iControlHandle, level);

        if (err < NO_ERROR)
        {
            if (err == NOT_READY)
            {
                NativeError.check(err);
            }
            else
            {
                throw new MediaException(
                    "Room level cannot be set: Symbian OS error " + err);
            }
        }
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getRoomLevel()
    {
        checkValid();

        return _getRoomLevel(iEventSource, iControlHandle);
    }

    // native methods
    private static native int _setRoomLevel(
        int aEventSource,
        int aControlHandle,
        int aLevel);

    private static native int _getRoomLevel(
        int aEventSource,
        int aControlHandle);
}
