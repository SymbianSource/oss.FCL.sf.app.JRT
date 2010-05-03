/*******************************************************************************
 * Copyright (c) 2004, 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * 
 * Contributors:
 *     Yu You (Nokia Corp.)- initial API specification 
 *     Nokia Corporation - S60 implementation
 *     Nokia Corporation - QT implementation
 *******************************************************************************/
package org.eclipse.ercp.swt.mobile;

import org.eclipse.swt.*;
import org.eclipse.swt.events.*;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.internal.qt.PackageProxy;
import org.eclipse.swt.internal.qt.SWTExtension;
import org.eclipse.swt.internal.qt.TextUtils;
import org.eclipse.swt.internal.qt.WidgetState;
import org.eclipse.swt.widgets.*;

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
public class ConstrainedText extends Control {
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
    
    private int validator, variant;

    /*
     * Used to store the position and length of selected text
     */
    Point lastSelection;

    static final class ConstrainedTextPackageProxy extends PackageProxy {
        public void createHandle(int index) {
            ((ConstrainedText)w).createHandle(index);
        }
        public void hookEvents() {
            ((ConstrainedText)w).hookEvents();
        }
        public void qt_signal_text_changed(String text) {
            ((ConstrainedText)w).qt_signal_text_changed(text);
        }
        public void checkBorder() {
            ((ConstrainedText)w).checkBorder();
        }
        public void setTraversalFlags(int type, int key, int modifier, int character) {
            ((ConstrainedText)w).setTraversalFlags(type, key, modifier, character);
        }
        public void qt_signal_text_cursorPositionChanged(int oldPos, int newPos) {
            ((ConstrainedText)w).qt_signal_text_cursorPositionChanged(oldPos, newPos);
        }
        public void qt_signal_text_selectionChanged() {
            ((ConstrainedText)w).qt_signal_text_selectionChanged();
        }
        public void qt_signal_return_pressed() {
            ((ConstrainedText)w).qt_signal_return_pressed();
        }
    }
    
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
public ConstrainedText(Composite parent, int style, int constrainedStyle) {
    this(parent, checkStyle(style), checkConstrainedStyle(constrainedStyle), false);
}

/**
 * <p>
 * <b>IMPORTANT:</b> This constructor is <em>not</em> part of the SWT
 * public API. It should never be referenced from application code. 
 * </p>
 */
protected ConstrainedText(Composite parent, int style, int constrainedStyle, boolean isExtended) {
    super(parent, checkStyle(style), checkConstrainedStyle(constrainedStyle), 
            new ConstrainedTextPackageProxy(), isExtended);
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
public void addModifyListener(ModifyListener listener) {
    checkWidget ();
    if (listener == null) SWT.error (SWT.ERROR_NULL_ARGUMENT);
    TypedListener typedListener = new TypedListener (listener);
    addListener (SWT.Modify, typedListener);
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
public void addSelectionListener(SelectionListener listener) {
    checkWidget ();
    if (listener == null) SWT.error (SWT.ERROR_NULL_ARGUMENT);
    TypedListener typedListener = new TypedListener (listener);
    addListener (SWT.Selection,typedListener);
    addListener (SWT.DefaultSelection,typedListener);
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
public void append(String string) {
    checkWidget();
    if (OS.QValidator_validate(validator, string)) {
        TextUtils.append(variant, handle(), string, OS.QLineEdit_maxLength(handle()), null);
    }
    else {
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
public void clearSelection() {
    checkWidget();
    TextUtils.clearSelection(variant, handle());
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
public void copy() {
    checkWidget();
    TextUtils.copy(variant, handle());
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
public void cut() {
    checkWidget();
    TextUtils.cut(variant, handle(), this);
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
public int getCaretPosition() {
    checkWidget();
    return TextUtils.getCaretPosition(variant, handle());
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
public int getCharCount() {
    checkWidget();
    return TextUtils.getCharCount(variant, handle());
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
public boolean getEditable() {
    checkWidget();
    return !TextUtils.getReadOnly(variant, handle());
}

String getSelectionText () {
    checkWidget();
    return TextUtils.getSelectionText(variant, Internal_PackageSupport.topHandle(this)); 
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
public String getText() {
    checkWidget();
    return TextUtils.getText(variant, handle());
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
public int getTextLimit() {
    checkWidget();
    return OS.QLineEdit_maxLength(handle());
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
public void insert(String string) {
    checkWidget();
    if (OS.QValidator_validate(validator, string)) {
        TextUtils.insert(variant, handle(), string, OS.QLineEdit_maxLength(handle()), null);
    } 
    else {
        SWT.error(SWT.ERROR_INVALID_ARGUMENT);
    }
}

private final boolean isExtended() {
    return ((Internal_PackageSupport.state(this) & WidgetState.IS_EXTENDED) != 0);
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
public void paste() {
    checkWidget();
    if (OS.QValidator_validate(validator, OS.QClipboard_text())) {
        TextUtils.paste(variant, handle(), OS.QLineEdit_maxLength(handle()), null);
    }
    else {
        SWT.error(SWT.ERROR_INVALID_ARGUMENT);
    }
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
public void removeModifyListener(ModifyListener listener) {
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
public void removeSelectionListener(SelectionListener listener) {
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
public void selectAll() {
    checkWidget();
    TextUtils.selectAll(variant, handle());
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
public void setEditable (boolean editable) {
    checkWidget();
    setStyle(style() & ~SWT.READ_ONLY);
    if (!editable) {
        setStyle(style() | SWT.READ_ONLY);
    }
    TextUtils.setReadOnly(variant, handle(), !editable);
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
public void setSelection(int start) {
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
public void setSelection(int start, int end) {
    checkWidget();
    if (start < 0 || end < 0) {
        SWT.error(SWT.ERROR_INVALID_ARGUMENT);
    }
    if (start > end || end > TextUtils.getText(variant, handle()).length()) {
        SWT.error(SWT.ERROR_INVALID_RANGE);
    }
    TextUtils.setSelection(variant, handle(), start, end);
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
public void setText(String text) {
    checkWidget();
    if (OS.QValidator_validate(validator, text)) {
        TextUtils.setText(variant, handle(), text, OS.QLineEdit_maxLength(handle()), null);
    }
    else {
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
public void setTextLimit(int limit) {
    checkWidget();
    if (limit <= 0 || limit > Text.LIMIT) SWT.error(SWT.ERROR_INVALID_ARGUMENT);
    Internal_PackageSupport.sendEvent(this, SWT.Modify);
    OS.QLineEdit_setMaxLength(handle(), limit);
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
public void showSelection() {
    checkWidget();
}

void setTraversalFlags(int type, int key, int modifier, int character) {
    Internal_PackageSupport.setTraverseDoIt(this, false);
    Internal_PackageSupport.setTraverseCancel(this, false);

    if (type == SWT.TRAVERSE_MNEMONIC) {
        Internal_PackageSupport.setTraverseDoIt(this, true);
        return;
    }
    
    switch (key) {
        case OS.QT_KEY_ENTER:
        case OS.QT_KEY_RETURN:
        case OS.QT_KEY_UP:
        case OS.QT_KEY_DOWN:
        case OS.QT_KEY_TAB:
            Internal_PackageSupport.setTraverseDoIt(this, true);
            break;
        case OS.QT_KEY_LEFT:
        case OS.QT_KEY_RIGHT:
            Internal_PackageSupport.setTraverseDoIt(this, false);
            break;
        default:
            Internal_PackageSupport.setTraversalFlags(this, type, key, modifier, character);
            break;
        }
}

void checkBorder () {
    Internal_PackageSupport.checkBorder(this);
    OS.QLineEdit_setFrame(Internal_PackageSupport.topHandle(this), (style() & SWT.BORDER) != 0);
}

void createHandle (int index) {
    if(isExtended()) {
        lastSelection = new Point(0,0);
    }
    
    variant = TextUtils.LINE_EDIT;
    
    int handle = OS.QLineEdit_new();
    Internal_PackageSupport.setHandle(this, handle);
    Internal_PackageSupport.setTopHandle(this, handle);
    
    OS.QWidget_setGeometry(handle, 0, 0, 0, 0);
    OS.QLineEdit_setMaxLength(handle, Text.LIMIT);
    
    int alignment = OS.QT_ALIGNLEFT;
    if ((style() & SWT.CENTER) != 0) { 
        alignment = OS.QT_ALIGNHCENTER; 
    } else if ((style() & SWT.RIGHT) != 0) {
        alignment = OS.QT_ALIGNRIGHT;
    }
    OS.QLineEdit_setAlignment(handle, alignment | OS.QT_ALIGNVCENTER);

    switch(Internal_PackageSupport.extraStyle(this)) {
    case NUMERIC:
        // Intermediate restriction: anything that builds up to a valid integer is allowed
        // I.E.: You can have + alone, since it is the beginning of a positive integer
        validator = OS.QRegExpValidator_new(handle, "[\\+\\-]?\\d*");
        OS.QWidget_setInputMethodHints(handle, OS.QT_IMHDIGITSONLY);
        break;
    case DECIMAL:
        // Intermediate restriction: anything that builds up to a valid real is allowed
        // I.E.: You can have '-.' alone, since it is the beginning of a negative real but
        // cannot have 'E' since a exponential notation needs a valid real before E 
        validator = OS.QRegExpValidator_new(handle, "[\\+\\-]?\\d*(\\.?(\\d+([eE][\\+\\-]?\\d*)?)?)?");
        OS.QWidget_setInputMethodHints(handle, OS.QT_IMHFORMATTEDNUMBERSONLY);
        break;
    case PHONENUMBER:
        // Very loose restriction: any of the chars in the set allowed in any order and amount
        validator = OS.QRegExpValidator_new(handle, "[\\+\\d\\sABCD,TP!W@;>IiGg\\*\\#\\.\\(\\)\\/\\-]*");
        OS.QWidget_setInputMethodHints(handle, OS.QT_IMHDIALABLECHARACTERSONLY);
        break;
    default:
        // Impossible to fall here
        break;
    }
    OS.QLineEdit_setValidator(handle, validator);
    
    Internal_PackageSupport.setState(this, Internal_PackageSupport.state(this) | WidgetState.HANDLE);
}

void hookEvents () {  
    Internal_PackageSupport.hookEvents(this);
    int handle = handle();
    int handler = OS.SignalHandler_new(handle, Internal_PackageSupport.display(this), OS.QSIGNAL_TEXT_CHANGED);
    OS.QObject_connectOrThrow(handle, "textChanged(const QString&)",
        handler, "widgetSignal(const QString&)", OS.QT_AUTOCONNECTION);
    handler = OS.SignalHandler_new(handle, Internal_PackageSupport.display(this), OS.QSIGNAL_RETURN_PRESSED);
    OS.QObject_connectOrThrow(handle, "returnPressed()",
        handler, "widgetSignal()", OS.QT_AUTOCONNECTION);

    if(isExtended()) {
        handler = OS.SignalHandler_new(handle, Internal_PackageSupport.display(this), OS.QSIGNAL_TEXT_CURSORPOSITIONCHANGED);
        OS.QObject_connectOrThrow(handle, "cursorPositionChanged(int, int)",
            handler, "widgetSignal(int, int)", OS.QT_AUTOCONNECTION);
        handler = OS.SignalHandler_new(handle, Internal_PackageSupport.display(this), OS.QSIGNAL_TEXT_SELECTIONCHANGED);
        OS.QObject_connectOrThrow(handle, "selectionChanged()",
            handler, "widgetSignal()", OS.QT_AUTOCONNECTION);
    }
}

void qt_signal_return_pressed() {
    Internal_PackageSupport.sendEvent(this, SWT.DefaultSelection);
}

void qt_signal_text_changed(String text) {
    Internal_PackageSupport.sendEvent(this, SWT.Modify);
}

/*
 * Extended ConstrainedText sends CaretMoved events
 */ 
void qt_signal_text_cursorPositionChanged(int oldPos, int newPos) {
    if(isExtended()) { 
        Event event = new Event();
        event.x = newPos;
        Internal_PackageSupport.sendEvent(this, SWTExtension.CaretMoved, event);
    }
    Internal_PackageSupport.qt_signal_text_cursorPositionChanged(this, oldPos, newPos);
}

/*
 * Extended ConstrainedText sends ContentSelected events
 */ 
void qt_signal_text_selectionChanged() {
    if(isExtended()) {
        Point selection = TextUtils.getSelection(variant, Internal_PackageSupport.topHandle(this));
        int length = selection.y - selection.x;
        
        // Qt sends selectionChanged signals when user drags on the text, even
        // if there the selection content does not change. Filter out excessive 
        // events here.
        if (lastSelection.y - lastSelection.x != length) {
            lastSelection = selection;
    
            Event event = new Event();
            event.x = selection.x;
            event.count = length;
            Internal_PackageSupport.sendEvent(this, SWTExtension.ContentSelected, event);
        }
    }
    Internal_PackageSupport.qt_signal_text_selectionChanged(this);
}

static int checkStyle(int style) {
    return style | SWT.SINGLE;
}

static int checkConstrainedStyle(int style) {
    switch (style) {
        case NUMERIC:
        case DECIMAL:
        case PHONENUMBER:
            return style;
        case 0:
        default:
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
            // Fall through
            return NUMERIC;
    }
}

private final int handle() {
    return Internal_PackageSupport.handle(this); 
}

private final int style() {
    return Internal_PackageSupport.style(this);
}

private final void setStyle(int style) {
    Internal_PackageSupport.setStyle(this, style);
}
}
