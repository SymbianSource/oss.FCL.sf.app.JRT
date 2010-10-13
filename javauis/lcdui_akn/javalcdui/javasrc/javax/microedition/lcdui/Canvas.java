/*
* Copyright (c) 1999 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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

import com.nokia.mj.impl.rt.legacy.NativeError;
import com.nokia.mj.impl.utils.Logger;

public abstract class Canvas extends Displayable
{
    //
    // Game keys
    //
    public static final int UP=1;
    public static final int DOWN=6;
    public static final int LEFT=2;
    public static final int RIGHT=5;
    public static final int FIRE=8;
    public static final int GAME_A=9;
    public static final int GAME_B=10;
    public static final int GAME_C=11;
    public static final int GAME_D=12;
    //
    // Numbers
    //
    private static final int KEY_NUM_FIRST = 48;
    public static final int KEY_NUM0 = KEY_NUM_FIRST;
    public static final int KEY_NUM1 = KEY_NUM_FIRST+1;
    public static final int KEY_NUM2 = KEY_NUM_FIRST+2;
    public static final int KEY_NUM3 = KEY_NUM_FIRST+3;
    public static final int KEY_NUM4 = KEY_NUM_FIRST+4;
    public static final int KEY_NUM5 = KEY_NUM_FIRST+5;
    public static final int KEY_NUM6 = KEY_NUM_FIRST+6;
    public static final int KEY_NUM7 = KEY_NUM_FIRST+7;
    public static final int KEY_NUM8 = KEY_NUM_FIRST+8;
    public static final int KEY_NUM9 = KEY_NUM_FIRST+9;
    public static final int KEY_STAR = 42;
    public static final int KEY_POUND = 35;

    /**
     * Static constants that are used for indexing the <code>canvasAttribs</code> attribute array
     * in the constructor method
     */
    private static final int ATTRIB_CONTENT_HANDLE_INDEX      = 0;
    private static final int ATTRIB_WIDTH_INDEX               = 1;
    private static final int ATTRIB_HEIGHT_INDEX              = 2;
    private static final int ATTRIB_NATIVE_FRAME_BUFFER_INDEX = 3;
    private static final int ATTRIB_BACKGROUND_INDEX          = 4;
    private static final int ATTRIB_COUNT                     = 5;

    /**
    * Static constants for media key keycodes
    */
    private static final int MEDIA_KEY_PLAY                   = -20;
    private static final int MEDIA_KEY_PREVIOUS               = -21;
    private static final int MEDIA_KEY_NEXT                   = -22;
    private static final int MEDIA_KEY_STOP                   = -23;

    Device          iDevice;

    // Repaint rect
    private boolean iRepaintPending; // repaint event is pending on queue
    private int     iRepaintX1;      // inclusive
    private int     iRepaintY1;      // inclusive
    private int     iRepaintX2;      // exclusive, >= iRepaintX1
    private int     iRepaintY2;      // exclusive, >= iRepaintY1
    private boolean iFullScreen;

    Graphics        iScreenGraphics;    // may be null if we have a native framebuffer
    Image           iFrameBuffer;       // frame buffer mutable image - may be wrapper for native frame buffer
    Graphics        iFrameGraphics;     // frame buffer graphics - always from iFrameBuffer
    final boolean   iNativeFrameBuffer; // true if native canvas peer has native framebuffer

    // This flag is valid only in NGA
    private boolean iM3GContent;
    private boolean iM3GDraw = false;
    /**
     * This member variable is set <code>true</code> if the <code>Nokia-UI-Enhancement</code>
     * attribute is "CanvasHasBackground". The attribute may be placed in the JAD or the manifest.
     */
    private boolean iHasBackgroundImage;
    private boolean iIsGameCanvas;

    protected Canvas()
    {
        super();
        synchronized (iToolkit)
        {
            iDevice = iToolkit.getDevice();

            int[] canvasAttribs = new int[ATTRIB_COUNT];
            iIsGameCanvas = (this instanceof javax.microedition.lcdui.game.GameCanvas);
            // Create native side counterpart
            NativeError.check(
                _create(iToolkit.getHandle(),
                        getContainerHandle(),
                        iIsGameCanvas,
                        canvasAttribs));

            // Set the content related attributes that were read from native side
            setContent(
                canvasAttribs[ATTRIB_CONTENT_HANDLE_INDEX],
                canvasAttribs[ATTRIB_WIDTH_INDEX],
                canvasAttribs[ATTRIB_HEIGHT_INDEX]);

            // Check if native frame buffer exists
            iNativeFrameBuffer =
                (canvasAttribs[ATTRIB_NATIVE_FRAME_BUFFER_INDEX] != 0);

            if (iDevice.isDoubleBuffered() || iNativeFrameBuffer)
            {
                // If we are double buffering, we must create the
                // offscreen framebuffer here.
                CreateFrameBuffer();
            }
            // Save the info about background.
            iHasBackgroundImage =
                (canvasAttribs[ATTRIB_BACKGROUND_INDEX] != 0);

            // If is Toolkit activated, we can add Displayable to Vector
            // Displayables are removed from Vector when SizeChangedEvent arrives see Displayable.sizeChangedEvent()
            if (iToolkit.activated)
            {
                iToolkit.tmpDisplayables.addElement(this);
            }

            iM3GContent = false;
        }
    }

    public boolean isDoubleBuffered()
    {
        return (null != iFrameBuffer);
    }

    public boolean hasPointerEvents()
    {
        return iDevice.hasPointerEvents();
    }

    public boolean hasPointerMotionEvents()
    {
        return iDevice.hasPointerMotionEvents();
    }

    public boolean hasRepeatEvents()
    {
        return iDevice.hasRepeatEvents();
    }

    public int getKeyCode(int aGameAction)
    {
        final int keyCode;

        synchronized (iToolkit)
        {
            keyCode = _getKeyCode(iToolkit.getHandle(), aGameAction);
        }

        if (keyCode == 0)
        {
            throw new IllegalArgumentException();
        }

        return keyCode;
    }

    public String getKeyName(int aKeyCode)
    {
        checkKeyCode(aKeyCode);
        final String keyName;
        synchronized (iToolkit)
        {
            keyName = _getKeyName(iToolkit.getHandle(), aKeyCode);
        }
        return keyName;
    }

    public int getGameAction(int aKeyCode)
    {
        checkKeyCode(aKeyCode);
        synchronized (iToolkit)
        {
            return _getGameAction(iToolkit.getHandle(),aKeyCode);
        }
    }

    public final void repaint(int aX, int aY, int aWidth, int aHeight)
    {
        synchronized (iToolkit)
        {
            Repaint(aX, aY, aWidth, aHeight);
        }
    }

    final void Repaint(int aX,int aY,int aWidth,int aHeight)
    //
    // Merge clipping rects if already set
    //
    {
        if (aWidth<=0 || aHeight<=0)
        {
            return;
        }

        if (!IsShown())
        {
            return;
        }

        if (Invalidate(aX,aY,aWidth,aHeight))
        {
            if (!iRepaintPending)
            {
                iToolkit.postEvent(this, Toolkit.EVENT_PAINT);
                iRepaintPending = true;
            }
        }
    }

    private boolean Invalidate(int aX, int aY, int aWidth, int aHeight)
    {
        // regularize bounds
        final int x1=aX;
        final int y1=aY;
        final int x2=aX+aWidth;
        final int y2=aY+aHeight;

        // Union the current and new damage rects.

        final boolean valid = ((iRepaintX2-iRepaintX1)<=0) && ((iRepaintY2-iRepaintY1)<=0);
        if (!valid)
        {
            iRepaintX1 = Math.min(iRepaintX1, x1);
            iRepaintY1 = Math.min(iRepaintY1, y1);
            iRepaintX2 = Math.max(iRepaintX2, x2);
            iRepaintY2 = Math.max(iRepaintY2, y2);
        }
        else
        {
            iRepaintX1 = x1;
            iRepaintY1 = y1;
            iRepaintX2 = x2;
            iRepaintY2 = y2;
        }

        final int w = iWidth;
        final int h = iHeight;

        // clip to bounds.
        iRepaintX1 = iRepaintX1 > 0 ? iRepaintX1 : 0;
        iRepaintY1 = iRepaintY1 > 0 ? iRepaintY1 : 0;
        iRepaintX2 = iRepaintX2 < w ? iRepaintX2 : w;
        iRepaintY2 = iRepaintY2 < h ? iRepaintY2 : h;

        return valid;
    }

    public final void repaint()
    {
        synchronized (iToolkit)
        {
            Repaint(0,0, iWidth, iHeight);
        }
    }

    public final void serviceRepaints()
    {
        paint(false);
    }

    // Canvas spec'd to have getWidth() method override.
    public int getWidth()
    {
        return super.getWidth();
    }

    // Canvas spec'd to have getHeight() method override.
    public int getHeight()
    {
        return super.getHeight();
    }

    public void setFullScreenMode(boolean aFullScreen)
    {
        synchronized (iToolkit)
        {
            if (iFullScreen != aFullScreen)
            {
                int[] sizeReturn = new int[2];
                final int toolkit   = iToolkit.getHandle();
                final int container = getContainerHandle();
                NativeError.check(_setFullScreenMode(toolkit, container, aFullScreen, sizeReturn));
                iFullScreen = aFullScreen;
                iWidth  = sizeReturn[0];
                iHeight = sizeReturn[1];

                if (iFullScreen && iHasBackgroundImage)
                {
                    // Background should be cleared with white color when
                    // it is switched from non-full-screen (normal) mode to full-screen mode
                    // and on non-full-screen mode the background has been filled with a skin.
                    drawBackground(false);
                }
            }
        }
    }

    protected void showNotify()
    {
    }

    protected void hideNotify()
    {
    }

    protected void keyPressed(int aKeyCode)
    {
    }

    protected void keyReleased(int aKeyCode)
    {
    }

    protected void keyRepeated(int keyCode)
    {
    }

    protected void pointerPressed(int aX,int aY)
    {
    }

    protected void pointerReleased(int aX,int aY)
    {
    }

    protected void pointerDragged(int aX,int aY)
    {
    }

    protected abstract void paint(Graphics aGraphics);

    private void handlePaint(int aX, int aY, int aWidth, int aHeight, boolean forced)
    {
        synchronized (iToolkit)
        {
            // Track java generated PAINT events so we do not flood the
            // event queue. We need to distinguish between PAINT events
            // generated by calling Canvas.repaint() and those generated
            // by the native peer.
            //
            // Ideally we would have distinct EXPOSE and REPAINT events
            // to distinguish the two cases. Here we rely on the fact that
            // java generated PAINT events have zero sized damage areas as
            // the damage are is stored in the canvas (iRepaintX1, iRepaintY1 etc..).
            //
            if ((aWidth > 0) && (aHeight > 0))
            {
                //
                // A paint event was generated by the native canvas.
                //
                Invalidate(aX, aY, aWidth, aHeight);
            }
            else
            {
                //
                // A repaint event was generated by the java canvas. Record the
                // fact that it has been received so another may be dispatched.
                //
                iRepaintPending = false;
            }
        }

        paint(forced);
    }

    private void paint(boolean forced)
    {
        Graphics graphics;
        int     x;
        int     y;
        int     w;
        int     h;

        synchronized (iCallbackLock)
        {
            final int width;
            final int height;

            //
            // Synchronize to protect the update region from concurrent
            // repaint()'s. Note that this is *not* sufficient to ensure
            // that no other callbacks are called by the event thread
            // whilst the current thread is in the repaint routine, for
            // that we use the callbacklock below.
            //
            synchronized (iToolkit)
            {
                width  = iWidth;
                height = iHeight;

                x = iRepaintX1;
                y = iRepaintY1;
                w = iRepaintX2-iRepaintX1;
                h = iRepaintY2-iRepaintY1;

                iRepaintX1 = 0;
                iRepaintY1 = 0;
                iRepaintX2 = 0;
                iRepaintY2 = 0;

                if (!((w>0) && (h>0) && (IsShown() || forced)))
                {
                    return;
                }

                graphics = GetPaintGraphics();
            }

            graphics.reset(width, height);
            graphics.setClip(x, y, w, h);

            // On a non-full-screen (normal) mode Canvas the background
            // image must be initially shown, if the value of the iHasBackgroundImage
            // is true.
            if (!iFullScreen && iHasBackgroundImage && !iIsGameCanvas)
            {
                drawBackground(true);
            }
            paint(graphics);
            flush(x, y, w, h);
        }




    }

    void flush(int aX, int aY, int aWidth, int aHeight)
    {
        final Graphics screenGraphics;
        final Image    frameBuffer;
        final int width;
        final int height;
        synchronized (iToolkit)
        {
            if (aWidth < 0)
            {
                aWidth = iWidth;
            }
            if (aHeight < 0)
            {
                aHeight = iHeight;
            }
            if (iNativeFrameBuffer)
            {
                //
                // We have a native framebuffer, so canvas is responsible for
                // updating screen - send sync command down.
                //
                iToolkit.sync(getContentHandle(), aX, aY, aWidth, aHeight);
                return;
            }
            else
            {
                screenGraphics = GetScreenGraphics();
                frameBuffer = iFrameBuffer;
                width = iWidth;
                height = iHeight;
            }
        }

        //
        // If we are using a java only framebuffer we need to update screen
        // after paint or in response to GameCanvas.flushGraphics.
        //
        if (null != screenGraphics)
        {
            if (null != frameBuffer)
            {
                screenGraphics.reset(width, height);
                screenGraphics.setClip(aX, aY, aWidth, aHeight);
                screenGraphics.drawImage(frameBuffer, 0, 0, Graphics.TOP|Graphics.LEFT);
            }
            screenGraphics.flush(aX, aY, aWidth, aHeight);
        }
    }

    /**
     * Must be called with toolkit locked.
     */
    private Graphics GetPaintGraphics()
    {
        if (isDoubleBuffered())
        {
            return GetFrameGraphics();
        }
        else
        {
            return GetScreenGraphics();
        }
    }

    private Graphics GetScreenGraphics()
    {
        if (null == iScreenGraphics)
        {
            iScreenGraphics = new Graphics(iToolkit, this, getContentHandle(), iWidth, iHeight);
        }
        return iScreenGraphics;
    }

    private Graphics GetFrameGraphics()
    {
        if (null == iFrameBuffer)
        {
            throw new RuntimeException("Null frame buffer");
        }

        if (null == iFrameGraphics)
        {
            // NGA specific change.
            if (iToolkit.checkFlags(Toolkit.FLAG_NGA))
            {
                iFrameGraphics = getBufferGraphics();
            }
            else
            {
                iFrameGraphics =
                    new Graphics(iToolkit, this, iFrameBuffer.getHandle(false), iWidth, iHeight);
            }
        }
        return iFrameGraphics;
    }

    /*
     * Method handles callback events from native side.
     * aEvent - id of event
     * aData0, aData1 - data from native side, each event has specific data:
     * * EVENT_PAINT:
     *           - aData0 carries x and y coordinations of specific region
     *           - aData1 carries w and h of specific region
     * * EVENT_KEY_PRESSED:
     * * EVENT_KEY_RELEASED:
     * * EVENT_KEY_REPEATED:
     *           - aData0 carries value of scan code
     *           - aData1 carries repeats count
     * * EVENT_POINTER_PRESSED:
     * * EVENT_POINTER_RELEASED:
     * * EVENT_POINTER_DRAGGED:
     *           - aData0 carries x coordinate of pointer event
     *           - aData1 carries y coordinate of pointer event
     */
    final void handleEvent(int aEvent,int aData0, int aData1)
    {
        final boolean shown = isShown();
        switch (aEvent)
        {
        case Toolkit.EVENT_PAINT:
        case Toolkit.EVENT_FORCED_PAINT:
        {
            final int x = aData0 >>> 16;
            final int y = aData0 & 0xffff;
            final int w = aData1 >>> 16;
            final int h = aData1 & 0xffff;
            boolean forced = (aEvent == Toolkit.EVENT_FORCED_PAINT);
            handlePaint(x, y, w, h, forced);
        }
        break;

        case Toolkit.EVENT_KEY_PRESSED:
            if (shown || isMediaKey(aData0))
            {
                synchronized (iCallbackLock)
                {
                    keyPressed(aData0);
                }
            }
            break;

        case Toolkit.EVENT_KEY_RELEASED:
            if (shown || isMediaKey(aData0))
            {
                synchronized (iCallbackLock)
                {
                    keyReleased(aData0);
                }
            }
            break;

        case Toolkit.EVENT_KEY_REPEATED:
            if (shown || isMediaKey(aData0))
            {
                synchronized (iCallbackLock)
                {
                    keyRepeated(aData0);
                }
            }
            break;

        case Toolkit.EVENT_POINTER_PRESSED:
            if (shown)
            {
                synchronized (iCallbackLock)
                {
                    pointerPressed(aData0, aData1);
                }
            }
            break;
        case Toolkit.EVENT_POINTER_RELEASED:
            if (shown)
            {
                synchronized (iCallbackLock)
                {
                    pointerReleased(aData0, aData1);
                }
            }
            break;
        case Toolkit.EVENT_POINTER_DRAGGED:
            if (shown)
            {
                synchronized (iCallbackLock)
                {
                    pointerDragged(aData0, aData1);
                }
            }
            break;
        case Toolkit.EVENT_M3G_DRAW:
            if (shown)
            {
                synchronized (iCallbackLock)
                {
                    Graphics graphics = GetPaintGraphics();
                    iM3GDraw = aData0 != 0;
                }
            }
            break;
        default:
            super.handleEvent(aEvent, aData0, aData1);
            break;
        }
    }

    final void update()
    {
        repaint();
    }

    /**
     * On a non-full-screen (normal) mode Canvas the background image must be initially shown,
     * if the value of the <code>iHasBackground</code> is true.
     * @param aShowBackgroundImage If true then a background image is rendered
     * @since S60 5.0
     */
    final void drawBackground(boolean aShowBackgroundImage)
    {
        try
        {
            Graphics graphics = GetPaintGraphics();
            if (!aShowBackgroundImage)
            {
                // NGA specific change. When M3G is used,
                // cannot clear frame buffer because native
                // CMIDGraphics tracks the updates canvas
                // areas to optimise 2D texture upload to OpenGl.
                if (!(iToolkit.checkFlags(Toolkit.FLAG_NGA) &&
                        iM3GContent))
                {
                    // Background is going to be cleared with white color
                    int origColor = graphics.getColor();
                    graphics.setColor(0xffffff);
                    graphics.fillRect(0, 0, getWidth(), getHeight());
                    // The original color is restored
                    graphics.setColor(origColor);
                }
            }
            else
            {
                // Draw a backgound image e.g. skin
                // NOTE: The clipping area need to be taken in account when
                // background image must be shown.
                graphics.drawBackground(
                    this,
                    graphics.getClipX(),
                    graphics.getClipY(),
                    graphics.getClipWidth(),
                    graphics.getClipHeight());
            }
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.EJavaUI,
                        "drawBackground() - ", e);
        }
    }

    final void showNotifyEvent(Display aDisplay)
    {
        super.showNotifyEvent(aDisplay);
        try
        {
            // Check if this Canvas object is the current Displayable object managed
            // by Display object.
            // If not then the background need to be cleared with white.
            // Note: if a background image must be shown then no need to draw the background
            // here because it will be done during the painting.
            if (!iIsGameCanvas && (iDisplay.getCurrent() != this))
            {
                if (!iHasBackgroundImage || iFullScreen)
                {
                    drawBackground(false);
                }
            }
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.EJavaUI,
                        "showNotifyEvent() - ", e);
        }
        try
        {
            showNotify();
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            Logger.ELOG(Logger.EJavaUI, "showNotify() - ", t);
        }
    }

    final void hideNotifyEvent(Display aDisplay)
    {
        super.hideNotifyEvent(aDisplay);
        try
        {
            hideNotify();
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            Logger.ELOG(Logger.EJavaUI, "hideNotify() - ", t);
        }
    }

    final void sizeChangedEvent(int aWidth, int aHeight)
    {
        super.sizeChangedEvent(aWidth, aHeight);
        synchronized (iToolkit)
        {
            if (null != iScreenGraphics)
            {
                iScreenGraphics.reset(iWidth, iHeight);
            }
            if (null != iFrameGraphics)
            {
                iFrameGraphics.reset(iWidth, iHeight);
            }
        }
        repaint();
    }

    //
    // GameCanvas support
    //
    final Image initGameCanvas(boolean aSuppressKeys)
    {
        synchronized (iToolkit)
        {
            if (aSuppressKeys)
            {
                _suppressKeyEvents(iToolkit.getHandle(), getContentHandle());
            }

            if (null == iFrameBuffer)
            {
                //
                // If we have not already created the frame buffer, Canvas must
                // be configured to run in non-double buffer mode. GameCanvas
                // still requires a framebuffer so we create one here.
                //
                CreateFrameBuffer();
            }

            return iFrameBuffer;
        }
    }

    //
    //  Called by GameCanvas. Must return new Graphics object.
    //
    final Graphics getBufferGraphics()
    {
        synchronized (iToolkit)
        {
            if (null == iFrameBuffer)
            {
                throw new RuntimeException("Null frame buffer");
            }
            // NGA specific change.
            if (iToolkit.checkFlags(Toolkit.FLAG_NGA))
            {
                return new Graphics(iToolkit, this, getContentHandle(), iWidth, iHeight);
            }
            else
            {
                return new Graphics(iToolkit, this, iFrameBuffer.getHandle(false), iWidth, iHeight);
            }
        }
    }


    final void CreateFrameBuffer()
    {
        if (iNativeFrameBuffer)
        {
            //
            // Old style canvas creates its own framebuffer and we wrap it.
            //
            iFrameBuffer = new Image(this);
        }
        else
        {
            //
            // New style canvas doesn't know about the framebuffer, we create
            // the frame buffer ourselves sized large enough for the screen.
            //
            iFrameBuffer = Image.createImage(iDevice.getScreenWidth(), iDevice.getScreenHeight());
        }
    }

    final int getGameKeyStates()
    {
        synchronized (iToolkit)
        {
            return _getKeyStates(iToolkit.getHandle(), getContentHandle());
        }
    }

    //
    // Media keys.
    //
    private final boolean isMediaKey(int aKeyCode)
    {
        if (aKeyCode == MEDIA_KEY_PLAY
                || aKeyCode == MEDIA_KEY_PREVIOUS
                || aKeyCode == MEDIA_KEY_NEXT
                || aKeyCode == MEDIA_KEY_STOP)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    private void checkKeyCode(int aKeyCode)
    {
        if (aKeyCode == 0)
        {
            throw new IllegalArgumentException();
        }
    }

    int GetContentSize()
    {
        return _getCanvasSize(iToolkit.getHandle(), getContentHandle());
    }

    /*
     * This method gets the position and size of the network indicator
     * on fullscreen canvas and converts it into property string
     * in format "x,y,w,h"
     * If the network indicator is not displayed, null is returned
     */
    String getNetworkIndicatorLocation()
    {
        int position[] = new int[2];
        int size[] = new int[2];
        String returnValue = null;
        boolean indicatorExists = _getNetworkIndicatorLocation(iToolkit.getHandle(), getContentHandle(), position, size);
        if (indicatorExists)
        {
            returnValue = String.valueOf(position[0]) + "," + String.valueOf(position[1]) + "," + String.valueOf(size[0]) + "," + String.valueOf(size[1]);
        }
        return returnValue;
    }

    /*
     * This method return true if Canvas is in full screen mode,
     * false otherwise.
     */
    boolean getFullScreenMode()
    {
        return iFullScreen;
    }

    /*
     * Informs canvas about M3G content.
     */
    void setM3GContent(boolean aValue)
    {
        iM3GContent = aValue;
    }

    /*
     * This function blocking downscaled when M3G drawing some content
     */
    boolean m3gDraw()
    {
        return iM3GDraw;
    }

    private native int _create(
        int aToolkit, int aDisplayable, boolean aIsGameCanvas, int[] aCanvasAttribArray);
    static native int _getGameAction(int aToolkit, int aKeyCode);
    private native int _getKeyCode(int aToolkit,int aGameAction);
    private native String _getKeyName(int aToolkit,int aKeyCode);
    private native int _getCanvasSize(int aToolkit, int aCanvas);
    private native void _suppressKeyEvents(int aToolkit, int aCanvas);
    private native int  _getKeyStates(int aToolkit, int aCanvas);
    private native int _setFullScreenMode(int aDisplayable,int aToolkit,boolean aFullScreen, int[] aSizeReturn);
    private native boolean _getNetworkIndicatorLocation(int aToolkit, int aCanvas, int position[], int size[]);
}
