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
/**
 * \file
 * \brief Target platform dependent Java module for Symbian.
 *
 */

package com.nokia.microedition.m2g;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;
import org.eclipse.swt.widgets.Display;

import com.nokia.mj.impl.nokialcdui.LCDUIInvoker;
import org.eclipse.swt.internal.extension.DisplayExtension;
import org.eclipse.swt.internal.extension.Toolkit;

/**
 * Implements platform-dependent functionality. At the moment, this
 * includes native finalization and some helper methods for
 * synchronizing 2D and 3D rendering.
 */
class Platform
{
    /**
     * eSWT display for ui thread access
     */
    private static Display display = null;
    private static boolean libraryLoaded = false;

    //------------------------------------------------------------------
    // Package private methods
    //------------------------------------------------------------------

    /**
     * Executes given runnable in UI thread if caller thread is not UI thread
     */
    static void executeInUIThread(M2GRunnableQt obj)
    {
        if (display == null)
        {
            if (!uiThreadAvailable())
            {
                throw new Error("Ui thread not available");
            }
        }
        if (display.isDisposed())
        {
            throw new Error("Display already disposed");
        }

        if (obj != null)
        {
            // If we are not in UI thread use display
            if (Thread.currentThread() != display.getThread())
            {
                display.syncExec(obj);
            }
            else
            {
                // In this case we are in UI thread so just execute directly
                obj.run();
            }
            // Check if any exceptions occured in execution
            // and throw forward in caller thread
            obj.checkAndThrow();
        }
    }

    /**
     * Check the UI thread / toolkit init status and store display if it is available
     * @return true if either lcdui or eswt toolkit is initialized and ui thread is accessible
     *              otherwise false
     */
    static boolean uiThreadAvailable()
    {
        if (display != null)
        {
            return true;
        }
        else
        {
            //display = DisplayExtension.getDisplayInstance();
            //As now the thread is always available we are taking it from Toolkit.
            display  = Toolkit. getInternalDisplay();


            if (display == null)
            {

                return false;
            }
            else
            {

                // UI thread is available, so load native library if not already loaded
                if (!libraryLoaded)
                {

                    com.nokia.mj.impl.rt.support.Jvm.loadSystemLibrary("javam2g");
                    libraryLoaded = true;
                }
                return true;
            }
        }
    }



    static final void finalizeObject(int handle)
    {
        try
        {
            final int finalHandle = handle;
            executeInUIThread(
                new M2GRunnableQt()
            {
                void doRun()
                {
                    _finalizeObject(finalHandle);
                }
            });
        }
        catch (Exception e)
        {
            // do nothing
        }
    }

    //------------------------------------------------------------------
    // Private methods
    //------------------------------------------------------------------

    /**
     * Trigger GC if minimum free memory limit has been exceeded in the native side
     */
    static final void heuristicGC()
    {
    }
    private static final native void _finalizeObject(int handle);
}

