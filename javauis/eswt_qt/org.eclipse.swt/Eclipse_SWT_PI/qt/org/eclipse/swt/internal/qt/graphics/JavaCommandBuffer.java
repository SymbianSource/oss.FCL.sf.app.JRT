/*******************************************************************************
 * Copyright (c) 2008, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/

package org.eclipse.swt.internal.qt.graphics;

import java.util.Vector;
import java.util.Enumeration;


/**
 * This class implements a command buffer that can be bound by GraphicsContext
 * to collect the drawing operations. Contents of the command buffer can be replayed to
 * GraphicsContext target with GraphicsContext.render() method.
 *
 * When JavaCommandBuffer is set as the target of GraphicsContext some of the methods
 * provided by GraphicsContext are not supported, e.g. getters, since the JavaCommandBuffer
 * does not have a real rendering target from which to query related data. To find out what is
 * supported see the methods of this class. In principal GraphicsContext supports all draw and set operations,
 * when JavaCommandBuffer is set as rendering target.
 */
public final class JavaCommandBuffer {

    // All collected drawing operations and their parameters
    private int[] intParams;
    private int intCount;
    private StringBuffer strParams;

    // Container for images
    private Vector images;

    // Container for rgbData
    private Vector rgbData;

    // This flag is indicates if this buffer is bound by some GraphicsContext
    private boolean bound;

    // This flag indicates if this buffer is empty or not
    private boolean containsData;

    // This flag indicates if this buffer has any draw commands in it
    private boolean containsPrimitiveData;

    // Memory management configuration constants
    private static final int INT_BUF_GRANULARITY         = 2560; // 10kB
    private static final int INT_BUF_INITIAL_SIZE        = 256; // 1kB
    private static final int IMAGE_BUF_INITIAL_SIZE      = 256; // 1kB
    private static final int IMAGE_BUF_GRANULARITY       = 256; // 1kB
    private static final int STRING_BUFFER_INITIAL_SIZE  = 256; // 1kB

    // Prefixes for different categories of op codes, stored in MSB
    final static int DRAW_PREFIX = 0;
    final static int FILL_PREFIX = 1;
    final static int SET_PREFIX = 2;
    final static int MISC_PREFIX = 3;

    // Unique operation codes for all the gc operations that can be buffered.
    static final int OP_DRAWARC            = ((DRAW_PREFIX << 24) | 1);
    static final int OP_DRAWFOCUS          = ((DRAW_PREFIX << 24) | 2);
    static final int OP_DRAWIMAGE1         = ((DRAW_PREFIX << 24) | 3);
    static final int OP_DRAWIMAGE2         = ((DRAW_PREFIX << 24) | 4);
    static final int OP_DRAWLINE           = ((DRAW_PREFIX << 24) | 5);
    static final int OP_DRAWELLIPSE        = ((DRAW_PREFIX << 24) | 6);
    static final int OP_DRAWPOINT          = ((DRAW_PREFIX << 24) | 7);
    static final int OP_DRAWPOLYGON        = ((DRAW_PREFIX << 24) | 8);
    static final int OP_DRAWPOLYLINE       = ((DRAW_PREFIX << 24) | 9);
    static final int OP_DRAWRECT           = ((DRAW_PREFIX << 24) | 10);
    static final int OP_DRAWRGB_INT        = ((DRAW_PREFIX << 24) | 11);
    static final int OP_DRAWRGB_BYTE       = ((DRAW_PREFIX << 24) | 12);
    static final int OP_DRAWRGB_SHORT      = ((DRAW_PREFIX << 24) | 13);
    static final int OP_DRAWROUNDRECT      = ((DRAW_PREFIX << 24) | 14);
    static final int OP_DRAWSTRING         = ((DRAW_PREFIX << 24) | 15);
    static final int OP_FILLARC            = ((FILL_PREFIX << 24) | 16);
    static final int OP_FILLGRADIENTRECT   = ((FILL_PREFIX << 24) | 17);
    static final int OP_FILLELLIPSE        = ((FILL_PREFIX << 24) | 18);
    static final int OP_FILLPOLYGON        = ((FILL_PREFIX << 24) | 19);
    static final int OP_FILLRECT           = ((FILL_PREFIX << 24) | 20);
    static final int OP_FILLROUNDRECT      = ((FILL_PREFIX << 24) | 21);
    static final int OP_SETBACKGROUNDALPHA = ((SET_PREFIX << 24) | 22);
    static final int OP_SETBACKGROUNDCOLOR = ((SET_PREFIX << 24) | 23);
    static final int OP_SETBLENDINGMODE    = ((SET_PREFIX << 24) | 24);
    static final int OP_SETCLIP            = ((SET_PREFIX << 24) | 25);
    static final int OP_CANCELCLIPPING     = ((SET_PREFIX << 24) | 26);
    static final int OP_SETFONT            = ((SET_PREFIX << 24) | 27);
    static final int OP_SETFOREGROUNDALPHA = ((SET_PREFIX << 24) | 28);
    static final int OP_SETFOREGROUNDCOLOR = ((SET_PREFIX << 24) | 29);
    static final int OP_SETSTROKESTYLE     = ((SET_PREFIX << 24) | 30);
    static final int OP_SETSTROKEWIDTH     = ((SET_PREFIX << 24) | 31);
    static final int OP_TRANSLATE          = ((MISC_PREFIX << 24) | 32);
    static final int OP_SCALE              = ((MISC_PREFIX << 24) | 33);
    static final int OP_RESETTRANSFORM     = ((MISC_PREFIX << 24) | 34);
    static final int OP_COPYAREA1          = ((MISC_PREFIX << 24) | 35);
    static final int OP_COPYAREA2          = ((MISC_PREFIX << 24) | 36);

    /**
     * Constructs empty command buffer with defined buffer sizes.
     */
    public JavaCommandBuffer() {
        intParams = new int[INT_BUF_INITIAL_SIZE];
        strParams = new StringBuffer(STRING_BUFFER_INITIAL_SIZE);
        images = new Vector(IMAGE_BUF_INITIAL_SIZE, IMAGE_BUF_GRANULARITY);
        rgbData = new Vector(IMAGE_BUF_INITIAL_SIZE, IMAGE_BUF_GRANULARITY);
    }

    /**
     * Resets the buffer, i.e. removes all recorded commands and related data. The integer array containing
     * the actual operation codes is not deleted, only the index pointer is rested, thus the memory consumption of that is not freed.
     * CommandBuffer can be reseted only if it is not bound by any GraphicsContext
     * @throws IllegalStateException if this CommandBuffer is bound while calling reset
     */
    public void reset() {
        if(bound) {
            throw new IllegalStateException("CommandBuffer is still bound by gc");
        }
        intCount = 0;
        strParams.setLength(0);

        Enumeration allImages = images.elements();
        while(allImages.hasMoreElements()) {
            Image image = (Image)allImages.nextElement();
            if(image != null) {
                image.freeCommandBufferCopy();
            }
        }
        images.removeAllElements();
        rgbData.removeAllElements();
        containsData = false;
        containsPrimitiveData = false;
    }

    /**
     * Checks that does this buffer contain any commands
     * @return true if any command has been written to buffer otherwise false
     */
    public boolean containsData() {
        return containsData;
    }

    /**
     * Checks that does this buffer contain any draw commands, i.e. any setters etc.
     * that does to cause any pixels to be rendered are ignored
     * @return true if any draw command has been written to buffer otherwise false
     */
    public boolean containsDrawnPrimitives() {
        return containsPrimitiveData;
    }

    /**
     * Binds this buffer
     */
    void bind() {
        bound = true;
    }

    /**
     * Provides the binding status of this buffer
     * @return true if this buffer has been already bound otherwise false
     */
    boolean isBound() {
        return bound;
    }

    /**
     * Releases this buffer, i.e. it can be bound by some other GraphicsContext
     */
    void release() {
        bound = false;
    }

    private void ensureIntArraySpace(final int items) {
        if( intParams.length < intCount + items) {
            int[] dst = new int[intParams.length + Math.max(INT_BUF_GRANULARITY, items)];
            System.arraycopy(intParams, 0, dst, 0, intParams.length);
            intParams = dst;
        }
        containsData = true;
    }

    private void reportNotSupported() {
        throw new RuntimeException("Intenal: Operation not supported with JavaCommandBuffer");
    }

//    private void printBufferInfo() {
//        System.out.println("CommandBuffer Info: " +this);
//        System.out.println("intParamCount: " + intCount);
//        System.out.println("intBuffer Size: " + intParams.length);
//        System.out.println("StringBuffer Size: " + strParams.length());
//    }

    private void raisePrimitiveFlag() {
        containsPrimitiveData = true;
    }

    int[] intParams() {
        return intParams;
    }

    int intParamCount() {
        return intCount;
    }

    Vector rgbParams() {
        return rgbData;
    }

    Vector images() {
        return images;
    }

    String strParams() {
        return strParams.toString();
    }

    void drawArc (final int x, final int y, final int width, final int height, final int startAngle, final int arcAngle) {
        ensureIntArraySpace(7);
        intParams[intCount++] = OP_DRAWARC;
        intParams[intCount++] = x;
        intParams[intCount++] = y;
        intParams[intCount++] = width;
        intParams[intCount++] = height;
        intParams[intCount++] = startAngle;
        intParams[intCount++] = arcAngle;
        raisePrimitiveFlag();
    }

    void drawFocus (final int x, final int y, final int width, final int height) {
        ensureIntArraySpace(5);
        intParams[intCount++] = OP_DRAWFOCUS;
        intParams[intCount++] = x;
        intParams[intCount++] = y;
        intParams[intCount++] = width;
        intParams[intCount++] = height;
        raisePrimitiveFlag();
    }

    // Must be called from UI thread as images cannot be creates outside that
    void drawImage(final Image image, final int x, final int y) {
        ensureIntArraySpace(3);
        intParams[intCount++] = OP_DRAWIMAGE1;
        intParams[intCount++] = x;
        intParams[intCount++] = y;
        // creating copy of image here uses implicit data sharing,
        // thus only a shallow copy is made
        images.addElement(image.getCommandBufferCopy());
        raisePrimitiveFlag();
    }

    // must be called from UI thread as images cannot be creates outside that
    void drawImage(final Image image, final int tx, final int ty, final int tw, final int th, final int sx, final int sy, final int sw, final int sh, final int manipulation) {
        ensureIntArraySpace(10);
        intParams[intCount++] = OP_DRAWIMAGE2;
        intParams[intCount++] = tx;
        intParams[intCount++] = ty;
        intParams[intCount++] = tw;
        intParams[intCount++] = th;
        intParams[intCount++] = sx;
        intParams[intCount++] = sy;
        intParams[intCount++] = sw;
        intParams[intCount++] = sh;
        intParams[intCount++] = manipulation;
        // creating copy of image here uses implicit data sharing,
        // thus only a shallow copy is made
        images.addElement(image.getCommandBufferCopy());
        raisePrimitiveFlag();
    }

    void drawLine (final int x1, final int y1, final int x2, final int y2) {
        ensureIntArraySpace(5);
        intParams[intCount++] = OP_DRAWLINE;
        intParams[intCount++] = x1;
        intParams[intCount++] = y1;
        intParams[intCount++] = x2;
        intParams[intCount++] = y2;
        raisePrimitiveFlag();
    }

    void drawEllipse (final int x, final int y, final int width, final int height) {
        ensureIntArraySpace(5);
        intParams[intCount++] = OP_DRAWELLIPSE;
        intParams[intCount++] = x;
        intParams[intCount++] = y;
        intParams[intCount++] = width;
        intParams[intCount++] = height;
        raisePrimitiveFlag();
    }

    void drawPoint (final int x, final int y) {
        ensureIntArraySpace(3);
        intParams[intCount++] = OP_DRAWPOINT;
        intParams[intCount++] = x;
        intParams[intCount++] = y;
        raisePrimitiveFlag();
    }

    void drawPolygon(final int[] pointArray) {
        ensureIntArraySpace(2 + pointArray.length);
        intParams[intCount++] = OP_DRAWPOLYGON;
        intParams[intCount++] = pointArray.length;
        for(int i = 0; i < pointArray.length; ++i) {
            intParams[intCount++] = pointArray[i];
        }
        raisePrimitiveFlag();
    }

    void drawPolyline(final int[] pointArray) {
        ensureIntArraySpace(2 + pointArray.length);
        intParams[intCount++] = OP_DRAWPOLYLINE;
        intParams[intCount++] = pointArray.length;
        for(int i = 0; i < pointArray.length; ++i) {
            intParams[intCount++] = pointArray[i];
        }
        raisePrimitiveFlag();
    }

    void drawRect (final int x, final int y, final int width, final int height) {
        ensureIntArraySpace(5);
        intParams[intCount++] = OP_DRAWRECT;
        intParams[intCount++] = x;
        intParams[intCount++] = y;
        intParams[intCount++] = width;
        intParams[intCount++] = height;
        raisePrimitiveFlag();
    }

    void drawRGB(final int[] rgb, final int offset, final int scanlength, final int x, final int y, final int width, final int height, final boolean processAlpha, final int manipulation) {
        ensureIntArraySpace(9);
        intParams[intCount++] = OP_DRAWRGB_INT;
        intParams[intCount++] = offset;
        intParams[intCount++] = scanlength;
        intParams[intCount++] = x;
        intParams[intCount++] = y;
        intParams[intCount++] = width;
        intParams[intCount++] = height;
        intParams[intCount++] = processAlpha? 1 : 0;
        intParams[intCount++] = manipulation;
        rgbData.addElement(rgb);
        raisePrimitiveFlag();
    }

    void drawRGB(final byte[] rgb, final byte[] transparencyMask, final int offset, final int scanlength, final int x, final int y, final int width, final int height, final int manipulation, final int format) {
        ensureIntArraySpace(9);
        intParams[intCount++] = OP_DRAWRGB_BYTE;
        intParams[intCount++] = offset;
        intParams[intCount++] = scanlength;
        intParams[intCount++] = x;
        intParams[intCount++] = y;
        intParams[intCount++] = width;
        intParams[intCount++] = height;
        intParams[intCount++] = manipulation;
        intParams[intCount++] = format;
        rgbData.addElement(rgb);
        rgbData.addElement(transparencyMask);
        raisePrimitiveFlag();
    }

    void drawRGB(final short[] rgb, final int offset, final int scanlength, final int x, final int y, final int width, final int height, final boolean processAlpha, final int manipulation, final int format) {
        ensureIntArraySpace(10);
        intParams[intCount++] = OP_DRAWRGB_SHORT;
        intParams[intCount++] = offset;
        intParams[intCount++] = scanlength;
        intParams[intCount++] = x;
        intParams[intCount++] = y;
        intParams[intCount++] = width;
        intParams[intCount++] = height;
        intParams[intCount++] = processAlpha? 1 : 0;
        intParams[intCount++] = manipulation;
        intParams[intCount++] = format;
        rgbData.addElement(rgb);
        raisePrimitiveFlag();
    }

    void drawRoundRect (final int x, final int y, final int width, final int height, final int arcWidth, final int arcHeight) {
        ensureIntArraySpace(7);
        intParams[intCount++] = OP_DRAWROUNDRECT;
        intParams[intCount++] = x;
        intParams[intCount++] = y;
        intParams[intCount++] = width;
        intParams[intCount++] = height;
        intParams[intCount++] = arcWidth;
        intParams[intCount++] = arcHeight;
        raisePrimitiveFlag();
    }

    void drawString(final String string, final int x, final int y, final int width, final int height, final int alignments, final int flags, final boolean isTransparent) {
        ensureIntArraySpace(9);
        intParams[intCount++] = OP_DRAWSTRING;
        intParams[intCount++] = string.length();
        intParams[intCount++] = x;
        intParams[intCount++] = y;
        intParams[intCount++] = width;
        intParams[intCount++] = height;
        intParams[intCount++] = alignments;
        intParams[intCount++] = flags;
        intParams[intCount++] = isTransparent? 1 : 0;
        strParams.append(string);
        raisePrimitiveFlag();
    }

    void fillArc (final int x, final int y, final int width, final int height, final int startAngle, final int arcAngle) {
        ensureIntArraySpace(7);
        intParams[intCount++] = OP_FILLARC;
        intParams[intCount++] = x;
        intParams[intCount++] = y;
        intParams[intCount++] = width;
        intParams[intCount++] = height;
        intParams[intCount++] = startAngle;
        intParams[intCount++] = arcAngle;
        raisePrimitiveFlag();
    }

    void fillGradientRect(final int x, final int y, final int width, final int height, final boolean vertical, final boolean swapColors) {
        ensureIntArraySpace(7);
        intParams[intCount++] = OP_FILLGRADIENTRECT;
        intParams[intCount++] = x;
        intParams[intCount++] = y;
        intParams[intCount++] = width;
        intParams[intCount++] = height;
        intParams[intCount++] = vertical ? 1 : 0;
        intParams[intCount++] = swapColors ? 1 : 0;
        raisePrimitiveFlag();
    }

    void fillEllipse (final int x, final int y, final int width, final int height) {
        ensureIntArraySpace(5);
        intParams[intCount++] = OP_FILLELLIPSE;
        intParams[intCount++] = x;
        intParams[intCount++] = y;
        intParams[intCount++] = width;
        intParams[intCount++] = height;
        raisePrimitiveFlag();
    }

    void fillPolygon (final int[] pointArray) {
        ensureIntArraySpace(2 + pointArray.length);
        intParams[intCount++] = OP_FILLPOLYGON;
        intParams[intCount++] = pointArray.length;
        for(int i = 0; i < pointArray.length; ++i) {
            intParams[intCount++] = pointArray[i];
        }
        raisePrimitiveFlag();
    }

    void fillRect (final int x, final int y, final int width, final int height) {
        ensureIntArraySpace(5);
        intParams[intCount++] = OP_FILLRECT;
        intParams[intCount++] = x;
        intParams[intCount++] = y;
        intParams[intCount++] = width;
        intParams[intCount++] = height;
        raisePrimitiveFlag();
    }

    void fillRoundRectangle (final int x, final int y, final int width, final int height, final int arcWidth, final int arcHeight) {
        ensureIntArraySpace(7);
        intParams[intCount++] = OP_FILLROUNDRECT;
        intParams[intCount++] = x;
        intParams[intCount++] = y;
        intParams[intCount++] = width;
        intParams[intCount++] = height;
        intParams[intCount++] = arcWidth;
        intParams[intCount++] = arcHeight;
        raisePrimitiveFlag();
    }

    public void setBackgroundAlpha(final int alpha) {
        ensureIntArraySpace(2);
        intParams[intCount++] = OP_SETBACKGROUNDALPHA;
        intParams[intCount++] = alpha;
    }

    void setBackgroundColor(final int argb, final boolean updateAlpha) {
        ensureIntArraySpace(3);
        intParams[intCount++] = OP_SETBACKGROUNDCOLOR;
        intParams[intCount++] = argb;
        intParams[intCount++] = updateAlpha? 1 : 0;
    }

    void setBlendingMode(final int mode) {
        ensureIntArraySpace(2);
        intParams[intCount++] = OP_SETBLENDINGMODE;
        intParams[intCount++] = mode;
    }

    void setClip(final int x, final int y, final int width, final int height, final boolean intersects) {
        ensureIntArraySpace(6);
        intParams[intCount++] = OP_SETCLIP;
        intParams[intCount++] = x;
        intParams[intCount++] = y;
        intParams[intCount++] = width;
        intParams[intCount++] = height;
        intParams[intCount++] = intersects? 1 : 0;
    }

    void cancelClipping () {
        ensureIntArraySpace(1);
        intParams[intCount++] = OP_CANCELCLIPPING;
    }

    void setFont(final int fontHandle) {
        ensureIntArraySpace(2);
        intParams[intCount++] = OP_SETFONT;
        intParams[intCount++] = fontHandle;
    }

    void setForegroundAlpha(final int alpha) {
        ensureIntArraySpace(2);
        intParams[intCount++] = OP_SETFOREGROUNDALPHA;
        intParams[intCount++] = alpha;
    }

    void setForegroundColor(final int argb, final boolean updateAlpha) {
        ensureIntArraySpace(3);
        intParams[intCount++] = OP_SETFOREGROUNDCOLOR;
        intParams[intCount++] = argb;
        intParams[intCount++] = updateAlpha? 1 : 0;
    }

    void setStrokeStyle(final int style) {
        ensureIntArraySpace(2);
        intParams[intCount++] = OP_SETSTROKESTYLE;
        intParams[intCount++] = style;
    }

    void setStrokeWidth(final int width) {
        ensureIntArraySpace(2);
        intParams[intCount++] = OP_SETSTROKEWIDTH;
        intParams[intCount++] = width;
    }

    void translate(final int x, final int y) {
        ensureIntArraySpace(3);
        intParams[intCount++] = OP_TRANSLATE;
        intParams[intCount++] = x;
        intParams[intCount++] = y;
    }

    void scale(final int x, final int y) {
        ensureIntArraySpace(3);
        intParams[intCount++] = OP_SCALE;
        intParams[intCount++] = x;
        intParams[intCount++] = y;
    }

    void resetTransform() {
        ensureIntArraySpace(1);
        intParams[intCount++] = OP_RESETTRANSFORM;
    }

    void copyArea(final Image image, final int x, final int y) {
        ensureIntArraySpace(3);
        intParams[intCount++] = OP_COPYAREA1;
        intParams[intCount++] = x;
        intParams[intCount++] = y;
        // TODO does this need flushing on the image
        images.addElement(new Image(image));
        raisePrimitiveFlag();
    }

    void copyArea(final int srcX, final int srcY, final int width, final int height, final int destX, final int destY, final boolean paint) {
        ensureIntArraySpace(8);
        intParams[intCount++] = OP_COPYAREA2;
        intParams[intCount++] = srcX;
        intParams[intCount++] = srcY;
        intParams[intCount++] = width;
        intParams[intCount++] = height;
        intParams[intCount++] = destX;
        intParams[intCount++] = destY;
        intParams[intCount++] = paint? 1 : 0;
        raisePrimitiveFlag();
    }

    // Unsupported operations
    int getAdvancedCharacterWidth(final char ch, final boolean isAdvanced) {
        reportNotSupported();
        return 0;
    }

    void getFontMetricsData(final int[] data, final int fontHandle) {
        reportNotSupported();
    }

    int getBackgroundAlpha() {
        reportNotSupported();
        return 0;
    }

    int getBackgroundColor() {
        reportNotSupported();
        return 0;
    }

    int getBlendingMode() {
        reportNotSupported();
        return 0;
    }

    void getClip(final int[] clip) {
        reportNotSupported();
    }

    int getForegroundAlpha() {
        reportNotSupported();
        return 0;
    }

    int getForegroundColor() {
        reportNotSupported();
        return 0;
    }

    void getTextBoundingBox(final int[] boundingBox, final String string, final int alignments, final int flags, final int rectX, final int rectY, final int rectWidth, final int rectHeight) {
        reportNotSupported();
    }

    int getStrokeWidth() {
        reportNotSupported();
        return 0;
    }

    int getStrokeStyle() {
        reportNotSupported();
        return 0;
    }

    int getTranslateX() {
        reportNotSupported();
        return 0;
    }

    int getTranslateY() {
        reportNotSupported();
        return 0;
    }

    boolean hasClipping() {
        reportNotSupported();
        return false;
    }

    void saveSettings() {
        reportNotSupported();
    }

    void restoreSettings() {
        reportNotSupported();
    }
}
