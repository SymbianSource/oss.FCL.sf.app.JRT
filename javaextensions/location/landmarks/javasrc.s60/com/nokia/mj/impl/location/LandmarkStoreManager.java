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
* Description:  Landmark store manager for handling landmark stores
 *
*/


package com.nokia.mj.impl.location;


import com.nokia.mj.impl.location.NativeError;
import java.util.Vector;
import java.io.IOException;

import javax.microedition.location.LandmarkStore;
import com.nokia.mj.impl.rt.support.Finalizer;


/**
 * Landmark store manager for handling landmark stores
 *
 * This class provides functionalities to open, create and delete landmark
 * stores. The class is a singleton instance which can be access using
 * the getInstance() method from everywhere and any time possible.
 */
public class LandmarkStoreManager
{
    // Singleton instance for this class
    private static LandmarkStoreManager sInstance;

    // Landmark store map
    private final LandmarkStoreMap iStoreMap;

    // Handle to this class' native peer
    private final int iManagerHandle;

    // Handle to native Function source
    private final int iFunctionSourceHandle;

    private Finalizer mFinalizer;

    // Error messages
    private static final String LANDMARKSTORE_OPERATION_ERROR
    = "Unable to do landmarkStore operation";

    private static final String LIST_LANDMARK_STORES_ERROR
    = "Unable to list landmarkStores: ";

    /**
     * Returns an instance from this class
     */
    public static synchronized LandmarkStoreManager getInstance()
    {
        // Method level synhronization makes this thread-safe
        if (sInstance == null)
        {
            sInstance = new LandmarkStoreManager();
        }
        return sInstance;
    }

    /**
     * Hidden constructor. Creates an instance from this class
     * the constructor also creates the native side peer object
     */
    private LandmarkStoreManager()
    {
        // Register this object for finalization
        mFinalizer = registerForFinalization();
        // Create native peer
        iFunctionSourceHandle =
            LAPIManager.getInstance().getFunctionSourceHandle();
        iManagerHandle = _createNativePeer(iFunctionSourceHandle);
        // Check if the creation failed and throw error
        if (iManagerHandle < NativeError.KErrNone)
        {
            throw new OutOfMemoryError(LANDMARKSTORE_OPERATION_ERROR);
        }

        iStoreMap = new LandmarkStoreMap();
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
     * Disposes the LandmarkStore Manager native peer, if the handles are valid.
     * Invalid (negative) handles indicate that their creation failed in
     * the first place.
     */
    final void doFinalize()
    {
        _dispose(iFunctionSourceHandle, iManagerHandle);
    }

    /**
     * Returns an opened store. null is returned if the specified store
     * has not been opened yet. Stores can be opened using openStore() method
     *
     * @param aLocator The locator indicating where the store is. null locator
     *        is interpreted as the default store
     * @return The opened landmark store instance
     */
    public LandmarkStore getStore(Locator aLocator)
    {
        String uri = aLocator != null ? aLocator.getStoreURI() : "";
        return iStoreMap.getStore(uri);
    }

    /**
     * Registers an opened landmark store to this manager
     *
     * @param aStore The store which is to be registered
     * @param aStoreUri The uri of the registered store. Note that
     *        this cannot acquired from the store object because
     *        getURI() method must be package private
     */
    public void registerStore(LandmarkStore aStore, String aStoreUri)
    {
        iStoreMap.registerStore(aStore, aStoreUri);
    }

    /**
     * Opens a landmark store. The invocation calls native side landmark store
     * manager to open a specific landmark store. If the locator provides a
     * null URI, the default store will be opened. Otherwise a named store will
     * be opened if it is available
     *
     * @param aLocator The locator indicating where the store is
     * @return Handle to the native side landmark store peer object
     */
    public int openStore(Locator aLocator)
    {
        String uri = aLocator != null ? aLocator.getStoreURI() : null;
        // Open the native side landmark store
        int handle = _openStore(iFunctionSourceHandle, iManagerHandle, uri);
        // return the handle to the caller which is responsible for
        // interpreting possible errors when opening the store
        return handle;
    }

    /**
     * Creates a landmark store. The invocation calls native side landmark store
     * manager to create a specific landmark store. The URI must not be null
     *
     * @param aLocator The locator indicating where the store is
     * @return NativeError.KErrNone if no error occured. Otherwise one of the
     *         system-wide error codes
     */
    public int createStore(Locator aLocator)
    {
        if (aLocator == null)
        {
            throw new NullPointerException();
        }

        String uri = aLocator.getStoreURI();
        // Create the store to the native database
        return _createStore(iFunctionSourceHandle, iManagerHandle, uri);
    }

    /**
     * Deletes a landmark store. The invocation calls native side landmark store
     * manager to delete a specific landmark store. The URI must not be null.
     * Note that all landmarks and categories will be removed from this store
     *
     * Note that this does not dispose the native side object. It only closes
     * the store so the store must be disposed separately an informed to this
     * class.
     *
     * @param aLocator The locator indicating where the store is
     * @return NativeError.KErrNone if no error occured. Otherwise one of the
     *         system-wide error codes
     */
    public int deleteStore(Locator aLocator)
    {
        if (aLocator == null)
        {
            throw new NullPointerException();
        }

        String uri = aLocator.getStoreURI();
        // Unregister the landmark store if there is an open store. This
        // quarantees that the closed store will not be provided to any
        // requesting client
        iStoreMap.unregisterStore(uri);

        // Delete the store from the native database
        return _deleteStore(iFunctionSourceHandle, iManagerHandle, uri);
    }

    /**
     * Removes a native side landmark store peer object from this class
     * @param aStoreHandle A handle to the store which is to be removed
     */
    public void removeStore(String aStoreUri, int aStoreHandle)
    {
        // Unregister the landmark store if there is an open store. This
        // quarantees that the closed store will not be provided to any
        // requesting client. Store URI must not be null.
        // "" indicates the default store
        if (aStoreUri != null)
        {
            iStoreMap.unregisterStore(aStoreUri);
        }

        _removeStore(iFunctionSourceHandle, iManagerHandle, aStoreHandle);
    }

    /**
     * Lists the landmark stores in the device. The store names are returned
     * as a vector holding Locator objects. The names can be acquired using
     * Locator.getName() method
     *
     * @return Vector holding the list of landmark stores in the device
     */
    public Vector listStores() throws IOException
    {
        int[] errors = new int[1];
        // Get store unified resource identifiers from the native
        String[] storeUris = _listStores(iFunctionSourceHandle, iManagerHandle,
                                         errors);
        // Check for errors and throw an IOException if something went wrong
        NativeError.checkIO(errors[0], LIST_LANDMARK_STORES_ERROR);

        Vector locators = null;
        // Create locator list from the stores
        if (storeUris != null && storeUris.length > 0)
        {
            locators = new Vector(storeUris.length);
            for (int i = 0; i < storeUris.length; i++)
            {
                // Create a new locator explicitly using the constuctor since
                // the URIs from the native side are already valid ones
                try
                {
                    Locator locator = new Locator(storeUris[i]);
                    locators.addElement(locator);
                }
                catch (IllegalArgumentException e)
                {
                    // Ignore invalid store URIs
                }
            }
        }
        // Return the list of store locators. null indicates that there are no
        // stores in the device
        return locators;
    }

    /**
     * Creates the native peer for this object
     * @param aEventSourceHandle Handle to event source
     * @retutn Handle to the native side peer object
     */
    private native int _createNativePeer(int aEventSourceHandle);

    /**
     * Opens a native side landmark store
     * @param aEventSourceHandle Handle to event source
     * @param aManagerHandle Handle to the landmark store manager
     * @param aURI Unified Resource Locator pointing where the store is
     * @return Handle to the opened store. NativeError.KErrNotFound is returned
     *         if the specified store does not exist
     */
    private native int _openStore(int aEventSourceHandle, int aManagerHandle,
                                  String aURI);

    /**
     * Creates a new landmark store to the native side
     * @param aEventSourceHandle Handle to event source
     * @param aManagerHandle Handle to the landmark store manager
     * @param aURI Unified Resource Locator pointing where the store is
     * @return NativeError.KErrNone if the was no error. Otherwise, one of the
     *         system wide error codes
     */
    private native int _createStore(int aEventSourceHandle, int aManagerHandle,
                                    String aURI);

    /**
     * Deletes an existing native landmark database
     * @param aEventSourceHandle Handle to event source
     * @param aManagerHandle Handle to the landmark store manager
     * @param aURI Unified Resource Locator pointing where the store is
     * @return NativeError.KErrNone if the was no error. Otherwise, one of the
     *         system wide error codes
     */
    private native int _deleteStore(int aEventSourceHandle, int aManagerHandle,
                                    String aURI);

    /**
     * Removes a landmark store native side peer object from this class
     * If the specified store was not found this call is simply ignored
     * @param aEventSourceHandle Handle to event source
     * @param aManagerHandle Handle to the landmark store manager
     * @param aStoreHandle Handle to the store which should be removed
     */
    private native void _removeStore(int aEventSourceHandle,
                                     int aManagerHandle, int aStoreHandle);

    /**
     * Returns a list of landmark store uris
     * @param aEventSourceHandle Handle to event source
     * @param aManagerHandle Handle to the landmark store manager
     * @param aErrors Error array. On return, contains the occured errors
     * @return Array holding the uris of the native landmark databases
     */
    private native String[] _listStores(int aEventSourceHandle,
                                        int aManagerHandle, int[] aErrors);

    /**
     * Disposes this object native side peer
     * @param aEventSourceHandle Handle to the event source
     * @param aManagerHandle Handle to the native side peer object
     */
    private native void _dispose(int aEventSourceHandle, int aManagerHandle);
}

// End of file
