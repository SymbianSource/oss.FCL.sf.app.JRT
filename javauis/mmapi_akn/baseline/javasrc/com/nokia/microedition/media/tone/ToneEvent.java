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
* Description:  Event processor class for tone sequence audible note events
*
*/


package com.nokia.microedition.media.tone;

import javax.microedition.media.control.ToneControl;

/**
 * Event processor class for tone sequence note events (both audible & silent)
 */
public class ToneEvent extends Event
{
    /**
     * ToneEvent constructor
     * @param aSequence tone sequence byte array (input)
     * @param aMidiSequence midi sequence object where to output midi events.
     */
    ToneEvent(byte[] aSequence, MidiSequence aMidiSequence)
    {
        super(aSequence, aMidiSequence);
    }

    /**
     * Inherited from Event
     */
    public int advance(int aPosition)
    throws MidiSequenceException
    {
        return staticAdvance(aPosition, iSequence, iMidiSequence);
    }

    /**
     * Static version of advance( ) to be used by RepeatEvent
     */
    public static int staticAdvance(int aPosition,
                                    byte[] aToneSequence,
                                    MidiSequence aMidiSequence)
    throws MidiSequenceException
    {
        int retVal = doStaticValidate(aPosition, aToneSequence);
        if (retVal == 0)
        {
            return 0;
        }
        // it is already checked that there is at least two bytes left
        byte type = aToneSequence[ aPosition ];
        byte data = aToneSequence[ aPosition + 1 ];

        if (type == ToneControl.SILENCE)
        {
            retVal = processToneEvent(type, data, true, aMidiSequence);
        }
        else if (type >= MidiToneConstants.TONE_MIN_NOTE &&
                 type <= MidiToneConstants.TONE_MAX_NOTE)
        {
            retVal = processToneEvent(type, data, false, aMidiSequence);
        }
        return retVal;
    }

    /**
     * Child class defined functionality for validate
     * @param aPosition position in tone sequence array where to validate
     */
    protected int doValidate(int aPosition)
    throws IllegalArgumentException
    {
        return doStaticValidate(aPosition, iSequence);
    }

    /**
     * Implementation for doValidate, for static context.
     * @param aPosition position where to validate
     * @param aToneSequence sequence which to validate
     */
    private static int doStaticValidate(int aPosition, byte[] aToneSequence)
    {
        byte type = aToneSequence[ aPosition ];
        byte data = aToneSequence[ aPosition + 1 ];
        int retVal = 0;

        if (type >= ToneControl.SILENCE &&
                type <= MidiToneConstants.TONE_MAX_NOTE)
        {
            if (data < MidiToneConstants.TONE_SEQUENCE_NOTE_MIN_DURATION ||
                    data > MidiToneConstants.TONE_SEQUENCE_NOTE_MAX_DURATION)
            {
                throw new IllegalArgumentException(
                    "Note duration out of range, valid range is 1 <= duration <=127");
            }
            retVal = EVENT_SIZE;
        }
        return retVal;
    }

    /**
     * Writes tone event into MIDI sequence.
     * @param aType tone event type
     * @param aData tone event parameter
     * @param aSilent whether this event is silent (note value -1) or not.
     * @param aMidiSequence midi sequence to write midi events to
     */
    private static int processToneEvent(
        byte aType,
        byte aData,
        boolean aSilent,
        MidiSequence aMidiSequence) throws MidiSequenceException
    {
        // If this is a silent note, two NOTE_OFFs are written into midi sequence.
        // otherwise NOTE_ON and NOTE_OFF.

        byte firstMidiEventType = MidiToneConstants.MIDI_NOTE_ON;
        if (aSilent)
        {
            firstMidiEventType = MidiToneConstants.MIDI_NOTE_OFF;
        }

        // write 'note on' on delta time 0
        aMidiSequence.writeMidiEvent(0,
                                     firstMidiEventType,
                                     aType,
                                     MidiToneConstants.MIDI_MAX_VELOCITY);
        // write 'note off' after delta time represented by  'data' variable
        aMidiSequence.writeMidiEvent(aData,
                                     MidiToneConstants.MIDI_NOTE_OFF,
                                     aType,
                                     MidiToneConstants.MIDI_MAX_VELOCITY);

        // N.B.! Above MIDI_NOTE_ON and MIDI_NOTE_OFF can be written without channel
        // value because MidiSequence attached correct channel value to them anyway.
        return EVENT_SIZE;
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
