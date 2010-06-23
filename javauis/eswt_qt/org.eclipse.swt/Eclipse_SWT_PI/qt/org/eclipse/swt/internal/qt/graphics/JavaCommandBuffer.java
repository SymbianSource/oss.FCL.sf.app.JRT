/*******************************************************************************
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

    // Holder for images
    private Vector images;

    // holder for rgbData
    private Vector rgbData;

    // This flag is indicates if this buffer is bound by some GraphicsContext
    private boolean bound;

    // This flag indicates if this buffer is empty or not
    private boolean containsData;
    
    // This flag indicates if this buffer has any draw commands in it
    private boolean containsPrimitiveData;

    // Memory management configuration constants
    private static final int INT_BUF_GRANULARITY         = 10;
    private static final int INT_BUF_INITIAL_SIZE        = 250;
    private static final int IMAGE_BUF_INITIAL_SIZE      = 3;
    private static final int IMAGE_BUF_GRANULARITY       = 2;
    private static final int STRING_BUFFER_INITIAL_SIZE = 10;

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

    /**
     * Writes an integer to given array.
     * @param item The item to be added to the array
     * @param array The array where to append given item
     * @param elementsUsed The size of slots used in given array
     * @param granularity The granularity used if the array needs to be enlarged
     * @return Array containing the added item
     */
    private int[] writeToArray(int item, int[] array, int elementsUsed, int granularity) {
        if( array.length < elementsUsed + 1) {
            int[] src = array;
            int[] dst = new int[array.length + granularity];
            System.arraycopy(src, 0, dst, 0, src.length);
            array = dst;
        }
        array[elementsUsed] = item;
        return array;
    }

    private void writeInt(int param) {
        intParams = writeToArray(param, intParams, intCount++, INT_BUF_GRANULARITY);
        containsData = true;
    }

    private void writeImage(Image image) {
        images.addElement(image);
    }

    private void writeStr(String string) {
        strParams.append(string);
    }

    private void writeRgb(int[] rgb) {
        rgbData.addElement(rgb);
    }
    private void writeRgb(byte[] rgb) {
        rgbData.addElement(rgb);
    }

    private void writeRgb(short[] rgb) {
        rgbData.addElement(rgb);
    }

    private void reportNotSupported() {
        throw new RuntimeException("Intenal: Operation not supported with JavaCommandBuffer");
    }

    private void printBufferInfo() {
        System.out.println("CommandBuffer Info: " +this);
        System.out.println("intParamCount: " + intCount);
        System.out.println("intBuffer Size: " + intParams.length);
        System.out.println("StringBuffer Size: " + strParams.length());
    }

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

    void drawArc (int x, int y, int width, int height, int startAngle, int arcAngle) {
        writeInt(OP_DRAWARC);
        writeInt(x);
        writeInt(y);
        writeInt(width);
        writeInt(height);
        writeInt(startAngle);
        writeInt(arcAngle);
        raisePrimitiveFlag();
    }

    void drawFocus (int x, int y, int width, int height) {
        writeInt(OP_DRAWFOCUS);
        writeInt(x);
        writeInt(y);
        writeInt(width);
        writeInt(height);
        raisePrimitiveFlag();
    }

    // must be called from UI thread as images cannot be creates outside that
    void drawImage(Image image, int x, int y) {
        writeInt(OP_DRAWIMAGE1);
        writeInt(x);
        writeInt(y);
        // creating copy of image here uses implicit data sharing,
        // thus only a shallow copy is made
        writeImage(image.getCommandBufferCopy());
        raisePrimitiveFlag();
    }

    // must be called from UI thread as images cannot be creates outside that
    void drawImage(Image image, int tx, int ty, int tw, int th,int sx, int sy, int sw, int sh, int manipulation) {
        writeInt(OP_DRAWIMAGE2);
        writeInt(tx);
        writeInt(ty);
        writeInt(tw);
        writeInt(th);
        writeInt(sx);
        writeInt(sy);
        writeInt(sw);
        writeInt(sh);
        writeInt(manipulation);
        // creating copy of image here uses implicit data sharing,
        // thus only a shallow copy is made
        writeImage(image.getCommandBufferCopy());
        raisePrimitiveFlag();
    }

    void drawLine (int x1, int y1, int x2, int y2) {
        writeInt(OP_DRAWLINE);
        writeInt(x1);
        writeInt(y1);
        writeInt(x2);
        writeInt(y2);
        raisePrimitiveFlag();
    }

    void drawEllipse (int x, int y, int width, int height) {
        writeInt(OP_DRAWELLIPSE);
        writeInt(x);
        writeInt(y);
        writeInt(width);
        writeInt(height);
        raisePrimitiveFlag();
    }

    void drawPoint (int x, int y) {
        writeInt(OP_DRAWPOINT);
        writeInt(x);
        writeInt(y);
        raisePrimitiveFlag();
    }

    void drawPolygon(int[] pointArray) {
        writeInt(OP_DRAWPOLYGON);
        writeInt(pointArray.length);
        for(int i = 0; i < pointArray.length; ++i) {
            writeInt(pointArray[i]);
        }
        raisePrimitiveFlag();
    }

    void drawPolyline(int[] pointArray) {
        writeInt(OP_DRAWPOLYLINE);
        writeInt(pointArray.length);
        for(int i = 0; i < pointArray.length; ++i) {
            writeInt(pointArray[i]);
        }
        raisePrimitiveFlag();
    }

    void drawRect (int x, int y, int width, int height) {
        writeInt(OP_DRAWRECT);
        writeInt(x);
        writeInt(y);
        writeInt(width);
        writeInt(height);
        raisePrimitiveFlag();
    }

    void drawRGB(int[] rgbData, int offset, int scanlength, int x, int y, int width, int height, boolean processAlpha, int manipulation) {
        writeInt(OP_DRAWRGB_INT);
        writeInt(offset);
        writeInt(scanlength);
        writeInt(x);
        writeInt(y);
        writeInt(width);
        writeInt(height);
        writeInt(processAlpha? 1 : 0);
        writeInt(manipulation);
        writeRgb(rgbData);
        raisePrimitiveFlag();
    }

    void drawRGB(byte[] rgbData, byte[] transparencyMask,int offset, int scanlength, int x, int y, int width, int height, int manipulation, int format) {
        writeInt(OP_DRAWRGB_BYTE);
        writeInt(offset);
        writeInt(scanlength);
        writeInt(x);
        writeInt(y);
        writeInt(width);
        writeInt(height);
        writeInt(manipulation);
        writeInt(format);
        writeRgb(rgbData);
        writeRgb(transparencyMask);
        raisePrimitiveFlag();
    }

    void drawRGB(short[] rgbData, int offset, int scanlength, int x, int y, int width, int height, boolean processAlpha, int manipulation, int format) {
        writeInt(OP_DRAWRGB_SHORT);
        writeInt(offset);
        writeInt(scanlength);
        writeInt(x);
        writeInt(y);
        writeInt(width);
        writeInt(height);
        writeInt(processAlpha? 1 : 0);
        writeInt(manipulation);
        writeInt(format);
        writeRgb(rgbData);
        raisePrimitiveFlag();
    }

    void drawRoundRect (int x, int y, int width, int height, int arcWidth, int arcHeight) {
        writeInt(OP_DRAWROUNDRECT);
        writeInt(x);
        writeInt(y);
        writeInt(width);
        writeInt(height);
        writeInt(arcWidth);
        writeInt(arcHeight);
        raisePrimitiveFlag();
    }

    void drawString(String string, int x, int y, int width, int height, int alignments, int flags, boolean isTransparent) {
        writeInt(OP_DRAWSTRING);
        writeStr(string);
        writeInt(string.length());
        writeInt(x);
        writeInt(y);
        writeInt(width);
        writeInt(height);
        writeInt(alignments);
        writeInt(flags);
        writeInt(isTransparent? 1 : 0);
        raisePrimitiveFlag();
    }

    void fillArc (int x, int y, int width, int height, int startAngle, int arcAngle) {
        writeInt(OP_FILLARC);
        writeInt(x);
        writeInt(y);
        writeInt(width);
        writeInt(height);
        writeInt(startAngle);
        writeInt(arcAngle);
        raisePrimitiveFlag();
    }

    void fillGradientRect(int x, int y, int width, int height, boolean vertical, boolean swapColors) {
        writeInt(OP_FILLGRADIENTRECT);
        writeInt(x);
        writeInt(y);
        writeInt(width);
        writeInt(height);
        writeInt(vertical ? 1 : 0);
        writeInt(swapColors ? 1 : 0);
        raisePrimitiveFlag();
    }

    void fillEllipse (int x, int y, int width, int height) {
        writeInt(OP_FILLELLIPSE);
        writeInt(x);
        writeInt(y);
        writeInt(width);
        writeInt(height);
        raisePrimitiveFlag();
    }

    void fillPolygon (int[] pointArray) {
        writeInt(OP_FILLPOLYGON);
        writeInt(pointArray.length);
        for(int i = 0; i < pointArray.length; ++i) {
            writeInt(pointArray[i]);
        }
        raisePrimitiveFlag();
    }

    void fillRect (int x, int y, int width, int height) {
        writeInt(OP_FILLRECT);
        writeInt(x);
        writeInt(y);
        writeInt(width);
        writeInt(height);
        raisePrimitiveFlag();
    }

    void fillRoundRectangle (int x, int y, int width, int height, int arcWidth, int arcHeight) {
        writeInt(OP_FILLROUNDRECT);
        writeInt(x);
        writeInt(y);
        writeInt(width);
        writeInt(height);
        writeInt(arcWidth);
        writeInt(arcHeight);
        raisePrimitiveFlag();
    }

    public void setBackgroundAlpha(int alpha) {
        writeInt(OP_SETBACKGROUNDALPHA);
        writeInt(alpha);
    }

    void setBackgroundColor(int argb, boolean updateAlpha) {
        writeInt(OP_SETBACKGROUNDCOLOR);
        writeInt(argb);
        writeInt(updateAlpha? 1 : 0);
    }

    void setBlendingMode(int mode) {
        writeInt(OP_SETBLENDINGMODE);
        writeInt(mode);
    }

    void setClip(int x, int y, int width, int height, boolean intersects) {
        writeInt(OP_SETCLIP);
        writeInt(x);
        writeInt(y);
        writeInt(width);
        writeInt(height);
        writeInt(intersects? 1 : 0 );
    }

    void cancelClipping () {
        writeInt(OP_CANCELCLIPPING);
    }

    void setFont(int fontHandle) {
        writeInt(OP_SETFONT);
        writeInt(fontHandle);
    }

    void setForegroundAlpha(int alpha) {
        writeInt(OP_SETFOREGROUNDALPHA);
        writeInt(alpha);
    }

    void setForegroundColor(int argb, boolean updateAlpha) {
        writeInt(OP_SETFOREGROUNDCOLOR);
        writeInt(argb);
        writeInt(updateAlpha? 1 : 0);
    }

    void setStrokeStyle(int style) {
        writeInt(OP_SETSTROKESTYLE);
        writeInt(style);
    }

    void setStrokeWidth(int width) {
        writeInt(OP_SETSTROKEWIDTH);
        writeInt(width);
    }

    void translate(int x, int y) {
        writeInt(OP_TRANSLATE);
        writeInt(x);
        writeInt(y);
    }

    void scale(int x, int y) {
        writeInt(OP_SCALE);
        writeInt(x);
        writeInt(y);
    }

    void resetTransform() {
        writeInt(OP_RESETTRANSFORM);
    }

    void copyArea(Image image, int x, int y) {
        writeInt(OP_COPYAREA1);
        writeInt(x);
        writeInt(y);
        // TODO does this need flushing on the image
        images.addElement(new Image(image));
        raisePrimitiveFlag();
    }

    void copyArea(int srcX, int srcY, int width, int height, int destX, int destY, boolean paint) {
        writeInt(OP_COPYAREA2);
        writeInt(srcX);
        writeInt(srcY);
        writeInt(width);
        writeInt(height);
        writeInt(destX);
        writeInt(destY);
        writeInt(paint? 1 : 0);
        raisePrimitiveFlag();
    }

    // Unsupported operations
    int getAdvancedCharacterWidth(char ch, boolean isAdvanced) {
        reportNotSupported();
        return 0;
    }

    void getFontMetricsData(int[] data, int fontHandle) {
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

    void getClip(int[] clip) {
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

    void getTextBoundingBox(int[] boundingBox, String string, int alignments, int flags, int rectX, int rectY, int rectWidth, int rectHeight) {
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
