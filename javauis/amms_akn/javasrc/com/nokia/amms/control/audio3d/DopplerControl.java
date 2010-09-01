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

package com.nokia.amms.control.audio3d;

import com.nokia.mj.impl.rt.legacy.NativeError;

public class DopplerControl
        extends com.nokia.amms.control.ControlImpl
        implements javax.microedition.amms.control.audio3d.DopplerControl
{
    /**
     * Constructor
     */
    public DopplerControl()
    {
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public boolean isEnabled()
    {
        checkValid();

        // have to ask the native control because of commiting
        return _isEnabled(iEventSource, iControlHandle);
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public int[] getVelocityCartesian()
    {
        checkValid();

        int[] error = new int[ 1 ];
        // have to ask the native control because of commiting
        int[] velocity = _getVelocityCartesian(iEventSource,
                                               iControlHandle,
                                               error);
        if (velocity == null)
        {
            // couldn't allocate memory
            throw new OutOfMemoryError("Unable to obatin velocity.");
        }

        NativeError.check(error[ 0 ]);

        return velocity;
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public void setEnabled(boolean aDopplerEnabled)
    {
        checkValid();

        int err = _setEnabled(iEventSource, iControlHandle, aDopplerEnabled);

        NativeError.check(err);
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public void setVelocityCartesian(int aX, int aY, int aZ)
    {
        checkValid();

        int err = _setVelocityCartesian(iEventSource, iControlHandle, aX, aY, aZ);

        NativeError.check(err);
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public void setVelocitySpherical(int aAzimuth, int aElevation, int aRadius)
    {
        checkValid();

        int err = _setVelocitySpherical(
                      iEventSource,
                      iControlHandle,
                      aAzimuth,
                      aElevation,
                      aRadius);

        NativeError.check(err);
    }

    private static native boolean _isEnabled(
        int aEventSource,
        int aControlHandle);
    private static native int[] _getVelocityCartesian(
        int aEventSource,
        int aControlHandle,
        int[] aError);
    private static native int _setEnabled(
        int aEventSource,
        int aControlHandle,
        boolean dopplerEnabled);
    private static native int _setVelocityCartesian(
        int aEventSource,
        int aControlHandle,
        int x,
        int y,
        int z);
    private static native int _setVelocitySpherical(
        int aEventSource,
        int aControlHandle,
        int azimuth,
        int elevation,
        int radius);
}
