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

public class DistanceAttenuationControl
        extends com.nokia.amms.control.ControlImpl
        implements javax.microedition.amms.control.audio3d.DistanceAttenuationControl
{
    private static final int MIN_MINIMUM_DISTANCE = 1;
    private static final int MIN_MAXIMUM_DISTANCE = 1;
    private static final int MIN_ROLLOF_FACTOR = 0;
    /**
     * Constructor
     */
    public DistanceAttenuationControl()
    {
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public void setParameters(int aMinDistance,
                              int aMaxDistance,
                              boolean aMuteAfterMax,
                              int aRolloffFactor)
    {
        checkValid();

        if (aMaxDistance <= aMinDistance)
        {
            throw new IllegalArgumentException("maxDistance must be equal or greater than minDistance");
        }
        if (aMinDistance < MIN_MINIMUM_DISTANCE)
        {
            throw new IllegalArgumentException("minDistance must be greater than zero");
        }
        if (aMaxDistance < MIN_MAXIMUM_DISTANCE)
        {
            throw new IllegalArgumentException("maxDistance must be greater than zero");
        }
        if (aRolloffFactor < MIN_ROLLOF_FACTOR)
        {
            throw new IllegalArgumentException("rolloffFactor cannot be less than zero");
        }

        int err = _setParameters(iEventSource, iControlHandle,
                                 aMinDistance, aMaxDistance, aMuteAfterMax, aRolloffFactor);

        NativeError.check(err);
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getMinDistance()
    {
        checkValid();

        // have to ask the native control because of commiting
        return _getMinDistance(iEventSource, iControlHandle);
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getMaxDistance()
    {
        checkValid();

        // have to ask the native control because of commiting
        return _getMaxDistance(iEventSource, iControlHandle);
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public boolean getMuteAfterMax()
    {
        checkValid();

        // have to ask the native control because of commiting
        return _getMuteAfterMax(iEventSource, iControlHandle);
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getRolloffFactor()
    {
        checkValid();

        // have to ask the native control because of commiting
        return _getRolloffFactor(iEventSource, iControlHandle);
    }

    // native functions
    private static native int _setParameters(
        int aEventSource,
        int aControlHandle,
        int aMinDistance,
        int aMaxDistance,
        boolean aMuteAfterMax,
        int aRolloffFactor);
    private static native int _getMaxDistance(
        int aEventSource,
        int aControlHandle);
    private static native int _getMinDistance(
        int aEventSource,
        int aControlHandle);
    private static native boolean _getMuteAfterMax(
        int aEventSource,
        int aControlHandle);
    private static native int _getRolloffFactor(
        int aEventSource,
        int aControlHandle);
}
