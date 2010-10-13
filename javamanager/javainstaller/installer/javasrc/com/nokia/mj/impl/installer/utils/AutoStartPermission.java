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
* Description: Permission object for managing access to auto start feature
*
*/



package com.nokia.mj.impl.installer.utils;

import java.security.Permission;
import java.security.PermissionCollection;
import com.nokia.mj.impl.security.common.PermissionBase;

/**
 * Permission class representing the token access to auto start functionality.
 *
 */
public class AutoStartPermission extends PermissionBase
{
    /**
     * Constructor
     *
     */
    public AutoStartPermission()
    {
        super(null);
    }

    /**
     * Returns the question (as localized text) associated with the security
     * prompt
     *
     * @return the localized text associated with the security prompt
     */
    public String getSecurityPromptQuestion(int aInteractionMode)
    {
        // no prompt details supported yet
        return null;
    }

    /**
     * Only implies permission objects of same instance
     */
    public boolean implies(Permission p)
    {
        if (p instanceof AutoStartPermission)
        {
            return true;
        }
        return false;
    }

    /**
     * String representation of the permission
     */
    public String toString()
    {
        return "AutoStartPermission";
    }
    
    /**
     * Don't allow cloning
     */
    public boolean equals(Object obj)
    {
        return (obj.hashCode() == hashCode());
    }

    /**
     * Not supported
     */
    public int hashCode()
    {
        return 0;
    }

    /**
     * Actions are not supported
     */
    public String getActions()
    {
        return null;
    }

    /**
     * Permission collections not supported
     */
    public PermissionCollection newPermissionCollection()
    {
        return null;
    }
}