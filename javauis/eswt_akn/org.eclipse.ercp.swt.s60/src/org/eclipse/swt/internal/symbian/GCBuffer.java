/*******************************************************************************
 * Copyright (c) 2008 Nokia Corporation.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/
package org.eclipse.swt.internal.symbian;

import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.RGB;

/**
 * This class implements a command buffer that is used internally in the
 * API implementation to collect the drawing operations done in a paint
 * listener with the graphics context that is passed in as a parameter of
 * the PaintEvent. The operations can then be executed efficiently in
 * the same redraw operation with native content of the Control.
 */
public final class GCBuffer
{

    // All collected drawing operations and their parameters
    int[] intParams;
    int intCount;
    StringBuffer strParams;

    // Handles that are passed through the buffer are stored also
    // here for controlling the reference counts. This is required
    // to guarantee that the handles are still valid when flushing.
    int[] imageHandles;
    int imageCount;
    int[] fontHandles;
    int fontCount;

    // This flag is set when a paint listener is using the buffer.
    // Used for checking if the shared buffer is available.
    boolean inUse;

    // Memory management configuration constants
    final int INT_BUF_GRANULARITY = 10;
    final int INT_BUF_INITIAL_SIZE = 10;
    final int HANDLE_BUF_INITIAL_SIZE = 5;
    final int HANDLE_BUF_GRANULARITY = 5;

    // Unique operation codes for all the gc operations that can be buffered.
    final int OP_DRAWARC = 1;
    final int OP_DRAWFOCUS = 2;
    final int OP_DRAWIMAGE = 3;
    final int OP_DRAWLINE = 4;
    final int OP_DRAWOVAL = 5;
    final int OP_DRAWPOINT = 6;
    final int OP_DRAWPOLYGON = 7;
    final int OP_DRAWPOLYLINE = 8;
    final int OP_DRAWRECTANGLE = 9;
    final int OP_DRAWROUNDRECTANGLE = 10;
    final int OP_DRAWTEXT = 11;
    final int OP_FILLARC = 12;
    final int OP_FILLGRADIENTRECTANGLE = 13;
    final int OP_FILLOVAL = 14;
    final int OP_FILLPOLYGON = 15;
    final int OP_FILLRECTANGLE = 16;
    final int OP_FILLROUNDRECTANGLE = 17;
    final int OP_SETALPHA = 18;
    final int OP_SETBACKGROUND = 19;
    final int OP_SETCLIPPING = 20;
    final int OP_CANCELCLIPPING = 21;
    final int OP_SETFONT = 22;
    final int OP_SETFOREGROUND = 23;
    final int OP_SETLINESTYLE = 24;
    final int OP_SETLINEWIDTH = 25;
    final int OP_SETXORMODE = 26;
    final int OP_COPYAREA = 27;
    final int OP_DRAWM2GIMAGE = 28;
    final int OP_COPYAREATOIMAGE = 29;

    // To be used for debugging, enable the checks in native code
    //protected final int OP_END = Integer.MAX_VALUE;

    public GCBuffer()
    {
        intParams = new int[INT_BUF_INITIAL_SIZE];
        strParams = new StringBuffer();
        imageHandles = new int[HANDLE_BUF_INITIAL_SIZE];
        fontHandles = new int[HANDLE_BUF_INITIAL_SIZE];
    }

    public void setReserved()
    {
        inUse = true;
    }

    public boolean isFree()
    {
        return !inUse;
    }

    public void setFree()
    {
        inUse = false;
        intCount = 0;
        strParams.setLength(0);
        for (int i = 0; i < imageCount; ++i)
        {
            OS.Image_RemoveRef(imageHandles[i]);
        }
        imageCount = 0;
        for (int i = 0; i < fontCount; ++i)
        {
            OS.Font_RemoveRef(fontHandles[i]);
        }
        fontCount = 0;
    }

    int[] writeToArray(int item, int[] array, int elementsUsed, int granularity)
    {
        if (array.length < elementsUsed + 1)
        {
            int[] src = array;
            int[] dst = new int[array.length + granularity];
            System.arraycopy(src, 0, dst, 0, src.length);
            array = dst;
        }
        array[elementsUsed] = item;
        return array;
    }

    void writeInt(int param)
    {
        intParams = writeToArray(param, intParams, intCount++, INT_BUF_GRANULARITY);
    }

    void addFontRef(int handle)
    {
        fontHandles = writeToArray(handle, fontHandles, fontCount++, HANDLE_BUF_GRANULARITY);
        OS.Font_AddRef(handle);
    }

    void addImageRef(int handle)
    {
        imageHandles = writeToArray(handle, imageHandles, imageCount++, HANDLE_BUF_GRANULARITY);
        OS.Image_AddRef(handle);
    }

    void writeStr(String string)
    {
        strParams.append(string);
    }

    public int[] intParams()
    {
        return intParams;
    }

    public int intParamCount()
    {
        return intCount;
    }

    public String strParams()
    {
        return strParams.toString();
    }

    int getRgb(Color color)
    {
        RGB rgb = color.getRGB();
        return (rgb.red << 16) | (rgb.green << 8)| rgb.blue;
    }

    public void drawArc(int x, int y, int width, int height, int startAngle, int arcAngle)
    {
        writeInt(OP_DRAWARC);
        writeInt(x);
        writeInt(y);
        writeInt(width);
        writeInt(height);
        writeInt(startAngle);
        writeInt(arcAngle);
        //writeInt(OP_END);
    }

    public void drawFocus(int x, int y, int width, int height)
    {
        writeInt(OP_DRAWFOCUS);
        writeInt(x);
        writeInt(y);
        writeInt(width);
        writeInt(height);
        //writeInt(OP_END);
    }

    public void drawImage(Image image, int srcX, int srcY, int srcWidth, int srcHeight, int destX, int destY, int destWidth, int destHeight)
    {
        writeInt(OP_DRAWIMAGE);
        writeInt(image.handle);
        addImageRef(image.handle);
        writeInt(srcX);
        writeInt(srcY);
        writeInt(srcWidth);
        writeInt(srcHeight);
        writeInt(destX);
        writeInt(destY);
        writeInt(destWidth);
        writeInt(destHeight);
        //writeInt(OP_END);
    }

    public void drawLine(int x1, int y1, int x2, int y2)
    {
        writeInt(OP_DRAWLINE);
        writeInt(x1);
        writeInt(y1);
        writeInt(x2);
        writeInt(y2);
        //writeInt(OP_END);
    }

    public void drawOval(int x, int y, int width, int height)
    {
        writeInt(OP_DRAWOVAL);
        writeInt(x);
        writeInt(y);
        writeInt(width);
        writeInt(height);
        //writeInt(OP_END);
    }

    public void drawPoint(int x, int y)
    {
        writeInt(OP_DRAWPOINT);
        writeInt(x);
        writeInt(y);
        //writeInt(OP_END);
    }

    public void drawPolygon(int[] pointArray)
    {
        writeInt(OP_DRAWPOLYGON);
        writeInt(pointArray.length);
        for (int i = 0; i < pointArray.length; ++i)
        {
            writeInt(pointArray[i]);
        }
        //writeInt(OP_END);
    }

    public void drawPolyline(int[] pointArray)
    {
        writeInt(OP_DRAWPOLYLINE);
        writeInt(pointArray.length);
        for (int i = 0; i < pointArray.length; ++i)
        {
            writeInt(pointArray[i]);
        }
        //writeInt(OP_END);
    }

    public void drawRectangle(int x, int y, int width, int height)
    {
        writeInt(OP_DRAWRECTANGLE);
        writeInt(x);
        writeInt(y);
        writeInt(width);
        writeInt(height);
        //writeInt(OP_END);
    }

    public void drawRoundRectangle(int x, int y, int width, int height, int arcWidth, int arcHeight)
    {
        writeInt(OP_DRAWROUNDRECTANGLE);
        writeInt(x);
        writeInt(y);
        writeInt(width);
        writeInt(height);
        writeInt(arcWidth);
        writeInt(arcHeight);
        //writeInt(OP_END);
    }

    public void drawText(String string, int x, int y, int flags)
    {
        writeInt(OP_DRAWTEXT);
        writeStr(string);
        writeInt(string.length());
        writeInt(x);
        writeInt(y);
        writeInt(flags);
        //writeInt(OP_END);
    }

    public void fillArc(int x, int y, int width, int height, int startAngle, int arcAngle)
    {
        writeInt(OP_FILLARC);
        writeInt(x);
        writeInt(y);
        writeInt(width);
        writeInt(height);
        writeInt(startAngle);
        writeInt(arcAngle);
        //writeInt(OP_END);
    }

    public void fillGradientRectangle(int x, int y, int width, int height, boolean vertical)
    {
        writeInt(OP_FILLGRADIENTRECTANGLE);
        writeInt(x);
        writeInt(y);
        writeInt(width);
        writeInt(height);
        writeInt(vertical ? 1 : 0);
        //writeInt(OP_END);
    }

    public void fillOval(int x, int y, int width, int height)
    {
        writeInt(OP_FILLOVAL);
        writeInt(x);
        writeInt(y);
        writeInt(width);
        writeInt(height);
        //writeInt(OP_END);
    }

    public void fillPolygon(int[] pointArray)
    {
        writeInt(OP_FILLPOLYGON);
        writeInt(pointArray.length);
        for (int i = 0; i < pointArray.length; ++i)
        {
            writeInt(pointArray[i]);
        }
        //writeInt(OP_END);
    }

    public void fillRectangle(int x, int y, int width, int height)
    {
        writeInt(OP_FILLRECTANGLE);
        writeInt(x);
        writeInt(y);
        writeInt(width);
        writeInt(height);
        //writeInt(OP_END);
    }

    public void fillRoundRectangle(int x, int y, int width, int height, int arcWidth, int arcHeight)
    {
        writeInt(OP_FILLROUNDRECTANGLE);
        writeInt(x);
        writeInt(y);
        writeInt(width);
        writeInt(height);
        writeInt(arcWidth);
        writeInt(arcHeight);
        //writeInt(OP_END);
    }

    public void setAlpha(int alpha)
    {
        writeInt(OP_SETALPHA);
        writeInt(alpha);
        //writeInt(OP_END);
    }

    public void setBackground(Color color)
    {
        writeInt(OP_SETBACKGROUND);
        writeInt(getRgb(color));
        //writeInt(OP_END);
    }

    public void setClipping(int x, int y, int width, int height)
    {
        writeInt(OP_SETCLIPPING);
        writeInt(x);
        writeInt(y);
        writeInt(width);
        writeInt(height);
        //writeInt(OP_END);
    }

    public void cancelClipping()
    {
        writeInt(OP_CANCELCLIPPING);
        //writeInt(OP_END);
    }

    public void setFont(Font font)
    {
        writeInt(OP_SETFONT);
        int handle = (font == null ? 0 : font.handle);
        writeInt(handle);
        if (handle != 0)
        {
            addFontRef(font.handle);
        }
        //writeInt(OP_END);
    }

    public void setForeground(Color color)
    {
        writeInt(OP_SETFOREGROUND);
        writeInt(getRgb(color));
        //writeInt(OP_END);
    }

    public void setLineStyle(int lineStyle)
    {
        writeInt(OP_SETLINESTYLE);
        writeInt(lineStyle);
        //writeInt(OP_END);
    }

    public void setLineWidth(int lineWidth)
    {
        writeInt(OP_SETLINEWIDTH);
        writeInt(lineWidth);
        //writeInt(OP_END);
    }

    public void setXORMode(boolean xor)
    {
        writeInt(OP_SETXORMODE);
        writeInt(xor ? 1 : 0);
        //writeInt(OP_END);
    }

    public void copyAreaToImage(int imageHandle, int srcX, int srcY)
    {
        writeInt(OP_COPYAREATOIMAGE);
        writeInt(imageHandle);
        writeInt(srcX);
        writeInt(srcY);
        //writeInt(OP_END);
    }

    public void copyArea(int srcX, int srcY, int width, int height, int destX, int destY)
    {
        writeInt(OP_COPYAREA);
        writeInt(srcX);
        writeInt(srcY);
        writeInt(width);
        writeInt(height);
        writeInt(destX);
        writeInt(destY);
        //writeInt(OP_END);
    }
    
    public void drawM2GImage(int[] bitmapHandles, int srcX, int srcY, int srcWidth, int srcHeight, int destX, int destY, int destWidth, int destHeight, int useNativeClear)
    {
        writeInt(OP_DRAWM2GIMAGE);
        writeInt(bitmapHandles[0]);
        writeInt(bitmapHandles[1]);
        writeInt(bitmapHandles[2]);
        writeInt(bitmapHandles[3]);
        writeInt(srcX);
        writeInt(srcY);
        writeInt(srcWidth);
        writeInt(srcHeight);
        writeInt(destX);
        writeInt(destY);
        writeInt(destWidth);
        writeInt(destHeight);
        writeInt(useNativeClear);
        //writeInt(OP_END);
    }
}
