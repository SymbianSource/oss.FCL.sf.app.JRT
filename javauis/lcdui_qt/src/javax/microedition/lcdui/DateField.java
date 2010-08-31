/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/
package javax.microedition.lcdui;

import java.util.*;

import org.eclipse.swt.graphics.Point;

/**
 * Implementation of LCDUI <code>DateField</code> class.
 */
public class DateField extends Item
{
    /**
     * Input mode that displays only date.
     */
    public static final int DATE = 1;

    /**
     * Input mode which displays only time.
     */
    public static final int TIME = 2;

    /**
     * Input mode which display both date and time.
     */
    public static final int DATE_TIME = 3;

    /**
     * If DateField is changed, reasons for Re-layouting.
     */
	static final int UPDATE_DATE = UPDATE_ITEM_MAX << 1;
	static final int UPDATE_INPUTMODE = UPDATE_ITEM_MAX << 2;


    private Date date;

    private int mode;

    private TimeZone timeZone;

    /**
     * Constructor.
     *
     * @param label Label of DateField.
     * @param mode DateField's input mode.
     * @throws IllegalArgumentException if input mode is not one of the
     *      constants defined in this class.
     */
    public DateField(String label, int mode)
    {
        this(label, mode, null);
    }

    /**
     * Constructor with time zone.
     *
     * @param label Label of DateField.
     * @param mode DateField's input mode.
     * @param timeZone Timezone of DateField or null if default timezone.
     * @throws IllegalArgumentException if input mode is not one of the
     *             constants defined in this class.
     */
    public DateField(String label, int mode, TimeZone timeZone)
    {
        setLabel(label);
        setInputMode(mode);
        this.timeZone = timeZone;
    }

    /**
     * Gets the date set to this DateField.
     *
     * @return Date, or null if date not set.
     */
    public Date getDate()
    {
        if(date == null)
        {
            return null;
        }
        Calendar calendar = Calendar.getInstance();
        calendar.setTime(date);
        if(mode == TIME)
        {
            calendar.set(Calendar.YEAR,Config.DATEFIELD_ZERO_EPOCH_YEAR);
            calendar.set(Calendar.MONTH,Calendar.JANUARY);
            calendar.set(Calendar.DAY_OF_MONTH,1);
        }
        else if(mode == DATE)
        {
            calendar.set(Calendar.HOUR_OF_DAY,0);
            calendar.set(Calendar.MINUTE,0);
            calendar.set(Calendar.SECOND,0);
            calendar.set(Calendar.MILLISECOND,0);
        }
        return calendar.getTime();
    }

    /**
     * Sets the date to this DateField.
     *
     * @param newDate New date. If null, current date is cleared.
     */
    public void setDate(Date newDate)
    {
        internalSetDate(newDate);
        updateParent((newDate == null ? UPDATE_DATE | UPDATE_SIZE_CHANGED : UPDATE_DATE)); 
    }

    /**
     * Gets input mode of this DateField.
     *
     * @return Input mode.
     */
    public int getInputMode()
    {
        return mode;
    }

    /**
     * Sets input mode of this DateField.
     *
     * @param inputMode New input mode. Must be one of constants
     *      defined in this class.
     * @throws IllegalArgumentException if input mode is not one of the
     *      constants defined in this class.
     */
    public void setInputMode(int inputMode)
    {
        if((inputMode != DATE)
                && (inputMode != TIME)
                && (inputMode != DATE_TIME))
        {
            throw new IllegalArgumentException(
                MsgRepository.DATEFIELD_EXCEPTION_INVALID_MODE);
        }

		if(getInputMode() != inputMode)
		{
	        mode = inputMode;
	        updateParent(UPDATE_INPUTMODE | UPDATE_SIZE_CHANGED);
		}
    }

    /**
     * Calculates minimum size of this item.
     *
     * @return Minimum size.
     */
    Point calculateMinimumSize()
    {
        return DateFieldLayouter.calculateMinimumBounds(this);
    }

    /**
     * Calculates preferred size of this item.
     *
     * @return Preferred size.
     */
    Point calculatePreferredSize()
    {
        return DateFieldLayouter.calculatePreferredBounds(this);
    }

    /**
     * Gets timezone.
     *
     * @return Timezone or null if no timezone set.
     */
    TimeZone getTimeZone()
    {
        return timeZone;
    }

    /**
     * Set the new date internally.
     *
     * @param newDate new date
     */
    void internalSetDate(Date newDate)
    {
        if(newDate == null)
        {
            date = null;
        }
        else
        {
            Calendar calendar = Calendar.getInstance();
            if(timeZone != null)
            {
                calendar.setTimeZone(timeZone);
            }
            calendar.setTime(newDate);

            switch(mode)
            {
            case TIME:
                if((calendar.get(Calendar.YEAR) != Config.DATEFIELD_ZERO_EPOCH_YEAR)
                        || (calendar.get(Calendar.MONTH) != Calendar.JANUARY)
                        || (calendar.get(Calendar.DAY_OF_MONTH) != 1))
                {
                    date = null;
                }
                else
                {
                    calendar.set(Calendar.SECOND,0);
	                calendar.set(Calendar.MILLISECOND,0);
                    date = calendar.getTime();
                }
                break;
            case DATE:
                calendar.set(Calendar.HOUR_OF_DAY, 0);
                calendar.set(Calendar.MINUTE, 0);
                calendar.set(Calendar.SECOND,0);
                calendar.set(Calendar.MILLISECOND,0);
                date = calendar.getTime();
                break;
            case DATE_TIME:
                calendar.set(Calendar.SECOND,0);
                calendar.set(Calendar.MILLISECOND,0);
                date = calendar.getTime();
                break;
            default:
                break;
            }
        }
    }

    /**
     * Return layout with optional custom flags.
     *
     * @return layout directive.
     */
    int internalGetLayout()
    {
        return super.internalGetLayout() | Item.LAYOUT_NEWLINE_BEFORE;
    }

    /* (non-Javadoc)
     * @see javax.microedition.lcdui.Item#isFocusable()
     */
    boolean isFocusable()
    {
        return true;
    }
}
