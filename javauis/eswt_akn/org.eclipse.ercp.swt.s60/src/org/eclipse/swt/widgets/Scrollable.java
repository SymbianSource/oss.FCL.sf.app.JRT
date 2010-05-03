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

import org.eclipse.swt.internal.symbian.*;
import org.eclipse.swt.*;
import org.eclipse.swt.graphics.*;

/**
 * This class is the abstract superclass of all classes which
 * represent controls that have standard scroll bars.
 * <dl>
 * <dt><b>Styles:</b></dt>
 * <dd>H_SCROLL, V_SCROLL</dd>
 * <dt><b>Events:</b>
 * <dd>(none)</dd>
 * </dl>
 * <p>
 * IMPORTANT: This class is intended to be subclassed <em>only</em>
 * within the SWT implementation.
 * </p>
 */

public abstract class Scrollable extends Control
{
    int scrollableHandle;
    ScrollBar horizontalBar, verticalBar;

    /**
     * Prevents uninitialized instances from being created outside the package.
     */
    Scrollable()
    {
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
     * @see SWT#H_SCROLL
     * @see SWT#V_SCROLL
     * @see Widget#checkSubclass
     * @see Widget#getStyle
     */
    public Scrollable(Composite parent, int style)
    {
        super(parent, style);
    }

    protected Scrollable(Composite parent, int style, int layoutStyle)
    {
        super(parent, style, layoutStyle);
    }

    protected final void internal_setScrollableHandle(int scrollableHandle)
    {
        this.scrollableHandle = scrollableHandle;
        handle = OS.Scrollable_ControlHandle(scrollableHandle);
    }

    /**
     * Given a desired <em>client area</em> for the receiver
     * (as described by the arguments), returns the bounding
     * rectangle which would be required to produce that client
     * area.
     * <p>
     * In other words, it returns a rectangle such that, if the
     * receiver's bounds were set to that rectangle, the area
     * of the receiver which is capable of displaying data
     * (that is, not covered by the "trimmings") would be the
     * rectangle described by the arguments (relative to the
     * receiver's parent).
     * </p>
     *
     * @param x the desired x coordinate of the client area
     * @param y the desired y coordinate of the client area
     * @param width the desired width of the client area
     * @param height the desired height of the client area
     * @return the required bounds to produce the given client area
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see #getClientArea
     */
    public Rectangle computeTrim(int x, int y, int width, int height)
    {
        checkWidget();
        return OS.Scrollable_ComputeTrim(scrollableHandle, x, y, width, height);
    }

    final ScrollBar createScrollBar(int type)
    {
        ScrollBar bar = new ScrollBar(this, type);

        if (bar.internal_getHandle() == 0)
        {
            bar = null;
            return bar;
        }

        return bar;
    }

    final int createScrollBarHandle(ScrollBar scrollbar, int style)
    {
        return OS.Scrollable_CreateScrollBar(scrollableHandle, scrollbar, style);
    }

    protected void internal_createWidget()
    {
        super.internal_createWidget();
        if ((getStyle() & SWT.H_SCROLL) != 0)
        {
            horizontalBar = createScrollBar(SWT.H_SCROLL);
        }
        if ((getStyle() & SWT.V_SCROLL) != 0)
        {
            verticalBar = createScrollBar(SWT.V_SCROLL);
        }
    }

    /**
     * Returns a rectangle which describes the area of the
     * receiver which is capable of displaying data (that is,
     * not covered by the "trimmings").
     *
     * @return the client area
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see #computeTrim
     */
    public Rectangle getClientArea()
    {
        checkWidget();
        return super.getClientArea();
    }

    /**
     * Returns the receiver's horizontal scroll bar if it has
     * one, and null if it does not.
     *
     * @return the horizontal scroll bar (or null)
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public ScrollBar getHorizontalBar()
    {
        checkWidget();
        return horizontalBar;
    }

    /**
     * Returns the receiver's vertical scroll bar if it has
     * one, and null if it does not.
     *
     * @return the vertical scroll bar (or null)
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public ScrollBar getVerticalBar()
    {
        checkWidget();
        return verticalBar;
    }


    protected void internal_releaseResources()
    {
        if (horizontalBar != null) horizontalBar.internal_release();
        if (verticalBar != null) verticalBar.internal_release();
        horizontalBar = verticalBar = null;
        super.internal_releaseResources();
    }

}
