/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Permission object for managing access to command line API feature
*
*/



package com.nokia.mj.impl.rt.utils;

import java.security.Permission;
import java.security.PermissionCollection;
import com.nokia.mj.impl.security.common.PermissionBase;
import com.nokia.mj.impl.security.utils.SecurityPromptMessage;

/**
 * Permission class representing the token access to command line API feature.
 *
 */
public class CmdLineArgsPermission extends PermissionBase
{
    /**
     * Constructor
     *
     */
    public CmdLineArgsPermission()
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
        return (SecurityPromptMessage.getInstance()).getText(
            SecurityPromptMessage.QUESTION_ID_APP_AUTO_LAUNCH,
            null);
    }

    /**
     * Only implies permission objects of same instance
     */
    public boolean implies(Permission p)
    {
        if (p instanceof CmdLineArgsPermission)
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
        return "com.nokia.webstart";
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