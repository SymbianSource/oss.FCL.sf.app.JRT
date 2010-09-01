/*******************************************************************************
 * Copyright (c) 2004, 2007 Nokia Corporation.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Yu You (Nokia Corp.) - initial API specification
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/
package org.eclipse.ercp.swt.mobile;

import java.util.*;

import org.eclipse.swt.SWT;
import org.eclipse.swt.SWTException;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.ercp.swt.mobile.internal.OS;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.TypedListener;

/**
 * A special data entry control that allows users to enter or choose a date. The
 * return value of <code>getDate()</code> is an instance of a Date class.
 *
 * <p>
 * The default locale and timezone for the date and time formatting reflects the
 * current configuration in the device. The default date used if a date is not
 * set is the current date. Applications may call
 * <code>setTimeZone(timeZone)</code> to change the differential added to UTC
 * time. This change only affects the widget instance and
 * does not affect other applications.
 * </p>
 * <p>
 * Example code
 * </p>
 * <pre>
 * DateEditor date1 = new DateEditor(composite, SWT.BORDER, DateEditor.DATE_TIME);
 * date1.setDate((new Date()));
 * </pre>
 * <dl>
 * <dt><b>DateEditor styles: </b></dt>
 * <dd>DATE: year, month and day</dd>
 * <dd>TIME: hours, minutes and seconds</dd>
 * <dd>DATE_TIME: date and time</dd>
 * <dd>DURATION: a period of time in hours, minutes and seconds. If the platform
 * does not support a unique appearance, the control will have the same
 * appearance used for TIME style.</dd>
 * <dd>OFFSET: a period of time in hours, minutes, and seconds which can be
 * subtracted or added to another time value. Usually a control with this
 * style has a different appearance with positive or negative signs
 * being used. If not, the control will have the same appearance used for TIME style.</dd>
 * <dd>COMPACT: a hint to display a smaller or less full featured widget which
 * perhaps de-emphasizes ease of use in order to save space.
 * <dd>FULL: a hint to display a full size or full capability widget which perhaps
 * emphasizes ease of use.</dd>
 * </dl>
 * Note: The ability to enter seconds is platform-specific.
 * <dl>
 * <dt><b>Events: </b></dt>
 * <dd>Selection, DefaultSelection, Modify</dd>
 * </dl>
 * <p>
 * IMPORTANT: This class is <em>not</em> intended to be subclassed.
 * </p>
 */
public class DateEditor extends Control
{
    private int dateEditorHandle;
    private int dateEditorStyle;
    // JavaDoc specifies a max offset value equal to 359999s (99:59:59 in hh:mm:ss)
    // but Symbian OS's offset/duration is limited 86399s (23:59:59)
    private static final int maxOffset = 86399;

    private Calendar iCalendar;
    private Calendar iDefaultCal;
    private Date iDateWrapper;

    /**
     * A date entry type for year, month and day.
     *
     * <P>
     * Value <code>1</code> is assigned to <code>DATE</code>.
     * </P>
     *
     */
    public static final int DATE = 1;

    /**
     * A time entry type for hours, minutes and seconds.
     *
     * <P>
     * Value <code>2</code> is assigned to <code>TIME</code>.
     * </P>
     *
     */
    public static final int TIME = 2;

    /**
     * An entry type for date and time.
     * <P>
     * Value <code>3</code> is assigned to <code>DATE_TIME</code>.
     * </P>
     * </DL>
     *
     */
    public static final int DATE_TIME = 3;

    /**
     * An entry type for a period of time in hours, minutes and seconds.
     * <P>
     * Value <code>4</code> is assigned to <code>DURATION</code>.
     * </P>
     * </DL>
     *
     */
    public static final int DURATION = 4;

    /**
     * An entry type for a period of time in hours, minutes, and seconds which
     * can be subtracted or added to another time value. Usually a control with
     * this style has a different appearance. Positive or negative signs are
     * used in this style.
     * <P>
     * Value <code>5</code> is assigned to <code>OFFSET</code>.
     * </P>
     * </DL>
     *
     */
    public static final int OFFSET = 5;

    /**
     * A hint that the widget should be displayed in a format which is smaller
     * or less full featured. It may de-emphasizes ease of use in order to use
     * the least amount of screen space.
     * <P>
     * Value <code>0x10</code> is assigned to <code>COMPACT</code>.
     * </P>
     * </DL>
     *
     */
    public static final int COMPACT = 0x10;

    /**
     * A hint that the widget should be displayed in a format which is full
     * featured and emphasizes ease of use rather than compactness.
     * <P>
     * Value <code>0x20</code> is assigned to <code>FULL</code>.
     * </P>
     * </DL>
     *
     */
    public static final int FULL = 0x20;


    /**
     * Constructs a new instance of this class given its parent and a style
     * value describing its behavior and appearance.
     * <p>
     * The style value is either one of the style constants defined in class
     * <code>SWT</code> which is applicable to instances of this class, or
     * must be built by <em>bitwise OR</em> 'ing together (that is, using the
     * <code>int</code> "|" operator) two or more of those <code>SWT</code>
     * style constants. The class description lists the style constants that are
     * applicable to the class. Style bits are also inherited from superclasses.
     * </p>
     *
     * <p>
     * <em>DATE|COMPACT</em> is used as the default date editor style value.
     * </p>
     *
     * @param parent
     *            a composite control which will be the parent of the new
     *            instance (cannot be null)
     * @param style
     *            the SWT style to construct
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                <li>ERROR_INVALID_SUBCLASS - if this class is not an
     *                allowed subclass</li>
     *                </ul>
     *
     * @see #DATE
     * @see #COMPACT
     */
    public DateEditor(Composite parent, int style)
    {
        this(parent, style, DateEditor.DATE|DateEditor.COMPACT);
    }

    /**
     * Constructs a new instance of this class given its parent, a style value
     * describing its appearance, and a widget-specific style describing its
     * behavior.
     * <p>
     * The style value is either one of the style constants defined in class
     * <code>SWT</code> which is applicable to instances of this class, or
     * must be built by <em>bitwise OR</em> 'ing together (that is, using the
     * <code>int</code> "|" operator) two or more of those <code>SWT</code>
     * style constants. The class description lists the style constants that are
     * applicable to the class. Style bits are also inherited from superclasses.
     * </p>
     * <p>
     * The DateEditor type constants:
     * DATE, TIME, DATE_TIME, DURATION, OFFSET may be <em>bitwise OR</em>'ed
     * with one of the widget-specific styles: COMPACT or FULL.
     * </p>
     *
     * @param parent
     *            a composite control which will be the parent of the new
     *            instance (cannot be null)
     * @param style
     *            the SWT style to construct
     * @param dateEditorStyle
     *            the DateEditor style to construct.
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
     *                <li>ERROR_INVALID_ARGUMENT - if the dateEditorStyle is
     *                invalid</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                <li>ERROR_INVALID_SUBCLASS - if this class is not an
     *                allowed subclass</li>
     *                </ul>
     *
     * @see #DATE
     * @see #TIME
     * @see #DATE_TIME
     * @see #DURATION
     * @see #OFFSET
     * @see #COMPACT
     * @see #FULL
     */
    public DateEditor(Composite parent, int style, int dateEditorStyle)
    {
        super(parent, style);

        try
        {
            this.dateEditorStyle = dateEditorStyle;
            internal_checkDateEditorStyle();
            OS.DateEditor_SetDateEditorStyle(dateEditorHandle, dateEditorStyle);
            if ((dateEditorStyle & 0xf) == DATE
                    || (dateEditorStyle & 0xf) == TIME
                    || (dateEditorStyle & 0xf) == DATE_TIME)
            {
                iCalendar = Calendar.getInstance(TimeZone.getDefault());
                iDefaultCal = Calendar.getInstance(TimeZone.getTimeZone("GMT"));
                iDateWrapper = new Date();
            }
        }
        catch (Error e)
        {
            if (handle != 0) org.eclipse.swt.internal.symbian.OS.Control_Dispose(handle);
            throw e;
        }
    }

    final void internal_setDateEditorHandle(int dateEditorHandle)
    {
        this.dateEditorHandle = dateEditorHandle;
        handle = OS.DateEditor_Handle(dateEditorHandle);
    }

    protected void internal_createHandle()
    {
        internal_setDateEditorHandle(OS.DateEditor_New(this, getParent().internal_getCompositeHandle(), internal_getStyle()));
    }

    private void internal_checkDateEditorStyle()
    {

        switch (dateEditorStyle & 0xf)
        {
        case DATE:
        case DATE_TIME:
        case DURATION:
        case OFFSET:
        case TIME:
            break;
        default:
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }

        switch (dateEditorStyle & 0xf0)
        {
        case COMPACT:
        case FULL:
        case 0:
            return;
        default:
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }
    }

    /**
     * Adds the listener to the collection of listeners that will be notified
     * when the receiver's content is modified. This listener will receive the
     * messages defined in the <code>ModifyListener</code> interface.
     *
     * @param listener
     *            the listener which should be notified
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     *
     * @see ModifyListener
     * @see #removeModifyListener
     */
    public void addModifyListener(ModifyListener listener)
    {
        checkWidget();
        if (listener == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener(listener);
        addListener(SWT.Modify, typedListener);
    }

    /**
     * Adds the listener to the collection of listeners that will be notified
     * when the control is selected. This listener will receive the messages
     * defined in the <code>SelectionListener</code> interface.
     * <p>
     * <code>widgetSelected</code> is called when a date or time selection is
     * finalized by single clicking or pressing the selection key.
     * <code>widgetDefaultSelected</code> is called when the user double clicks
     * or presses the ENTER key.
     * </p>
     *
     * @param listener
     *            the listener which should be notified
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     *
     * @see SelectionListener
     * @see #removeSelectionListener
     * @see SelectionEvent
     */
    public void addSelectionListener(SelectionListener listener)
    {
        checkWidget();
        if (listener == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener(listener);
        addListener(SWT.Selection,typedListener);
        addListener(SWT.DefaultSelection,typedListener);
    }

    /**
     * Returns the date when DATE or DATE_TIME styles are used.
     *
     * Updating the returned <code>Date</code> instance will not affect the
     * date value in the control.
     *
     * @return Date an instance of date/time in UTC.
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                </ul>
     */
    public Date getDate()
    {
        checkWidget();

        if ((dateEditorStyle & 0xf) != DATE
                && (dateEditorStyle & 0xf) != TIME
                && (dateEditorStyle & 0xf) != DATE_TIME)
        {
            return null;
        }

        // verification of the validity of the date,
        // in relation to January 1, 1970 00:00:00
        // should have been made natively
        int dateInTwoParts[] = OS.DateEditor_GetDate(dateEditorHandle);
        if (dateInTwoParts == null)
            return null;

        long maskLow = 0xffffffff;
        maskLow = maskLow >>> 32;
        long longPartLow = maskLow & (long)dateInTwoParts[1];
        long longPartHigh = (long)dateInTwoParts[0];
        longPartHigh = longPartHigh << 32;

        final long currentDate = (longPartHigh + longPartLow);
        iDateWrapper.setTime(currentDate);
        iDefaultCal.setTime(iDateWrapper);

        int year = iDefaultCal.get(Calendar.YEAR);
        int month = iDefaultCal.get(Calendar.MONTH);
        int day = iDefaultCal.get(Calendar.DAY_OF_MONTH);
        int hour = iDefaultCal.get(Calendar.HOUR_OF_DAY);
        int minute = iDefaultCal.get(Calendar.MINUTE);
        int second = iDefaultCal.get(Calendar.SECOND);

//      convert to local time
        iCalendar.set(Calendar.YEAR,year);
        iCalendar.set(Calendar.MONTH,month);
        iCalendar.set(Calendar.DAY_OF_MONTH,day);
        iCalendar.set(Calendar.HOUR_OF_DAY,hour);
        iCalendar.set(Calendar.MINUTE,minute);
        iCalendar.set(Calendar.SECOND,second);

        return iCalendar.getTime();
    }

    /**
     * Returns the number of seconds when TIME, OFFSET or DURATION styles are used.
     * The return value is the total number of seconds derived from the hours, minutes, and seconds displayed in the widget.
     * The maximum value is +/- 86399 seconds which is equivalent to 23:59:59 in hh:mm:ss.
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                </ul>
     */
    public int getTime()
    {
        checkWidget();

        if ((dateEditorStyle & 0xf) != TIME
                && (dateEditorStyle & 0xf) != OFFSET
                && (dateEditorStyle & 0xf) != DURATION)
        {
            return 0;
        }

        return OS.DateEditor_GetOffset(dateEditorHandle);
    }

    /**
     * Removes the listener from the collection of listeners that will be
     * notified when the receiver's text is modified.
     *
     * @param listener
     *            the listener which should no longer be notified
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     *
     * @see ModifyListener
     * @see #addModifyListener
     */
    public void removeModifyListener(ModifyListener listener)
    {
        removeListener(SWT.Modify, listener);
    }

    /**
     * Removes the listener from the collection of listeners that will be
     * notified when the control is selected.
     *
     * @param listener
     *            the listener which should be notified
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     *
     * @see SelectionListener
     * @see #addSelectionListener
     */
    public void removeSelectionListener(SelectionListener listener)
    {
        removeListener(SWT.Selection,        listener);
        removeListener(SWT.DefaultSelection, listener);
    }

    /**
     * Sets the date for the editor when DATE, TIME or DATE_TIME styles are used.
     * The control does not retain the Date object or modify its value.
     *
     * @param date
     *            an instance of date/time in UTC
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the date is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void setDate(Date date)
    {
        checkWidget();
        if (date==null) SWT.error(SWT.ERROR_NULL_ARGUMENT);

        if ((dateEditorStyle & 0xf) != DATE
                && (dateEditorStyle & 0xf) != TIME
                && (dateEditorStyle & 0xf) != DATE_TIME)
        {
            return;
        }

        long longDate = date.getTime();

        iCalendar.setTime(date);

        // convert to local date\time
        iDefaultCal.set(Calendar.YEAR, iCalendar.get(Calendar.YEAR));
        iDefaultCal.set(Calendar.MONTH, iCalendar.get(Calendar.MONTH));
        iDefaultCal.set(Calendar.DAY_OF_MONTH, iCalendar.get(Calendar.DAY_OF_MONTH));
        iDefaultCal.set(Calendar.HOUR, iCalendar.get(Calendar.HOUR));
        iDefaultCal.set(Calendar.MINUTE, iCalendar.get(Calendar.MINUTE));
        iDefaultCal.set(Calendar.SECOND, iCalendar.get(Calendar.SECOND));
        iDefaultCal.set(Calendar.AM_PM, iCalendar.get(Calendar.AM_PM));
        longDate = iDefaultCal.getTime().getTime();

        int intDateHigh = (int)(longDate >>> 32);
        int intDateLow  = (int)longDate;
        try
        {
            OS.DateEditor_SetDate(dateEditorHandle, intDateHigh, intDateLow);
            internal_sendEvent(SWT.Modify);
        }
        catch (Throwable e)
        {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }
    }

    /**
     * Sets the time for the editor when TIME, OFFSET or DURATION styles are used.
     *
     * @param seconds
     *          the number of seconds which will be displayed in hours,
     *          minutes, and seconds. The maximum value is +/- 86399 seconds
     *          which is equivalent to 23:59:59 in hh:mm:ss.
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_ARGUMENT - if the number of seconds is
     *                out of range</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                </ul>
     */
    public void setTime(int seconds)
    {
        checkWidget();

        if ((dateEditorStyle & 0xf) != TIME
                && (dateEditorStyle & 0xf) != OFFSET
                && (dateEditorStyle & 0xf) != DURATION)
        {
            return;
        }

        if ((this.dateEditorStyle & 0x0f) == DURATION)
        {
            // To implement a DateEditor with the DURATION STYLE, we rely on the symbian
            // definition of DURATION in class CEikDurationEditor:
            // "... the duration is a length of time (from 0 to 24 hours), ...".
            // So that we can not handle negatives values.
            if (seconds < 0)
            {
                SWT.error(SWT.ERROR_INVALID_ARGUMENT);
            }
        }

        if ((seconds < -maxOffset) || (maxOffset < seconds))
        {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }

        try
        {
            OS.DateEditor_SetOffset(dateEditorHandle, seconds);
            internal_sendEvent(SWT.Modify);
        }
        catch (Throwable e)
        {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }
    }

    /**
     * Defines the time zone, which is applied on the DateEditor (which is UTC
     * time). The date and time presented in the user interface of DateEditor is
     * a local time in this time zone. The argument can be null, which defaults
     * to currently active time zone of the device. TimeZone has no affect for
     * DURATION and OFFSET styles.
     *
     * @param timeZone
     *            an instance of TimeZone or null
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void setTimeZone(TimeZone timeZone)
    {
        checkWidget();

        if ((dateEditorStyle & 0xf) != DATE
                && (dateEditorStyle & 0xf) != TIME
                && (dateEditorStyle & 0xf) != DATE_TIME)
        {
            return;
        }

        if (timeZone==null)
        {
            iCalendar.setTimeZone(TimeZone.getDefault());
        }
        else
        {
            iCalendar.setTimeZone(timeZone);
        }
    }
}