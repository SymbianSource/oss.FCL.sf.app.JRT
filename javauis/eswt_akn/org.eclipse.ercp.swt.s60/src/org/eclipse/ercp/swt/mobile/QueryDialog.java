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

import org.eclipse.ercp.swt.mobile.internal.OS;
import org.eclipse.swt.SWT;
import org.eclipse.swt.SWTException;
import org.eclipse.swt.widgets.Dialog;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;

/**
 * A modal window used to prompt the end-user for data input.
 *
 * <p>
 * A QueryDialog contains a prompt text and an entry field. The QueryDialog
 * supports five types of entry fields: STANDARD, NUMERIC, PASSWORD, TIME and
 * DATE. The position and size of the dialog is implementation-dependent.
 * </p>
 *
 * <dl>
 * <dt><b>Styles: </b></dt>
 * <dd>APPLICATION_MODAL, PRIMARY_MODAL</dd>
 * <dt><b>Query types: </b></dt>
 * <dd>STANDARD: alphanumeric data input</dd>
 * <dd>NUMERIC: numerical only data input</dd>
 * <dd>PASSWORD: a platform-dependent way for sensitive information input.
 * The initial input mode is set to allow entry of digit characters</dd>
 * <dd>TIME: time input. The string format must be the international standard
 * notation (ISO 8601), hh:mm:ss, where hh is the number of complete hours that
 * have passed since midnight (00-24), mm is the number of complete minutes that
 * have passed since the start of the hour (00-59), and ss is the number of
 * complete seconds since the start of the minute (00-60). If the hour value is
 * 24, then the minute and second values must be zero. If the implementation
 * does not display the number of seconds, the second value must be zero.</dd>
 * <dd>DATE: date input. The string format must be the international date (ISO
 * 8601) notation "YYYY-MM-DD", where YYYY is the year in the usual Gregorian
 * calendar, MM is the month of the year between 01 (January) and 12 (December),
 * and DD is the day of the month between 01 and 31</dd>
 * <dt><b>Events: </b></dt>
 * <dd>(none)</dd>
 * </dl>
 * <p>
 * Note: When the style is either DATE or TIME, the date or time format for input must
 * be locale specific. The data user entered must be constrained to a valid date
 * or time and converted to the ISO 8601 string format that is returned from
 * open().
 * </p>
 * <p>
 * Example:
 *
 * <code><pre>
 * QueryDialog dialog = new QueryDialog(shell, SWT.NONE, QueryDialog.STANDARD);
 * dialog.setPromptText(&quot;Enter name:&quot;, &quot;game1&quot;);
 * String gameName = dialog.open();
 * if (gameName != null) {
 *  // OK
 *  // do something
 * } else {
 *  // Cancelled
 *  // do something else
 * }
 * </pre></code>
 * <p>
 * IMPORTANT: This class is <em>not</em> intended to be subclassed.
 * </p>
 */
public class QueryDialog extends Dialog
{
    private int     queryType;
    private int     minimum;
    private int     maximum;
    private String  promptText;
    private String  defaultValue;
    private boolean isMidnight;

    static final private String     midnightHour = "24:00:00";
    static final private String     platformMidnightHour = "00:00:00";

    /**
     * Alphanumeric data entry type
     */
    public static final int STANDARD = 0;

    /**
     * Password entry type. The input method and resulting string value
     * are platform-dependent.
     */
    public static final int PASSWORD = 1;

    /**
     * Numerical data entry type. Only locale-dependent digit characters are
     * allowed.
     */
    public static final int NUMERIC = 2;

    /**
     * Time entry type. The implementation can decide to use either 12 or 24 hour
     * mode for input, but the returg value is in 24 hour format.
     */
    public static final int TIME = 3;

    /**
     * Date entry type.
     */
    public static final int DATE = 4;

    /**
     * Constructs a new instance of this class given its parent.
     *
     * <p>
     * By default, APPLICATION_MODAL style and STANDARD query type is used.
     * </p>
     *
     * @param parent
     *            a shell which will be the parent of the new instance
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
     * @see #QueryDialog(Shell, int, int)
     */
    public QueryDialog(Shell parent)
    {
        this(parent, SWT.APPLICATION_MODAL, STANDARD);
    }

    /**
     * Constructs a new instance of this class given its parent and style .
     *
     * <p>
     * The style value is either one of the style constants defined in class
     * <code>Dialog</code>. By default STANDARD query type is used.
     * </p>
     *
     * @param parent
     *            a shell which will be the parent of the new instance
     * @param style
     *            the style of control to construct
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                </ul>
     * @see SWT#APPLICATION_MODAL
     * @see SWT#PRIMARY_MODAL
     * @see #QueryDialog(Shell, int, int)
     */
    public QueryDialog(Shell parent, int style)
    {
        this(parent, internal_checkStyle(style), STANDARD);
    }

    /**
     * Constructs a new instance of this class given its parent, style and query
     * type.
     *
     * <p>
     * The style value is either one of the style constants defined in class
     * <code>Dialog</code>.
     * </p>
     *
     * @param parent
     *            a shell which will be the parent of the new instance
     * @param style
     *            the style of control to construct
     * @param queryType
     *            one of STANDARD, NUMERIC, PASSWORD, TIME, or DATE.
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
     *                <li>ERROR_INVALID_ARGUMENT - if the queryType is invalid
     *                </li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                <li>ERROR_INVALID_SUBCLASS - if this class is not an
     *                allowed subclass</li>
     *                </ul>
     * @see SWT#APPLICATION_MODAL
     * @see SWT#PRIMARY_MODAL
     * @see #STANDARD
     * @see #PASSWORD
     * @see #NUMERIC
     * @see #TIME
     * @see #DATE
     */
    public QueryDialog(Shell parent, int style, int queryType)
    {
        super(parent, internal_checkStyle(style));
        checkSubclass();
        if (!internal_checkQueryType(queryType))
        {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }
        this.queryType = queryType;
        this.defaultValue = null;
        isMidnight = false;
    }

    static int internal_checkStyle(int style)
    {
        int mask = SWT.APPLICATION_MODAL | SWT.PRIMARY_MODAL;
        if ((style & mask) == 0) style |= SWT.APPLICATION_MODAL;
        if ((style & SWT.APPLICATION_MODAL) != 0) style = (style & ~mask) | SWT.APPLICATION_MODAL;
        if ((style & SWT.PRIMARY_MODAL) != 0) style = (style & ~mask) | SWT.PRIMARY_MODAL;
        return style;
    }

    static boolean internal_checkQueryType(int queryType)
    {
        if (queryType != STANDARD &&
                queryType != NUMERIC &&
                queryType != PASSWORD &&
                queryType != TIME &&
                queryType != DATE)
        {
            return false;
        }
        return true;
    }

    private void internal_check()
    {
        // method is private but may be changed if further requirement happen.
        Display currentDisplay = getParent().getDisplay();

        if (currentDisplay.getThread() != Thread.currentThread()) SWT.error(SWT.ERROR_THREAD_INVALID_ACCESS);

        // The two following lines are not usefull since accessing to the display provoke the execution
        // of these following lines. The lines remain here so as to remind why just calling getDisplay is performed.
        // if (currentDisplay == null) SWT.error (SWT.ERROR_WIDGET_DISPOSED);
        // if ( getParent().isDisposed() == true) SWT.error (SWT.ERROR_WIDGET_DISPOSED);
    }

    /**
     * Creates the prompt dialog in front of its parent shell and waits for input.
     * This method will return when the input is finished or cancelled.
     *
     * @return String the value entered. Null means that input was cancelled,
     *         regardless of whether a default value was specified. For STANDARD,
     *         NUMERIC, and PASSWORD types, the string contains precisely
     *         the characters entered without any formatting, up to the maximum
     *         number of characters. For DATE and TIME
     *         types, the string contains an ISO 8601 formatted value.
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                <li>ERROR_WIDGET_DISPOSED - if the dialog has been
     *                disposed</li>
     *                </ul>
     */
    public String open()
    {
        this.internal_check();
        OS.QueryDialog_Open(this, getText(), getStyle(), queryType, promptText, defaultValue, minimum, maximum);
        iIsOpen = true;
        //looping until native dialog broker sets the flag (iIsOpen) to false
        internal_waitForClose();
        // get dialog result
        String inputResult = iResultString;
        if (inputResult != null && inputResult.length() == 0)
        {
            inputResult = null;
        }
        iResultString = null;
        if (isMidnight && inputResult.compareTo(platformMidnightHour) == 0)
        {
            inputResult = midnightHour;
        }
        getParent().update();
        return inputResult;
    }

    /**
     * Defines the maximum number of characters that can be entered.
     * If the input string already exceeds the maximum, the excessive part of
     * the value is not displayed.
     *
     * Usually the maximum length is system-dependent, and applications should
     * not specify it.
     *
     * Note: This method has no effect for DATE and TIME types.
     *
     * @param maximum
     *            the maximum character length. Must be equal or greater than
     *            zero. Zero means no limit.
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if maximum is negative, or less
     *                than the minimum length defined by
     *                {@link #setMinimum(int) setMinimum(int)}.</li>
     *                <li>ERROR_INVALID_ARGUMENT - if maximum is greater than
     *                the real maximum length the receiver can hold. Typically
     *                the real maximum length is defined by
     *                {@link org.eclipse.swt.widgets.Text#LIMIT Text.LIMIT}
     *                </li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                <li>ERROR_WIDGET_DISPOSED - if the dialog has been
     *                disposed</li>
     *                </ul>
     *
     * @see #setMinimum(int)
     */
    public void setMaximum(int maximum)
    {
        internal_check();
        if (maximum < 0 || maximum < minimum)
        {
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        }
        if (maximum > Text.LIMIT)
        {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }
        this.maximum = maximum;
    }

    /**
     * Defines the minimum number of characters that must be entered before
     * the dialog can be completed (not cancelled).
     *
     * Note: This method has no effect for DATE and TIME types.
     *
     * @param minimum
     *            the minimum number of characters. Must be equal or greater than
     *            zero. Zero means no limit.
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if minimum is negative, or
     *                greater than the maximum length defined by
     *                {@link #setMaximum(int) setMaximum(int)}.</li>
     *                </li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                <li>ERROR_WIDGET_DISPOSED - if the dialog has been
     *                disposed</li>
     *                </ul>
     *
     * @see #setMaximum(int)
     */
    public void setMinimum(int minimum)
    {
        internal_check();
        if (minimum < 0 || (maximum > 0 && minimum > maximum))
        {
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        }
        this.minimum = minimum;
    }

    /**
     * Sets the prompt text and default input value.
     *
     * @param promptText
     *            the prompt text. Can be null.
     * @param defaultValue
     *            the initial value. Cannot be null. The {@link #open() open()}
     *            method may return the same value even when no input is
     *            received from the end-user. If the value length is greater than
     *            the maximum number of characters, only the maximum will be
     *            displayed.
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if defaultValue is null</li>
     *                <li>ERROR_INVALID_ARGUMENT - if defaultValue format is
     *                invalid (only for DATE and TIME styles)</li>
     *                </li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                <li>ERROR_WIDGET_DISPOSED - if the dialog has been
     *                disposed</li>
     *                </ul>
     */
    public void setPromptText(String promptText, String defaultValue)
    {
        internal_check();
        boolean isMidnightOld = isMidnight;
        isMidnight = false;
        if (defaultValue == null)
        {
            isMidnight = isMidnightOld;
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        }
        if (checkFormat(defaultValue))
        {
            if (queryType == DATE || queryType == TIME)
            {
                try
                {
                    // Convert default value from ISO 8601 to OS format (check the validity of date or time too)
                    this.defaultValue = OS.QueryDialog_ConvertDefaultValue(defaultValue, queryType);
                }
                catch (Throwable th)
                {
                    isMidnight = isMidnightOld;
                    SWT.error(SWT.ERROR_NULL_ARGUMENT);
                }
            }
            else if (maximum > 0 && defaultValue.length() > maximum)
            {
                this.defaultValue = defaultValue.substring(0, maximum);
            }
            else
            {
                this.defaultValue = defaultValue;
            }
            this.promptText = promptText;
        }
        else
        {
            isMidnight = isMidnightOld;
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }
    }

    private boolean checkFormat(String text)
    {
        switch (queryType)
        {
        case NUMERIC:
        {
            try
            {
                int value = Integer.parseInt(text);
                if (value < 0)
                {
                    return false;
                }
            }
            catch (Throwable th)
            {
                return false;
            }
        }
        break;
        case TIME:
        {
            try
            {
                if (text.length() != 8 || text.charAt(2) != ':' || text.charAt(5) != ':')
                {
                    return false;
                }
                String hour = text.substring(0,2);
                int hourInt = Integer.parseInt(hour);
                if (hourInt < 0)
                {
                    return false;
                }
                String minutes = text.substring(3,5);
                int minutesInt = Integer.parseInt(minutes);
                if ((hourInt == 24 && minutesInt != 0) || minutesInt < 0 || minutesInt > 59)
                {
                    return false;
                }
                String seconds = text.substring(6,8);
                int secondsInt = Integer.parseInt(seconds);
                if ((hourInt == 24 && secondsInt != 0) || secondsInt < 0 || secondsInt > 60)
                {
                    return false;
                }
                if (hourInt == 24)
                {
                    isMidnight = true;
                }
            }
            catch (Throwable th)
            {
                return false;
            }
        }
        break;
        case DATE:
        {
            try
            {
                if (text.length() != 10 || text.charAt(4) != '-' || text.charAt(7) != '-')
                {
                    return false;
                }
                String year = text.substring(0,4);
                int yearInt = Integer.parseInt(year);
                if (yearInt < 0)
                {
                    return false;
                }
                String month = text.substring(5,7);
                int monthInt = Integer.parseInt(month);
                if (monthInt < 1 || monthInt > 12)
                {
                    return false;
                }
                String day = text.substring(8,10);
                int dayInt = Integer.parseInt(day);
                if (dayInt < 1 || dayInt > 31)
                {
                    return false;
                }
            }
            catch (Throwable th)
            {
                return false;
            }
        }
        break;
        }
        return true;
    }
}
