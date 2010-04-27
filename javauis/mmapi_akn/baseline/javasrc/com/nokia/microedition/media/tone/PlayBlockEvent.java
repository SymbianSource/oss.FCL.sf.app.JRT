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
* Description:  Event processor class for tone sequence PLAY_BLOCK events
*
*/


package com.nokia.microedition.media.tone;

import javax.microedition.media.control.ToneControl;

/**
 * Processor for play block events
 */
public class PlayBlockEvent extends Event
{

    // CONSTANTS

    /* Maximum and minimum values for block number */
    public static final int PLAY_BLOCK_MAX_VALUE = 127;
    public static final int PLAY_BLOCK_MIN_VALUE = 0;

    PlayBlockEvent(byte[] aSequence, MidiSequence aMidiSequence)
    {
        super(aSequence, aMidiSequence);
    }

    /**
     * Inherited from Event
     */
    public int advance(int aPosition)
    {
        int retVal = doValidate(aPosition);
        if (retVal != 0)
        {
            // Push the position to stack that is going to be
            // played after this play block has completed
            iReturnPositionStack.push(new Integer(aPosition + EVENT_SIZE));

            int data = iSequence[ aPosition + 1 ];
            retVal = findBlock(data) - aPosition;
        }
        return retVal;
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
        // it is already checked that there is at least two bytes left
        int type = iSequence[ aPosition ];
        int data = iSequence[ aPosition + 1 ];
        int retVal = 0;

        if (type == ToneControl.PLAY_BLOCK)
        {
            if (data < PLAY_BLOCK_MIN_VALUE ||
                    data > PLAY_BLOCK_MAX_VALUE)
            {
                throw new IllegalArgumentException(
                    "Block number out of range");
            }
            findBlock(data);   // for check only
            retVal = EVENT_SIZE;
        }
        return retVal;
    }

    /**
     * Find block
     * @param block number
     * @return position of corresponding BLOCK_START event
     * @exception IllegalArgumentException if block is not found
     */
    private int findBlock(int aBlockNumber)
    {
        for (int i = 0; i < iSequence.length; i += EVENT_SIZE)
        {
            if (iSequence[ i ] == ToneControl.BLOCK_START &&
                    iSequence[ i + 1 ] == aBlockNumber)
            {
                return i;
            }
        }
        // if block is not found, input is illegal
        throw new IllegalArgumentException("No block found, sequence is corrupted");
    }

    /**
     * Child class defined functionality for checkEventAtNextPosition
     * @param aPosition position in tone sequence array where to check
     */
    protected void checkEventAtNextPosition(int aPosition)
    throws IllegalArgumentException
    {
        // After this event there can be:
        // Tone, BlockEnd PlayBlock, Volume, Repeat or end of sequence

        int type = 0;
        try
        {
            type = iSequence[ aPosition ];
        }
        catch (ArrayIndexOutOfBoundsException aioobe)
        {
            return; // end of sequence is OK
        }


        if (type >= ToneControl.SILENCE ||
                type == ToneControl.BLOCK_END ||
                type == ToneControl.PLAY_BLOCK ||
                type == ToneControl.SET_VOLUME ||
                type == ToneControl.REPEAT)
        {
            return;
        }
        throw new IllegalArgumentException("Illegal event found, sequence is corrupted");
    }
} // end of class

