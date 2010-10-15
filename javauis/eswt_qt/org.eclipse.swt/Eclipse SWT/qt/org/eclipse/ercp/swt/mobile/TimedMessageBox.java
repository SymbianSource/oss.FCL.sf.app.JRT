/*******************************************************************************
 * Copyright (c) 2004, 2010 Nokia Corporation and/or its subsidiary(-ies).
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

import org.eclipse.swt.SWT;
import org.eclipse.swt.SWTException;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Internal_GfxPackageSupport;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.widgets.Dialog;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Internal_PackageSupport;
import org.eclipse.swt.widgets.Shell;

/**
 * A modal window used to shortly inform the user of limited information using a
 * standard style.
 *
 * <p>
 * A TimedMessageBox is capable of closing itself automatically after a certain
 * period of time. No need to define button styles in MessageBox.
 * </p>
 *
 * <p>
 * Unlike TaskTip, the TimedMessageBox is a modal dialog and the execution will
 * be blocked until the dialog is closed after a short period of time. The exact
 * time-out duration is implementation dependent. Applications can not change
 * the period of time.
 * </p>
 * <dl>
 * <dt><b>Styles: </b></dt>
 * <dd>ICON_WORKING: informs that an action was successful</dd>
 * <dd>ICON_INFORMATION: informs of a situation that may not require user
 * action</dd>
 * <dd>ICON_WARNING: informs of a situation that may require user intervention
 * </dd>
 * <dd>ICON_ERROR: informs that a serious situation occurred</dd>
 * </dd>
 * <dt><b>Events: </b></dt>
 * <dd>(none)</dd>
 * </dl>
 * <p>
 * <p>
 * IMPORTANT: This class is <em>not</em> intended to be subclassed.
 * </p>
 */
public class TimedMessageBox extends Dialog {

    String message;
    Image image;
    String dialogID;
    static int dialogCount;

    /**
     * Constructs a new instance of this class given only its parent.
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
     *                </ul>
     */
    public TimedMessageBox(Shell parent) {
        this(parent, SWT.ICON_INFORMATION | SWT.APPLICATION_MODAL);
    }

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
     * @param parent
     *            a shell which will be the parent of the new instance
     * @param style
     *            the style of control to construct
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
     */
    public TimedMessageBox(Shell parent, int style) {
        super(parent, checkStyle(parent, style));
        checkSubclass();
        message = null;
        image = null;
        dialogCount++;
        dialogID = toString() + String.valueOf(dialogCount);
    }

    static protected int checkStyle(Shell parent, int style) {
        style = Internal_PackageSupport.checkStyle(parent, style);

        if ((style & (SWT.PRIMARY_MODAL | SWT.APPLICATION_MODAL )) == 0) {
            style |= SWT.APPLICATION_MODAL;
        }

        if ((style & (SWT.ICON_ERROR | SWT.ICON_INFORMATION |
                    SWT.ICON_WARNING)) == 0) {
            style |= SWT.ICON_INFORMATION;
        }
        return style;
    }

    /**
     * Returns the icon object.
     *
     * @return Image the icon reference. May by null if the implementation does
     *         not support custom icons, or has no way to get the image object.
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public Image getImage() {
        checkDialog();
        return image;
    }

    /**
     * Returns the dialog's message, which is a description of the purpose for
     * which it was opened. This message will be visible on the dialog while it
     * is open.
     *
     * @return the message
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public String getMessage() {
        checkDialog();
        return message;
    }

    /**
     * Makes the dialog visible and brings it to the front of the display.
     *
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the dialog</li>
     *                </ul>
     */

    public void open() {
        checkDialog();

        DisposeListener listener = new DisposeListener() {
            public void widgetDisposed(DisposeEvent e) {
                if (e.widget == getParent()) {
                    // close message box if parent is disposed
                    int parentHandle = Internal_PackageSupport.handle(getParent());
                    OS.QDialog_swt_closeDialogs(parentHandle, dialogID);
                }
            }
        };
        getParent().addDisposeListener(listener);

        int style = Internal_PackageSupport.style(this);
        int modality = OS.QT_WINDOWMODALITY_WINDOWMODAL;
        if ((style & SWT.APPLICATION_MODAL) != 0) {
            modality = OS.QT_WINDOWMODALITY_APPLICATIONMODAL;
        }
        int layoutDirection = (style & SWT.RIGHT_TO_LEFT) != 0 ? OS.QT_RIGHTTOLEFT
                : OS.QT_LEFTTORIGHT;

        int cgImageHandle = (image == null) ? 0 : Internal_GfxPackageSupport.getImageHandle(image);

        int icon = OS.QMESSAGEBOX_NOICON;
        if (image == null) {
            if ((style & (SWT.ICON_INFORMATION)) != 0)  icon = OS.QMESSAGEBOX_INFORMATION;
            if ((style & (SWT.ICON_WARNING)) != 0)  icon = OS.QMESSAGEBOX_WARNING;
            if ((style & (SWT.ICON_ERROR)) != 0)    icon = OS.QMESSAGEBOX_CRITICAL;
        }

        int parentHandle = Internal_PackageSupport.handle(getParent());
        OS.QMessageBox_swt_execTimer(icon, getText(), message, parentHandle,
                dialogID, layoutDirection, modality, cgImageHandle);

        if (getParent() != null && !getParent().isDisposed()) {
            getParent().removeDisposeListener(listener);
        }
    }

    /**
     * Sets the receiver's icon to the argument.
     *
     * The argument cannot be null.
     *
     * @param image
     *            the image to display on the receiver.
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the image is null</li>
     *                <li>ERROR_INVALID_ARGUMENT - if the image has been
     *                disposed</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void setImage(Image image) {
        checkDialog();
        if (image == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        if (image.isDisposed()) SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        this.image = image;
    }

    /**
     * Sets the dialog's message, which is a description of the purpose for
     * which it was opened. This message will be visible on the dialog while it
     * is open.
     *
     * @param string
     *            the message
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the string is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void setMessage(String string) {
        checkDialog();
        if (string == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        message = string;
    }

    /**
     * Throws an <code>SWTException</code> if the receiver can not
     * be accessed by the caller. This may include both checks on
     * the state of the receiver and more generally on the entire
     * execution context. This method <em>should</em> be called by
     * widget implementors to enforce the standard SWT invariants.
     * <p>
     * Currently, it is an error to invoke any method (other than
     * <code>isDisposed()</code>) on a widget that has had its
     * <code>dispose()</code> method called. It is also an error
     * to call widget methods from any thread that is different
     * from the thread that created the widget.
     * </p><p>
     * In future releases of SWT, there may be more or fewer error
     * checks and exceptions may be thrown for different reasons.
     * </p>
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    private void checkDialog () {
        // Following call throws error, if parent is disposed or
        // parent's display is null.
        Display currentDisplay = getParent().getDisplay();
        if (currentDisplay.getThread() != Thread.currentThread ())
            SWT.error (SWT.ERROR_THREAD_INVALID_ACCESS);
    }

}
