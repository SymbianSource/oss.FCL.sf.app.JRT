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


package com.nokia.amms.control;

import com.nokia.amms.AMMSError;

public class VolumeControl extends ControlImpl
        implements javax.microedition.media.control.VolumeControl
{
    private static final int MAX_VOLUME = 100;
    private int iLevel = MAX_VOLUME;
    private boolean iMuted = false;

    /**
     * Constructor
     */
    public VolumeControl()
    {
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public boolean isMuted()
    {
        checkValid();
        return iMuted;
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getLevel()
    {
        checkValid();
        return iLevel;
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public int setLevel(int aLevel)
    {
        checkValid();
        int level = aLevel;

        // Set level to between 0 and 100
        if (level < 0)
        {
            level = 0;
        }
        else if (level > MAX_VOLUME)
        {
            level = MAX_VOLUME;
        }

        if (!iMuted)
        {
            AMMSError.check(_setLevel(iEventSource,
                                      iControlHandle,
                                      level));
        }
        iLevel = level;
        return level;
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public void setMute(boolean aMuted)
    {
        checkValid();

        // Check if mute status is changed
        if (aMuted != iMuted)
        {
            iMuted = aMuted;

            int level = iLevel;

            // If mute is on set volume to 0, otherwise set to level set before
            if (iMuted)
            {
                level = 0;
            }

            AMMSError.check(_setLevel(iEventSource,
                                      iControlHandle,
                                      level));
        }
    }

    /**
     * @param aEventSourceHandle Handle to native event source.
     * @param aControlHandle Handle to native control.
     * @param aLevel New volume level.
     */
    private static native int _setLevel(int aEventSourceHandle,
                                        int aControlHandle,
                                        int aLevel);
}
