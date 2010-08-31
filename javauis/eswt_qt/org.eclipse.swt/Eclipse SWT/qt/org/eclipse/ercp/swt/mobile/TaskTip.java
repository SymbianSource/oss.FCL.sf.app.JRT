/*******************************************************************************
 * Copyright (c) 2000, 2005 IBM Corporation and others.
 * Portion Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *     Nokia Corporation - S60 implementation
 *     Nokia Corporation - QT implementation
 *******************************************************************************/
package org.eclipse.ercp.swt.mobile;

import org.eclipse.swt.SWT;
import org.eclipse.swt.SWTException;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.internal.qt.PackageProxy;
import org.eclipse.swt.internal.qt.WidgetState;
import org.eclipse.swt.widgets.Internal_PackageSupport;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Widget;

/**
 * 
 * This class provides feedback to the user about the state of a long-running
 * task.
 * 
 * <p>
 * A TaskTip may contain text and an optional progress bar or another object to
 * illustrate current task state. Like a MessageBox, the look and feel of
 * TaskTip is platform-dependent, which means that there is no API level access
 * to the UI components inside a TaskTip. Unlike a MessageBox, the TaskTip is a
 * non-focusable window and will not change the current window's activation or
 * focus.
 * </p>
 * <p>
 * When constructed without a style constant, a TaskTip displays plain text to 
 * indicate task progress.
 * </p>
 * <p>
 * A TaskTip becomes visible by calling <code>setVisible(true)</code> and
 * remains visible until the application calls
 * <code>setVisible(false)</code>. When a new TaskTip is created before
 * hiding or disposing of a prior TaskTip, the newest becomes the top-most one 
 * and obscures the prior ones, if any.
 * </p>
 * <p>
 * A TaskTip can not be positioned programmatically or by the user. The position
 * is implementation-dependent.
 * </p>
 * <dl>
 * <dt><b>Styles: </b></dt>
 * <dd>SMOOTH: displays a visual indicator of what portion of the progress is
 * left to go</dd>
 * <dd>INDETERMINATE: displays a visual indicator that a long-running process
 * is progressing</dd>
 * <dt><b>Events: </b></dt>
 * <dd>(none)</dd>
 * </dl>
 * 
 * <p>
 * Note: The methods <code>setMaximum</code>, <code>setMinimum</code>,
 * and <code>setSelection</code>, are silently ignored if the style is not
 * SWT.SMOOTH.
 * 
 * @see SWT#SMOOTH
 * @see SWT#INDETERMINATE
  * <p>
 * IMPORTANT: This class is <em>not</em> intended to be subclassed.
 * </p>
 */
public class TaskTip extends Widget {
    private final static int PROGRESSBAR_DEFAULT_MINIMUM = 0;
    private final static int PROGRESSBAR_DEFAULT_MAXIMUM = 100;

    Shell parent;

    static final class TaskTipPackageProxy extends PackageProxy {
        public void createHandle(int index) {
            ((TaskTip)w).createHandle(index);
        }
        public void qt_swt_event_widgetResized(int widgetHandle, int oldWidth, int oldHeight, int width, int height) {
            ((TaskTip)w).qt_swt_event_widgetResized(widgetHandle, oldWidth, oldHeight, width, height);
        }
        public void releaseHandle() {
            ((TaskTip)w).releaseHandle();
        }
    }
    
    /**
     * 
     * Constructs a TaskTip with plain text style.
     * 
     * The default style displays plain text to indicate progress. The TaskTip 
     * is disposed if the parent Shell is disposed.
     * 
     * @param shell
     *            The Shell the TaskTip is associated with
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the shell value is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the display</li>
     *                <li>ERROR_INVALID_SUBCLASS - if this class is not an
     *                allowed subclass</li>
     *                </ul>
     */
    public TaskTip(Shell shell) {
        this(shell, SWT.NONE);
    }

    /**
     * 
     * Constructs a TaskTip with a given style value. The TaskTip 
     * is disposed if the parent Shell is disposed.
     * 
     * @param shell
     *            The Shell the TaskTip is associated with
     * @param style
     *            style of TaskTip: NONE, SMOOTH or INDETERMINATE.
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the shell value is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the display</li>
     *                <li>ERROR_INVALID_SUBCLASS - if this class is not an
     *                allowed subclass</li>
     *                </ul>
     * @see SWT#SMOOTH
     * @see SWT#INDETERMINATE
     */
    public TaskTip(Shell shell, int style) {
        super(shell, style, 0, new TaskTipPackageProxy(), false);
        
        parent = shell;       
        parent.addDisposeListener(new DisposeListener() {
            public void widgetDisposed(DisposeEvent e) {
                dispose();
        }});
        
        Internal_PackageSupport.checkOrientation(this, parent);
        createHandle(0);
        Internal_PackageSupport.register(this);
        
        if ((style & SWT.RIGHT_TO_LEFT) != 0) {
            OS.QWidget_setLayoutDirection(topHandle(), OS.QT_RIGHTTOLEFT);
        } 
        
        updatePosition();
    }

    void createHandle (int index) {
        Internal_PackageSupport.createHandle(this, index);
        boolean showBar = false;
        
        if ((style() & SWT.SMOOTH) != 0 || (style() & SWT.INDETERMINATE) != 0) {
            showBar = true;
        }
        
        int parentHandle = Internal_PackageSupport.handle(parent);
        int handle = OS.QProgressDialog_swt_new(parentHandle, showBar);
        Internal_PackageSupport.setHandle(this, handle);
        Internal_PackageSupport.setTopHandle(this, handle);
        Internal_PackageSupport.setState(this, Internal_PackageSupport.state(this) | WidgetState.HANDLE);
        
        OS.QProgressDialog_setCancelButton(handle, 0);
        OS.QProgressDialog_setAutoClose(handle, false);
        OS.QProgressDialog_setAutoReset(handle, false);
        
        if ((style() & SWT.SMOOTH) != 0) {
            OS.QProgressDialog_setMinimum(handle, PROGRESSBAR_DEFAULT_MINIMUM);
            OS.QProgressDialog_setMaximum(handle, PROGRESSBAR_DEFAULT_MAXIMUM);
            OS.QProgressDialog_setValue(handle, PROGRESSBAR_DEFAULT_MINIMUM);            
        } else if ((style() & SWT.INDETERMINATE) != 0) {
            // Setting QProgressDialog's minimum = maximum causes the 
            // progress bar to go to indeterminate state
            OS.QProgressDialog_setMinimum(handle, PROGRESSBAR_DEFAULT_MINIMUM);
            OS.QProgressDialog_setMaximum(handle, PROGRESSBAR_DEFAULT_MINIMUM);
        } 
    }
    
    void releaseHandle () {
        OS.QWidget_setVisible(topHandle(), false);
        parent = null;
        Internal_PackageSupport.releaseHandle(this);
    }
    
    void qt_swt_event_widgetResized(int widgetHandle, int oldWidth, int oldHeight, int width, int height)
    {
        if (widgetHandle == topHandle()) {
            updatePosition();
        }
        
        Internal_PackageSupport.qt_swt_event_widgetResized(this, widgetHandle, oldWidth, oldHeight, width, height);
    }
    
    void updatePosition() {
        // TaskTip is positioned in the top-right corner of the display,
        // if style is RIGHT_TO_LEFT it's in the top-left corner
        int x = 0;
        
        if ((style() & SWT.RIGHT_TO_LEFT) == 0) {
            Rectangle displayBounds = Internal_PackageSupport.display(this).getBounds();
            Point size = OS.QWidget_size(topHandle());
            x = displayBounds.width-size.x;
        }
        
        OS.QWidget_move(topHandle(), x, 0);
    }
    
    /**
     * Sets the maximum value that the TaskTip will allow.
     * If the new maximum is applied, the TaskTip's selection value is
     * adjusted if necessary to fall within the new range. If a maximum 
     * value is not set, the default is 100.
     * <p>
     * This method works only for {@link SWT#SMOOTH SWT.SMOOTH} style. This method 
     * is silently ignored for other styles.
     * </p>
     * 
     * @param value
     *            the new maximum, which must be positive and greater than or 
     *            equal to the current minimum
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_ARGUMENT - if the value is negative or
     *                less than the current minimum</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void setMaximum(int value) {
        checkWidget();
        if ((style() & SWT.SMOOTH) == 0) return;

        int topHandle = topHandle();
        
        if(value < 0 || value < OS.QProgressDialog_minimum(topHandle)) {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }
        
        if (OS.QProgressDialog_value(topHandle) > value) {
            OS.QProgressDialog_setValue(topHandle, value);
        }
        
        OS.QProgressDialog_setMaximum(topHandle, value);
    }

    /**
     * Sets the minimum value that the TaskTip will allow. 
     * If the new minimum is applied, the TaskTip's selection
     * value is adjusted if necessary to fall within the new range.
     * If a minimum value is not set, the default is 0.
     * 
     * <p>
     * This method works only for {@link SWT#SMOOTH SWT.SMOOTH} style. This method 
     * is silently ignored for other styles.
     * </p>
     * 
     * @param value
     *            the new minimum, which must be positive and less than or 
     *            equal to the current maximum
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_ARGUMENT - if the value is negative or
     *                greater than the current maximum</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void setMinimum(int value) {
        checkWidget();
        if ((style() & SWT.SMOOTH) == 0) return;
        
        int topHandle = topHandle();
        
        if(value < 0 || value > OS.QProgressDialog_maximum(topHandle)) {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }
        
        if(OS.QProgressDialog_value(topHandle) < value) {
            OS.QProgressDialog_setValue(topHandle, value);
        }
            
        OS.QProgressDialog_setMinimum(topHandle, value);
    }
    
    /**
     * 
     * Sets the position of the TaskTip's indicator to the provided value.
     * The value must fall in the range defined by setMinimum() and setMaximum().
     * If a value is not set, the default is 0.
     * 
     * <p>
     * This method works only for {@link SWT#SMOOTH SWT.SMOOTH} style. This method 
     * is silently ignored for other styles.
     * </p>
     * 
     * @param value
     *            the new selection (must be zero or greater).
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_ARGUMENT - if the value is not in
     *                between the current minimum and maximum</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>IllegalArgumentException - if the vlaue falls out of
     *                the range defined by setMinimum() and setMaximum()</li>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     *  
     */
    public void setSelection(int value) {
        checkWidget();
        if ((style() & SWT.SMOOTH) == 0) return;
        
        int topHandle = topHandle();
        
        if (value < OS.QProgressDialog_minimum(topHandle) || 
            value > OS.QProgressDialog_maximum(topHandle)) {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }
        
        OS.QProgressDialog_setValue(topHandle, value);
    }

    /**
     * Sets the label text.
     * <p>
     * This method sets a TaskTip's label . The label can be null. The label's
     * position and the visibility of label text is platform-dependent.
     * </p>
     * 
     * @param string
     *            the label text
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     *  
     */
    public void setText(String string) {
        checkWidget();
        OS.QProgressDialog_setLabelText(topHandle(), string);
    }

    /**
     * Makes the TaskTip visible and brings it to the front of the display.
     * 
     * <p>
     * This method will return immediately after showing or hiding the TaskTip.
     * </p>
     * <p>
     * Despite setting a TaskTip visible, it may not actually be displayed if the
     * device's main screen is not active.
     * </p>
     * 
     * @param visible
     *            the value to show or hide the TaskTip. The method will not
     *            return an error if the value matches the current visibility
     *            state.
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the TaskTip has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void setVisible(boolean visible) {
        checkWidget();        
        OS.QWidget_setVisible(topHandle(), visible);
    }
    
    private final int topHandle() {
        return Internal_PackageSupport.topHandle(this); 
    }
    
    private final int style() {
        return Internal_PackageSupport.style(this);
    }
}