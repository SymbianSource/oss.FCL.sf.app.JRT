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


package com.nokia.mj.impl.installer.ui;

/**
 * Permission info for JavaInstaller UI.
 */
public class PermissionInfo
{
    /** Permission names. */
    private String[] iPermissionNames = null;
    /** Permissions details message shown to the user. */
    private String iPermissionsDetails = null;
    /** Flag telling if user allowed or denied the permissions. */
    private Boolean iPermissionAllowed = null;

    /** Constructor. */
    private PermissionInfo()
    {
    }

    /** Constructor. */
    public PermissionInfo(String[] aPermissionNames, String aPermissionsDetails)
    {
        iPermissionNames = aPermissionNames;
        iPermissionsDetails = aPermissionsDetails;
    }

    /** Get permission names. */
    public String[] getPermissionNames()
    {
        return iPermissionNames;
    }

    /** Get permissions details. */
    public String getPermissionsDetails()
    {
        return iPermissionsDetails;
    }

    /**
     * Returns true if user allowed the requested permissions,
     * false if user denied the requested permissions, and null
     * if user has not answered.
     */
    public Boolean isPermissionAllowed()
    {
        return iPermissionAllowed;
    }

    /**
     * Set the flag telling if user allowed or denied the permissions.
     */
    public void setPermissionAllowed(boolean aPermissionAllowed)
    {
        iPermissionAllowed = new Boolean(aPermissionAllowed);
    }

    public String toString()
    {
        StringBuffer buf = new StringBuffer("PermissionInfo\n");
        if (isPermissionAllowed() != null)
        {
            buf.append("PermissionAllowed: ")
            .append(isPermissionAllowed()).append("\n");
        }
        for (int i = 0; iPermissionNames != null && i < iPermissionNames.length; i++)
        {
            buf.append("PermissionName[").append(i).append("]: ")
            .append(iPermissionNames[i]).append("\n");
        }
        buf.append("PermissionsDetails: ").append(getPermissionsDetails());
        return buf.toString();
    }
}
