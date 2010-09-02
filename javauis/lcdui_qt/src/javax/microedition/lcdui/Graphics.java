/*
* Copyright (c) 2009, 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/
package javax.microedition.lcdui;


import org.eclipse.swt.graphics.Internal_GfxPackageSupport;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.internal.qt.graphics.GraphicsContext;

import org.eclipse.swt.internal.qt.graphics.FontUtils;

import com.nokia.mid.ui.DirectGraphics;

/**
 * Implementation of LCDUI <code>Graphics</code> class.
 */
public class Graphics
{

    /**
     * Constant for horizontal center alignment of the text.
     */
    public static final int HCENTER = 1;

    /**
     * Constant for vertical alignment of the image.
     */
    public static final int VCENTER = 2;

    /**
     * Constant for horizontal left alignment of the text.
     */
    public static final int LEFT = 4;

    /**
     * Constant for horizontal right alignment of the text.
     */
    public static final int RIGHT = 8;

    /**
     * Constant for vertical top alignment of the text.
     */
    public static final int TOP = 16;

    /**
     * Constant for vertical bottom alignment of the text.
     */
    public static final int BOTTOM = 32;

    /**
     * Constant for vertical baseline alignment of the text.
     */
    public static final int BASELINE = 64;

    /**
     * Constant for solid stroke style.
     */
    public static final int SOLID = 0;

    /**
     * Constant for dotted stroke style.
     */
    public static final int DOTTED = 1;

    static final int INVALID_STROKE_STYLE = -1;

    static final int RGB_MASK = 0x00FFFFFF;

    static final int OPAQUE_ALPHA = 0xff000000;

    static final int COMPONENT_MASK = 0xFF;

    /**
     * Constants for sync strategy
     */
    static final int SYNC_LEAVE_SURFACE_SESSION_CLOSED = 10;
    static final int SYNC_LEAVE_SURFACE_SESSION_OPEN = 11;
    
    // Set default sync strategy as closed
    private int syncStrategy = SYNC_LEAVE_SURFACE_SESSION_CLOSED; 
    
    private DirectGraphics directGraphics;
    private Buffer graphicsBuffer;
    
    // Cache for settings
    // these members have package visibility,
    // however they meant to be accessed only by 
    // this (Graphics) instance or related (Buffer) 
    // greaphicsBuffer instance
    Font currentFont;
    int currentColor;
    int translateX;
    int translateY;
    int[] currentClip = new int[4];
    int currentStrokeStyle;
    
    private com.nokia.mj.impl.rt.support.Finalizer finalizer;

    // Constructor
    Graphics(Buffer buffer, Rectangle clipRect)
    {
        finalizer = ((finalizer != null) ? finalizer
                     : new com.nokia.mj.impl.rt.support.Finalizer()
        {
            public void finalizeImpl()
            {
                if(finalizer != null)
                {
                    finalizer = null;
                    if(!ESWTUIThreadRunner.isDisposed())
                    {
                        dispose();
                    }
                }
            }
        });
        currentClip[0] = clipRect.x;
        currentClip[1] = clipRect.y;
        currentClip[2] = clipRect.width;
        currentClip[3] = clipRect.height;
        setDefaultSettings();
        graphicsBuffer = buffer;
    }

    /**
     * Disposes objects with native counterparts
     */
    void dispose()
    {
        synchronized(graphicsBuffer)
        {
            graphicsBuffer.removeRef();
        }
    }

    /**
     * Resets Graphics state to initial.
     * Reset does not set the clip.
     */
    void reset()
    {
        synchronized(graphicsBuffer) {
            // setDefaultSettings() must be called 
            // before the setGraphicsDefaults() since
            // graphicsBuffer (Buffer implementation) uses 
            // the member values of this instance when setting the defaults
            setDefaultSettings();
            graphicsBuffer.setGraphicsDefaults(this);
        }
    }

    void setDefaultSettings() 
    {
        currentFont = Buffer.defaultFont;
        currentColor = Buffer.defaultColor;
        currentStrokeStyle = Buffer.defaultStrokeStyle;
        translateX = Buffer.defaultTranslateX;
        translateY = Buffer.defaultTranslateY;
    }
    
    /**
     * Cleans the Canvas background.
     */
    void cleanBackground(Rectangle area)
    {
        int savedColor = currentColor;
        setColor(255, 255, 255);
        fillRect(area.x, area.y, area.width, area.height);
        setColor(savedColor);
    }

    /**
     * Cleans the Canvas background.
     */
    void cleanBackground(int x, int y, int w, int h)
    {
        int savedColor = currentColor;
        setColor(255, 255, 255);
        fillRect(x, y, w, h);
        setColor(savedColor);
    }

    /**
     * Sets the sync strategy for this instance.
     * This affects on the behavior of the sync method of this class
     * which is called via LCDUIInvoker
     */
    void setSyncStrategy(int strategy)
    {
        if((strategy != SYNC_LEAVE_SURFACE_SESSION_CLOSED) && (strategy != SYNC_LEAVE_SURFACE_SESSION_OPEN)) 
        {
            throw new IllegalArgumentException("Internal: Invalid strategy value");
        }
        syncStrategy = strategy;
    }

    /**
     * Sets coordinate translation. Translations are cumulative.
     *
     * @param xDelta x-shift for coordinates.
     * @param yDelta y-shift for coordinates.
     */
    public void translate(int xDelta, int yDelta)
    {
        synchronized(graphicsBuffer)
        {
            graphicsBuffer.translate(xDelta, yDelta, this);
            translateX += xDelta;
            translateY += yDelta;
        }
    }

    /**
     * Returns current X-shift of coordinate translation.
     *
     * @return Current X-shift of coordinate translation.
     */
    public int getTranslateX()
    {
        synchronized(graphicsBuffer)
        {
            return translateX;
        }
    }

    /**
     * Returns current Y-shift of coordinate translation.
     *
     * @return current Y-shift of coordinate translation.
     */
    public int getTranslateY()
    {
        synchronized(graphicsBuffer)
        {
            return translateY;
        }
    }

    /**
     * Returns current color in 0x00RRGGBB format.
     *
     * @return Current color in 0x00RRGGBB format.
     */
    public int getColor()
    {
        synchronized(graphicsBuffer)
        {
            return (currentColor & RGB_MASK);
        }
    }

    /**
     * Returns red component of current color.
     *
     * @return Red component of current color in the range of 0-255.
     */
    public int getRedComponent()
    {
        synchronized(graphicsBuffer)
        {
            return (currentColor >> 16) & COMPONENT_MASK;
        }
    }

    /**
     * Returns green component of current color.
     *
     * @return Green component of current color in the range of 0-255.
     */
    public int getGreenComponent()
    {
        synchronized(graphicsBuffer)
        {
            return (currentColor >> 8) & COMPONENT_MASK;
        }
    }

    /**
     * Returns blue component of current color.
     *
     * @return Blue component of current color in the range of 0-255.
     */
    public int getBlueComponent()
    {
        synchronized(graphicsBuffer)
        {
            return currentColor & COMPONENT_MASK;
        }
    }

    /**
     * Returns current grayscale color.
     *
     * @return Returns current grayscale color in the range 0-255.
     */
    public int getGrayScale()
    {
        synchronized(graphicsBuffer)
        {
            return (getRedComponent() + getGreenComponent() + getBlueComponent()) / 3;
        }
    }

    /**
     * Sets the current color by color components for red, green and blue.
     *
     * @param r - red component of the color to be set.
     * @param g - green component of the color to be set.
     * @param b - blue component of the color to be set.
     */
    public void setColor(int r, int g, int b)
    {
        if(r < 0 || r > 255 ||
                g < 0 || g > 255 ||
                b < 0 || b > 255)
        {
            throw new IllegalArgumentException();
        }
        synchronized(graphicsBuffer)
        {
            graphicsBuffer.setColor(r, g, b, this);
            currentColor = (OPAQUE_ALPHA | (r << 16) | (g << 8) | b);
        }

    }

    /**
     * Sets the current color.
     *
     * @param RGB - color to be set in the form of 0x00RRGGBB.
     */
    public void setColor(int RGB)
    {
        int maskedRGB = RGB & RGB_MASK;
        final int r = maskedRGB >> 16;
        final int g = (maskedRGB >> 8) & COMPONENT_MASK;
        final int b = maskedRGB & COMPONENT_MASK;
        synchronized(graphicsBuffer)
        {
            graphicsBuffer.setColor(r, g, b, this);
            currentColor = (OPAQUE_ALPHA | maskedRGB);
        }

    }

    /**
     * Sets gray-scale color.
     *
     * @param val - gray-scale value to be set in the range of 0-255.
     */
    public void setGrayScale(int val)
    {
        if((val < 0) || (val > 255))
        {
            throw new IllegalArgumentException();
        }
        final int col = val & COMPONENT_MASK;
        synchronized(graphicsBuffer)
        {
            graphicsBuffer.setColor(col, col, col, this);
            currentColor = (OPAQUE_ALPHA | (col << 16) | (col << 8) | col);
        }

    }

    /**
     * Returns the current font.
     *
     * @return Current font.
     */
    public Font getFont()
    {
        synchronized(graphicsBuffer)
        {
            return currentFont;
        }
    }

    /**
     * Sets the font to be used for string rendering.
     *
     * @param newFont - the font to be used for string rendering.
     */
    public void setFont(Font newFont)
    {
        synchronized(graphicsBuffer)
        {
            if(newFont == null)
            {
                newFont = Font.getDefaultFont();
            }
            graphicsBuffer.setFont(Font.getESWTFont(newFont).handle, this);
            currentFont = newFont;
        }
    }

    /**
     * Returns left bound of clip rectangle.
     *
     * @return Left bound of clip rectangle.
     */
    public int getClipX()
    {
        synchronized(graphicsBuffer)
        {
            return currentClip[0];
        }
    }

    /**
     * Returns top bound of clip rectangle.
     *
     * @return Top bound of clip rectangle.
     */
    public int getClipY()
    {
        synchronized(graphicsBuffer)
        {
            return currentClip[1];
        }
    }

    /**
     * Returns width of clip rectangle.
     *
     * @return Width of clip rectangle.
     */
    public int getClipWidth()
    {
        synchronized(graphicsBuffer)
        {
            return currentClip[2];
        }
    }

    /**
     * Returns height of clip rectangle.
     *
     * @return Height of clip rectangle.
     */
    public int getClipHeight()
    {
        synchronized(graphicsBuffer)
        {
            return currentClip[3];
        }
    }

    /**
     * Intersects current clip rectangle with the specified one.
     *
     * @param x - left bound of the clip rectangle to intersect with the
     *            current one.
     * @param y - top bound of the clip rectangle to intersect with the
     *            current one.
     * @param w - width of the clip rectangle to intersect with the current one.
     * @param h - height of the clip rectangle to intersect with the current
     *            one.
     */
    public void clipRect(int x, int y, int w, int h)
    {
        synchronized(graphicsBuffer)
        {
            final int cx2 = Math.min(currentClip[0] + currentClip[2], x + w);
            final int cy2 = Math.min(currentClip[1] + currentClip[3], y + h);
            // setting of clip to Java Graphics
            currentClip[0] = Math.max(x, currentClip[0]);
            currentClip[1] = Math.max(y, currentClip[1]);
            currentClip[2] = cx2 - currentClip[0];
            currentClip[3] = cy2 - currentClip[1];
            graphicsBuffer.setClip(currentClip[0], currentClip[1], currentClip[2], currentClip[3], this);
        }
    }

    /**
     * Sets the clip rectangle.
     *
     * @param x - left bound of the new clip rectangle.
     * @param y - top bound of the new clip rectangle.
     * @param w - width of the new clip rectangle.
     * @param h - height of the new clip rectangle.
     */
    public void setClip(int x, int y, int w, int h)
    {
        synchronized(graphicsBuffer)
        {
            currentClip[0] = x;
            currentClip[1] = y;
            currentClip[2] = w;
            currentClip[3] = h;
            graphicsBuffer.setClip(currentClip[0], currentClip[1], currentClip[2], currentClip[3], this);
        }
    }

    /**
     * Draws a line with current color and current stroke style.
     *
     * @param xStart - X-coordinate of line starting point.
     * @param yStart - Y-coordinate of line starting point.
     * @param xEnd - X-coordinate of line end point.
     * @param yEnd - Y-coordinate of line end point.
     */
    public void drawLine(int xStart, int yStart, int xEnd, int yEnd)
    {
        synchronized(graphicsBuffer)
        {
            graphicsBuffer.drawLine(xStart, yStart, xEnd, yEnd, this);
        }
    }

    /**
     * Fills a rectangle with current color.
     *
     * @param x - x coordinate of top left corner
     * @param y - y coordinate of top left corner
     * @param w - width of the rectangle
     * @param h - height of the rectangle
     */
    public void fillRect(int x, int y, int w, int h)
    {
        if((w < 0) || (h < 0))
        {
            return;
        }
        synchronized(graphicsBuffer)
        {
            graphicsBuffer.fillRect(x, y, w, h, this);
        }
    }

    /**
     * Draws a rectangle with specified color and stroke style.
     *
     * @param x - x coordinate of top left corner
     * @param y - y coordinate of top left corner
     * @param w - width of the rectangle
     * @param h - height of the rectangle
     */
    public void drawRect(int x, int y, int w, int h)
    {
        if((w < 0) || (h < 0))
        {
            return;
        }
        synchronized(graphicsBuffer)
        {
            graphicsBuffer.drawRect(x, y, w, h, this);
        }
    }

    /**
     * Draws a rounded rectangle with specified color and stroke style.
     *
     * @param x - x coordinate of top left corner
     * @param y - y coordinate of top left corner
     * @param w - width of the rectangle
     * @param h - height of the rectangle
     * @param arcW - arc width for corner rounding.
     * @param arcH - arc height for corner rounding.
     */
    public void drawRoundRect(int x, int y, int w, int h, int arcW, int arcH)
    {
        if((w < 0) || (h < 0) || (arcW < 0) || (arcH < 0))
        {
            return;
        }
        synchronized(graphicsBuffer)
        {
            graphicsBuffer.drawRoundRect(x, y, w, h, arcW, arcH, this);
        }
    }

    /**
     * Fills a rounded rectangle with specified color.
     *
     * @param x - x coordinate of top left corner
     * @param y - y coordinate of top left corner
     * @param w - width of the rectangle
     * @param h - height of the rectangle
     * @param arcW - arc width for corner rounding.
     * @param arcH - arc height for corner rounding.
     */
    public void fillRoundRect(int x, int y, int w, int h, int arcW, int arcH)
    {
        if((w < 0) || (h < 0) || (arcW < 0) || (arcH < 0))
        {
            return;
        }
        synchronized(graphicsBuffer)
        {
            graphicsBuffer.fillRoundRect(x, y, w, h, arcW, arcH, this);
        }
    }

    /**
     * Fills an arc with specified color.
     *
     * @param x - X-coordinate of top left corner of the rectangle to contain
     *            the arc.
     * @param y - Y-coordinate of top left corner of the rectangle to contain
     *            the arc.
     * @param w - width of the rectangle to contain the arc.
     * @param h - height of the rectangle to contain the arc.
     * @param startAngle - starting angle of the arc in degrees.
     * @param arcAngle - angle to spread the arc in degrees.
     */
    public void fillArc(int x, int y, int w, int h, int startAngle, int arcAngle)
    {
        if((w < 0) || (h < 0))
        {
            return;
        }
        synchronized(graphicsBuffer)
        {
            graphicsBuffer.fillArc(x, y, w, h, startAngle, arcAngle, this);
        }
    }

    /**
     * Draws an arc with specified color and stroke style.
     *
     * @param x - X-coordinate of top left corner of the rectangle to contain
     *            the arc.
     * @param y - Y-coordinate of top left corner of the rectangle to contain
     *            the arc.
     * @param w - width of the rectangle to contain the arc.
     * @param h - height of the rectangle to contain the arc.
     * @param startAngle - starting angle of the arc in degrees.
     * @param arcAngle - angle to spread the arc in degrees.
     */
    public void drawArc(int x, int y, int w, int h, int startAngle, int arcAngle)
    {
        if((w < 0) || (h < 0))
        {
            return;
        }
        synchronized(graphicsBuffer)
        {
            graphicsBuffer.drawArc(x, y, w, h, startAngle, arcAngle, this);
        }
    }

    /**
     * Draws a string with specified color and font.
     *
     * @param string - the string to draw.
     * @param xPos - X-coordinate of the anchor point.
     * @param yPos - Y-coordinate of the anchor point
     * @param anch - anchor, specifies the way to locate the text around the anchor
     *            point. May be the combination of Graphics.TOP,
     *            Graphics.BOTTOM, Graphics.BASELINE, Graphics.LEFT,
     *            Graphics.RIGHT, Graphics.HCENTER.
     */
    public void drawString(String string, int xPos, int yPos, int anch)
    {
        if(string == null)
        {
            throw new NullPointerException(
                MsgRepository.GRAPHICS_EXCEPTION_STRING_IS_NULL);
        }

        if(!checkTextAnchors(anch))
        {
            throw new IllegalArgumentException(
                MsgRepository.GRAPHICS_EXCEPTION_INVALID_ANCHOR);
        }

        synchronized(graphicsBuffer)
        {
            final int alignments = GraphicsContext.ALIGNMENT_TOP | GraphicsContext.ALIGNMENT_LEFT;
            final int[] boundingBox = new int[4];
            final String localStr = string;

            ESWTUIThreadRunner.safeSyncExec(new Runnable()
            {
                public void run()
                {
                    FontUtils fu = new FontUtils(Font.getESWTFont(currentFont).handle);
                    fu.getBoundingRect(boundingBox, localStr);
                }
            });


            // Arrange vertical alignments
            int y = yPos;
            if(isFlag(anch, Graphics.BOTTOM))
            {
                y = yPos - boundingBox[GraphicsContext.RECT_HEIGHT];
            }
            if(isFlag(anch, Graphics.BASELINE))
            {
                y = yPos - currentFont.getBaselinePosition();
            }
            // Arrange horizontal alignments
            int x = xPos;
            if(isFlag(anch, Graphics.RIGHT))
            {
                x = xPos - boundingBox[GraphicsContext.RECT_WIDTH];
            }
            if(isFlag(anch, Graphics.HCENTER))
            {
                x = xPos - boundingBox[GraphicsContext.RECT_WIDTH] / 2;
            }

            graphicsBuffer.drawString(localStr, x, y, this);
        }
    }

    private static boolean isFlag(int anchor, int flag)
    {
        return (anchor & flag) != 0;
    }

    /**
     * Draws a string with specified color and font.
     *
     * @param string - the string to draw.
     * @param offset - offset of substring in the string.
     * @param length - length of substring.
     * @param xPos - X-coordinate of the anchor point.
     * @param yPos - Y-coordinate of the anchor point
     * @param anch - anchor, specifies the way to locate the text around the
     *            anchor point. May be the combination of Graphics.TOP,
     *            Graphics.BOTTOM, Graphics.BASELINE, Graphics.LEFT,
     *            Graphics.RIGHT, Graphics.HCENTER.
     */
    public void drawSubstring(java.lang.String string, int offset, int length,
                              int xPos, int yPos, int anch)
    {
        drawString(string.substring(offset, offset+length), xPos, yPos, anch);
    }

    /**
     * Draws a character with specified color and font.
     *
     * @param c - character to draw.
     * @param xPos - X-coordinate of the anchor point.
     * @param yPos - Y-coordinate of the anchor point
     * @param anch - anchor, specifies the way to locate the text around the
     *            anchor point. May be the combination of Graphics.TOP,
     *            Graphics.BOTTOM, Graphics.BASELINE, Graphics.LEFT,
     *            Graphics.RIGHT, Graphics.HCENTER.
     */
    public void drawChar(char c, int xPos, int yPos, int anch)
    {
        drawString(String.valueOf(c), xPos, yPos, anch);
    }

    /**
     * Draws characters from array with specified color and font.
     *
     * @param ch -character array to draw.
     * @param offset - offset of the starting character in the array.
     * @param length - length of character sequence from the array to draw.
     * @param xPos - X-coordinate of the anchor point.
     * @param yPos - Y-coordinate of the anchor point
     * @param anch - anchor, specifies the way to locate the text around the
     *            anchor point. May be the combination of Graphics.TOP,
     *            Graphics.BOTTOM, Graphics.BASELINE, Graphics.LEFT,
     *            Graphics.RIGHT, Graphics.HCENTER.
     */
    public void drawChars(char[] ch, int offset, int length,
                          int xPos, int yPos, int anch)
    {
        if(ch == null)
        {
            throw new NullPointerException(
                MsgRepository.GRAPHICS_EXCEPTION_ARRAY_IS_NULL);
        }
        String str = null;
        try
        {
            str = String.valueOf(ch, offset, length);
        }
        catch(Exception e)
        {
            throw new ArrayIndexOutOfBoundsException(
                MsgRepository.GRAPHICS_EXCEPTION_ARRAY_OUT_OF_BOUNDS);
        }

        drawString(str, xPos, yPos, anch);
    }

    /**
     * Checks if anchors combination is valid.
     */
    private boolean checkTextAnchors(int anch)
    {
        boolean retVal = false;

        int vertMask = Graphics.TOP | Graphics.BASELINE | Graphics.BOTTOM;
        int horMask = Graphics.LEFT | Graphics.RIGHT | Graphics.HCENTER;

        if(anch == 0)
        {
            return true;
        }

        if((anch & ~(vertMask | horMask)) != 0)
        {
            return false;
        }

        int vertAchor = anch & vertMask;
        int horAchor = anch & horMask;

        if((vertAchor == Graphics.TOP)
                || (vertAchor == Graphics.BASELINE)
                || (vertAchor == Graphics.BOTTOM))
        {
            retVal = true;
        }

        if((horAchor == Graphics.LEFT)
                || (vertAchor == Graphics.RIGHT)
                || (vertAchor == Graphics.HCENTER))
        {
            retVal = true;
        }

        return retVal;
    }

    /**
     * Draw an Image to the graphical context.
     *
     * @param image - Image to be drawn.
     * @param xPos - X-coordinate of the anchor point.
     * @param yPos - Y-coordinate of the anchor point.
     * @param anch - anchor value.
     */
    public void drawImage(javax.microedition.lcdui.Image image, int xPos,
                          int yPos, int anch)
    {

        if(image == null)
        {
            throw new NullPointerException(
                MsgRepository.IMAGE_EXCEPTION_IS_NULL);
        }
        if(!checkImageAnchors(anch))
        {
            throw new IllegalArgumentException(
                MsgRepository.GRAPHICS_EXCEPTION_INVALID_ANCHOR);
        }
        synchronized(graphicsBuffer)
        {
            int y = yPos;
            if(isFlag(anch, Graphics.VCENTER))
            {
                y = yPos - image.getHeight() / 2;
            }
            if(isFlag(anch, Graphics.BOTTOM))
            {
                y = yPos - image.getHeight();
            }

            int x = xPos;
            if(isFlag(anch, Graphics.HCENTER))
            {
                x = xPos - image.getWidth() / 2;
            }
            if(isFlag(anch, Graphics.RIGHT))
            {
                x = xPos - image.getWidth();
            }

            synchronized(image.graphicsBuffer)
            {
                final Image localLcduiImage = image;
                final org.eclipse.swt.internal.qt.graphics.Image localCgfxImage = 
                    Internal_GfxPackageSupport.getImage(Image.getESWTImage(image));
                final int localX = x;
                final int localY = y;
                final Graphics self = this;
                
                if(image.graphicsBuffer.containsDrawnPrimitives()) {
                    localLcduiImage.sync(true);
                }
                graphicsBuffer.drawImage(localCgfxImage, localX, localY, self);
            }
        }
    }

    /**
     * Checks if anchors combination is valid.
     */
    private boolean checkImageAnchors(int anch)
    {
        boolean retVal = false;

        int vertMask = Graphics.TOP | Graphics.VCENTER | Graphics.BOTTOM;
        int horMask = Graphics.LEFT | Graphics.RIGHT | Graphics.HCENTER;

        if(anch == 0)
        {
            return true;
        }

        if((anch & ~(vertMask | horMask)) != 0)
        {
            return false;
        }

        int vertAchor = anch & vertMask;
        int horAchor = anch & horMask;

        if((vertAchor == Graphics.TOP)
                || (vertAchor == Graphics.VCENTER)
                || (vertAchor == Graphics.BOTTOM))
        {
            retVal = true;
        }

        if((horAchor == Graphics.LEFT)
                || (vertAchor == Graphics.RIGHT)
                || (vertAchor == Graphics.HCENTER))
        {
            retVal = true;
        }

        return retVal;
    }

    /**
     * Sets the stroke style for drawing graphical primitives.
     *
     * @param newStyle - new style, valid values are Graphics.SOLID and
     *            Graphics.DOTTED.
     * @throws IllegalArgumentException if the new style value is invalid.
     */
    public void setStrokeStyle(int newStyle)
    {
        if(newStyle == currentStrokeStyle)
        {
            return;
        }
        synchronized(graphicsBuffer)
        {
            int styleToApply = mapStrokeStyle(newStyle);
            if(styleToApply == INVALID_STROKE_STYLE)
            {
                throw new IllegalArgumentException(
                    MsgRepository.GRAPHICS_EXCEPTION_ILLEGAL_STROKE_STYLE);
            }
            graphicsBuffer.setStrokeStyle(styleToApply, newStyle, this);
            currentStrokeStyle = newStyle;
        }
    }

    /**
     * Returns current stroke style.
     *
     * @return Current stroke style.
     */
    public int getStrokeStyle()
    {
        synchronized(graphicsBuffer)
        {
            return currentStrokeStyle;
        }
    }

    /**
     * Returns the color that will be used if the specified color is requested.
     *
     * @param color - color to use in 0x00RRGGBB form.
     * @return Color that will be actually used in 0x00RRGGBB form.
     */
    public int getDisplayColor(int color)
    {
        return color & RGB_MASK;
    }

    /**
     * Renders 0xAARRGGBB pixels.
     *
     * @param rgb - array of ARGB values
     * @param offset - index of the first value in the array.
     * @param scanlength - relative distance in the array between. corresponding
     *            pixels of consecutive rows.
     * @param x - X-coordinate of the top-left corner of the rectangle to be
     *            rendered.
     * @param y - Y-coordinate of the top-left corner of the rectangle to be
     *            rendered.
     * @param w - width of the rectangle to be rendered.
     * @param h - height of the rectangle to be rendered.
     * @param alpha - true if alpha values should be rendered, false otherwise.
     */
    public void drawRGB(int[] rgb,
                        int offset,
                        int scanlength,
                        int x,
                        int y,
                        int w,
                        int h,
                        boolean alpha)
    {

        if(rgb == null)
        {
            throw new NullPointerException(
                MsgRepository.IMAGE_EXCEPTION_DATA_IS_NULL);
        }
        synchronized(graphicsBuffer)
        {
            graphicsBuffer.drawRGB(rgb, offset, scanlength, x, y, w, h, alpha, this);
        }
    }

    /**
     * Fills the specified triangle with current color.
     *
     * @param xPos1 - X-coordinate of first vertex.
     * @param yPos1 - Y-coordinate of first vertex.
     * @param xPos2 - X-coordinate of second vertex.
     * @param yPos2 - Y-coordinate of second vertex.
     * @param xPos3 - X-coordinate of third vertex.
     * @param yPos3 - Y-coordinate of third vertex.
     */
    public void fillTriangle(int xPos1,
                             int yPos1,
                             int xPos2,
                             int yPos2,
                             int xPos3,
                             int yPos3)
    {
        synchronized(graphicsBuffer)
        {
            final int[] points = {xPos1, yPos1, xPos2, yPos2, xPos3, yPos3};
            graphicsBuffer.fillTriangle(points, this);
        }
    }

    /**
     * Copies the specified area.
     *
     * @param xFrom - X-coordinate of the top-left corner of the region to copy.
     * @param yFrom - Y-coordinate of the top-left corner of the region to copy.
     * @param w - width of the region to copy.
     * @param h - height of the region to copy.
     * @param xTo - X-coordinate of the anchor point to copy region to.
     * @param yTo - Y-coordinate of the anchor point to copy region to.
     * @param anch - anchor point specification.
     */
    public void copyArea(int xFrom,
                         int yFrom,
                         int w,
                         int h,
                         int xTo,
                         int yTo,
                         int anch)
    {

        if(graphicsBuffer.getHostType() != Buffer.HOST_TYPE_IMAGE)
        {
            // this Graphics belongs to a screen device.
            throw new IllegalStateException(
                MsgRepository.GRAPHICS_EXCEPTION_DESTINATION_IS_SCREEN);
        }
        synchronized(graphicsBuffer)
        {
            Image image = (Image)graphicsBuffer.getHost();
            if(!javax.microedition.lcdui.Image.validateRegion(image
                    .getWidth(), image.getHeight(), xFrom, yFrom, w, h))
            {
                throw new IllegalArgumentException(
                    MsgRepository.IMAGE_EXCEPTION_INVALID_REGION);
            }

            // Arrange vertical alignments
            int destY = yTo;
            if(isFlag(anch, Graphics.BOTTOM))
            {
                destY = yTo - h;
            }
            if(isFlag(anch, Graphics.VCENTER))
            {
                destY = yTo - h / 2;
            }

            // Arrange horizontal alignments
            int destX = xTo;
            if(isFlag(anch, Graphics.RIGHT))
            {
                destX = xTo - w;
            }
            if(isFlag(anch, Graphics.HCENTER))
            {
                destX = xTo - w / 2;
            }
            graphicsBuffer.copyArea(xFrom, yFrom, w, h, destX, destY, this);
        }
    }

    /**
     * Renders a portion of source image with possible transforms.
     *
     * @param srcImage - source Image
     * @param xSrc - X-coordinate of the top-left corner of the rectangle in
     *            source image.
     * @param ySrc - Y-coordinate of the top-left corner of the rectangle in
     *            source image.
     * @param width - width of the rectangle in source image.
     * @param height - height of the rectangle in source image.
     * @param transform - transform to apply to the image before rendering.
     * @param xDst - X-coordinate of the anchor point in destination.
     * @param yDst - Y-coordinate of the anchor point in destination.
     * @param anch - anchor point definition.
     */
    public void drawRegion(javax.microedition.lcdui.Image srcImage,
                           int xSrc,
                           int ySrc,
                           int width,
                           int height,
                           int transform,
                           int xDst,
                           int yDst,
                           int anch)
    {

        if(srcImage == null)
        {
            throw new NullPointerException(
                MsgRepository.IMAGE_EXCEPTION_IS_NULL);
        }
        if(srcImage == graphicsBuffer.getHost())
        {
            throw new IllegalArgumentException(
                MsgRepository.GRAPHICS_EXCEPTION_SAME_SOURCE_AND_DESTINATION);
        }
        if(!javax.microedition.lcdui.Image.validateTransform(transform))
        {
            throw new IllegalArgumentException(
                MsgRepository.IMAGE_EXCEPTION_INVALID_TRANSFORM);
        }
        if(!checkImageAnchors(anch))
        {
            throw new IllegalArgumentException(
                MsgRepository.GRAPHICS_EXCEPTION_INVALID_ANCHOR);
        }
        if(!javax.microedition.lcdui.Image.validateRegion(srcImage.getWidth(),
                srcImage.getHeight(), xSrc, ySrc, width, height))
        {
            throw new IllegalArgumentException(
                MsgRepository.IMAGE_EXCEPTION_INVALID_REGION);
        }
        synchronized(graphicsBuffer)
        {
            // Arrange vertical alignments
            int y = yDst;
            if(isFlag(anch, Graphics.VCENTER))
            {
                y = yDst - srcImage.getHeight() / 2;
            }
            if(isFlag(anch, Graphics.BOTTOM))
            {
                y = yDst - srcImage.getHeight();
            }

            // Arrange horizontal alignments
            int x = xDst;
            if(isFlag(anch, Graphics.HCENTER))
            {
                x = xDst - srcImage.getWidth() / 2;
            }
            if(isFlag(anch, Graphics.RIGHT))
            {
                x = xDst - srcImage.getWidth();
            }
            final int gcTransform = Image.getCgTransformValue(transform);
            synchronized(srcImage.graphicsBuffer)
            {
                final Image localLcduiSrcImage = srcImage;
                final org.eclipse.swt.internal.qt.graphics.Image localCgfxImage = 
                    Internal_GfxPackageSupport.getImage(Image.getESWTImage(srcImage));
                final int localX = x;
                final int localY = y;
                final int localW = width;
                final int localH = height;
                final int localXSrc = xSrc;
                final int localYSrc = ySrc;
                final int localGcTransform = gcTransform;
                final Graphics self = this;
                if(srcImage.graphicsBuffer.containsDrawnPrimitives()) {
                    localLcduiSrcImage.sync(true);
                }
                graphicsBuffer.drawImage(localCgfxImage,
                    localX, localY, localW, localH, localXSrc, localYSrc, localW, localH, localGcTransform, self);
            }
        }
    }

    /**
     * Performs synchronization on the graphics buffer, i.e.
     * the buffered draw commands are rasterized to the surface.
     */
    void sync()
    {
        synchronized(graphicsBuffer) 
        {
            if(syncStrategy == SYNC_LEAVE_SURFACE_SESSION_OPEN)
            {
                // This instance is used only with paint callbacks, thus  
                // sync is called with the indication that surface paint  
                // session can be left open as it will be closed when the 
                // callback returns.
                graphicsBuffer.sync(false);
            }
            else 
            {
                graphicsBuffer.sync(true);
            } 
        }
    }
    
    
    /**
     * Return DirectGraphics associated with this instance.
     */
    DirectGraphics getDirectGraphics()
    {
        if(directGraphics == null)
        {
            directGraphics = new DirectGraphicsImpl(this);
        }
        return directGraphics;
    }

    /**
     * Getter for graphics buffer.
     * @return The Buffer.
     */
    Buffer getGraphicsBuffer()
    {
        return graphicsBuffer;
    }
    
    /**
     * Maps stroke style constant from values used by
     * Graphics to values defined in GraphicsContext
     */
    static int mapStrokeStyle(int strokeStyle)
    {
        if(strokeStyle == SOLID)
        {
            return GraphicsContext.STROKE_SOLID;
        }
        else if(strokeStyle == DOTTED)
        {
            return GraphicsContext.STROKE_DOT;
        }
        return INVALID_STROKE_STYLE;
    }

    //
    // Nokia UI API support
    //
    void drawRGB(int[] rgb, int offset, int scanlength, int x, int y, int w,
                 int h, boolean processAlpha, int manipulation)
    {
        synchronized(graphicsBuffer)
        {
            graphicsBuffer.drawRGB(rgb, offset, scanlength, x, y, w, h, processAlpha, manipulation, this);
        }
    }

    void drawRGB(byte[] rgb, byte[] transparencyMask, int offset, int scanlength, int x, int y, int w,
                 int h, int manipulation, int format)
    {
        synchronized(graphicsBuffer)
        {
            graphicsBuffer.drawRGB(rgb, transparencyMask, offset, scanlength, x, y, w, h, manipulation, format, this);
        }
    }

    void drawRGB(short[] rgb, int offset, int scanlength, int x, int y, int w,
                 int h, boolean processAlpha, int manipulation, int format)
    {
        synchronized(graphicsBuffer)
        {
            graphicsBuffer.drawRGB(rgb, offset, scanlength, x, y, w, h, processAlpha, manipulation, format, this);
        }
    }

    void drawPolygon(int[] points)
    {
        synchronized(graphicsBuffer)
        {
            graphicsBuffer.drawPolygon(points, this);
        }
    }

    void fillPolygon(int[] points)
    {
        synchronized(graphicsBuffer)
        {
            graphicsBuffer.fillPolygon(points, this);
        }
    }

    void setARGBColor(int argb)
    {
        synchronized(graphicsBuffer)
        {
            graphicsBuffer.setARGBColor(argb, this);
            currentColor = argb;
        }

    }

}
