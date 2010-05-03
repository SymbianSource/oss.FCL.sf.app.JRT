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

import org.eclipse.swt.graphics.ImageData;

final class OS {    
    // Graphics Context
    static final native int  graphicsContext_get_windowsurface(int handle);
    static final native int  graphicsContext_init();
    static final native void graphicsContext_dispose(int handle);
    static final native void graphicsContext_bindTarget(int handle, int target, int type, int bufferFlushTargetHandle);
    static final native void graphicsContext_render(int handle, int buffer);
    static final native void graphicsContext_releaseTarget(int handle);
    static final native void graphicsContext_copyArea(int handle, int targetHandle, int x, int y);
    static final native void graphicsContext_copyArea(int handle, int srcX, int srcY, int width, int height, int destX, int destY, boolean paint);
    static final native void graphicsContext_drawArc(int handle, int x, int y, int width, int height, int startAngle, int arcAngle);
    static final native void graphicsContext_drawEllipse(int handle, int x, int y, int width, int height);
    static final native void graphicsContext_drawFocus(int handle, int x, int y, int width, int height);
    static final native void graphicsContext_drawImage(int handle, int imageHandle, int x, int y);
    static final native void graphicsContext_drawImage(int handle, int imageHandle, int tx, int ty, int tw, 
                                                       int th, int sx, int sy, int sw, int sh, int manipulation);
    static final native void graphicsContext_drawLine(int handle, int x1, int y1, int x2,int y2);
    static final native void graphicsContext_drawPoint(int handle, int x, int y);
    static final native void graphicsContext_drawPolygon(int handle, int[] pointArray);
    static final native void graphicsContext_drawPolyline(int handle, int[] pointArray);
    static final native void graphicsContext_drawRect(int handle, int x, int y, int width, int height);
    static final native void graphicsContext_drawRGB(int handle, int[] rgbData, int offset, int scanlenght, int x, int y, int width, int height, boolean processAlpha, int manipulation);
    static final native void graphicsContext_drawRGB(int handle, byte[] rgbData, byte[] transparencyMask,int offset, int scanlenght, int x, int y, int width, int height, int manipulation, int format);
    static final native void graphicsContext_drawRGB(int handle, short[] rgbData, int offset, int scanlenght, int x, int y, int width, int height, boolean processAlpha, int manipulation, int format);
    static final native void graphicsContext_drawRoundRect(int handle,int x,int y,int width,int height,int arcWidth,int arcHeight);
    static final native void graphicsContext_drawString(int handle, String string, int x, int y, int width, int height, 
                                                        int alignments, int flags, boolean isTransparent);
    static final native void graphicsContext_fillArc(int handle, int x, int y, int width, int height, int startAngle, int arcAngle);
    static final native void graphicsContext_fillEllipse(int handle, int x, int y, int width, int height);
    static final native void graphicsContext_fillGradientRect(int handle, int x, int y, int width, int height, boolean vertical, boolean swapColors);
    static final native void graphicsContext_fillPolygon(int handle, int[]pointArray);
    static final native void graphicsContext_fillRect(int handle, int x, int y, int width, int height);
    static final native void graphicsContext_fillRoundRect(int handle,int x,int y,int width,int height,int arcWidth,int arcHeight);
    static final native int  graphicsContext_getBackgroundAlpha(int handle);
    static final native int  graphicsContext_getBackgroundColor(int handle);
    static final native int  graphicsContext_getCharacterWidth(int handle, char ch, boolean isAdvanced);
    static final native void graphicsContext_getFontMetricsData(int handle, int[] data, int fontHandle);
    static final native int  graphicsContext_getBlendingMode(int handle);
    static final native boolean graphicsContext_hasClipping(int handle);
    static final native void graphicsContext_cancelClipping(int handle);
    static final native void graphicsContext_getClip(int handle, int[] clip);
    static final native int  graphicsContext_getForegroundAlpha(int handle);
    static final native int  graphicsContext_getForegroundColor(int handle);
    static final native void graphicsContext_getTextBoundingBox(int handle, int[] boundingBox, String string, int alignments,  
                                                                int flags, int rectX, int rectY, int rectWidth, int rectHeight);
    static final native int  graphicsContext_getStrokeWidth(int handle);
    static final native int  graphicsContext_getStrokeStyle(int handle);
    static final native int  graphicsContext_getTranslateX(int handle);
    static final native int  graphicsContext_getTranslateY(int hanle);
    static final native void graphicsContext_setBackgroundAlpha(int handle, int alpha);
    static final native void graphicsContext_setBackgroundColor(int handle, int argb, boolean updateAlpha);
    static final native void graphicsContext_setBlendingMode(int handle, int mode);
    static final native void graphicsContext_setClip(int handle, int x, int y, int width, int height, boolean intersects);
    static final native void graphicsContext_setFont(int handle, int fontHandle);
    static final native void graphicsContext_setForegroundAlpha(int handle, int alpha);
    static final native void graphicsContext_setForegroundColor(int handle, int argb, boolean updateAplha);
    static final native void graphicsContext_setStrokeWidth(int handle, int width);
    static final native void graphicsContext_setStrokeStyle(int handle, int style);
    static final native void graphicsContext_translate(int handle, int x, int y);
    static final native void graphicsContext_scale(int handle, int x, int y);
    static final native void graphicsContext_resetTransform(int handle);
    static final native void graphicsContext_saveSettings(int handle);
    static final native void graphicsContext_restoreSettings(int handle);

    // Image
    static final native int image_create(int width, int Height, int fillColor); 
    static final native int image_create(int imageHandle, int x, int y, int width, int height);
    static final native int image_create(ImageData imageData);
    static final native int image_create(int[] argbData, int width, int height, boolean hasAlpha);
    static final native int image_getFormat(int handle);
    static final native int image_getHeight(int handle);
    static final native int image_getWidth(int handle);
    static final native void image_getRGB(int handle, int[] argbData, int offset, int scanlength,int x, int y, int width, int height);
    static final native void image_getRGB(int handle, byte[] argbData, byte[] transparencyMask, int offset, int scanlength,int x, int y, int width, int height, int format);
    static final native void image_getRGB(int handle, short[] argbData, int offset, int scanlength,int x, int y, int width, int height, int format);
    static final native ImageData image_getImageData(int imageHandle);
    static final native void image_transform(int handle, int transform);
    static final native void image_dispose(int handle);
    static final native int image_getPixmapHandle(int handle);
    static final native boolean image_detectCollision(int image1Handle, int transform1, int p1x, int p1y, int r1x1, int r1y1, int r1x2, int r1y2,
                                                      int image2Handle, int transform2, int p2x, int p2y, int r2x1, int r2y1, int r2x2, int r2y2);

    // ImageLoader
    static final native void imageLoader_append(int handle, int length, int offset, byte[] data);
    static final native void imageLoader_beginStream(int handle, int bufferSize);
    static final native int imageLoader_endStream(int handle);
    static final native int imageLoader_init();
    static final native void imageLoader_dispose(int handle);
    static final native int imageLoader_load(int handle, String fileName);
    static final native void imageLoader_setLoadSize(int handle, int width, int height);
    
    // FontUtils
    static final native int  fontUtils_getAscent(int fontHandle);
    static final native void fontUtils_getBoundingRect(int fontHandle, int[] rect, String str);
    static final native void fontUtils_getBoundingRect(int fontHanlde, int[] boundingBox, String string, int alignments, int flags,
            int rectX, int rectY, int rectWidth, int rectHeight);
    static final native int  fontUtils_getDescent(int fontHandle);
    static final native int  fontUtils_getStringWidth(int fontHandle, String text);
    
    // Buffer
    static final native int buffer_init();
    static final native void buffer_dispose(int handle);
    static final native void buffer_getInvalidRect(int handle, int[] rect);
    
    // WindowSurface
    static final native int windowsurface_create(int handle, boolean autoRefresh);
    static final native void windowsurface_beginPaint(int handle, int x, int y, int height, int width);
    static final native void windowsurface_endPaint(int handle);
    static final native void windowsurface_flush(int handle);
    static final native int windowsurface_getType(int handle);
    static final native void windowsurface_dispose(int handle);
    static final native void windowsurface_refresh(int handle);
}
