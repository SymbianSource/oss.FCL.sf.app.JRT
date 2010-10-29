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
package org.eclipse.swt.widgets;


import org.eclipse.swt.internal.*;
import org.eclipse.swt.widgets.Control;
import javax.microedition.m2g.*;
import com.nokia.microedition.m2g.*;
import java.util.*;
import java.lang.ref.WeakReference;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.events.*;
import com.nokia.mj.impl.utils.Logger;
import org.eclipse.swt.widgets.Internal_PackageSupport;
import org.eclipse.swt.graphics.Internal_GfxPackageSupport;



/**
 * Control
 */
public class SVGAnimatorControl extends Canvas
        implements ControlListener,
        KeyListener,
        MouseListener,
        DisposeListener,
        ShellListener,
        PaintListener
{
    //--------------------------------------------------
    // STATIC CONSTANTS
    //--------------------------------------------------
    public static final int STATE_STOPPED = 1;
    public static final int STATE_PLAYING = 2;
    public static final int STATE_PAUSED  = 3;
    public static final float DEFAULT_DELTA_TIME = 0.1f;

    //--------------------------------------------------
    // VARIABLES
    //--------------------------------------------------
    private int iState;
    private SVGImage iSVGImage;
    private M2GScalableGraphics iSg;
    private float iDeltaTime;
    private SVGAnimatorRunnable iTask = null;
    private SVGEventListener iEventListener = null;

    //--------------------------------------------------
    // METHODS
    //--------------------------------------------------

    /**
     * @see
     */
    public SVGAnimatorControl(SVGImage aSVGImage)
    {
        iSVGImage = aSVGImage;
        iState = STATE_STOPPED;
        // Create render context, use M2GScalableGraphics
        // directly to get access to render method with native
        // side clearing parameter
        iSg = new M2GScalableGraphics();
        iSg.setRenderingQuality(ScalableGraphics.RENDERING_QUALITY_HIGH);
        iDeltaTime = DEFAULT_DELTA_TIME;
        iTask = new SVGAnimatorRunnable(this);
    }

    /**
     * @see org.eclipse.swt.widgets.Control#setParent()
     */
    public boolean setParent(Composite aParent)
    {
        parent = aParent;
        display = aParent.display;

        // Init widget after parent and display are known
        //Have to create Widget.

        // Add this to necessary listeners
        addControlListener(this);
        addKeyListener(this);
        addMouseListener(this);
        addDisposeListener(this);
        parent.getShell().addShellListener(this);
        addPaintListener(this);

        return true;
    }

    /**
     * @see org.eclipse.swt.widgets.Control#paint()
     */
    public void paintControl(PaintEvent e)
    {
        GC gc = e.gc;
        // Render image
        try
        {
            iSg.bindTarget(gc);
            // Parameter true to use native side clearing
            // -> animator background is always white
            iSg.render(0, 0, iSVGImage, true);
        }
        finally
        {
            iSg.releaseTarget();
        }
    }

    /**
     * Returns event listener
     * @return event listener
     */
    public synchronized SVGEventListener getEventListener()
    {
        return iEventListener;
    }

    /**
     * @see javax.microedition.m2g.SVGAnimator#getTimeIncrement()
     */
    public synchronized float getTimeIncrement()
    {
        return iDeltaTime;
    }

    /**
     * Increases the increment time of the SVGImage.
     * Increment is done only if the playing state is active.
     * @see javax.microedition.m2g.SVGImage#incrementTime()
     */
    public synchronized void increaseCurrentTime()
    {
        if (iState == STATE_PLAYING)
        {
            iSVGImage.incrementTime(iDeltaTime);
        }
    }

    /**
     * Checks if playing
     * @return true if playing
     */
    public synchronized boolean isPlaying()
    {
        return iState == STATE_PLAYING;
    }

    /**
     * Checks if paused
     * @return true if paused
     */
    public synchronized boolean isPaused()
    {
        return iState == STATE_PAUSED;
    }

    /**
     * Checks if stopped
     * @return true if stopped
     */
    public synchronized boolean isStopped()
    {
        return iState == STATE_STOPPED;
    }

    /**
     * @see org.eclipse.swt.events.DisposeListener#widgetDisposed()
     * @see javax.microedition.m2g.SVGEventListener#hideNotify()
     */
    public synchronized void widgetDisposed(DisposeEvent e)
    {
        if (iEventListener != null)
        {
            iEventListener.hideNotify();
        }
    }

    /**
     * @see org.eclipse.swt.events.ShellListener#shellActivated()
     * @see javax.microedition.m2g.SVGEventListener#showNotify()
     */
    public synchronized void shellActivated(ShellEvent e)
    {
        if (iEventListener != null)
        {
            iEventListener.showNotify();
        }
    }

    /**
     * @see org.eclipse.swt.events.ShellListener#shellClosed()
     * @see javax.microedition.m2g.SVGEventListener#hideNotify()
     */
    public synchronized void shellClosed(ShellEvent e)
    {
        if (iEventListener != null)
        {
            iEventListener.hideNotify();
        }
    }

    /**
     * @see org.eclipse.swt.events.ShellListener#shellDeactivated()
     * @see javax.microedition.m2g.SVGEventListener#hideNotify()
     */
    public synchronized void shellDeactivated(ShellEvent e)
    {
        if (iEventListener != null)
        {
            iEventListener.hideNotify();
        }
    }

    /**
     * @see org.eclipse.swt.events.ShellListener#shellDeiconified()
     * @see javax.microedition.m2g.SVGEventListener#hideNotify()
     */
    public synchronized void shellDeiconified(ShellEvent e)
    {
        if (iEventListener != null)
        {
            iEventListener.hideNotify();
        }
    }

    /**
     * @see org.eclipse.swt.events.ShellListener#shellIconified()
     * @see javax.microedition.m2g.SVGEventListener#showNotify()
     */
    public synchronized void shellIconified(ShellEvent e)
    {
        if (iEventListener != null)
        {
            iEventListener.showNotify();
        }
    }

    /**
     * @see org.eclipse.swt.events.KeyListener#keyPressed()
     * @see javax.microedition.m2g.SVGEventListener#keyPressed()
     */
    public synchronized void keyPressed(KeyEvent e)
    {
        if (iEventListener != null)
        {
            iEventListener.keyPressed(e.keyCode);
        }
    }

    /**
     * @see org.eclipse.swt.events.KeyListener#keyReleased()
     * @see javax.microedition.m2g.SVGEventListener#keyPReleased()
     */
    public synchronized void keyReleased(KeyEvent e)
    {
        if (iEventListener != null)
        {
            iEventListener.keyReleased(e.keyCode);
        }
    }

    /**
    * @see org.eclipse.swt.events.MouseListener#mouseDown()
    * @see javax.microedition.m2g.SVGEventListener#pointerPressed()
    */
    public synchronized void mouseDown(MouseEvent e)
    {
        if (iEventListener != null)
        {
            iEventListener.pointerPressed(e.x, e.y);
        }
    }

    /**
     * @see org.eclipse.swt.events.MouseListener#mouseUp()
     * @see javax.microedition.m2g.SVGEventListener#pointerReleased()
     */
    public synchronized void mouseUp(MouseEvent e)
    {
        if (iEventListener != null)
        {
            iEventListener.pointerReleased(e.x, e.y);
        }
    }

    /**
     * @see org.eclipse.swt.events.MouseListener#mouseDoubleClick()
     */
    public synchronized void mouseDoubleClick(MouseEvent e)
    {
        // No implementation needed
    }

    /**
     * @see org.eclipse.swt.events.ControlListener#controlResized()
     * @see javax.microedition.m2g.SVGEventListener#sizeChanged()
     */
    public synchronized void controlResized(ControlEvent e)
    {
        Rectangle bounds = getBounds();
        if (iEventListener != null)
        {
            iEventListener.sizeChanged(bounds.width, bounds.height);
        }
    }

    /**
     * @see org.eclipse.swt.events.ControlListener#controlMoved()
     */
    public synchronized void controlMoved(ControlEvent e)
    {
        // No implementation needed
    }

    /**
     * @see javax.microedition.m2g.SVGAnimator#pause()
     */
    public synchronized void pause()
    {
        iState = STATE_PAUSED;
    }

    /**
     * @see javax.microedition.m2g.SVGAnimator#play()
     */
    public synchronized void play()
    {
        if (iState == STATE_STOPPED)
        {
            // Set runnable to be run in UI thread
            display.asyncExec(iTask);
        }
        iState = STATE_PLAYING;
    }

    /**
     * @see javax.microedition.m2g.SVGAnimator#setSVGEventListener()
     */
    public synchronized void setEventListener(SVGEventListener eventListener)
    {
        iEventListener = eventListener;
    }

    /**
     * @see javax.microedition.m2g.SVGAnimator#setTimeIncrement()
     */
    public synchronized void setTimeIncrement(float aDeltaTime)
    {
        iDeltaTime = aDeltaTime;
    }

    /**
     * @see javax.microedition.m2g.SVGAnimator#stop()
     */
    public synchronized void stop()
    {
        iState = STATE_STOPPED;
    }
}

/**
 * SVGAnimatorRunnable
 * Runnable class to perform control redrawing.
 */
class SVGAnimatorRunnable implements Runnable
{
    //--------------------------------------------------
    // VARIABLES
    //--------------------------------------------------

    private WeakReference iWeakControl;

    //--------------------------------------------------
    // METHODS
    //--------------------------------------------------

    public SVGAnimatorRunnable(SVGAnimatorControl aControl)
    {
        iWeakControl = new WeakReference(aControl);
    }
    /*
     * @see java.lang.Runnable#run()
     */
    public void run()
    {
        SVGAnimatorControl control = (SVGAnimatorControl)iWeakControl.get();
        if (control != null && !control.isDisposed())
        {
            try
            {
                synchronized (control)
                {
                    if (control.isPlaying())
                    {
                        // Playing: increase time
                        control.increaseCurrentTime();
                    }
                    if (!control.isStopped())
                    {
                        // Playing or paused: request repainting and new run
                        // If animator has been stopped no new run is requested
                        control.redraw();
                        control.getDisplay().timerExec(
                            (int)(control.getTimeIncrement() * 1000),
                            this);
                    }
                }
            }
            catch (Exception e)
            {
                Logger.ELOG(Logger.EJavaUI, "SVGAnimatorRunnable: run() - exception: "
                            + e.toString());
            }
        }
    }
}
