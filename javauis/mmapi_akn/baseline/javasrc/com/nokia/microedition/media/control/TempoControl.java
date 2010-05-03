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
* Description:  TempoControl is used to control the tempo.
*
*/

package com.nokia.microedition.media.control;
import javax.microedition.media.Player;

/**
 * TempoControl or RateControl methods cannot throw any exceptions.
 * If an error occurs in native side, native side return default value.
 */
public class TempoControl extends RateControl
        implements javax.microedition.media.control.TempoControl
{
    // these constants come from native side
    protected static final int NO_ERROR = 0; // native side returns if OK

    // Tempo holder for PREFETCHED state. Initially set to default value.
    private static final int DEFAULT_TEMPO = 120000;

    /**
     * @see TempoControl
     */
    public int setTempo(int aMilliTempo)
    {
        checkState();
        // _setTempo returns real value (STARTED) or user set value
        // (PREFETCHED). In latter case user set value is adjusted
        // in native side according to max/min bounds of tempo.
        int tempo = _setTempo(iControlHandle, iEventSource, aMilliTempo);

        if (tempo < NO_ERROR)
        {
            throw new Error(
                "setTempo() failed, Symbian OS error: " + tempo);
        }
        return tempo;
    }

    /**
     * @see TempoControl
     */
    public int getTempo()
    {
        checkState();

        int tempo = _getTempo(iControlHandle, iEventSource);
        return tempo;
    }


    /**
     * Native implementation.
     *
     * @param aControlHandle Pointer to the native TempoControl object.
     * @param aEventSourceHandle Pointer to the native event source object.
     * @param aMilliTempo The tempo specified in milli-beats per minute
     * @return The actual tempo set.
     */
    private native int _setTempo(int aControlHandle,
                                 int aEventSourceHandle,
                                 int aMilliTempo);

    /**
     * Native implementation.
     *
     * @param aControlHandle Pointer to the native TempoControl object.
     * @param aEventSourceHandle Pointer to the native event source object.
     * @return The tempo.
     */
    private native int _getTempo(int aControlHandle,
                                 int aEventSourceHandle);

}
