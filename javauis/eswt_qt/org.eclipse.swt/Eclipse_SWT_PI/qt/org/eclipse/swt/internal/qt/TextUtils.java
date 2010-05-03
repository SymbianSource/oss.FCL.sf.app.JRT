/*******************************************************************************
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * 
 * Contributors:
 *     Nokia Corporation - initial implementation 
 *******************************************************************************/
package org.eclipse.swt.internal.qt;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Widget;

public class TextUtils extends Object {
    public static final int LINE_EDIT = 0;
    public static final int TEXT_EDIT = 1;
    
/**
 * @see Text.append
 * @param limit - if char count bigger than limit, text gets truncated
 * @param verifier - owner of verify listeners, can be null
 */
public static void append (int variant, int handle, String string, int limit, Widget verifier) {
    if (string == null) {
        SWT.error(SWT.ERROR_NULL_ARGUMENT);
    }

    int count = getCharCount(variant, handle);
    string = verify(variant, handle, string, verifier, limit, count, count);
    if (string == null) {
        return;   
    }

    if (variant == LINE_EDIT) {
        OS.QLineEdit_end(handle, false);
        OS.QLineEdit_insert(handle, string);
    }
    else {
        OS.QTextEdit_swt_append(handle, string);
    }
}

/**
 * @see Text.append
 */
public static void clearSelection (int variant, int handle) {
    if (variant == LINE_EDIT) {
        OS.QLineEdit_setCursorPosition(handle, OS.QLineEdit_cursorPosition(handle));
    } else {
        OS.QTextEdit_swt_clearSelection(handle);
    }
}

/**
 * @see Text.copy
 */
public static void copy (int variant, int handle) {
    if (variant == LINE_EDIT) {
        OS.QLineEdit_copy(handle);
    } else {
        OS.QTextEdit_copy(handle);
    }
}

/**
 * @see Text.cut
 * @param verifier - owner of verify listeners, can be null
 */
public static void cut (int variant, int handle, Widget verifier) {
    Point selection = getSelection(variant, handle);
    String string = verify(variant, handle, "", verifier, -1, 
        Math.min(selection.x, selection.y), Math.max(selection.x, selection.y));
    if (string == null) {
        return;
    }

    if (variant == LINE_EDIT) {
        OS.QLineEdit_cut(handle);
    } else {
        OS.QTextEdit_cut(handle);
    }

    if (!string.equals("")) {
        if (variant == LINE_EDIT) {
            OS.QLineEdit_insert(handle, string);
        } else {
            OS.QTextEdit_insertPlainText(handle, string);
        }
    }
}

/**
 * @see Text.getEditable
 */
public static boolean getReadOnly (int variant, int handle) {
    if (variant == TextUtils.LINE_EDIT) {
        return OS.QLineEdit_isReadOnly(handle);
    } else {
        return OS.QTextEdit_isReadOnly(handle);
    }
}

/**
 * @see Text.getSelection
 */
public static Point getSelection (int variant, int handle) {
    if (variant == LINE_EDIT) {
        int firstSelected = OS.QLineEdit_selectionStart(handle);
        if (firstSelected == -1) {
            int cursorPos = OS.QLineEdit_cursorPosition(handle);
            return new Point(cursorPos, cursorPos);
        }
        return new Point(firstSelected, firstSelected
                + OS.QLineEdit_selectedText(handle).length());

    } else {
        int start = OS.QTextEdit_swt_selectionStart(handle);
        int end = OS.QTextEdit_swt_selectionEnd(handle);
        return new Point(Math.min(start, end), Math.max(start, end));
    }
}

/**
 * @see Text.getSelectionText
 */
public static String getSelectionText (int variant, int handle) {
    if (variant == LINE_EDIT) {
        return OS.QLineEdit_selectedText(handle);
    } else {
        return OS.QTextEdit_swt_getSelectionText(handle);
    }
}

/**
 * @see Text.getCaretPosition
 */
public static int getCaretPosition (int variant, int handle) {
    if (variant == LINE_EDIT) {
        return OS.QLineEdit_cursorPosition(handle);
    } else {
        return OS.QTextEdit_swt_getCaretPosition(handle);
    }
}

/**
 * @see Text.getCharCount
 */
public static int getCharCount (int variant, int handle) {
    if (variant == LINE_EDIT) {
        return OS.QLineEdit_text(handle).length();
    } else {
        return OS.QTextEdit_swt_getCharCount(handle);
    }
}

/**
 * @see Text.getText
 */
public static String getText (int variant, int handle) {
    if (variant == LINE_EDIT) {
        return OS.QLineEdit_text(handle);
    } else {
        return OS.QTextEdit_toPlainText(handle);
    }
}

/**
 * @see Text.insert
 * @param limit - if char count bigger than limit, text gets truncated
 * @param verifier - owner of verify listeners, can be null
 */
public static void insert (int variant, int handle, String string, int limit, Widget verifier) {
    if (string == null) { 
        SWT.error (SWT.ERROR_NULL_ARGUMENT);
    }
    
    Point selection = getSelection(variant, handle);
    string = verify(variant, handle, string, verifier, limit, 
        Math.min(selection.x, selection.y), Math.max(selection.x, selection.y));
    if (string == null) {
        return;
    }
    
    if (variant == LINE_EDIT) {
        OS.QLineEdit_insert(handle, string);
    } else {
        OS.QTextEdit_insertPlainText(handle, string);
    }
}

/**
 * @see Text.insert
 * @param limit - if char count bigger than limit, text gets truncated
 * @param verifier - owner of verify listeners, can be null
 */
public static void paste (int variant, int handle, int limit, Widget verifier) {
    Point selection = getSelection(variant, handle);
    String string = verify(variant, handle, OS.QClipboard_text(), verifier, limit, 
        Math.min(selection.x, selection.y), Math.max(selection.x, selection.y));
    if (string == null) {
        return;
    }

    if (variant == LINE_EDIT) {
        OS.QLineEdit_insert(handle, string);
    } else {
        OS.QTextEdit_insertPlainText(handle, string);
    }
}

/**
 * @see Text.selectAll
 */
public static void selectAll (int variant, int handle) {
    if (variant == TextUtils.LINE_EDIT) {
        OS.QLineEdit_selectAll(handle);
    } else {
        OS.QTextEdit_swt_selectAll(handle);
    }
}

/**
 * @see Text.setEditable
 */
public static void setReadOnly (int variant, int handle, boolean readOnly) {
    if (variant == TextUtils.LINE_EDIT) {
        OS.QLineEdit_setReadOnly(handle, readOnly);
    } else {
        OS.QTextEdit_setReadOnly(handle, readOnly);
    }
}

/**
 * @see Text.setSelection
 */
public static void setSelection (int variant, int handle, int start, int end) {
    int length = getCharCount(variant, handle);
    start = Math.min(Math.max(0, start), length);
    end = Math.min(Math.max(0, end), length);

    if (variant == LINE_EDIT) {
        if(start == end) {
            OS.QLineEdit_deselect(handle);
            OS.QLineEdit_setCursorPosition(handle, start);
        } else {
            OS.QLineEdit_setSelection(handle, start, (end - start));
        }
    } else {
        OS.QTextEdit_swt_setCursorPosition(handle, start, true);
        OS.QTextEdit_swt_setCursorPosition(handle, end, false);
    }
}

/**
 * @see Text.setText
 * @param limit - if char count bigger than limit, text gets truncated
 * @param verifier - owner of verify listeners, can be null
 */
public static void setText (int variant, int handle, String string, int limit, Widget verifier) {
    if (string == null) {
        SWT.error(SWT.ERROR_NULL_ARGUMENT);
    }
    
    string = verify(variant, handle, string, verifier, limit, 0, getCharCount(variant, handle));
    if (string == null) {
        return;
    }
    
    if (variant == LINE_EDIT) {
        OS.QLineEdit_setText(handle, string);
        OS.QLineEdit_setCursorPosition(handle, 0);
    } else {
        OS.QTextEdit_setPlainText(handle, string);
    }
}

/**
 * @see Text.qt_event_keypress
 */
public static boolean handle_keypress(int variant, int handle, int key, 
    int modifier, int character, Event event, int textLimit, Widget verifier) {
    
    String origText = "" + (char) character;
    Point selection = getSelection(variant, handle);
    int start = selection.x;
    int end = selection.y;

    // keyboard shortcuts for paste
    if ((modifier == OS.QT_SHIFTMODIFIER && key == OS.QT_KEY_INSERT)
        || (modifier == OS.QT_CONTROLMODIFIER && key == OS.QT_KEY_V)) {

        origText = OS.QClipboard_text();
        if (origText == null || origText.equals("")) {
            return false;
        }
        event = null;

        // keyboard shortcuts for cut
    } else if ((modifier == OS.QT_SHIFTMODIFIER && key == OS.QT_KEY_DELETE)
        || (modifier == OS.QT_CONTROLMODIFIER && key == OS.QT_KEY_X)) {

        if (end == start) {
            return false;
        }
        event = null;
        origText = "";

        // Cancel ctrl + Del, ctrl + Backspace, alt + Bs and ctrl+k because these key bindings
        // behave differently in QLineEdit and in QTextEdit and making verifyEvents
        // to work fully reliably in all platforms is impossible.
    } else if (((modifier & OS.QT_CONTROLMODIFIER) != 0 && key == OS.QT_KEY_DELETE)
        || ((modifier & OS.QT_CONTROLMODIFIER) != 0 && key == OS.QT_KEY_BACKSPACE)
        || ((modifier & OS.QT_ALTMODIFIER) != 0 && key == OS.QT_KEY_BACKSPACE)
        || (modifier == OS.QT_CONTROLMODIFIER && key == OS.QT_KEY_K)) {
        return true;
        // Non-printable keys, return, backspace and delete
    } else if ((key & OS.QT_NONPRINTABLEKEYMASK) != 0) {

        switch (key) {
        case OS.QT_KEY_RETURN:
        case OS.QT_KEY_ENTER:
            if (variant == LINE_EDIT) {
                return false;
            }
            break;
        case OS.QT_KEY_BACKSPACE:
            if (end == start) {
                if (start == 0) {
                    return false;
                }
                --start;
            }
            origText = "";
            break;
        case OS.QT_KEY_DELETE:
            if (modifier != OS.QT_NOMODIFIER) {
                return false;
            }
            if (end == start) {
                if (end == getCharCount(variant, handle)) {
                    return false;
                }
                ++end;
            }
            origText = "";
            break;
        default:
            // No need to verify other non-printable characters
            return false;
        }

        // Don't verify key pressed that don't change the text. If
        // chararcter is smaller than the first printable ASCII char 
        // (= space), the text content is not modified.
        // For some unknown reason when ctrl+8 is pressed, character is
        // set Del. Ctrl+8 does not modify the text content anyway.
    } else if ((modifier & OS.QT_CONTROLMODIFIER) != 0 && ((char) character < ' ' || (char) character == SWT.DEL)) {
        return false;
    }

    String verifiedText = verify(variant, handle, origText, verifier, textLimit, start, end, event);

    if (verifiedText == null) {
        // Key event is not forwarded to native widget
        return true;
    } else if (verifiedText.equals(origText)) {
        return false;
    } else {
        setSelection(variant, handle, start, end);

        if (variant == LINE_EDIT) {
            OS.QLineEdit_insert(handle, verifiedText);
        } else {
            OS.QTextEdit_insertPlainText(handle, verifiedText);
        }

        // Key event is not forwarded to native widget
        return true;
    }
}

/**
 * Verify, limit, range
 */
public static String verify (int variant, int handle, String string, 
    Widget verifier, int limit, int start, int end) {
    return verify(variant, handle, string, verifier, limit, start, end, null);
}

/**
 * Verify, limit, range, key event
 */
public static String verify (int variant, int handle, String string, 
    Widget verifier, int limit, int start, int end, Event keyEvent) {
    if (string.length() == 0 && start == end) {
        return null;
    }
    
    if ((verifier != null) && ((verifier.getStyle() & SWT.SINGLE) != 0)) {
        string = string.replace('\n', '\u0020');//Replace with space
    }

    if (verifier != null) {
        Event event = new Event();
        event.text = string;
        event.start = start;
        event.end = end;

        if (keyEvent != null) {
            event.keyCode = keyEvent.keyCode;
            event.character = keyEvent.character;
            event.stateMask = keyEvent.stateMask;
        } else {
            event.keyCode = 0;
            event.character = 0;
            event.stateMask = 0;
        }
        
        verifier.notifyListeners(SWT.Verify, event);
        if (!event.doit || verifier.isDisposed()) {
            return null;
        }
        string = event.text;
    }
    
    if (limit > 0 && string.length() > 0) {
        int count = 0;
        if (variant == LINE_EDIT) {
            count = OS.QLineEdit_text(handle).length();
        }
        else {
            count = OS.QTextEdit_swt_getCharCount(handle); 
        }
        int spaceLeft = limit - count + (end - start);
        int endIdx = Math.min(Math.max(0, spaceLeft), string.length());
        string = string.substring(0, endIdx);
    }
    
    if (string == null || (string.length() == 0 && start == end)) {
        return null;
    }

    return string;
}

}
