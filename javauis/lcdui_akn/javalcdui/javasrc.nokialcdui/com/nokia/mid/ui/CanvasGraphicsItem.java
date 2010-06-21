/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0"
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description:  Public abstract class defining the
 *               interface for MIDP Canvas Graphics Item.
 *
 */

package com.nokia.mid.ui;

import java.util.Enumeration;
import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.CustomItem;
import javax.microedition.lcdui.Graphics;

import com.nokia.mid.ui.CanvasItem;
import com.nokia.mid.ui.CanvasGraphicsItemPainter;

import com.nokia.mj.impl.rt.legacy.NativeError;
import com.nokia.mj.impl.rt.legacy.ToolkitInvoker;
import com.nokia.mj.impl.rt.support.Finalizer;

/**
 * <P>
 * A <code>CanvasGraphicsItem</code> is a drawable component that is used with a
 * parent object; with <code>Canvas</code>. The <code>CanvasGraphicsItem</code>
 * may be added to and removed from <code>Canvas</code> through
 * <code>setParent</code> method.
 * </P>
 *
 * <P>
 * Parent for <code>CanvasGraphicsItem</code> must be set, through
 * <code>setParent</code> method, before other methods are called and change the
 * state or parameters of <code>CanvasGraphicsItem</code>.
 * </P>
 *
 * <P>
 * The implementation presents the <code>CanvasGraphicsItem</code> with the same
 * draw capabilities as <code>Canvas</code> has. See <a
 * href="Graphics.html">Graphics</a> for drawing options.
 * </P>
 *
 * <P>
 * <code>CanvasGraphicsItem</code> does not support DSA over its area. Playing
 * video or using camera over <code>CanvasGraphicsItem</code> area can lead to
 * artifacts on screen.
 * </P>
 *
 * <P>
 * <code>CanvasGraphicsItem</code> does not support touch input. Pointer events
 * are delivered to parent object from the area of the visible
 * <code>CanvasGraphicsItem</code>.
 * </P>
 *
 * <P>
 * Visibility of <code>CanvasGraphicsItem</code> is set through
 * <code>setVisible()</code>. The default visbility is set to false. Changing
 * parent does not affect the visibility status.
 * </P>
 *
 * <P>
 * The <code>CanvasGraphicsItem</code> cannot have focus.
 * </P>
 *
 * @since 1.4
 */
public abstract class CanvasGraphicsItem
        extends CanvasItem
{
    // Private data

    // Native handle
    int iHandle;

    // LCDUI Toolkit invoker object.
    ToolkitInvoker iToolkitInvoker;

    // LCDUI Toolkit object.
    Object iToolkit;

    //Object finalizer
    private Finalizer mFinalizer;

    private com.nokia.mid.ui.CanvasGraphicsItemPainter iItemPainter;

    /**
     * Creates a new <code>CanvasGraphicsItem</code> object with the given
     * initial size.
     *
     * @param width
     *            width in pixels
     * @param height
     *            height in pixels
     *
     * @throws IllegalArgumentException
     *             if the width or height is less than one pixel
     */
    public CanvasGraphicsItem(int width, int height)
    {
        // Validate width and height.
        if (!(width >= 1 && height >= 1))
        {
            throw new IllegalArgumentException(ERROR_GIVEN_ARGUMENTS_NOT_VALID);
        }

        // Toolkit invoker is needed for accessing javax.microedition.lcdui
        // package
        iToolkitInvoker = ToolkitInvoker.getToolkitInvoker();
        iToolkit = iToolkitInvoker.getToolkit();

        // Create painter
        iItemPainter =
            CanvasGraphicsItemPainterInvoker.createCanvasGraphicsItemPainter(
                this, iToolkit, width, height);

        int painterHandle = iItemPainter.getHandle();

        int handle = 0;

        synchronized (iToolkit)
        {
            // Create native peer object for this Java object.
            handle =
                _createNativePeer(getToolkitHandle(), painterHandle, width,
                                  height);
        }

        // Check if construction failed and throw out of memory error.
        if (handle <= NativeError.KErrNone)
        {
            throw new OutOfMemoryError();
        }

        // Operation was a success, store size.
        iWidth = width;
        iHeight = height;

        // Sets parent to null
        iParent = null;

        // Set visibility
        iVisible = false;

        // Construction was successful. Store handle and register for
        // finalization.
        iHandle = handle;

        mFinalizer = new Finalizer()
        {
            public void finalizeImpl()
            {
                registeredFinalize();
            }
        };
    }

    /**
     * Set the parent object of this <code>CanvasGraphicsItem</code>.
     *
     * Typically the parent object would be Canvas. Setting the parameter to
     * null removes the association to the parent. If
     * <code>setParent(null)</code> is called for a
     * <code>CanvasGraphicsItem</code> yet not having any parent or
     * <code>setParent(parent)</code> is called with the same parent, the call
     * is silently ignored.
     *
     * @param parent
     *            the parent object
     *
     * @throws IllegalArgumentException
     *             if <code>aParent</code> is not a valid object with which a
     *             CanvasGraphicsItem can be associated, or if CanvasItem
     *             is already set to another another parent
     */
    public void setParent(java.lang.Object parent)
    {
        // Ignore argument check if parent is set to null.
        // Custom item support will be added in future, currently it's
        // unsupported.
        if (((parent != null) && !(parent instanceof Canvas)) ||
                ((parent != null) && (iParent != null) && (iParent != parent)))
        {
            throw new IllegalArgumentException(ERROR_NOT_A_VALID_PARENT_OBJECT);
        }

        if (parent != iParent)
        {
            // Lock this object so that parent cannot be adjusted from other
            // threads.
            synchronized (iToolkit)
            {
                int parentHandle = 0;

                // Toolkit invoker can be used to get proper handles.
                if (parent instanceof Canvas)
                {
                    // Note that canvas is shown inside a displayable so in this
                    // case we use the handle to the displayble object.
                    parentHandle = iToolkitInvoker.getDisplayableHandle(parent);
                }
                else if (parent instanceof CustomItem)
                {
                    parentHandle = iToolkitInvoker.itemGetHandle(parent);
                }

                // parentHandle = 0 indicates removal of the parent.
                int error =
                    _setParent(getToolkitHandle(), iHandle, parentHandle);

                // Check if an error occured when setting the parent object.
                NativeError.check(error);

                // Store new parent
                iParent = parent;
            }
        }
    }

    /**
     * Sets the size of this <code>CanvasGraphicsItem</code> in pixels.
     *
     * @param width
     *            width in pixels
     * @param height
     *            height in pixels
     *
     * @throws IllegalArgumentException
     *             if the width or height is less than one pixel
     */
    public void setSize(int width, int height)
    {
        // Validate width and row count
        if (width < 1 || height < 1)
        {
            throw new IllegalArgumentException(ERROR_GIVEN_ARGUMENTS_NOT_VALID);
        }

        synchronized (iToolkit)
        {
            NativeError.check(_setSize(getToolkitHandle(), iHandle, width,
                                       height));

            // Operation was a success, store size.
            iWidth = width;
            iHeight = height;
        }
    }

    /**
     * <P>
     * Sets the rendering position of this <code>CanvasGraphicsItem</code>. The
     * anchor point given is relative to the upper left corner of the target
     * <code>Canvas</code>.
     * </P>
     *
     * <P>
     * The <code>CanvasGraphicsItem</code> may be placed fully off or partially
     * of the visible area of the target <code>Canvas</code> by the
     * <code>setPosition</code> method; in this case the
     * <code>CanvasGraphicsItem</code> is just parly visible.
     * </P>
     *
     * @param x
     *            the x coordinate of the anchor point, in pixels.
     * @param y
     *            the y coordinate of the anchor point, in pixels.
     *
     * @throws IllegalStateException
     *             if a valid parent object hasn't been set.
     *
     * @see #setParent(java.lang.Object aParent)
     */
    public void setPosition(int x, int y)
    {
        synchronized (iToolkit)
        {
            checkParent();

            NativeError.check(_setPosition(getToolkitHandle(), iHandle, x, y));

            // Store current position.
            iPositionX = x;
            iPositionY = y;
        }
    }

    /**
     * <P>
     * Sets the visibility value of <code>CanvasGraphicsItem</code>. Initially
     * <code>CanvasGraphicsItem</code> is not visible so it must be explicitly
     * set to visible in order it to appear on UI.
     * </P>
     *
     * <P>
     * Setting visibility to true shows the graphics item with its content. If
     * the graphics item is already visible calling
     * <code>setVisible(true)</code> does nothing.
     * </P>
     *
     * <P>
     * Setting the visibility to false hides the graphics item and its content.
     * If the graphics item is already hidden calling
     * <code>setVisible(false)</code> does nothing.
     * </P>
     *
     * @param visible
     *            visibility of the <code>CanvasGraphicsItem</code>
     *
     * @throws IllegalStateException
     *             if a valid parent object has not been set.
     *
     * @see #setParent(java.lang.Object aParent)
     */
    public void setVisible(boolean visible)
    {
        synchronized (iToolkit)
        {
            checkParent();

            NativeError
            .check(_setVisible(getToolkitHandle(), iHandle, visible));

            iVisible = visible;
        }
    }

    /**
     * <P>
     * Sets the Z-position (elevation) of the item.
     * <p>
     * The Z-position decides the stacking order of items on the same parent. An
     * item of higher Z-position will be drawn on top of an item with a lower
     * Z-position.
     * <p>
     * The Z-position is unique for each item, meaning that changing a
     * Z-position of an item may change the Z-position of the items that share
     * the same parent. The Z-position does not affect the item's size in any
     * way.
     * <p>
     * When items are added with {@link CanvasItem#setParent} they will get a
     * Z-position that is increased by 1 from the item that is the top most item
     * at that time.
     * </P>
     *
     * @param z
     *            the Z-position of the item.
     *
     * @throws IllegalArgumentException
     *             if <code>z</code> < 0
     * @throws IllegalStateException
     *             if a valid parent object has not been set.
     *
     * @see #getZPosition()
     * @see #setParent(java.lang.Object aParent)
     */
    public void setZPosition(int z)
    {
        if (z < 0)
        {
            throw new IllegalArgumentException();
        }

        synchronized (iToolkit)
        {
            checkParent();

            NativeError.check(_setZPosition(getToolkitHandle(), iHandle, z));
        }
    }

    /**
     * <P>
     * Returns the Z-position (elevation) of the item.
     *
     * @see #setZPosition(int) </P>
     *
     * @throws IllegalStateException
     *             if a valid parent object has not been set.
     *
     * @see #setParent(java.lang.Object aParent)
     */
    public int getZPosition()
    {
        int positionZ = -1;

        synchronized (iToolkit)
        {
            checkParent();

            positionZ = _getZPosition(getToolkitHandle(), iHandle);
        }

        NativeError.check(positionZ);

        return positionZ;
    }

    /*
     * Hidden default constructor.
     */
    CanvasGraphicsItem()
    {
    }

    /**
     * <P>
     * Renders the <code>CanvasGraphicsItem</code>.
     * <P>
     * The application implements this method to paint graphics on
     * <code>CanvasGraphicsItem</code>.
     * <P>
     * <code>CanvasGraphicsItem</code> paint works in the same way as
     * <code>Canvas</code> paint method. The only difference to
     * <code>Canvas</code> paint method is that application does not need to
     * paint all the pixels of the clip region as that the clip region needing
     * updating is cleared and fully transparent before paint is called. Content
     * rendered by application in previous paint method calls is not available
     * in the clip region. Coordinates used in <code>Graphics</code> methods are
     * relative to <code>CanvasGraphicsItem</code> itself. This means (0,0)
     * coordinate is top left pixel of <code>CanvasGraphicsItem</code> as
     * positioned in the containing parent (e.g. <code>Canvas</code>).
     * </P>
     *
     * @param graphics
     *            the <code>Graphics</code> object to be used for rendering the
     *            <code>Canvas</code>
     *
     */
    protected abstract void paint(javax.microedition.lcdui.Graphics graphics);

    /**
     * Requests a repaint for the specified region of the
     * <code>CanvasGraphicsItem</code>.
     * <P>
     * Coordinates are relative to the <code>CanvasGraphicsItem</code>
     * coordinates. The method works in similar manner as <code>Canvas</code>
     * repaint method.
     *
     * @param x
     *            the x coordinate of the rectangle to be repainted, in pixels
     * @param y
     *            the y coordinate of the rectangle to be repainted, in pixels
     * @param width
     *            the width of the rectangle to be repainted, in pixels
     * @param height
     *            the height of the rectangle to be repainted, in pixels
     *
     */
    public final void repaint(int x, int y, int width, int height)
    {
        iItemPainter.Repaint(x, y, width, height);
    }

    /**
     * Requests a repaint for the entire <code>CanvasGraphicsItem</code>. The
     * effect is identical to
     * <code>repaint(0, 0, getWidth(), getHeight());</code>
     *
     * @see #repaint(int x, int y, int width, int height)
     */
    public final void repaint()
    {
        iItemPainter.Repaint(0, 0, getWidth(), getHeight());
    }

    /*
     * Disposes the Landmark native peer object, if the handles are valid.
     * Invalid (negative) handles indicate that their creation failed in the
     * first place.
     */
    final void registeredFinalize()
    {
        synchronized (iToolkit)
        {
            if (iHandle > 0)
            {
                _dispose(getToolkitHandle(), iHandle);
                iHandle = 0;
            }
        }
    }

    void Repaint(Graphics aGraphics)
    {
        paint(aGraphics);
    }

    /*
     * Returns the native side handle to the LCDUI Toolkit.
     *
     * @return The native side handle to the LCDUI Toolkit.
     */
    synchronized int getToolkitHandle()
    {
        return iToolkitInvoker.toolkitGetHandle(iToolkit);
    }

    // Native methods

    /*
     * Creates the native side peer object for this CanvasGraphicsItem.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aPainterHandle A handle of a CanvasGraphicsItemPainter native side
     * peer
     *
     * @param width The new width of this canvas graphics item.
     *
     * @param height The new hight of this canvas graphics item.
     *
     * @return A handle to the the native side peer object or a system-wide
     * error code.
     */
    private native int _createNativePeer(
        int aToolkitHandle,
        int aPainterHandle,
        int width,
        int height);

    /*
     * Disposes the native side peer object.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     */
    private native void _dispose(int aToolkitHandle, int aNativePeerHandle);

    /*
     * Sets the size of this graphics item using the given width and height.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param width The new width of this graphics item.
     *
     * @param height The new hight of this graphics item.
     *
     * @return NativeError.KErrNone if the operation was successful. Otherwise,
     * a system-wide error code is returned.
     */
    private native int _setSize(
        int aToolkitHandle,
        int aNativePeerHandle,
        int width,
        int height);

    /*
     * Disposes the native side peer object.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param aParentHandle A handle to the parent object.
     *
     * @return NativeError.KErrNone if the operation was successful. Otherwise,
     * a system-wide error code is returned.
     */
    private native int _setParent(
        int aToolkitHandle,
        int aNativePeerHandle,
        int aParentHandle);

    /*
     * Sets the graphics item visible.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param aVisible The visibility status of this graphics item.
     *
     * @return NativeError.KErrNone if the operation was successful. Otherwise,
     * a system-wide error code is returned.
     */
    private native int _setVisible(
        int aToolkitHandle,
        int aNativePeerHandle,
        boolean aVisible);

    /*
     * Sets the graphics item's position.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param x The x coordinate of the anchor point.
     *
     * @param y The y coordinate of the anchor point.
     *
     * @return NativeError.KErrNone if the operation was successful. Otherwise,
     * a system-wide error code is returned.
     */
    private native int _setPosition(
        int aToolkitHandle,
        int aNativePeerHandle,
        int x,
        int y);

    /*
     * Sets the elevation of this graphics item.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param aZ The new z-position.
     *
     * @return NativeError.KErrNone if the operation was successful. Otherwise,
     * a system-wide error code is returned.
     */
    private native int _setZPosition(
        int aToolkitHandle,
        int aNativePeerHandle,
        int aZ);

    /*
     * Gets the elevation of this graphics item.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @return The elevation if the operation was successful. Otherwise, a
     * system-wide error code is returned.
     */
    private native int _getZPosition(int aToolkitHandle, int aNativePeerHandle);
}