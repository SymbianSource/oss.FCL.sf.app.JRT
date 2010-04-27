/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Event list implementation.
 *
*/


// PACKAGE
package com.nokia.mj.impl.pim;

// IMPORTS
import javax.microedition.pim.Event;
import javax.microedition.pim.EventList;
import javax.microedition.pim.PIM;
import javax.microedition.pim.PIMException;
import javax.microedition.pim.PIMItem;
import javax.microedition.pim.RepeatRule;
import java.util.Enumeration;
import com.nokia.mj.impl.pim.ErrorString;
import com.nokia.mj.impl.pim.GenericException;
import com.nokia.mj.impl.pim.utils.NativeError;

// CLASS DEFINITION
/**
 * Event list implementation.
 */
public final class EventListImpl extends PIMListImpl implements EventList
{
    // Constructors

    EventListImpl(int aListHandle, int aMode)
    {
        super(aListHandle, aMode);
    }

    // Methods from PIMListImpl

    int getListType()
    {
        return PIM.EVENT_LIST;
    }

    /**
     * @par Implementation notes: Creating an Event is a bit complicated. As
     *      with the other PIM types, the native side Event already exists and
     *      handle to it is given as an argument. In addition to that, the
     *      native side Event owns its associated native side RepeatRule, to
     *      which there is no handle yet. Handle to that existing native side
     *      RepeatRule is made and the ownership of that RepeatRule is
     *      transferred to a Java side RepeatRule, which is created here. A Java
     *      side Event is created and associated with its native peer object and
     *      the new Java side RepeatRule is given to the ownership of the Event.
     *      So, as a result the new Event owns its native side peer and a Java
     *      side RepeatRule which owns its native side peer which the native
     *      side Event uses but does not own. Got it? ;-)
     */
    protected PIMItemImpl createAssociatedItem(int aItemHandle)
    {
        int[] err = new int[1];
        int repeatRuleHandle = _createRepeatRuleHandle(

                                   aItemHandle, err);
        if (!NativeError.checkSuccess(err[0]))
        {
            throw new GenericException(ErrorString.GENERAL_ERROR_COLON + err[0]);
        }

        RepeatRule repeatRule = RepeatRuleInvoker.createRepeatRule(
                                    repeatRuleHandle);

        return new EventImpl(

                   aItemHandle, repeatRule, this);
    }

    // Methods from EventList
    public synchronized Event createEvent()
    {
        return (EventImpl) createItem();
    }

    public synchronized Event importEvent(Event aEvent)
    {
        if (aEvent == null)
        {
            throw new NullPointerException(ErrorString.IMPORTING_FAILED_COLON +
                                           ErrorString.IMPORT_ITEM_IS_NULL);
        }
        EventImpl eventItem = (EventImpl) createItem();
        eventItem.copyFrom(aEvent);

        // Set REVISION field
        updateRevisionField(eventItem, Event.REVISION, PIMItem.ATTR_NONE);

        return eventItem;
    }

    public synchronized void removeEvent(Event aEvent) throws PIMException
    {
        checkModeMinimum(PIM.WRITE_ONLY);
        removeItem(aEvent);
    }

    public synchronized Enumeration items(int aSearchType, long aStartDate,
                                          long aEndDate, boolean aInitialEventOnly) throws PIMException
    {
        ensurePimPermission(PIM.READ_ONLY);

        if (aSearchType != EventList.STARTING
                && aSearchType != EventList.ENDING
                && aSearchType != EventList.OCCURRING)
        {
            throw new IllegalArgumentException(ErrorString.SEARCHING_FAILED_COLON +
                                               ErrorString.INVALID_SEARCH_TYPE_COLON + aSearchType);
        }

        if (aStartDate > aEndDate)
        {
            throw new IllegalArgumentException(ErrorString.SEARCHING_FAILED_COLON +
                                               ErrorString.START_DATE_IS_AFTER_END_DATE);
        }

        updateList(0);
        int[] error = new int[1];

        int[] itemHandles = _itemsByDate(
                                iListHandle, aSearchType, aStartDate, aEndDate, aInitialEventOnly,
                                error);

        return processItemsResults(itemHandles, error[0]);
    }

    public synchronized int[] getSupportedRepeatRuleFields(int aFrequency)
    {
        if (aFrequency != RepeatRule.DAILY && aFrequency != RepeatRule.WEEKLY
                && aFrequency != RepeatRule.MONTHLY
                && aFrequency != RepeatRule.YEARLY)
        {
            throw new IllegalArgumentException(ErrorString.INVALID_FREQUENCY_COLON
                                               + aFrequency);
        }

        int[] err = new int[1];

        int[] supportedRepeatRuleFields = _getSupportedRepeatRuleFields(
                                              iListHandle, aFrequency, err);

        NativeError.handleRepeatRuleFieldsError(err[0], aFrequency);

        if (supportedRepeatRuleFields == null)
        {
            throw new OutOfMemoryError();
        }

        return supportedRepeatRuleFields;
    }

    // Native operations

    private native int[] _itemsByDate(int aEventListHandle, int aSearchType,
                                      long aStartDate, long aEndDate, boolean aInitialEventOnly,
                                      int[] aError);

    /**
     * @return Supported Repeat Rule fields for given frequency or NULL in
     *         error.
     */
    private native int[] _getSupportedRepeatRuleFields(int aEventListHandle,
            int aFrequency, int[] aError);

    /**
     * Creates the handle for a native repeat rule, trasfers the ownership of
     * the native repeat rule to its Java side peer. This list is not involved
     * in the process; the operation is just needed here.
     */
    private native int _createRepeatRuleHandle(int aEventItemHandle,
            int[] aError);

}