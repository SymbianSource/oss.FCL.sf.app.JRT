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
package org.eclipse.swt.graphics;

import org.eclipse.swt.*;

import org.eclipse.swt.internal.symbian.*;

/**
 * Class <code>GC</code> is where all of the drawing capabilities that are
 * supported by SWT are located. Instances are used to draw on either an
 * <code>Image</code>, a <code>Control</code>, or directly on a <code>Display</code>.
 * <dl>
 * <dt><b>Styles:</b></dt>
 * <dd>LEFT_TO_RIGHT, RIGHT_TO_LEFT</dd>
 * </dl>
 *
 * <p>
 * The SWT drawing coordinate system is the two-dimensional space with the origin
 * (0,0) at the top left corner of the drawing area and with (x,y) values increasing
 * to the right and downward respectively.
 * </p>
 *
 * <p>
 * Application code must explicitly invoke the <code>GC.dispose()</code>
 * method to release the operating system resources managed by each instance
 * when those instances are no longer required. This is <em>particularly</em>
 * important on Windows95 and Windows98 where the operating system has a limited
 * number of device contexts available.
 * </p>
 *
 * <p>
 * Note: Only one of LEFT_TO_RIGHT and RIGHT_TO_LEFT may be specified.
 * </p>
 *
 * @see org.eclipse.swt.events.PaintEvent
 */

public final class GC
{

    /**
     * the handle to the OS device context
     * (Warning: This field is platform dependent)
     */
    public int handle;

    Drawable drawable;
    GCData   data;
    GCBuffer  buffer;
    Color     customForeground;
    Color     customBackground;
    Rectangle clippingRect;
    int       lineStyle = -1;
    int       lineWidth = -1;
    int       xorMode = -1;

    /**
     * Prevents uninitialized instances from being created outside the package.
     */
    GC()
    {
    }

    /**
     * Constructs a new instance of this class which has been
     * configured to draw on the specified drawable. Sets the
     * foreground and background color in the GC to match those
     * in the drawable.
     * <p>
     * You must dispose the graphics context when it is no longer required.
     * </p>
     * @param drawable the drawable to draw on
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the drawable is null</li>
     *    <li>ERROR_NULL_ARGUMENT - if there is no current device</li>
     *    <li>ERROR_INVALID_ARGUMENT
     *          - if the drawable is an image that is not a bitmap or an icon
     *          - if the drawable is an image that is already selected
     *            into another graphics context</li>
     * </ul>
     * @exception SWTError <ul>
     *    <li>ERROR_NO_HANDLES if a handle could not be obtained for gc creation</li>
     * </ul>
     */
    public GC(Drawable drawable)
    {
        this(drawable, SWT.LEFT_TO_RIGHT);
    }

    /**
     * Constructs a new instance of this class which has been
     * configured to draw on the specified drawable. Sets the
     * foreground and background color in the GC to match those
     * in the drawable.
     * <p>
     * You must dispose the graphics context when it is no longer required.
     * </p>
     *
     * @param drawable the drawable to draw on
     * @param style the style of GC to construct
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the drawable is null</li>
     *    <li>ERROR_NULL_ARGUMENT - if there is no current device</li>
     *    <li>ERROR_INVALID_ARGUMENT
     *          - if the drawable is an image that is not a bitmap or an icon
     *          - if the drawable is an image that is already selected
     *            into another graphics context</li>
     * </ul>
     * @exception SWTError <ul>
     *    <li>ERROR_NO_HANDLES if a handle could not be obtained for gc creation</li>
     * </ul>
     *
     * @since 2.1.2
     */
    public GC(Drawable drawable, int style)
    {
        this(drawable, new GCData(), style);
    }

    GC(Drawable drawable, GCData data, int style)
    {
        if (drawable == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        if (drawable.internal_isDisposed()) SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        data.style = checkStyle(style);
        Device device = data.device;
        if (device == null) device = Device.getDevice();
        if (device == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        data.device = device;
        int gcHandle = drawable.internal_new_GC(data);
        try
        {
            init(drawable, data, gcHandle);
            if (device.tracking) device.new_Object(this);
        }
        catch (Error e)
        {
            drawable.internal_dispose_GC(gcHandle, data);
            throw e;
        }
    }

    public void internal_buffer_init(GCBuffer buffer)
    {
        this.buffer = buffer;
        buffer.setReserved();
    }

    public void internal_buffer_free()
    {
        buffer.setFree();
        buffer = null;
    }

    static int checkStyle(int style)
    {
        if ((style & SWT.LEFT_TO_RIGHT) != 0) style &= ~SWT.RIGHT_TO_LEFT;
        return style & (SWT.LEFT_TO_RIGHT | SWT.RIGHT_TO_LEFT);
    }

    /**
     * Copies a rectangular area of the receiver at the specified
     * position into the image, which must be of type <code>SWT.BITMAP</code>.
     *
     * @param image the image to copy into
     * @param x the x coordinate in the receiver of the area to be copied
     * @param y the y coordinate in the receiver of the area to be copied
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the image is null</li>
     *    <li>ERROR_INVALID_ARGUMENT - if the image is not a bitmap or has been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public void copyArea(Image image, int x, int y)
    {
        if (image == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        if (image.type != SWT.BITMAP) SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        if (buffer != null)
        {
            buffer.copyAreaToImage(image.handle, x, y);
            return;
        }
        OS.GC_CopyAreaToImage(data.device.handle, handle, image.handle, x, y);
    }

    /**
     * Copies a rectangular area of the receiver at the source
     * position onto the receiver at the destination position.
     *
     * @param srcX the x coordinate in the receiver of the area to be copied
     * @param srcY the y coordinate in the receiver of the area to be copied
     * @param width the width of the area to copy
     * @param height the height of the area to copy
     * @param destX the x coordinate in the receiver of the area to copy to
     * @param destY the y coordinate in the receiver of the area to copy to
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public void copyArea(int srcX, int srcY, int width, int height, int destX, int destY)
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        if (buffer != null)
        {
            buffer.copyArea(srcX, srcY, width, height, destX, destY);
            return;
        }
        OS.GC_CopyArea(data.device.handle, handle, srcX, srcY, width, height, destX, destY);
    }

    /**
     * Disposes of the operating system resources associated with
     * the graphics context. Applications must dispose of all GCs
     * which they allocate.
     */
    public void dispose()
    {
        if (handle == 0) return;
        if (data.device.isDisposed()) return;

        Device device = data.device;
        if (drawable != null) drawable.internal_dispose_GC(handle, data);
        drawable = null;
        handle = 0;
        data.image = null;
        if (device.tracking) device.dispose_Object(this);
        data.device = null;

        if (data.background != null) data.background.dispose();
        if (data.foreground != null) data.foreground.dispose();

        data = null;
    }

    /**
     * Draws the outline of a circular or elliptical arc
     * within the specified rectangular area.
     * <p>
     * The resulting arc begins at <code>startAngle</code> and extends
     * for <code>arcAngle</code> degrees, using the current color.
     * Angles are interpreted such that 0 degrees is at the 3 o'clock
     * position. A positive value indicates a counter-clockwise rotation
     * while a negative value indicates a clockwise rotation.
     * </p><p>
     * The center of the arc is the center of the rectangle whose origin
     * is (<code>x</code>, <code>y</code>) and whose size is specified by the
     * <code>width</code> and <code>height</code> arguments.
     * </p><p>
     * The resulting arc covers an area <code>width + 1</code> pixels wide
     * by <code>height + 1</code> pixels tall.
     * </p>
     *
     * @param x the x coordinate of the upper-left corner of the arc to be drawn
     * @param y the y coordinate of the upper-left corner of the arc to be drawn
     * @param width the width of the arc to be drawn
     * @param height the height of the arc to be drawn
     * @param startAngle the beginning angle
     * @param arcAngle the angular extent of the arc, relative to the start angle
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public void drawArc(int x, int y, int width, int height, int startAngle, int arcAngle)
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        if (drawable != null && (drawable.internal_isDisposed() || !drawable.internal_isVisible())) return;
        if (buffer != null)
        {
            buffer.drawArc(x, y, width, height, startAngle, arcAngle);
            return;
        }
        OS.GC_DrawArc(data.device.handle, handle, x, y, width, height, startAngle, arcAngle, false);
    }

    /**
     * Draws a rectangle, based on the specified arguments, which has
     * the appearance of the platform's <em>focus rectangle</em> if the
     * platform supports such a notion, and otherwise draws a simple
     * rectangle in the receiver's foreground color.
     *
     * @param x the x coordinate of the rectangle
     * @param y the y coordinate of the rectangle
     * @param width the width of the rectangle
     * @param height the height of the rectangle
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     *
     * @see #drawRectangle
     */
    public void drawFocus(int x, int y, int width, int height)
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        if (drawable != null && (drawable.internal_isDisposed() || !drawable.internal_isVisible())) return;
        if (buffer != null)
        {
            buffer.drawFocus(x, y, width, height);
            return;
        }
        OS.GC_DrawFocusRect(data.device.handle, handle, x, y, width, height);
    }

    /**
     * Draws the given image in the receiver at the specified
     * coordinates.
     *
     * @param image the image to draw
     * @param x the x coordinate of where to draw
     * @param y the y coordinate of where to draw
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the image is null</li>
     *    <li>ERROR_INVALID_ARGUMENT - if the image has been disposed</li>
     *    <li>ERROR_INVALID_ARGUMENT - if the given coordinates are outside the bounds of the image</li>
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     * @exception SWTError <ul>
     *    <li>ERROR_NO_HANDLES - if no handles are available to perform the operation</li>
     * </ul>
     */
    public void drawImage(Image image, int x, int y)
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        if (image == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        if (image.isDisposed()) SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        if (drawable != null && (drawable.internal_isDisposed() || !drawable.internal_isVisible())) return;
        if (buffer != null)
        {
            buffer.drawImage(image, 0, 0, -1, -1, x, y, -1, -1);
            return;
        }
        OS.GC_DrawImage(data.device.handle, handle, image.handle, 0, 0, -1, -1, x, y, -1, -1);
    }

    /**
     * Copies a rectangular area from the source image into a (potentially
     * different sized) rectangular area in the receiver. If the source
     * and destination areas are of differing sizes, then the source
     * area will be stretched or shrunk to fit the destination area
     * as it is copied. The copy fails if any part of the source rectangle
     * lies outside the bounds of the source image, or if any of the width
     * or height arguments are negative.
     *
     * @param image the source image
     * @param srcX the x coordinate in the source image to copy from
     * @param srcY the y coordinate in the source image to copy from
     * @param srcWidth the width in pixels to copy from the source
     * @param srcHeight the height in pixels to copy from the source
     * @param destX the x coordinate in the destination to copy to
     * @param destY the y coordinate in the destination to copy to
     * @param destWidth the width in pixels of the destination rectangle
     * @param destHeight the height in pixels of the destination rectangle
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the image is null</li>
     *    <li>ERROR_INVALID_ARGUMENT - if the image has been disposed</li>
     *    <li>ERROR_INVALID_ARGUMENT - if any of the width or height arguments are negative.
     *    <li>ERROR_INVALID_ARGUMENT - if the source rectangle is not contained within the bounds of the source image</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     * @exception SWTError <ul>
     *    <li>ERROR_NO_HANDLES - if no handles are available to perform the operation</li>
     * </ul>
     */
    public void drawImage(Image image, int srcX, int srcY, int srcWidth, int srcHeight, int destX, int destY, int destWidth, int destHeight)
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        if (srcWidth == 0 || srcHeight == 0 || destWidth == 0 || destHeight == 0) return;
        if (srcX < 0 || srcY < 0 || srcWidth < 0 || srcHeight < 0 || destWidth < 0 || destHeight < 0)
        {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }
        if (image == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        if (image.isDisposed()) SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        if (drawable != null && (drawable.internal_isDisposed() || !drawable.internal_isVisible())) return;
        if (buffer != null)
        {
            buffer.drawImage(image, srcX, srcY, srcWidth, srcHeight, destX, destY, destWidth, destHeight);
            return;
        }
        OS.GC_DrawImage(data.device.handle, handle, image.handle, srcX, srcY, srcWidth, srcHeight, destX, destY, destWidth, destHeight);
    }

    /**
     * Draws a line, using the foreground color, between the points
     * (<code>x1</code>, <code>y1</code>) and (<code>x2</code>, <code>y2</code>).
     *
     * @param x1 the first point's x coordinate
     * @param y1 the first point's y coordinate
     * @param x2 the second point's x coordinate
     * @param y2 the second point's y coordinate
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public void drawLine(int x1, int y1, int x2, int y2)
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        if (drawable != null && (drawable.internal_isDisposed() || !drawable.internal_isVisible())) return;
        if (buffer != null)
        {
            buffer.drawLine(x1, y1, x2, y2);
            return;
        }
        OS.GC_DrawLine(data.device.handle, handle, x1, y1, x2, y2);
    }

    /**
     * Draws the outline of an oval, using the foreground color,
     * within the specified rectangular area.
     * <p>
     * The result is a circle or ellipse that fits within the
     * rectangle specified by the <code>x</code>, <code>y</code>,
     * <code>width</code>, and <code>height</code> arguments.
     * </p><p>
     * The oval covers an area that is <code>width + 1</code>
     * pixels wide and <code>height + 1</code> pixels tall.
     * </p>
     *
     * @param x the x coordinate of the upper left corner of the oval to be drawn
     * @param y the y coordinate of the upper left corner of the oval to be drawn
     * @param width the width of the oval to be drawn
     * @param height the height of the oval to be drawn
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public void drawOval(int x, int y, int width, int height)
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        if (drawable != null && (drawable.internal_isDisposed() || !drawable.internal_isVisible())) return;
        if (buffer != null)
        {
            buffer.drawOval(x, y, width, height);
            return;
        }
        OS.GC_DrawOval(data.device.handle, handle, x, y, width, height, false);
    }

    /**
     * Draws a pixel, using the foreground color, at the specified
     * point (<code>x</code>, <code>y</code>).
     * <p>
     * Note that the receiver's line attributes do not affect this
     * operation.
     * </p>
     *
     * @param x the point's x coordinate
     * @param y the point's y coordinate
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     *
     * @since 3.0
     */
    public void drawPoint(int x, int y)
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        if (drawable != null && (drawable.internal_isDisposed() || !drawable.internal_isVisible())) return;
        if (buffer != null)
        {
            buffer.drawPoint(x, y);
            return;
        }
        OS.GC_DrawPoint(data.device.handle, handle, x, y);
    }

    /**
     * Draws the closed polygon which is defined by the specified array
     * of integer coordinates, using the receiver's foreground color. The array
     * contains alternating x and y values which are considered to represent
     * points which are the vertices of the polygon. Lines are drawn between
     * each consecutive pair, and between the first pair and last pair in the
     * array.
     *
     * @param pointArray an array of alternating x and y values which are the vertices of the polygon
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT if pointArray is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public void drawPolygon(int[] pointArray)
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        if (pointArray == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        if (drawable != null && (drawable.internal_isDisposed() || !drawable.internal_isVisible())) return;
        if (buffer != null)
        {
            buffer.drawPolygon(pointArray);
            return;
        }
        OS.GC_DrawPolygon(data.device.handle, handle, pointArray, false);
    }

    /**
     * Draws the polyline which is defined by the specified array
     * of integer coordinates, using the receiver's foreground color. The array
     * contains alternating x and y values which are considered to represent
     * points which are the corners of the polyline. Lines are drawn between
     * each consecutive pair, but not between the first pair and last pair in
     * the array.
     *
     * @param pointArray an array of alternating x and y values which are the corners of the polyline
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the point array is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public void drawPolyline(int[] pointArray)
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        if (pointArray == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        if (drawable != null && (drawable.internal_isDisposed() || !drawable.internal_isVisible())) return;
        if (buffer != null)
        {
            buffer.drawPolyline(pointArray);
            return;
        }
        OS.GC_DrawPolyline(data.device.handle, handle, pointArray);
    }

    /**
     * Draws the outline of the rectangle specified by the arguments,
     * using the receiver's foreground color. The left and right edges
     * of the rectangle are at <code>x</code> and <code>x + width</code>.
     * The top and bottom edges are at <code>y</code> and <code>y + height</code>.
     *
     * @param x the x coordinate of the rectangle to be drawn
     * @param y the y coordinate of the rectangle to be drawn
     * @param width the width of the rectangle to be drawn
     * @param height the height of the rectangle to be drawn
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public void drawRectangle(int x, int y, int width, int height)
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        if (drawable != null && (drawable.internal_isDisposed() || !drawable.internal_isVisible())) return;
        if (buffer != null)
        {
            buffer.drawRectangle(x, y, width, height);
            return;
        }
        OS.GC_DrawRectangle(data.device.handle, handle, x, y, width, height, false);
    }

    /**
     * Draws the outline of the specified rectangle, using the receiver's
     * foreground color. The left and right edges of the rectangle are at
     * <code>rect.x</code> and <code>rect.x + rect.width</code>. The top
     * and bottom edges are at <code>rect.y</code> and
     * <code>rect.y + rect.height</code>.
     *
     * @param rect the rectangle to draw
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the rectangle is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public void drawRectangle(Rectangle rect)
    {
        if (rect == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        drawRectangle(rect.x, rect.y, rect.width, rect.height);
    }

    /**
     * Draws the outline of the round-cornered rectangle specified by
     * the arguments, using the receiver's foreground color. The left and
     * right edges of the rectangle are at <code>x</code> and <code>x + width</code>.
     * The top and bottom edges are at <code>y</code> and <code>y + height</code>.
     * The <em>roundness</em> of the corners is specified by the
     * <code>arcWidth</code> and <code>arcHeight</code> arguments, which
     * are respectively the width and height of the ellipse used to draw
     * the corners.
     *
     * @param x the x coordinate of the rectangle to be drawn
     * @param y the y coordinate of the rectangle to be drawn
     * @param width the width of the rectangle to be drawn
     * @param height the height of the rectangle to be drawn
     * @param arcWidth the width of the arc
     * @param arcHeight the height of the arc
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public void drawRoundRectangle(int x, int y, int width, int height, int arcWidth, int arcHeight)
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        if (drawable != null && (drawable.internal_isDisposed() || !drawable.internal_isVisible())) return;
        if (buffer != null)
        {
            buffer.drawRoundRectangle(x, y, width, height, arcWidth, arcHeight);
            return;
        }
        OS.GC_DrawRoundRectangle(data.device.handle, handle, x, y, width, height, arcWidth, arcHeight, false);
    }

    /**
     * Draws the given string, using the receiver's current font and
     * foreground color. No tab expansion or carriage return processing
     * will be performed. The background of the rectangular area where
     * the string is being drawn will be filled with the receiver's
     * background color.
     *
     * @param string the string to be drawn
     * @param x the x coordinate of the top left corner of the rectangular area where the string is to be drawn
     * @param y the y coordinate of the top left corner of the rectangular area where the string is to be drawn
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the string is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public void drawString(String string, int x, int y)
    {
        drawText(string, x, y, 0);
    }

    /**
     * Draws the given string, using the receiver's current font and
     * foreground color. No tab expansion or carriage return processing
     * will be performed. If <code>isTransparent</code> is <code>true</code>,
     * then the background of the rectangular area where the string is being
     * drawn will not be modified, otherwise it will be filled with the
     * receiver's background color.
     *
     * @param string the string to be drawn
     * @param x the x coordinate of the top left corner of the rectangular area where the string is to be drawn
     * @param y the y coordinate of the top left corner of the rectangular area where the string is to be drawn
     * @param isTransparent if <code>true</code> the background will be transparent, otherwise it will be opaque
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the string is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public void drawString(String string, int x, int y, boolean isTransparent)
    {
        int flags = (isTransparent) ? SWT.DRAW_TRANSPARENT : 0;
        drawText(string, x, y, flags);
    }

    /**
     * Draws the given string, using the receiver's current font and
     * foreground color. Tab expansion and carriage return processing
     * are performed. The background of the rectangular area where
     * the text is being drawn will be filled with the receiver's
     * background color.
     *
     * @param string the string to be drawn
     * @param x the x coordinate of the top left corner of the rectangular area where the text is to be drawn
     * @param y the y coordinate of the top left corner of the rectangular area where the text is to be drawn
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the string is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public void drawText(String string, int x, int y)
    {
        drawText(string, x, y, SWT.DRAW_DELIMITER | SWT.DRAW_TAB);
    }

    /**
     * Draws the given string, using the receiver's current font and
     * foreground color. Tab expansion, line delimiter and mnemonic
     * processing are performed according to the specified flags. If
     * <code>flags</code> includes <code>DRAW_TRANSPARENT</code>,
     * then the background of the rectangular area where the text is being
     * drawn will not be modified, otherwise it will be filled with the
     * receiver's background color.
     * <p>
     * The parameter <code>flags</code> may be a combination of:
     * <dl>
     * <dt><b>DRAW_DELIMITER</b></dt>
     * <dd>draw multiple lines</dd>
     * <dt><b>DRAW_TAB</b></dt>
     * <dd>expand tabs</dd>
     * <dt><b>DRAW_MNEMONIC</b></dt>
     * <dd>underline the mnemonic character</dd>
     * <dt><b>DRAW_TRANSPARENT</b></dt>
     * <dd>transparent background</dd>
     * </dl>
     * </p>
     *
     * @param string the string to be drawn
     * @param x the x coordinate of the top left corner of the rectangular area where the text is to be drawn
     * @param y the y coordinate of the top left corner of the rectangular area where the text is to be drawn
     * @param flags the flags specifing how to process the text
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the string is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public void drawText(String string, int x, int y, int flags)
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        if (string == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        if (drawable != null && (drawable.internal_isDisposed() || !drawable.internal_isVisible())) return;
        if (buffer != null)
        {
            buffer.drawText(string, x, y, flags);
            return;
        }
        OS.GC_DrawText(data.device.handle, handle, string, x, y, flags);
    }

    /**
     * Compares the argument to the receiver, and returns true
     * if they represent the <em>same</em> object using a class
     * specific comparison.
     *
     * @param object the object to compare with this object
     * @return <code>true</code> if the object is the same as this object and <code>false</code> otherwise
     *
     * @see #hashCode
     */
    public boolean equals(Object object)
    {
        return (object == this) || ((object instanceof GC) && (handle == ((GC)object).handle));
    }

    /**
     * Fills the interior of a circular or elliptical arc within
     * the specified rectangular area, with the receiver's background
     * color.
     * <p>
     * The resulting arc begins at <code>startAngle</code> and extends
     * for <code>arcAngle</code> degrees, using the current color.
     * Angles are interpreted such that 0 degrees is at the 3 o'clock
     * position. A positive value indicates a counter-clockwise rotation
     * while a negative value indicates a clockwise rotation.
     * </p><p>
     * The center of the arc is the center of the rectangle whose origin
     * is (<code>x</code>, <code>y</code>) and whose size is specified by the
     * <code>width</code> and <code>height</code> arguments.
     * </p><p>
     * The resulting arc covers an area <code>width + 1</code> pixels wide
     * by <code>height + 1</code> pixels tall.
     * </p>
     *
     * @param x the x coordinate of the upper-left corner of the arc to be filled
     * @param y the y coordinate of the upper-left corner of the arc to be filled
     * @param width the width of the arc to be filled
     * @param height the height of the arc to be filled
     * @param startAngle the beginning angle
     * @param arcAngle the angular extent of the arc, relative to the start angle
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     *
     * @see #drawArc
     */
    public void fillArc(int x, int y, int width, int height, int startAngle, int arcAngle)
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        if (drawable != null && (drawable.internal_isDisposed() || !drawable.internal_isVisible())) return;
        if (buffer != null)
        {
            buffer.fillArc(x, y, width, height, startAngle, arcAngle);
            return;
        }
        OS.GC_DrawArc(data.device.handle, handle, x, y, width, height, startAngle, arcAngle, true);
    }

    /**
     * Fills the interior of the specified rectangle with a gradient
     * sweeping from left to right or top to bottom progressing
     * from the receiver's foreground color to its background color.
     *
     * @param x the x coordinate of the rectangle to be filled
     * @param y the y coordinate of the rectangle to be filled
     * @param width the width of the rectangle to be filled, may be negative
     *        (inverts direction of gradient if horizontal)
     * @param height the height of the rectangle to be filled, may be negative
     *        (inverts direction of gradient if vertical)
     * @param vertical if true sweeps from top to bottom, else
     *        sweeps from left to right
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     *
     * @see #drawRectangle
     */
    public void fillGradientRectangle(int x, int y, int width, int height, boolean vertical)
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        if (width == 0 || height == 0) return;
        if (drawable != null && (drawable.internal_isDisposed() || !drawable.internal_isVisible())) return;
        if (buffer != null)
        {
            buffer.fillGradientRectangle(x, y, width, height, vertical);
            return;
        }
        OS.GC_FillGradientRectangle(data.device.handle, handle, x, y, width, height, vertical);
    }

    /**
     * Fills the interior of an oval, within the specified
     * rectangular area, with the receiver's background
     * color.
     *
     * @param x the x coordinate of the upper left corner of the oval to be filled
     * @param y the y coordinate of the upper left corner of the oval to be filled
     * @param width the width of the oval to be filled
     * @param height the height of the oval to be filled
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     *
     * @see #drawOval
     */
    public void fillOval(int x, int y, int width, int height)
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        if (drawable != null && (drawable.internal_isDisposed() || !drawable.internal_isVisible())) return;
        if (buffer != null)
        {
            buffer.fillOval(x, y, width, height);
            return;
        }
        OS.GC_DrawOval(data.device.handle, handle, x, y, width, height, true);
    }

    /**
     * Fills the interior of the closed polygon which is defined by the
     * specified array of integer coordinates, using the receiver's
     * background color. The array contains alternating x and y values
     * which are considered to represent points which are the vertices of
     * the polygon. Lines are drawn between each consecutive pair, and
     * between the first pair and last pair in the array.
     *
     * @param pointArray an array of alternating x and y values which are the vertices of the polygon
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT if pointArray is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     *
     * @see #drawPolygon
     */
    public void fillPolygon(int[] pointArray)
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        if (pointArray == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        if (drawable != null && (drawable.internal_isDisposed() || !drawable.internal_isVisible())) return;
        if (buffer != null)
        {
            buffer.fillPolygon(pointArray);
            return;
        }
        OS.GC_DrawPolygon(data.device.handle, handle, pointArray, true);
    }

    /**
     * Fills the interior of the rectangle specified by the arguments,
     * using the receiver's background color.
     *
     * @param x the x coordinate of the rectangle to be filled
     * @param y the y coordinate of the rectangle to be filled
     * @param width the width of the rectangle to be filled
     * @param height the height of the rectangle to be filled
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     *
     * @see #drawRectangle
     */
    public void fillRectangle(int x, int y, int width, int height)
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        if (drawable != null && (drawable.internal_isDisposed() || !drawable.internal_isVisible())) return;
        if (buffer != null)
        {
            buffer.fillRectangle(x, y, width, height);
            return;
        }
        OS.GC_DrawRectangle(data.device.handle, handle, x, y, width, height, true);
    }

    /**
     * Fills the interior of the specified rectangle, using the receiver's
     * background color.
     *
     * @param rect the rectangle to be filled
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the rectangle is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     *
     * @see #drawRectangle
     */
    public void fillRectangle(Rectangle rect)
    {
        if (rect == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        fillRectangle(rect.x, rect.y, rect.width, rect.height);
    }

    /**
     * Fills the interior of the round-cornered rectangle specified by
     * the arguments, using the receiver's background color.
     *
     * @param x the x coordinate of the rectangle to be filled
     * @param y the y coordinate of the rectangle to be filled
     * @param width the width of the rectangle to be filled
     * @param height the height of the rectangle to be filled
     * @param arcWidth the width of the arc
     * @param arcHeight the height of the arc
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     *
     * @see #drawRoundRectangle
     */
    public void fillRoundRectangle(int x, int y, int width, int height, int arcWidth, int arcHeight)
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        if (drawable != null && (drawable.internal_isDisposed() || !drawable.internal_isVisible())) return;
        if (buffer != null)
        {
            buffer.fillRoundRectangle(x, y, width, height, arcWidth, arcHeight);
            return;
        }
        OS.GC_DrawRoundRectangle(data.device.handle, handle, x, y, width, height, arcWidth, arcHeight, true);
    }

    /**
     * Returns the <em>advance width</em> of the specified character in
     * the font which is currently selected into the receiver.
     * <p>
     * The advance width is defined as the horizontal distance the cursor
     * should move after printing the character in the selected font.
     * </p>
     *
     * @param ch the character to measure
     * @return the distance in the x direction to move past the character before painting the next
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public int getAdvanceWidth(char ch)
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        return OS.GC_GetAdvanceWidth(data.device.handle, handle, ch);
    }

    /**
     * Returns the receiver's alpha value.
     *
     * @return the alpha value
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     *
     * @since 3.1
     */
    public int getAlpha()
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        return data.alpha;
    }

    /**
     * Returns the background color.
     *
     * @return the receiver's background color
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public Color getBackground()
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        return customBackground != null ? customBackground : data.background;
    }

    /**
     * Returns the width of the specified character in the font
     * selected into the receiver.
     * <p>
     * The width is defined as the space taken up by the actual
     * character, not including the leading and tailing whitespace
     * or overhang.
     * </p>
     *
     * @param ch the character to measure
     * @return the width of the character
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public int getCharWidth(char ch)
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        return OS.GC_GetCharWidth(data.device.handle, handle, ch);
    }

    /**
     * Returns the bounding rectangle of the receiver's clipping
     * region. If no clipping region is set, the return value
     * will be a rectangle which covers the entire bounds of the
     * object the receiver is drawing on.
     *
     * @return the bounding rectangle of the clipping region
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public Rectangle getClipping()
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        if (clippingRect == null)
        {
            clippingRect = OS.GC_GetClippingRect(data.device.handle, handle);
        }
        return new Rectangle(clippingRect.x, clippingRect.y, clippingRect.width, clippingRect.height);
    }

    /**
     * Returns the font currently being used by the receiver
     * to draw and measure text.
     *
     * @return the receiver's font
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public Font getFont()
    {
        if (handle == 0) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        int hFont = OS.GC_GetFont(data.device.handle, handle);
        // Returned font should not be disposed by user so we set it as system.
        // In case the font was originally allocated by user, a reference to the font should be
        // kept by user when allocating the font and font should be disposed through this reference,
        // not through GC.getFont.
        return Font.internal_newSystemFont(data.device, hFont);
    }

    /**
     * Returns a FontMetrics which contains information
     * about the font currently being used by the receiver
     * to draw and measure text.
     *
     * @return font metrics for the receiver's font
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public FontMetrics getFontMetrics()
    {
        if (handle == 0) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);

        int data[] = OS.GC_GetFontMetrics(this.data.device.handle, handle);

        FontMetrics fontMetrics = new FontMetrics();
        fontMetrics.ascent = data[0];
        fontMetrics.averageCharWidth = data[1];
        fontMetrics.descent = data[2];
        fontMetrics.height = data[3];
        fontMetrics.leading = data[4];
        return fontMetrics;
    }

    /**
     * Returns the receiver's foreground color.
     *
     * @return the color used for drawing foreground things
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public Color getForeground()
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        return customForeground != null ? customForeground : data.foreground;
    }

    /**
     * Returns the receiver's line style, which will be one
     * of the constants <code>SWT.LINE_SOLID</code>, <code>SWT.LINE_DASH</code>,
     * <code>SWT.LINE_DOT</code>, <code>SWT.LINE_DASHDOT</code> or
     * <code>SWT.LINE_DASHDOTDOT</code>.
     *
     * @return the style used for drawing lines
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public int getLineStyle()
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        if (lineStyle == -1)
        {
            lineStyle = OS.GC_GetLineStyle(data.device.handle, handle);
        }
        return lineStyle;
    }

    /**
     * Returns the width that will be used when drawing lines
     * for all of the figure drawing operations (that is,
     * <code>drawLine</code>, <code>drawRectangle</code>,
     * <code>drawPolyline</code>, and so forth.
     *
     * @return the receiver's line width
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public int getLineWidth()
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        if (lineWidth == -1)
        {
            lineWidth = OS.GC_GetLineWidth(data.device.handle, handle);
        }
        return lineWidth;
    }

    /**
     * Returns the receiver's style information.
     * <p>
     * Note that the value which is returned by this method <em>may
     * not match</em> the value which was provided to the constructor
     * when the receiver was created. This can occur when the underlying
     * operating system does not support a particular combination of
     * requested styles.
     * </p>
     *
     * @return the style bits
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     *
     * @since 2.1.2
     */
    public int getStyle()
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        return data.style;
    }

    /**
     * Returns <code>true</code> if this GC is drawing in the mode
     * where the resulting color in the destination is the
     * <em>exclusive or</em> of the color values in the source
     * and the destination, and <code>false</code> if it is
     * drawing in the mode where the destination color is being
     * replaced with the source color value.
     *
     * @return <code>true</code> true if the receiver is in XOR mode, and false otherwise
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public boolean getXORMode()
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        if (xorMode == -1)
        {
            boolean xor = OS.GC_GetXORMode(data.device.handle, handle);
            if (xor)
            {
                xorMode = 1;
            }
            else
            {
                xorMode = 0;
            }
        }
        return (xorMode == 1);
    }

    /**
     * Returns an integer hash code for the receiver. Any two
     * objects which return <code>true</code> when passed to
     * <code>equals</code> must return the same value for this
     * method.
     *
     * @return the receiver's hash
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     *
     * @see #equals
     */
    public int hashCode()
    {
        return handle;
    }
    void init(Drawable drawable, GCData data, int gcHandle)
    {
        if (data.foreground != null)
        {
            OS.GC_SetForeground(data.device.handle, handle, data.foreground.handle);
        }
        else
        {
            data.foreground = new Color(data.device, 0, 0, 0);
        }

        if (data.background != null)
        {
            OS.GC_SetBackground(data.device.handle, handle, data.background.handle);
        }
        else
        {
            data.background = new Color(data.device, 255, 255, 255);
        }

        this.drawable = drawable;
        this.data     = data;
        handle        = gcHandle;
        this.setFont(null); // gets the system font
    }


    /**
     * Returns <code>true</code> if the receiver has a clipping
     * region set into it, and <code>false</code> otherwise.
     * If this method returns false, the receiver will draw on all
     * available space in the destination. If it returns true,
     * it will draw only in the area that is covered by the region
     * that can be accessed with <code>getClipping(region)</code>.
     *
     * @return <code>true</code> if the GC has a clipping region, and <code>false</code> otherwise
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public boolean isClipped()
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        return OS.GC_IsClipped(data.device.handle, handle);
    }

    /**
     * Returns <code>true</code> if the GC has been disposed,
     * and <code>false</code> otherwise.
     * <p>
     * This method gets the dispose state for the GC.
     * When a GC has been disposed, it is an error to
     * invoke any other method using the GC.
     *
     * @return <code>true</code> when the GC is disposed and <code>false</code> otherwise
     */
    public boolean isDisposed()
    {
        return handle == 0;
    }

    /**
     * Sets the receiver's alpha value.
     *
     * @param alpha the alpha value
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     *
     * @since 3.1
     */
    public void setAlpha(int alpha)
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        int alphaValue = alpha & 0xFF;
        data.alpha = alphaValue;
        if (buffer != null)
        {
            buffer.setAlpha(alpha);
        }
        else
        {
            OS.GC_SetAlpha(data.device.handle, handle, alphaValue);
        }
    }

    /**
     * Sets the background color. The background color is used
     * for fill operations and as the background color when text
     * is drawn.
     *
     * @param color the new background color for the receiver
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the color is null</li>
     *    <li>ERROR_INVALID_ARGUMENT - if the color has been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public void setBackground(Color color)
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        if (color == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        if (color.isDisposed()) SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        customBackground = color;
        if (buffer != null)
        {
            buffer.setBackground(color);
        }
        else
        {
            OS.GC_SetBackground(data.device.handle, handle, color.handle);
        }
    }

    /**
     * Sets the area of the receiver which can be changed
     * by drawing operations to the rectangular area specified
     * by the arguments.
     *
     * @param x the x coordinate of the clipping rectangle
     * @param y the y coordinate of the clipping rectangle
     * @param width the width of the clipping rectangle
     * @param height the height of the clipping rectangle
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public void setClipping(int x, int y, int width, int height)
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        clippingRect = new Rectangle(x, y, width, height);
        if (buffer != null)
        {
            buffer.setClipping(x, y, width, height);
        }
        else
        {
            OS.GC_SetClippingRect(data.device.handle, handle, x, y, width, height);
        }
    }

    /**
     * Sets the area of the receiver which can be changed
     * by drawing operations to the rectangular area specified
     * by the argument.
     *
     * @param rect the clipping rectangle
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public void setClipping(Rectangle rect)
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        if (rect != null)
        {
            setClipping(rect.x, rect.y, rect.width, rect.height);
        }
        else
        {
            OS.GC_CancelClipping(data.device.handle, handle);
            clippingRect = null;
            if (buffer != null)
            {
                buffer.cancelClipping();
            }
        }
    }

    /**
     * Sets the font which will be used by the receiver
     * to draw and measure text to the argument. If the
     * argument is null, then a default font appropriate
     * for the platform will be used instead.
     *
     * @param font the new font for the receiver, or null to indicate a default font
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_ARGUMENT - if the font has been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */

    public void setFont(Font font)
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        int hFont = 0;
        if (font == null)
        {
            hFont = Font.internal_newSystemFont(data.device, OS.Device_GetSystemFont(data.device.handle)).handle;
        }
        else
        {
            if (font.isDisposed()) SWT.error(SWT.ERROR_INVALID_ARGUMENT);
            hFont = font.handle;
        }

        OS.GC_SetFont(data.device.handle, handle, hFont);
        if (buffer != null)
        {
            buffer.setFont(font);
        }
    }

    /**
     * Sets the foreground color. The foreground color is used
     * for drawing operations including when text is drawn.
     *
     * @param color the new foreground color for the receiver
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the color is null</li>
     *    <li>ERROR_INVALID_ARGUMENT - if the color has been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public void setForeground(Color color)
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        if (color == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        if (color.isDisposed()) SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        customForeground = color;
        if (buffer != null)
        {
            buffer.setForeground(color);
        }
        else
        {
            OS.GC_SetForeground(data.device.handle, handle, color.handle);
        }
    }

    /**
     * Sets the receiver's line style to the argument, which must be one
     * of the constants <code>SWT.LINE_SOLID</code>, <code>SWT.LINE_DASH</code>,
     * <code>SWT.LINE_DOT</code>, <code>SWT.LINE_DASHDOT</code> or
     * <code>SWT.LINE_DASHDOTDOT</code>.
     *
     * @param lineStyle the style to be used for drawing lines
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public void setLineStyle(int lineStyle)
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        this.lineStyle = lineStyle;
        if (buffer != null)
        {
            buffer.setLineStyle(lineStyle);
        }
        else
        {
            OS.GC_SetLineStyle(data.device.handle, handle, lineStyle);
        }
    }

    /**
     * Sets the width that will be used when drawing lines
     * for all of the figure drawing operations (that is,
     * <code>drawLine</code>, <code>drawRectangle</code>,
     * <code>drawPolyline</code>, and so forth.
     *
     * @param lineWidth the width of a line
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public void setLineWidth(int lineWidth)
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        this.lineWidth = lineWidth;
        if (buffer != null)
        {
            buffer.setLineWidth(lineWidth);
        }
        else
        {
            OS.GC_SetLineWidth(data.device.handle, handle, lineWidth);
        }
    }

    /**
     * If the argument is <code>true</code>, puts the receiver
     * in a drawing mode where the resulting color in the destination
     * is the <em>exclusive or</em> of the color values in the source
     * and the destination, and if the argument is <code>false</code>,
     * puts the receiver in a drawing mode where the destination color
     * is replaced with the source color value.
     *
     * @param xor if <code>true</code>, then <em>xor</em> mode is used, otherwise <em>source copy</em> mode is used
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public void setXORMode(boolean xor)
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        xorMode = xor ? 1 : 0;
        if (buffer != null)
        {
            buffer.setXORMode(xor);
        }
        else
        {
            OS.GC_SetXORMode(data.device.handle, handle, xor);
        }
    }

    /**
     * Returns the extent of the given string. No tab
     * expansion or carriage return processing will be performed.
     * <p>
     * The <em>extent</em> of a string is the width and height of
     * the rectangular area it would cover if drawn in a particular
     * font (in this case, the current font in the receiver).
     * </p>
     *
     * @param string the string to measure
     * @return a point containing the extent of the string
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the string is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public Point stringExtent(String string)
    {
        return textExtent(string, 0);
    }

    /**
     * Returns the extent of the given string. Tab expansion and
     * carriage return processing are performed.
     * <p>
     * The <em>extent</em> of a string is the width and height of
     * the rectangular area it would cover if drawn in a particular
     * font (in this case, the current font in the receiver).
     * </p>
     *
     * @param string the string to measure
     * @return a point containing the extent of the string
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the string is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public Point textExtent(String string)
    {
        return textExtent(string, SWT.DRAW_DELIMITER | SWT.DRAW_TAB);
    }

    /**
     * Returns the extent of the given string. Tab expansion, line
     * delimiter and mnemonic processing are performed according to
     * the specified flags, which can be a combination of:
     * <dl>
     * <dt><b>DRAW_DELIMITER</b></dt>
     * <dd>draw multiple lines</dd>
     * <dt><b>DRAW_TAB</b></dt>
     * <dd>expand tabs</dd>
     * <dt><b>DRAW_MNEMONIC</b></dt>
     * <dd>underline the mnemonic character</dd>
     * <dt><b>DRAW_TRANSPARENT</b></dt>
     * <dd>transparent background</dd>
     * </dl>
     * <p>
     * The <em>extent</em> of a string is the width and height of
     * the rectangular area it would cover if drawn in a particular
     * font (in this case, the current font in the receiver).
     * </p>
     *
     * @param string the string to measure
     * @param flags the flags specifing how to process the text
     * @return a point containing the extent of the string
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the string is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public Point textExtent(String string, int flags)
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        if (string == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        return OS.GC_TextExtent(data.device.handle, handle, string, flags);
    }

    /**
     * Returns a string containing a concise, human-readable
     * description of the receiver.
     *
     * @return a string representation of the receiver
     */
    public String toString()
    {
        if (isDisposed()) return "GC {*DISPOSED*}";
        return "GC {" + handle + "}";
    }

    /**
     * Method used to draw M2G images using our buffered GC.
     *
     * @param bitmapHandles the handles to the bitmap and mask bitmap
     * @param renderData[0] contains other information about the image:
     *      renderData[0] = destX       the x coordinate in the destination to copy to
     *      renderData[1] = destY       the y coordinate in the destination to copy to
     *      renderData[2] = srcX        the x coordinate in the source image to copy from
     *      renderData[3] = srcY        the y coordinate in the source image to copy from
     *      renderData[4] = srcWidth        the width in pixels to copy from the source
     *      renderData[5] = srcHeight       the height in pixels to copy from the source

     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_ARGUMENT - if any of the width or height arguments are negative.
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     * @exception SWTError <ul>
     *    <li>ERROR_NO_HANDLES - if no handles are available to perform the operation</li>
     * </ul>
     */

    public void internal_drawM2GImage(int[] bitmapHandles, int[] renderData, int useNativeClear)
    {

        int destX = renderData[0];
        int destY = renderData[1];
        int srcX = renderData[2];
        int srcY = renderData[3];
        int srcWidth = renderData[4];
        int srcHeight = renderData[5];

        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        if (srcWidth == 0 || srcHeight == 0) return;
        if (srcX < 0 || srcY < 0 || srcWidth < 0 || srcHeight < 0)
        {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }
        if (bitmapHandles[0] == 0) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        if (drawable != null && (drawable.internal_isDisposed() || !drawable.internal_isVisible())) return;
        if (buffer != null)
        {
            buffer.drawM2GImage(bitmapHandles, srcX, srcY, srcWidth, srcHeight, destX, destY, srcWidth, srcHeight, useNativeClear);
            return;
        }
        OS.GC_DrawM2GImage(data.device.handle, handle, bitmapHandles, srcX, srcY, srcWidth, srcHeight, destX, destY, srcWidth, srcHeight, useNativeClear);
    }
}
