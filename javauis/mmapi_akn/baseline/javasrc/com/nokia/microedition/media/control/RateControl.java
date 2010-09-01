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
* Description:  RateControl controls the rate.
*
*/

package com.nokia.microedition.media.control;
import javax.microedition.media.Player;

/**
 * RateControl or RateControl methods cannot throw any exceptions.
 * If an error occurs in native side, native side return default value.
 */
public class RateControl extends ControlImpl
        implements javax.microedition.media.control.RateControl
{
    // these constants come from native side
    protected static final int NO_ERROR = 0; // native side returns if OK

    /**
     * @see RateControl
     */
    public int setRate(int aMilliRate)
    {
        checkState();
        int rate = _setRate(iControlHandle, iEventSource, aMilliRate);
        if (rate < NO_ERROR)
        {
            throw new Error(
                "setRate() failed, Symbian OS error: " + rate);
        }
        return rate;
    }

    /**
     * @see RateControl
     */
    public int getRate()
    {
        checkState();
        int rate = _getRate(iControlHandle, iEventSource);
        if (rate < NO_ERROR)
        {
            throw new Error(
                "getRate() failed, Symbian OS error: " + rate);
        }
        return rate;
    }

    /**
     * @see RateControl
     */
    public int getMaxRate()
    {
        checkState();
        int rate = _getMaxRate(iControlHandle, iEventSource);
        if (rate < NO_ERROR)
        {
            throw new Error(
                "getMaxRate() failed, Symbian OS error: "
                + rate);
        }
        return rate;
    }

    /**
     * @see RateControl
     */
    public int getMinRate()
    {
        checkState();
        int rate = _getMinRate(iControlHandle, iEventSource);
        if (rate < NO_ERROR)
        {
            throw new Error(
                "getMinRate() failed, Symbian OS error: "
                + rate);
        }
        return rate;
    }


    /**
     * Native implementation.
     *
     * @param aControlHandle Pointer to the native RateControl object.
     * @param aEventSourceHandle Pointer to the native event source object.
     * @param aMilliRate The playback rate to set
     * @return The actual rate set.
     */
    private native int _setRate(int aControlHandle,
                                int aEventSourceHandle,
                                int aMilliRate);

    /**
     * Native implementation.
     *
     * @param aControlHandle Pointer to the native RateControl object.
     * @param aEventSourceHandle Pointer to the native event source object.
     * @return The current playback rate.
     */
    private native int _getRate(int aControlHandle,
                                int aEventSourceHandle);

    /**
     * Native implementation.
     *
     * @param aControlHandle Pointer to the native RateControl object.
     * @param aEventSourceHandle Pointer to the native event source object.
     * @return The maximum rate.
     */
    private native int _getMaxRate(int aControlHandle,
                                   int aEventSourceHandle);

    /**
     * Native implementation.
     *
     * @param aControlHandle Pointer to the native RateControl object.
     * @param aEventSourceHandle Pointer to the native event source object.
     * @return The minimum rate.
     */
    private native int _getMinRate(int aControlHandle,
                                   int aEventSourceHandle);

}
