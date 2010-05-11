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
* Description:  PitchControl controls the pitch.
*
*/

package com.nokia.microedition.media.control;

/**
 * TempoControl or RateControl methods cannot throw any exceptions.
 * If an error occurs in native side, native side return default value.
 */
public class PitchControl extends ControlImpl
        implements javax.microedition.media.control.PitchControl
{
    // these constants come from native side
    protected static final int NO_ERROR = 0; // native side returns if OK
    protected static final int PITCH_OFFSET = 1000000;

    /**
     * @see PitchControl
     */
    public int setPitch(int aMillisemitones)
    {
        checkState();
        int pitch =
            _setPitch(iControlHandle, iEventSource, aMillisemitones);
        if (pitch < NO_ERROR)
        {
            throw new Error("setPitch() failed, Symbian OS error: "
                            + pitch);
        }
        return pitch - PITCH_OFFSET;
    }

    /**
     * @see PitchControl
     */
    public int getPitch()
    {
        checkState();
        int pitch = _getPitch(iControlHandle, iEventSource);
        if (pitch < NO_ERROR)
        {
            throw new Error(
                "getPitch() failed, SymbianOS error: " + pitch);
        }
        return pitch - PITCH_OFFSET;
    }

    /**
     * @see PitchControl
     */
    public int getMaxPitch()
    {
        checkState();
        int pitch = _getMaxPitch(iControlHandle, iEventSource);
        if (pitch < NO_ERROR)
        {
            throw new Error(
                "getMaxPitch() failed, SymbianOS error: "
                + pitch);
        }
        return pitch - PITCH_OFFSET;
    }

    /**
     * @see PitchControl
     */
    public int getMinPitch()
    {
        checkState();
        int pitch = _getMinPitch(iControlHandle, iEventSource);
        if (pitch < NO_ERROR)
        {
            throw new Error(
                "getMinPitch() failed, SymbianOS error: "
                + pitch);
        }
        return pitch - PITCH_OFFSET;
    }

    /**
     * Native implementation.
     *
     * @param aControlHandle Pointer to the native PitchControl object.
     * @param aEventSourceHandle Pointer to the native event source object.
     * @param aMillisemitones The number of semi tones to raise the playback
     * pitch.
     * @return The actual pitch raise set
     */
    private native int _setPitch(int aControlHandle,
                                 int aEventSourceHandle,
                                 int aMillisemitones);

    /**
     * Native implementation.
     *
     * @param aControlHandle Pointer to the native PitchControl object.
     * @param aEventSourceHandle Pointer to the native event source object.
     * @return the current playback pitch raise in "milli-semitones".
     */
    private native int _getPitch(int aControlHandle,
                                 int aEventSourceHandle);


    /**
     * Native implementation.
     *
     * @param aControlHandle Pointer to the native PitchControl object.
     * @param aEventSourceHandle Pointer to the native event source object.
     * @return the maximum pitch raise in "milli-semitones".
     */
    private native int _getMaxPitch(int aControlHandle,
                                    int aEventSourceHandle);

    /**
     * Native implementation.
     *
     * @param aControlHandle Pointer to the native PitchControl object.
     * @param aEventSourceHandle Pointer to the native event source object.
     * @return the minimum pitch raise in "milli-semitones".
     */
    private native int _getMinPitch(int aControlHandle,
                                    int aEventSourceHandle);

}
