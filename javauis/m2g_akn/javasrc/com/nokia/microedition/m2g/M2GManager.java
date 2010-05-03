/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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

import java.lang.ref.WeakReference;
import com.nokia.mj.impl.rt.legacy.MIDEventServer;
import java.util.Hashtable;
import com.nokia.mj.impl.rt.legacy.ToolkitInvoker;
import com.nokia.mj.impl.rt.legacy.MemoryUtil;

import java.util.Enumeration;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.*;


public final class M2GManager implements M2GDestroyable, Listener
{
    //--------------------------------------------------
    // STATIC VARIABLES
    //--------------------------------------------------

    public static final boolean LCDUI_TOOLKIT = false;
    public static final boolean eSWT_TOOLKIT  = true;

    private static WeakReference sWeakManagerProxy = null;
    static Object sQuard = new Object();

    //--------------------------------------------------
    // VARIABLES
    //--------------------------------------------------
    private Hashtable iLiveObjects = new Hashtable();
    private M2GWeakManager iWeakManager = null;
    private M2GDestroyer iDestroyer = null;
    private int iEventSourceHandle = M2GObject.INVALID_NATIVE_HANDLE;
    private int iSVGProxyHandle = M2GObject.INVALID_NATIVE_HANDLE;
    private boolean iUIToolkit          = LCDUI_TOOLKIT;

    //--------------------------------------------------
    // METHODS
    //--------------------------------------------------

    /**
     * Constructor
     */
    private M2GManager(boolean aUIToolkit)
    {
        super();
        com.nokia.mj.impl.rt.support.Jvm.loadSystemLibrary("javam2g");
        iDestroyer = new M2GDestroyer(this);
        iUIToolkit = aUIToolkit;

        if (aUIToolkit == eSWT_TOOLKIT) // eSWT is the current UITOOLKIT.
        {
            // Initialize eswt function server
            iEventSourceHandle = _initSwtEventSource();

            // setup the finalization via eswt's Display
            Display display = Display.getCurrent();
            if (display == null)
            {
                return;  // ?
            }

            display.addListener(SWT.Dispose, (Listener)this);
        }
        else
        {
            // Initialize toolkit event source
            iEventSourceHandle = _initEventSource(M2GManager.getEventServerHandle());
            // Initialize svg proxy
        }

        iSVGProxyHandle = _createSvgProxy(iEventSourceHandle, iUIToolkit);
        M2GManager.heuristicGC();
    }

    /**
     * Impelements 'handleEvent' from Listener
     */
    public void handleEvent(Event e)
    {
        if (e.type == SWT.Dispose)
        {
            doCleanup();
        }
    }

    /**
     * Does the cleanuping
     */
    protected void doCleanup()
    {
        finalizeObjects();
        synchronized (sQuard)
        {
            _deleteSvgProxy(iEventSourceHandle, iSVGProxyHandle, iUIToolkit);
            // Finalize event source
            _finalizeEventSource(iEventSourceHandle, iUIToolkit);
            iEventSourceHandle = M2GObject.INVALID_NATIVE_HANDLE;
            iSVGProxyHandle = M2GObject.INVALID_NATIVE_HANDLE;
            sWeakManagerProxy = null;
            iLiveObjects = null;
        }
    }

    /**
     * @see M2GDestroyable#doDestroy()
     */
    public void doDestroy()
    {
        doCleanup();
    }

    /**
     * Find an element from the handle-to-element map.
     * @param aElementHandle Handle
     * @return SVGElement. Null if not found
     */
    synchronized private void finalizeObjects()
    {
        Enumeration objects = iLiveObjects.elements();
        while (objects.hasMoreElements())
        {
            Object weakObject = ((WeakReference)objects.nextElement()).get();
            if (weakObject != null)
            {
                ((M2GObject)weakObject).registeredFinalize();
            }
        }
        // Clear table
        iLiveObjects.clear();
    }

    /**
     * Gets toolkit event source handle
     * @return Toolkit event source handle
     */
    public int getEventSourceHandle()
    {
        synchronized (sQuard)
        {
            return iEventSourceHandle;
        }
    }

    /**
     * Gets native SVG proxy handle
     * @return SVG proxy handle
     */
    public int getSVGProxyHandle()
    {
        synchronized (sQuard)
        {
            return iSVGProxyHandle;
        }
    }

    /**
     * Gets native SVG proxy handle
     * @return SVG proxy handle
     */
    void init(M2GWeakManager aWeakManager)
    {
        iWeakManager = aWeakManager;
    }

    /**
     * Register an object
     * @param aObject Object
     */
    synchronized public void register(M2GObject aObject)
    {
        if (aObject == null)
        {
            return;
        }
        iLiveObjects.put(new Integer(aObject.getHandle()), new WeakReference(aObject));
    }

    /**
     * Unregister an object
     * @param aHandle Object's native handle
     */
    synchronized public void unregister(Integer aHandle)
    {
        iLiveObjects.remove(aHandle);
    }

    //--------------------------------------------------
    // STATIC METHODS
    //--------------------------------------------------

    /**
     * Gets instance of singleton manager object
     * @return manager object reference
     */
    static public M2GManager getInstance(boolean aUIToolkit)
    {
        M2GWeakManager weakManager = null;
        synchronized (sQuard)
        {
            if (sWeakManagerProxy != null)
            {
                weakManager = (M2GWeakManager)sWeakManagerProxy.get();
            }
            // Check if object null
            if (weakManager == null)
            {
                // Create a new object and put it into the static member variable
                weakManager = new M2GWeakManager(new M2GManager(aUIToolkit));
                sWeakManagerProxy = new WeakReference(weakManager);
            }
            return weakManager.getInstance();
        }
    }

    /**
     * Synchronize toolkit.
     */
    static void flushDisplayBuffer()
    {
        ToolkitInvoker invoker = ToolkitInvoker.getToolkitInvoker();
        invoker.toolkitSync(invoker.getToolkit());
    }

    /**
     * Returns toolkit handle
     * @return toolkit handle
     */
    static int getToolkitHandle()
    {
        ToolkitInvoker invoker = ToolkitInvoker.getToolkitInvoker();
        return invoker.toolkitGetHandle(invoker.getToolkit());
    }

    /**
     * Returns UI event server handle
     * @return UI event server handle
     */
    static int getEventServerHandle()
    {
        ToolkitInvoker invoker = ToolkitInvoker.getToolkitInvoker();
        return invoker.getUIEventServerHandle();
    }

    /**
     * Trigger GC every N objects
     */
    static final void heuristicGC()
    {
        MemoryUtil.ensureMinFreeRAM();
    }

    //--------------------------------------------------
    // NATIVE METHODS
    //--------------------------------------------------
    private static native int _createSvgEngine(
        int aEventSourceHandle, int aSvgProxyHandle, boolean aUIToolkit);

    private static native int _createSvgProxy(
        int aEventSourceHandle, boolean aUIToolkit);

    private static native void _deleteSvgEngine(
        int aEventSourceHandle, int aSvgProxyHandle, int aSvgEngineHandle, boolean aUIToolkit);

    private static native void _deleteSvgProxy(
        int aEventSourceHandle, int aSvgProxyHandle, boolean aUIToolkit);

    // Native event source finalization
    private static native void _finalizeEventSource(int aEventSourceHandle, boolean aUIToolkit);

    // Native event source constructor
    private static native int _initEventSource(int aEventServerHandle);

    private static native int _initSwtEventSource();
}

//--------------------------------------------------
// OTHER CLASSES
//--------------------------------------------------

/*
 * M2GWeakManager takes care of carbage colletion of an M2GManager object
 */
class M2GWeakManager
{
    //--------------------------------------------------
    // VARIABLES
    //--------------------------------------------------
    M2GManager iManager;

    //--------------------------------------------------
    // METHODS
    //--------------------------------------------------

    /**
     * Constructor
     * @param aManager Manager object
     */
    public M2GWeakManager(M2GManager aManager)
    {
        reset(aManager);
    }

    /**
     * Return a reference to the manager
     * @return a reference to the manager
     */
    M2GManager getInstance()
    {
        return iManager;
    }

    /**
     * Reset
     * @param aManager
     */
    void reset(M2GManager aManager)
    {
        iManager = aManager;
        if (iManager != null)
        {
            iManager.init(this);
        }
    }
}
