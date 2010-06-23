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

import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Widget;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.internal.qt.graphics.GraphicsContext;
import org.eclipse.swt.internal.qt.graphics.JavaCommandBuffer;
import org.eclipse.swt.internal.qt.graphics.WindowSurface;

/**
 * This class works a proxy between instances of Graphics class and common graphics
 * (command buffer and GraphicsContext). The relation of this class to Graphics is one-to-many, i.e.
 * one GraphicsBuffer instance handles multiple Graphics instances.
 *
 * Each LCDUI API class instance providing access to Graphics (Image, Canvas, GameCanvas, CustomItem),
 * must instantiate one GraphicsBuffer and use it for creating instances of Graphics class.
 *
 * The implementation is not thread safe, thus all serialization must be implemented in
 * callers, for instance the Graphics implementation serializes all calls to this class.
 * Furthermore this class does not take care of switching to UI thread, it must be done by
 * caller object.
 */

abstract class Buffer
{

    // Default values for Graphics 
    final static Font defaultFont = Font.getDefaultFont();
    final static int defaultColor = 0xff000000;
    final static int defaultStrokeStyle = Graphics.SOLID;
    final static int defaultTranslateX = 0;
    final static int defaultTranslateY = 0;

    // Constants for buffer host types
    final static int HOST_TYPE_IMAGE = 1;
    final static int HOST_TYPE_CANVAS = 2;
    final static int HOST_TYPE_CUSTOMITEM = 3;

    // Flags for raising settings validation
    // FORCE_SETTINGS Forces settings to validated also in case
    // there's only one Graphics instance as client, normally 
    // settings are not checked in such case
    final static int NONE            = 0;
    final static int COLOR           = 1;
    final static int CLIP            = 2;
    final static int FONT            = 4;
    final static int STROKESTYLE     = 8;
    final static int COORDS_TRANSLATION = 16; 
    final static int FORCE_SETTINGS    = 32; 

    // Graphics settings active in buffer
    // all values are comparable to those
    // in Graphics except font which is stored
    // as handle instead of LCDUI Font
    private int bufferFontHandle;
    private int bufferColor;
    private int bufferStrokeStyle;
    private int bufferTranslateX;
    private int bufferTranslateY;
    private Rectangle bufferClip;

    private GraphicsContext gc;
    private JavaCommandBuffer commandBuffer;
    private Rectangle hostBounds;
    private Graphics currentClient;
    private boolean isInitialized;
    private int clientCount;
    
    private boolean isSurfaceSessionOpen;

    /**
     * Constructor
     */
    protected Buffer()
    {
        hostBounds = new Rectangle(0,0,0,0);
        bufferClip = new Rectangle(0,0,0,0);
        bufferFontHandle = 0;
        bufferColor = 0xff000000;
        bufferStrokeStyle = Graphics.SOLID;
        bufferTranslateX = 0;
        bufferTranslateY = 0;
    }

    /**
     * Creates Buffer instance based on the type of given host object 
     * and the platform (symbian/linux) currently running on.
     * 
     * @param host The host target where pixels are drawn. Given object must be Canvas, CustomItem or Image.
     * @param control The eSWT control associated with the target, or null if the host is Image
     * @return New buffer instance
     */
    static Buffer createInstance(Object host, Control control)
    {
        if(host instanceof Canvas) 
        {
            if(OS.windowServer == OS.WS_SYMBIAN_S60)
            {   
                return new CanvasBufferSymbian((Canvas) host, control);
            }
            else if(OS.windowServer == OS.WS_X11)
            {
                return new CanvasBufferLinux((Canvas) host, control);
            }
            return null;
        } 
        else if(host instanceof CustomItem)
        {
            if(OS.windowServer == OS.WS_SYMBIAN_S60)
            {   
                return new CustomItemBufferSymbian((CustomItem) host, control);
            }
            else if(OS.windowServer == OS.WS_X11)
            {
                return new CustomItemBufferLinux((CustomItem) host, control);
            }
            return null;
        } 
        else if(host instanceof Image) 
        {
            return new ImageBuffer((Image) host);   
        }
        return null;
    }
    
    /**
     * Initializes data, called once
     */
    protected void init()
    {
        clientCount = 0;
        gc = new GraphicsContext();
        commandBuffer = new JavaCommandBuffer();
        gc.bindTarget(commandBuffer);
        writeControlBoundsToBuffer(false);
        isInitialized = true;
    }

    /**
     * Defines the bounds of the host.
     * Bounds are used for restricting the rendering in
     * the area of the control that is being updated. 
     *
     * @param crtl The Control of the host
     * @param clienArea The area of the control which can be drawn by Graphics
     */
    void setControlBounds(final Control control)
    {
        // This implementation is based on the fact that
        // the QWindowSurface has the size of the shell active area
        // not the whole display, thus Shell clientArea equals QWindowSurface.
        // This might change in future if/when Qt starts
        // rendering e.g. the status pane i.e. the whole display
        // to window surface
        Point controlLoc = control.toDisplay(0,0);
        Point shellLoc = control.getShell().toDisplay(0,0);
        hostBounds.x = controlLoc.x - shellLoc.x;
        hostBounds.y = controlLoc.y - shellLoc.y;
        hostBounds.width = control.getBounds().width;
        hostBounds.height = control.getBounds().height;
    }

    /**
     * Defines the bounds of the host.
     *
     * @param crtl The Control of the host
     * @param clienArea The area of the control which can be drawn by Graphics
     */
    void setImageBounds(int width, int height)
    {
        hostBounds.x = 0;
        hostBounds.y = 0;
        hostBounds.width = width;
        hostBounds.height = height;
    }

    /**
     * Prepares surface for a new frame and starts paint session. 
     * Must be called in UI thread (sync calls this automatically)
     * and at the start of new frame. The rectangle provided as 
     * arguments are in control coordinates.
     * 
     * @param x The x-coordinate of the area to be painted
     * @param y The y-coordinate of the area to be painted
     * @param w The width of the area to be painted
     * @param h The height of the area to be painted
     */
    void startFrame(int x, int y, int w, int h)
    {
        if(!isSurfaceSessionOpen)
        {
            beginPaint(x, y, w, h);
            isSurfaceSessionOpen = true;
        }
    }
    
    /**
     * Ends frame painting session. Must be called in UI thread and
     * at the end of the frame. BlitToDisplay calls this automatically.
     */
    void endFrame()
    {
        if(isSurfaceSessionOpen)
        {
            endPaint();
            isSurfaceSessionOpen = false;
        }
    }
    
    /**
     * Transfers the result of rendering to display.
     * @param gc The graphics context used for blit, may be null in some cases
     * @param widget The widget that is the target 
     */
    void blitToDisplay(GraphicsContext gc, Widget widget)
    {
        endFrame();
        blit(gc, widget);
    }
    
    /** 
     * Prepares surface for painting, implemented by
     * child implementation.
     * @param x The x-coordinate of the area to be painted
     * @param y The y-coordinate of the area to be painted
     * @param w The width of the area to be painted
     * @param h The height of the area to be painted
     */
    abstract void beginPaint(int x, int y, int w, int h);
    
    /**
     * Ends frame painting session. Must be called in UI thread and
     * at the end of the frame. Implemented by
     * child implementation.
     */
    abstract void endPaint();
    
    /**
     * Performs binding to target in host specific way. Implemented by
     * child implementation.
     */
    abstract void bindToHost(GraphicsContext gc);
    
    /** 
     * Performs the actual blit operation in child class implementation.
     * @param gc The graphics context used for blit, may be null in some cases
     * @param widget The widget that is the target 
     */
    abstract void blit(GraphicsContext gc, Widget widget);
    
    /**
     * Getter for the host of the buffer, implemented by
     * child implementation.
     * @return The host
     */
    abstract Object getHost();

    /**
     * Getter for the host type,implemented by
     * child implementation.
     * @return One of host types defined in this class
     */
    abstract int getHostType();

    /**
     * Status checker that indicates if this instance has requested a synchronous paint event, 
     * implemented by child implementation.
     * @return True if this instance has requested a redraw paint event, otherwise false
     */
    abstract boolean isPaintingActive(); 
    
    /**
     * Creates and returns new Graphics instance
     * @return new Graphics instance
     */
    Graphics getGraphics()
    {
        if(!isInitialized)
        {
            init();
        }
        clientCount++;
        // In case this is the first Graphics instance
        // write the default values to the buffer
        if(clientCount == 1) 
        {
            writeDefaultValuesToBuffer();   
        }
        return new Graphics(this, hostBounds );
    }

    /**
     * Synchronizes this buffer with the actual target
     * must be called in UI thread. If no Graphics instances
     * are created, sync has no effect. This variant always closes 
     * the surface session unconditionally
     */
    void sync() 
    {
        sync(true);
    }
    
    /**
     * Synchronizes this buffer with the actual target
     * must be called in UI thread. If no Graphics instances
     * are created, sync has no effect
     * 
     * @param closeSurfaceSession If true the endFrame is called after sync has been 
     *                            performed closing the surface session, otherwise
     *                            endFrame is performed and surface session is left open
     */
    void sync(boolean closeSurfaceSession)
    {
        if(!isInitialized)
        {
            return;
        }
        
        // if there's nothing to flush return
        if(!commandBuffer.containsDrawnPrimitives())
        {
            return;
        }
        
        // Start surface session if not started yet
        startFrame(hostBounds.x, hostBounds.y , hostBounds.width , hostBounds.height);
        
        doRelease();
        bindToHost(gc);
        gc.render(commandBuffer);
        doRelease();

        // Close surface session
        if(closeSurfaceSession)
        {
            endFrame();
        }
        
        // Reset commands
        commandBuffer.reset();

        // Write last settings to buffer
        // as they are reset in bind
        gc.bindTarget(commandBuffer);
        gc.setFont(bufferFontHandle);
        gc.setBackgroundColor(bufferColor, true);
        gc.setForegroundColor(bufferColor, true);
        writeControlBoundsToBuffer(true);
    }

    /**
     * Decreases the client reference count,
     * should be called by Graphics instances when
     * they are about to be disposed
     */
    void removeRef()
    {
        clientCount--;
    }

    /**
     * Disposes this instance
     */
    void dispose()
    {
        if(gc != null) 
        {
            doRelease();
            gc.dispose();
            gc = null;
        }
        commandBuffer = null;
    }

    void copyArea(int x1, int y1, int width, int height, int x2, int y2, Graphics client)
    {
        gc.copyArea(x1, y1, width, height, x2, y2);
    }

    void fillRect(int x, int y, int w, int h, Graphics client)
    {
        validateAndApplySettings((COLOR|CLIP|COORDS_TRANSLATION), client);
        gc.fillRect(x, y, w, h);
    }

    void fillRoundRect(int x, int y, int w, int h, int arcW, int arcH, Graphics client)
    {
        validateAndApplySettings((COLOR|CLIP|COORDS_TRANSLATION), client);
        gc.fillRoundRect(x, y, w, h, arcW, arcH);
    }

    void fillArc(int x, int y, int w, int h, int startAngle, int arcAngle, Graphics client)
    {
        validateAndApplySettings((COLOR|CLIP|COORDS_TRANSLATION), client);
        gc.fillArc(x, y, w, h, startAngle, arcAngle);
    }

    void fillTriangle(int[] points, Graphics client)
    {
        validateAndApplySettings((COLOR|CLIP|COORDS_TRANSLATION), client);
        gc.fillPolygon(points);
    }

    void setClip(int x, int y, int w, int h, Graphics client)
    {
        validateAndApplySettings(COORDS_TRANSLATION, client);
        // check if given clip is already active in buffer
        if((bufferClip.x == x) && (bufferClip.y == y) &&
                (bufferClip.width == w) && (bufferClip.height== h))
        {
            return;
        }
        // Images do not need special handling
        if(getHostType() == HOST_TYPE_IMAGE)
        {
            gc.setClip(x, y, w, h, false);
            return;
        }

        // translate clip to display coordinates and apply
        Rectangle rect = clipToWindowCoords(x, y, w, h);
        if(rect.isEmpty())
        {
            // check is buffer clip is already up to date
            if(bufferClip.isEmpty())
            {
                return;
            }
            else
            {
                // This is a special case, where the clip defines
                // an area outside control bounds and due to that the clip
                // is set to zero size, in order t prevent drawing on top of other controls.
                // In all other cases the buffer clip is always set
                // to the same values as the client, i.e. Graphics has.
                // So by setting the bufferClip here to zero means that the clip
                // in Graphics and the buffer are not in sync and not comparable,
                // however they will be back in sync when client sets clip
                // with an area partly or completely inside the host control.
                bufferClip = rect;
            }
        }
        else
        {
            bufferClip.x = x;
            bufferClip.y = y;
            bufferClip.width = w;
            bufferClip.height = h;
        }
        gc.setClip(rect.x, rect.y, rect.width, rect.height, false);
    }

    void setGraphicsDefaults(Graphics client)
    {
    	validateAndApplySettings((FONT|COLOR|STROKESTYLE|COORDS_TRANSLATION|FORCE_SETTINGS), client);
    }
    
    void setColor(int r, int g, int b, Graphics client)
    {
        // check if given color is already active in buffer
        if(bufferColor == (Graphics.OPAQUE_ALPHA | (r << 16) | (g << 8) | b))
        {
            return;
        }
        gc.setForegroundColor(r, g, b);
        gc.setBackgroundColor(r, g, b);

        // Cache active color
        bufferColor = (Graphics.OPAQUE_ALPHA | (r << 16) | (g << 8) | b);
    }

    void setFont(int fontHandle, Graphics client)
    {
        // check if given font is already active in buffer
        if(bufferFontHandle == fontHandle)
        {
            return;
        }
        gc.setFont(fontHandle);
        // Cache active setting
        bufferFontHandle = fontHandle;
    }

    void setStrokeStyle(int cgfxStyle, int graphicsStyle, Graphics client)
    {
        if(bufferStrokeStyle == graphicsStyle)
        {
            return;
        }
        gc.setStrokeStyle(cgfxStyle);
        // Cache active setting
        bufferStrokeStyle = graphicsStyle;
    }

    void translate(int xDelta, int yDelta, Graphics client)
    {
        if((xDelta == 0) && (yDelta == 0))
        {
            return;
        }
        gc.translate(xDelta, yDelta);
        // Cache active settings
        bufferTranslateX += xDelta;
        bufferTranslateY += yDelta;
    }

    void drawLine(int xStart, int yStart, int xEnd, int yEnd, Graphics client)
    {
        validateAndApplySettings((COLOR|CLIP|COORDS_TRANSLATION|STROKESTYLE), client);
        gc.drawLine(xStart, yStart, xEnd, yEnd);
    }

    void drawRect(int x, int y, int w, int h, Graphics client)
    {
        validateAndApplySettings((COLOR|CLIP|COORDS_TRANSLATION|STROKESTYLE), client);
        gc.drawRect(x, y, w, h);
    }

    void drawRoundRect(int x, int y, int w, int h, int arcW, int arcH, Graphics client)
    {
        validateAndApplySettings((COLOR|CLIP|COORDS_TRANSLATION|STROKESTYLE), client);
        gc.drawRoundRect(x, y, w, h, arcW, arcH);
    }

    void drawArc(int x, int y, int w, int h, int startAngle, int arcAngle, Graphics client)
    {
        validateAndApplySettings((COLOR|CLIP|COORDS_TRANSLATION|STROKESTYLE), client);
        gc.drawArc(x, y, w, h, startAngle, arcAngle);
    }

    void drawString(String string, int x, int y, Graphics client)
    {
        validateAndApplySettings((COLOR|CLIP|COORDS_TRANSLATION|FONT), client);
        gc.drawString(string, x, y, true);
    }

    void drawImage(org.eclipse.swt.internal.qt.graphics.Image image, int x,int y, Graphics client)
    {
        validateAndApplySettings((CLIP|COORDS_TRANSLATION), client);
        gc.drawImage(image, x, y);
    }

    void drawImage(org.eclipse.swt.internal.qt.graphics.Image image, int xDst, int yDst,
                   int wDst, int hDst, int xSrc, int ySrc, int wSrc, int hSrc,
                   int transform, Graphics client)
    {
        validateAndApplySettings((CLIP|COORDS_TRANSLATION), client);
        gc.drawImage(image, xDst, yDst, wDst, hDst, xSrc, ySrc, wSrc, hSrc, transform);
    }

    void drawRGB(int[] rgb,
                 int offset,
                 int scanlength,
                 int x,
                 int y,
                 int w,
                 int h,
                 boolean alpha,
                 Graphics client)
    {
        validateAndApplySettings((CLIP|COORDS_TRANSLATION), client);
        gc.drawRGB(rgb, offset, scanlength, x, y, w, h, alpha);
    }

    void drawRGB(int[] rgb,
                 int offset,
                 int scanlength,
                 int x,
                 int y,
                 int w,
                 int h,
                 boolean alpha,
                 int manipulation,
                 Graphics client)
    {
        validateAndApplySettings((CLIP|COORDS_TRANSLATION), client);
        gc.drawRGB(rgb, offset, scanlength, x, y, w, h, alpha, manipulation);
    }

    void drawRGB(byte[] rgb,
                 byte[] transparencyMask,
                 int offset,
                 int scanlength,
                 int x,
                 int y,
                 int w,
                 int h,
                 int manipulation,
                 int format,
                 Graphics client)
    {
        validateAndApplySettings((CLIP|COORDS_TRANSLATION), client);
        gc.drawRGB(rgb, transparencyMask, offset, scanlength, x, y, w, h, manipulation, format);
    }

    void drawRGB(short[] rgb,
                 int offset,
                 int scanlength,
                 int x,
                 int y,
                 int w,
                 int h,
                 boolean alpha,
                 int manipulation,
                 int format,
                 Graphics client)
    {
        validateAndApplySettings((CLIP|COORDS_TRANSLATION), client);
        gc.drawRGB(rgb, offset, scanlength, x, y, w, h, alpha, manipulation, format);
    }

    void drawPolygon(int[] points, Graphics client)
    {
        validateAndApplySettings((COLOR|CLIP|COORDS_TRANSLATION|STROKESTYLE), client);
        gc.drawPolygon(points);
    }

    void fillPolygon(int[] points, Graphics client)
    {
        validateAndApplySettings((COLOR|CLIP|COORDS_TRANSLATION), client);
        gc.fillPolygon(points);
    }

    void setARGBColor(int argb, Graphics client)
    {
        if(bufferColor == argb)
        {
            return;
        }
        gc.setBackgroundColor(argb, true);
        gc.setForegroundColor(argb, true);
        // Cache active color
        bufferColor = argb;
    }

    /**
     * Translates given rectangle to window coordinates.
     * Coordinate system translation does not affect on this method.
     *
     * @param x The x-coordinate of the rectangle
     * @param y The y-coordinate of the rectangle
     * @param w The width of the rectangle
     * @param h The height of the rectangle
     * @return
     */
    Rectangle toWindowCoordinates(int x, int y, int w, int h)
    {
        final int xInDpy = hostBounds.x + x;
        final int yInDpy = hostBounds.y + y;
        return new Rectangle(xInDpy, yInDpy, xInDpy + w, yInDpy + h);
    }


    /**
     * Returns the WindowSurface that relates to this Buffer
     *
     * @return WindowSurface owned by this Buffer
     */
    WindowSurface getWindowSurface()
    {
        return null;
    }
    
    boolean containsDrawnPrimitives() {
        boolean result = false;
        if(commandBuffer != null) {
            result = commandBuffer.containsDrawnPrimitives();
        }
        return result;
    }
    
    /**
     * Translates given rectangle to window surface coordinates
     * and outlines the clip inside the control bounds.
     *
     * @param x The x-coordinate of the rectangle
     * @param y The y-coordinate of the rectangle
     * @param w The width of the rectangle
     * @param h The height of the rectangle
     *
     */
    private Rectangle clipToWindowCoords(int x, int y, int w, int h)
    {
        // Bottom-right corner of control bounds in window coordinates
        final int hostX2 = hostBounds.x + hostBounds.width;
        final int hostY2 = hostBounds.y + hostBounds.height;
        // clip in window coordinates
        final int clipX1Dpy = hostBounds.x + bufferTranslateX + x;
        final int clipY1Dpy = hostBounds.y + bufferTranslateY + y;
        final int clipX2Dpy = hostBounds.x + bufferTranslateX + x + w;
        final int clipY2Dpy = hostBounds.y + bufferTranslateY + y + h;
        int clipX1 = x;
        int clipY1 = y;
        int clipX2 = x + w;
        int clipY2 = y + h;

        // check if the clip is completely outside of control bounds
        if(!hostBounds.contains(clipX1Dpy, clipY1Dpy) && !hostBounds.contains(clipX2Dpy, clipY2Dpy))
        {
            return new Rectangle(0,0,0,0);
        }

        // At least one corner is inside control bounds so
        // adjust clip coordinates so that they lie within control bounds
        clipX1 = clipX1Dpy < hostBounds.x ? (x + (hostBounds.x - clipX1Dpy)) : x;
        clipX1 = clipX1Dpy > hostX2 ? (x - (clipX1Dpy - hostX2)) : x;
        clipY1 = clipY1Dpy < hostBounds.y ? (y + (hostBounds.y - clipY1Dpy)) : y;
        clipY1 = clipY1Dpy > hostY2 ? (y - (clipY1Dpy - hostY2)) : y;

        clipX2 = clipX2Dpy < hostBounds.x ? (clipX2 + (hostBounds.x - clipX1Dpy)) : clipX2;
        clipX2 = clipX2Dpy > hostX2 ? (clipX2 - (clipX1Dpy - hostX2)) : clipX2;
        clipY2 = clipY2Dpy < hostBounds.y ? (clipY2 + (hostBounds.y - clipY1Dpy)) : clipY2;
        clipY2 = clipY2Dpy > hostY2 ? (clipY2 - (clipY1Dpy - hostY2)) : clipY2;

        return new Rectangle(clipX1, clipY1, (clipX2 - clipX1) , (clipY2 - clipY1));
    }

    /**
     * Validates the current settings active in buffer against
     * caller settings and updated needed settings in buffer when
     * that is required. This method does not update anything if
     * there is only one (reference_count == 1)Graphics instance using this buffer.
     *
     * @param flags The settings that need to be checked
     * @param client The Graphics instance that made the call
     */
    private void validateAndApplySettings(int flags, Graphics client)
    {
        if(!clientChanged(client) && (FORCE_SETTINGS & flags) == 0)
        {
            return;
        }
        if((COLOR & flags) != 0)
        {
            if(bufferColor != client.currentColor)
            {
                gc.setBackgroundColor(client.currentColor, true);
                gc.setForegroundColor(client.currentColor, true);
                bufferColor = client.currentColor;
            }
        }
        if((CLIP & flags) != 0)
        {
            if(client.currentClip[0] != bufferClip.x &&
                    client.currentClip[1] != bufferClip.y &&
                    client.currentClip[2] != bufferClip.width &&
                    client.currentClip[3] != bufferClip.height)
            {

                Rectangle rect = clipToWindowCoords(client.currentClip[0], client.currentClip[1],
                                                     client.currentClip[2], client.currentClip[3]);
                gc.setClip(rect.x, rect.y, rect.width, rect.height, false);
                bufferClip.x = client.currentClip[0];
                bufferClip.y = client.currentClip[1];
                bufferClip.width = client.currentClip[2];
                bufferClip.height = client.currentClip[3];
            }
        }
        if((COORDS_TRANSLATION & flags) != 0)
        {
            if((bufferTranslateX != client.translateX) && (bufferTranslateY != client.translateY))
            {
                gc.translate((client.translateX - bufferTranslateX), (client.translateY - bufferTranslateY));
                bufferTranslateX = client.translateX;
                bufferTranslateY = client.translateY;
            }
        }
        if((FONT & flags) != 0)
        {
            int fontHandle = Font.getESWTFont(client.currentFont).handle;
            if(bufferFontHandle != fontHandle)
            {
                gc.setFont(fontHandle);
                bufferFontHandle = fontHandle;
            }
        }
        if((STROKESTYLE & flags) != 0)
        {
            if(bufferStrokeStyle != client.currentStrokeStyle)
            {
                gc.setStrokeStyle(Graphics.mapStrokeStyle(client.currentStrokeStyle));
                bufferStrokeStyle = client.currentStrokeStyle;
            }
        }
    }

    private boolean clientChanged(Graphics client)
    {
        if(clientCount == 1)
        {
            return false;
        }
        if(currentClient != client)
        {
            currentClient = client;
            return true;
        }
        return false;
    }

    /**
     * Writes control bounds to buffer, including translation and clip
     * @param writeClientTranslation If true write also client translation to buffer, otherwise not
     */
    private void writeControlBoundsToBuffer(boolean writeClientTranslation)
    {
        if((hostBounds.x != 0) || (hostBounds.y != 0))
        {
            gc.translate(hostBounds.x, hostBounds.y);
        }
        gc.setClip(0, 0, hostBounds.width, hostBounds.height, false);
        // Cache buffer settings
        bufferClip.x = 0;
        bufferClip.y = 0;
        bufferClip.width = hostBounds.width;
        bufferClip.height = hostBounds.height;
        // write client translation if requested
        if(writeClientTranslation)
        {
            if((bufferTranslateX != 0) || (bufferTranslateY != 0))
            {
                gc.translate(bufferTranslateX, bufferTranslateY);
            }
        }
    }

   /**
    * Writes Graphics default values to buffer
    * @param force If true defaults are written unconditionally to buffer, 
    *              otherwise only values that different than the ones in buffer
    *              are written
    */
    private void writeDefaultValuesToBuffer()
    {
        int handle = Font.getESWTFont(defaultFont).handle;
        gc.setFont(handle);
        bufferFontHandle = handle;
        gc.setBackgroundColor(defaultColor, true);
        gc.setForegroundColor(defaultColor, true);
        bufferColor = defaultColor;
        gc.setStrokeStyle(Graphics.mapStrokeStyle(defaultStrokeStyle));
        bufferStrokeStyle = defaultStrokeStyle;
        gc.resetTransform();
        bufferTranslateX = defaultTranslateX;
        bufferTranslateY = defaultTranslateY;
    }
    
    private void doRelease()
    {
        gc.releaseTarget();
    }
}
