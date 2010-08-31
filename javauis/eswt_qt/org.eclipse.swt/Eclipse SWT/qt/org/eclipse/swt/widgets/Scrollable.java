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
 * This class is the abstract superclass of all classes which represent controls
 * that have standard scroll bars.
 * <dl>
 * <dt><b>Styles:</b></dt>
 * <dd>H_SCROLL, V_SCROLL</dd>
 * <dt><b>Events:</b>
 * <dd>(none)</dd>
 * </dl>
 * <p>
 * IMPORTANT: This class is intended to be subclassed <em>only</em> within the
 * SWT implementation.
 * </p>
 */
public abstract class Scrollable extends Control {
    ScrollBar horizontalBar;
    ScrollBar verticalBar;

    /*
     * Handle of the native object implementing QAbstractScrollArea for the
     * widget. May be 0 because not all Scrollable objects derive from
     * QAbstractScrollArea in native side.
     * Must be checked if this is valid before using!
     */
    int scrollAreaHandle;

    /**
     * Prevents uninitialized instances from being created outside the package.
     */
    Scrollable() {
    }

    /**
     * Constructs a new instance of this class given its parent and a style
     * value describing its behavior and appearance.
     * <p>
     * The style value is either one of the style constants defined in class
     * <code>SWT</code> which is applicable to instances of this class, or must
     * be built by <em>bitwise OR</em>'ing together (that is, using the
     * <code>int</code> "|" operator) two or more of those <code>SWT</code>
     * style constants. The class description lists the style constants that are
     * applicable to the class. Style bits are also inherited from superclasses.
     * </p>
     * 
     * @param parent
     *            a composite control which will be the parent of the new
     *            instance (cannot be null)
     * @param style
     *            the style of control to construct
     * 
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
     * 
     * @see SWT#H_SCROLL
     * @see SWT#V_SCROLL
     * @see Widget#checkSubclass
     * @see Widget#getStyle
     */
    public Scrollable(Composite parent, int style) {
        this(parent, style, 0, null, false);
    }
  
    /**
     * <p>
     * <b>IMPORTANT:</b> This constructor is <em>not</em> part of the SWT
     * public API. It should never be referenced from application code. 
     * </p>
     */
    protected Scrollable(Composite parent, int style, int extraStyle, Object packageProxy, 
            boolean isExtended) {
        super(parent, style, extraStyle, packageProxy, isExtended);
    }

    int clientHandle() {
        return handle;
    }

    public Point computeSize(int wHint, int hHint, boolean changed) {
        checkWidget();

        if (scrollAreaHandle == 0) {
            return super.computeSize(wHint, hHint, changed);
        }

        if (changed) {
            OS.QWidget_updateGeometry(topHandle);
        }

        Point preferredSize = (packageProxy != null ? 
                packageProxy.getPreferredClientAreaSize() : getPreferredClientAreaSize_pp());
        Rectangle trim = computeTrim(0, 0, preferredSize.x, preferredSize.y);
        Point size = new Point(trim.width, trim.height);

        if (wHint != SWT.DEFAULT) {
            size.x = wHint + trim.width - preferredSize.x;
        }
        if (hHint != SWT.DEFAULT) {
            size.y = hHint + trim.height - preferredSize.y;
        }

        return size;
    }

    /**
     * Given a desired <em>client area</em> for the receiver (as described by
     * the arguments), returns the bounding rectangle which would be required to
     * produce that client area.
     * <p>
     * In other words, it returns a rectangle such that, if the receiver's
     * bounds were set to that rectangle, the area of the receiver which is
     * capable of displaying data (that is, not covered by the "trimmings")
     * would be the rectangle described by the arguments (relative to the
     * receiver's parent).
     * </p>
     * 
     * @param x
     *            the desired x coordinate of the client area
     * @param y
     *            the desired y coordinate of the client area
     * @param width
     *            the desired width of the client area
     * @param height
     *            the desired height of the client area
     * @return the required bounds to produce the given client area
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * 
     * @see #getClientArea
     */
    public Rectangle computeTrim(int x, int y, int width, int height) {
        checkWidget();
        int border = getBorderWidth();
        x -= border;
        y -= border;
        width += 2 * border;
        height += 2 * border;

        ScrollBar vBar = getVerticalBar();
        if (vBar != null) {
                     int barWidth = vBar.getSize().x;
                     width += barWidth;
        }

        ScrollBar hBar = getHorizontalBar();
        if (hBar != null) {
            height += hBar.getSize().y;
        }

        return new Rectangle(x, y, width, height);
    }
    

    final ScrollBar createScrollBar(int style) {
        if ((style & SWT.H_SCROLL) != 0) {
            setHBarPolicy(true);
        } else {
            setVBarPolicy(true);
        }
        return new ScrollBar(this, style);
    }

    void createWidget(int index) {
        super.createWidget(index);
        // If topHandle wasn't set by a subclass
        if(topHandle == 0) {
            // If scrollAreaHandle was set by a subclass then use that as the
            // topHandle by default
            if(scrollAreaHandle != 0) {
                topHandle = scrollAreaHandle;
            }
        }
        // Same for frameHandle
        if(frameHandle == 0) {
            if(scrollAreaHandle != 0) {
                frameHandle = scrollAreaHandle;
            }            
        }
        createBars();
        if(Display.objectNames) {
            if(scrollAreaHandle != 0 && scrollAreaHandle != topHandle && scrollAreaHandle != handle) {
                OS.QObject_setObjectName(scrollAreaHandle, 
                        this.getClass().getName() + " Scrollable.scrollAreaHandle");
            }
        }
    }

    private void createBars() {
        if (scrollAreaHandle != 0) {
            if (((style & SWT.H_SCROLL) != 0) && (state & WidgetState.EMBEDDED_SCROLLBARS) == 0) {
                if (horizontalBar == null) {
                    horizontalBar = createScrollBar(SWT.H_SCROLL);
                }
            } else {
                setHBarPolicy(false);
            }

            if (((style & SWT.V_SCROLL) != 0) && (state & WidgetState.EMBEDDED_SCROLLBARS) == 0) {
                if (verticalBar == null) {
                    verticalBar = createScrollBar(SWT.V_SCROLL);
                }
            } else {
                setVBarPolicy(false);
            }
        }
    }

    /**
     * Returns a rectangle which describes the area of the receiver which is
     * capable of displaying data (that is, not covered by the "trimmings").
     * 
     * @return the client area
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * 
     * @see #computeTrim
     */
    public Rectangle getClientArea() {
        checkWidget();
        if(!isVisible()) updateQLayouts();

        // QWidget::rect() always returns (0,0) as top left. 
        // It is ok to use (0,0) as the client area top left because native viewport widget   
        // is the parent widget of the Scrollable's children and viewport does have any trimmings.
        // E.g. implementations of Layout.layout(Composite, boolean) read the client 
        // area top left when setting bounds for Composite's children. 
        Rectangle clientArea = OS.QWidget_rect(handle);
        
        // Null size, invisible SortedList gets negative viewPort. Qt layouts suspected.
        if (clientArea.width < 0) clientArea.width = 0;
        if (clientArea.height < 0) clientArea.height = 0;
        
        return clientArea;
    }

    /*
     * There's a problem that Qt doesn't apply the layouts or size policies when
     * widgets are not visible. The purpose of this method is to force Qt to
     * update the layouts e.g. when getClientArea is called before widget is
     * shown. Without this getClientArea won't return correct values. 
     */
    void updateQLayouts() {
        if(parent != null) parent.updateQLayouts();
        updateLayoutOfQWidget(topHandle);
        if(scrollAreaHandle != 0 && scrollAreaHandle != topHandle)
            updateLayoutOfQWidget(scrollAreaHandle);
        if(handle != scrollAreaHandle)
            updateLayoutOfQWidget(handle);
        
        // There's no public way to directly make QAbstractScrollArea execute its
        // layout calculation code. Corner widget functionality can be used to 
        // invoke it indirectly if there's no corner widget. I.e. this has nothing
        // to do with corner widget - the purpose is just to call layout calculation 
        // code by any means available. 
        if(scrollAreaHandle != 0) {
            int cornerWidget = OS.QAbstractScrollArea_cornerWidget(scrollAreaHandle);
            if(cornerWidget == 0) OS.QAbstractScrollArea_setCornerWidget(scrollAreaHandle, 0);
        }       
    }
    
    void updateLayoutOfQWidget(int handle) {
        int layout = OS.QWidget_layout(handle);
        if(layout != 0) {
            OS.QLayout_activate(layout);
            OS.QLayout_update(layout);
        }
    }

    /**
     * Returns the receiver's horizontal scroll bar if it has one, and null if
     * it does not.
     * 
     * @return the horizontal scroll bar (or null)
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public ScrollBar getHorizontalBar() {
        checkWidget();
        return horizontalBar;
    }

    /**
     * Returns the receiver's vertical scroll bar if it has one, and null if it
     * does not.
     * 
     * @return the vertical scroll bar (or null)
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public ScrollBar getVerticalBar() {
        checkWidget();
        return verticalBar;
    }

    /*
     * Gets the preferred size of the widget excluding trimings.
     */
    Point getPreferredClientAreaSize_pp() {
        /*
         * Here, the handle is supposed to be a view port's handle. If not,
         * overwrite this method in the subclass
         */
        if (handle == 0)
            return new Point(WidgetConstant.DEFAULT_WIDTH, WidgetConstant.DEFAULT_HEIGHT);
        /*
        /* It seems that next line always returns an invalid size since there is no associated layout for viewport widget.
        /* So the function ends up with returning a default size
         */
        Point size = OS.QWidget_sizeHint(handle);
        if (size.x < 0)
            size.x = WidgetConstant.DEFAULT_WIDTH;
        if (size.y < 0)
            size.y = WidgetConstant.DEFAULT_HEIGHT;
        return size;
    }

    public void setData(String key, Object value) {
        super.setData(key, value);
         
        if (key.equals(WidgetConstant.SET_EMBEDDED_SCROLLBARS_STATE_KEY)) {
            if ((state & WidgetState.EMBEDDED_SCROLLBARS) != 0) {
                disposeBars();
                releaseBars();
            } else {
                createBars();
            }
        }
    }
    
    private void disposeBars() {
        if (horizontalBar != null && !horizontalBar.isDisposed()) {
            horizontalBar.dispose();
        }
        if (verticalBar != null && !verticalBar.isDisposed()) {
            verticalBar.dispose();
        }
    }

    void releaseChildren_pp(boolean destroy) {
        releaseBars();
        super.releaseChildren_pp(destroy);
    }

    private void releaseBars() {
        if (horizontalBar != null) {
            horizontalBar.release(false);
            horizontalBar = null;
        }
        if (verticalBar != null) {
            verticalBar.release(false);
            verticalBar = null;
        }
    }

    void deregister_pp() {
        super.deregister_pp();
        Display.removeWidget(scrollAreaHandle);
    }

    void register_pp() {
        super.register_pp();
        Display.addWidget(scrollAreaHandle, this);
    }

    void releaseHandle_pp() {
        scrollAreaHandle = 0;
        super.releaseHandle_pp();
    }

    void setOrientation(int handle, int orientation) {
        checkWidget();
        int flags = SWT.RIGHT_TO_LEFT | SWT.LEFT_TO_RIGHT;
        if ((orientation & flags) == 0 || (orientation & flags) == flags)
            return;

        style &= ~flags;
        style |= orientation & flags;

        OS.QWidget_setLayoutDirection(handle,
                orientation == SWT.LEFT_TO_RIGHT ? OS.QT_LEFTTORIGHT
                        : OS.QT_RIGHTTOLEFT);
    }
    
    void setHBarPolicy(boolean status) {
        if (status)
            OS.QAbstractScrollArea_setHorizontalScrollBarPolicy(scrollAreaHandle, 
                OS.QT_SCROLLBARALWAYSON);
        else
            OS.QAbstractScrollArea_setHorizontalScrollBarPolicy(scrollAreaHandle, 
                OS.QT_SCROLLBARALWAYSOFF);
    }
    
    void setVBarPolicy(boolean status) {
        if (status)
            OS.QAbstractScrollArea_setVerticalScrollBarPolicy(scrollAreaHandle, 
                OS.QT_SCROLLBARALWAYSON);
        else
            OS.QAbstractScrollArea_setVerticalScrollBarPolicy(scrollAreaHandle, 
                OS.QT_SCROLLBARALWAYSOFF);
    }

}
