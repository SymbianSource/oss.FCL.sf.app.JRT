/*******************************************************************************
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *      Nokia Corporation - Qt implementation
 *******************************************************************************/
package org.eclipse.swt.widgets;

import org.eclipse.swt.SWT;
import org.eclipse.swt.SWTException;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.internal.qt.OS;

/**
 * Instances of this class are used to inform or warn the user.
 * <dl>
 * <dt><b>Styles:</b></dt>
 * <dd>ICON_ERROR, ICON_INFORMATION, ICON_QUESTION, ICON_WARNING, ICON_WORKING</dd>
 * <dd>OK, OK | CANCEL</dd>
 * <dd>YES | NO, YES | NO | CANCEL</dd>
 * <dd>RETRY | CANCEL</dd>
 * <dd>ABORT | RETRY | IGNORE</dd>
 * <dt><b>Events:</b></dt>
 * <dd>(none)</dd>
 * </dl>
 * <p>
 * Note: Only one of the styles ICON_ERROR, ICON_INFORMATION, ICON_QUESTION,
 * ICON_WARNING and ICON_WORKING may be specified.
 * </p><p>
 * IMPORTANT: This class is intended to be subclassed <em>only</em>
 * within the SWT implementation.
 * </p>
 */
public class MessageBox extends Dialog {

    private String message = "";
    private String dialogID;
    static int dialogCount;
    
    /**
     * Constructs a new instance of this class given only its parent.
     *
     * @param parent a shell which will be the parent of the new instance
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the parent</li>
     *    <li>ERROR_INVALID_SUBCLASS - if this class is not an allowed subclass</li>
     * </ul>
     */
    public MessageBox(Shell parent) {
        this(parent, SWT.OK | SWT.ICON_INFORMATION | SWT.APPLICATION_MODAL);
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
     *
     * @param parent a shell which will be the parent of the new instance
     * @param style the style of dialog to construct
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the parent</li>
     *    <li>ERROR_INVALID_SUBCLASS - if this class is not an allowed subclass</li>
     * </ul>
     */
    public MessageBox(Shell parent, int style) {
        super(parent, parent == null ? checkStyle(style) : checkStyle(parent, style));
        checkSubclass();
        checkDialog();
        dialogCount++;
        dialogID = toString() + String.valueOf(dialogCount);
    }

    /**
     * Returns the dialog's message, or an empty string if it does not have one.
     * The message is a description of the purpose for which the dialog was opened.
     * This message will be visible in the dialog while it is open.
     *
     * @return the message
     */
    public String getMessage() {
        return message;
    }

    /**
     * Sets the dialog's message, which is a description of
     * the purpose for which it was opened. This message will be
     * visible on the dialog while it is open.
     *
     * @param string the message
     * 
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the string is null</li>
     * </ul>
     */
    public void setMessage(String string) {
        if (string == null) error(SWT.ERROR_NULL_ARGUMENT);
        message = string;
    }

    /**
     * Makes the dialog visible and brings it to the front
     * of the display.
     *
     * @return the ID of the button that was selected to dismiss the
     *         message box (e.g. SWT.OK, SWT.CANCEL, etc.)
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the dialog has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the dialog</li>
     * </ul>
     */
    public int open() {
        checkDialog();
        
        DisposeListener listener = new DisposeListener() {
            public void widgetDisposed(DisposeEvent e) {
                if (e.widget == parent) {
                    // close dialogs which are opened on top of parent
                    OS.QDialog_swt_closeDialogs(parent.handle, dialogID);
                }

            }
        };

        parent.addDisposeListener(listener);
        
        int icon = OS.QMESSAGEBOX_NOICON;
        if ((style & (SWT.ICON_INFORMATION)) != 0)  icon = OS.QMESSAGEBOX_INFORMATION;
        if ((style & (SWT.ICON_WARNING)) != 0)  icon = OS.QMESSAGEBOX_WARNING;
        if ((style & (SWT.ICON_QUESTION)) != 0) icon = OS.QMESSAGEBOX_QUESTION;
        if ((style & (SWT.ICON_ERROR)) != 0)    icon = OS.QMESSAGEBOX_CRITICAL;
        
        int buttons = 0;
        if ((style & SWT.OK) != 0) buttons |= OS.QMESSAGEBOX_OK;
        if ((style & SWT.CANCEL) != 0) buttons |= OS.QMESSAGEBOX_CANCEL;
        if ((style & SWT.YES) != 0) buttons |= OS.QMESSAGEBOX_YES;
        if ((style & SWT.NO) != 0) buttons |= OS.QMESSAGEBOX_NO;
        if ((style & SWT.ABORT) != 0) buttons |= OS.QMESSAGEBOX_ABORT;
        if ((style & SWT.RETRY) != 0) buttons |= OS.QMESSAGEBOX_RETRY; 
        if ((style & SWT.IGNORE) != 0) buttons |= OS.QMESSAGEBOX_IGNORE;
        
        int modality = OS.QT_WINDOWMODALITY_WINDOWMODAL; 
        if ((style & (SWT.APPLICATION_MODAL | SWT.SYSTEM_MODAL)) != 0) {
            modality = OS.QT_WINDOWMODALITY_APPLICATIONMODAL;
        }
        
        int qtLayoutDirection = (style & SWT.RIGHT_TO_LEFT) != 0 ? OS.QT_RIGHTTOLEFT : OS.QT_LEFTTORIGHT;
        
        int ret = OS.QMessageBox_swt_exec(icon, title, message, buttons, 
                parent.handle, modality, dialogID, qtLayoutDirection);
        
        if (parent != null && !parent.isDisposed()) {
            parent.removeDisposeListener(listener);
        }
        
        return translateToSwtButton(ret);
    }

    private int translateToSwtButton(int nativeButton) {
        int button = 0;
        
        switch (nativeButton) {
        case OS.QMESSAGEBOX_OK: 
            button = SWT.OK;
            break;
        case OS.QMESSAGEBOX_CANCEL:
            button = SWT.CANCEL;
            break;
        case OS.QMESSAGEBOX_YES:
            button = SWT.YES;
            break;
        case OS.QMESSAGEBOX_NO:
            button = SWT.NO;
            break;
        case OS.QMESSAGEBOX_ABORT:
            button = SWT.ABORT;
            break;
        case OS.QMESSAGEBOX_RETRY:
            button = SWT.RETRY;
            break;
        case OS.QMESSAGEBOX_IGNORE:
            button = SWT.IGNORE;
            break;
        default:
            break;
        }
        
        return button;
    }
    
    void checkDialog() {
        if (parent == null) error(SWT.ERROR_WIDGET_DISPOSED);
        if (parent.isDisposed()) error(SWT.ERROR_WIDGET_DISPOSED);
        Display display = parent.display;
        if (display == null) error(SWT.ERROR_WIDGET_DISPOSED);
        if (display.thread != Thread.currentThread()) error(SWT.ERROR_THREAD_INVALID_ACCESS);
    }
    
    static private int checkStyle(int style) {
        int mask = (SWT.YES | SWT.NO | SWT.OK | SWT.CANCEL | SWT.ABORT | SWT.RETRY | SWT.IGNORE);
        int bits = style & mask;
        if (bits == SWT.OK || bits == SWT.CANCEL || bits == (SWT.OK | SWT.CANCEL)) return style;
        if (bits == SWT.YES || bits == SWT.NO || bits == (SWT.YES | SWT.NO) || bits == (SWT.YES | SWT.NO | SWT.CANCEL)) return style;
        if (bits == (SWT.RETRY | SWT.CANCEL) || bits == (SWT.ABORT | SWT.RETRY | SWT.IGNORE)) return style;
        style = (style & ~mask) | SWT.OK;
        return style;
    }

    static int checkStyle(Shell parent, int style) {
        return checkStyle(Dialog.checkStyle(parent, style));
    }
}
