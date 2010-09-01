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

import com.nokia.mj.impl.rt.legacy.NativeError;

public class EqualizerControl
        extends com.nokia.amms.control.EffectControl
        implements javax.microedition.amms.control.audioeffect.EqualizerControl
{
    private static final int MAX_LEVEL = 100;
    private static final int MIN_LEVEL = 0;

    /**
     * Constructor
     */
    public EqualizerControl()
    {
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getMinBandLevel()
    {
        checkValid();

        return _getMinBandLevel(iEventSource, iControlHandle);
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getMaxBandLevel()
    {
        checkValid();

        return _getMaxBandLevel(iEventSource, iControlHandle);
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public void setBandLevel(int aLevel, int aBand)
    throws IllegalArgumentException
    {
        checkValid();

        int err = _setBandLevel(iEventSource, iControlHandle, aLevel, aBand);

        // throws IllegalArgumentException on KErrArgument
        NativeError.check(err);
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getBandLevel(int aBand) throws IllegalArgumentException
    {
        checkValid();

        int[] level = new int[ 1 ];

        int error = _getBandLevel(iEventSource, iControlHandle, aBand, level);

        // throws IllegalArgumentException on KErrArgument
        NativeError.check(error);

        return level[ 0 ];
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getNumberOfBands()
    {
        checkValid();

        return _getNumberOfBands(iEventSource, iControlHandle);
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getCenterFreq(int aBand) throws IllegalArgumentException
    {
        checkValid();

        int err = _getCenterFreq(iEventSource, iControlHandle, aBand);
        NativeError.check(err);

        return err;
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getBand(int aFrequency)
    {
        checkValid();

        int band = _getBand(iEventSource, iControlHandle, aFrequency);

        return band;
    }

    /**
     * Please refer to JSR 234 for more details.
    */
    public int setBass(int aLevel) throws IllegalArgumentException
    {
        checkValid();

        if (aLevel < MIN_LEVEL || aLevel > MAX_LEVEL)
        {
            throw new IllegalArgumentException("Bass level out of range");
        }

        int err = _setBass(iEventSource, iControlHandle, aLevel);
        NativeError.check(err);

        return err;
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public int setTreble(int aLevel) throws IllegalArgumentException
    {
        checkValid();

        if (aLevel < MIN_LEVEL || aLevel > MAX_LEVEL)
        {
            throw new IllegalArgumentException("Treble level out of range");
        }

        int err = _setTreble(iEventSource, iControlHandle, aLevel);
        NativeError.check(err);
        return err;
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getBass()
    {
        checkValid();

        return _getBass(iEventSource, iControlHandle);
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getTreble()
    {
        checkValid();

        return _getTreble(iEventSource, iControlHandle);
    }

    private static native int _getMinBandLevel(
        int iEventSource,
        int iControlHandle);
    private static native int _getMaxBandLevel(
        int iEventSource,
        int iControlHandle);
    private static native int _setBandLevel(
        int iEventSource,
        int iControlHandle,
        int level,
        int band);
    private static native int _getBandLevel(
        int iEventSource,
        int iControlHandle,
        int band,
        int[] level);
    private static native int _getCenterFreq(
        int iEventSource,
        int iControlHandle,
        int band);
    private static native int _getBand(
        int iEventSource,
        int iControlHandle,
        int frequency);
    private static native int _setBass(
        int iEventSource,
        int iControlHandle,
        int level);
    private static native int _setTreble(
        int iEventSource,
        int iControlHandle,
        int level);
    private static native int _getBass(
        int iEventSource,
        int iControlHandle);
    private static native int _getTreble(
        int iEventSource,
        int iControlHandle);
    private static native int _getNumberOfBands(
        int iEventSource,
        int iControlHandle);
}
