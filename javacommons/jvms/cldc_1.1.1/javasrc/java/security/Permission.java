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
 * The Permission class is the quantification of the concept of protecting
 * access to sensitive functionality. Structurally, it is made of a target
 * and action where target defines the protected resource and the action
 * defines the action to be carried upon the resource
 *
 * @author Nokia Corporation
 * @version 1.0
 */
public abstract class Permission
{
    private String target;

    /**
     * Constructs a permission with the specified target.
     *
     * @param target target associated with the Permission object.
     */
    public Permission(String target)
    {
        this.target = target;
    }

    /**
     * Checks if a certain object equals this object.
     *
     * @param obj the object to check against
     */
    public abstract boolean equals(Object obj);

    /**
     * Returns the hash code value for this object.
     *
     * @return hash code associated with this Permission object
     */
    public abstract int hashCode();

    /**
     * Returns the actions (as a comma separated list) associated with
     * this Permission
     */
    public abstract String getActions();

    /**
     * Returns the target accosiated with this Permission object.
     *
     * @return the target accosiated with this Permission object
     */
    public final String getName()
    {
        return target;
    }

    /**
     * Checks if a certain permission is implied by this permission.
     *
     * @param p the permission to check against
     * @return true if provided permission is implied by this permission,
     *         false otherwise
     */
    public abstract boolean implies(Permission permission);

    /**
     * Returns a new PermissionCollection object suitable for storing Permission
     * objects.
     * This method returns always null, therefore the subclasses might want to
     * overwrite it.

     * @return null
     */
    public PermissionCollection newPermissionCollection()
    {
        return null;
    }

    /**
     * Returns the string representation of this collection
     *
     * @return the string representation of this collection
     */
    public String toString()
    {
        String actions = getActions();
        return "(" + getClass().getName() + " " + getName() + " " + actions + ")";
    }
}
