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
* Description:  Event item implementation.
 *
*/


// PACKAGE
package com.nokia.mj.impl.pim;

// IMPORTS
import javax.microedition.pim.Event;
import javax.microedition.pim.RepeatRule;
import javax.microedition.pim.PIM;
import java.util.Date;
import java.util.Enumeration;
import com.nokia.mj.impl.pim.GenericException;
import com.nokia.mj.impl.pim.utils.NativeError;

// CLASS DEFINITION
/**
 * Event item implementation.
 */
public final class EventImpl extends PIMItemImpl implements Event
{
    // Member data

    /**
     * Repeating information, owned by the Event. The RepeatRule is never given
     * outside the object; copies are provided instead.
     */
    private RepeatRule iRepeatRule;

    // Constructors

    /**
     * Creates new EventImpl and associates it with given native peer object.
     *
     * @param aRepeatRule
     *            Must be the Java peer object for a native Repeat Rule which is
     *            already set for the native side Event object.
     */
    EventImpl(int aItemHandle, RepeatRule aRepeatRule)
    {
        super(aItemHandle);
        iRepeatRule = aRepeatRule;
        iItemType = PIM.EVENT_LIST;
    }

    /**
     * Creates new EventImpl and associates it with given native peer object and
     * given EventList.
     *
     * @param aRepeatRule
     *            Must be the Java peer object for a native Repeat Rule which is
     *            already set for the native side Event object.
     */
    EventImpl(int aItemHandle, RepeatRule aRepeatRule, EventListImpl aEventList)
    {
        super(aItemHandle, aEventList);
        iItemType = PIM.EVENT_LIST;
        iRepeatRule = aRepeatRule;
    }

    /**
     * Creates new EventImpl and its native side peer object. Associated Java
     * side RepeatRule will be created as well. The new EventImpl object is not
     * associated with a list.
     */
    EventImpl()
    {
        super();

        iItemType = PIM.EVENT_LIST;

        int[] repeatRuleHandle = new int[1];
        int[] err = new int[1];
        iItemHandle = _createNativePeer(((PIMManager) PIMManager.getInstance()).managerHandle(),
                                        repeatRuleHandle, err);
        if (!NativeError.checkSuccess(err[0]))
        {
            throw new GenericException(ErrorString.GENERAL_ERROR_COLON + err[0]);
        }

        iRepeatRule = RepeatRuleInvoker.createRepeatRule(

                          repeatRuleHandle[0]);
    }

    //  Methods from PIMItemImpl

    /**
     * Provides the essential info about this Event so that the user
     * can distinguish it among other Events.
     *
     * One of these values is returned (in preferred order):
     * @li Summary (Subject or Occasion)
     * @li Location
     * @li \c null
     */
    String getShortInfo()
    {
        String info = null;

        // If the item does not belong to a list, all fields are supported;
        // otherwise we must consult the list for permission to avoid
        // unexpected UnsupportedFieldException.
        if (isFieldSupportedAndPresent(Event.SUMMARY))
        {
            info = getString(Event.SUMMARY, 0);
        }
        else if (isFieldSupportedAndPresent(Event.LOCATION))
        {
            info = getString(Event.LOCATION, 0);
        }
        // else leave info null

        return info;
    }



    // Methods from Event
    public synchronized RepeatRule getRepeat()
    {
        boolean isRepeating = _isRepeating(iItemHandle);
        RepeatRule duplicate = null;

        if (isRepeating)
        {
            duplicate = new RepeatRule();
            copyRepeatRule(duplicate, iRepeatRule);
        }
        // else leave duplicate null

        return duplicate;
    }

    public synchronized void setRepeat(RepeatRule aValue)
    {
        if (aValue == null)
        {
            _setRepeating(iItemHandle, false);
        }
        else
        {
            // Clear the existing repeat
            int repeatRuleHandle = RepeatRuleInvoker.getHandle(iRepeatRule);
            _clearRepeatRule(repeatRuleHandle);

            copyRepeatRule(iRepeatRule, aValue);
            _setRepeating(iItemHandle, true);
        }
    }

    // New methods

    /**
     * Copy repeat rule fields and exception dates. The new values are put on
     * top of the existing values.
     */
    protected void copyRepeatRule(RepeatRule aToRule, RepeatRule aFromRule)
    {
        // Copy fields
        int[] fields = aFromRule.getFields();
        for (int i = 0; i < fields.length; i++)
        {
            int field = fields[i];

            // RepeatRule.END is the only date (long) field
            if (field == RepeatRule.END)
            {
                long value = aFromRule.getDate(field);
                aToRule.setDate(field, value);
            }
            else
                // it must be int field
            {
                int value = aFromRule.getInt(field);
                aToRule.setInt(field, value);
            }

        }

        // Copy exception dates
        Enumeration exceptionDates = aFromRule.getExceptDates();
        while (exceptionDates.hasMoreElements())
        {
            Date exceptionDate = (Date) exceptionDates.nextElement();
            aToRule.addExceptDate(exceptionDate.getTime());
        }
    }

    /**
     * Copies the contents of given Event-implementing object. New data is
     * appended to the existing data.
     */
    void copyFrom(Event aEvent)
    {
        baseCopyFrom(aEvent);
        setRepeat(aEvent.getRepeat());
    }

    // Native operations

    /**
     * @param aRepeatRuleHandle
     *            Handle to the native side repeat rule associated with the
     *            event is returned through this parameter.
     *
     * @return Handle to the native side peer object or < 0 in error.
     */
    private native int _createNativePeer(int aManagerHandle,
                                         int[] aRepeatRuleHandle, int[] aError);

    private native boolean _isRepeating(int aEventHandle);

    private native void _setRepeating(int aEventHandle, boolean aSetRepeating);

    private native void _clearRepeatRule(int aRepeatRuleHandle);

}
