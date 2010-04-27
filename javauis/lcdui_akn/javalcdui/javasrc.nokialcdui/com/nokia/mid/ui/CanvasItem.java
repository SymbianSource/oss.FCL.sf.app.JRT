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
 * Description:  Public class defining the interface for MIDP CanvasItem
 *
 */

// PACKAGE
package com.nokia.mid.ui;

/**
 * Parent object just for components on Canvas. Currently there are two
 * components: <code>CanvasGraphicsItem</code> and <code>TextEditor</code>.
 *
 * Methods of CanvasItem are implemented in subclasses
 * <code>CanvasGraphicsItem</code> and <code>TextEditor</code>.
 *
 * @since 1.4
 */

public abstract class CanvasItem
{
    // Protected data for extending classes.

    // Parent object
    Object iParent;

    // Size of this canvas item component in pixels.
    int iHeight;
    int iWidth;

    // The current position of this canvas item component.
    int iPositionX;
    int iPositionY;

    // Visibility value of CanvasItem
    boolean iVisible;

    // Error strings
    static final String ERROR_NOT_A_VALID_PARENT_OBJECT =
        "The object is not a valid parent object";
    static final String ERROR_SETTING_PARENT_OBJECT =
        "Error setting the parent object";
    static final String ERROR_GIVEN_ARGUMENTS_NOT_VALID =
        "The given arguments are not valid";
    static final String ERROR_NO_PARENT = "Parent has not been set";

    /**
     * Set the parent object of this <code>CanvasItem</code>.
     * <P>
     * Typically the parent object would be <code>Canvas</code>. Setting the
     * parameter to null removes the association to the parent. If
     * <code>setParent(null)</code> is called for a <code>CanvasItem</code> yet
     * not having any parent or <code>setParent(parent)</code> is called with
     * the same parent, the call is silently ignored.
     * <P>
     *
     * @param parent
     *            the parent object
     *
     * @throws IllegalArgumentException
     *             if <code>parent</code> is not a valid object with which a
     *             <code>CanvasItem</code> can be associated, or if CanvasItem
     *             is already set to another another parent
     */
    public void setParent(Object parent)
    {
    }

    /**
     * Get the parent object of this <code>CanvasItem</code> or null, if no
     * parent set.
     *
     * @return Object parent object or null
     *
     * @see #setParent(java.lang.Object parent )
     */
    public synchronized Object getParent()
    {
        return iParent;
    }

    /**
     * Sets the size of this <code>CanvasItem</code> in pixels.
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
    }

    /**
     * Gets the height of this <code>CanvasItem</code> in pixels.
     *
     * @return height in pixels
     */
    public int getHeight()
    {
        // JDEBUG( "CanvasItem.getHeight() +, iHeight=" + iHeight );

        return iHeight;
    }

    /**
     * Gets the width of this <code>CanvasItem</code> in pixels.
     *
     * @return width in pixels
     */
    public int getWidth()
    {
        // JDEBUG( "CanvasItem.getWidth() +, iWidth=" + iWidth );

        return iWidth;
    }

    /**
     * <P>
     * Sets the rendering position of this <code>CanvasItem</code>. The origin
     * is relative to the coordinate system of parent.
     * </P>
     *
     * <P>
     * The <code>CanvasItem</code> may be placed fully off or partially of the
     * visible area of the parent by the <code>setPosition</code> method; in
     * this case the <code>CanvasItem</code> is just partly visible.
     * </P>
     *
     * @param x
     *            the x coordinate of the anchor point, in pixels.
     * @param y
     *            the y coordinate of the anchor point, in pixels.
     */
    public void setPosition(int x, int y)
    {
    }

    /**
     * <P>
     * Gets the rendering position of this <code>CanvasItem</code>.
     * </P>
     *
     * <P>
     * The origin is relative to the coordinate system of parent.
     * </P>
     *
     * @return X location of the CanvasItem
     */
    public int getPositionX()
    {
        return iPositionX;
    }

    /**
     * <P>
     * Sets the rendering position of this <code>CanvasItem</code>.
     * </P>
     *
     * <P>
     * The origin is relative to the coordinate system of parent.
     * </P>
     *
     * <P>
     * The <code>CanvasItem</code> may be placed partially or fully off of the
     * visible area of the parent by the <code>setPosition</code> method; in
     * this case the CanvasItem is just not fully visible.
     * </P>
     *
     * @return Y location of the CanvasItem
     */
    public int getPositionY()
    {
        return iPositionY;
    }

    /**
     * <P>
     * Sets the visibility value of <code>CanvasItem</code>.
     * </P>
     *
     * Initially <code>CanvasItem</code> is not visible so it must be explicitly
     * set to visible in order it to appear on UI. </P>
     *
     * <P>
     * If the item is already visible calling <code>setVisible(true)</code> does
     * nothing. If the item is not visible calling
     * <code>setVisible(false)</code> does nothing.
     * </P>
     *
     * @param visible
     *            visibility of the <code>CanvasItem</code>
     *
     * @throws IllegalStateException
     *             If the item is not added to any parent
     *
     */
    public void setVisible(boolean visible)
    {
    }

    /**
     * <P>
     * Returns the visibility value of <code>CanvasItem</code>.
     * </P>
     *
     * <p>
     * Initially <code>CanvasItem</code> is not visible so it must be explicitly
     * set to visible in order it to appear on UI.
     * </P>
     *
     * @return true if currently visible, false otherwise
     *
     * @see #setVisible(boolean visible )
     */
    public boolean isVisible()
    {
        return iVisible;
    }

    /**
     * <P>
     * Sets the Z-position, or the elevation, of the item.
     * <p>
     * The elevation decides the stacking order of neighboring items. An item of
     * high Z-position will be drawn on top of an item with a lower Z-position
     * if they share the same parent item.
     * <p>
     * The z-position is unique for each item meaning that changing a Z-position
     * of an item may change the Z-position of the items that share the same
     * parent item. The Z-position does not affect the item's size in any way.
     * <p>
     * When items are added with {@link CanvasItem#setParent} they will get a
     * Z-position that is increased by 1 from the item that is the top most item
     * at that time.
     * </P>
     *
     * @param z
     *            the z position of the item.
     *
     * @throws IllegalArgumentException
     *             if <code>z</code> < 0
     *
     * @see #getZPosition()
     *
     */
    public void setZPosition(int z)
    {
    }

    /**
     * <P>
     * Returns the Z-position, or the elevation, of the item. The Z-position
     * decides the stacking order of neighboring items.
     * </P>
     *
     * @return The Z-position, or the elevation, of the item.
     *
     * @see #setZPosition(int)
     *
     */
    public int getZPosition()
    {
        return 0;
    }

    /*
     * Package private contructor.
     */
    CanvasItem()
    {
    }

    /*
     * Checks if the parent for this canvas item has been set.
     *
     * @throws java.lang.IllegalStateException if parent has not been set.
     */
    void checkParent()
    {
        if (iParent == null)
        {
            throw new IllegalStateException(ERROR_NO_PARENT);
        }
    }
}

// End of file
