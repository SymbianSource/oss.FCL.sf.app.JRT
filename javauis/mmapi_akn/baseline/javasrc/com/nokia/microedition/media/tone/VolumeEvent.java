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
* Description:  Event processor class for tone sequence VOLUME events
*
*/


package com.nokia.microedition.media.tone;

import javax.microedition.media.control.ToneControl;

/**
 * Event processor class for tone sequence VOLUME events
 */
public class VolumeEvent extends Event
{
    /**
     * ToneEvent constructor
     * @param aSequence tone sequence byte array (input)
     * @param aMidiSequence midi sequence object where to output midi events.
     */
    VolumeEvent(byte[] aSequence, MidiSequence aMidiSequence)
    {
        super(aSequence, aMidiSequence);
    }

    /**
     * Inherited from Event
     */
    public int advance(int aPosition) throws MidiSequenceException
    {
        int retVal = doValidate(aPosition);
        {
            if (retVal == 0)
            {
                return 0;
            }
        }
        // it is already checked that there is at least two bytes left
        byte type = iSequence[ aPosition ];
        byte data = iSequence[ aPosition + 1 ];

        // calculate equivalent midi TONE_VOLUME
        float vol = (float)MidiToneConstants.MIDI_MAX_VOLUME /
                    (float)MidiToneConstants.TONE_MAX_VOLUME * (float)data;

        byte volume = (byte) vol;
        if (volume < MidiToneConstants.MIDI_MIN_VOLUME)
        {
            volume = MidiToneConstants.MIDI_MIN_VOLUME;
        }

        if (volume > MidiToneConstants.MIDI_MAX_VOLUME)
        {
            volume = MidiToneConstants.MIDI_MAX_VOLUME;
        }
        // write TONE_VOLUME change on delta time 0
        iMidiSequence.writeMidiEvent(
            0,
            MidiToneConstants.MIDI_CONTROL_CHANGE,
            MidiToneConstants.MIDI_CONTROL_MAIN_VOLUME,
            volume);

        // N.B.! Above MIDI_CONTROL_CHANGE can be written without channel
        // value because MidiSequence will attach correct channel value to them anyway.
        return EVENT_SIZE;
    }

    /**
     * Child class defined functionality for validate
     * @param aPosition position in tone sequence array where to validate
     * @return new position offset related to aPosition in tone sequence
     * array. Must be positive.
     */
    protected int doValidate(int aPosition)
    throws IllegalArgumentException
    {
        byte type = iSequence[ aPosition ];
        byte data = iSequence[ aPosition + 1 ];
        int retVal = 0;
        if (type == ToneControl.SET_VOLUME)
        {
            if (data < MidiToneConstants.TONE_MIN_VOLUME  ||
                    data > MidiToneConstants.TONE_MAX_VOLUME)
            {
                throw new IllegalArgumentException(
                    "Volume is out of range, valid range is  0 <= volume <= 100");
            }
            retVal = EVENT_SIZE;
        }
        return retVal;
    }

    /**
     * Child class defined functionality for checkEventAtNextPosition
     * @param aPosition position in tone sequence array where to check
     */
    protected void checkEventAtNextPosition(int aPosition)
    throws IllegalArgumentException
    {
        // After this event there can be:
        // Tone, BlockEnd, PlayBlock, Volume, Repeat or
        // end of sequence

        int type = 0;
        try
        {
            type = iSequence[ aPosition ];
        }
        catch (ArrayIndexOutOfBoundsException aioobe)
        {
            return; // end of sequence is ok for this event
        }

        if (type >= ToneControl.SILENCE ||
                type == ToneControl.BLOCK_END ||
                type == ToneControl.PLAY_BLOCK ||
                type == ToneControl.SET_VOLUME ||
                type == ToneControl.REPEAT)
        {
            return;
        }
        throw new IllegalArgumentException(
            "Illegal event found; sequence is corrupted");
    }

} // end of class

