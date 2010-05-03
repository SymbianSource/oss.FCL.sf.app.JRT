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
* Description: 
*
*/
package javax.microedition.lcdui;

import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Internal_GfxPackageSupport;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.internal.qt.graphics.GraphicsContext;
import org.eclipse.swt.internal.qt.graphics.JavaCommandBuffer;
import org.eclipse.swt.internal.qt.graphics.FontUtils;

import com.nokia.mid.ui.DirectGraphics;

/**
 * Implementation of LCDUI <code>Graphics</code> class.
 */
public class Graphics {

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

    static final int RGB_MASK = 0x00FFFFFF;

    static final int COMPONENT_MASK = 0xFF;

    private DirectGraphics directGraphics;

    // Graphics rectangle (off-screen buffer rectangle).
    // private Rectangle graphicsClientArea;

    // Graphics context for drawing to off-screen buffer.
    private GraphicsContext gc;

    // Off-screen buffer.
    //private org.eclipse.swt.internal.qt.graphics.Image frameBuffer;

    // Current font for rendering texts.
    private Font currentFont;
    private int currentColor;
    private int translateX;
    private int translateY;
    private int[] currentClip = new int[4];
    private int currentStrokeSyle;

    // Image owning the Graphics if the Graphics object belongs to an Image.
    private Image parentImage;

    // Flag indicating that we are in Canvas.paint() callback,
    // thus inside UI thread
    private boolean paintCallBack;
    private boolean buffered;
    private JavaCommandBuffer cmdBuffer;
    private com.nokia.mj.impl.rt.support.Finalizer finalizer;
    // serialization lock for command buffering and flush
    private final Object flushLock = new Object();
    private Canvas canvasParent;
    private CustomItem customItemParent;
    
    //Constructor
    Graphics() {
       finalizer = ((finalizer != null) ? finalizer
                : new com.nokia.mj.impl.rt.support.Finalizer() {
                    public void finalizeImpl() {
                        if (finalizer != null) {
                            finalizer = null;
                            if (!ESWTUIThreadRunner.isDisposed()) {
                                ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                                   public void run() {
                                      dispose();
                                   }
                                });
                               
                            }
                        }
                    }
                });
    }
    /**
     * Set the parent image of this Graphics.
     *
     * @param image an image
     */
    void eswtSetParentImage(final Image image) {
        if(buffered) {
            return;
        }
        if (parentImage != image) {
            parentImage = image;

            if (gc == null) {
                gc = new GraphicsContext();
            }
            else {
                gc.releaseTarget();
            }
            ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                public void run() {
                    org.eclipse.swt.graphics.Image eswtImage = Image.getESWTImage(image);
                        
                    if (eswtImage != null) {
                        gc.bindTarget(Internal_GfxPackageSupport.getImage(eswtImage));
                        Rectangle clipRect = eswtImage.getBounds();
                        setClip(0, 0, clipRect.width, clipRect.height);
                    }
                    reset();
                }});
        }
    }

    /**
     * Initializes this instance of Graphics to use command buffer.
     * 
     * Can be called in a non-UI thread. Not thread-safe. 
     * 
     * @param x The x-coordinate of clip
     * @param y The y-coordinate of clip
     * @param width The width of clip
     * @param height The height of clip
     */
    void initBuffered(final Canvas parent, final int x, final int y, final int width, final int height) {
      ESWTUIThreadRunner.safeSyncExec(new Runnable() {
          public void run() {
              cmdBuffer = new JavaCommandBuffer();
              if (gc == null) {
                  gc = new GraphicsContext();
              }
              gc.bindTarget(cmdBuffer);
              setClip(x, y, width, height);
              reset();
              buffered = true;
              javax.microedition.lcdui.Graphics.this.canvasParent = parent;
          }});
    }
    
    /**
     * Initializes this instance of Graphics to use command buffer.
     * 
     * Can be called in a non-UI thread. Not thread-safe. 
     * 
     * @param x The x-coordinate of clip
     * @param y The y-coordinate of clip
     * @param width The width of clip
     * @param height The height of clip
     */
    void initBuffered(CustomItem parent, int x, int y, int width, int height) {
        cmdBuffer = new JavaCommandBuffer();
        if (gc == null) {
            gc = new GraphicsContext();
        }
        gc.bindTarget(cmdBuffer);
        setClip(x, y, width, height);
        reset();
        buffered = true;
        this.customItemParent = parent;
    }

	/**
	 * Resets the command buffer contents.
	 * 
	 * This is safe to call only in the UI thread. 
	 */
    void resetCommandBuffer() {
        gc.releaseTarget();
        cmdBuffer.reset();
        gc.bindTarget(cmdBuffer);
        // write settings which were active before flush 
        // if they are not the same as defaults
        gc.setFont(Font.getESWTFont(currentFont).handle);
        gc.setBackgroundColor(currentColor, false);
        gc.setForegroundColor(currentColor, false);
        if ((translateX != 0) || (translateY != 0)) { 
            gc.translate(translateX, translateY);
        }
        
        // Note that if called in a non-UI thread then the size of the
        // Canvas or CustomItem can change between getting the height and
        // getting the width. Those are modified by the UI thread and there
        // is no synchronization. 
        int w = 0;
        int h = 0;
        if(canvasParent != null) {
            w = canvasParent.getWidth();
            h = canvasParent.getHeight();
        } else {
            w = customItemParent.getContentWidth();
            h = customItemParent.getContentHeight();
        }
        
        if((currentClip[0] != 0) && (currentClip[1] != 0) && 
           (currentClip[2] != w) && (currentClip[2] != h)) {
            gc.setClip(currentClip[0], currentClip[1], currentClip[2], currentClip[3], false);
        }
        if (currentStrokeSyle != SOLID) {
            gc.setStrokeStyle(GraphicsContext.STROKE_DOT);
        }
    }
    
    /**
     * Returns the current command buffer or null. 
     * 
     * This method is thread-safe. 
     */
    JavaCommandBuffer getCommandBuffer() {
        return cmdBuffer;
    }
    
    /**
     * Get the parent image of this Graphics.
     */
    Image getParentImage() {
        return parentImage;
    }

    /**
     * Set Graphics context to a target - the GC is coming from paint event.
     *
     * @param eswtGC
     */
    void eswtSetGC(final GC eswtGC) {
       if(buffered) {
            return;
        }
        gc = eswtGC.getGCData().internalGc;
        ESWTUIThreadRunner.safeSyncExec(new Runnable() {
            public void run() {
                Rectangle clipRect = eswtGC.getClipping();
                setClip(clipRect.x, clipRect.y, clipRect.width, clipRect.height);
                reset();
            }});
    }

    /**
     * Disposes Graphics context resources.
     */
    void dispose() {
        if (parentImage != null) {
            parentImage = null;
        }
        if(gc != null) {
            ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                public void run() {
                    gc.dispose();
                }});
          gc = null;
        }
        if (cmdBuffer != null) {
          cmdBuffer = null;
        }
    }

    /**
     * Resets Graphics state to initial.
     */
    void reset() {
        setColor(0, 0, 0);
        setFont(Font.getDefaultFont());
        setStrokeStyle(Graphics.SOLID);
        this.translateX = 0;
        this.translateY = 0;
    }

    /**
     * Cleans the Canvas background.
     */
    void cleanBackground(Rectangle area) {
        int savedColor = currentColor;
        setColor(255, 255, 255);
        fillRect(area.x, area.y, area.width, area.height);
        setColor(savedColor);
    }

    /**
     * Cleans the Canvas background.
     */
    void cleanBackground(int x, int y, int w, int h) {
        int savedColor = currentColor;
        setColor(255, 255, 255);
        fillRect(x, y, w, h);
        setColor(savedColor);
    }

    /**
     * Sets flag indicating that we are in Canvas.paint() callback
     */
    void beginPaint() {
        paintCallBack = true;
    }

    /**
     * Sets flag indicating that we are exiting Canvas.paint() callback
     */
    void endPaint() {
        paintCallBack = false;
    }

    /**
     * Provides the serialization lock for buffer writing and flushing
     * @return lock used for synchronizing command buffer access
     */
    Object getLock() {
        return flushLock;
    }
    
    /**
     * Sets coordinate translation. Translations are cumulative.
     *
     * @param xDelta x-shift for coordinates.
     * @param yDelta y-shift for coordinates.
     */
    public void translate(int xDelta, int yDelta) {
        synchronized(flushLock) {
            if(!buffered) { 
                final int xDelta_ = xDelta;
                final int yDelta_ = yDelta;
                ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                    public void run() {
                        gc.translate(xDelta_, yDelta_);
                    }});
            } else {
                gc.translate(xDelta, yDelta);
            }
        }
        translateX += xDelta;
        translateY += yDelta;
    }

    /**
     * Returns current X-shift of coordinate translation.
     *
     * @return Current X-shift of coordinate translation.
     */
    public int getTranslateX() {
        return translateX;
    }

    /**
     * Returns current Y-shift of coordinate translation.
     *
     * @return current Y-shift of coordinate translation.
     */
    public int getTranslateY() {
        return translateY;
    }

    /**
     * Returns current color in 0x00RRGGBB format.
     *
     * @return Current color in 0x00RRGGBB format.
     */
    public int getColor() {
        return currentColor;
    }

    /**
     * Returns red component of current color.
     *
     * @return Red component of current color in the range of 0-255.
     */
    public int getRedComponent() {
        return currentColor >> 16;
    }

    /**
     * Returns green component of current color.
     *
     * @return Green component of current color in the range of 0-255.
     */
    public int getGreenComponent() {
        return (currentColor >> 8) & COMPONENT_MASK;
    }

    /**
     * Returns blue component of current color.
     *
     * @return Blue component of current color in the range of 0-255.
     */
    public int getBlueComponent() {
        return currentColor & COMPONENT_MASK;
    }

    /**
     * Returns current grayscale color.
     *
     * @return Returns current grayscale color in the range 0-255.
     */
    public int getGrayScale() {
        return (getRedComponent() + getGreenComponent() + getBlueComponent()) / 3;
    }

    /**
     * Sets the current color by color components for red, green and blue.
     *
     * @param r - red component of the color to be set.
     * @param g - green component of the color to be set.
     * @param b - blue component of the color to be set.
     */
    public void setColor(int r, int g, int b) {
        if (r < 0 || r > 255 ||
            g < 0 || g > 255 ||
            b < 0 || b > 255 ) {
                throw new IllegalArgumentException();
        }
        synchronized(flushLock) {
            if(!buffered) { 
                final int r_ = r;
                final int g_ = g;
                final int b_ = b;
                ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                    public void run() {
                        gc.setForegroundColor(r_, g_, b_);
                        gc.setBackgroundColor(r_, g_, b_);
                    }});
            } else {
                gc.setForegroundColor(r, g, b);
                gc.setBackgroundColor(r, g, b);
            }
        }
        currentColor = ((r << 16) | (g << 8) | b) & RGB_MASK;
    }

    /**
     * Sets the current color.
     *
     * @param RGB - color to be set in the form of 0x00RRGGBB.
     */
    public void setColor(int RGB) {
        int maskedRGB = RGB & RGB_MASK;
        final int r = maskedRGB >> 16;
        final int g = (maskedRGB >> 8) & COMPONENT_MASK;
        final int b = maskedRGB & COMPONENT_MASK;
        synchronized(flushLock) {
            if(!buffered) { 
                ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                    public void run() {
                        gc.setForegroundColor(r, g, b);
                        gc.setBackgroundColor(r, g, b);
                    }});
            } else {
                gc.setForegroundColor(r, g, b);
                gc.setBackgroundColor(r, g, b);
            }
        }
        currentColor = maskedRGB;
    }

    /**
     * Sets gray-scale color.
     *
     * @param val - gray-scale value to be set in the range of 0-255.
     */
    public void setGrayScale(int val) {
        if ( (val < 0) || (val > 255)) {
        throw new IllegalArgumentException();
        }
        final int col = val & COMPONENT_MASK;
        synchronized(flushLock) {
            if(!buffered) { 
                ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                    public void run() {
                        gc.setForegroundColor(col, col, col);
                        gc.setBackgroundColor(col, col, col);
                    }});
            } else {
                gc.setForegroundColor(col, col, col);
                gc.setBackgroundColor(col, col, col);
            }
        }
        currentColor = ((col << 16) | (col << 8) | col) & RGB_MASK;
    }

    /**
     * Returns the current font.
     *
     * @return Current font.
     */
    public Font getFont() {
        return currentFont;
    }

    /**
     * Sets the font to be used for string rendering.
     *
     * @param newFont - the font to be used for string rendering.
     */
    public void setFont(Font newFont) {
        synchronized(flushLock) {
            if(!buffered) { 
                final Font newFont_ = newFont;
                ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                    public void run() {
                        gc.setFont(Font.getESWTFont(newFont_).handle);
                    }});
            } else {
                gc.setFont(Font.getESWTFont(newFont).handle);
            }
        }
        currentFont = newFont;
    }

    /**
     * Returns left bound of clip rectangle.
     *
     * @return Left bound of clip rectangle.
     */
    public int getClipX() {
        return currentClip[0];
    }

    /**
     * Returns top bound of clip rectangle.
     *
     * @return Top bound of clip rectangle.
     */
    public int getClipY() {
        return currentClip[1];
    }

    /**
     * Returns width of clip rectangle.
     *
     * @return Width of clip rectangle.
     */
    public int getClipWidth() {
        return currentClip[2];
    }

    /**
     * Returns height of clip rectangle.
     *
     * @return Height of clip rectangle.
     */
    public int getClipHeight() {
        return currentClip[3];
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
    public void clipRect(int x, int y, int w, int h) {
        final int cx2 = Math.min( currentClip[0] + currentClip[2], x + w );
        final int cy2 = Math.min( currentClip[1] + currentClip[3], y + h );
        // setting of clip to Java Graphics
        currentClip[0] = Math.max( x, currentClip[0]);
        currentClip[1] = Math.max( y, currentClip[1]);
        currentClip[2] = cx2 - currentClip[0];
        currentClip[3] = cy2 - currentClip[1];
        synchronized(flushLock) {
            if(!buffered) { 
                ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                    public void run() {
                        gc.setClip(currentClip[0], currentClip[1], currentClip[2], currentClip[3], false);
                    }});
            } else {
                gc.setClip(currentClip[0], currentClip[1], currentClip[2], currentClip[3], false);
            }
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
    public void setClip(int x, int y, int w, int h) {
        synchronized(flushLock) {
            if(!buffered) { 
                final int x_ = x;
                final int y_ = y;
                final int w_ = w;
                final int h_ = h;
        
                ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                    public void run() {
                        gc.setClip(x_, y_, w_, h_, false);
                    }});
            } else {
                gc.setClip(x, y, w, h, false);
            }
        }
        currentClip[0] = x;
        currentClip[1] = y;
        currentClip[2] = w;
        currentClip[3] = h;
    }

    /**
     * Draws a line with current color and current stroke style.
     *
     * @param xStart - X-coordinate of line starting point.
     * @param yStart - Y-coordinate of line starting point.
     * @param xEnd - X-coordinate of line end point.
     * @param yEnd - Y-coordinate of line end point.
     */
    public void drawLine(int xStart, int yStart, int xEnd, int yEnd) {
        synchronized(flushLock) {
            if(!buffered) { 
                final int xs_ = xStart;
                final int ys_ = yStart;
                final int xe_ = xEnd;
                final int ye_ = yEnd;
        
                ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                    public void run() {
                        gc.drawLine(xs_, ys_, xe_, ye_);
                    }});
            } else {
                gc.drawLine(xStart, yStart, xEnd, yEnd);
            }
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
    public void fillRect(int x, int y, int w, int h) {
        if ((w < 0) || (h < 0)) {
            return;
        }
        synchronized(flushLock) {
            if(!buffered) { 
                final int x_ = x;
                final int y_ = y;
                final int w_ = w;
                final int h_ = h;
        
                ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                    public void run() {
                        gc.fillRect(x_, y_, w_, h_);
                    }});
            } else {
                gc.fillRect(x, y, w, h);
            }
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
    public void drawRect(int x, int y, int w, int h) {
        if ((w < 0) || (h < 0)) {
            return;
        }
        synchronized(flushLock) {
            if(!buffered) {
                final int x_ = x;
                final int y_ = y;
                final int w_ = w;
                final int h_ = h;
                ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                    public void run() {
                        gc.drawRect(x_, y_, w_, h_);
                    }});
            } else {
                gc.drawRect(x, y, w, h);
            }
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
    public void drawRoundRect(int x, int y, int w, int h, int arcW, int arcH) {
        if ((w < 0) || (h < 0) || (arcW < 0) || (arcH < 0)) {
            return;
        }
        synchronized(flushLock) {
            if(!buffered) {
                final int x_ = x;
                final int y_ = y;
                final int w_ = w;
                final int h_ = h;
                final int arcW_ = arcW;
                final int arcH_ = arcH;
                
                ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                    public void run() {
                        gc.drawRoundRect(x_, y_, w_, h_, arcW_, arcH_);
                    }});
            } else {
                gc.drawRoundRect(x, y, w, h, arcW, arcH);
            }
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
    public void fillRoundRect(int x, int y, int w, int h, int arcW, int arcH) {
        if ((w < 0) || (h < 0) || (arcW < 0) || (arcH < 0)) {
            return;
        }
        synchronized(flushLock) {
            if(!buffered) {
                final int x_ = x;
                final int y_ = y;
                final int w_ = w;
                final int h_ = h;
                final int arcW_ = arcW;
                final int arcH_ = arcH;
                
                ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                    public void run() {
                        gc.fillRoundRect(x_, y_, w_, h_, arcW_, arcH_);
                    }});
            } else {
                gc.fillRoundRect(x, y, w, h, arcW, arcH);
            }
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
    public void fillArc(int x, int y, int w, int h, int startAngle, int arcAngle) {
        if ((w < 0) || (h < 0)) {
            return;
        }
        synchronized(flushLock) {
            if(!buffered) {
                final int x_ = x;
                final int y_ = y;
                final int w_ = w;
                final int h_ = h;
                final int startAngle_ = startAngle;
                final int arcAngle_ = arcAngle;
                
                ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                    public void run() {
                        gc.fillArc(x_, y_, w_, h_, startAngle_, arcAngle_);
                    }});
            } else {
                gc.fillArc(x, y, w, h, startAngle, arcAngle);
            }
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
    public void drawArc(int x, int y, int w, int h, int startAngle, int arcAngle) {
        if ((w < 0) || (h < 0)) {
            return;
        }
        synchronized(flushLock) {
            if(!buffered) {
                final int x_ = x;
                final int y_ = y;
                final int w_ = w;
                final int h_ = h;
                final int startAngle_ = startAngle;
                final int arcAngle_ = arcAngle;
                
                ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                    public void run() {
                        gc.drawArc(x_, y_, w_, h_, startAngle_, arcAngle_);
                    }});
            } else {
                gc.drawArc(x, y, w, h, startAngle, arcAngle);
            }
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
    public void drawString(String string, int xPos, int yPos, int anch) {
        if (string == null) {
            throw new NullPointerException(
                    MsgRepository.GRAPHICS_EXCEPTION_STRING_IS_NULL);
        }

        if (!checkTextAnchors(anch)) {
            throw new IllegalArgumentException(
                    MsgRepository.GRAPHICS_EXCEPTION_INVALID_ANCHOR);
        }

        final int alignments = GraphicsContext.ALIGNMENT_TOP | GraphicsContext.ALIGNMENT_LEFT;
        final int[] boundingBox = new int[4];
        final String localStr = string;
        
        if (paintCallBack) {
            gc.getTextBoundingBox(boundingBox, string, alignments, 0);
        }
        else {
            ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                public void run() {
                    FontUtils fu = new FontUtils(Font.getESWTFont(currentFont).handle);
                    fu.getBoundingRect(boundingBox, localStr);
                }
            });
        }

        // boundingBox[RECT_X] - top-left x of the text bounding box
        // boundingBox[RECT_Y] - top-left y of the text bounding box
        // boundingBox[RECT_WIDTH] - the width of the text bounding box
        // boundingBox[RECT_HEIGHT] - the height of the text bounding box

        // Arrange vertical alignments
        int y = yPos;
        if (isFlag(anch, Graphics.BOTTOM)) {
            y = yPos - boundingBox[GraphicsContext.RECT_HEIGHT];
        }
        if (isFlag(anch, Graphics.BASELINE)) {
            y = yPos - currentFont.getBaselinePosition();
        }

        // Arrange horizontal alignments
        int x = xPos;
        if (isFlag(anch, Graphics.RIGHT)) {
            x = xPos - boundingBox[GraphicsContext.RECT_WIDTH];
        }
        if (isFlag(anch, Graphics.HCENTER)) {
            x = xPos - boundingBox[GraphicsContext.RECT_WIDTH] / 2;
        }
        synchronized(flushLock) {
            if(!buffered) {
                final int x_ = x;
                final int y_ = y;
                ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                    public void run() {
                        gc.drawString(localStr, x_, y_, true);
                    }});
            } else {
                gc.drawString(localStr, x, y, true);
            }
        }
    }

    private static boolean isFlag(int anchor, int flag) {
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
            int xPos, int yPos, int anch) {
        drawString(string.substring(offset, length), xPos, yPos, anch);
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
    public void drawChar(char c, int xPos, int yPos, int anch) {
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
                          int xPos, int yPos, int anch) {
        if (ch == null) {
            throw new NullPointerException(
                    MsgRepository.GRAPHICS_EXCEPTION_ARRAY_IS_NULL);
        }
        String str = null;
        try {
            str = String.valueOf(ch, offset, length);
        }
        catch (Exception e) {
            throw new ArrayIndexOutOfBoundsException(
                    MsgRepository.GRAPHICS_EXCEPTION_ARRAY_OUT_OF_BOUNDS);
        }

        drawString(str, xPos, yPos, anch);
    }

    /**
     * Checks if anchors combination is valid.
     */
    private boolean checkTextAnchors(int anch) {
        boolean retVal = false;

        int vertMask = Graphics.TOP | Graphics.BASELINE | Graphics.BOTTOM;
        int horMask = Graphics.LEFT | Graphics.RIGHT | Graphics.HCENTER;

        if (anch == 0) {
            return true;
        }

        if ((anch & ~(vertMask | horMask)) != 0) {
            return false;
        }

        int vertAchor = anch & vertMask;
        int horAchor = anch & horMask;

        if ((vertAchor == Graphics.TOP)
            || (vertAchor == Graphics.BASELINE)
            || (vertAchor == Graphics.BOTTOM)) {
            retVal = true;
        }

        if ((horAchor == Graphics.LEFT)
                || (vertAchor == Graphics.RIGHT)
                || (vertAchor == Graphics.HCENTER)) {
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
            int yPos, int anch) {

        if (image == null) {
            throw new NullPointerException(
                    MsgRepository.IMAGE_EXCEPTION_IS_NULL);
        }
        if (!checkImageAnchors(anch)) {
            throw new IllegalArgumentException(
                    MsgRepository.GRAPHICS_EXCEPTION_INVALID_ANCHOR);
        }

        int y = yPos;
        if (isFlag(anch, Graphics.VCENTER)) {
            y = yPos - image.getHeight() / 2;
        }
        if (isFlag(anch, Graphics.BOTTOM)) {
            y = yPos - image.getHeight();
        }

        int x = xPos;
        if (isFlag(anch, Graphics.HCENTER)) {
            x = xPos - image.getWidth() / 2;
        }
        if (isFlag(anch, Graphics.RIGHT)) {
            x = xPos - image.getWidth();
        }

        synchronized(flushLock) {
            if(!buffered) {
                final int x_ = x;
                final int y_ = y;
                final org.eclipse.swt.internal.qt.graphics.Image image_ = 
                    Internal_GfxPackageSupport.getImage(Image.getESWTImage(image));
                ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                    public void run() {
                        gc.drawImage(image_, x_, y_);
                    }});
            } else {
                gc.drawImage(Internal_GfxPackageSupport.getImage(Image.getESWTImage(image)), x, y);
            }
        }
    }

    /**
     * Checks if anchors combination is valid.
     */
    private boolean checkImageAnchors(int anch) {
        boolean retVal = false;

        int vertMask = Graphics.TOP | Graphics.VCENTER | Graphics.BOTTOM;
        int horMask = Graphics.LEFT | Graphics.RIGHT | Graphics.HCENTER;

        if (anch == 0) {
            return true;
        }

        if ((anch & ~(vertMask | horMask)) != 0) {
            return false;
        }

        int vertAchor = anch & vertMask;
        int horAchor = anch & horMask;

        if ((vertAchor == Graphics.TOP)
            || (vertAchor == Graphics.VCENTER)
            || (vertAchor == Graphics.BOTTOM)) {
            retVal = true;
        }

        if ((horAchor == Graphics.LEFT)
                || (vertAchor == Graphics.RIGHT)
                || (vertAchor == Graphics.HCENTER)) {
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
    public void setStrokeStyle(int newStyle) {
        if (newStyle == currentStrokeSyle) {
            return;
        }
        if (newStyle == SOLID) {
            synchronized(flushLock) {
                if(!buffered) {
                    ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                        public void run() {
                            gc.setStrokeStyle(GraphicsContext.STROKE_SOLID);
                        }});
                } else {
                    gc.setStrokeStyle(GraphicsContext.STROKE_SOLID);
                }
            }
        } else {
            if (newStyle == DOTTED) {
                synchronized(flushLock) {
                    if(!buffered) {
                        ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                            public void run() {
                                gc.setStrokeStyle(GraphicsContext.STROKE_DOT);
                            }});
                    } else {
                        gc.setStrokeStyle(GraphicsContext.STROKE_DOT);
                    }
                }
            } else {
                throw new IllegalArgumentException(
                    MsgRepository.GRAPHICS_EXCEPTION_ILLEGAL_STROKE_STYLE);
            }
        }
        currentStrokeSyle = newStyle;
    }

    /**
     * Returns current stroke style.
     *
     * @return Current stroke style.
     */
    public int getStrokeStyle() {
        return currentStrokeSyle;
    }

    /**
     * Returns the color that will be used if the specified color is requested.
     *
     * @param color - color to use in 0x00RRGGBB form.
     * @return Color that will be actually used in 0x00RRGGBB form.
     */
    public int getDisplayColor(int color) {
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
                        boolean alpha) {

        if (rgb == null) {
            throw new NullPointerException(
                MsgRepository.IMAGE_EXCEPTION_DATA_IS_NULL);
        }
        synchronized(flushLock) {
            if(!buffered) {
                final int[] rgb_ = rgb;
                final int offset_ = offset;
                final int scanlength_ = scanlength;
                final int x_ = x;
                final int y_ = y;
                final int w_ = w;
                final int h_ = h;
                final boolean alpha_ = alpha;
                
                ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                    public void run() {
                        gc.drawRGB(rgb_, offset_, scanlength_, x_, y_, w_, h_, alpha_);
                    }});
            } else {
                gc.drawRGB(rgb, offset, scanlength, x, y, w, h, alpha);
            }
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
                             int yPos3) {
        final int[] points = {xPos1, yPos1, xPos2, yPos2, xPos3, yPos3};
        synchronized(flushLock) {
            if(!buffered) {
                ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                    public void run() {
                        gc.fillPolygon(points);
                    }});
            } else {
                gc.fillPolygon(points);
            }
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
                         int anch) {

        if (this.parentImage == null) {
            // this Graphics belongs to a screen device.
            throw new IllegalStateException(
                MsgRepository.GRAPHICS_EXCEPTION_DESTINATION_IS_SCREEN);
        }

        if (!javax.microedition.lcdui.Image.validateRegion(parentImage
                .getWidth(), parentImage.getHeight(), xFrom, yFrom, w, h)) {
            throw new IllegalArgumentException(
                MsgRepository.IMAGE_EXCEPTION_INVALID_REGION);
        }

        // Arrange vertical alignments
        int destY = yTo;
        if (isFlag(anch, Graphics.BOTTOM)) {
            destY = yTo - h;
        }
        if (isFlag(anch, Graphics.VCENTER)) {
            destY = yTo - h / 2;
        }

        // Arrange horizontal alignments
        int destX = xTo;
        if (isFlag(anch, Graphics.RIGHT)) {
            destX = xTo - w;
        }
        if (isFlag(anch, Graphics.HCENTER)) {
            destX = xTo - w / 2;
        }

        synchronized(flushLock) {
            if(!buffered) {
                final int x1 = xFrom;
                final int y1 = yFrom;
                final int width = w;
                final int height = h;
                final int x2 = destX;
                final int y2 = destY;

                ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                    public void run() {
                        gc.copyArea(x1, y1, width, height, x2, y2);
                    }});
            } else {
                gc.copyArea(xFrom, yFrom, w, h, destX, destY);
            }
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
                           int anch) {
      
        if (srcImage == null) {
            throw new NullPointerException(
                    MsgRepository.IMAGE_EXCEPTION_IS_NULL);
        }
        if (srcImage == parentImage) {
            throw new IllegalArgumentException(
                    MsgRepository.GRAPHICS_EXCEPTION_SAME_SOURCE_AND_DESTINATION);
        }
        if (!javax.microedition.lcdui.Image.validateTransform(transform)) {
            throw new IllegalArgumentException(
                    MsgRepository.IMAGE_EXCEPTION_INVALID_TRANSFORM);
        }
        if (!checkImageAnchors(anch)) {
            throw new IllegalArgumentException(
                    MsgRepository.GRAPHICS_EXCEPTION_INVALID_ANCHOR);
        }
        if (!javax.microedition.lcdui.Image.validateRegion(srcImage.getWidth(),
                srcImage.getHeight(), xSrc, ySrc, width, height)) {
            throw new IllegalArgumentException(
                    MsgRepository.IMAGE_EXCEPTION_INVALID_REGION);
        }

        // Arrange vertical alignments
        int y = yDst;
        if (isFlag(anch, Graphics.VCENTER)) {
            y = yDst - srcImage.getHeight() / 2;
        }
        if (isFlag(anch, Graphics.BOTTOM)) {
            y = yDst - srcImage.getHeight();
        }

        // Arrange horizontal alignments
        int x = xDst;
        if (isFlag(anch, Graphics.HCENTER)) {
            x = xDst - srcImage.getWidth() / 2;
        }
        if (isFlag(anch, Graphics.RIGHT)) {
            x = xDst - srcImage.getWidth();
        }
        
        final int gcTransform = Image.getCgTransformValue(transform);
        synchronized(flushLock) {
            if(!buffered) { 
                final int localXDst = x;
                final int localYDst = y;
                final int localW = width;
                final int localH = height;
                final int localXSrc = xSrc;
                final int localYSrc = ySrc;
                final org.eclipse.swt.internal.qt.graphics.Image localImage = 
                    Internal_GfxPackageSupport.getImage(Image.getESWTImage(srcImage));
                ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                    public void run() {
                        gc.drawImage(localImage, localXDst, localYDst, localW, localH, localXSrc, localYSrc, localW, localH, gcTransform);
                    }});
            } else {
                gc.drawImage(Internal_GfxPackageSupport.getImage(Image.getESWTImage(srcImage)), 
                             x, y, width, height, xSrc, ySrc, width, height, gcTransform);
            }
        }
    }

    /**
     * Return DirectGraphics associated with this instance.
     */
    DirectGraphics getDirectGraphics() {
        if (directGraphics == null) {
            directGraphics = new DirectGraphicsImpl(this);
        }
        return directGraphics;
    }

    /**
     * Return native graphic context.
     */
    GraphicsContext getGc() {
        return gc;
    }

}
