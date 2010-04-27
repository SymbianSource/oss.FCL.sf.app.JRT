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
* Description:  Event processor class for tone sequence BLOCK_END events
*
*/


package com.nokia.microedition.media.tone;

import javax.microedition.media.control.ToneControl;

/**
 * Event processor class for tone sequence BLOCK_END events
 */
public class BlockEndEvent extends Event
{
    BlockEndEvent(byte[] aSequence, MidiSequence aMidiSequence)
    {
        super(aSequence, aMidiSequence);
    }

    public int advance(int aPosition)
    throws MidiSequenceException, IllegalArgumentException
    {
        int retVal = doValidate(aPosition);
        if (retVal == 0)
        {
            return 0;
        }
        // if stack is already empty, we cannot revert back. Thus
        // this BLOCK_END must be invalid
        if (iReturnPositionStack.empty())
        {
            throw new IllegalArgumentException("Illegal BLOCK_END");
        }
        // If valid, go back to last position before entering block
        int lastPos = ((Integer)iReturnPositionStack.pop()).intValue();
        retVal = lastPos - aPosition;
        return retVal;
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

        if (type == ToneControl.BLOCK_END)
        {
            leaveBlock(data);   // fails if block number is incorrect
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
        // Tone, BlockStart, PlayBlock, Volume, Repeat

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
                type == ToneControl.BLOCK_START ||
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

