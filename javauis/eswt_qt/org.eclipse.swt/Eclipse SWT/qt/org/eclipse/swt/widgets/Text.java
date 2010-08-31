/*******************************************************************************
 * Copyright (c) 2000, 2007 IBM Corporation and others.
 * Portion Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *     Nokia Corporation - Qt implementation
 *******************************************************************************/
package org.eclipse.swt.widgets;


import org.eclipse.swt.SWT;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.internal.qt.TextUtils;
import org.eclipse.swt.internal.qt.WidgetConstant;
import org.eclipse.swt.internal.qt.WidgetState;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.events.VerifyListener;

/**
 * Instances of this class are selectable user interface
 * objects that allow the user to enter and modify text.
 * <p>
 * <dl>
 * <dt><b>Styles:</b></dt>
 * <dd>CANCEL, CENTER, LEFT, MULTI, PASSWORD, SEARCH, SINGLE, RIGHT, READ_ONLY, WRAP</dd>
 * <dt><b>Events:</b></dt>
 * <dd>DefaultSelection, Modify, Verify</dd>
 * </dl>
 * <p>
 * Note: Only one of the styles MULTI and SINGLE may be specified,
 * and only one of the styles LEFT, CENTER, and RIGHT may be specified.
 * </p><p>
 * IMPORTANT: This class is <em>not</em> intended to be subclassed.
 * </p>
 */
public class Text extends Scrollable {

    /*
     *  The text limit set by the application. In MULTI case limiting for the text length 
     *  is implemented on Java side as QTextEdit does not have support for it. 
     */
    private int textLimit;
    
    /*
     * The double click enabled flag.
     */
    private boolean doubleClick;
    
    int variant;
        
    /**
    * The maximum number of characters that can be entered
    * into a text widget.
    * <p>
    * Note that this value is platform dependent, based upon
    * the native widget implementation.
    * </p>
    */
    public final static int LIMIT;
    
    /**
    * The delimiter used by multi-line text widgets.  When text
    * is queried and from the widget, it will be delimited using
    * this delimiter.
    */
    public final static String DELIMITER;
    
    /*
    * These values can be different on different platforms.
    * Therefore they are not initialized in the declaration
    * to stop the compiler from inlining.
    */
    static {
        LIMIT = 0x7FFFFFFF;
        DELIMITER = "\n";
    }

/**
 * Constructs a new instance of this class given its parent
 * and a style value describing its behavior and appearance.
 * <p>
 * The style value is either one of the style constants defined in
 * class <code>SWT</code> which is applicable to instances of this
 * class, or must be built by <em>bitwise OR</em>'ing together 
 * (that is, using the <code>int</code> "|" operator) two or more
 * of those <code>SWT</code> style constants. The class description
 * lists the style constants that are applicable to the class.
 * Style bits are also inherited from superclasses.
 * </p>
 *
 * @param parent a composite control which will be the parent of the new instance (cannot be null)
 * @param style the style of control to construct
 *
 * @exception IllegalArgumentException <ul>
 *    <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
 * </ul>
 * @exception SWTException <ul>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the parent</li>
 *    <li>ERROR_INVALID_SUBCLASS - if this class is not an allowed subclass</li>
 * </ul>
 *
 * @see SWT#SINGLE
 * @see SWT#MULTI
 * @see SWT#READ_ONLY
 * @see SWT#WRAP
 * @see Widget#checkSubclass
 * @see Widget#getStyle
 */
public Text (Composite parent, int style) {
    this(parent, style, 0, null, false);
}

/**
 * <p>
 * <b>IMPORTANT:</b> This constructor is <em>not</em> part of the SWT
 * public API. It should never be referenced from application code. 
 * </p>
 */
protected Text (Composite parent, int style, int extraStyle, Object packageProxy, boolean isExtended) {
    super (parent, checkStyle (style), extraStyle, packageProxy, isExtended);
}

static int checkStyle (int style) {
    if ((style & SWT.SINGLE) != 0 && (style & SWT.MULTI) != 0) {
        style &= ~SWT.MULTI;
    }
    style = checkBits (style, SWT.LEFT, SWT.CENTER, SWT.RIGHT, 0, 0, 0);
    if ((style & SWT.SINGLE) != 0) style &= ~(SWT.H_SCROLL | SWT.V_SCROLL | SWT.WRAP);
    if ((style & SWT.WRAP) != 0) {
        style |= SWT.MULTI;
        style &= ~SWT.H_SCROLL;
    }
    if ((style & SWT.MULTI) != 0) style &= ~SWT.PASSWORD;
    if ((style & (SWT.SINGLE | SWT.MULTI)) != 0) return style;
    if ((style & (SWT.H_SCROLL | SWT.V_SCROLL)) != 0) return style | SWT.MULTI;
    return style | SWT.SINGLE;
}

void createHandle_pp (int index) {
    if(variant == 0) {
        variant = ((getStyle() & SWT.SINGLE) != 0 ? TextUtils.LINE_EDIT : TextUtils.TEXT_EDIT);
    }
    
    int alignment = OS.QT_ALIGNLEFT;
    if ((style & SWT.CENTER) != 0) { 
        alignment = OS.QT_ALIGNHCENTER; 
    } else if ((style & SWT.RIGHT) != 0) {
        alignment = OS.QT_ALIGNRIGHT;
    }
    
    if (variant == TextUtils.LINE_EDIT) {
        
        scrollAreaHandle = 0;
        topHandle = handle = OS.QLineEdit_new();
        
        OS.QLineEdit_setAlignment(handle, alignment | OS.QT_ALIGNVCENTER);
        OS.QLineEdit_setMaxLength(handle, LIMIT);
        
        if ((style & SWT.PASSWORD) != 0) {
            OS.QLineEdit_setEchoMode(handle, OS.QLINEEDIT_ECHOMODE_PASSWORD);
        }
        
    } else {
        
        frameHandle = topHandle = scrollAreaHandle = OS.QTextEdit_new();
        handle = OS.QAbstractScrollArea_viewPort(scrollAreaHandle);
        
        OS.QTextEdit_setAlignment(scrollAreaHandle, alignment);
        
        forceTextLayout();
    }
    
    OS.QWidget_setGeometry(topHandle, 0, 0, 0, 0);
    
    state |= WidgetState.HANDLE;
    
    if ((style & SWT.READ_ONLY) != 0) {
        setEditable(false);
    }
}

void createWidget (int index) {
    super.createWidget(index);
    doubleClick = true;
    textLimit = 0;
}

/**
 * Adds the listener to the collection of listeners who will
 * be notified when the receiver's text is modified, by sending
 * it one of the messages defined in the <code>ModifyListener</code>
 * interface.
 *
 * @param listener the listener which should be notified
 *
 * @exception IllegalArgumentException <ul>
 *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
 * </ul>
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 *
 * @see ModifyListener
 * @see #removeModifyListener
 */
public void addModifyListener(ModifyListener listener) {
    checkWidget();
    if (listener == null) {
        error(SWT.ERROR_NULL_ARGUMENT);
    }
    TypedListener typedListener = new TypedListener(listener);
    addListener(SWT.Modify, typedListener);
}

/**
 * Adds the listener to the collection of listeners who will
 * be notified when the control is selected by the user, by sending
 * it one of the messages defined in the <code>SelectionListener</code>
 * interface.
 * <p>
 * <code>widgetSelected</code> is not called for texts.
 * <code>widgetDefaultSelected</code> is typically called when ENTER is pressed in a single-line text,
 * or when ENTER is pressed in a search text. If the receiver has the <code>SWT.SEARCH | SWT.CANCEL</code> style
 * and the user cancels the search, the event object detail field contains the value <code>SWT.CANCEL</code>.
 * </p>
 *
 * @param listener the listener which should be notified when the control is selected by the user
 *
 * @exception IllegalArgumentException <ul>
 *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
 * </ul>
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 *
 * @see SelectionListener
 * @see #removeSelectionListener
 * @see SelectionEvent
 */
public void addSelectionListener(SelectionListener listener) {
    checkWidget ();
    if (listener == null) { 
        error(SWT.ERROR_NULL_ARGUMENT);
    }
    TypedListener typedListener = new TypedListener(listener);
    addListener(SWT.Selection,typedListener);
    addListener(SWT.DefaultSelection,typedListener);
}

/**
 * Adds the listener to the collection of listeners who will
 * be notified when the receiver's text is verified, by sending
 * it one of the messages defined in the <code>VerifyListener</code>
 * interface.
 *
 * @param listener the listener which should be notified
 *
 * @exception IllegalArgumentException <ul>
 *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
 * </ul>
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 *
 * @see VerifyListener
 * @see #removeVerifyListener
 */
public void addVerifyListener(VerifyListener listener) {
    checkWidget();
    if (listener == null) {
        error(SWT.ERROR_NULL_ARGUMENT);
    }
    TypedListener typedListener = new TypedListener(listener);
    addListener(SWT.Verify, typedListener);
}

/**
 * Appends a string.
 * <p>
 * The new text is appended to the text at
 * the end of the widget.
 * </p>
 *
 * @param string the string to be appended
 *
 * @exception IllegalArgumentException <ul>
 *    <li>ERROR_NULL_ARGUMENT - if the string is null</li>
 * </ul>
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public void append (String string) {
    checkWidget();
    TextUtils.append(variant, topHandle, string, textLimit, this);
}

void checkBorder_pp () {
    super.checkBorder_pp();
    if (variant == TextUtils.LINE_EDIT) {
        OS.QLineEdit_setFrame(topHandle, (style & SWT.BORDER) != 0);
    }
}

/**
 * Clears the selection.
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public void clearSelection () {
    checkWidget();
    TextUtils.clearSelection(variant, topHandle);
}

    public Point computeSize(int wHint, int hHint, boolean changed) {
        checkWidget();

        if (changed) {
            OS.QWidget_updateGeometry(topHandle);
        }

        if (wHint != SWT.DEFAULT && wHint < 0)
            wHint = SWT.DEFAULT;
        if (hHint != SWT.DEFAULT && hHint < 0)
            hHint = SWT.DEFAULT;

        Point preferredSize;
        if (wHint != SWT.DEFAULT && hHint != SWT.DEFAULT) {
            preferredSize = new Point(wHint, hHint);
        } else if (variant == TextUtils.LINE_EDIT) {

            if (wHint == SWT.DEFAULT && hHint == SWT.DEFAULT) {
                preferredSize = getPreferredSingleLineClientAreaSize();
            } else if (hHint == SWT.DEFAULT) {
                preferredSize = new Point(wHint,
                        getPreferredSingleLineClientAreaSize().y);
            } else {
                preferredSize = new Point(
                        getPreferredSingleLineClientAreaSize().x, hHint);
            }
        } else {
            if (wHint == SWT.DEFAULT && hHint == SWT.DEFAULT) {
                preferredSize = getPreferredClientAreaSize(-1);
            } else if (hHint == SWT.DEFAULT) {
                preferredSize = new Point(wHint, getPreferredClientAreaSize(wHint).y);
            } else {
                preferredSize = new Point(getPreferredClientAreaSize(hHint).y, hHint);
            }
        }

        Rectangle trim = computeTrim(0, 0, preferredSize.x, preferredSize.y);

        return new Point(trim.width, trim.height);
    }


public Rectangle computeTrim(int x, int y, int width, int height) {
    checkWidget();
    
    if (variant == TextUtils.LINE_EDIT) {
        if ((style & SWT.BORDER) != 0) {
            int border = OS.QLineEdit_swt_getBorderWidth(topHandle);
            x -= border;
            y -= border;
            width += 2 * border;
            height += 2 * border;
        } 
        return new Rectangle(x, y, width, height);
    } else {
        return super.computeTrim(x, y, width, height);
    }
}

/**
 * Copies the selected text.
 * <p>
 * The current selection is copied to the clipboard.
 * </p>
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public void copy () {
    checkWidget();
    TextUtils.copy(variant, topHandle);
}

/**
 * Cuts the selected text.
 * <p>
 * The current selection is first copied to the
 * clipboard and then deleted from the widget.
 * </p>
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public void cut () {
    checkWidget();
    TextUtils.cut(variant, topHandle, this);
}

void deregister_pp () {  
    super.deregister_pp ();
    if (variant == TextUtils.TEXT_EDIT) {
        Display.removeWidget(scrollAreaHandle);
    }
}

void forceTextLayout() {
    // Hack: Qt does not do the layouting of the QTextDocument until QTextEdit comes visible on the screen.
    // Need to force the layouting of the invisible QTextEdit/QTextDocument here by changing the line wrap mode.
    // (Qt's default is QTEXTEDIT_WIDGETWIDTH).
    // This is needed in order that method's like getLineHeight(), getCaretLineNumber(), getCaretLocation(),
    // setTopIndex, etc. work correctly also for invisble text widgets.
    if (isVisible()) return;
    OS.QTextEdit_setLineWrapMode(scrollAreaHandle, OS.QTEXTEDIT_NOWRAP);
    if ((style & SWT.WRAP) != 0) {
        OS.QTextEdit_setLineWrapMode(scrollAreaHandle, OS.QTEXTEDIT_WIDGETWIDTH);
    }
}

public int getBorderWidth () {
   checkWidget(); 
   if (variant == TextUtils.LINE_EDIT) {
       return (style & SWT.BORDER) != 0 ? OS.QLineEdit_swt_getBorderWidth(topHandle) : 0;
   } else {
       return super.getBorderWidth();
   }
}

/**
 * Returns the line number of the caret.
 * <p>
 * The line number of the caret is returned.
 * </p>
 *
 * @return the line number
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public int getCaretLineNumber () {
    checkWidget();
    if (variant == TextUtils.LINE_EDIT) {
        return 0;
    } else {
        return OS.QTextEdit_swt_getCaretLineNumber(topHandle);
    }
}

/**
 * Returns a point describing the receiver's location relative
 * to its parent (or its display if its parent is null).
 * <p>
 * The location of the caret is returned.
 * </p>
 *
 * @return a point, the location of the caret
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public Point getCaretLocation () {
    checkWidget();
    if (variant == TextUtils.LINE_EDIT) {
        // Not supported in Qt
        return new Point(0, 0);
    } else {
        Rectangle cursorRect = OS.QTextEdit_cursorRect(topHandle);  
        return new Point(cursorRect.x, cursorRect.y);
    }
}

/**
 * Returns the character position of the caret.
 * <p>
 * Indexing is zero based.
 * </p>
 *
 * @return the position of the caret
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public int getCaretPosition () {
    checkWidget();
    return TextUtils.getCaretPosition(variant, topHandle);
}

/**
 * Returns the number of characters.
 *
 * @return number of characters in the widget
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public int getCharCount () {
    checkWidget();
    return TextUtils.getCharCount(variant, topHandle);
}

/**
 * Returns the double click enabled flag.
 * <p>
 * The double click flag enables or disables the
 * default action of the text widget when the user
 * double clicks.
 * </p>
 * 
 * @return whether or not double click is enabled
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public boolean getDoubleClickEnabled () {
    checkWidget ();
    return doubleClick;
}

/**
 * Returns the echo character.
 * <p>
 * The echo character is the character that is
 * displayed when the user enters text or the
 * text is changed by the programmer.
 * </p>
 * 
 * @return the echo character
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 * 
 * @see #setEchoChar
 */
public char getEchoChar () {
    checkWidget ();
    
    if (variant == TextUtils.LINE_EDIT) {
        int echoMode = OS.QLineEdit_echoMode(topHandle);
        if (echoMode == OS.QLINEEDIT_ECHOMODE_PASSWORD || 
            echoMode == OS.QLINEEDIT_ECHOMODE_PASSWORDECHOONEDIT) {
            return '*';
        }
    } 
    
    return '\0';
}

/**
 * Returns the editable state.
 *
 * @return whether or not the receiver is editable
 * 
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public boolean getEditable () {
    checkWidget();
    return !TextUtils.getReadOnly(variant, topHandle);
}

/**
 * Returns the number of lines.
 *
 * @return the number of lines in the widget
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public int getLineCount () {
    checkWidget();
    if (variant == TextUtils.LINE_EDIT) {
        return 1;
    } else {
        forceTextLayout();
        return OS.QTextEdit_swt_getLineCount(topHandle);
    }
}

/**
 * Returns the line delimiter.
 *
 * @return a string that is the line delimiter
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 * 
 * @see #DELIMITER
 */
public String getLineDelimiter() {
    checkWidget();
    return DELIMITER;
}

/**
 * Returns the height of a line.
 *
 * @return the height of a row of text
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public int getLineHeight () {
    checkWidget();
    if (variant == TextUtils.LINE_EDIT) {
        return OS.QWidget_QFontMetrics_lineSpacing(topHandle);
    } else {
        return OS.QTextEdit_swt_getLineHeight(topHandle);
    }
}

/**
 * Returns the orientation of the receiver, which will be one of the
 * constants <code>SWT.LEFT_TO_RIGHT</code> or <code>SWT.RIGHT_TO_LEFT</code>.
 *
 * @return the orientation style
 * 
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 * 
 * @since 2.1.2
 */
public int getOrientation () {
    checkWidget();
    return style & (SWT.LEFT_TO_RIGHT | SWT.RIGHT_TO_LEFT);
}

/*
 * Gets the preferred size for client area
 */
Point getPreferredClientAreaSize_pp() {
        if (variant == TextUtils.LINE_EDIT) {
            return getPreferredSingleLineClientAreaSize();
        } else {
            return getPreferredClientAreaSize(-1);
        }
}


/*
 * Gets the preferred size for QTextEdit client area
 */
private Point getPreferredClientAreaSize(int wHint) {
        Point size = OS
                .QTextEdit_swt_preferredClientSize(topHandle, wHint);
        if (size == null)
            return new Point(WidgetConstant.DEFAULT_WIDTH, WidgetConstant.DEFAULT_HEIGHT);
        if (size.x < 0)
            size.x = WidgetConstant.DEFAULT_WIDTH;
        if (size.y < 0)
            size.y = WidgetConstant.DEFAULT_HEIGHT;
        return size;
    }

/*
 * Gets the preferred size for QLineEdit client area
 */
private Point getPreferredSingleLineClientAreaSize() {
    Point size = OS.QLineEdit_swt_preferredClientSize(topHandle);
        if (size == null)
            return new Point(WidgetConstant.DEFAULT_WIDTH, WidgetConstant.DEFAULT_HEIGHT);
        if (size.x < 0)
            size.x = WidgetConstant.DEFAULT_WIDTH;
        if (size.y < 0)
            size.y = WidgetConstant.DEFAULT_HEIGHT;
        return size;
}



/**
 * Returns a <code>Point</code> whose x coordinate is the
 * character position representing the start of the selected
 * text, and whose y coordinate is the character position
 * representing the end of the selection. An "empty" selection
 * is indicated by the x and y coordinates having the same value.
 * <p>
 * Indexing is zero based.  The range of a selection is from
 * 0..N where N is the number of characters in the widget.
 * </p>
 *
 * @return a point representing the selection start and end
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public Point getSelection () {
    checkWidget();
    return TextUtils.getSelection(variant, topHandle); 
}

/**
 * Returns the number of selected characters.
 *
 * @return the number of selected characters.
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public int getSelectionCount () {
    checkWidget();
    Point selection = getSelection();
    return Math.abs(selection.y - selection.x);
}

/**
 * Gets the selected text, or an empty string if there is no current selection.
 *
 * @return the selected text
 * 
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public String getSelectionText () {
    checkWidget();
    return TextUtils.getSelectionText(variant, topHandle); 
}

/**
 * Returns the widget text.
 * <p>
 * The text for a text widget is the characters in the widget, or
 * an empty string if this has never been set.
 * </p>
 *
 * @return the widget text
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public String getText () {
    checkWidget();
    return TextUtils.getText(variant, topHandle);
}

/**
 * Returns a range of text.  Returns an empty string if the
 * start of the range is greater than the end.
 * <p>
 * Indexing is zero based.  The range of
 * a selection is from 0..N-1 where N is
 * the number of characters in the widget.
 * </p>
 *
 * @param start the start of the range
 * @param end the end of the range
 * @return the range of text
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public String getText (int start, int end) {
    checkWidget ();
    if (start > end || end < 0) {
        return "";
    }
    
    String text = getText();
    if (text != null) {
        if (start >= text.length()) {
            text = "";
        }
        else {
            start = Math.max(0, start);
            end = Math.min(end, text.length() - 1);
            text = text.substring(start, end + 1);
        }
    }
    return text;
}

/**
 * Returns the maximum number of characters that the receiver is capable of holding. 
 * <p>
 * If this has not been changed by <code>setTextLimit()</code>,
 * it will be the constant <code>Text.LIMIT</code>.
 * </p>
 * 
 * @return the text limit
 * 
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 * 
 * @see #LIMIT
 */
public int getTextLimit () {
    checkWidget(); 
    if (variant == TextUtils.LINE_EDIT) {
        return OS.QLineEdit_maxLength(topHandle);
    } else {
        return textLimit > 0 ? textLimit : LIMIT;
    }
}

/**
 * Returns the zero-relative index of the line which is currently
 * at the top of the receiver.
 * <p>
 * This index can change when lines are scrolled or new lines are added or removed.
 * </p>
 *
 * @return the index of the top line
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public int getTopIndex () {
    checkWidget ();
    if (variant == TextUtils.LINE_EDIT) {
        return 0;
    } else {  
        return OS.QTextEdit_swt_getTopIndex(topHandle);
    }
}

/**
 * Returns the top pixel.
 * <p>
 * The top pixel is the pixel position of the line
 * that is currently at the top of the widget.  On
 * some platforms, a text widget can be scrolled by
 * pixels instead of lines so that a partial line
 * is displayed at the top of the widget.
 * </p><p>
 * The top pixel changes when the widget is scrolled.
 * The top pixel does not include the widget trimming.
 * </p>
 *
 * @return the pixel position of the top line
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public int getTopPixel () {
    checkWidget ();
    if (variant == TextUtils.LINE_EDIT) {
        return 0;
    } else {
        return OS.QScrollBar_value(OS.QAbstractScrollArea_verticalScrollBar(scrollAreaHandle));
    }
}


void hookEvents_pp () {  
    super.hookEvents_pp();
    int textChangedProxy = OS.SignalHandler_new(topHandle, OS.QSIGNAL_TEXT_CHANGED);
    
    if (variant == TextUtils.LINE_EDIT) {
        OS.QObject_connectOrThrow(topHandle, "textChanged(const QString&)",
                textChangedProxy, "widgetSignal(const QString&)", OS.QT_AUTOCONNECTION);

        int returnPressedProxy = OS.SignalHandler_new(topHandle, OS.QSIGNAL_RETURN_PRESSED);
        OS.QObject_connectOrThrow(topHandle, "returnPressed()",
                returnPressedProxy, "widgetSignal()", OS.QT_AUTOCONNECTION);
    } else {
        OS.QObject_connectOrThrow(topHandle, "textChanged()", 
                textChangedProxy, "widgetSignal()", OS.QT_AUTOCONNECTION);
    }
}


/**
 * Inserts a string.
 * <p>
 * The old selection is replaced with the new text.
 * </p>
 *
 * @param string the string
 *
 * @exception IllegalArgumentException <ul>
 *    <li>ERROR_NULL_ARGUMENT - if the string is <code>null</code></li>
 * </ul>
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public void insert (String string) {
    checkWidget ();
    TextUtils.insert(variant, topHandle, string, textLimit, this);
}

/**
 * Pastes text from clipboard.
 * <p>
 * The selected text is deleted from the widget
 * and new text inserted from the clipboard.
 * </p>
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public void paste () {
    TextUtils.paste(variant, topHandle, textLimit, this);
}

/**
 * Removes the listener from the collection of listeners who will
 * be notified when the receiver's text is modified.
 *
 * @param listener the listener which should no longer be notified
 *
 * @exception IllegalArgumentException <ul>
 *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
 * </ul>
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 *
 * @see ModifyListener
 * @see #addModifyListener
 */
public void removeModifyListener (ModifyListener listener) {
    checkWidget ();
    if (listener == null) {
        error(SWT.ERROR_NULL_ARGUMENT);
    }
    if (eventTable == null) {
        return;
    }
    eventTable.unhook(SWT.Modify, listener);    
}

/**
 * Removes the listener from the collection of listeners who will
 * be notified when the control is selected by the user.
 *
 * @param listener the listener which should no longer be notified
 *
 * @exception IllegalArgumentException <ul>
 *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
 * </ul>
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 *
 * @see SelectionListener
 * @see #addSelectionListener
 */
public void removeSelectionListener(SelectionListener listener) {
    checkWidget ();
    if (listener == null) {
        error(SWT.ERROR_NULL_ARGUMENT);
    }
    if (eventTable == null) {
        return;
    }
    eventTable.unhook(SWT.Selection, listener);
    eventTable.unhook(SWT.DefaultSelection,listener);   
}

/**
 * Removes the listener from the collection of listeners who will
 * be notified when the control is verified.
 *
 * @param listener the listener which should no longer be notified
 *
 * @exception IllegalArgumentException <ul>
 *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
 * </ul>
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 *
 * @see VerifyListener
 * @see #addVerifyListener
 */
public void removeVerifyListener (VerifyListener listener) {
    checkWidget ();
    if (listener == null) {
        error(SWT.ERROR_NULL_ARGUMENT);
    }
    if (eventTable == null) {
        return;
    }
    eventTable.unhook(SWT.Verify, listener);    
}

/**
 * Selects all the text in the receiver.
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public void selectAll () {
    checkWidget();
    TextUtils.selectAll(variant, topHandle);
}

/**
 * Sets the double click enabled flag.
 * <p>
 * The double click flag enables or disables the
 * default action of the text widget when the user
 * double clicks.
 * </p><p>
 * Note: This operation is a hint and is not supported on
 * platforms that do not have this concept.
 * </p>
 * 
 * @param doubleClick the new double click flag
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public void setDoubleClickEnabled (boolean doubleClick) {
    checkWidget ();
    this.doubleClick = doubleClick;
}

/**
 * Sets the echo character.
 * <p>
 * The echo character is the character that is
 * displayed when the user enters text or the
 * text is changed by the programmer. Setting
 * the echo character to '\0' clears the echo
 * character and redraws the original text.
 * If for any reason the echo character is invalid,
 * or if the platform does not allow modification
 * of the echo character, the default echo character
 * for the platform is used.
 * </p>
 *
 * @param echo the new echo character
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public void setEchoChar(char echo) {  
    checkWidget();
    if (variant == TextUtils.LINE_EDIT) { 
        int echoMode = (echo == '\0') ? OS.QLINEEDIT_ECHOMODE_NORMAL : 
                                    OS.QLINEEDIT_ECHOMODE_PASSWORD;
        OS.QLineEdit_setEchoMode(topHandle, echoMode);
    }
}

/**
 * Sets the editable state.
 *
 * @param editable the new editable state
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public void setEditable (boolean editable) {
    checkWidget();
    style &= ~SWT.READ_ONLY;
    if (!editable) {
        style |= SWT.READ_ONLY;
    }
    TextUtils.setReadOnly(variant, topHandle, !editable);
} 

/**
 * Sets the orientation of the receiver, which must be one
 * of the constants <code>SWT.LEFT_TO_RIGHT</code> or <code>SWT.RIGHT_TO_LEFT</code>.
 * <p>
 * Note: This operation is a hint and is not supported on
 * platforms that do not have this concept.
 * </p>
 *
 * @param orientation new orientation style
 * 
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 * 
 * @since 2.1.2
 */
public void setOrientation(int orientation) {
    state &= ~WidgetState.FOLLOWS_SYSLANG_DIRECTION;
    setOrientation(topHandle, orientation);
}

/**
 * Sets the selection.
 * <p>
 * Indexing is zero based.  The range of
 * a selection is from 0..N where N is
 * the number of characters in the widget.
 * </p><p>
 * Text selections are specified in terms of
 * caret positions.  In a text widget that
 * contains N characters, there are N+1 caret
 * positions, ranging from 0..N.  This differs
 * from other functions that address character
 * position such as getText () that use the
 * regular array indexing rules.
 * </p>
 *
 * @param start new caret position
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public void setSelection(int start) {
    checkWidget();
    start = Math.min(Math.max(0, start), getCharCount());
    setSelection(start, start);
}

/**
 * Sets the selection to the range specified
 * by the given start and end indices.
 * <p>
 * Indexing is zero based.  The range of
 * a selection is from 0..N where N is
 * the number of characters in the widget.
 * </p><p>
 * Text selections are specified in terms of
 * caret positions.  In a text widget that
 * contains N characters, there are N+1 caret
 * positions, ranging from 0..N.  This differs
 * from other functions that address character
 * position such as getText () that use the
 * usual array indexing rules.
 * </p>
 *
 * @param start the start of the range
 * @param end the end of the range
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public void setSelection (int start, int end) {
    checkWidget();
    TextUtils.setSelection(variant, topHandle, start, end);
}

/**
 * Sets the selection to the range specified
 * by the given point, where the x coordinate
 * represents the start index and the y coordinate
 * represents the end index.
 * <p>
 * Indexing is zero based.  The range of
 * a selection is from 0..N where N is
 * the number of characters in the widget.
 * </p><p>
 * Text selections are specified in terms of
 * caret positions.  In a text widget that
 * contains N characters, there are N+1 caret
 * positions, ranging from 0..N.  This differs
 * from other functions that address character
 * position such as getText () that use the
 * usual array indexing rules.
 * </p>
 *
 * @param selection the point
 *
 * @exception IllegalArgumentException <ul>
 *    <li>ERROR_NULL_ARGUMENT - if the point is null</li>
 * </ul>
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public void setSelection (Point selection) {
    checkWidget ();
    if (selection == null) {
        error(SWT.ERROR_NULL_ARGUMENT);
    }
    setSelection(selection.x, selection.y);
}

/**
 * Sets the contents of the receiver to the given string. If the receiver has style
 * SINGLE and the argument contains multiple lines of text, the result of this
 * operation is undefined and may vary from platform to platform.
 *
 * @param string the new text
 *
 * @exception IllegalArgumentException <ul>
 *    <li>ERROR_NULL_ARGUMENT - if the string is null</li>
 * </ul>
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public void setText (String string) {
    checkWidget();
    TextUtils.setText(variant, topHandle, string, textLimit, this);
    if(isDisposed()) return;
    if (variant == TextUtils.TEXT_EDIT) {
        OS.QScrollBar_setValue(OS.QAbstractScrollArea_verticalScrollBar(scrollAreaHandle), 0);
    }
}

/**
 * Sets the maximum number of characters that the receiver
 * is capable of holding to be the argument.
 * <p>
 * Instead of trying to set the text limit to zero, consider
 * creating a read-only text widget.
 * </p><p>
 * To reset this value to the default, use <code>setTextLimit(Text.LIMIT)</code>.
 * Specifying a limit value larger than <code>Text.LIMIT</code> sets the
 * receiver's limit to <code>Text.LIMIT</code>.
 * </p>
 *
 * @param limit new text limit
 *
 * @exception IllegalArgumentException <ul>
 *    <li>ERROR_CANNOT_BE_ZERO - if the limit is zero</li>
 * </ul>
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 * 
 * @see #LIMIT
 */
public void setTextLimit (int limit) {
    checkWidget ();
    if (limit == 0) {
        error(SWT.ERROR_CANNOT_BE_ZERO);
    }
    
    if (limit < 0 || limit > LIMIT) {
        limit = LIMIT;
    }
    
    if (variant == TextUtils.LINE_EDIT) {
        OS.QLineEdit_setMaxLength(topHandle, limit);
    } else {
        textLimit = limit;
        if (getCharCount() > limit) {
            OS.QTextEdit_setPlainText(topHandle, getText().substring(0, limit));
        }
    }
}

/**
 * Sets the zero-relative index of the line which is currently
 * at the top of the receiver. This index can change when lines
 * are scrolled or new lines are added and removed.
 *
 * @param index the index of the top item
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public void setTopIndex (int index) {
    checkWidget ();
    if (variant == TextUtils.TEXT_EDIT) {
        index = Math.min(Math.max(index, 0), OS.QTextEdit_swt_getLineCount(topHandle) - 1);
        OS.QTextEdit_swt_setTopIndex(topHandle, index);
    }
}

/**
 * Shows the selection.
 * <p>
 * If the selection is already showing
 * in the receiver, this method simply returns.  Otherwise,
 * lines are scrolled until the selection is visible.
 * </p>
 * 
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public void showSelection () {
    checkWidget ();
    if (variant == TextUtils.TEXT_EDIT) {
        OS.QTextEdit_ensureCursorVisible(topHandle);
    }
}

boolean qt_event_keypress_pp(int widgetHandle, int key, int modifier, int character, int nativeScanCode) {
    // In QTextEdit key events normally come with QTextEdit handle (=scrollAreaHandle).
    // KeyPress events may come with viewport widget handle (=handle) when Display.post()
    // is used to generate the event. In this case just let the event through so that it is 
    // passed to QTextEdit in native side.
    if (variant == TextUtils.TEXT_EDIT && widgetHandle != scrollAreaHandle) {
        return false;
    }
    
    if (super.qt_event_keypress_pp(widgetHandle, key, modifier, character, nativeScanCode)) {
        return true;
    }
    
    if ( !(hooks(SWT.Verify) || filters(SWT.Verify)) && textLimit <= 0 ) {
        return false;
    }
    
    Event keyEv = makeKeyEvent(key, modifier, character, nativeScanCode);
    return TextUtils.handle_keypress(variant, widgetHandle,
        key, modifier, character, keyEv, textLimit, this);
}

boolean qt_event_mouseButtonDblClick_pp(int widgetHandle, int button, int x, int y, int state, int buttons) {
    super.qt_event_mouseButtonDblClick_pp(widgetHandle, button, x, y, state, buttons);
    return !doubleClick;
}

void qt_signal_return_pressed_pp() {
    sendEvent(SWT.DefaultSelection);
}

void qt_signal_text_changed_pp(String text) {
    sendEvent(SWT.Modify);
}
  
void setTraversalFlags_pp(int type, int key, int modifier, int character) {    
    traverseDoit = false;
    traverseCancel = false;
    
    // If the system is about to traverse out of the Text because mnemonic
    // shortcut of some widget was pressed then it is allowed.
    // Note that if keys used to launch shortcuts overlap the keys that are used
    // by Text then shortcut has priority over text with this implementation. 
    // However, platforms usually define the shortucts in such a way that they
    // don't overlap. E.g. in X11 Alt+something. 
    if (type == SWT.TRAVERSE_MNEMONIC) {
        traverseDoit = true;
        return;
    }
    
    switch(key) {
        case OS.QT_KEY_ENTER:
            // fall through
        case OS.QT_KEY_RETURN:
            traverseDoit = false;
            // Cancel event in TextExtension case
            if ((getStyle() & SWT.SINGLE) != 0 && variant == TextUtils.TEXT_EDIT) {
                traverseCancel = true;
            }
            break;
        case OS.QT_KEY_TAB:
            traverseDoit = (getStyle() & SWT.SINGLE) != 0 ; // intentionally not using variant()
            break;
        case OS.QT_KEY_UP:
            if ( (modifier & OS.QT_SHIFTMODIFIER) == 0 &&
                 (getCaretLineNumber() == 0) ) {
                traverseDoit = true;
            }
            break;
        case OS.QT_KEY_DOWN:
            if ( (modifier & OS.QT_SHIFTMODIFIER) == 0 &&
                 (getCaretLineNumber() == getLineCount() - 1) ) {
                traverseDoit = true;
            }
            break;
        case OS.QT_KEY_LEFT:
        case OS.QT_KEY_RIGHT:
            traverseDoit = false;
            break;
        default:
            super.setTraversalFlags_pp(type, key, modifier, character);
            break;
    }
}

void setBackground_pp () {
    // MULTI uses QLineEdit and might have modified viewport background brush
    // that needs to be restored to default.
    if((style & SWT.MULTI) != 0) {
        OS.QWidget_swt_unsetPalette(handle);
    }
    super.setBackground_pp();
}

void backgroundImageApplied_pp(Image image) {
    // There's a problem with scrolling QTextEdit when non-single-color
    // backgrounds are used. Background can't be scrolled but needs to be
    // redrawn. To fix this we detect when background image is taken into use
    // with QTextEdit and temporarily set null background brush to viewport.
    // Viewport will inherit the background from its parent and whole area is
    // updated correctly.
    
    // Only MULTI uses QLineEdit
    if((style & SWT.MULTI) == 0) return;
    
    int textPalette = 0;
    try {
        textPalette = OS.QWidget_swt_palette_new(handle);
        int[] bkRoles = getBackgroundImageRoles();
        for(int i = 0; i < bkRoles.length; ++i) {
            OS.QPalette_swt_setBrush(textPalette, bkRoles[i], 0);
        }
        OS.QWidget_setPalette(handle, textPalette);
    } finally {
        OS.QPalette_delete(textPalette);
    }
}
}
