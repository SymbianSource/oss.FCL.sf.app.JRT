/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Location API manager
 *
*/


// PACKAGE
package com.nokia.mj.impl.location;

// IMPORTS
import com.nokia.mj.impl.location.NativeError;

import com.nokia.mj.impl.rt.support.Finalizer;

// CLASS DESCRIPTION
/**
 * LAPIManager class is class for creating and holding common function server
 * handle.
 */
public class LAPIManager
{
    // Singleton instance
    private static LAPIManager iInstance;

    // Handle to the native event source
    private final int iFunctionSourceHandle;

    private Finalizer iFinalizer;

    private final static String UNABLE_TO_CREATE_LAPI_MANAGER = "Unable to create Location API manager";

    /**
     * Returns an instance from this class
     */
    public static synchronized LAPIManager getInstance()
    {
        // Method level synhronization makes this thread-safe
        if (iInstance == null)
        {
            iInstance = new LAPIManager();
        }
        return iInstance;
    }

    /**
     * Returns a handle to the native Function source
     */
    public int getFunctionSourceHandle()
    {
        return iFunctionSourceHandle;
    }

    /**
     * Hidden constructor
     */
    private LAPIManager()
    {
        // Register this class for finalization
        iFinalizer = registerForFinalization();

        iFunctionSourceHandle = _createFunctionSource();

        if (iFunctionSourceHandle < NativeError.KErrNone)
        {
            throw new OutOfMemoryError(UNABLE_TO_CREATE_LAPI_MANAGER);
        }
    }

    Finalizer registerForFinalization()
    {
        return new Finalizer()
        {
            public void finalizeImpl()
            {
                doFinalize();
            }
        };

    }

    /**
     * Disposes the LandmarkStore native peer, if the handles are valid.
     * Invalid (negative) handles indicate that their creation failed in
     * the first place.
     */
    final void doFinalize()
    {
        _dispose(iFunctionSourceHandle);
    }

    /**
     * Creates native event source

     */
    private native int _createFunctionSource();

    /**
     * Disposes the native event source
     * @param aEventSourceHandle Handle to the native Function source
     */
    private native void _dispose(int aFunctionSourceHandle);
}

// End of file
