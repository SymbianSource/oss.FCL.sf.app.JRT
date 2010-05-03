/*******************************************************************************
 * Copyright (c) 2000, 2004 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *     Lynne Kues (IBM Corp) - modified to reflect eSWT API subset
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/
package org.eclipse.swt.widgets;


import org.eclipse.swt.*;
import org.eclipse.swt.graphics.*;
import org.eclipse.swt.internal.symbian.OS;

/**
 * Instances of the receiver represent is an unselectable
 * user interface object that is used to display progress,
 * typically in the form of a bar.
 * <dl>
 * <dt><b>Styles:</b></dt>
 * <dd>SMOOTH, HORIZONTAL, VERTICAL, INDETERMINATE</dd>
 * <dt><b>Events:</b></dt>
 * <dd>(none)</dd>
 * </dl>
 * <p>
 * Note: Only one of the styles HORIZONTAL and VERTICAL may be specified.
 * </p><p>
 * IMPORTANT: This class is intended to be subclassed <em>only</em>
 * within the SWT implementation.
 * </p>
 */
public class ProgressBar extends Control
{
    private int    progressBarHandle;

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
     * @see SWT#SMOOTH
     * @see SWT#HORIZONTAL
     * @see SWT#VERTICAL
     * @see Widget#checkSubclass
     * @see Widget#getStyle
     */
    public ProgressBar(Composite parent, int style)
    {
        super(parent, internal_checkStyle(style));
    }

    protected final void internal_setProgressBarHandle(int progressBarHandle)
    {
        this.progressBarHandle = progressBarHandle;
        handle = OS.ProgressBar_ControlHandle(progressBarHandle);
    }

    protected void internal_createHandle()
    {
        internal_setProgressBarHandle(OS.ProgressBar_New(this, parent.compositeHandle, style));
    }

    protected static int internal_checkStyle(int style)
    {
        style |= SWT.NO_FOCUS;
        style = internal_checkBits(style, SWT.HORIZONTAL, SWT.VERTICAL, 0, 0, 0, 0);
        return internal_checkBits(style, SWT.SMOOTH, SWT.INDETERMINATE, 0, 0, 0, 0);
    }

    public Point computeSize(int wHint, int hHint, boolean changed)
    {
        checkWidget();
        return OS.Control_ComputeSize(handle, wHint, hHint);
    }

    /**
     * Returns the maximum value which the receiver will allow.
     *
     * @return the maximum
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public int getMaximum()
    {
        checkWidget();
        return OS.ProgressBar_GetMaximum(progressBarHandle);
    }

    /**
     * Returns the minimum value which the receiver will allow.
     *
     * @return the minimum
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public int getMinimum()
    {
        checkWidget();
        return OS.ProgressBar_GetMinimum(progressBarHandle);
    }

    /**
     * Returns the single <em>selection</em> that is the receiver's position.
     *
     * @return the selection
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public int getSelection()
    {
        checkWidget();
        return OS.ProgressBar_GetSelection(progressBarHandle);
    }

    /**
     * Sets the maximum value that the receiver will allow.  This new
     * value will be ignored if it is not greater than the receiver's current
     * minimum value.  If the new maximum is applied then the receiver's
     * selection value will be adjusted if necessary to fall within its new range.
     *
     * @param value the new maximum, which must be greater than the current minimum
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setMaximum(int value)
    {
        checkWidget();
        if (value > 0)
        {
            OS.ProgressBar_SetMaximum(progressBarHandle, value);
        }
    }

    /**
     * Sets the minimum value that the receiver will allow.  This new
     * value will be ignored if it is negative or is not less than the receiver's
     * current maximum value.  If the new minimum is applied then the receiver's
     * selection value will be adjusted if necessary to fall within its new range.
     *
     * @param value the new minimum, which must be nonnegative and less than the current maximum
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setMinimum(int value)
    {
        checkWidget();
        if (value >= 0)
        {
            OS.ProgressBar_SetMinimum(progressBarHandle, value);
        }
    }

    /**
     * Sets the single <em>selection</em> that is the receiver's
     * position to the argument which must be greater than or equal
     * to zero.
     *
     * @param value the new selection (must be zero or greater)
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setSelection(int value)
    {
        checkWidget();
        OS.ProgressBar_SetSelection(progressBarHandle, value);
    }

}
