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
* Description:  PIM list implementation base.
 *
*/


// PACKAGE
package com.nokia.mj.impl.pim;

// IMPORTS
import java.util.Hashtable;

// CLASS DEFINITION
/**
 * An utility class for holding PIMItemImpl objects and finding them by their
 * handles to their native side peers.
 */
class ItemTable
{

    /**
     * Items.
     *
     * @li Keys are of type Integer, the native handles of items.
     * @li Values are of type PIMItemImpl.
     */
    protected Hashtable iItems;

    /**
     * Constructor.
     */
    ItemTable()
    {
        iItems = new Hashtable();
    }

    /**
     * Adds a new item safely to the item table.
     *
     * @param aItem
     *            Item to be added.
     *
     * @throws NullPointerException
     *             if \a aItem is null.
     * @throws IllegalArgumentException
     *             if \a aItem already exists in the table.
     */
    void addItem(PIMItemImpl aItem)
    {
        exceptIfNull(aItem);
        int itemHandle = aItem.jniNativeHandle();
        Integer key = new Integer(itemHandle);

        if (!iItems.containsKey(key))
        {
            iItems.put(key, aItem);
        }
        else
        {
            throw new IllegalArgumentException();
        }
    }

    /**
     * Gets an item, indicated by a handle, from the item table.
     *
     * @param aHandle
     *            Handle to the item to be found.
     *
     * @return The item which has the handle aHandle; null if not found.
     */
    PIMItemImpl getItem(int aHandle)
    {
        Integer key = new Integer(aHandle);
        PIMItemImpl item = (PIMItemImpl) iItems.get(key);
        return item;
    }

    /**
     * Removes an item from the item table.
     *
     * @param aItem
     *            The item to be removed.
     * @return The removed item; null if not found.
     * @throws NullPointerException
     *             if aItem is null.
     */
    PIMItemImpl removeItem(PIMItemImpl aItem)
    {
        int itemHandle = aItem.jniNativeHandle();
        return removeItem(itemHandle);
    }

    /**
     * Removes an item, indicated by a handle, from the item table.
     *
     * @param aItemHandle
     *            Handle to the item to be removed.
     * @return The removed item; null if not found.
     */
    PIMItemImpl removeItem(int aItemHandle)
    {
        Integer key = new Integer(aItemHandle);
        PIMItemImpl removedItem = (PIMItemImpl) iItems.remove(key);
        return removedItem;
    }

    /**
     * Checks whether given item is found in the item table.
     *
     * @param aItem
     *            The item to be found.
     * @return true if the item is found; false otherwise.
     */
    boolean containsItem(PIMItemImpl aItem)
    {
        exceptIfNull(aItem);
        int itemHandle = aItem.jniNativeHandle();
        return containsItem(itemHandle);
    }

    /**
     * Checks whether given item, indicated by a handle, is found in the item
     * table.
     *
     * @param aHandle
     *            Handle to the item to be found.
     * @return true if the item is found; false otherwise.
     */
    boolean containsItem(int aHandle)
    {
        Integer key = new Integer(aHandle);
        return iItems.containsKey(key);
    }

    /**
     * Checks whether given argument is null.
     *
     * @throws NullPointerException
     *             if aObject is null.
     */
    protected void exceptIfNull(Object aObject)
    {
        if (aObject == null)
        {
            throw new NullPointerException();
        }
    }

}

// End of File
