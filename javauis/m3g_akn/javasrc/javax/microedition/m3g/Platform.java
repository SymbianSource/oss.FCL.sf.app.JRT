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

package javax.microedition.m3g;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;

import com.nokia.mj.impl.rt.legacy.MemoryUtil;
import com.nokia.mj.impl.rt.legacy.ToolkitInvoker;

/**
 * Implements platform-dependent functionality. At the moment, this
 * includes native finalization and some helper methods for
 * synchronizing 2D and 3D rendering.
 */
class Platform
{

    //------------------------------------------------------------------
    // Package private methods
    //------------------------------------------------------------------

    /**
     * Registers an object for finalization
     */
    static final void registerFinalizer(Object obj)
    {
        if (obj instanceof Object3D || obj instanceof Graphics3D)
        {
            heuristicGC();
        }
    }

    /**
     * Flushes all pending rendering to a Graphics context and blocks
     * until finished
     */
    static final void sync(Graphics g)
    {
        ToolkitInvoker invoker = ToolkitInvoker.getToolkitInvoker();
        // NGA specific change.
        // Graphics is passed to toolkit in order
        // that target Canvas of Graphics gets notified
        // about start of M3G content.
        invoker.toolkitSync(invoker.getToolkit(), g);
    }

    /**
     * Flushes all pending rendering to an Image object
     */
    static final void sync(Image img)
    {
        ToolkitInvoker invoker = ToolkitInvoker.getToolkitInvoker();
        invoker.toolkitSync(invoker.getToolkit());
    }

    /**
     * Finalizes the native peer of an interface
     */
    static final native void finalizeInterface(int handle, int eventSourceHandle);

    /**
     * Finalizes the native peer of an object
     * @param eventSourceHandle Event source native handle
     * @param handle Native handle
     */
    static final void finalizeObject(int eventSourceHandle, int handle)
    {
        try
        {
            _finalizeObject(eventSourceHandle, handle);
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
        MemoryUtil.ensureMinFreeRAM();
    }
    private static final native void _finalizeObject(int eventSourceHandle, int handle);
}

