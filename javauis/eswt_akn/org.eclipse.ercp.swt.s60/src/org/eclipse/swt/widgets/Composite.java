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

import java.util.*;

/**
 * Instances of this class are controls which are capable
 * of containing other controls.
 * <dl>
 * <dt><b>Styles:</b></dt>
 * <dd>NO_BACKGROUND, NO_FOCUS, NO_MERGE_PAINTS, NO_REDRAW_RESIZE, NO_RADIO_GROUP</dd>
 * <dt><b>Events:</b></dt>
 * <dd>(none)</dd>
 * </dl>
 * <p>
 * Note: The <code>NO_BACKGROUND</code>, <code>NO_FOCUS</code>, <code>NO_MERGE_PAINTS</code>,
 * and <code>NO_REDRAW_RESIZE</code> styles are intended for use with <code>Canvas</code>.
 * They can be used with <code>Composite</code> if you are drawing your own, but their
 * behavior is undefined if they are used with subclasses of <code>Composite</code> other
 * than <code>Canvas</code>.
 * </p><p>
 * This class may be subclassed by custom control implementors
 * who are building controls that are constructed from aggregates
 * of other controls.
 * </p>
 *
 * @see Canvas
 */

public class Composite extends Scrollable
{
    int compositeHandle;
    Layout layout;
    Vector children = new Vector();
    int layoutCount = 0;

    // This reference is for case that Composite will be disposed from
    // DisposeListener of one of its children. When Control is being disposed
    // then first it is removed from Composite's children array in java side
    // but not in native side which causes inconsistency. Before it will be
    // removed also from native array there is among other also Control's
    // DisposeListener called. If the Composite is disposed from that listener
    // then the Control will remain in native children array which causes to fail
    // the assertion expecting no children in that array. Therefore store
    // reference to the Control which is being disposed (Control is responsible
    // for doing that) and eventually release its native part also. It is not
    // enough only to remove handle of the Control from native array because there
    // can be whole Display disposed.
    Control childBeingDisposed = null;
    
    Point sizeOfLastLayout = new Point(0, 0);

    /**
     * Prevents uninitialized instances from being created outside the package.
     */
    Composite()
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
     * @param parent a widget which will be the parent of the new instance (cannot be null)
     * @param style the style of widget to construct
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the parent</li>
     * </ul>
     *
     * @see SWT#NO_BACKGROUND
     * @see SWT#NO_FOCUS
     * @see SWT#NO_MERGE_PAINTS
     * @see SWT#NO_REDRAW_RESIZE
     * @see SWT#NO_RADIO_GROUP
     * @see Widget#getStyle
     */
    public Composite(Composite parent, int style)
    {
        super(parent, style);
    }
    
    protected void checkSubclass()
    {
        /* Do nothing - Subclassing is allowed */
    }
    
    public Point computeSize(int wHint, int hHint, boolean changed)
    {
        checkWidget();
        Point size;
        if (layout != null)
        {
            if (wHint == SWT.DEFAULT || hHint == SWT.DEFAULT)
            {
                changed |= (state & LAYOUT_CHANGED) != 0;
                state &= ~LAYOUT_CHANGED;
                size = layout.computeSize(this, wHint, hHint, changed);
            }
            else
            {
                size = new Point(wHint, hHint);
            }
        }
        else
        {
            size = minimumSize(wHint, hHint, changed);
        }
        if (size.x == 0) size.x = DEFAULT_WIDTH;
        if (size.y == 0) size.y = DEFAULT_HEIGHT;
        if (wHint != SWT.DEFAULT) size.x = wHint;
        if (hHint != SWT.DEFAULT) size.y = hHint;
        Rectangle trim = computeTrim(0, 0, size.x, size.y);
        return new Point(trim.width, trim.height);
    }
    
    protected final void internal_setCompositeHandle(int compositeHandle)
    {
        this.compositeHandle = compositeHandle;
        internal_setScrollableHandle(OS.Composite_ScrollableHandle(compositeHandle));
        state |= CANVAS;
    }
    
    protected void internal_createHandle()
    {
        internal_setCompositeHandle(OS.Composite_New(this, parent.compositeHandle, style));
    }
    
    /**
     * Returns an array containing the receiver's children.
     * Children are returned in the order that they are drawn.
     * <p>
     * Note: This is not the actual structure used by the receiver
     * to maintain its list of children, so modifying the array will
     * not affect the receiver.
     * </p>
     *
     * @return an array of children
     *
     * @see Control#moveAbove
     * @see Control#moveBelow
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public Control [] getChildren()
    {
        checkWidget();
        return _getChildren();
    }
    
    /* We cannot use the children array as it does not store them
     * in the correct order. Its purpose is in fact just to prevent
     * them from being garbage collected.
     * */
    Control [] _getChildren()
    {
        return OS.Composite_GetChildren(compositeHandle);
    }

    /**
     * Returns layout which is associated with the receiver, or
     * null if one has not been set.
     *
     * @return the receiver's layout or null
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public Layout getLayout()
    {
        checkWidget();
        return layout;
    }

    /**
     * Gets the last specified tabbing order for the control.
     *
     * @return tabList the ordered list of controls representing the tab order
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see #setTabList
     */
    public Control [] getTabList()
    {
        checkWidget();
        return OS.Composite_GetTabList(compositeHandle);
    }
    
    public boolean internal_isLayoutDeferred()
    {
        checkWidget();
        if (parent != null)
            return layoutCount > 0 || parent.internal_isLayoutDeferred();
        else
            return layoutCount > 0;
    }

    /**
     * If the receiver has a layout, asks the layout to <em>lay out</em>
     * (that is, set the size and location of) the receiver's children.
     * If the receiver does not have a layout, do nothing.
     * <p>
     * This is equivalent to calling <code>layout(true)</code>.
     * </p>
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void layout()
    {
        checkWidget();
        layout(true);
    }

    /**
     * If the receiver has a layout, asks the layout to <em>lay out</em>
     * (that is, set the size and location of) the receiver's children.
     * If the argument is <code>true</code> the layout must not rely
     * on any information it has cached about the immediate children. If it
     * is <code>false</code> the layout may (potentially) optimize the
     * work it is doing by assuming that none of the receiver's
     * children has changed state since the last layout.
     * If the receiver does not have a layout, do nothing.
     * <p>
     * If a child is resized as a result of a call to layout, the
     * resize event will invoke the layout of the child.  The layout
     * will cascade down through all child widgets in the receiver's widget
     * tree until a child is encountered that does not resize.  Note that
     * a layout due to a resize will not flush any cached information
     * (same as <code>layout(false)</code>).</p>
     *
     * @param changed <code>true</code> if the layout must flush its caches, and <code>false</code> otherwise
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void layout(boolean changed)
    {
        checkWidget();
        if (layout == null) return;
        layout(changed, false);
    }

    void layout(boolean changed, boolean all)
    {
        checkWidget();
        if (layout == null && !all) return;
        markLayout(changed, all);
        updateLayout(all, false); // all, resized
    }

    void markLayout(boolean changed, boolean all)
    {
        if (layout != null)
        {
            state |= LAYOUT_NEEDED;
            if (changed) state |= LAYOUT_CHANGED;
        }
        if (all) 
        {
            Control [] children = _getChildren ();
            for (int i=0; i<children.length; i++) 
            {
                children [i].markLayout (changed, all);
            }
        }
    }
    
    Point minimumSize(int wHint, int hHint, boolean changed)
    {
        Control [] children = _getChildren();
        int width = 0, height = 0;
        for (int i=0; i<children.length; i++)
        {
            Rectangle rect = children [i].getBounds();
            width = Math.max(width, rect.x + rect.width);
            height = Math.max(height, rect.y + rect.height);
        }
        return new Point(width, height);
    }
    
    protected void releaseChildren()
    {
        for (int i=0; i<children.size(); ++i)
        {
            Control child = (Control)children.elementAt(i);
            if (!child.isDisposed())
            {
                child.internal_controlsPaintGcClean();
                child.internal_release();
            }
        }
        children.removeAllElements();

        if (childBeingDisposed != null)
        {
            // Composite is being disposed from one of its child which was already
            // removed from children vector but it was stored to childBeingDisposed
            // reference. Disposal of this child is needed before deleting Composite's
            // native side.
            childBeingDisposed.internal_releaseHandle();
            childBeingDisposed = null;
        }
    }
    
    protected void internal_releaseResources()
    {
        releaseChildren();
        super.internal_releaseResources();
        layout = null;
        children = null;
    }
    
    void addControl(Control control)
    {
        children.addElement(control);
    }

    void removeControl(Control control)
    {
        children.removeElement(control);
    }
    
    int setBounds (int x, int y, int width, int height, boolean move, boolean resize) {
        int result = super.setBounds (x, y, width, height, move, resize);
        if ((result & Widget.RESIZED) != 0 && layout != null) {
            markLayout (false, false);
            updateLayout (false, true); // all, resized
        }
        return result;
    }
    
    /**
     * Sets the layout which is associated with the receiver to be
     * the argument which may be null.
     *
     * @param layout the receiver's new layout or null
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setLayout(Layout layout)
    {
        checkWidget();
        this.layout = layout;
    }
    
    /**
     * Sets the tabbing order for the specified controls to
     * match the order that they occur in the argument list.
     *
     * @param tabList the ordered list of controls representing the tab order or null
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_ARGUMENT - if a widget in the tabList is null or has been disposed</li>
     *    <li>ERROR_INVALID_PARENT - if widget in the tabList is not in the same widget tree</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setTabList(Control [] tabList)
    {
        checkWidget();
        int[] handleList = null;
        if (tabList != null)
        {
            handleList = new int [tabList.length];
            for (int i=0; i<tabList.length; i++)
            {
                Control control = tabList [i];
                if (control == null) internal_error(SWT.ERROR_INVALID_ARGUMENT);
                if (control.isDisposed()) internal_error(SWT.ERROR_INVALID_ARGUMENT);
                if (control.parent != this) internal_error(SWT.ERROR_INVALID_PARENT);
                handleList[i] = tabList[i].handle;
            }
        }
        OS.Composite_SetTabList(compositeHandle, handleList);
    }
    
    /**
     * Makes the final call to layout.
     * @param all - recursive call
     * @param resized - update required after the Control has been resized
     */
    void updateLayout (boolean all, boolean resized)
    {
        if (internal_isLayoutDeferred()) {
            return;
        }
        if ((state & LAYOUT_NEEDED) != 0)
        {
            boolean changed = (state & LAYOUT_CHANGED) != 0;
            state &= ~(LAYOUT_NEEDED | LAYOUT_CHANGED);

            // The last layout bounds are considered only if the layout updates after a resize.
            Point currentSize = OS.Control_GetSize(handle);
            if (!resized || !currentSize.equals(sizeOfLastLayout))
            {
                sizeOfLastLayout = currentSize;
                layout.layout(this, changed);
            }
        }
        
        if (all) 
        {
            Control [] children = _getChildren ();
            for (int i=0; i<children.length; i++) 
            {
                children [i].updateLayout (all, resized);
            }
        }
    }
    
    void sendResizeEvent()
    {
        super.sendResizeEvent();

        if (isDisposed()) return;
        if (layout != null) 
        {
            markLayout (false, false);
            updateLayout (false, true); // all, resized
        }
    }
    
    public final int internal_getCompositeHandle()
    {
        return compositeHandle;
    }

    Rectangle paint(Rectangle rect, boolean paintNatively, boolean paintChildren)
    {
        // Paint itself
        Rectangle dirtyClient = super.paint(rect, paintNatively, paintChildren);
        // Paint children
        if (dirtyClient != null && paintChildren)
        {
            // The foreground child is the first one, so draw from last to first
            Control[] children = getChildren();
            for (int i=children.length-1; i>=0; --i)
            {
                Control child = children[i];
                if (child.getVisible())
                {
                    Rectangle childBounds = toDisplay(child.getBounds());
                    Rectangle dirtyChild  = rectangleIntersection(dirtyClient, childBounds);
                    if (dirtyChild != null) child.paint(dirtyChild, true, true);
                }
            }
        }

        return dirtyClient;
    }
    
    protected void internal_setScrolledCompositeContent(Control content)
    {
        OS.Composite_SetScrolledCompositeContent(compositeHandle, content.handle);
    }
}
