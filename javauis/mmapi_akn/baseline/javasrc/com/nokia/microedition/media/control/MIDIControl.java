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
* Description:  MIDIControl.
*
*/


package com.nokia.microedition.media.control;

import javax.microedition.media.MediaException;
import javax.microedition.media.Player;


public class MIDIControl extends ControlImpl
        implements javax.microedition.media.control.MIDIControl
{
    // these constants come from native side
    protected static final int NO_ERROR = 0; // native side returns if OK
    protected static final int NOT_FOUND = -1;

    /**
     * Max data value for midi events.
     */
    private static final int MAX_VALUE = 127;

    /**
     * Min data value for midi event types.
     */
    private static final int MIN_TYPE = 0x80;

    /**
     * Max data value for midi event types.
     */
    private static final int MAX_TYPE = 0xFF;

    /**
     * Midi type reserved for system.
     */
    private static final int RESERVED_TYPE_A = 0xF0;

    /**
     * Midi type reserved for system.
     */
    private static final int RESERVED_TYPE_B = 0xF7;

    /**
     * Max channel number.
     */
    private static final int MAX_CHANNEL = 15;

    /**
     * Max bank number.
     */
    private static final int MAX_BANK = 16383;

    /**
     * Constant to use the default bank. Also the minimum bank value accepted.
     */
    private static final int DEFAULT_BANK = -1;

    /**
     * Constant value for the default bank.
     */
    private static final int DEFAULT_BANK_NUMBER = 0x3c80;

    /**
     * Constant MIDI control ID value for volume.
     */
    private static final int CONTROL_VOLUME = 0x07;

    /**
     * @see MIDIControl
     */
    public boolean isBankQuerySupported()
    {
        return false;
    }

    /**
     * @see MIDIControl
     */
    public int[] getProgram(int aChannel)
    throws MediaException
    {
        checkPlayerState();
        checkChannel(aChannel);
        // this method is not currently supported
        throw new MediaException("Not supported");
    }

    /**
     * @see MIDIControl
     */
    public int getChannelVolume(int aChannel)
    {
        checkPlayerState();
        checkChannel(aChannel);

        // Native side returns -1 if current volume cannot be
        // determined for sure

        int volume =
            _getChannelVolume(iControlHandle, iEventSource, aChannel);

        // return 0-127, or -1 if not known
        if (volume < NOT_FOUND)
        {
            volume = NOT_FOUND;
        }
        return volume;
    }

    /**
     * @see MIDIControl
     */
    public void setProgram(int aChannel, int aBank, int aProgram)
    {
        checkPlayerState();
        checkChannel(aChannel);
        checkBank(aBank);
        checkProgram(aProgram);

        // Handle query for default bank
        if (aBank == DEFAULT_BANK)
        {
            aBank = DEFAULT_BANK_NUMBER;
        }

        int err = _setProgram(iControlHandle, iEventSource, aChannel, aBank, aProgram);

        if (err < NO_ERROR)
        {
            throw new Error("setProgram() failed, Symbian OS error: " + err);
        }
    }

    /**
     * @see MIDIControl
     */
    public synchronized void setChannelVolume(int aChannel, int aVolume)
    {
        checkPlayerState();
        checkChannel(aChannel);

        // Volume must be between 0 - MAX_VALUE
        if (aVolume < 0 ||
                aVolume > MAX_VALUE)
        {
            throw new IllegalArgumentException("Invalid volume " + aVolume +
                                               ", valid range is  0 <= volume <= 127");
        }

        int err = _setChannelVolume(iControlHandle, iEventSource, aChannel, aVolume);
        if (err < NO_ERROR)
        {
            throw new Error("setChannelVolume() failed, SymbianO S error: " + err);
        }
    }

    /**
     * @see MIDIControl
     */
    public int[] getBankList(boolean aCustom)
    throws MediaException
    {
        checkPlayerState();
        // this method is not currently supported
        throw new MediaException("Not supported");
    }

    /**
     * @see MIDIControl
     */
    public int[] getProgramList(int aBank)
    throws MediaException
    {
        checkPlayerState();
        checkBank(aBank);
        // this method is not currently supported
        throw new MediaException("Not supported");
    }

    /**
     * @see MIDIControl
     */
    public String getProgramName(int aBank, int aProg)
    throws MediaException
    {
        checkPlayerState();
        checkBank(aBank);
        checkProgram(aProg);
        // this method is not currently supported
        throw new MediaException("Not supported");
    }

    /**
     * @see MIDIControl
     */
    public String getKeyName(int aBank, int aProg, int aKey)
    throws MediaException
    {
        checkPlayerState();
        checkBank(aBank);
        checkProgram(aProg);
        checkKey(aKey);
        // this method is not currently supported
        throw new MediaException("Not supported");
    }

    /**
     * Short midi events are delegated to the longMidiEvent method.
     *
     * @see MIDIControl
     */
    public void shortMidiEvent(int aType, int aData1, int aData2)
    {
        checkPlayerState();

        // Data parameters must be between 0 to MAX_VALUE
        if (aData1 < 0 ||
                aData1 > MAX_VALUE ||
                aData2 < 0 ||
                aData2 > MAX_VALUE)
        {
            throw new IllegalArgumentException(
                "Invalid data, valid range is  0 <= data <= 127");
        }

        // Check type:
        // must be MIN_TYPE..MAX_TYPE, excluding RESERVED_TYPE_A and
        // RESERVED_TYPE_B, which are reserved for system exclusive.
        if (aType == RESERVED_TYPE_A ||
                aType == RESERVED_TYPE_B ||
                aType < MIN_TYPE ||
                aType > MAX_TYPE)
        {
            throw new IllegalArgumentException("Invalid midi type " + aType +
                                               ", valid range is  0x80 <= type <= 0xFF or 0xF0 or 0xF7");
        }

        // Format to long MIDI event
        byte[] data = new byte[ 3 ];

        // aType and datas are checked to fit in a byte range 0..127
        data[ 0 ] = (byte)aType;
        data[ 1 ] = (byte)aData1;
        data[ 2 ] = (byte)aData2;

        // Send short midi event as a long midi event.
        // Length of short midi event is 3.
        longMidiEvent(data, 0, 3);
    }

    /**
     * @see MIDIControl
     */
    public int longMidiEvent(byte[] aData, int aOffset, int aLength)
    {
        checkPlayerState();

        // Check parameters
        if ((aData == null) ||
                (aData.length < aOffset + aLength) ||   // Do not exceed array length
                (aOffset < 0) ||   // Offset may not be negative
                (aLength < 0))     // Length may not be negative
        {
            throw new IllegalArgumentException("Illegal array");
        }

        // Zero length, just return
        if (aLength == 0)
        {
            return 0;
        }

        // Send to the native side.
        int retVal = _sendMidiEvent(iControlHandle,
                                    iEventSource,
                                    aData,
                                    aOffset,
                                    aLength);

        if (retVal < NO_ERROR)
        {
            return -1;
        }

        return retVal;
    }

    /**
     * Restarts native MIDI engine and initializes it with specified sequence.
     * For internal use.
     */
    public void reInitializeMidi(byte[] aMidiSequence, int aOffset, int aLength)
    {
        int retVal = _reInitializeMidi(iControlHandle,
                                       iEventSource,
                                       aMidiSequence,
                                       aOffset,
                                       aLength);
        if (retVal < NO_ERROR)
        {
            throw new IllegalStateException();
        }

    }

    /**
     * Checks program parameter.
     */
    private void checkProgram(int aProgram)
    {
        // Program must be between 0 - MAX_VALUE
        if (aProgram < 0 ||
                aProgram > MAX_VALUE)
        {
            throw new IllegalArgumentException("Invalid program "
                                               + aProgram + ", valid range is  0 <= program <= 127");
        }
    }

    /**
     * Checks bank parameter.
     */
    private void checkBank(int aBank)
    {
        // Bank must be between DEFAULT_BANK - MAX_BANK
        if (aBank < DEFAULT_BANK ||
                aBank > MAX_BANK)
        {
            throw new IllegalArgumentException("Invalid bank "
                                               + aBank + ", valid range is  1 <= bank <= 16383");
        }
    }

    /**
     * Checks channel parameter.
     */
    private void checkChannel(int aChannel)
    {
        // Channel must be between 0 - MAX_CHANNEL
        if (aChannel < 0 ||
                aChannel > MAX_CHANNEL)
        {
            throw new IllegalArgumentException("Invalid channel "
                                               + aChannel + ",  valid range is  0 <= channel <= 15");
        }
    }

    /**
     * Checks key number parameter.
     */
    private void checkKey(int aKey)
    {
        // Channel must be between 0 - MAX_VALUE
        if (aKey < 0 ||
                aKey > MAX_VALUE)
        {
            throw new IllegalArgumentException("Invalid key "
                                               + aKey + ", valid range is  0 <= key <= 127");
        }
    }

    /**
     * Checks that player has been prefetched.
     */
    private void checkPlayerState()
    {
        // Player must be in PREFETCHED or STARTED state.
        int state = iPlayer.getState();
        if (state != Player.PREFETCHED &&
                state != Player.STARTED)
        {
            throw new IllegalStateException(
                "Player must be in PREFETCHED or STARTED state");
        }
    }

    //
    // Native methods
    //

    /**
     * Native implementation.
     *
     * @param aControlHandle Pointer to the native MIDIControl object.
     * @param aEventSourceHandle Pointer to the native event source object.
     * @param aChannel number of channel which is subject to program change.
     * @param aBank bank to be set.
     * @param aProgram program to be set.
     * @return error code. Negative is error occured.
     */
    private native int _setProgram(int aControlHandle,
                                   int aEventSourceHandle,
                                   int aChannel,
                                   int aBank,
                                   int aProgram);

    /**
     * Native implementation.
     *
     * @param aControlHandle Pointer to the native MIDIControl object.
     * @param aEventSourceHandle Pointer to the native event source object.
     * @param aChannel channel which volume to get
     * @return The channel volume. -1 if an error occured or if not known.
     */
    private native int _getChannelVolume(int aControlHandle,
                                         int aEventSourceHandle,
                                         int aChannel);

    /**
     * Native Implementation.
     * @param aControlHandle Pointer to the native MIDIControl object.
     * @param aEventSourceHandle Pointer to the native event source object.
     * @param aChannel channel which volume to set
     * @param aVolume volume to set
     */
    private native int _setChannelVolume(int aControlHandle,
                                         int aEventSource,
                                         int aChannel,
                                         int aVolume);

    /**
     * Native implementation. Array length will asserted only in debug builds.
     *
     * @param aControlHandle Pointer to the native MIDIControl object.
     * @param aEventSourceHandle Pointer to the native event source object.
     * @param aData Array of the bytes to send.
     * @param aOffset Start offset in data array.
     * @param aLength Number of bytes to be sent.
     * @return The number of bytes actually sent to the device.
     *         -1 if an error occured.
     *         KErrArgument if IllegalArgument exception must be thrown
     */
    private native int _sendMidiEvent(int aControlHandle,
                                      int aEventSourceHandle,
                                      byte[] aData,
                                      int aOffset,
                                      int aLength);

    /**
     * Native implementation.
     *
     * @param aControlHandle Pointer to the native MIDIControl object.
     * @param aEventSourceHandle Pointer to the native event source object.
     * @param aMidiSequence Array of the bytes containing midi sequence
     * for initialization
     * @param aOffset Start offset in data array.
     * @param aLength Number of bytes to be sent.
     * @return Native error code.
     */
    private native int _reInitializeMidi(int aControlHandle,
                                         int aEventSourceHandle,
                                         byte[] aMidiSequence,
                                         int aOffset,
                                         int aLength);
}
