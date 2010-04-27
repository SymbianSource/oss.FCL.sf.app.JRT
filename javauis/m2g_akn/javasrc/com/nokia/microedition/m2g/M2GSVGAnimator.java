/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.microedition.m2g;

import javax.microedition.m2g.*;
import java.util.*;
import javax.microedition.lcdui.*;
import javax.microedition.lcdui.game.GameCanvas;
import java.lang.ref.WeakReference;
import com.nokia.mj.impl.rt.support.Finalizer;
import com.nokia.mj.impl.utils.Logger;


public class M2GSVGAnimator extends SVGAnimator implements M2GDestroyable
{
    //--------------------------------------------------
    // STATIC CONSTANTS
    //--------------------------------------------------
    private static final String ANIMATOR_CANVAS_BASE_CLASS =
        "javax.microedition.lcdui.Canvas";
    // Exception text
    /* Optimization: static finals changed to local variables
    private static final String COMPONENT_BASE_CLASS_NOT_SUPPORTED_ESTR =
    "The requested componentBaseClass is not supported by the implementation.";
    private static final String ILLEGAL_TIME_INCREMENT_ESTR =
    "The timeIncrement is less than or equal to zero.";
    private static final String ANIMATOR_PLAY_ESTR =
    "The animator is not currently in the stopped or paused state.";
    private static final String ANIMATOR_PAUSE_ESTR =
    "The animator is not in the playing  state.";
    private static final String ANIMATOR_STOP_ESTR =
    "The animator is not in the playing or paused state.";
    private static final String INVALID_RUNNABLE_ESTR =
    "The runnable is null.";
    private static final String ANIMATOR_IS_STOPPED_ESTR =
    "The animator is in the stopped state.";
    private static final String RUNNABLE_IS_NULL_ESTR =
    "The runnable is null.";
    private static final String ANIMATOR_INVOKE_ESTR =
    "The animator is in the stopped state.";
    */

    //--------------------------------------------------
    // VARIABLES
    //--------------------------------------------------
    private M2GSVGCanvas    iSVGCanvas      = null;
    private M2GDestroyer    iDestroyer      = null;
    private Finalizer mFinalizer;

    //--------------------------------------------------
    // METHODS
    //--------------------------------------------------
    /**
     * Constructor
     * @param aImage
     */
    protected M2GSVGAnimator(SVGImage aImage)
    {
        iSVGCanvas = new M2GSVGCanvas(false, aImage);
        mFinalizer = new Finalizer()
        {
            public void finalizeImpl()
            {
                doFinalize();
            }
        };
        iDestroyer = new M2GDestroyer(this);
    }

    /**
     * @see M2GDestroyable#doDestroy()
     */
    public void doDestroy()
    {
        registeredFinalize();
    }

    /**
     * @see javax.microedition.m2g.SVGAnimator#getTargetComponent()
     */
    public Object getTargetComponent()
    {
        return iSVGCanvas;
    }

    /**
    * @see javax.microedition.m2g.SVGAnimator#getTimeIncrement()
    */
    public float getTimeIncrement()
    {
        return iSVGCanvas.getTimeIncrement();
    }

    /**
    * @see javax.microedition.m2g.SVGAnimator#invokeAndWait()
    */
    public void invokeAndWait(java.lang.Runnable runnable)
    {
        if (runnable == null)
        {
            throw new NullPointerException(
                /*SF*/"The runnable is null."/*SF*/);
        }
        if (iSVGCanvas.isStopped())
        {
            throw new IllegalStateException(
                /*SF*/"The animator is in the stopped state."/*SF*/);
        }
        Logger.LOG(Logger.EJavaUI, Logger.EInfo, "invokeAndWait()");
        runnable.run();
    }

    /**
     * @see javax.microedition.m2g.SVGAnimator#invokeLater()
     */
    public void invokeLater(java.lang.Runnable runnable)
    {
        if (runnable == null)
        {
            throw new NullPointerException(
                /*SF*/"The runnable is null."/*SF*/);
        }
        if (iSVGCanvas.isStopped())
        {
            throw new IllegalStateException(
                /*SF*/"The animator is in the stopped state."/*SF*/);
        }
        Logger.LOG(Logger.EJavaUI, Logger.EInfo, "invokeLater()");
        Thread thread = new Thread(runnable);
        thread.start();
    }

    /**
     * @see javax.microedition.m2g.SVGAnimator#pause()
     */
    public void pause()
    {
        if (!iSVGCanvas.isPlaying())
        {
            throw new IllegalStateException(
                /*SF*/"The animator is not in the playing or paused state."/*SF*/);
        }
        Logger.LOG(Logger.EJavaUI, Logger.EInfo, "pause()");
        iSVGCanvas.pause();
    }

    /**
    * @see javax.microedition.m2g.SVGAnimator#play()
    */
    public void play()
    {
        if (iSVGCanvas.isPlaying())
        {
            throw new IllegalStateException(
                /*SF*/"The animator is not currently in the stopped or paused state."/*SF*/);
        }
        Logger.LOG(Logger.EJavaUI, Logger.EInfo, "play()");
        iSVGCanvas.play();
    }

    private void doFinalize()
    {
        if (mFinalizer != null)
        {
            registeredFinalize();
            mFinalizer = null;
        }
    }

    /**
     * Finalize
     */
    synchronized void registeredFinalize()
    {
        iSVGCanvas.cancel();
        iSVGCanvas = null;
    }


    /**
     * @see javax.microedition.m2g.SVGAnimator#setSVGEventListener()
     */
    public void setSVGEventListener(SVGEventListener svgEventListener)
    {
        iSVGCanvas.setEventListener(svgEventListener);
    }

    /**
     * @see javax.microedition.m2g.SVGAnimator#setTimeIncrement()
     */
    public void setTimeIncrement(float timeIncrement)
    {
        if (timeIncrement <= 0)
        {
            throw new IllegalArgumentException(
                /*SF*/"The time increment is less than or equal to zero."/*SF*/);
        }
        Logger.LOG(Logger.EJavaUI, Logger.EInfo, "setTimeIncrement() - "
                   + timeIncrement);
        iSVGCanvas.setTimeIncrement(timeIncrement);
    }

    /**
     * @see javax.microedition.m2g.SVGAnimator#stop()
     */
    public void stop()
    {
        if (iSVGCanvas.isStopped())
        {
            throw new IllegalStateException(
                /*SF*/"The animator is not in the playing or paused state."/*SF*/);
        }
        Logger.LOG(Logger.EJavaUI, Logger.EInfo, "stop()");

        iSVGCanvas.stop();
    }

    //--------------------------------------------------
    // STATIC METHODS
    //--------------------------------------------------
    /**
     * Builds animator
     * @param svgImage -
     * @return SVGAnimator
     * @see javax.microedition.m2g.SVGAnimator#createAnimator()
     */
    public static SVGAnimator buildAnimator(SVGImage svgImage)
    {
        if (svgImage == null)
        {
            throw new NullPointerException();
        }
        return new M2GSVGAnimator(svgImage);
    }

    /**
     * Builds animator
     * @param svgImage -
     * @param componentBaseClass -
     * @return SVGAnimator
     * @see javax.microedition.m2g.SVGAnimator#createAnimator()
     */
    public static SVGAnimator buildAnimator(
        SVGImage svgImage, String componentBaseClass)
    {
        if (svgImage == null)
        {
            throw new NullPointerException();
        }
        if ((componentBaseClass != null) &&
                (!componentBaseClass.equals(ANIMATOR_CANVAS_BASE_CLASS)))
        {
            throw new IllegalArgumentException(
                /*SF*/"The requested componentBaseClass is not supported by the implementation."/*SF*/);
        }
        return buildAnimator(svgImage);
    }
}

//--------------------------------------------------
// OTHER CLASSES
//--------------------------------------------------

/**
 * Canvas
 */
class M2GSVGCanvas extends GameCanvas implements M2GDOMChangeObserver
{
    //--------------------------------------------------
    // STATIC CONSTANTS
    //--------------------------------------------------
    public static final int STATE_STOPPED = 1;
    public static final int STATE_PLAYING = 2;
    public static final int STATE_PAUSED  = 3;
    public static final float DEFAULT_DELTA_TIME = 0.1f;  // (10fps) - defined by specs

    //--------------------------------------------------
    // VARIABLES
    //--------------------------------------------------
    private int                 iState;
    private SVGImage            iSVGImage;
    private ScalableGraphics    iSg;
    private Graphics            iOffscreen      = null;
    private M2GSVGSVGElement    iRootElement    = null;

    private float               iDeltaTime;
    private Timer               iTimer          = null;
    private SVGCanvasTask       iTask           = null;

    private SVGEventListener    iEventListener  = null;

    /**
     * True if the GameCanvas is in background or false otherwise
     */
    private boolean iWasPlaying                 = false;


    //--------------------------------------------------
    // METHODS
    //--------------------------------------------------
    /**
     * @see javax.microedition.lcdui.game.GameCanvas#GameCanvas()
     */
    public M2GSVGCanvas(boolean aSuppressKeyEvents, SVGImage aSVGImage)
    {
        super(aSuppressKeyEvents);

        // get the instance to the Graphics of the offscreen buffer
        iOffscreen = getGraphics();

        iSVGImage     = aSVGImage;

        // down-casting to M2GDocument/M2GSVGSVGElement to have access to internal methods
        M2GDocument doc = (M2GDocument)iSVGImage.getDocument();
        iRootElement  = (M2GSVGSVGElement)iSVGImage.getDocument().getDocumentElement();

        iState = STATE_STOPPED;
        // Create render context
        iSg = ScalableGraphics.createInstance();
        iSg.setRenderingQuality(ScalableGraphics.RENDERING_QUALITY_HIGH);
        iDeltaTime = DEFAULT_DELTA_TIME;

        doc.registerDOMChangeObserver(this);
        Logger.LOG(Logger.EJavaUI, Logger.EInfo, "Ctor - delta time:"
                   + iDeltaTime + ", state:" + iState);
    }

    /**
     * Handles any change in DOM.
     * <br>
     * While in PAUSE state, SVGAnimator must repaint any changes in SVGImage
     * done via the API (e.g. setTrait(), insertBefore())
     * <br>
     * @see M2GDOMChangeObserver.notifyDOMChange()
     * @since S60 3.2
     */
    public void notifyDOMChange()
    {
        if (isPaused())
        {
            repaint();
        }
    }

    /**
     * Cancel a timed task
     */
    public void cancel()
    {
        if (iTimer != null)
        {
            iTimer.cancel();
        }
        if (iTask != null)
        {
            iTask.cancel();
        }
        iTask = null;
        iTimer = null;
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
        Logger.LOG(Logger.EJavaUI, Logger.EInfo, "getTimeIncrement() - "
                   + iDeltaTime);
        return iDeltaTime;
    }

    /**
     * Increases the increment time of the SVGImage.
     * Increment is done only if the playing state is active.
     * @see javax.microedition.m2g.SVGImage#incrementTime()
     */
    public synchronized void increaseCurrentTime(float time)
    {
        if (iState == STATE_PLAYING)
        {
            // update the time only in java side
            // the time in engine side is updated during rendering
            iRootElement.incrementTimeNoUpdate(time);
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
     * @see javax.microedition.lcdui.Canvas#keyPressed()
     */
    protected synchronized void keyPressed(int keyCode)
    {
        Logger.LOG(Logger.EJavaUI, Logger.EInfo, "keyPressed() - " + keyCode);
        if (iEventListener != null)
        {
            iEventListener.keyPressed(keyCode);
        }
    }

    /**
     * @see javax.microedition.lcdui.Canvas#keyReleased()
     */
    protected synchronized void keyReleased(int keyCode)
    {
        Logger.LOG(Logger.EJavaUI, Logger.EInfo, "keyReleased() - " + keyCode);
        if (iEventListener != null)
        {
            iEventListener.keyReleased(keyCode);
        }
    }

    /**
     * @see javax.microedition.lcdui.Canvas#pointerPressed()
     * @see javax.microedition.m2g.SVGEventListener#pointerPressed()
     */
    protected synchronized void pointerPressed(int x, int y)
    {
        Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                   "pointerPressed() - x:" + x + ", y:" + y);
        if (iEventListener != null)
        {
            iEventListener.pointerPressed(x, y);
        }
    }

    /**
     * @see javax.microedition.lcdui.Canvas#pointerReleased()
     * @see javax.microedition.m2g.SVGEventListener#pointerReleased()
     */
    protected synchronized void pointerReleased(int x, int y)
    {
        Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                   "pointerReleased() - x:" + x + ", y:" + y);
        if (iEventListener != null)
        {
            iEventListener.pointerReleased(x, y);
        }
    }

    /**
     * @see javax.microedition.lcdui.game.GameCanvas#paint()
     */
    public void paint(Graphics g)
    {
        // Clears bitmap
        g.setColor(255, 255, 255);
        g.fillRect(0, 0, getWidth(), getHeight());

        try
        {
            iSg.bindTarget(g);

            // NOTE: Source is defaultly fully opaque
            iSg.render(0, 0, iSVGImage);
        }
        finally
        {
            iSg.releaseTarget();
        }
    }

    /**
     * Paints a frame to the offscreen of this GameCanvas
     * @note GameCanvas.getGraphics() is not used since it always creates a new instance of Graphics
     */
    public void paintToOffscreen()
    {
        paint(iOffscreen);
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
        Logger.LOG(Logger.EJavaUI, Logger.EInfo, "setTimeIncrement() - "
                   + aDeltaTime);

        iDeltaTime = aDeltaTime;
    }


    /**
     * @see javax.microedition.lcdui.Canvas#showNotify()
     * @see javax.microedition.m2g.SVGEventListener#showNotify()
     */
    protected synchronized void showNotify()
    {
        Logger.LOG(Logger.EJavaUI, Logger.EInfo, "showNotify()");

        if (iEventListener != null)
        {
            iEventListener.showNotify();
        }

        // A common use-case could be that the developer plays the animation by themselves
        // in SVGEventListener.showNotify().
        // Therefore we play the animation only if the developer didn't resume it already.
        if (iWasPlaying)
        {
            iWasPlaying = false;
            play();
        }
    }

    /**
     * @see javax.microedition.lcdui.Canvas#hideNotify()
     * @see javax.microedition.m2g.SVGEventListener#hideNotify()
     */
    protected synchronized void hideNotify()
    {
        Logger.LOG(Logger.EJavaUI, Logger.EInfo, "hideNotify()");

        if (iEventListener != null)
        {
            iEventListener.hideNotify();
        }

        // A common use-case could be that developer pause the animation by themselves
        // in SVGEventListener.hideNotify().
        // Therefore we pause the animation only if the developer didn't pause it already.
        if (isPlaying())
        {
            pause();
            iWasPlaying = true;
        }
    }


    /**
     * @see javax.microedition.lcdui.Canvas#sizeChanged()
     * @see javax.microedition.m2g.SVGEventListener#sizeChanged()
     */
    protected synchronized void sizeChanged(int w, int h)
    {
        Logger.LOG(Logger.EJavaUI, Logger.EInfo, "sizeChanged() - w:"
                   + w + ", h:" + h);

        // get a new instance of offscreen Graphics since the Graphics object
        // does not update its size or clipping area.
        // If not updating this, the GameCanvas is clipped when it is switched
        // to full-screen
        if (iOffscreen != null)
        {
            // sizeChanged() is called twice when the Canvas is set as Displayable.
            // If we try to get the Graphics object (first time), the Graphics object is not
            // fully initialized, so GameCanvas.getGraphics() will throw a NullPointerException
            iOffscreen = getGraphics();
        }

        if (iSVGImage != null)
        {
            iSVGImage.setViewportWidth(w);
            iSVGImage.setViewportHeight(h);
        }

        if (iEventListener != null)
        {
            iEventListener.sizeChanged(w, h);
        }
    }

    /**
     * @see javax.microedition.m2g.SVGAnimator#play()
     */
    public synchronized void play()
    {
        Logger.LOG(Logger.EJavaUI, Logger.EInfo, "play()");

        if (iState == STATE_PLAYING)
        {
            // don't do anything if animation is already playing
            return;
        }

        if ((iState == STATE_PAUSED)||(iState == STATE_STOPPED))
        {
            iTask = new SVGCanvasTask(this);
            iTimer = new Timer();
        }
        iState = STATE_PLAYING;
        if (iTimer != null && iTask != null)
        {

            iTimer.schedule(iTask,0,(long)(iDeltaTime*1000.0f));

        }
    }

    /**
     * @see javax.microedition.m2g.SVGAnimator#pause()
     */
    public synchronized void pause()
    {
        Logger.LOG(Logger.EJavaUI, Logger.EInfo, "pause()");

        if (iState == STATE_PLAYING)
        {
            iTask.cancel();
            iTask = null;
            iTimer.cancel();
            iTimer = null;
        }
        iState = STATE_PAUSED;

    }

    /**
     * @see javax.microedition.m2g.SVGAnimator#stop()
     */
    public synchronized void stop()
    {
        Logger.LOG(Logger.EJavaUI, Logger.EInfo, "stop()");
        if (iState == STATE_PLAYING)
        {
            iTask.cancel();
            iTask = null;
            iTimer.cancel();
            iTimer = null;
        }
        iState = STATE_STOPPED;
    }
}

/**
 * Timer task for rendering frames
 */
class SVGCanvasTask extends TimerTask
{
    //--------------------------------------------------
    // VARIABLES
    //--------------------------------------------------
    // private WeakReference   iWeakCanvas;

    private M2GSVGCanvas    iCanvas                 = null;
    private long           iPrevTime        = 0;

    //--------------------------------------------------
    // METHODS
    //--------------------------------------------------
    /**
     * Constructor
     * @param aCanvas SVG canvas
     */
    public SVGCanvasTask(M2GSVGCanvas aCanvas)
    {
        // iWeakCanvas = new WeakReference(aCanvas);
        // iCanvas = (M2GSVGCanvas)iWeakCanvas.get();

        iCanvas = aCanvas;
        iPrevTime = System.currentTimeMillis();
    }
    /**
        * Updates the animation time and generates frames which get flushed to the screen
        * <br>
        * * @note The timing for the next frame is decided upon the max(iDeltaTime*1000, elapsedTime)
          * iDeltaTime:  FrameTime set by client.
          * elapsedTime: Actual Time taken for rendering
              the  max(iDeltaTime*1000, elapsedTime) will be incremented on the SVGElement
        * @note rendering will be done only in PLAY state.
        * @note While in PAUSE state, SVGAnimator will repaint any changes done
        * to SVGImage via <code>M2GDOMChangeObserver.notifyDOMChange()</code>
        */
    public void run()
    {
        Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                   "SVGCanvasTask: run() - begin");

        if (iCanvas == null)
        {
            return;
        }

        try
        {
            synchronized (iCanvas)
            {
                long elapsedTime = System.currentTimeMillis()- iPrevTime;
                if (elapsedTime > 0)
                    iCanvas.increaseCurrentTime((float)(elapsedTime/ 1000.));
                iPrevTime = System.currentTimeMillis();
                iCanvas.paintToOffscreen();
                iCanvas.flushGraphics();
            }
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.EJavaUI,
                        "SVGCanvasTask: run() - exception: " + e.toString());
        }
        Logger.LOG(Logger.EJavaUI, Logger.EInfo, "SVGCanvasTask: run() - end");

    }
}
