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
//import com.nokia.mj.impl.rt.legacy.MIDEventServer;
import java.util.Hashtable;
//import com.nokia.mj.impl.rt.legacy.MemoryUtil;
import java.util.Enumeration;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.*;


public final class M2GManager implements  Listener
{
    //--------------------------------------------------
    // STATIC VARIABLES
    //--------------------------------------------------

    private static WeakReference sWeakManagerProxy = null;
    static Object sQuard = new Object();

    //--------------------------------------------------
    // VARIABLES
    //--------------------------------------------------
    private Hashtable iLiveObjects = new Hashtable();
    private M2GWeakManager iWeakManager = null;
    private int iSVGProxyHandle = M2GObject.INVALID_NATIVE_HANDLE;

    //--------------------------------------------------
    // METHODS
    //--------------------------------------------------

    /**
     * Constructor
     */
    private M2GManager()
    {
        super();
				
				
				//As Display is created in Thread only.. So no need to handle display over here.
				//scom.nokia.mj.impl.rt.support.Jvm.loadSystemLibrary("javam2g");      
        /*// setup the finalization via eswt's Display
        Display display = Display.getCurrent();
        if (display == null)
        {
            return;  // ?
        }
        display.addListener(SWT.Dispose, (Listener)this); */
        
				
		// Execute in UI thread     
        	Platform.executeInUIThread(
                new M2GRunnableQt() {
                    public void doRun() {
    															    	iSVGProxyHandle = _createSvgProxy();
    															  		}
    															  });
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
            _deleteSvgProxy(iSVGProxyHandle);
            iSVGProxyHandle = M2GObject.INVALID_NATIVE_HANDLE;
            sWeakManagerProxy = null;
            iLiveObjects = null;
        }
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
    static public M2GManager getInstance()
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
                
                weakManager = new M2GWeakManager(new M2GManager());
                sWeakManagerProxy = new WeakReference(weakManager);
            }
            return weakManager.getInstance();
        }
    }

    /**
     * Trigger GC every N objects
     */
    static final void heuristicGC()
    {
        //MemoryUtil.ensureMinFreeRAM();
    }

    //--------------------------------------------------
    // NATIVE METHODS
    //--------------------------------------------------
    private static native int _createSvgEngine(int aSvgProxyHandle );

    private static native int _createSvgProxy();

    private static native void _deleteSvgEngine(int aSvgProxyHandle, int aSvgEngineHandle);

    private static native void _deleteSvgProxy( int aSvgProxyHandle);


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
