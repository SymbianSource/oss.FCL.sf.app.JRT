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
* Description:
*
*/


package java.security;

import java.util.Enumeration;


/**
 * Abstract class representing a collection of Permission objects.
 *
 * @author Nokia Corporation
 * @version 1.0
 */

public abstract class PermissionCollection
{
    public PermissionCollection()
    {
    }

    /**
     * Adds a permission object to the collection.
     *
     * @param permission the Permission object to be added.
     */
    public abstract void add(Permission permission);

    /**
     * Returns an enumeration containing all the Permission objects in the collection.
     * @return an enumeration of all the Permissions objects.
     */
    public abstract Enumeration elements();

    /**
     * Checks to see if the specified permission is implied by any pf the Permission
     * objects contained in the collection.
     * @param permission the Permission object to compare.
     * @return true if "permission" is implied by any of the the permissions in the
     *         collection, false otherwise.
     */
    public abstract boolean implies(Permission permission);

    /**
     * Returns the string representation of this collection
     *
     * @return the string representation of this collection
     */
    public String toString()
    {
        String newline = System.getProperty("line.separator");
        StringBuffer sb = new StringBuffer();

        Enumeration el = elements();
        while (el.hasMoreElements())
        {
            sb.append(newline);
            sb.append(el.nextElement());
        }

        sb.append(newline);
        return sb.toString();
    }
}
