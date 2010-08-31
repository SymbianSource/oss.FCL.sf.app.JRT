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
package org.eclipse.swt.graphics;

import org.eclipse.swt.SWT;
import org.eclipse.swt.internal.qt.GCData;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.internal.qt.graphics.GraphicsContext;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;

/**
 * Class <code>GC</code> is where all of the drawing capabilities that are
 * supported by SWT are located. Instances are used to draw on either an
 * <code>Image</code>, a <code>Control</code>, or directly on a
 * <code>Display</code>.
 * <dl>
 * <dt><b>Styles:</b></dt>
 * <dd>LEFT_TO_RIGHT, RIGHT_TO_LEFT</dd>
 * </dl>
 * 
 * <p>
 * The SWT drawing coordinate system is the two-dimensional space with the
 * origin (0,0) at the top left corner of the drawing area and with (x,y) values
 * increasing to the right and downward respectively.
 * </p>
 * 
 * <p>
 * Application code must explicitly invoke the <code>GC.dispose()</code> method
 * to release the operating system resources managed by each instance when those
 * instances are no longer required. This is <em>particularly</em> important on
 * Windows95 and Windows98 where the operating system has a limited number of
 * device contexts available.
 * </p>
 * 
 * <p>
 * Note: Only one of LEFT_TO_RIGHT and RIGHT_TO_LEFT may be specified.
 * </p>
 * 
 * @see org.eclipse.swt.events.PaintEvent
 */
public final class GC {

// FontMetrics
private final static int FM_ASCENT = 0;
private final static int FM_AVERAGE_CHAR_WIDTH = 1;
private final static int FM_DESCENT = 2;
private final static int FM_HEIGHT = 3;
private final static int FM_LEADING = 4;
private final static int FM_DATA_COUNT = 5;

// Configuration flags
final static int FOREGROUND = 1 << 0;
final static int BACKGROUND = 1 << 1;
final static int FONT = 1 << 2;
final static int LINE_STYLE = 1 << 3;
final static int LINE_CAP = 1 << 4;
final static int LINE_JOIN = 1 << 5;
final static int LINE_WIDTH = 1 << 6;
final static int LINE_MITERLIMIT = 1 << 7;
final static int BACKGROUND_BG = 1 << 8;
final static int DRAW_OFFSET = 1 << 9;
final static int DRAW = FOREGROUND | LINE_WIDTH | LINE_STYLE | LINE_CAP
        | LINE_JOIN | LINE_MITERLIMIT | DRAW_OFFSET;
final static int FILL = BACKGROUND;

static int checkStyle(int style) {
    if ((style & SWT.LEFT_TO_RIGHT) != 0) {
        style &= ~SWT.RIGHT_TO_LEFT;
    }
    return style & (SWT.LEFT_TO_RIGHT | SWT.RIGHT_TO_LEFT);
}

int handle;
GCData data;
Color customForeground;
Color customBackground;

Drawable drawable;

/**
 * Prevents uninitialized instances from being created outside the package.
 */
GC() {
}

/**
 * Constructs a new instance of this class which has been configured to draw on
 * the specified drawable. Sets the foreground color, background color and font
 * in the GC to match those in the drawable.
 * <p>
 * You must dispose the graphics context when it is no longer required.
 * </p>
 * 
 * @param drawable
 *            the drawable to draw on
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the drawable is null</li>
 *                <li>ERROR_NULL_ARGUMENT - if there is no current device</li>
 *                <li>ERROR_INVALID_ARGUMENT - if the drawable is an image that
 *                is not a bitmap or an icon - if the drawable is an image or
 *                printer that is already selected into another graphics context
 *                </li>
 *                </ul>
 * @exception SWTError
 *                <ul>
 *                <li>ERROR_NO_HANDLES if a handle could not be obtained for GC
 *                creation</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS if not called from the thread
 *                that created the drawable</li>
 *                </ul>
 */
public GC(Drawable drawable) {
    this(drawable, 0);
}

/**
 * Constructs a new instance of this class which has been configured to draw on
 * the specified drawable. Sets the foreground color, background color and font
 * in the GC to match those in the drawable.
 * <p>
 * You must dispose the graphics context when it is no longer required.
 * </p>
 * 
 * @param drawable
 *            the drawable to draw on
 * @param style
 *            the style of GC to construct
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the drawable is null</li>
 *                <li>ERROR_NULL_ARGUMENT - if there is no current device</li>
 *                <li>ERROR_INVALID_ARGUMENT - if the drawable is an image that
 *                is not a bitmap or an icon - if the drawable is an image or
 *                printer that is already selected into another graphics context
 *                </li>
 *                </ul>
 * @exception SWTError
 *                <ul>
 *                <li>ERROR_NO_HANDLES if a handle could not be obtained for GC
 *                creation</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS if not called from the thread
 *                that created the drawable</li>
 *                </ul>
 * 
 */
public GC(Drawable drawable, int style) {
    if (drawable == null) {
        SWT.error(SWT.ERROR_NULL_ARGUMENT);
    }

    GCData data = new GCData();
    data.style = checkStyle(style);

    int gcHandle = drawable.internal_new_GC(data);
    try {
        init(drawable, data, gcHandle);
    } catch (Error e) {
        drawable.internal_dispose_GC(data);
        throw e;
    }
}

/*
 * Here it's checked based on the data.state flags which gc attributes need to
 * be configured. State flags are set when configuration is up-to-date and
 * cleared when configuration is invalidated.
 */
void checkGC(int mask) {
    int state = data.state;
    if ((state & mask) == mask)
        return;
    state = (state ^ mask) & mask; // take out bits not in mask
    data.state |= mask;

    if((data.state & FOREGROUND) != 0) {
        if ((data.style & SWT.MIRRORED) != 0) {
            int width = OS.QPaintDevice_width(data.drawable);
            data.internalGc.resetTransform();
            data.internalGc.translate(width - 1, 0);
            data.internalGc.scale(-1, 1);
        }
    }
}

/**
 * Copies a rectangular area of the receiver at the specified position into the
 * image, which must be of type <code>SWT.BITMAP</code>.
 * 
 * @param image
 *            the image to copy into
 * @param x
 *            the x coordinate in the receiver of the area to be copied
 * @param y
 *            the y coordinate in the receiver of the area to be copied
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the image is null</li>
 *                <li>ERROR_INVALID_ARGUMENT - if the image is not a bitmap or
 *                has been disposed</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public void copyArea(Image image, int x, int y) {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    if (image == null) {
        SWT.error(SWT.ERROR_NULL_ARGUMENT);
    }
    if (image.type != SWT.BITMAP || image.isDisposed()) {
        SWT.error(SWT.ERROR_INVALID_ARGUMENT);
    }

    // Reset all mirroring transformation and calculate the mirrored coordinates
    // manually.
    if ((data.style & SWT.MIRRORED) != 0) {
        data.internalGc.resetTransform();
        data.state = ~FOREGROUND;
        int clientWidth = OS.QPaintDevice_width(data.drawable);
        x = clientWidth - x - image.getBounds().width;
    }

    data.internalGc.copyArea(image.getImage(), x, y);
}

/**
 * Copies a rectangular area of the receiver at the source position onto the
 * receiver at the destination position.
 * 
 * @param srcX
 *            the x coordinate in the receiver of the area to be copied
 * @param srcY
 *            the y coordinate in the receiver of the area to be copied
 * @param width
 *            the width of the area to copy
 * @param height
 *            the height of the area to copy
 * @param destX
 *            the x coordinate in the receiver of the area to copy to
 * @param destY
 *            the y coordinate in the receiver of the area to copy to
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public void copyArea(int srcX, int srcY, int width, int height, int destX,
        int destY) {
    copyArea(srcX, srcY, width, height, destX, destY, true);
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
 * @param paint if <code>true</code> paint events will be generated for old and obscured areas
 *
 * @exception SWTException <ul>
 *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
 * </ul>
 * 
 * @since 3.1 
 */
public void copyArea(int srcX, int srcY, int width, int height, int destX, int destY, boolean paint) {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    if (width <= 0 || height <= 0) {
        return;
    }
    int deltaX = destX - srcX;
    int deltaY = destY - srcY;
    if (deltaX == 0 && deltaY == 0) {
        return;
    }

    // Reset all mirroring transformation and calculate the mirrored coordinates
    // manually.
    if ((data.style & SWT.MIRRORED) != 0) {
        data.internalGc.resetTransform();
        data.state = ~FOREGROUND;
        int clientWidth = OS.QPaintDevice_width(data.drawable);
        srcX = clientWidth - srcX - width;
        destX = clientWidth - destX - width;
    }
    data.internalGc.copyArea(srcX, srcY, width, height, destX, destY, paint);
}

/**
 * Disposes of the operating system resources associated with the graphics
 * context. Applications must dispose of all the GCs which they allocate.
 * 
 * @exception SWTError
 *                <ul>
 *                <li>ERROR_THREAD_INVALID_ACCESS if not called from the thread
 *                that created the drawable</li>
 */
public void dispose() {
	final Display display = (Display)data.device; 
	if(display.getThread() != Thread.currentThread()) {
		SWT.error(SWT.ERROR_THREAD_INVALID_ACCESS);
	}
    if (drawable != null && handle != 0) {
        if (data.buffered && (drawable instanceof Control)) {
            display.syncExec(new Runnable() {
                public void run() {
                    ((Control) drawable).redraw();
                    while (display.readAndDispatch())
                        ;
                }
            });
        }
        drawable.internal_dispose_GC(data);
    }

    data.destroy();

    handle = SWT.NULL;
    customForeground = null;
    customBackground = null;
    drawable = null;
}

/**
 * Draws the outline of a circular or elliptical arc within the specified
 * rectangular area.
 * <p>
 * The resulting arc begins at <code>startAngle</code> and extends for <code>
 * arcAngle</code> degrees, using the current color. Angles are interpreted such
 * that 0 degrees is at the 3 o'clock position. A positive value indicates a
 * counter-clockwise rotation while a negative value indicates a clockwise
 * rotation.
 * </p>
 * <p>
 * The center of the arc is the center of the rectangle whose origin is (<code>x
 * </code>, <code>y</code>) and whose size is specified by the <code>width
 * </code> and <code>height</code> arguments.
 * </p>
 * <p>
 * The resulting arc covers an area <code>width + 1</code> pixels wide by <code>
 * height + 1</code> pixels tall.
 * </p>
 * 
 * @param x
 *            the x coordinate of the upper-left corner of the arc to be drawn
 * @param y
 *            the y coordinate of the upper-left corner of the arc to be drawn
 * @param width
 *            the width of the arc to be drawn
 * @param height
 *            the height of the arc to be drawn
 * @param startAngle
 *            the beginning angle
 * @param arcAngle
 *            the angular extent of the arc, relative to the start angle
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public void drawArc(int x, int y, int width, int height, int startAngle,
        int arcAngle) {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    if (width < 0) {
        x = x + width;
        width = -width;
    }
    if (height < 0) {
        y = y + height;
        height = -height;
    }
    if (width == 0 || height == 0 || arcAngle == 0) {
        return;
    }
    checkGC(FOREGROUND);
    data.internalGc.drawArc(x, y, width, height, startAngle, arcAngle);
}

/**
 * Draws a rectangle, based on the specified arguments, which has the appearance
 * of the platform's <em>focus rectangle</em> if the platform supports such a
 * notion, and otherwise draws a simple rectangle in the receiver's foreground
 * color.
 * 
 * @param x
 *            the x coordinate of the rectangle
 * @param y
 *            the y coordinate of the rectangle
 * @param width
 *            the width of the rectangle
 * @param height
 *            the height of the rectangle
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 * 
 * @see #drawRectangle(int, int, int, int)
 */
public void drawFocus(int x, int y, int width, int height) {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    checkGC(FOREGROUND);
    data.internalGc.drawFocus(x, y, width, height);
}

/**
 * Draws the given image in the receiver at the specified coordinates.
 * 
 * @param image
 *            the image to draw
 * @param x
 *            the x coordinate of where to draw
 * @param y
 *            the y coordinate of where to draw
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the image is null</li>
 *                <li>ERROR_INVALID_ARGUMENT - if the image has been disposed</li>
 *                <li>ERROR_INVALID_ARGUMENT - if the given coordinates are
 *                outside the bounds of the image</li>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 * @exception SWTError
 *                <ul>
 *                <li>ERROR_NO_HANDLES - if no handles are available to perform
 *                the operation</li>
 *                </ul>
 */
public void drawImage(Image image, int x, int y) {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    if (image == null) {
        SWT.error(SWT.ERROR_NULL_ARGUMENT);
    }
    if (image.isDisposed()) {
        SWT.error(SWT.ERROR_INVALID_ARGUMENT);
    }
    // Reset all mirroring transformation and calculate the mirrored coordinates
    // manually.
    if ((data.style & SWT.MIRRORED) != 0) {
        data.internalGc.resetTransform();
        data.state = ~FOREGROUND;
        int clientWidth = OS.QPaintDevice_width(data.drawable);
        x = clientWidth - x - image.getBounds().width;
    }
    data.internalGc.drawImage(image.getImage(), x, y);
}

/**
 * Copies a rectangular area from the source image into a (potentially different
 * sized) rectangular area in the receiver. If the source and destination areas
 * are of differing sizes, then the source area will be stretched or shrunk to
 * fit the destination area as it is copied. The copy fails if any part of the
 * source rectangle lies outside the bounds of the source image, or if any of
 * the width or height arguments are negative.
 * 
 * @param image
 *            the source image
 * @param srcX
 *            the x coordinate in the source image to copy from
 * @param srcY
 *            the y coordinate in the source image to copy from
 * @param srcWidth
 *            the width in pixels to copy from the source
 * @param srcHeight
 *            the height in pixels to copy from the source
 * @param destX
 *            the x coordinate in the destination to copy to
 * @param destY
 *            the y coordinate in the destination to copy to
 * @param destWidth
 *            the width in pixels of the destination rectangle
 * @param destHeight
 *            the height in pixels of the destination rectangle
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the image is null</li>
 *                <li>ERROR_INVALID_ARGUMENT - if the image has been disposed</li>
 *                <li>ERROR_INVALID_ARGUMENT - if any of the width or height
 *                arguments are negative.
 *                <li>ERROR_INVALID_ARGUMENT - if the source rectangle is not
 *                contained within the bounds of the source image</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 * @exception SWTError
 *                <ul>
 *                <li>ERROR_NO_HANDLES - if no handles are available to perform
 *                the operation</li>
 *                </ul>
 */
public void drawImage(Image image, int srcX, int srcY, int srcWidth,
        int srcHeight, int destX, int destY, int destWidth, int destHeight) {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    if (srcWidth == 0 || srcHeight == 0 || destWidth == 0 || destHeight == 0) {
        return;
    }
    if (srcX < 0 || srcY < 0 || srcWidth < 0 || srcHeight < 0 || destWidth < 0
            || destHeight < 0) {
        SWT.error(SWT.ERROR_INVALID_ARGUMENT);
    }
    if (image == null) {
        SWT.error(SWT.ERROR_NULL_ARGUMENT);
    }
    if (image.isDisposed()) {
        SWT.error(SWT.ERROR_INVALID_ARGUMENT);
    }
    // Reset all mirroring transformation and calculate the mirrored coordinates
    // manually.
    if ((data.style & SWT.MIRRORED) != 0) {
        data.internalGc.resetTransform();
        data.state = ~FOREGROUND;
        int clientWidth = OS.QPaintDevice_width(data.drawable);
        srcX = image.getBounds().width - srcX - srcWidth;
        destX = clientWidth - destX - destWidth;
    }
    data.internalGc.drawImage(image.getImage(), destX, destY,
            destWidth, destHeight, srcX, srcY, srcWidth, srcHeight);
}

/**
 * Draws a line, using the foreground color, between the points (<code>x1</code>
 * , <code>y1</code>) and (<code>x2</code>, <code>y2</code>).
 * 
 * @param x1
 *            the first point's x coordinate
 * @param y1
 *            the first point's y coordinate
 * @param x2
 *            the second point's x coordinate
 * @param y2
 *            the second point's y coordinate
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public void drawLine(int x1, int y1, int x2, int y2) {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    checkGC(FOREGROUND);
    data.internalGc.drawLine(x1, y1, x2, y2);
}

/**
 * Draws the outline of an oval, using the foreground color, within the
 * specified rectangular area.
 * <p>
 * The result is a circle or ellipse that fits within the rectangle specified by
 * the <code>x</code>, <code>y</code>, <code>width</code>, and
 * <code>height</code> arguments.
 * </p>
 * <p>
 * The oval covers an area that is <code>width + 1</code> pixels wide and
 * <code>height + 1</code> pixels tall.
 * </p>
 * 
 * @param x
 *            the x coordinate of the upper left corner of the oval to be drawn
 * @param y
 *            the y coordinate of the upper left corner of the oval to be drawn
 * @param width
 *            the width of the oval to be drawn
 * @param height
 *            the height of the oval to be drawn
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public void drawOval(int x, int y, int width, int height) {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    if (width < 0) {
        x = x + width;
        width = -width;
    }
    if (height < 0) {
        y = y + height;
        height = -height;
    }
    checkGC(FOREGROUND);
    data.internalGc.drawEllipse(x, y, width, height);
}

/**
 * Draws a pixel, using the foreground color, at the specified point (
 * <code>x</code>, <code>y</code>).
 * <p>
 * Note that the receiver's line attributes do not affect this operation.
 * </p>
 * 
 * @param x
 *            the point's x coordinate
 * @param y
 *            the point's y coordinate
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 * 
 * @since 3.0
 */
public void drawPoint(int x, int y) {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    checkGC(FOREGROUND);
    data.internalGc.drawPoint(x, y);
}

/**
 * Draws the closed polygon which is defined by the specified array of integer
 * coordinates, using the receiver's foreground color. The array contains
 * alternating x and y values which are considered to represent points which are
 * the vertices of the polygon. Lines are drawn between each consecutive pair,
 * and between the first pair and last pair in the array.
 * 
 * @param pointArray
 *            an array of alternating x and y values which are the vertices of
 *            the polygon
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT if pointArray is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public void drawPolygon(int[] pointArray) {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    if (pointArray == null) {
        SWT.error(SWT.ERROR_NULL_ARGUMENT);
    }
    checkGC(FOREGROUND);
    data.internalGc.drawPolygon(pointArray);
}

/**
 * Draws the polyline which is defined by the specified array of integer
 * coordinates, using the receiver's foreground color. The array contains
 * alternating x and y values which are considered to represent points which are
 * the corners of the polyline. Lines are drawn between each consecutive pair,
 * but not between the first pair and last pair in the array.
 * 
 * @param pointArray
 *            an array of alternating x and y values which are the corners of
 *            the polyline
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the point array is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public void drawPolyline(int[] pointArray) {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    if (pointArray == null) {
        SWT.error(SWT.ERROR_NULL_ARGUMENT);
    }
    checkGC(FOREGROUND);
    data.internalGc.drawPolyline(pointArray);
}

/**
 * Draws the outline of the rectangle specified by the arguments, using the
 * receiver's foreground color. The left and right edges of the rectangle are at
 * <code>x</code> and <code>x + width</code>. The top and bottom edges are at
 * <code>y</code> and <code>y + height</code>.
 * 
 * @param x
 *            the x coordinate of the rectangle to be drawn
 * @param y
 *            the y coordinate of the rectangle to be drawn
 * @param width
 *            the width of the rectangle to be drawn
 * @param height
 *            the height of the rectangle to be drawn
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public void drawRectangle(int x, int y, int width, int height) {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    if (width < 0) {
        x = x + width;
        width = -width;
    }
    if (height < 0) {
        y = y + height;
        height = -height;
    }
    checkGC(FOREGROUND);
    data.internalGc.drawRect(x, y, width, height);
}

/**
 * Draws the outline of the specified rectangle, using the receiver's foreground
 * color. The left and right edges of the rectangle are at <code>rect.x</code>
 * and <code>rect.x + rect.width</code>. The top and bottom edges are at
 * <code>rect.y</code> and <code>rect.y + rect.height</code>.
 * 
 * @param rect
 *            the rectangle to draw
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the rectangle is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public void drawRectangle(Rectangle rect) {
    if (rect == null) {
        SWT.error(SWT.ERROR_NULL_ARGUMENT);
    }
    drawRectangle(rect.x, rect.y, rect.width, rect.height);
}

/**
 * Draws the outline of the round-cornered rectangle specified by the arguments,
 * using the receiver's foreground color. The left and right edges of the
 * rectangle are at <code>x</code> and <code>x + width</code>. The top and
 * bottom edges are at <code>y</code> and <code>y + height</code>. The
 * <em>roundness</em> of the corners is specified by the <code>arcWidth</code>
 * and <code>arcHeight</code> arguments, which are respectively the width and
 * height of the ellipse used to draw the corners.
 * 
 * @param x
 *            the x coordinate of the rectangle to be drawn
 * @param y
 *            the y coordinate of the rectangle to be drawn
 * @param width
 *            the width of the rectangle to be drawn
 * @param height
 *            the height of the rectangle to be drawn
 * @param arcWidth
 *            the width of the arc
 * @param arcHeight
 *            the height of the arc
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public void drawRoundRectangle(int x, int y, int width, int height,
        int arcWidth, int arcHeight) {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    checkGC(FOREGROUND);
    data.internalGc.drawRoundRect(x, y, width, height, arcWidth, arcHeight);
}

/**
 * Draws the given string, using the receiver's current font and foreground
 * color. No tab expansion or carriage return processing will be performed. The
 * background of the rectangular area where the string is being drawn will be
 * filled with the receiver's background color.
 * 
 * @param string
 *            the string to be drawn
 * @param x
 *            the x coordinate of the top left corner of the rectangular area
 *            where the string is to be drawn
 * @param y
 *            the y coordinate of the top left corner of the rectangular area
 *            where the string is to be drawn
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the string is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public void drawString(String string, int x, int y) {
    drawString(string, x, y, false);
}

/**
 * Draws the given string, using the receiver's current font and foreground
 * color. No tab expansion or carriage return processing will be performed. If
 * <code>isTransparent</code> is <code>true</code>, then the background of the
 * rectangular area where the string is being drawn will not be modified,
 * otherwise it will be filled with the receiver's background color.
 * 
 * @param string
 *            the string to be drawn
 * @param x
 *            the x coordinate of the top left corner of the rectangular area
 *            where the string is to be drawn
 * @param y
 *            the y coordinate of the top left corner of the rectangular area
 *            where the string is to be drawn
 * @param isTransparent
 *            if <code>true</code> the background will be transparent, otherwise
 *            it will be opaque
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the string is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public void drawString(String string, int x, int y, boolean isTransparent) {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    if (string == null) {
        SWT.error(SWT.ERROR_NULL_ARGUMENT);
    }
    if (string.length() == 0) {
        return;
    }
    // Reset all mirroring transformation and calculate the mirrored coordinates
    // manually.
    if ((data.style & SWT.MIRRORED) != 0) {
        data.internalGc.resetTransform();
        data.state = ~FOREGROUND;
        int clientWidth = OS.QPaintDevice_width(data.drawable);
        int box[] = new int[4];
        data.internalGc
                .getTextBoundingBox(
                        box,
                        string,
                        (GraphicsContext.ALIGNMENT_LEFT | GraphicsContext.ALIGNMENT_TOP),
                        GraphicsContext.TEXT_SINGLE_LINE);
        x = clientWidth - x - box[GraphicsContext.RECT_WIDTH];
    }
    data.internalGc.drawString(string, x, y, isTransparent);
}

/**
 * Draws the given string, using the receiver's current font and foreground
 * color. Tab expansion and carriage return processing are performed. The
 * background of the rectangular area where the text is being drawn will be
 * filled with the receiver's background color.
 * 
 * @param string
 *            the string to be drawn
 * @param x
 *            the x coordinate of the top left corner of the rectangular area
 *            where the text is to be drawn
 * @param y
 *            the y coordinate of the top left corner of the rectangular area
 *            where the text is to be drawn
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the string is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public void drawText(String string, int x, int y) {
    drawText(string, x, y, SWT.DRAW_DELIMITER | SWT.DRAW_TAB);
}

/**
 * Draws the given string, using the receiver's current font and foreground
 * color. Tab expansion, line delimiter and mnemonic processing are performed
 * according to the specified flags. If <code>flags</code> includes
 * <code>DRAW_TRANSPARENT</code>, then the background of the rectangular area
 * where the text is being drawn will not be modified, otherwise it will be
 * filled with the receiver's background color.
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
 * @param string
 *            the string to be drawn
 * @param x
 *            the x coordinate of the top left corner of the rectangular area
 *            where the text is to be drawn
 * @param y
 *            the y coordinate of the top left corner of the rectangular area
 *            where the text is to be drawn
 * @param flags
 *            the flags specifying how to process the text
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the string is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public void drawText(String string, int x, int y, int flags) {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    if (string == null) {
        SWT.error(SWT.ERROR_NULL_ARGUMENT);
    }
    if (string.length() == 0) {
        return;
    }

    int translatedFlags = 0;
    boolean isTransparent = false;

    if ((flags & SWT.DRAW_DELIMITER) == 0) {
        translatedFlags |= GraphicsContext.TEXT_SINGLE_LINE;
    }
    if ((flags & SWT.DRAW_TAB) != 0) {
        translatedFlags |= GraphicsContext.TEXT_EXPAND_TABS;
    }
    if ((flags & SWT.DRAW_MNEMONIC) != 0) {
        translatedFlags |= GraphicsContext.TEXT_SHOW_MNEMONIC;
    }
    if ((flags & SWT.DRAW_TRANSPARENT) != 0) {
        isTransparent = true;
    }
    // Reset all mirroring transformation and calculate the mirrored coordinates
    // manually.
    if ((data.style & SWT.MIRRORED) != 0) {
        data.internalGc.resetTransform();
        data.state = ~FOREGROUND;
        int clientWidth = OS.QPaintDevice_width(data.drawable);
        int box[] = new int[4];
        data.internalGc
                .getTextBoundingBox(
                        box,
                        string,
                        (GraphicsContext.ALIGNMENT_HCENTER | GraphicsContext.ALIGNMENT_LEFT),
                        translatedFlags);
        x = clientWidth - x - box[GraphicsContext.RECT_WIDTH];
    }
    data.internalGc
            .drawString(
                    string,
                    x,
                    y,
                    0,
                    0,
                    (GraphicsContext.ALIGNMENT_HCENTER | GraphicsContext.ALIGNMENT_LEFT),
                    translatedFlags, isTransparent);
}

/**
 * Compares the argument to the receiver, and returns true if they represent the
 * <em>same</em> object using a class specific comparison.
 * 
 * @param object
 *            the object to compare with this object
 * @return <code>true</code> if the object is the same as this object and
 *         <code>false</code> otherwise
 * 
 * @see #hashCode
 */
public boolean equals(Object object) {
    if (object == this) {
        return true;
    }
    if (!(object instanceof GC)) {
        return false;
    }
    return handle == ((GC) object).handle;
}

/**
 * Fills the interior of a circular or elliptical arc within the specified
 * rectangular area, with the receiver's background color.
 * <p>
 * The resulting arc begins at <code>startAngle</code> and extends for
 * <code>arcAngle</code> degrees, using the current color. Angles are
 * interpreted such that 0 degrees is at the 3 o'clock position. A positive
 * value indicates a counter-clockwise rotation while a negative value indicates
 * a clockwise rotation.
 * </p>
 * <p>
 * The center of the arc is the center of the rectangle whose origin is (
 * <code>x</code>, <code>y</code>) and whose size is specified by the
 * <code>width</code> and <code>height</code> arguments.
 * </p>
 * <p>
 * The resulting arc covers an area <code>width + 1</code> pixels wide by
 * <code>height + 1</code> pixels tall.
 * </p>
 * 
 * @param x
 *            the x coordinate of the upper-left corner of the arc to be filled
 * @param y
 *            the y coordinate of the upper-left corner of the arc to be filled
 * @param width
 *            the width of the arc to be filled
 * @param height
 *            the height of the arc to be filled
 * @param startAngle
 *            the beginning angle
 * @param arcAngle
 *            the angular extent of the arc, relative to the start angle
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 * 
 * @see #drawArc
 */
public void fillArc(int x, int y, int width, int height, int startAngle,
        int arcAngle) {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    if (width < 0) {
        x = x + width;
        width = -width;
    }
    if (height < 0) {
        y = y + height;
        height = -height;
    }
    if (width == 0 || height == 0 || arcAngle == 0) {
        return;
    }
    checkGC(FOREGROUND);
    data.internalGc.fillArc(x, y, width, height, startAngle, arcAngle);
}

/**
 * Fills the interior of the specified rectangle with a gradient sweeping from
 * left to right or top to bottom progressing from the receiver's foreground
 * color to its background color.
 * 
 * @param x
 *            the x coordinate of the rectangle to be filled
 * @param y
 *            the y coordinate of the rectangle to be filled
 * @param width
 *            the width of the rectangle to be filled, may be negative (inverts
 *            direction of gradient if horizontal)
 * @param height
 *            the height of the rectangle to be filled, may be negative (inverts
 *            direction of gradient if vertical)
 * @param vertical
 *            if true sweeps from top to bottom, else sweeps from left to right
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 * 
 * @see #drawRectangle(int, int, int, int)
 */
public void fillGradientRectangle(int x, int y, int width, int height,
        boolean vertical) {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    if ((width == 0) || (height == 0)) {
        return;
    }

    boolean swapColors = false;
    if (width < 0) {
        if (!vertical) {
            swapColors = true;
        }
        width = Math.abs(width);
    }
    if (height < 0) {
        if (vertical) {
            swapColors = true;
        }
        height = Math.abs(height);
    }
    checkGC(FOREGROUND);
    data.internalGc.fillGradientRect(x, y, width, height, vertical, swapColors);
}

/**
 * Fills the interior of an oval, within the specified rectangular area, with
 * the receiver's background color.
 * 
 * @param x
 *            the x coordinate of the upper left corner of the oval to be filled
 * @param y
 *            the y coordinate of the upper left corner of the oval to be filled
 * @param width
 *            the width of the oval to be filled
 * @param height
 *            the height of the oval to be filled
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 * 
 * @see #drawOval
 */
public void fillOval(int x, int y, int width, int height) {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    if (width < 0) {
        x = x + width;
        width = -width;
    }
    if (height < 0) {
        y = y + height;
        height = -height;
    }
    checkGC(FOREGROUND);
    data.internalGc.fillEllipse(x, y, width, height);
}

/**
 * Fills the interior of the closed polygon which is defined by the specified
 * array of integer coordinates, using the receiver's background color. The
 * array contains alternating x and y values which are considered to represent
 * points which are the vertices of the polygon. Lines are drawn between each
 * consecutive pair, and between the first pair and last pair in the array.
 * 
 * @param pointArray
 *            an array of alternating x and y values which are the vertices of
 *            the polygon
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT if pointArray is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 * 
 * @see #drawPolygon
 */
public void fillPolygon(int[] pointArray) {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    if (pointArray == null) {
        SWT.error(SWT.ERROR_NULL_ARGUMENT);
    }
    checkGC(FOREGROUND);
    data.internalGc.fillPolygon(pointArray);
}

/**
 * Fills the interior of the rectangle specified by the arguments, using the
 * receiver's background color.
 * 
 * @param x
 *            the x coordinate of the rectangle to be filled
 * @param y
 *            the y coordinate of the rectangle to be filled
 * @param width
 *            the width of the rectangle to be filled
 * @param height
 *            the height of the rectangle to be filled
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 * 
 * @see #drawRectangle(int, int, int, int)
 */
public void fillRectangle(int x, int y, int width, int height) {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    if (width < 0) {
        x = x + width;
        width = -width;
    }
    if (height < 0) {
        y = y + height;
        height = -height;
    }
    checkGC(FOREGROUND);
    data.internalGc.fillRect(x, y, width, height);
}

/**
 * Fills the interior of the specified rectangle, using the receiver's
 * background color.
 * 
 * @param rect
 *            the rectangle to be filled
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the rectangle is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 * 
 * @see #drawRectangle(int, int, int, int)
 */
public void fillRectangle(Rectangle rect) {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    if (rect == null) {
        SWT.error(SWT.ERROR_NULL_ARGUMENT);
    }
    fillRectangle(rect.x, rect.y, rect.width, rect.height);
}

/**
 * Fills the interior of the round-cornered rectangle specified by the
 * arguments, using the receiver's background color.
 * 
 * @param x
 *            the x coordinate of the rectangle to be filled
 * @param y
 *            the y coordinate of the rectangle to be filled
 * @param width
 *            the width of the rectangle to be filled
 * @param height
 *            the height of the rectangle to be filled
 * @param arcWidth
 *            the width of the arc
 * @param arcHeight
 *            the height of the arc
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 * 
 * @see #drawRoundRectangle
 */
public void fillRoundRectangle(int x, int y, int width, int height,
        int arcWidth, int arcHeight) {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    checkGC(FOREGROUND);
    data.internalGc.fillRoundRect(x, y, width, height, arcWidth, arcHeight);
}

/**
 * Returns the <em>advance width</em> of the specified character in the font
 * which is currently selected into the receiver.
 * <p>
 * The advance width is defined as the horizontal distance the cursor should
 * move after printing the character in the selected font.
 * </p>
 * 
 * @param ch
 *            the character to measure
 * @return the distance in the x direction to move past the character before
 *         painting the next
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public int getAdvanceWidth(char ch) {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    return data.internalGc.getAdvancedCharacterWidth(ch);
}

/**
 * Returns the receiver's alpha value. Default alpha value is 0xFF, fully opaque
 * 
 * 
 * @return the alpha value
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public int getAlpha() {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    return data.alpha;
}

/**
 * Returns the background color.
 * 
 * @return the receiver's background color
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public Color getBackground() {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    return customBackground != null ? customBackground : data.background;
}

/**
 * Returns the width of the specified character in the font selected into the
 * receiver.
 * <p>
 * The width is defined as the space taken up by the actual character, not
 * including the leading and tailing whitespace or overhang.
 * </p>
 * 
 * @param ch
 *            the character to measure
 * @return the width of the character
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public int getCharWidth(char ch) {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    return data.internalGc.getCharacterWidth(ch);
}

/**
 * Returns the bounding rectangle of the receiver's clipping region. If no
 * clipping region is set, the return value will be a rectangle which covers the
 * entire bounds of the object the receiver is drawing on.
 * 
 * @return the bounding rectangle of the clipping region
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public Rectangle getClipping() {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    checkGC(FOREGROUND);
    if (data.clippingRect == null) {
        int[] clip = new int[4];
        data.internalGc.getClip(clip);
        data.clippingRect = new Rectangle(clip[GraphicsContext.RECT_X],
                clip[GraphicsContext.RECT_Y], clip[GraphicsContext.RECT_WIDTH],
                clip[GraphicsContext.RECT_HEIGHT]);
    }
    return new Rectangle(data.clippingRect.x, data.clippingRect.y,
            data.clippingRect.width, data.clippingRect.height);
}

/**
 * Returns the font currently being used by the receiver to draw and measure
 * text.
 * 
 * @return the receiver's font
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public Font getFont() {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    return data.font;
}

/**
 * Returns a FontMetrics which contains information about the font currently
 * being used by the receiver to draw and measure text. Note that if the font is
 * changed later, the font metrics object is not updated accordingly.
 * 
 * @return font metrics for the receiver's font
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public FontMetrics getFontMetrics() {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }

    // Read the FontMetrics data from the native
    int fmData[] = new int[FM_DATA_COUNT];
    data.internalGc.getFontMetricsData(fmData, 0, false);
    // Create and fill up FonMetrics object.
    FontMetrics fm = new FontMetrics();
    fm.ascent = fmData[FM_ASCENT];
    fm.averageCharWidth = fmData[FM_AVERAGE_CHAR_WIDTH];
    fm.descent = fmData[FM_DESCENT];
    fm.height = fmData[FM_HEIGHT];
    fm.leading = fmData[FM_LEADING];
    return fm;
}

/**
 * Returns the receiver's foreground color.
 * 
 * @return the color used for drawing foreground things
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public Color getForeground() {
    if (isDisposed()) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    return customForeground != null ? customForeground : data.foreground;
}

/**
 * Returns the GCData.
 * <p>
 * <b>IMPORTANT:</b> This method is <em>not</em> part of the public API for
 * <code>GC</code>. It is marked public only so that it can be shared within the
 * packages provided by SWT. It is not available on all platforms, and should
 * never be called from application code.
 * </p>
 * 
 * @return the receiver's GCData
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 * 
 * @see GCData
 * 
 * @since 3.2
 */
public GCData getGCData() {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    return data;
}

/**
 * Returns the receiver's line style, which will be one of the constants
 * <code>SWT.LINE_SOLID</code>, <code>SWT.LINE_DASH</code>,
 * <code>SWT.LINE_DOT</code>, <code>SWT.LINE_DASHDOT</code> or
 * <code>SWT.LINE_DASHDOTDOT</code>.
 * 
 * @return the style used for drawing lines
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public int getLineStyle() {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    if (data.lineStyle == GCData.NOT_DEFINED) {
        data.lineStyle = GCData.translateStrokeStyle(data.internalGc
                .getStrokeStyle(), false);
    }
    return data.lineStyle;
}

/**
 * Returns the width that will be used when drawing lines for all of the figure
 * drawing operations (that is, <code>drawLine</code>,
 * <code>drawRectangle</code>, <code>drawPolyline</code>, and so forth.
 * 
 * @return the receiver's line width
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public int getLineWidth() {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    if (data.lineWidth == GCData.NOT_DEFINED) {
        data.lineWidth = data.internalGc.getStrokeWidth();
    }
    return data.lineWidth;
}

/**
 * Returns the receiver's style information.
 * <p>
 * Note that the value which is returned by this method <em>may
 * not match</em> the value which was provided to the constructor when the
 * receiver was created. This can occur when the underlying operating system
 * does not support a particular combination of requested styles.
 * </p>
 * 
 * @return the style bits. Possible values are:
 *         <ul>
 *         <li>LEFT_TO_RIGHT</li>
 *         <li>RIGHT_TO_LEFT</li>
 *         </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 * 
 * @since 2.1.2
 */
public int getStyle() {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    return data.style;
}

/**
 * Returns <code>true</code> if this GC is drawing in the mode where the
 * resulting color in the destination is the <em>exclusive or</em> of the color
 * values in the source and the destination, and <code>false</code> if it is
 * drawing in the mode where the destination color is being replaced with the
 * source color value.
 * 
 * @return <code>true</code> true if the receiver is in XOR mode, and false
 *         otherwise
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public boolean getXORMode() {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    if (data.xorMode == GCData.NOT_DEFINED) {
        data.xorMode = (data.internalGc.getBlendingMode() == GraphicsContext.BLENDING_MODE_XOR ? GCData.XOR_MODE_ON
                : GCData.XOR_MODE_OFF);
    }
    return (data.xorMode == GCData.XOR_MODE_ON);
}

/**
 * Returns an integer hash code for the receiver. Any two objects that return
 * <code>true</code> when passed to <code>equals</code> must return the same
 * value for this method.
 * 
 * @return the receiver's hash
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 * 
 * @see #equals
 */
public int hashCode() {
    return handle;
}

void init(Drawable drawable, GCData data, int /* long */gcHandle) {
    this.drawable = drawable;
    this.data = data;
    this.handle = gcHandle;
    if (data.foreground == null) {
        data.foreground = data.device.getSystemColor(SWT.COLOR_BLACK);
    }
    if (data.background == null) {
        data.background = data.device.getSystemColor(SWT.COLOR_WHITE);
    }
    data.internalGc.setForegroundColor(data.foreground.getRed(),
            data.foreground.getGreen(), data.foreground.getBlue());
    data.internalGc.setBackgroundColor(data.background.getRed(),
            data.background.getGreen(), data.background.getBlue());
    this.drawable = drawable;
    this.data = data;
    handle = gcHandle;
}

/**
 * Returns <code>true</code> if the receiver has a clipping region set into it,
 * and <code>false</code> otherwise. If this method returns false, the receiver
 * will draw on all available space in the destination. If it returns true, it
 * will draw only in the area that is covered by the region that can be accessed
 * with <code>getClipping(region)</code>.
 * 
 * @return <code>true</code> if the GC has a clipping region, and
 *         <code>false</code> otherwise
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public boolean isClipped() {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    return data.internalGc.hasClipping();
}

/**
 * Returns <code>true</code> if the GC has been disposed, and <code>false</code>
 * otherwise.
 * <p>
 * This method gets the dispose state for the GC. When a GC has been disposed,
 * it is an error to invoke any other method using the GC.
 * 
 * @return <code>true</code> when the GC is disposed and <code>false</code>
 *         otherwise
 */
public boolean isDisposed() {
    return handle == 0;
}

/**
 * Sets the receiver's alpha value.
 * <p>
 * This operation requires the operating system's advanced graphics subsystem
 * which may not be available on some platforms.
 * </p>
 * 
 * @param alpha
 *            the alpha value
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                <li>ERROR_NO_GRAPHICS_LIBRARY - if advanced graphics are not
 *                available</li>
 *                </ul>
 * 
 * @see #getAdvanced
 * @see #setAdvanced
 * 
 * @since 3.1
 */
public void setAlpha(int alpha) {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    data.alpha = alpha & 0xff;
    data.internalGc.setForegroundAlpha(data.alpha);
    data.internalGc.setBackgroundAlpha(data.alpha);
}

/**
 * Sets the background color. The background color is used for fill operations
 * and as the background color when text is drawn.
 * 
 * @param color
 *            the new background color for the receiver
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the color is null</li>
 *                <li>ERROR_INVALID_ARGUMENT - if the color has been disposed</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public void setBackground(Color color) {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    if (color == null) {
        SWT.error(SWT.ERROR_NULL_ARGUMENT);
    }
    if (color.isDisposed()) {
        SWT.error(SWT.ERROR_INVALID_ARGUMENT);
    }
    this.customBackground = color;
    data.internalGc.setBackgroundColor(color.getRed(), color.getGreen(), color
            .getBlue());
}

/**
 * Sets the area of the receiver which can be changed by drawing operations to
 * the rectangular area specified by the arguments.
 * 
 * @param x
 *            the x coordinate of the clipping rectangle
 * @param y
 *            the y coordinate of the clipping rectangle
 * @param width
 *            the width of the clipping rectangle
 * @param height
 *            the height of the clipping rectangle
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public void setClipping(int x, int y, int width, int height) {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    if (width < 0) {
        x = x + width;
        width = -width;
    }
    if (height < 0) {
        y = y + height;
        height = -height;
    }
    // Reset all mirroring transformation and calculate the mirrored coordinates
    // manually.
    if ((data.style & SWT.MIRRORED) != 0) {
        data.internalGc.resetTransform();
        data.state = ~FOREGROUND;
        int clientWidth = OS.QPaintDevice_width(data.drawable);
        x = clientWidth - x - (width + 1);
    }
    data.clippingRect = new Rectangle(x, y, width, height);
    data.internalGc.setClip(x, y, width + 1, height, false);
}

/**
 * Sets the area of the receiver which can be changed by drawing operations to
 * the rectangular area specified by the argument. Specifying <code>null</code>
 * for the rectangle reverts the receiver's clipping area to its original value.
 * 
 * @param rect
 *            the clipping rectangle or <code>null</code>
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public void setClipping(Rectangle rect) {
    if (handle == 0)
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    if (rect != null) {
        setClipping(rect.x, rect.y, rect.width, rect.height);
    } else {
        data.internalGc.cancelClipping();
    }
}

/**
 * Sets the font which will be used by the receiver to draw and measure text to
 * the argument. If the argument is null, then a default font appropriate for
 * the platform will be used instead.
 * 
 * @param font
 *            the new font for the receiver, or null to indicate a default font
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_INVALID_ARGUMENT - if the font has been disposed</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public void setFont(Font font) {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    if (font != null && font.isDisposed()) {
        SWT.error(SWT.ERROR_INVALID_ARGUMENT);
    }
    data.font = font != null ? font : Font.qt_new(data.device, OS.SwtFontCache_cache(OS.QApplication_swt_font_new()));
    data.internalGc.setFont(data.font.handle);
}

/**
 * Sets the foreground color. The foreground color is used for drawing
 * operations including when text is drawn.
 * 
 * @param color
 *            the new foreground color for the receiver
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the color is null</li>
 *                <li>ERROR_INVALID_ARGUMENT - if the color has been disposed</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public void setForeground(Color color) {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    if (color == null) {
        SWT.error(SWT.ERROR_NULL_ARGUMENT);
    }
    if (color.isDisposed()) {
        SWT.error(SWT.ERROR_INVALID_ARGUMENT);
    }
    customForeground = color;
    data.internalGc.setForegroundColor(color.getRed(), color.getGreen(), color
            .getBlue());
}

/**
 * Sets the receiver's line style to the argument, which must be one of the
 * constants <code>SWT.LINE_SOLID</code>, <code>SWT.LINE_DASH</code>,
 * <code>SWT.LINE_DOT</code>, <code>SWT.LINE_DASHDOT</code> or
 * <code>SWT.LINE_DASHDOTDOT</code>.
 * 
 * @param lineStyle
 *            the style to be used for drawing lines
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_INVALID_ARGUMENT - if the style is not valid</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public void setLineStyle(int lineStyle) {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    if (data.lineStyle == lineStyle) {
        return;
    }
    data.internalGc
            .setStrokeStyle(GCData.translateStrokeStyle(lineStyle, true));
    data.lineStyle = lineStyle;
}

/**
 * Sets the width that will be used when drawing lines for all of the figure
 * drawing operations (that is, <code>drawLine</code>,
 * <code>drawRectangle</code>, <code>drawPolyline</code>, and so forth.
 * <p>
 * Note that line width of zero is used as a hint to indicate that the fastest
 * possible line drawing algorithms should be used. This means that the output
 * may be different from line width one.
 * </p>
 * 
 * @param lineWidth
 *            the width of a line
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public void setLineWidth(int lineWidth) {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    if (data.lineWidth == lineWidth) {
        return;
    }
    data.lineWidth = lineWidth;
    data.internalGc.setStrokeWidth(lineWidth);
}

/**
 * If the argument is <code>true</code>, puts the receiver in a drawing mode
 * where the resulting color in the destination is the <em>exclusive or</em> of
 * the color values in the source and the destination, and if the argument is
 * <code>false</code>, puts the receiver in a drawing mode where the destination
 * color is replaced with the source color value.
 * <p>
 * Note that this mode in fundamentally unsupportable on certain platforms,
 * notably Carbon (Mac OS X). Clients that want their code to run on all
 * platforms need to avoid this method.
 * </p>
 * 
 * @param xor
 *            if <code>true</code>, then <em>xor</em> mode is used, otherwise
 *            <em>source copy</em> mode is used
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public void setXORMode(boolean xor) {
    if (handle == 0) {
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    }
    if (xor) {
        if (data.xorMode == GCData.XOR_MODE_ON) {
            return;
        }
        data.xorMode = GCData.XOR_MODE_ON;
        data.internalGc.setBlendingMode(GraphicsContext.BLENDING_MODE_XOR);
    } else {
        if (data.xorMode == GCData.XOR_MODE_OFF) {
            return;
        }
        data.xorMode = GCData.XOR_MODE_OFF;
        data.internalGc.setBlendingMode(GraphicsContext.BLENDING_MODE_SRC_OVER);
    }
}

/**
 * Returns the extent of the given string. No tab expansion or carriage return
 * processing will be performed.
 * <p>
 * The <em>extent</em> of a string is the width and height of the rectangular
 * area it would cover if drawn in a particular font (in this case, the current
 * font in the receiver).
 * </p>
 * 
 * @param string
 *            the string to measure
 * @return a point containing the extent of the string
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the string is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public Point stringExtent(String string) {
    return textExtent(string, 0);
}

/**
 * Returns the extent of the given string. Tab expansion and carriage return
 * processing are performed.
 * <p>
 * The <em>extent</em> of a string is the width and height of the rectangular
 * area it would cover if drawn in a particular font (in this case, the current
 * font in the receiver).
 * </p>
 * 
 * @param string
 *            the string to measure
 * @return a point containing the extent of the string
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the string is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public Point textExtent(String string) {
    return textExtent(string, SWT.DRAW_DELIMITER | SWT.DRAW_TAB);
}

/**
 * Returns the extent of the given string. Tab expansion, line delimiter and
 * mnemonic processing are performed according to the specified flags, which can
 * be a combination of:
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
 * The <em>extent</em> of a string is the width and height of the rectangular
 * area it would cover if drawn in a particular font (in this case, the current
 * font in the receiver).
 * </p>
 * 
 * @param string
 *            the string to measure
 * @param flags
 *            the flags specifying how to process the text
 * @return a point containing the extent of the string
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the string is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed
 *                </li>
 *                </ul>
 */
public Point textExtent(String string, int flags) {
    if (handle == 0)
        SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
    if (string == null)
        SWT.error(SWT.ERROR_NULL_ARGUMENT);

    int[] box = new int[4];
    int textFlags = 0;

    if ((flags & SWT.DRAW_DELIMITER) == 0) {
        textFlags |= GraphicsContext.TEXT_SINGLE_LINE;
    }
    if ((flags & SWT.DRAW_TAB) != 0) {
        textFlags |= GraphicsContext.TEXT_EXPAND_TABS;
    }
    if ((flags & SWT.DRAW_MNEMONIC) != 0) {
        textFlags |= GraphicsContext.TEXT_SHOW_MNEMONIC;
    }

    data.internalGc.getTextBoundingBox(box, string,
            GraphicsContext.ALIGNMENT_LEFT, textFlags);

    return new Point(box[GraphicsContext.RECT_WIDTH],
            box[GraphicsContext.RECT_HEIGHT]);
}

/**
 * Returns a string containing a concise, human-readable description of the
 * receiver.
 * 
 * @return a string representation of the receiver
 */
public String toString() {
    if (isDisposed())
        return "GC {*DISPOSED*}";
    return "GC {" + handle + "}";
}
}
