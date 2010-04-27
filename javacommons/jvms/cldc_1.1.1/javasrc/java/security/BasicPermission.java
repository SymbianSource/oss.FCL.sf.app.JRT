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

/**
 * The base class for permissions build on the concepts of target and actions.
 *
 * @see Permission
 * @see PermissionCollection
 *
 * @author Nokia Corporation
 * @version 1.0
 */
public abstract class BasicPermission
        extends Permission
{
    /**
     * Constructor.
     *
     * @param target the protected resource associated with this BasicPermission
     * @param actions the actions (as a comma separate list) to be carried upon the resource
     */
    public BasicPermission(String target, String actions)
    {
        super(target);
    }

    /**
     * Constructor.
     *
     * @param target the protected resource associated with this BasicPermission
     */
    public BasicPermission(String target)
    {
        super(target);
    }

    /**
     * Checks if a certain permission is implied by this permission.
     * This method returns always true, therefore the subclasses might want to
     * overwrite it.
     *
     * @param p the permission to check against
     * @return true.
     */
    public boolean implies(Permission p)
    {
        return true;
    }

    /**
     * Checks if a certain object equals this object.
     * This method returns always true, therefore the subclasses might want to
     * overwrite it.
     *
     * @param obj the object to check against
     * @return true
     */
    public boolean equals(Object obj)
    {
        return true;
    }

    /**
     * Returns the hash code value for this object.
     * This method returns always 0, therefore the subclasses might want to
     * overwrite it.
     *
     * @return value 0
     */
    public int hashCode()
    {
        return 0;
    }

    /**
     * Returns the actions (as a comma separated list) associated with
     * this BasicPermission
     *
     * @return the empty string "".
     */
    public String getActions()
    {
        return "";
    }

    /**
     * Returns a new PermissionCollection object suitable for storing BasicPermission
     * objects.
     * This method returns always 0, therefore the subclasses might want to
     * overwrite it.

     * @return null
     */
    public PermissionCollection newPermissionCollection()
    {
        return null;
    }
}
