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

import com.nokia.mj.impl.rt.legacy.NativeError;
import com.nokia.mid.ui.CanvasGraphicsItem;
import com.nokia.mj.impl.rt.support.Finalizer;

class CanvasGraphicsItemPainter
        extends com.nokia.mid.ui.CanvasGraphicsItemPainter
{
    // Toolkit
    private Toolkit iToolkit;

    // Native handle
    private int iHandle;

    // Graphics
    private Graphics iGraphics;

    //Object finalizer
    private Finalizer mFinalizer;

    //
    // Repainting
    //
    private boolean  iRepaintPending; //waiting for paint to be called
    private int      iRepaintX1;
    private int      iRepaintY1;
    private int      iRepaintX2;
    private int      iRepaintY2;
    private boolean  iPaintEventPending;

    Object iCallbackLock;

    CanvasGraphicsItemPainter(
        CanvasGraphicsItem aItem,
        Object aToolkit,
        int aWidth,
        int aHeight)
    {
        iItem = aItem;
        iToolkit = (Toolkit)aToolkit;

        synchronized (iToolkit)
        {
            iHandle = Toolkit.checkHandle(
                          _createNativePeer(iToolkit.getHandle(), aWidth, aHeight));

            iCallbackLock = iToolkit.getCallbackLock();
        }

        iGraphics = null;

        mFinalizer = new Finalizer()
        {
            public void finalizeImpl()
            {
                registeredFinalize();
            }
        };
    }

    /**
     * Hidden default constructor.
     */
    protected CanvasGraphicsItemPainter()
    {
    }

    protected void Repaint(int aX, int aY, int aWidth, int aHeight)
    {

        synchronized (iToolkit)
        {
            if (iItem.isVisible() && (aWidth > 0) && (aHeight > 0))
            {
                int x1 = aX;
                int y1 = aY;
                int x2 = aX + aWidth;
                int y2 = aY + aHeight;

                // Union of clip rect and damage rect
                if (iRepaintPending)
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
                    iRepaintPending = true;
                }

                // Clip to bounds
                iRepaintX1 = Math.max(iRepaintX1, 0);
                iRepaintY1 = Math.max(iRepaintY1, 0);
                iRepaintX2 = Math.min(iRepaintX2, iItem.getWidth());
                iRepaintY2 = Math.min(iRepaintY2, iItem.getHeight());

                if (!iPaintEventPending)
                {
                    iToolkit.postEvent(this, Toolkit.EVENT_PAINT);
                    iPaintEventPending = true;
                }
            }
        }
    }

    private void paint()
    {
        boolean  clipped;
        int      rx;
        int      ry;
        int      rw;
        int      rh;
        int      width;
        int      height;

        synchronized (iToolkit)
        {
            iPaintEventPending = false;

            if (!iRepaintPending)
            {
                return; // Don't draw if all the repaints have already been done
            }

            iRepaintPending = false;

            if (!iItem.isVisible())
            {
                return;
            }

            rx = iRepaintX1;
            ry = iRepaintY1;
            rw = iRepaintX2 - iRepaintX1;
            rh = iRepaintY2 - iRepaintY1;

            width    = iItem.getWidth();
            height   = iItem.getHeight();

            if (iGraphics == null)
            {
                iGraphics = new Graphics(
                    iToolkit, this, iHandle,
                    iItem.getWidth(), iItem.getHeight());
            }

            iGraphics.reset(width, height);
        }

        clipped = (rw < width) || (rh < height);
        if (clipped)
        {
            iGraphics.setClip(rx, ry, rw, rh);
        }

        _resetFrameBuffer(iToolkit.getHandle(), iHandle,
                          rx, ry, rw, rh);

        synchronized (iCallbackLock)
        {
            Repaint(iGraphics); //this will invoke paint(Graphics) in MIDlet
        }

        synchronized (iToolkit)
        {
            if (clipped)
            {
                iToolkit.sync(getHandle(), rx, ry, rw, rh);
            }
            else
            {
                iToolkit.sync(getHandle());
            }
        }
    }

    public int getHandle()
    {
        if (iHandle <= 0)
        {
            throw new RuntimeException(
                "CanvasGraphicsItemPainter.getHandle(" +
                this +
                ") dead item");
        }

        return iHandle;
    }

    private void handleSizeChanged(int aWidth, int aHeight)
    {
        // We don't need to check here if size was really changed
        // because it is done on native side and event is sent
        // only if size was really changed there
        synchronized (iToolkit)
        {
            DisposeGraphics();
        }
        Repaint(0, 0, iItem.getWidth(), iItem.getHeight());
    }

    /*
     * Method handles callback events from native side.
     * aEvent - id of event, only EVENT_PAINT and EVENT_SIZE_CHANGED are used.
     */
    final void handleEvent(int aEvent, int aData0, int aData1)
    {
        switch (aEvent)
        {
        case Toolkit.EVENT_PAINT:
            paint();
            break;

        case Toolkit.EVENT_SIZE_CHANGED:
            handleSizeChanged(aData0, aData1);
            break;

        case Toolkit.EVET_CANVAS_GRAPHICS_ITEM_REPAINT:
            Repaint(0, 0, iItem.getWidth(), iItem.getHeight());
            break;
        }
    }

    private void DisposeGraphics()
    {
        final Graphics graphics = iGraphics;
        iGraphics = null;
        if (null != graphics)
        {
            graphics.dispose();
        }
    }

    /*
     * Disposes the Landmark native peer object, if the handles are valid.
     * Invalid (negative) handles indicate that their creation failed in the
     * first place.
     */
    final void registeredFinalize()
    {
        synchronized (iToolkit)
        {
            if (iHandle > 0)
            {
                _dispose(iToolkit.getHandle(), iHandle);
                iHandle = 0;
            }
        }
    }

    // Native methods

    /*
     * Disposes the native side peer object.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     */
    private native void _dispose(int aToolkitHandle, int aNativePeerHandle);

    private native int _createNativePeer(
        int aToolkit,
        int aWidth,
        int aHeight);

    private native int _resetFrameBuffer(
        int aToolkit,
        int aNativePeerHandle,
        int aPositionX,
        int aPositionY,
        int aWidth,
        int aHeight);

}