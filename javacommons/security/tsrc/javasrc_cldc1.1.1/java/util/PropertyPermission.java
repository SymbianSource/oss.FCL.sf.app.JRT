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

package java.util;

import java.security.BasicPermission;
import java.security.Permission;
import java.security.PermissionCollection;

/**
 * This class is for property permissions.
 * <p>
 * The name is the name of the property. The naming convention follows the
 * hierarchical property naming convention. Also, an asterisk may appear at
 * the end of the name, following a ".", or by itself, to signify a wildcard
 * match. For example: "java.*" or "*" is valid, "*java" or "a*b" is not valid.
 * <p>
 * The actions to be granted are passed to the constructor in a string
 * containing a list of zero or more comma-separated keywords. The possible
 * keywords are "read" and "write". Their meaning is defined as follows:
 * <dl>
 *    <dt> read
 *    <dd> read permission. Allows <code>System.getProperty</code> to
 *         be called.
 *    <dt> write
 *    <dd> write permission. Allows <code>System.setProperty</code> to
 *         be called.
 * </dl>
 * <p>
 * The actions string is converted to lowercase before processing.
 *
 * @see BasicPermission
 * @see Permission
 * @see PermissionCollection
 *
 * @author Nokia Corporation
 * @version 1.0
 */
public final class PropertyPermission
        extends BasicPermission
{
    /**
     * Creates a new PropertyPermission object with the specified name.
     * The name is the name of the system property, and actions contains
     * a comma-separated list of the desired actions granted on the property.
     * Possible actions are "read" and "write".
     *
     * @param name the name of the PropertyPermission.
     * @param actions the actions string.
     * @throws NullPointerException if name is null.
     * @throws IllegalArgumentException if name is empty or if actions is
     *         invalid.
     */
    public PropertyPermission(String name, String actions)
    {
        super(name);
    }

    /**
     * Checks if this PropertyPermission object "implies" the specified
     * permission.
     * <p>
     * More specifically, this method returns true if:
     * <ul>
     * <li>p is an instanceof PropertyPermission,</li>
     * <li>p's actions are a subset of this object's actions, and</li>
     * <li>p's name is implied by this object's name. For example, "java.*"
     * implies "java.home".</li>
     * </ul>
     * @param p the permission to check against.
     * @return true if the specified permission is implied by this object,
     *        false if not.
     */
    public boolean implies(Permission p)
    {
        if (p instanceof PropertyPermission)
        {
            return true;
        }
        return false;
    }

    /**
     * Checks two PropertyPermission objects for equality. Checks that obj is
     * a PropertyPermission, and has the same name and actions as this object.
     *
     * @param obj the object we are testing for equality with this object.
     * @return true if obj is a PropertyPermission, and has the same name and
     *         actions as this PropertyPermission object.
     */
    public boolean equals(Object obj)
    {
        return true;
    }

    /**
     * Returns the hash code value for this object. The hash code used is the
     * hash code of this permissions name, that is, getName().hashCode(),
     * where getName is from the Permission superclass
     *
     * @return a hash code value for this object.
     */
    public int hashCode()
    {
        return 0;
    }

    /**
     * Returns the "canonical string representation" of the actions. That is,
     * this method always returns present actions in the following order:
     * read, write. For example, if this PropertyPermission object allows both
     * write and read actions, a call to getActions will return the string
     * "read,write".
     *
     * @return the canonical string representation of the actions.
     */
    public String getActions()
    {
        return null;
    }

    /**
     * Returns a new PermissionCollection object for storing PropertyPermission
     * objects.
     *
     * @return a new PermissionCollection object suitable for storing
     *         PropertyPermissions.
     */
    public PermissionCollection newPermissionCollection()
    {
        return null;
    }
}
