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
* Description:  Weak hash map which stores LandmarkStore objects
 *
*/


// PACKAGE
package com.nokia.mj.impl.location;

// IMPORTS
import java.util.Hashtable;
import java.lang.ref.WeakReference;
import javax.microedition.location.LandmarkStore;

// CLASS DESCRIPTION
/**
 * Weak hash map which stores LandmarkStore objects
 */
public class LandmarkStoreMap
{
    // Map of landmark stores
    private Hashtable iStoreTable;

    /**
     * Constructor.
     */
    public LandmarkStoreMap()
    {
        iStoreTable = new Hashtable();
    }

    /**
     * Registers the specified landmark store to this hash map
     *
     * @param aStore The store which is to be registered
     * @param aStoreUri The uri of the registered store. Note that
     *        this cannot acquired from the store object because
     *        getURI() method must be package private
     */
    public void registerStore(LandmarkStore aStore, String aStoreUri)
    {
        WeakReference weak = new WeakReference(aStore);
        // Synchronized because Garbage collector can cleanup store objects at any time
        synchronized (iStoreTable)
        {
            iStoreTable.put(aStoreUri.toLowerCase(), weak);
        }
    }

    /**
     * Unregisters the specified landmark store from the store map
     * @param aStoreUri The URI of the requested store
     */
    public void unregisterStore(String aStoreUri)
    {
        // Synchronized because Garbage collector can cleanup store objects at any time
        synchronized (iStoreTable)
        {
            iStoreTable.remove(aStoreUri.toLowerCase());
        }
    }

    /**
     * Gets the specified landmark store from the landmark store map
     * @param aStoreUri The URI of the requested store
     * @return The landmark store which matched the store uri. null is returned if
     *         there is no such store
     */
    public LandmarkStore getStore(String aStoreUri)
    {
        LandmarkStore store = null;
        // Synchronized because Garbage collector can cleanup store objects at any time
        synchronized (iStoreTable)
        {
            WeakReference weak = (WeakReference) iStoreTable.get(aStoreUri
                                 .toLowerCase());
            if (weak != null)
            {
                store = (LandmarkStore) weak.get();
            }
        }
        return store; // Returns null if there is no such store
    }
}

// End of file
