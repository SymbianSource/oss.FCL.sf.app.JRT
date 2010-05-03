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

import org.eclipse.swt.*;
import org.eclipse.swt.events.*;
import org.eclipse.swt.widgets.*;
import org.eclipse.ercp.swt.mobile.internal.OS;

/**
 * A single-line Text control which constrains the user input by styles.
 *
 * <p>
 * This control limits the characters that users can enter by the styles to make
 * data entry more efficient. This capability is usually implemented by the
 * underlying platform and applications are not allowed to change or override
 * the constraints. An IllegalArgumentException will be thrown if illegal
 * content is set programmatically by applications.
 * </p>
 * <dl>
 * <dt><b>Constraint Styles (exclusive): </b></dt>
 * <dd>NUMERIC: limit to digits and an optional minus sign in the beginning of
 * the string</dd>
 * <dd>DECIMAL: limit to digits, fractional separator and an optional minus
 * sign in the beginning of the string</dd>
 * <dd>PHONENUMBER: all dialling digitals: 0-9, *, #, +, A, B, C, D and
 * modifier characters (", T P ! W @ ; > I i G g") (see the standard at <a
 * href="http://www.3gpp.org/ftp/Specs/html-info/27007.htm">3GPP TS 27.007, ITU-T dial
 * command </a></dd>
 *
 * <dt><b>Events: </b></dt>
 * <dd>Selection, DefaultSelection, Modify</dd>
 * </dl>
 */
public final class ConstrainedText extends Control
{

    private int constrainedTextHandle;
    private int constrainedStyle;
    private int textLimit = Text.LIMIT;

    /**
     * Numeric input style allows the input of numeric values.
     *
     * <P>
     * Value <code>1</code> is assigned to <code>NUMERIC</code>.
     * </P>
     *
     */
    public static final int NUMERIC = 1;

    /**
     * Decimal input style allows the input of numeric values with optional
     * decimal fractions, for example, "-123", "0.123", or ".5" are all valid
     * input.
     *
     * <P>
     * Value <code>2</code> is assigned to <code>DECIMAL</code>.
     * </P>
     *
     */
    public static final int DECIMAL = 2;

    /**
     * Phone number input style allows the input of numeric values with optional
     * phone specific characters like "+", "* and "#".
     *
     * <P>
     * Value <code>4</code> is assigned to <code>PHONENUMBER</code>.
     * </P>
     *
     */
    public static final int PHONENUMBER = 4;

    /**
     * Constructs a new instance of this class given its parent, a style value
     * describing behaviour and appearance, and an additional constrained style
     * specifying the input styles defined above.
     * <p>
     * The style value is either one of the style constants defined in class
     * <code>SWT</code> which is applicable to instances of this class, or
     * must be built by <em>bitwise OR</em> 'ing together (that is, using the
     * <code>int</code> "|" operator) two or more of those <code>SWT</code>
     * style constants. Style bits are also inherited from superclasses. The
     * class description lists the constrained style constants that can be set
     * in constrainedStyle.
     * </p>
     *
     * @param parent
     *            a composite control which will be the parent of the new
     *            instance (cannot be null)
     * @param style
     *            the style of control to construct
     * @param constrainedStyle
     *            the constrained style
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
     *                </ul>
     *                <ul>
     *                <li>ERROR_INVALID_ARGUMENT - if either the style or the
     *                constrainedStyle is invalid</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                </ul>
     *
     * @see #NUMERIC
     * @see #DECIMAL
     * @see #PHONENUMBER
     */
    public ConstrainedText(Composite parent, int style, int constrainedStyle)
    {
        super(parent, checkStyle(style));

        try
        {
            this.constrainedStyle = constrainedStyle;
            checkConstrainedStyle();
            OS.ConstrainedText_SetConstraint(constrainedTextHandle, checkConstrainedStyle(constrainedStyle));
        }
        catch (Error e)
        {
            if (handle != 0) org.eclipse.swt.internal.symbian.OS.Control_Dispose(handle);
            throw e;
        }
    }

    protected void internal_createHandle()
    {
        constrainedTextHandle = OS.ConstrainedText_New(this, getParent().internal_getCompositeHandle(), internal_getStyle());
        handle = OS.ConstrainedText_ControlHandle(constrainedTextHandle);
    }

    private void checkConstrainedStyle()
    {
        switch (constrainedStyle)
        {
        case NUMERIC:
        case DECIMAL:
        case PHONENUMBER:
            return;
        default:
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }
    }

    static int checkStyle(int style)
    {
        return style | SWT.SINGLE;
    }

    static int checkConstrainedStyle(int style)
    {
        switch (style)
        {
        case NUMERIC:
        case DECIMAL:
        case PHONENUMBER:
            return style;
        default:
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
            // Intentional fallthrough to prevent compiling error
        case 0:
            return NUMERIC;
        }
    }

    /**
     * Adds the listener to the collection of listeners who will be notified
     * when the receiver's text is modified, by sending it one of the messages
     * defined in the <code>ModifyListener</code> interface.
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
        if (listener == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener(listener);
        addListener(SWT.Modify, typedListener);
    }

    /**
     * Adds the listener to the collection of listeners who will be notified
     * when the control is selected, by sending it one of the messages defined
     * in the <code>SelectionListener</code> interface.
     * <p>
     * <code>widgetSelected</code> is not called for this control. <br />
     * <code>widgetDefaultSelected</code> is called when a selection key is
     * pressed.
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
        if (listener == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener(listener);
        addListener(SWT.Selection,typedListener);
        addListener(SWT.DefaultSelection,typedListener);
    }

    /**
     * Appends a string.
     * <p>
     * The new text is appended to the text at the end of the widget. The
     * excessive characters will be truncated if the length exceeds the maximum
     * number of characters that the receiver is capable of holding.
     * </p>
     *
     * @param string
     *            the string to be appended
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the string is null</li>
     *                <li>ERROR_INVALID_ARGUMENT - if the string is invalid
     *                </li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void append(String string)
    {
        checkWidget();
        if (string == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        try
        {
            boolean notify = OS.ConstrainedText_Append(constrainedTextHandle, string);
            if (notify) internal_sendEvent(SWT.Modify);
        }
        catch (Throwable e)
        {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }
    }

    /**
     * Clears the selection.
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void clearSelection()
    {
        checkWidget();
        OS.ConstrainedText_ClearSelection(constrainedTextHandle);
    }

    /**
     * Copies the selected text.
     * <p>
     * The current selection is copied to the clipboard.
     * </p>
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void copy()
    {
        checkWidget();
        OS.ConstrainedText_Copy(constrainedTextHandle);
    }

    /**
     * Cuts the selected text.
     * <p>
     * The current selection is first copied to the clipboard and then deleted
     * from the widget.
     * </p>
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void cut()
    {
        checkWidget();
        boolean notify = OS.ConstrainedText_Cut(constrainedTextHandle);
        if (notify) internal_sendEvent(SWT.Modify);
    }

    /**
     * Gets the position of the caret.
     * <p>
     * The character position of the caret is returned.
     * </p>
     *
     * @return the position of the caret
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public int getCaretPosition()
    {
        checkWidget();
        return OS.ConstrainedText_GetCaretPosition(constrainedTextHandle);
    }

    /**
     * Gets the number of characters.
     *
     * @return number of characters in the widget
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public int getCharCount()
    {
        checkWidget();
        return OS.ConstrainedText_GetCharCount(constrainedTextHandle);
    }

    /**
     * Gets the editable state.
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public boolean getEditable()
    {
        checkWidget();
        return OS.ConstrainedText_GetEditable(constrainedTextHandle);
    }

    /**
     * Gets the widget text.
     * <p>
     * The text for a text widget is the characters in the widget.
     * </p>
     *
     * @return the widget text
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public String getText()
    {
        checkWidget();
        String text = OS.ConstrainedText_GetText(constrainedTextHandle);
        if (text == null)
        {
            text = new String();
        }
        return text;
    }

    /**
     * Returns the maximum number of characters that the receiver is capable of
     * holding.
     * <p>
     * If this has not been changed by <code>setTextLimit()</code>, it will
     * be the constant <code>Text.LIMIT</code>.
     * </p>
     *
     * @return the text limit
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public int getTextLimit()
    {
        checkWidget();
        return textLimit;
    }

    /**
     * Inserts a string after the caret position.
     * <p>
     * The old selection is replaced with the new text. The excessive characters
     * will be truncated if the length exceeds the maximum number of characters
     * that the receiver is capable of holding.
     * </p>
     *
     * @param string
     *            the string value.
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the string is null</li>
     *                <li>ERROR_INVALID_ARGUMENT - if the string is invalid
     *                </li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void insert(String string)
    {
        checkWidget();
        if (string == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        boolean notify = OS.ConstrainedText_Insert(constrainedTextHandle, string);
        if (notify) internal_sendEvent(SWT.Modify);
    }

    /**
     * Pastes text from clipboard.
     * <p>
     * The selected text is deleted from the widget and new text inserted from
     * the clipboard. The excessive characters will be truncated if the length
     * exceeds the maximum number of characters that the receiver is capable of
     * holding.
     * </p>
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_ARGUMENT - if the string is invalid
     *                </li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void paste()
    {
        checkWidget();
        boolean notify = OS.ConstrainedText_Paste(constrainedTextHandle);
        if (notify) internal_sendEvent(SWT.Modify);
    }

    /**
     * Removes the listener from the collection of listeners who will be
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
     * Removes the listener from the collection of listeners who will be
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
     * Selects all the text in the receiver.
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void selectAll()
    {
        checkWidget();
        OS.ConstrainedText_SelectAll(constrainedTextHandle);
    }

    /**
     * Sets the editable state.
     *
     * @param editable
     *            the new editable state
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void setEditable(boolean editable)
    {
        checkWidget();
        OS.ConstrainedText_SetEditable(constrainedTextHandle, editable);
    }

    /**
     * Sets the selection.
     * <p>
     * Indexing is zero based. The range of a selection is from 0..N where N is
     * the number of characters in the widget.
     * </p>
     * <p>
     * Text selections are specified in terms of caret positions. In a text
     * widget that contains N characters, there are N+1 caret positions, ranging
     * from 0..N. This differs from other functions that address character
     * position such as getText () that use the regular array indexing rules.
     * </p>
     *
     * @param start
     *            new caret position
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_ARGUMENT - if the start is less than
     *                zero.</li>
     *                <ul>
     *                <li>ERROR_INVALID_RANGE - if the start value is greater
     *                than N.</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void setSelection(int start)
    {
        setSelection(start, start);
    }

    /**
     * Sets the selection.
     * <p>
     * Indexing is zero based. The range of a selection is from 0..N where N is
     * the number of characters in the widget.
     * </p>
     * <p>
     * Text selections are specified in terms of caret positions. In a text
     * widget that contains N characters, there are N+1 caret positions, ranging
     * from 0..N. This differs from other functions that address character
     * position such as getText () that use the usual array indexing rules.
     * </p>
     *
     * @param start
     *            the start of the range
     * @param end
     *            the end of the range
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_ARGUMENT - if the start or end values
     *                are less than zero.</li>
     *                <ul>
     *                <li>ERROR_INVALID_RANGE - if the start value is greater
     *                than the end value; or if the end value is greater than N.
     *                </li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void setSelection(int start, int end)
    {
        checkWidget();
        if (start < 0) SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        if (end < 0) SWT.error(SWT.ERROR_INVALID_RANGE);
        OS.ConstrainedText_SetSelection(constrainedTextHandle, start, end);
    }

    /**
     * Sets the initial contents of the receiver to the given string. If the
     * argument contains multiple lines of text, the result of this operation is
     * undefined and may vary from platform to platform.
     * <p>
     * It is not recommended to set the initial contents especially when the
     * style is <em>PHONENUMBER</em> because an IllegalArgumentException will
     * be thrown when the string format does not match the present
     * locale-specific format.
     * </p>
     *
     * @param text
     *            the new text
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the string is null</li>
     *                <li>ERROR_INVALID_ARGUMENT - if the string format is
     *                invalid</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void setText(String text)
    {
        checkWidget();
        if (text == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        try
        {
            boolean notify = OS.ConstrainedText_SetText(constrainedTextHandle, text);
            if (notify) internal_sendEvent(SWT.Modify);
        }
        catch (Throwable e)
        {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }
    }

    /**
     * Sets the maximum number of characters that the receiver is capable of
     * holding to be the argument.
     * <p>
     * Instead of trying to set the text limit to zero, consider creating a
     * read-only text widget.
     * </p>
     * <p>
     * To reset this value to the default, use
     * <code>setTextLimit(Text.LIMIT)</code>.
     * </p>
     *
     * @param limit
     *            new text limit
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_ARGUMENT - if the limit is lower or
     *                equal to zero, or greater than Text.LIMIT</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * @see org.eclipse.swt.widgets.Text#LIMIT
     */
    public void setTextLimit(int limit)
    {
        checkWidget();
        if (limit <= 0 || limit > Text.LIMIT) SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        boolean notify = OS.ConstrainedText_SetTextLimit(constrainedTextHandle, limit);
        if (notify) internal_sendEvent(SWT.Modify);
        textLimit = limit;
    }

    /**
     * Shows the selection.
     * <p>
     * If the selection is already showing in the receiver, this method simply
     * returns. Otherwise, lines are scrolled until the selection is visible.
     * </p>
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void showSelection()
    {
        checkWidget();
        OS.ConstrainedText_ShowSelection(constrainedTextHandle);
    }

    /**
     * Sets a hint to the implementation as to the input mode that should be
     * used when the user initiates editing of this control.
     *
     * The argument can be null. If the argument is null, or not a valid Unicode
     * character subset, the default input mode will be used according to the
     * device's locale settings. see <a href="">setInitialInputMode </a> in
     * <code>Text</code>.
     *
     * @param mode
     *            a string naming a Unicode character blocks and unicode
     *            subsets, or null
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * @see Text#setInitialInputMode
     */
    //public void setInitialInputMode(String mode) {
    //}
}
