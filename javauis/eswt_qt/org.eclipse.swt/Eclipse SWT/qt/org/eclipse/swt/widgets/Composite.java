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
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.internal.qt.WidgetConstant;
import org.eclipse.swt.internal.qt.WidgetState;

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
public class Composite extends Scrollable {
    Layout layout;
    Control[] tabList;
    int layoutCount;
    int backgroundMode;
    
/**
 * Prevents uninitialized instances from being created outside the package.
 */
Composite () {
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
public Composite (Composite parent, int style) {
    super (parent, style);
}

/**
 * <p>
 * <b>IMPORTANT:</b> This constructor is <em>not</em> part of the SWT
 * public API. It should never be referenced from application code. 
 * </p>
 */
protected Composite(Composite parent, int style, int extraStyle, Object packageProxy, 
        boolean isExtended) {
    super(parent, style, extraStyle, packageProxy, isExtended);
}

void addControl_pp (Control control) {
}

void checkBuffered () {
    if ((state & WidgetState.CANVAS) == 0) {
        super.checkBuffered ();
    }
}

void checkNoBackground() {
    if((style & SWT.NO_BACKGROUND) != 0) {
        if(handle != 0) {
            OS.QWidget_setAttribute(handle, OS.QT_WA_NOSYSTEMBACKGROUND, true);
        }
    }
}

void checkNoFocus() {
    if((style & SWT.NO_FOCUS) != 0) {
        OS.QWidget_setFocusPolicy(handle, OS.QT_FOCUSPOLICY_NOFOCUS);
        int topHandle;
        if((topHandle = this.topHandle) != handle) {
            OS.QWidget_setFocusPolicy(topHandle, OS.QT_FOCUSPOLICY_NOFOCUS);
        }
    }
}

void checkNoRedrawResize() {
    if((style & SWT.NO_REDRAW_RESIZE) != 0) {
        if(handle != 0) {
            OS.QWidget_setAttribute(handle, OS.QT_WA_STATICCONTENTS, true);
        }
    }
}

protected void checkSubclass () {
    /* Do nothing - Subclassing is allowed */
}

public Point computeSize (int wHint, int hHint, boolean changed) {
    checkWidget ();

    if ((state & WidgetState.CANVAS) != 0) {
        Point size;
        if (layout != null) {
            if (wHint == SWT.DEFAULT || hHint == SWT.DEFAULT) {
                changed |= (state & WidgetState.LAYOUT_CHANGED) != 0;
                state &= ~WidgetState.LAYOUT_CHANGED;
                size = layout.computeSize (this, wHint, hHint, changed);
            } else {
                size = new Point (wHint, hHint);
            }
        } else {
            size = minimumSize (wHint, hHint, changed);
        }
        if (size.x == 0) size.x = WidgetConstant.DEFAULT_WIDTH;
        if (size.y == 0) size.y = WidgetConstant.DEFAULT_HEIGHT;
        if (wHint != SWT.DEFAULT) size.x = wHint;
        if (hHint != SWT.DEFAULT) size.y = hHint;
        Rectangle trim = computeTrim (0, 0, size.x, size.y);
        return new Point (trim.width, trim.height);        
    } else {
        // If this is not a stand-alone Composite, Shell or Canvas, 
        // call Scrollable's implementation
        return super.computeSize(wHint, hHint, changed);
    }
}

Control [] computeTabList () {
    Control result [] = super.computeTabList ();
    if (result.length == 0) return result;
    Control [] list = tabList != null ? _getTabList () : _getChildren ();
    for (int i=0; i<list.length; i++) {
        Control child = list [i];
        Control [] childList = child.computeTabList ();
        if (childList.length != 0) {
            Control [] newResult = new Control [result.length + childList.length];
            System.arraycopy (result, 0, newResult, 0, result.length);
            System.arraycopy (childList, 0, newResult, result.length, childList.length);
            result = newResult;
        }
    }
    return result;
}

void createHandle_pp (int index) {
    super.createHandle_pp (index);
    frameHandle = topHandle = scrollAreaHandle = OS.QScrollArea_new(0);
    handle = OS.QAbstractScrollArea_viewPort(scrollAreaHandle);

    // Composite by itself must not by default accept focus by clicking.
    int policy = OS.QWidget_focusPolicy(topHandle) & ~OS.QT_FOCUSPOLICY_CLICKFOCUS;
    OS.QWidget_setFocusPolicy(topHandle, policy);
    
    // Stand-alone Composites, Shells, Canvases set the CANVAS flag
    state |= (WidgetState.HANDLE | WidgetState.CANVAS);
}

boolean doHandleMouseEvent(int type, int widgetHandle, int button, int x, int y, int state, int buttons) {
    boolean cancel = super.doHandleMouseEvent(type, widgetHandle, button, x, y, state, buttons);
    if((this.state & WidgetState.CANVAS) != 0) {
        // From the CANVAS flag it's known if this is a Shell/stand-alone
        // Composite/Canvas. Those don't process the native mouse event but let it
        // through to the widget below. If the widget below also passes the event
        // to this Java widget then it will be getting multiple events. To avoid this 
        // the native event must be canceled.
        // Note: Also WA_NoMousePropagation could be used for this. 
        cancel = true;
        
        // Set focus for a canvas with no children
        if(type == SWT.MouseDown) {
            if ((style & SWT.NO_FOCUS) == 0 && hooksKeys ()) {
                Control[] children = getChildren();
                if (children == null || children.length == 0) {
                    setFocus (OS.QT_OTHERFOCUSREASON);
                }
            }
        }
    }
    return cancel;
}

Composite findDeferredControl () {
    return layoutCount > 0 ? this : parent.findDeferredControl ();
}

Menu [] findMenus (Control control) {
    if (control == this) return new Menu [0];
    Menu result [] = super.findMenus (control);
    Control [] children = _getChildren ();
    for (int i=0; i<children.length; i++) {
        Control child = children [i];
        Menu [] menuList = child.findMenus (control);
        if (menuList.length != 0) {
            Menu [] newResult = new Menu [result.length + menuList.length];
            System.arraycopy (result, 0, newResult, 0, result.length);
            System.arraycopy (menuList, 0, newResult, result.length, menuList.length);
            result = newResult;
        }
    }
    return result;
}

void fixTabList (Control control) {
    if (tabList == null) return;
    int count = 0;
    for (int i=0; i<tabList.length; i++) {
        if (tabList [i] == control) count++;
    }
    if (count == 0) return;
    Control [] newList = null;
    int length = tabList.length - count;
    if (length != 0) {
        newList = new Control [length];
        int index = 0;
        for (int i=0; i<tabList.length; i++) {
            if (tabList [i] != control) {
                newList [index++] = tabList [i];
            }
        }
    }
    tabList = newList;
}

/**
 * Returns the receiver's background drawing mode. This
 * will be one of the following constants defined in class
 * <code>SWT</code>:
 * <code>INHERIT_NONE</code>, <code>INHERIT_DEFAULT</code>,
 * <code>INHERTIT_FORCE</code>.
 *
 * @return the background mode
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 *
 * @see SWT
 * 
 * @since 3.2
 */
public int getBackgroundMode () {
    checkWidget ();
    return backgroundMode;
}

/**
 * Returns a (possibly empty) array containing the receiver's children.
 * Children are returned in the order that they are drawn.  The topmost
 * control appears at the beginning of the array.  Subsequent controls
 * draw beneath this control and appear later in the array.
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
public Control [] getChildren () {
    checkWidget();
    return _getChildren ();
}

Control [] _getChildren() {
    
    // For some reason MultiPageDialog's children are not children of the handle
    int widgetHandle = packageProxy != null ? packageProxy.handleWithChildren() : handleWithChildren_pp();
    
    int[] list = OS.QObject_children(widgetHandle); 
    int count = list.length;
    if (count == 0) return new Control [0];
    Control [] children = new Control [count];
    int childControls = 0;
    for(int i = 0; i < count; ++i) {
        int handle = list[i];
        if (handle != 0) {
            Widget widget = display.getWidget (handle);
            if (widget != null && widget != this) {
                if (widget instanceof Control && !(widget instanceof Shell)) {
                    children [childControls++] = (Control) widget;
                }
            }
        }
    }
    if (childControls == count) return children;
    Control [] newChildren = new Control [childControls];
    System.arraycopy (children, 0, newChildren, 0, childControls);
    return newChildren;
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
public Layout getLayout () {
    checkWidget();
    return layout;
}

/**
 * Gets the (possibly empty) tabbing order for the control.
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
public Control [] getTabList () {
    checkWidget ();
    Control [] tabList = _getTabList ();
    if (tabList == null) {
        int count = 0;
        Control [] list =_getChildren ();
        for (int i=0; i<list.length; i++) {
            if (list [i].isTabGroup ()) count++;
        }
        tabList = new Control [count];
        int index = 0;
        for (int i=0; i<list.length; i++) {
            if (list [i].isTabGroup ()) {
                tabList [index++] = list [i];
            }
        }
    }
    return tabList;
}

Control [] _getTabList () {
    if (tabList == null) return tabList;
    int count = 0;
    for (int i=0; i<tabList.length; i++) {
        if (!tabList [i].isDisposed ()) count++;
    }
    if (count == tabList.length) return tabList;
    Control [] newList = new Control [count];
    int index = 0;
    for (int i=0; i<tabList.length; i++) {
        if (!tabList [i].isDisposed ()) {
            newList [index++] = tabList [i];
        }
    }
    tabList = newList;
    return tabList;
}

/*
 * This method is here because MultiPageDialog for some reason has a Composite
 * which has its children not as children of the handle. It overrides this. 
 */ 
int handleWithChildren_pp() {
    return handle; // handle == viewport
}

boolean hooksKeys () {
    return hooks (SWT.KeyDown) || hooks (SWT.KeyUp);
}

/**
 * If the receiver has a layout, asks the layout to <em>lay out</em>
 * (that is, set the size and location of) the receiver's children. 
 * If the receiver does not have a layout, do nothing.
 * <p>
 * This is equivalent to calling <code>layout(true)</code>.
 * </p>
 * <p>
 * Note: Layout is different from painting. If a child is
 * moved or resized such that an area in the parent is
 * exposed, then the parent will paint. If no child is
 * affected, the parent will not paint.
 * </p>
 * 
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public void layout () {
    checkWidget ();
    layout (true);
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
 * (same as <code>layout(false)</code>).
 * </p>
 * <p>
 * Note: Layout is different from painting. If a child is
 * moved or resized such that an area in the parent is
 * exposed, then the parent will paint. If no child is
 * affected, the parent will not paint.
 * </p>
 *
 * @param changed <code>true</code> if the layout must flush its caches, and <code>false</code> otherwise
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public void layout (boolean changed) {
    checkWidget ();
    if (layout == null) return;
    layout (changed, false);
}

void layout (boolean changed, boolean all) {
    checkWidget ();
    if (layout == null && !all) return;
    markLayout (changed, all);
    updateLayout (all);
}

void markLayout (boolean changed, boolean all) {
    if (layout != null) {
        state |= WidgetState.LAYOUT_NEEDED;
        if (changed) state |= WidgetState.LAYOUT_CHANGED;
    }
    if (all) {
        Control [] children = _getChildren ();
        for (int i=0; i<children.length; i++) {
            children [i].markLayout (changed, all);
        }
    }
}

Point minimumSize (int wHint, int hHint, boolean changed) {
    Control [] children = _getChildren ();
    int width = 0, height = 0;
    for (int i=0; i<children.length; i++) {
        Rectangle rect = children [i].getBounds ();
        width = Math.max (width, rect.x + rect.width);
        height = Math.max (height, rect.y + rect.height);
    }
    return new Point (width, height);
}

void qt_swt_event_widgetResized_pp(int widgetHandle, int oldWidth, int oldHeight, int width, int height) {
    if (isDisposed ()) return;
    if((widgetHandle == handle) && isMirrored()) {
        // In real coordinate system the children need to be moved so that they
        // appear stationary in the emulated mirrored coordinate system. 
        final int dx = width - oldWidth;
        if(dx != 0) {
            Control[] children =_getChildren();
            try {
                display.blockedQtEventType = OS.QSWTEVENT_WIDGETMOVED;
                for (int i = 0; i < children.length; ++i) {
                    Control child = children[i];
                    if (!child.isDisposed()) {
                        final int childHandle = child.topHandle;
                        Point pos = OS.QWidget_pos(childHandle);
                        OS.QWidget_move(childHandle, pos.x + dx, pos.y);
                    }
                }
            } finally {
                display.blockedQtEventType = OS.QEVENT_NONE;
            }
        }
    }
    super.qt_swt_event_widgetResized_pp(widgetHandle, oldWidth, oldHeight, width, height);
    if (layout != null) {
        markLayout (false, false);
        updateLayout (false);
    }
}

void redrawChildren () {
    super.redrawChildren ();
    Control [] children = _getChildren ();
    for (int i = 0; i < children.length; i++) {
        Control child = children [i];
        if ((child.state & WidgetState.PARENT_BACKGROUND) != 0) {
            child.redraw();
            child.redrawChildren ();
        }
    }
}

void releaseChildren_pp (boolean destroy) {
    Control [] children = _getChildren ();
    for (int i=0; i<children.length; i++) {
        Control child = children [i];
        if (child != null && !child.isDisposed ()) {
            child.release (false);
        }
    }
    super.releaseChildren_pp (destroy);
}

void releaseWidget_pp () {
    super.releaseWidget_pp ();
    layout = null;
    tabList = null;
}

void removeControl_pp (Control control) {
    fixTabList (control);
}

/**
 * Sets the background drawing mode to the argument which should
 * be one of the following constants defined in class <code>SWT</code>:
 * <code>INHERIT_NONE</code>, <code>INHERIT_DEFAULT</code>,
 * <code>INHERIT_FORCE</code>.
 *
 * @param mode the new background mode
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 *
 * @see SWT
 * 
 * @since 3.2
 */
public void setBackgroundMode (int mode) {
    checkWidget();
    backgroundMode = mode;
    Control [] children = _getChildren ();
    for (int i = 0; i < children.length; i++) {
        children [i].updateBackgroundMode ();       
    }
}

int setBounds (int x, int y, int width, int height, boolean move, boolean resize) {
    int result = super.setBounds (x, y, width, height, move, resize);
    if ((result & WidgetState.RESIZED) != 0 && layout != null) {
        markLayout (false, false);
        updateLayout (false);
    }
    return result;
}

public boolean setFocus () {
    checkWidget();
    return setFocus(OS.QT_OTHERFOCUSREASON);
}

// Given focus reason affects if the tabbing focus highlight is drawn. 
// For QT_TABFOCUSREASON it is drawn.  For QT_OTHERFOCUSREASON it's not. 
boolean setFocus (int focusReason) {
    Control [] children = _getChildren ();
    for (int i=0; i<children.length; i++) {
        Control child = children [i];
        if (child.getVisible () && child.setFocus (focusReason)) return true;
    }
    return super.setFocus (focusReason);
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
public void setLayout (Layout layout) {
    checkWidget();
    this.layout = layout;
}

boolean setTabGroupFocus_pp () {
    if (isTabItem ()) return setTabItemFocus ();
    boolean takeFocus = (style & SWT.NO_FOCUS) == 0;

    // Canvas is getting focus only if it seems to implement key handling. 
    if ((state & WidgetState.CANVAS) != 0) {
        takeFocus = hooksKeys ();
        if ((style & SWT.EMBEDDED) != 0) takeFocus = true;
    }
    
    if (takeFocus && setTabItemFocus ()) return true;
    Control [] children = _getChildren ();
    for (int i=0; i<children.length; i++) {
        Control child = children [i];
        if (child.isTabItem () && child.setRadioFocus (true)) return true;
    }
    for (int i=0; i<children.length; i++) {
        Control child = children [i];
        if (child.isTabItem () && !child.isTabGroup () && child.setTabItemFocus ()) {
            return true;
        }
    }
    return false;
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
public void setTabList (Control [] tabList) {
    checkWidget ();
    if (tabList != null) {
        for (int i=0; i<tabList.length; i++) {
            Control control = tabList [i];
            if (control == null) error (SWT.ERROR_INVALID_ARGUMENT);
            if (control.isDisposed ()) error (SWT.ERROR_INVALID_ARGUMENT);
            if (control.parent != this) error (SWT.ERROR_INVALID_PARENT);
        }
        Control [] newList = new Control [tabList.length];
        System.arraycopy (tabList, 0, newList, 0, tabList.length);
        tabList = newList;
    } 
    this.tabList = tabList;
}

void updateBackground() {
    super.updateBackground ();
    Control [] children = _getChildren ();
    for (int i=0; i<children.length; i++) {
        if ((children [i].state & WidgetState.PARENT_BACKGROUND) != 0) {
            children [i].updateBackground ();
        }
    }
}

void updateBackgroundMode () {
    super.updateBackgroundMode ();
    Control [] children = _getChildren ();
    for (int i = 0; i < children.length; i++) {
        children [i].updateBackgroundMode ();       
    }
}

void updateLayout (boolean all) {
    Composite parent = findDeferredControl ();
    if (parent != null) {
        parent.state |= WidgetState.LAYOUT_CHILD;
        return;
    }
    if ((state & WidgetState.LAYOUT_NEEDED) != 0) {
        boolean changed = (state & WidgetState.LAYOUT_CHANGED) != 0;
        state &= ~(WidgetState.LAYOUT_NEEDED | WidgetState.LAYOUT_CHANGED);
        layout.layout (this, changed);
    }
    if (all) {
        state &= ~WidgetState.LAYOUT_CHILD;
        Control [] children = _getChildren ();
        for (int i=0; i<children.length; i++) {
            children [i].updateLayout (all);
        }
    }
}
}
