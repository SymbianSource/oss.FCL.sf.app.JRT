/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Control for routing the player's audio output.
*
*/
package com.nokia.mid.impl.media;

import com.nokia.microedition.media.NativeError;
import javax.microedition.media.Player;
import java.util.Hashtable;
import java.util.Enumeration;
import java.util.*;
import com.nokia.mid.impl.media.AudioOutput;

/**
 * Control for adjusting player's Audio output routing. Note : This doesn't have any interface defined in javax.microediton.amms.control package
 * as its internal to nokia
 */
public class AudioOutputControl
        extends com.nokia.amms.control.ControlImpl
        implements com.nokia.mid.media.AudioOutputControl
{
    // The player owning this control.
    private Player iPlayer;
    private AudioOutput audioOutputObj;
    /**
     * Constructor
     */
    public AudioOutputControl(Player aPlayer)
    {
        iPlayer = aPlayer;
    }

    public void SetAudioOutputToNative()
    {
        // Create an AudioOutput object and pass it to native
        audioOutputObj = new AudioOutput(AudioOutputControl.DEFAULT);
        setAudioOutputObject(audioOutputObj);
    }
    /**
     * Sets the new mode.
     * Throws IllegalArgumentException if the argument is invalid
     * @param mode The new Output mode to be set.
     */
    public int setOutputMode(int mode)
    {
        checkValid();
        int value = _setOutputMode(iEventSource, iControlHandle, mode);
        NativeError.check(value);
        return value;
    }

    /**
     * Gets the previous set mode by user.
     *
     * @return The last set user mode .
     */
    public int getOutputMode()
    {
        checkValid();
        int value = _getOutputMode(iEventSource, iControlHandle);
        return value;
    }

    /**
    * Gets the actual routing at that time.
    * @return The current preference.
    */

    public com.nokia.mid.media.AudioOutput getCurrent()
    {
        checkValid();
        int value = _getCurrent(iEventSource, iControlHandle);
        NativeError.check(value);
        return audioOutputObj;
    }

    /**
     * @return all the available output modes.
     */
    public int[] getAvailableOutputModes()
    {
        checkValid();
        int[] availableOutputModes = { DEFAULT, ALL, NONE, PRIVATE, PUBLIC };
        return availableOutputModes;
    }


    public void checkValid()
    {
        super.checkValid();

        if (iPlayer.getState() == Player.CLOSED)
        {
            throw new IllegalStateException("Closed player");
        }
    }
    /**
    * pass the AudioOutput object to native
    */
    public void setAudioOutputObject(AudioOutput obj)
    {
        checkValid();
        int value = _setAudioOutputObject(iEventSource, iControlHandle, obj);
        NativeError.check(value);
    }


    // native methods
    private static native int _setOutputMode(
        int aEventSource,
        int aControlHandle,
        int aMode);
    private static native int _getOutputMode(
        int aEventSource,
        int aControlHandle);
    private static native int _getCurrent(
        int aEventSource,
        int aControlHandle);
    private static native int _setAudioOutputObject(
        int aEventSource, int aControlHandle,
        AudioOutput audioOutputObj);

}
