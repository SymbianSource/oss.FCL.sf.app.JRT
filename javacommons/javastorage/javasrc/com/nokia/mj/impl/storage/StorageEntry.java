/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.storage;

import java.util.Enumeration;
import java.util.Hashtable;

/**
 * StorageEntry consists of StorageAttributes. It represents one storage row.
 * Row can contain several columns i.e. StorageAttributes.
 */
public final class StorageEntry
{
    private Hashtable iAttributes;

    public StorageEntry()
    {
        iAttributes = new Hashtable();
    }

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Append attribute. Appended to end of the list.
     *
     * @param aAttribute to be added.
     * @throws StorageException if attribute is null.
     */
    public void addAttribute(StorageAttribute aAttribute)
    throws StorageException
    {
        if (aAttribute == null)
        {
            throw new StorageException("Invalid argument");
        }
        iAttributes.put(aAttribute.getName(), aAttribute);
    }

    /**
     * Return elements.
     *
     * @return enumeration of attributes.
     */
    public Enumeration elements()
    {
        return iAttributes.elements();
    }

    /**
     * Return StorageAttribute by name.
     *
     * @param aName Name of the attribute to be returned.
     * @return storage attribute matching to name null otherwise.
     * @throws StorageException if aName is null or empty String.
     */
    public StorageAttribute getAttribute(String aName)
    throws StorageException
    {
        if (aName == null || aName.equals(""))
        {
            throw new StorageException("Invalid argument name");
        }

        return (StorageAttribute) iAttributes.get(aName);
    }

    /**
     * Remove attribute. First occurrence of the attribute is removed.
     *
     * @param aAttribute to be removed.
     * @return true if attribute removed, false otherwise.
     */
    public boolean removeAttribute(StorageAttribute aAttribute)
    {
        if (null == aAttribute)
        {
            return false;
        }

        return removeAttribute(aAttribute.getName());
    }

    /**
     * Remove StorageAttribute by name. First occurrence of the attribute
     * is removed.
     *
     * @param aName attribute name to be removed.
     * @return true if attribute removed, false otherwise
     */
    public boolean removeAttribute(String aName)
    {
        boolean success = false;

        if (aName != null)
        {
            StorageAttribute attr =
                (StorageAttribute) iAttributes.remove(aName);
            if (attr != null) success = true;
        }

        return success;
    }

    /**
     * Remove all attributes. Size is set to zero.
     */
    public void removeAll()
    {
        iAttributes.clear();
    }

    /**
     * Amount of StorageAttributes stored in StorageEntry.
     *
     * @return amount of entry attributes.
     */
    public int size()
    {
        return iAttributes.size();
    }

    /**
     * Print StorageEntry as String.
     *
     * @return entry in String format. {KEY=ATTR.toString(), ...}
     */
    public String toString()
    {
        return iAttributes.toString();
    }

    /*** ---------------------------- PROTECTED --------------------------- */
    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */
    /*** ----------------------------- NATIVE ----------------------------- */

}
