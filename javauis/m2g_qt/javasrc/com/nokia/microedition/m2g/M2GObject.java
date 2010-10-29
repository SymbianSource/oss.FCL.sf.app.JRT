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

import java.io.IOException;

import com.nokia.microedition.m2g.NativeError;
import com.nokia.mj.impl.rt.support.Finalizer;
import com.nokia.mj.impl.utils.Logger;


/**
 * Base class
 */
abstract class M2GObject
{
    //--------------------------------------------------
    // STATIC CONSTANTS
    //--------------------------------------------------
    public static final int INVALID_NATIVE_HANDLE = 0;
    // static final String ESWT_PACKAGE              =   "org.eclipse.swt.widgets.Display" ;
    //--------------------------------------------------
    // VARIABLES
    //--------------------------------------------------
    int iNativeHandle;
    M2GManager iManager;
    private Finalizer mFinalizer;

    //--------------------------------------------------
    // METHODS
    //--------------------------------------------------
    /**
     * Constructor.
     */
    M2GObject()
    {
        this(INVALID_NATIVE_HANDLE);

    }

    /**
     * Constructor.
     * @param a native handle
     */
    M2GObject(int aHandle)
    {

        iNativeHandle = aHandle;

        iManager = M2GManager.getInstance();

    }

    /**
     * Checks handle validity.
     * @return true if valid
     */
    boolean doCheckValidity() throws IOException
    {
        return isHandleValid();
    }

    /**
     * Cleanup operations.
     */
    abstract void doCleanup();

    /**
     * Construct operations. Registration for finalization is done here.
     */
    void doConstruct()
    {
        Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                   "doConstruct(), register for finalization: "
                   + this.getClass().getName());
        M2GManager.heuristicGC();
        mFinalizer = new Finalizer()
        {
            public void finalizeImpl()
            {
                doFinalize();
            }
        };
    }

    /**
     * Create destroyer.
     */
    void createDestroyer()
    {
        Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                   "createDestroyer(): " + this.getClass().getName());

    }

    /**
     * Gets native handle
     * @return native handle
     */
    int getHandle()
    {
        return iNativeHandle;
    }

    /**
     * Gets native proxy handle
     * @return native handle
     */
    int getNativeSVGProxyHandle()
    {
        return iManager.getSVGProxyHandle();
    }

    /**
     * Checks if native handle is valid.
     */
    boolean isHandleValid()
    {
        M2GObject.checkNativeError(iNativeHandle);
        return M2GObject.checkHandle(iNativeHandle);
    }

    /**
     * Register an object
     * @param aObject Object
     */
    void register(M2GObject aObject)
    {
        iManager.register(aObject);
    }

    /**
     * @see ObjectExtensions#registerForFinalization()
     */
    private void doFinalize()
    {
        if (mFinalizer != null)
        {
            registeredFinalize();
            mFinalizer = null;
        }
    }

    synchronized final void registeredFinalize()
    {
        Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                   "registeredFinalize() begin - doCleanup() is called");
        doCleanup();
        Logger.LOG(Logger.EJavaUI, Logger.EInfo, "registeredFinalize() end");
    }

    /**
     * This method is called in Toolkit's destroyNotify call.
     * This will release convenient native resources. All native resource
     * will be deleted in registeredFinalize() method.
     */
    synchronized final void release()
    {
        Logger.LOG(Logger.EJavaUI, Logger.EInfo, "release()");
    }

    /**
     * Initializes native handles.
     */
    void resetHandles()
    {
        iNativeHandle = INVALID_NATIVE_HANDLE;
    }

    /**
     * Sets a native handle.
     */
    void setHandle(int aNativeHandle)
    {
        iNativeHandle = aNativeHandle;
    }

    /**
     * Unregister an object
     * @param aHandle Object's native handle
     */
    void unregister(Integer aHandle)
    {
        iManager.unregister(aHandle);
    }

    //--------------------------------------------------
    // STATIC METHODS
    //--------------------------------------------------
    /**
     * Checks that the handle is valid.
     * @param a native handle
     * @return true if handle is valid.
     */
    static boolean checkHandle(int aHandle)
    {
        return (aHandle > INVALID_NATIVE_HANDLE ? true : false);
    }

    /**
     * Checks for basic native error codes that maps to
     * standard Java exceptions and throws the exception if the error
     * code matches. Otherwise throws basic Error class.
     * @param aErrorCode Error code
     * @return Value passed in is returned if not an error
     * @throws Error if error
     */
    static int checkNativeError(int aErrorCode)
    {
        return NativeError.check(aErrorCode);
    }
}

