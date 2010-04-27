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

package com.nokia.mj.impl.security.midp.common;

import com.nokia.mj.impl.security.common.PermissionBase;

/**
 * Encapsulation of a user permission. A user permission is a MIDP permission
 * with settings and optionally prompt details
 */
public class UserPermission
        extends MIDPPermission
{
    /**
     * The class encapsulating the prompt details
     */
    private PermissionBase securityPromptDetails;

    private UserSecuritySettings userSecuritySettings;

    public UserPermission(
        String name,
        String target,
        String actionList,
        UserSecuritySettings userSecuritySettings)
    {
        this(name, target, actionList, userSecuritySettings, null);
    }

    public UserPermission(
        String name,
        String target,
        String actionList,
        UserSecuritySettings userSecuritySettings,
        PermissionBase securityPromptDetails)
    {
        super(name, target, actionList);
        this.userSecuritySettings = userSecuritySettings;
        this.securityPromptDetails = securityPromptDetails;
    }

    public UserSecuritySettings getUserSecuritySettings()
    {
        return userSecuritySettings;
    }

    public PermissionBase getPromptDetails()
    {
        return securityPromptDetails;
    }

    public boolean equals(Object o)
    {
        if (!(o instanceof UserPermission))
        {
            return false;
        }
        if (this == o)
        {
            return true;
        }
        UserPermission p = (UserPermission)o;
        return (super.equals((MIDPPermission)o)
                && ((this.userSecuritySettings == null
                     && p.getUserSecuritySettings() == null)
                    || (this.userSecuritySettings != null
                        && this.userSecuritySettings.equals(
                            p.getUserSecuritySettings()))));
    }

    public String toString()
    {
        return super.toString()
               + (userSecuritySettings != null ? userSecuritySettings.toString(): "");
    }

}
