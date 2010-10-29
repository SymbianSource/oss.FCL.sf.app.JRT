/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
import org.eclipse.swt.widgets.SVGAnimatorControl;
import com.nokia.mj.impl.rt.support.Finalizer;
import com.nokia.mj.impl.utils.Logger;

public class M2GSVGeSWTAnimator extends SVGAnimator
{
    //--------------------------------------------------
    // STATIC CONSTANTS
    //--------------------------------------------------
    private static final String ANIMATOR_CONTROL_BASE_CLASS =
        "org.eclipse.swt.widgets.Canvas";

    // Exception text


    //--------------------------------------------------
    // VARIABLES
    //--------------------------------------------------
    private SVGAnimatorControl iAnimatorControl;

    private Finalizer mFinalizer = new Finalizer()
    {
        public void finalizeImpl()
        {
            doFinalize();
        }
    };

    //--------------------------------------------------
    // METHODS
    //--------------------------------------------------
    /**
     * Constructor
     * @param aImage
     */
    protected M2GSVGeSWTAnimator(SVGImage aImage)
    {
        iAnimatorControl = new SVGAnimatorControl(/*false,*/ aImage);
    }

    /**
     * @see javax.microedition.m2g.SVGAnimator#getTargetComponent()
     */
    public Object getTargetComponent()
    {
        return iAnimatorControl;
    }

    /**
    * @see javax.microedition.m2g.SVGAnimator#getTimeIncrement()
    */
    public float getTimeIncrement()
    {
        Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                   "getTimeIncrement() " + iAnimatorControl.getTimeIncrement());
        return iAnimatorControl.getTimeIncrement();
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
        if (iAnimatorControl.isStopped())
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
        if (iAnimatorControl.isStopped())
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
        if (!iAnimatorControl.isPlaying())
        {
            throw new IllegalStateException(
                /*SF*/"The animator is not in the playing or paused state."/*SF*/);
        }
        Logger.LOG(Logger.EJavaUI, Logger.EInfo, "pause()");
        iAnimatorControl.pause();
    }

    /**
    * @see javax.microedition.m2g.SVGAnimator#play()
    */
    public void play()
    {
        if (iAnimatorControl.isPlaying())
        {
            throw new IllegalStateException(
                /*SF*/"The animator is not currently in the stopped or paused state."/*SF*/);
        }
        Logger.LOG(Logger.EJavaUI, Logger.EInfo, "play()");
        iAnimatorControl.play();
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
        Logger.LOG(Logger.EJavaUI, Logger.EInfo, "registeredFinalize()");
        iAnimatorControl = null;
    }

    /**
     * @see javax.microedition.m2g.SVGAnimator#setSVGEventListener()
     */
    public void setSVGEventListener(SVGEventListener svgEventListener)
    {
        iAnimatorControl.setEventListener(svgEventListener);
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
        Logger.LOG(Logger.EJavaUI, Logger.EInfo, "setTimeIncrement() - " + timeIncrement);
        iAnimatorControl.setTimeIncrement(timeIncrement);
    }

    /**
     * @see javax.microedition.m2g.SVGAnimator#stop()
     */
    public void stop()
    {
        if (iAnimatorControl.isStopped())
        {
            throw new IllegalStateException(
                /*SF*/"The animator is not in the playing or paused state."/*SF*/);
        }
        Logger.LOG(Logger.EJavaUI, Logger.EInfo, "stop()");
        iAnimatorControl.stop();
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
        return new M2GSVGeSWTAnimator(svgImage);
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
                (!componentBaseClass.equals(ANIMATOR_CONTROL_BASE_CLASS)))
        {
            throw new IllegalArgumentException(
                /*SF*/"The requested componentBaseClass is not supported by the implementation."/*SF*/);
        }

        return buildAnimator(svgImage);
    }
}
