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
import com.nokia.microedition.media.NativeError;

public class ReverbControl
        extends com.nokia.amms.control.EffectControl
        implements javax.microedition.amms.control.audioeffect.ReverbControl
{
    private static final int MAX_REVERB_LEVEL = 0;
    private static final int MIN_REVERB_TIME = 0;

    /**
     * Constructor
     */
    public ReverbControl()
    {
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public int setReverbLevel(int aLevel) throws IllegalArgumentException
    {
        checkValid();

        if (aLevel > MAX_REVERB_LEVEL)
        {
            throw new IllegalArgumentException("Reverb level cannot be greater than zero.");
        }

        int err[] = new int[ 1 ];
        int setLevel = _setReverbLevel(iEventSource,
                                       iControlHandle,
                                       aLevel,
                                       err);
        NativeError.check(err[ 0 ]);

        return setLevel;
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getReverbLevel()
    {
        checkValid();

        int level = _getReverbLevel(iEventSource, iControlHandle);

        return level;
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public void setReverbTime(int aTime)
    throws IllegalArgumentException, MediaException
    {
        checkValid();

        if (aTime < MIN_REVERB_TIME)
        {
            throw new IllegalArgumentException("Reverb time cannot be less than zero");
        }

        int err = _setReverbTime(iEventSource, iControlHandle, aTime);
        if (err < NO_ERROR)
        {
            if (err == NOT_READY)
            {
                NativeError.check(err);
            }
            else
            {
                throw new MediaException(
                    "Setting reverb time failed: Symbian OS error " + err);
            }
        }
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getReverbTime() throws javax.microedition.media.MediaException
    {
        checkValid();

        int time = _getReverbTime(iEventSource, iControlHandle);

        if (time == NOT_SUPPORTED)
        {
            throw new MediaException(
                "Obtaining reverb time failed: Symbian OS error " + time);
        }

        return time;
    }

    // native methods
    private static native int _setReverbLevel(
        int aEventSource,
        int aControlHandle,
        int aLevel,
        int aError[]);
    private static native int _getReverbLevel(
        int aEventSource,
        int aControlHandle);
    private static native int _setReverbTime(
        int aEventSource,
        int aControlHandle,
        int aTime);
    private static native int _getReverbTime(
        int aEventSource,
        int aControlHandle);
}
