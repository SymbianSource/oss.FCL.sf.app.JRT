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

public class LocationControl
        extends com.nokia.amms.control.ControlImpl
        implements javax.microedition.amms.control.audio3d.LocationControl
{
    private static final int MIN_RADIUS = 0;

    /**
     * Constructor
     */
    public LocationControl()
    {
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public void setCartesian(int aX, int aY, int aZ)
    {
        checkValid();

        int err = _setCartesian(iEventSource, iControlHandle, aX, aY, aZ);
        NativeError.check(err);
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public void setSpherical(int aAzimuth, int aElevation, int aRadius)
    {
        checkValid();

        if (aRadius < MIN_RADIUS)
        {
            throw new IllegalArgumentException("Radius cannot be less than zero");
        }

        int err = _setSpherical(
                      iEventSource,
                      iControlHandle,
                      aAzimuth,
                      aElevation,
                      aRadius);

        NativeError.check(err);
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public int[] getCartesian()
    {
        checkValid();

        int[] error = new int[ 1 ];
        int[] location = _getCartesian(iEventSource, iControlHandle, error);
        if (location == null)
        {
            throw new OutOfMemoryError("Unable to obtain coordinates");
        }

        NativeError.check(error[ 0 ]);

        return location;
    }

    private static native int[] _getCartesian(
        int aEventSource,
        int aControlHandle,
        int[] aError);
    private static native int _setCartesian(
        int aEventSource,
        int aControlHandle,
        int x,
        int y,
        int z);
    private static native int _setSpherical(
        int aEventSource,
        int aControlHandle,
        int azimuth,
        int elevation,
        int radius);
}
