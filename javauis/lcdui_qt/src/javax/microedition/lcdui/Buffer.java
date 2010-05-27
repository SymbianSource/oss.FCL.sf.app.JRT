/*
* Copyright (c) 2009,2010 Nokia Corporation and/or its subsidiary(-ies).
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
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
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

    // Constants for buffer host types
    final static int HOST_TYPE_IMAGE = 1;
    final static int HOST_TYPE_CANVAS = 2;
    final static int HOST_TYPE_CUSTOMITEM = 3;

    // Flags for raising settings validation
    private final static int NONE            = 0;
    private final static int COLOR           = 1;
    private final static int CLIP            = 2;
    private final static int FONT            = 4;
    private final static int STROKESTYLE     = 8;
    private final static int COORS_TRANSLATION = 16;

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
    private boolean started;
    private int clientCount;

    // The target window surface where this
    // instance flushes draw primitives
    WindowSurface windowSurface;

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
     * Initializes data, called once
     */
    void init()
    {
        clientCount = 0;
        gc = new GraphicsContext();
        commandBuffer = new JavaCommandBuffer();
        gc.bindTarget(commandBuffer);
        writeControlBoundsToBuffer(false);
        started = true;
    }

    /**
     * Defines the bounds of the host.
     * Bounds are used for restricting the rendering in
     * the area of the control that is being updated. With Images
     * the bounds are not used.
     *
     * @param crtl The Control of the host
     * @param clienArea The area of the control which can be drawn by Graphics
     */
    void setControlBounds(final Control control)
    {
        ESWTUIThreadRunner.safeSyncExec(new Runnable()
        {
            public void run()
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
        });
    }

    /**
     * Performs binding to target in host specific way
     */
    abstract void bindToHost(GraphicsContext gc);

    /**
     * Getter for the host of the buffer
     * @return The host
     */
    abstract Object getHost();

    /**
     * Getter for the host type
     * @return One of host types defined in this class
     */
    abstract int getHostType();

    /**
     *  Setups the window surface if not setup already.
     *  This method must be called before flushing buffer to
     *  window surface
     *  Note. must be called in UI thread
     */
    abstract void setupWindowSurface();

    /**
     * Creates and returns new Graphics instance
     * @return new Graphics instance
     */
    Graphics getGraphics()
    {
        if(!started)
        {
            init();
        }
        clientCount++;
        return new Graphics(this);
    }

    /**
     * Synchronizes this buffer with the actual target
     * must be called in UI thread. If no Graphics instances
     * are creates, sync has no effect
     */
    void sync()
    {
        if(!started)
        {
            return;
        }
        // if there's nothing to flush return
        if(!commandBuffer.containsDrawnPrimitives())
        {
            return;
        }
        doRelease();
        bindToHost(gc);
        gc.render(commandBuffer);
        doRelease();

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
        doRelease();
        gc.dispose();
        gc = null;
        commandBuffer = null;
    }

    void copyArea(int x1, int y1, int width, int height, int x2, int y2, Graphics client)
    {
        gc.copyArea(x1, y1, width, height, x2, y2);
    }

    void fillRect(int x, int y, int w, int h, Graphics client)
    {
        validateAndApplySettings((COLOR|CLIP|COORS_TRANSLATION), client);
        gc.fillRect(x, y, w, h);
    }

    void fillRoundRect(int x, int y, int w, int h, int arcW, int arcH, Graphics client)
    {
        validateAndApplySettings((COLOR|CLIP|COORS_TRANSLATION), client);
        gc.fillRoundRect(x, y, w, h, arcW, arcH);
    }

    void fillArc(int x, int y, int w, int h, int startAngle, int arcAngle, Graphics client)
    {
        validateAndApplySettings((COLOR|CLIP|COORS_TRANSLATION), client);
        gc.fillArc(x, y, w, h, startAngle, arcAngle);
    }

    void fillTriangle(int[] points, Graphics client)
    {
        validateAndApplySettings((COLOR|CLIP|COORS_TRANSLATION), client);
        gc.fillPolygon(points);
    }

    void setClip(int x, int y, int w, int h, Graphics client)
    {
        validateAndApplySettings(COORS_TRANSLATION, client);
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
        Rectangle rect = clipToDisplayCoords(x, y, w, h);
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
        validateAndApplySettings((COLOR|CLIP|COORS_TRANSLATION|STROKESTYLE), client);
        gc.drawLine(xStart, yStart, xEnd, yEnd);
    }

    void drawRect(int x, int y, int w, int h, Graphics client)
    {
        validateAndApplySettings((COLOR|CLIP|COORS_TRANSLATION|STROKESTYLE), client);
        gc.drawRect(x, y, w, h);
    }

    void drawRoundRect(int x, int y, int w, int h, int arcW, int arcH, Graphics client)
    {
        validateAndApplySettings((COLOR|CLIP|COORS_TRANSLATION|STROKESTYLE), client);
        gc.drawRoundRect(x, y, w, h, arcW, arcH);
    }

    void drawArc(int x, int y, int w, int h, int startAngle, int arcAngle, Graphics client)
    {
        validateAndApplySettings((COLOR|CLIP|COORS_TRANSLATION|STROKESTYLE), client);
        gc.drawArc(x, y, w, h, startAngle, arcAngle);
    }

    void drawString(String string, int x, int y, Graphics client)
    {
        validateAndApplySettings((COLOR|CLIP|COORS_TRANSLATION|FONT), client);
        gc.drawString(string, x, y, true);
    }

    void drawImage(org.eclipse.swt.internal.qt.graphics.Image image, int x,int y, Graphics client)
    {
        validateAndApplySettings((CLIP|COORS_TRANSLATION), client);
        gc.drawImage(image, x, y);
    }

    void drawImage(org.eclipse.swt.internal.qt.graphics.Image image, int xDst, int yDst,
                   int wDst, int hDst, int xSrc, int ySrc, int wSrc, int hSrc,
                   int transform, Graphics client)
    {
        validateAndApplySettings((CLIP|COORS_TRANSLATION), client);
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
        validateAndApplySettings((CLIP|COORS_TRANSLATION), client);
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
        validateAndApplySettings((CLIP|COORS_TRANSLATION), client);
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
        validateAndApplySettings((CLIP|COORS_TRANSLATION), client);
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
        validateAndApplySettings((CLIP|COORS_TRANSLATION), client);
        gc.drawRGB(rgb, offset, scanlength, x, y, w, h, alpha, manipulation, format);
    }

    void drawPolygon(int[] points, Graphics client)
    {
        validateAndApplySettings((COLOR|CLIP|COORS_TRANSLATION|STROKESTYLE), client);
        gc.drawPolygon(points);
    }

    void fillPolygon(int[] points, Graphics client)
    {
        validateAndApplySettings((COLOR|CLIP|COORS_TRANSLATION), client);
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
     * Translates given rectangle to display/window surface coordinates
     * and outlines the clip inside the control bounds.
     *
     * @param x The x-coordinate of the rectangle
     * @param y The y-coordinate of the rectangle
     * @param w The width of the rectangle
     * @param h The height of the rectangle
     *
     */
    private Rectangle clipToDisplayCoords(int x, int y, int w, int h)
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

        return new Rectangle(clipX1, clipY1, (clipX2 - clipX1) , (clipY1 - clipY2));
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
        if(!clientChanged(client))
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

                Rectangle rect = clipToDisplayCoords(client.currentClip[0], client.currentClip[1],
                                                     client.currentClip[2], client.currentClip[3]);
                gc.setClip(rect.x, rect.y, rect.width, rect.height, false);
                bufferClip.x = client.currentClip[0];
                bufferClip.y = client.currentClip[1];
                bufferClip.width = client.currentClip[2];
                bufferClip.height = client.currentClip[3];
            }
        }
        if((COORS_TRANSLATION & flags) != 0)
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
            if(bufferStrokeStyle != client.currentStrokeSyle)
            {
                gc.setStrokeStyle(Graphics.mapStrokeStyle(client.currentStrokeSyle));
                bufferStrokeStyle = client.currentStrokeSyle;
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

    private void doRelease()
    {
        gc.releaseTarget();
    }
}
