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
* Description:  Event processor class for tone sequence BLOCK_START events
*
*/


package com.nokia.microedition.media.tone;

import javax.microedition.media.control.ToneControl;

/**
 * Event processor class for tone sequence BLOCK_START events
 */
public class BlockStartEvent extends Event
{
    BlockStartEvent(byte[] aSequence, MidiSequence aMidiSequence)
    {
        super(aSequence, aMidiSequence);
    }

    public int advance(int aPosition)
    throws MidiSequenceException, IllegalArgumentException
    {
        return doValidate(aPosition);
    }

    /**
     * Child class defined functionality for validate
     * @return new position offset related to aPosition in tone sequence
     * array. Must be positive.
     */
    protected int doValidate(int aPosition)
    throws IllegalArgumentException
    {
        int type = iSequence[ aPosition ];
        int data = iSequence[ aPosition + 1 ];
        int retVal = 0;

        if (type == ToneControl.BLOCK_START)
        {
            if (data > MidiToneConstants.TONE_MAX_BLOCK ||
                    data < MidiToneConstants.TONE_MIN_BLOCK)
            {
                throw new IllegalArgumentException(
                    "Illegal sequence, block number not in range, valid range is 0 <= tempo <= 127");
            }
            enterBlock(data);
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
        // Tone, PlayBlock, Volume, Repeat

        int type = 0;
        try
        {
            type = iSequence[ aPosition ];
        }
        catch (ArrayIndexOutOfBoundsException aioobe)
        {
            throw new IllegalArgumentException(
                "Validation failed, invalid position found in sequence");
        }

        if (type >= ToneControl.SILENCE ||
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

