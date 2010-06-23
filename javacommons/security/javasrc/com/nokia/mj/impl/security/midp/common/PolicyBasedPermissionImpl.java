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
 * It defines three types of permissions: user permission, allowed or
 * assigned permissions
 */
public class PolicyBasedPermissionImpl
        extends UserPermission
        implements PolicyBasedPermission
{
    private int type;

    public PolicyBasedPermissionImpl(
        String name,
        String target,
        String actionList,
        int type,
        UserSecuritySettings userSettings,
        PermissionBase securityPromptDetails)
    {
        super(name, target, actionList, userSettings, securityPromptDetails);
        this.type = type;
    }

    public PolicyBasedPermissionImpl(
        String name,
        String target,
        String actionList,
        int type,
        UserSecuritySettings userSettings)
    {
        this(name, target, actionList, type, userSettings, null);
    }

    public PolicyBasedPermissionImpl(PolicyBasedPermission p)
    {
        this(p.getName(), p.getTarget(), p.getActionList(), p.getType(),
             (p.getUserSecuritySettings() != null ?
              new UserSecuritySettingsImpl(p.getUserSecuritySettings().getName(),
                                           p.getUserSecuritySettings().getCurrentInteractionMode(),
                                           p.getUserSecuritySettings().getAllowedInteractionModes()) : null),
             null);
    }

    public PolicyBasedPermissionImpl(
        String name,
        String target,
        String actionList,
        UserSecuritySettings userSettings)
    {
        this(name, target, actionList, USER_TYPE, userSettings, null);
    }


    public PolicyBasedPermissionImpl(
        String name,
        String target,
        String actionList,
        int type)
    {
        this(name, target, actionList, type, null, null);
    }

    public int getType()
    {
        return type;
    }

    public boolean equals(Object o)
    {
        if (!(o instanceof PolicyBasedPermissionImpl))
        {
            return false;
        }
        if (this == o)
        {
            return true;
        }
        PolicyBasedPermissionImpl p = (PolicyBasedPermissionImpl)o;
        return (super.equals((UserPermission)o));
    }

    public String toString()
    {
        StringBuffer sb = new StringBuffer();
        sb.append(" Permission type:" + type);
        return super.toString() + sb.toString();
    }

}
