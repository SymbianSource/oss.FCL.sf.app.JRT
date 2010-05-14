/*******************************************************************************
 * Copyright (c) 2004, 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Yu You (Nokia Corp.) - initial API specification
 *     Nokia Corporation - S60 implementation
 *     Nokia Corporation - QT implementation
 *******************************************************************************/
package org.eclipse.ercp.swt.mobile;

import org.eclipse.swt.SWT;
import org.eclipse.swt.SWTException;
import org.eclipse.swt.widgets.Dialog;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Internal_PackageSupport;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Widget;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.events.TraverseEvent;
import org.eclipse.swt.events.TraverseListener;
import org.eclipse.ercp.swt.mobile.DateEditor;
import org.eclipse.swt.internal.qt.OS;
import java.util.Date;
import java.util.Calendar;


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
public class QueryDialog extends Dialog {
    private int     queryType;
    private int     minimum;
    private int     maximum;
    private String  promptText;
    private String  defaultValue;
    private boolean isMidnight;

    private static final String     midnightHour = "24:00:00";
    private static final String     platformMidnightHour = "00:00:00";

    private String dialogID;
    static int dialogCount;

    /**
     * The maximum number of digits that can be entered after the decimals point.
     */
    private static final int MAX_DECIMALS = 10;

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
     * mode for input, but the return value is in 24 hour format.
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
    public QueryDialog(Shell parent) {
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
    public QueryDialog(Shell parent, int style) {
        this(parent, style, STANDARD);
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
    public QueryDialog(Shell parent, int style, int queryType) {
        super(parent, checkStyle(parent, style));
        checkSubclass();
        checkDialog();

        if (!internal_checkQueryType(queryType)) {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }

        dialogCount++;
        dialogID = toString() + String.valueOf(dialogCount);

        this.queryType = queryType;
        this.defaultValue = null;
        isMidnight = false;
    }


    static protected int checkStyle(Shell parent, int style) {
        style = Internal_PackageSupport.checkStyle(parent, style);

        int mask = SWT.APPLICATION_MODAL | SWT.PRIMARY_MODAL;
        if ((style & mask) == 0) style |= SWT.APPLICATION_MODAL;
        if ((style & SWT.APPLICATION_MODAL) != 0) style = (style & ~mask) | SWT.APPLICATION_MODAL;
        if ((style & SWT.PRIMARY_MODAL) != 0) style = (style & ~mask) | SWT.PRIMARY_MODAL;
        return style;
    }

    static boolean internal_checkQueryType(int queryType) {
        if (queryType != STANDARD &&
            queryType != NUMERIC &&
            queryType != PASSWORD &&
            queryType != TIME &&
            queryType != DATE) {
            return false;
        }
        return true;
    }

    private void checkDialog() {
        Display currentDisplay = getParent().getDisplay();
        if (currentDisplay.getThread() != Thread.currentThread ()) SWT.error(SWT.ERROR_THREAD_INVALID_ACCESS);
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
    public String open() {
        checkDialog();
        String result = null;
        if (queryType == TIME || queryType == DATE) {
            internal_DateTimeDialog dialog = new internal_DateTimeDialog();
            result = dialog.open();
            if (isMidnight && result.equals(platformMidnightHour)) {
                result = midnightHour;
            }

        } else {

            DisposeListener listener = new DisposeListener() {
                public void widgetDisposed(DisposeEvent e) {
                    if (e.widget == getParent()) {
                        // Close dialogs which are opened on top of parent.
                        OS.QDialog_swt_closeDialogs(handle(getParent()), dialogID);
                    }
                }
            };
            getParent().addDisposeListener(listener);

            int qtLayoutDirection = (Internal_PackageSupport.style(this) & SWT.RIGHT_TO_LEFT) != 0 ? OS.QT_RIGHTTOLEFT
                    : OS.QT_LEFTTORIGHT;
            if (queryType == STANDARD || queryType == PASSWORD) {

                int echoMode = (queryType == PASSWORD) ? OS.QLINEEDIT_ECHOMODE_PASSWORD : OS.QLINEEDIT_ECHOMODE_NORMAL;
                result = OS.QInputDialog_swt_getText(handle(getParent()), getText(), promptText, echoMode, defaultValue, dialogID, qtLayoutDirection);

            } else { // NUMERIC

                double max = Double.MAX_VALUE;
                double defaultInput = (defaultValue == null) ? 0 : Double.parseDouble(defaultValue);

                if (maximum > 0) {
                    // Calculate the max double value that can be entered
                    // with the current maximum
                    String str = "";
                    for (int i = 0; i < maximum; ++i) {
                        str += 9;
                    }
                    max = Double.parseDouble(str);
                }

                result = OS.QInputDialog_swt_getDouble(handle(getParent()), getText(), promptText, -max, max,
                                                    defaultInput, MAX_DECIMALS, dialogID, qtLayoutDirection);
            }

            if (getParent() != null && !getParent().isDisposed()) {
                getParent().removeDisposeListener(listener);
            }
        }

        return result;
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
    public void setMaximum(int maximum) {
        checkDialog();
        if (maximum < 0 || maximum < minimum) {
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        }

        // 0x7fff is the default max length for QLineEdit
        if (maximum > 0x7fff) {
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
    public void setMinimum(int minimum) {
        checkDialog();
        if (minimum < 0 || (maximum > 0 && minimum > maximum)) {
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
    public void setPromptText(String promptText, String defaultValue) {
        checkDialog();

        if (defaultValue == null) {
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        }

        if (checkFormat(defaultValue)) {
            if (queryType != DATE && queryType != TIME &&
                maximum > 0 && defaultValue.length() > maximum) {
                this.defaultValue = defaultValue.substring(0, maximum);
            } else {
                this.defaultValue = defaultValue;
            }
            this.promptText = promptText;
        } else {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }
    }

    private boolean checkFormat(String text) {
        switch (queryType) {
            case NUMERIC:
            {
                try {
                    Double.parseDouble(text);
                } catch (NumberFormatException e) {
                    return false;
                }
            }
            break;
            case TIME:
            {
                try {
                    if (text.length() != 8 || text.charAt(2) != ':' || text.charAt(5) != ':') {
                        return false;
                    }
                    String hour = text.substring(0,2);
                    int hourInt = Integer.parseInt(hour);
                    if (hourInt < 0) {
                        return false;
                    }
                    String minutes = text.substring(3,5);
                    int minutesInt = Integer.parseInt(minutes);
                    if ((hourInt == 24 && minutesInt != 0) || minutesInt < 0 || minutesInt > 59) {
                        return false;
                    }
                    String seconds = text.substring(6,8);
                    int secondsInt = Integer.parseInt(seconds);
                    if ((hourInt == 24 && secondsInt != 0) || secondsInt < 0 || secondsInt > 60) {
                        return false;
                    }
                    isMidnight = (hourInt == 24);
                } catch (NumberFormatException e) {
                    return false;
                }
            }
            break;
            case DATE:
            {
                try {
                    if (text.length() != 10 || text.charAt(4) != '-' || text.charAt(7) != '-') {
                        return false;
                    }
                    String year = text.substring(0,4);
                    int yearInt = Integer.parseInt(year);
                    if (yearInt < 0) {
                        return false;
                    }
                    String month = text.substring(5,7);
                    int monthInt = Integer.parseInt(month);
                    if (monthInt < 1 || monthInt > 12) {
                        return false;
                    }
                    String day = text.substring(8,10);
                    int dayInt = Integer.parseInt(day);
                    if (dayInt < 1 || dayInt > 31) {
                        return false;
                    }
                } catch (NumberFormatException e) {
                    return false;
                }
            }
            break;
        }
        return true;
    }

    /**
     * Custom dialog implementation for DATE and TIME mode QueryDialog. Dialog shell
     * includes a Label for prompt text, DateEditor for time/date input and OK and Cancel buttons.
     * Controls of the dialog are layouted into a GridLayout.
     */
    private class internal_DateTimeDialog implements SelectionListener {

        private Shell shell;
        private DateEditor editor;
        private Button okButton;
        private Button cancelButton;
        private Date retValue;

        public internal_DateTimeDialog() {
            retValue = null;
        }

        public String open() {
            createShell();
            shell.open();
            Display display = getParent().getDisplay();
            while (!shell.isDisposed()) {
                if (!display.readAndDispatch()) display.sleep();
            }

            if (retValue == null) {
                return null;
            }

            Calendar calendar = Calendar.getInstance();
            calendar.setTime(retValue);

            if (queryType == TIME) {
                return getIntValue(calendar.get(Calendar.HOUR_OF_DAY)) + ":" +
                       getIntValue(calendar.get(Calendar.MINUTE)) + ":" +
                       getIntValue(calendar.get(Calendar.SECOND));
            } else {
                return getIntValue(calendar.get(Calendar.YEAR)) + "-" +
                       getIntValue(calendar.get(Calendar.MONTH) + 1) + "-" +
                       getIntValue(calendar.get(Calendar.DAY_OF_MONTH));
            }
        }

        private void createShell() {
            shell = new Shell(getParent(), SWT.DIALOG_TRIM | getStyle());
            shell.setText(getText());

            Label label = new Label(shell, SWT.HORIZONTAL | SWT.LEFT | SWT.WRAP);
            if (promptText != null) {
                label.setText(promptText);
            }

            int orientationBits = SWT.RIGHT_TO_LEFT | SWT.LEFT_TO_RIGHT;
            int editorStyle = (queryType == QueryDialog.DATE) ? DateEditor.DATE : DateEditor.TIME;
            editor = new DateEditor(shell, getStyle() & orientationBits, editorStyle);

            if(queryType == QueryDialog.DATE)
                editor.setDate(new Date());
            else
                editor.setTime(0);

            // Do always return traverse from the editor, so that default button of the dialog
            // gets the enter key press
            editor.addTraverseListener(new TraverseListener() {
                public void keyTraversed(TraverseEvent e) {
                    if (e.detail == SWT.TRAVERSE_RETURN) {
                        e.doit = true;
                    }
                }
            });

            if (defaultValue != null) {
                Calendar calendar = Calendar.getInstance();
                if (queryType == DATE) {
                    calendar.set(Calendar.YEAR, Integer.parseInt(defaultValue.substring(0, 4)));
                    calendar.set(Calendar.MONTH, Integer.parseInt(defaultValue.substring(5, 7)) - 1);
                    calendar.set(Calendar.DAY_OF_MONTH, Integer.parseInt(defaultValue.substring(8)));
                } else {
                    calendar.set(Calendar.HOUR_OF_DAY, Integer.parseInt(defaultValue.substring(0,2)));
                    calendar.set(Calendar.MINUTE, Integer.parseInt(defaultValue.substring(3,5)));
                    calendar.set(Calendar.SECOND, Integer.parseInt(defaultValue.substring(6,8)));
                }
                editor.setDate(calendar.getTime());
            }

            cancelButton = new Button(shell, SWT.PUSH);
            okButton = new Button(shell, SWT.PUSH);
            shell.setDefaultButton(okButton);

            okButton.setText(SWT.getMessage("ok"));
            cancelButton.setText(SWT.getMessage("cancel"));
            okButton.addSelectionListener(this);
            cancelButton.addSelectionListener(this);

            // Layout content
            GridLayout gridLayout = new GridLayout(2, false);
            shell.setLayout(gridLayout);

            GridData labelData = new GridData(SWT.FILL, SWT.END, false, true, 2, 1);
            GridData editorData = new GridData(SWT.FILL, SWT.CENTER, false, true, 2, 1);
            label.setLayoutData(labelData);
            editor.setLayoutData(editorData);

            GridData cancelButtonData = new GridData(SWT.END, SWT.TOP, true, true, 1, 1);
            GridData okButtonData = new GridData(SWT.END, SWT.TOP, false, true, 1, 1);
            cancelButton.setLayoutData(cancelButtonData);
            okButton.setLayoutData(okButtonData);

            shell.pack();
            shell.layout(true);
        }

        private String getIntValue(int value) {
            String string = "";
            if (value < 10) {
                string += Integer.toString(0);
            }
            return string + Integer.toString(value);
        }

        public void widgetDefaultSelected(SelectionEvent e) {
        }

        public void widgetSelected(SelectionEvent e) {
            if (e.widget == okButton) {
                retValue = editor.getDate();
            }
            shell.dispose();
        }
    }

    private final int handle(Widget w) {
        return Internal_PackageSupport.handle(w);
    }
}
