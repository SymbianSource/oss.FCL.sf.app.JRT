/*******************************************************************************
 * Copyright (c) 2000, 2005 IBM Corporation and others.
 * Portion Copyright (c) 2004, 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
       Nokia Corporation - S60 implementation
 *     Nokia Corporation - QT implementation
 *******************************************************************************/
package org.eclipse.ercp.swt.mobile;

import java.util.Calendar;
import java.util.Date;
import java.util.TimeZone;

import org.eclipse.swt.SWT;
import org.eclipse.swt.SWTException;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Internal_PackageSupport;
import org.eclipse.swt.widgets.TypedListener;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.internal.qt.PackageProxy;
import org.eclipse.swt.internal.qt.WidgetState;

/**
 * A special data entry control that allows users to enter or choose a date. The
 * return value of <code>getDate()</code> is an instance of a Date class.
 *
 * <p>
 * The default locale and timezone for the date and time formatting reflects the
 * current configuration in the device. The default date used if a date is not
 * set is the current date. Applications may call
 * <code>setTimeZone(timeZone)</code> to change the differential added to UTC
 * time. This change only affects the widget instance and does not affect other
 * applications.
 * </p>
 * <p>
 * Example code
 * </p>
 *
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
 * subtracted or added to another time value. Usually a control with this style
 * has a different appearance with positive or negative signs being used. If
 * not, the control will have the same appearance used for TIME style.</dd>
 * <dd>COMPACT: a hint to display a smaller or less full featured widget which
 * perhaps de-emphasizes ease of use in order to save space.
 * <dd>FULL: a hint to display a full size or full capability widget which
 * perhaps emphasizes ease of use.</dd>
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
public class DateEditor extends Control {

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
     * Used to store the time zone set by the user
     */
    TimeZone timeZone;

    /**
     * If set to true, dateTimeChanged signal is ignored.
     */
    boolean ignoreModify;

    /**
     * Handle to the QLineEdit editor inside of the DateEditor
     */
    int editorHandle;

    /*
     * For null dateEditor. By default editor text(date or time) is null.
     */
    boolean textVisible;

    /**
     * Handle to the QCalendarWidget inside of the DateEditor
     */
    int calendarHandle;

    /**
     * Max/min offset is +/- 86399 seconds which is equivalent to (-)23:59:59 in
     * hh:mm:ss.
     */
    private static final int MAXIMUM_OFFSET = 86399;
    private static final int MINIMUM_OFFSET = -86399;

    /**
     * OFFSET and DURATION style DateEditors show hours minutes and seconds
     */
    private static final String DURATION_FORMAT = "hh:mm:ss";

    static final class DateEditorPackageProxy extends PackageProxy {
        public void createHandle(int index) {
            ((DateEditor)w).createHandle(index);
        }
        public void deregister () {
            ((DateEditor)w).deregister();
        }
        public boolean qt_event_keypress( int widgetHandle, int key, int modifier, int character, int nativeScanCode ) {
            return ((DateEditor)w).qt_event_keypress(widgetHandle, key, modifier, character, nativeScanCode);
        }
        public boolean qt_event_mouseButtonDblClick(int widgetHandle, int button, int x, int y, int state, int buttons) {
            return ((DateEditor)w).qt_event_mouseButtonDblClick(widgetHandle, button, x, y, state, buttons);
        }
        public boolean qt_event_mouseButtonPress (int widgetHandle, int button, int x, int y, int state, int buttons) {
            return ((DateEditor)w).qt_event_mouseButtonPress(widgetHandle, button, x, y, state, buttons);
        }
        public void qt_signal_dateedit_dateTimeChanged() {
            ((DateEditor)w).qt_signal_dateedit_dateTimeChanged();
        }
        public void register() {
            ((DateEditor)w).register();
        }
        public void releaseHandle() {
            ((DateEditor)w).releaseHandle();
        }
        public void setTraversalFlags(int type, int key, int modifier, int character) {
            ((DateEditor)w).setTraversalFlags(type, key, modifier, character);
        }
        public void hookEvents() {
            ((DateEditor)w).hookEvents();
        }
    }

    /**
     * Constructs a new instance of this class given its parent and a style
     * value describing its behavior and appearance.
     * <p>
     * The style value is either one of the style constants defined in class
     * <code>SWT</code> which is applicable to instances of this class, or must
     * be built by <em>bitwise OR</em> 'ing together (that is, using the
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
    public DateEditor(Composite parent, int style) {
        this(parent, style, DateEditor.COMPACT|DateEditor.DATE);
    }

    /**
     * Constructs a new instance of this class given its parent, a style value
     * describing its appearance, and a widget-specific style describing its
     * behavior.
     * <p>
     * The style value is either one of the style constants defined in class
     * <code>SWT</code> which is applicable to instances of this class, or must
     * be built by <em>bitwise OR</em> 'ing together (that is, using the
     * <code>int</code> "|" operator) two or more of those <code>SWT</code>
     * style constants. The class description lists the style constants that are
     * applicable to the class. Style bits are also inherited from superclasses.
     * </p>
     * <p>
     * The DateEditor type constants: DATE, TIME, DATE_TIME, DURATION, OFFSET
     * may be <em>bitwise OR</em>'ed with one of the widget-specific styles:
     * COMPACT or FULL.
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
    public DateEditor(Composite parent, int style, int dateEditorStyle) {
        super(parent, Internal_PackageSupport.checkBits(style, 0, 0, 0, 0, 0, 0),
                checkStyle(dateEditorStyle), new DateEditorPackageProxy(), false);
    }

    /*
     * Sets native QDateTimeEdit dateTime to dateTime specified in Calendar
     *
     * @param c dateTime to set
     */
    void setDateTime(Calendar c) {
        int[] dateTime = new int[6];

        dateTime[0] = c.get(Calendar.YEAR);
        dateTime[1] = c.get(Calendar.MONTH) + 1;
        dateTime[2] = c.get(Calendar.DAY_OF_MONTH);
        dateTime[3] = c.get(Calendar.HOUR_OF_DAY);
        dateTime[4] = c.get(Calendar.MINUTE);
        dateTime[5] = c.get(Calendar.SECOND);

        ignoreModify = true;
        OS.QDateTimeEdit_swt_setDateTime(topHandle(), dateTime);
        ignoreModify = false;
    }

    static int checkStyle(int dateEditorStyle) {
        int minor = dateEditorStyle & 0xf;
        int major = dateEditorStyle & 0xf0;

        // Minor part must be unique, cannot apply bitwise operations
        switch (minor) {
            case DATE:
            case TIME:
            case DATE_TIME:
            case DURATION:
            case OFFSET:
                break;
            default:
                SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }

        // Major part is hint, can apply bitwise operations
        major = Internal_PackageSupport.checkBits(major, COMPACT, FULL, 0, 0, 0, 0);

        return major | minor;
    }

    void createHandle(int index) {
        int xStyle = extraStyle() & 0xf;
        int handle = 0;
        switch (xStyle) {
        case OFFSET:
            handle = OS.QOffsetTimeEdit_new();
            Internal_PackageSupport.setHandle(this, handle);
            Internal_PackageSupport.setTopHandle(this, handle);
            OS.QOffsetTimeEdit_setMinimum(handle(), MINIMUM_OFFSET);
            OS.QOffsetTimeEdit_setMaximum(handle(), MAXIMUM_OFFSET);
            _setTime(0);
            break;
        case DURATION:
            handle = OS.QSwtTimeEdit_swt_new(xStyle);
            Internal_PackageSupport.setHandle(this, handle);
            Internal_PackageSupport.setTopHandle(this, handle);
            OS.QDateTimeEdit_setDisplayFormat(handle(), DURATION_FORMAT);
            _setTime(0);
            break;
        case DATE:
        case TIME:
        case DATE_TIME:
            handle = OS.QSwtTimeEdit_swt_new(xStyle);
            Internal_PackageSupport.setHandle(this, handle);
            Internal_PackageSupport.setTopHandle(this, handle);
            _setDate(new Date());
            break;
        }

        Internal_PackageSupport.setState(this, Internal_PackageSupport.state(this) | WidgetState.HANDLE);

        editorHandle = OS.QSwtTimeEdit_editor(handle);

        if ((extraStyle() & 0xf0) == FULL) {
            OS.QDateTimeEdit_setCalendarPopup(handle, true);
            calendarHandle = OS.QDateTimeEdit_calendarWidget(handle);
        } else {
            calendarHandle = 0;
        }

        // Calling sizeHint() causes native QDateTimeEdit to cache proper
        // dimensions for the editor. This way we have correctly sized
        // editor even after hiding the text.
        OS.QWidget_sizeHint(handle());
        OS.QSwtTimeEdit_swt_setTextVisible(handle, false);
        textVisible = false;
    }

    void register() {
        Internal_PackageSupport.register(this);
        Internal_PackageSupport.addWidget(this, editorHandle);
        Internal_PackageSupport.addWidget(this, calendarHandle);
    }

    void deregister() {
        Internal_PackageSupport.removeWidget(this, calendarHandle);
        Internal_PackageSupport.removeWidget(this, editorHandle);
        Internal_PackageSupport.deregister(this);
    }

    void releaseHandle() {
        editorHandle = 0;
        calendarHandle = 0;
        Internal_PackageSupport.releaseHandle(this);
    }

    void hookEvents() {
        int signalProxy = OS.SignalHandler_new(handle(),
                OS.QSIGNAL_DATETIMEEDIT_CHANGED);
        OS.QObject_connectOrThrow(handle(), "dateTimeChanged(const QDateTime&)",
                signalProxy, "widgetSignal(const QDateTime&)",
                OS.QT_AUTOCONNECTION);
    }

    void qt_signal_dateedit_dateTimeChanged() {
        if (!textVisible) {
            textVisible = true;
            OS.QSwtTimeEdit_swt_setTextVisible(topHandle(), true);
        }

        if (!ignoreModify) {
            Internal_PackageSupport.sendEvent(this, SWT.Modify);
        }
    }

    boolean qt_event_mouseButtonPress(int widgetHandle, int button,
            int x, int y, int state, int buttons) {
        boolean val = Internal_PackageSupport.qt_event_mouseButtonPress(this, widgetHandle, button, x,
                y, state, buttons);
        Internal_PackageSupport.sendEvent(this, SWT.Selection);
        return val;
    }

    boolean qt_event_mouseButtonDblClick(int widgetHandle,
            int button, int x, int y, int DATE, int buttons) {
        boolean val = Internal_PackageSupport.qt_event_mouseButtonDblClick(this, widgetHandle, button,
                x, y, DATE, buttons);
        if (button == 1) {
            Internal_PackageSupport.sendEvent(this, SWT.DefaultSelection);
        }
        return val;
    }

    boolean qt_event_keypress(int widgetHandle, int key,
            int modifier, int character, int nativeScanCode) {
        if (Internal_PackageSupport.qt_event_keypress(this, widgetHandle, key, modifier, character, nativeScanCode)) {
            return true;
        }

        if (key == OS.QT_KEY_RETURN) {
            Internal_PackageSupport.sendEvent(this, SWT.DefaultSelection);
        } else if (Internal_PackageSupport.isSelectionKey(this, 0, character)) {
            Internal_PackageSupport.sendEvent(this, SWT.Selection);
        }
        return false;
    }

    /**
     * Returns a calendar instance, set to a time zone if one is set
     *
     * @return Calendar instance
     */
    Calendar getCalendar(boolean absolute) {
        if (absolute == false && timeZone != null) {
            return Calendar.getInstance(timeZone);
        }
        return Calendar.getInstance();
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
    public void addModifyListener(ModifyListener listener) {
        checkWidget();
        if (listener == null)
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
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
    public void addSelectionListener(SelectionListener listener) {
        checkWidget();
        if (listener == null)
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener(listener);
        addListener(SWT.Selection, typedListener);
        addListener(SWT.DefaultSelection, typedListener);
    }

    /**
     * Returns the date when DATE, TIME or DATE_TIME styles are used. Updating
     * the returned <code>Date</code> instance will not affect the date value in
     * the control.
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
    public Date getDate() {
        checkWidget();
        int xStyle = extraStyle() & 0xf;
        if (xStyle == OFFSET || xStyle == DURATION) {
            return null;
        }

        Calendar c = getCalendar(true);
        int[] dateTime = OS.QDateTimeEdit_swt_dateTime(topHandle());

        c.set(Calendar.YEAR, dateTime[OS.QT_DATETIME_YEAR]);
        c.set(Calendar.MONTH, dateTime[OS.QT_DATETIME_MONTH] - 1);
        c.set(Calendar.DAY_OF_MONTH, dateTime[OS.QT_DATETIME_DAY]);
        c.set(Calendar.HOUR_OF_DAY, dateTime[OS.QT_DATETIME_HOUR]);
        c.set(Calendar.MINUTE, dateTime[OS.QT_DATETIME_MINUTE]);
        c.set(Calendar.SECOND, dateTime[OS.QT_DATETIME_SECOND]);
        c.set(Calendar.MILLISECOND, 0);

        return c.getTime();
    }

    /**
     * Returns the number of seconds when TIME, OFFSET or DURATION styles are
     * used. The return value is the total number of seconds derived from the
     * hours, minutes, and seconds displayed in the widget. The maximum value is
     * +/- 86399 seconds which is equivalent to 23:59:59 in hh:mm:ss.
     *
     * @return time value converted to number of seconds
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                </ul>
     */
    public int getTime() {
        checkWidget();
        int offset = 0;

        int xStyle = extraStyle() & 0xf;
        if (xStyle == TIME || xStyle == DURATION) {
            int[] dateTime = OS
                    .QDateTimeEdit_swt_dateTime(topHandle());
            offset = dateTime[OS.QT_DATETIME_HOUR] * 3600
                    + dateTime[OS.QT_DATETIME_MINUTE] * 60
                    + dateTime[OS.QT_DATETIME_SECOND];
        } else if (xStyle == OFFSET) {
            offset = OS.QOffsetTimeEdit_offset(topHandle());
        }

        return offset;
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
    public void removeModifyListener(ModifyListener listener) {
        checkWidget();
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
    public void removeSelectionListener(SelectionListener listener) {
        checkWidget();
        if (listener == null)
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        removeListener(SWT.Selection, listener);
        removeListener(SWT.DefaultSelection, listener);
    }

    /**
     * Sets the date for the editor when DATE, TIME or DATE_TIME styles are
     * used. The control does not retain the Date object or modify its value.
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
    public void setDate(Date date) {
        checkWidget();
        if (date == null)
            SWT.error(SWT.ERROR_NULL_ARGUMENT);

        if (!textVisible) {
            OS.QSwtTimeEdit_swt_setTextVisible(topHandle(), true);
        }

        _setDate(date);

        if (!textVisible) {
            OS.QLineEdit_setSelection(editorHandle, 2, -2);
            textVisible = true;
        }
    }

    private void _setDate(Date date) {
        Calendar calendar = getCalendar(false);
        calendar.setTime(date);
        setDateTime(calendar);
    }

    /**
     * Sets the time for the editor when TIME, OFFSET or DURATION styles are
     * used.
     *
     * @param seconds
     *            the number of seconds which will be displayed in hours,
     *            minutes, and seconds. The maximum value is +/- 86399 seconds
     *            which is equivalent to 23:59:59 in hh:mm:ss.
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
    public void setTime(int seconds) {
        checkWidget();
        int xStyle = extraStyle() & 0xf;
        if (xStyle == OFFSET) {
            if (seconds > MAXIMUM_OFFSET || seconds < MINIMUM_OFFSET) {
                SWT.error(SWT.ERROR_INVALID_ARGUMENT);
            }
        }
        if (xStyle == TIME || xStyle == DURATION) {
            if (seconds > MAXIMUM_OFFSET || seconds < 0) {
                SWT.error(SWT.ERROR_INVALID_ARGUMENT);
            }
        }

        if (!textVisible) {
            OS.QSwtTimeEdit_swt_setTextVisible(topHandle(), true);
        }

        _setTime(seconds);

        if (!textVisible) {
            if (xStyle == DATE || xStyle == DATE_TIME)
                _setDate(getDate());

            OS.QLineEdit_setSelection(editorHandle, 2, -2);
            textVisible = true;
        }
    }

    void _setTime(int seconds) {
        int xStyle = extraStyle() & 0xf;
        if (xStyle == TIME || xStyle == DURATION) {
            Calendar c = getCalendar(true);

            if (c != null) {
                c.setTime(new Date());
                int hour = seconds / 3600;
                int minute = (seconds % 3600) / 60;
                int second = (seconds % 3600) % 60;

                c.set(Calendar.HOUR_OF_DAY, hour);
                c.set(Calendar.MINUTE, minute);
                c.set(Calendar.SECOND, second);

                setDateTime(c);
            }
        } else if (xStyle == OFFSET) {
            OS.QOffsetTimeEdit_setOffset(topHandle(), seconds);
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
    public void setTimeZone(TimeZone timeZone) {
        checkWidget();
        this.timeZone = timeZone;
        int xStyle = extraStyle() & 0xf;
        if (xStyle == DATE || xStyle == DATE_TIME || xStyle == TIME) {
            setDate(getDate());
        }
    }

    void setTraversalFlags(int type, int key, int modifier,
            int character) {
        Internal_PackageSupport.setTraverseDoIt(this, true);
        Internal_PackageSupport.setTraverseCancel(this, true);

        switch (key) {
        case OS.QT_KEY_LEFT:
            if (OS.QSwtTimeEdit_cursorPosition(topHandle()) > 0) {
                Internal_PackageSupport.setTraverseDoIt(this, false);
                Internal_PackageSupport.setTraverseCancel(this, false);
            }
            break;
        case OS.QT_KEY_RIGHT:
            if (OS.QSwtTimeEdit_cursorPosition(topHandle()) < OS
                    .QSwtTimeEdit_textLength(topHandle())) {
                Internal_PackageSupport.setTraverseDoIt(this, false);
                Internal_PackageSupport.setTraverseCancel(this, false);
            }
            break;
        case OS.QT_KEY_UP:
        case OS.QT_KEY_DOWN:
            Internal_PackageSupport.setTraverseDoIt(this, false);
            Internal_PackageSupport.setTraverseCancel(this, false);
            break;
        case OS.QT_KEY_PAGEUP:
        case OS.QT_KEY_PAGEDOWN:
        case OS.QT_KEY_HOME:
        case OS.QT_KEY_END:
        case OS.QT_KEY_ENTER: // By default, enter key is used for
        case OS.QT_KEY_RETURN: // default selection, not traversing.
            Internal_PackageSupport.setTraverseDoIt(this, false);
            Internal_PackageSupport.setTraverseCancel(this, false);
            break;
        default:
            break;
        }

    }

    private final int handle() {
        return Internal_PackageSupport.handle(this);
    }

    private final int topHandle() {
        return Internal_PackageSupport.topHandle(this);
    }
    private final int extraStyle() {
        return Internal_PackageSupport.extraStyle(this);
    }
}
