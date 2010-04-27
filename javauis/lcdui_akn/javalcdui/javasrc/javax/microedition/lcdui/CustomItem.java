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

public abstract class CustomItem extends Item
{
    protected static final int NONE = 0;
    protected static final int TRAVERSE_HORIZONTAL= 1;
    protected static final int TRAVERSE_VERTICAL = 2;
    protected static final int KEY_PRESS = 4;
    protected static final int KEY_RELEASE = 8;
    protected static final int KEY_REPEAT = 0x10;
    protected static final int POINTER_PRESS = 0x20;
    protected static final int POINTER_RELEASE = 0x40;
    protected static final int POINTER_DRAG = 0x80;
    //
    // Event type : must keep in sync with TEventType in lcdui.h
    // Notice the '....ED' at the end of each name which distinguishes them from
    // the interaction mode bits above
    //

    private static final int MAX_WIDTH  = 640;
    private static final int MAX_HEIGHT = 320;
    private static final int DEFAULT_WIDTH  = 200;
    private static final int DEFAULT_HEIGHT = 200;


    //
    // Graphics
    //
    private Graphics iGraphics;

    //
    // Repainting
    //
    private boolean  iRepaintPending; //waiting for paint to be called
    private int      iRepaintX1;
    private int      iRepaintY1;
    private int      iRepaintX2;
    private int      iRepaintY2;
    private boolean  iPaintEventPending;

    //
    private boolean iShown;
    //
    private int     iWidth;
    private int     iHeight;

    //
    // Traversal
    //
    private boolean iFocusEntered;
    private boolean iSupportsInternalTraversal;
    //

    protected CustomItem(String aLabel)
    {
        super();
        synchronized (iToolkit)
        {
            iHandle = Toolkit.checkHandle(_create(iToolkit.getHandle(),this,aLabel));
        }
        iLabel = aLabel;
    }

    public int getGameAction(int aKeyCode)
    {
        if (aKeyCode == 0)
        {
            throw new IllegalArgumentException();
        }
        synchronized (iToolkit)
        {
            return NativeError.check(Canvas._getGameAction(iToolkit.getHandle(),aKeyCode));
        }
    }

    protected final int getInteractionModes()
    {
        synchronized (iToolkit)
        {
            return NativeError.check(_getInteractionModes(iToolkit.getHandle(), getHandle()));
        }
    }

    protected final void invalidate()
    {
        int min_width;
        int min_height;
        int pref_width;
        int pref_height;

        synchronized (iCallbackLock)
        {
            min_width   = getMinContentWidth();
            min_height  = getMinContentHeight();
            pref_width  = getPrefContentWidth(-1);
            pref_height = getPrefContentHeight(-1);
        }
        synchronized (iToolkit)
        {
            _invalidate(iToolkit.getHandle(), getHandle(), min_width, min_height, pref_width, pref_height);
        }
        repaint();
    }

    /**
     * !!!CALLBACK!!!
     */
    protected boolean traverse(int aDirection,int aViewportWidth,int aViewportHeight,int[] aVisRect_inout)
    {
        return false;
    }

    /**
     * !!!CALLBACK!!!
     */
    protected void traverseOut()
    {
    }

    /**
     * !!!CALLBACK!!!
     */
    protected abstract int getMinContentWidth();

    /**
     * !!!CALLBACK!!!
     */
    protected abstract int getMinContentHeight();

    /**
     * !!!CALLBACK!!!
     */
    protected abstract int getPrefContentWidth(int aHeight);

    /**
     * !!!CALLBACK!!!
     */
    protected abstract int getPrefContentHeight(int aWidth);

    /**
     * !!!CALLBACK!!!
     */
    protected abstract void paint(Graphics aGraphics,int aWidth,int aHeight);

    protected final void repaint()
    {
        repaint(0,0, iWidth, iHeight);
    }

    /**
     * Merge clipping rects if already set
     */
    protected final void repaint(int aX,int aY,int aWidth,int aHeight)
    {
        synchronized (iToolkit)
        {
            if (iShown && (aWidth > 0) && (aHeight > 0))
            {
                int x1 = aX;
                int y1 = aY;
                int x2 = aX+aWidth;
                int y2 = aY+aHeight;

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
                iRepaintX2 = Math.min(iRepaintX2, iWidth);
                iRepaintY2 = Math.min(iRepaintY2, iHeight);

                if (!iPaintEventPending)
                {
                    iToolkit.postEvent(this, Toolkit.EVENT_PAINT);
                    iPaintEventPending = true;
                }
            }
        }
    }

    protected void keyPressed(int aKeyCode)
    {
    }

    protected void keyReleased(int aKeyCode)
    {
    }

    protected void keyRepeated(int aKeyCode)
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

    protected void showNotify()
    {
    }

    protected void hideNotify()
    {
    }

    protected void sizeChanged(int aWidth,int aHeight)
    {
    }

    final void handleEvent(int aEvent,int aData0,int aData1,int aData2)
    {
        switch (aEvent)
        {
        case Toolkit.EVENT_PAINT:
            paint();
            break;
        case Toolkit.EVENT_KEY_PRESSED:
            synchronized (iCallbackLock)
            {
                keyPressed(aData0);
            }
            break;
        case Toolkit.EVENT_KEY_RELEASED:
            synchronized (iCallbackLock)
            {
                keyReleased(aData0);
            }
            break;
        case Toolkit.EVENT_KEY_REPEATED:
            synchronized (iCallbackLock)
            {
                keyRepeated(aData0);
            }
            break;
        case Toolkit.EVENT_POINTER_PRESSED:
            synchronized (iCallbackLock)
            {
                pointerPressed(aData0, aData1);
            }
            break;
        case Toolkit.EVENT_POINTER_RELEASED:
            synchronized (iCallbackLock)
            {
                pointerReleased(aData0, aData1);
            }
            break;
        case Toolkit.EVENT_POINTER_DRAGGED:
            synchronized (iCallbackLock)
            {
                pointerDragged(aData0, aData1);
            }
            break;
        case Toolkit.EVENT_VISIBLE:
            handleVisible(aData0>0);
            break;
        case Toolkit.EVENT_SIZE_CHANGED:
            handleSizeChanged(aData0, aData1);
            break;
        case Toolkit.EVENT_TRAVERSE:
        {
            final boolean focus = (aData0 >> 16) > 0;
            final int direction = (aData0 & 0xffff);
            final int visX = (aData1>>16);
            final int visY = (aData1&0xffff);
            final int visW = (aData2>>16);
            final int visH = (aData2&0xffff);
            handleFocusChange(focus, direction, visX, visY, visW, visH);
        }
        break;
        default:
            super.handleEvent(aEvent, aData0, aData1, aData2);
        }
    }

    /**
    aData represents        (focus | direction)
    aData1 represents   (X of the visRect_Out Rect | Y of the visRect_Out Rect)
    aData2 represents   (width of the visRect_Out Rect | height of the visRect_Out Rect)
    */
    final void handleFocusChange(boolean aFocus, int aDirection, int aVisX, int aVisY, int aVisW, int aVisH)
    {
        if (aFocus)
        {
            final Form form = getForm();
            if (form!=null)
            {
                final int viewportWidth  = form.getWidth();
                final int viewportHeight = form.getHeight();

                final int visRect_Out[] = new int[4];

                //
                // The visRect is supplied by subclass. If the subclass supports internal
                // traversal and wants to keep focus, visRect represents the rectangle
                // that should become visible, i.e. the focused area.
                //
                visRect_Out[0] = aVisX;
                visRect_Out[1] = aVisY;
                visRect_Out[2] = aVisW;
                visRect_Out[3] = aVisH;

                // Ask subclass if it wants to keep focus
                final boolean keepFocus;

                synchronized (iCallbackLock)
                {
                    keepFocus = traverse(aDirection,viewportWidth,viewportHeight,visRect_Out);
                }

                // If the subclass wants focus, maintain the visRect
                final int[] scroll = keepFocus ? visRect_Out : null;

                synchronized (iToolkit)
                {
                    FirstTimeEntered(keepFocus);
                    if (!iSupportsInternalTraversal)
                    {
                        _setFocusAndScroll(iToolkit.getHandle(), getHandle(), true, null, aDirection);
                    }
                    else
                    {
                        _setFocusAndScroll(iToolkit.getHandle(), getHandle(), keepFocus, scroll, aDirection);
                        repaint();
                    }
                }
            }
        }
        else
        {
            // The native object has lost focus therfore we must
            // call traverseOut.
            synchronized (iCallbackLock)
            {
                traverseOut();
            }
        }
    }


    /**
    The first time focus enters the CustomItem subclass is special in that it
    signifies whether this CustomItem suports internal traversal.
    */
    final void FirstTimeEntered(boolean aKeepFocus)
    {
        if (!iFocusEntered)
        {
            iFocusEntered = true;
            iSupportsInternalTraversal = aKeepFocus;

            if (!iSupportsInternalTraversal)
            {
                // If internal trversal is not supported we must let the native code know
                // that is the case. Addtional call (below) is necessary to actually transer
                // focus to this item.
                _setFocusAndScroll(iToolkit.getHandle(), getHandle(),false, null, NONE);
            }
        }
    }

    final void handleVisible(boolean aVisible)
    {
        final boolean changed;

        synchronized (iToolkit)
        {
            changed = (iShown != aVisible);
            iShown = aVisible;
        }

        if (changed)
        {
            if (aVisible)
            {
                synchronized (iCallbackLock)
                {
                    showNotify();
                }
                repaint();
            }
            else
            {
                synchronized (iCallbackLock)
                {
                    hideNotify();
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
        Graphics graphics;

        synchronized (iToolkit)
        {
            iPaintEventPending = false;

            if (!iRepaintPending)
            {
                return; // Don't draw if all the repaints have already been done
            }

            iRepaintPending = false;

            if (!iShown)
            {
                return;
            }

            rx = iRepaintX1;
            ry = iRepaintY1;
            rw = iRepaintX2 - iRepaintX1;
            rh = iRepaintY2 - iRepaintY1;

            graphics = GetGraphics();
            width    = iWidth;
            height   = iHeight;
        }

        clipped = (rw < width) || (rh < height);
        if (clipped)
        {
            graphics.setClip(rx,ry,rw,rh);
        }

        synchronized (iCallbackLock)
        {
            paint(graphics, width, height);
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
            ReleaseGraphics(graphics);
        }
    }

    private void handleSizeChanged(int aWidth,int aHeight)
    {
        boolean  changed;

        synchronized (iToolkit)
        {
            changed = (iWidth != aWidth) || (iHeight != aHeight);
            if (changed)
            {
                iWidth  = aWidth;
                iHeight = aHeight;
                DisposeGraphics();
            }
        }

        if (changed)
        {
            synchronized (iCallbackLock)
            {
                sizeChanged(aWidth,aHeight);
            }
            repaint();
        }
    }

    void addNotify(Screen aScreen)
    {
        super.addNotify(aScreen);
        invalidate();
        synchronized (iToolkit)
        {
            DisposeGraphics();
        }
    }

    void removeNotify(Screen aScreen)
    {
        super.removeNotify(aScreen);
        synchronized (iToolkit)
        {
            DisposeGraphics();
        }
    }

    private Graphics GetGraphics()
    {
        Graphics graphics = iGraphics;
        iGraphics = null;
        if (graphics == null)
        {
            graphics = new Graphics(iToolkit, this, getHandle(), iWidth, iHeight);
        }
        else
        {
            graphics.reset(iWidth, iHeight);
        }
        return graphics;
    }

    private void ReleaseGraphics(Graphics aGraphics)
    {
        if ((null == iGraphics) && (iWidth==aGraphics.iWidth) && (iHeight==aGraphics.iHeight))
        {
            iGraphics=aGraphics;
        }
        else
        {
            aGraphics.dispose();
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

    private native int _create(int aToolkit,CustomItem aCustomItem,String aLabel);
    private native int  _getInteractionModes(int aToolkit, int aCustomItem);
    private native void _invalidate(int aToolkit, int aCustomItem,int aMinWidth,int aMinHeight,int aPrefWidth,int aPrefHeight);
    private native void _setFocusAndScroll(int aToolkit, int aCustomItem, boolean aFocus, int[] aScroll, int aDirection);
}
