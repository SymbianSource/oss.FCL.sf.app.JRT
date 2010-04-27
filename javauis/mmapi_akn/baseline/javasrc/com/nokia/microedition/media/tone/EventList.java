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
* Description:  This class is holder/handler for other event types
*
*/


package com.nokia.microedition.media.tone;

import javax.microedition.media.control.ToneControl;
import java.util.Vector;
import java.util.Enumeration;

/**
 * This class is holder/handler for other event types
 */
public class EventList extends Event
{

    // CONSTANTS

    private static final int EVENT_PROCESSOR_COUNT = 6;

    // MEMBERS

    private Vector iEvents;

    EventList(byte[] aSequence, MidiSequence aMidiSequence)
    {
        super(aSequence, aMidiSequence);

        iEvents = new Vector(EVENT_PROCESSOR_COUNT);

        iEvents.addElement(
            new ToneEvent(aSequence, aMidiSequence));
        iEvents.addElement(
            new BlockStartEvent(aSequence, aMidiSequence));
        iEvents.addElement(
            new BlockEndEvent(aSequence, aMidiSequence));
        iEvents.addElement(
            new PlayBlockEvent(aSequence, aMidiSequence));
        iEvents.addElement(
            new RepeatEvent(aSequence, aMidiSequence));
        iEvents.addElement(
            new VolumeEvent(aSequence, aMidiSequence));
    }

    public int advance(int aPosition) throws MidiSequenceException
    {
        // first check that we have at least two bytes left in iSequence
        // in aPosition.
        if (iSequence.length - aPosition < EVENT_SIZE)
        {
            // return with 0 if end of sequence is reached
            return 0;
        }

        Event event = null;
        int retVal = 0;

        for (Enumeration e = iEvents.elements(); e.hasMoreElements();)
        {
            event = (Event)e.nextElement();
            retVal = event.advance(aPosition);
            if (retVal != 0)
            {
                return retVal;
            }
        }
        // if none of event processors accepts data at current position,
        // parameter data is illegal.
        throw new IllegalArgumentException("Illegal event found, sequence is corrupted");
    }


    /**
     * Inherited from Event.
     * Special definition for validate. EventList is the main
     * class performing the actual validation and is thus
     * excempt from the usual validation process.
     * @param aPosition position to validate
     */
    public int validate(int aPosition)
    throws IllegalArgumentException
    {
        Event event = null;
        int retVal = 0;
        for (Enumeration e = iEvents.elements(); e.hasMoreElements();)
        {
            event = (Event)e.nextElement();
            retVal = event.validate(aPosition);
            if (retVal != 0)
            {
                return retVal;
            }
        }
        // if none of event processors accepts data at current position,
        // parameter data is illegal.
        throw new IllegalArgumentException("Illegal event found, sequence is corrupted");
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
        throw new Error("Illegal validation call");
    }

    /**
     * Child class defined functionality for checkEventAtNextPosition
     * @param aPosition position in tone sequence array where to check
     */
    protected void checkEventAtNextPosition(int aPosition)
    throws IllegalArgumentException
    {
        throw new Error("Illegal validation call");
    }

} // end of class

