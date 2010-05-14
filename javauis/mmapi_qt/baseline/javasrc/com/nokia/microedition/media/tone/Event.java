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
* Description:  Base class for tone event processors
*
*/


package com.nokia.microedition.media.tone;

import javax.microedition.media.control.ToneControl;
import java.util.Stack;
import java.util.Hashtable;
import com.nokia.mj.impl.utils.Logger;


/**
 * Base class for tone event processors
 */
public abstract class Event
{
    // CONSTANTS

    /* Size of single tone event in bytes */
    public static final byte EVENT_SIZE = 2;

    // MEMBER DATA

    /* Hold original tone sequence bytes */
    protected byte[] iSequence;

    /* Holds the new tone sequence converted to MIDI */
    protected MidiSequence iMidiSequence;

    /* Holds block position index in iSequence associated with block
       number as key. Both are Integer. */
    protected static Hashtable iBlocksHashtable;

    /* Stack for tone event processors. Used by the method advance. */
    protected static Stack iReturnPositionStack;

    /* Current block number holder for validating blocks. If not in any
       block, the value is -1. Manipulated by accessor methods. */
    private static Stack iCurrentBlockNumStack;

    /* Defines which event types (byte code) are accepted after this event.
       List is very probably overridden by child events. Defaults to none. */
//  protected int[] ACCEPTED_EVENTS = {};

    // Static initialization
    static
    {
        iBlocksHashtable = new Hashtable();
        iReturnPositionStack = new Stack();
        iCurrentBlockNumStack = new Stack();
    }

    protected Event(byte[] aSequence, MidiSequence aMidiSequence)
    {
        iSequence = aSequence;
        iMidiSequence = aMidiSequence;
    }

    /**
     * Reset events for reuse
     */
    public void reset()
    {
        if (!iReturnPositionStack.empty())
        {
            iReturnPositionStack = new Stack();
        }
        if (!iBlocksHashtable.isEmpty())
        {
            iBlocksHashtable = new Hashtable();
        }
        if (!iCurrentBlockNumStack.isEmpty())
        {
            iCurrentBlockNumStack = new Stack();
        }
    }

    /**
     * Process events step by step. Does not validate and does not go
     * through whole sequence, but only those positions that are
     * needed for playing.
     * @param aPosition position where to process
     * @return int offset of position after processing, relative to aPosition.
     * If zero, no processing has happened.
     */
    public abstract int advance(int aPosition)
    throws MidiSequenceException, IllegalArgumentException;

    /**
     * Child class defined functionality for validate
     * @param aPosition position in tone sequence array where to validate
     * @return new position offset related to aPosition in tone sequence
     * array. Must be positive.
     */
    protected abstract int doValidate(int aPosition)
    throws IllegalArgumentException;

    /**
     * Validate sequence step by step. Does not process events, but can do
     * initialization along validation. Validation includes: Check event
     * type, parameters and that next event type after this one is allowed
     * to be there.
     * @param aPosition position where to validate
     * @return new position offset related to aPosition in tone sequence
     * array. Must be positive.
     */
    public int validate(int aPosition)
    throws IllegalArgumentException
    {
        int nextPos = 0;
        try
        {
            nextPos = doValidate(aPosition);
        }
        catch (ArrayIndexOutOfBoundsException aioobe)
        {
            Logger.WLOG(Logger.EJavaMMAPI, "MMA: Event: validate: AIOOBE");
            // just return
            return nextPos;
        }

        if (nextPos < 0)
        {
            // doValidate must never return negative value
            throw new Error();
        }

        if (nextPos != 0)
        {
            // check that event type in next position is allowed
            checkEventAtNextPosition(aPosition + nextPos);
        }
        return nextPos;
    }

    /**
     * Check whether event or end of sequence at next position
     * is legal after this type of event. Throws
     * IllegalArgumentException if not accepted.
     * @param aPosition position of next event
     */

    protected abstract void checkEventAtNextPosition(int aPosition)
    throws IllegalArgumentException;

    /**
     * Called when entering a block.
     * @param aBlockNum number of block to enter
     */
    protected void enterBlock(int aBlockNum)
    {
        iCurrentBlockNumStack.push(new Integer(aBlockNum));
    }

    /**
     * Called when leaving a block.
     * @param aBlockNum number of block to leave
     * @exception IllegalArgumentException if aBlockNum does not
     * correspond to last block entered or if no block has been
     * entered.
     */
    protected void leaveBlock(int aBlockNum) throws IllegalArgumentException
    {
        if (iCurrentBlockNumStack.isEmpty())
        {
            Logger.ELOG(Logger.EJavaMMAPI,
                        "MMA: Event: leaveBlock: Not inside block, IAE");
            throw new IllegalArgumentException(
                "Illegal Sequence, invalid block number found");
        }

        if (aBlockNum !=
                (((Integer)iCurrentBlockNumStack.pop()).intValue()))
        {
            Logger.ELOG(Logger.EJavaMMAPI,
                        "MMA: Event: leaveBlock: Incorrect block number, IAE");
            throw new IllegalArgumentException(
                "Illegal Sequence, invalid block number found");
        }
    }

} // end of class

