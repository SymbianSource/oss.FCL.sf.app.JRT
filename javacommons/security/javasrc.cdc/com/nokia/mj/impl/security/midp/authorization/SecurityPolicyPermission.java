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

package com.nokia.mj.impl.security.midp.authorization;

import com.nokia.mj.impl.security.midp.common.PolicyBasedPermission;
import com.nokia.mj.impl.security.midp.common.UserSecuritySettings;
import java.io.ByteArrayOutputStream;
import java.io.IOException;

/**
 * An implementation of a policy based permission which is read/flatten
 * from/into the security policy file
 */
public class SecurityPolicyPermission
        implements PolicyBasedPermission
{
    private int type;

    /**
     * The name of the permission
     */
    private String name = null;

    /**
     * The target of the permission
     */
    private String target = null;

    /**
     * The action list
     */
    private String actionList = null;

    private SecurityPolicyPermissionSettings settings;

    public SecurityPolicyPermission(
        String name,
        String target,
        String actionList,
        int type,
        SecurityPolicyPermissionSettings settings)
    {
        this.name = name;
        this.target = target;
        this.actionList = actionList;
        this.type = type;
        this.settings = settings;
    }

    public SecurityPolicyPermission(
        String name,
        String target,
        String actionList,
        SecurityPolicyPermissionSettings settings)
    {
        this(name, target, actionList, USER_TYPE, settings);
    }

    public SecurityPolicyPermission(
        String name,
        String target,
        String actionList,
        int type)
    {
        this(name, target, actionList, type, null);
    }

    public int getType()
    {
        return type;
    }

    public UserSecuritySettings getUserSecuritySettings()
    {
        return settings;
    }

    public String getName()
    {
        return name;
    }

    public String getTarget()
    {
        return target;
    }

    public String getActionList()
    {
        return actionList;
    }

    public boolean equals(Object o)
    {
        if (!(o instanceof SecurityPolicyPermission))
        {
            return false;
        }
        if (this == o)
        {
            return true;
        }
        SecurityPolicyPermission p = (SecurityPolicyPermission)o;
        // check everything else but the settings
        if ((this.name == null && p.getName() != null)
                || (this.name != null && !this.name.equals(p.getName()))
                || (this.target == null && p.getTarget() != null)
                || (this.target != null && !this.target.equals(p.getTarget()))
                || (this.actionList == null && p.getActionList() != null)
                || (this.actionList != null
                    && !this.actionList.equals(p.getActionList())))
        {
            return false;
        }
        // check the settings as well
        return (((this.settings == null
                  && p.getUserSecuritySettings() == null)
                 || (this.settings != null
                     && this.settings.equals(
                         p.getUserSecuritySettings()))));

    }

    public byte[] getBytes() throws IOException
    {
        ByteArrayOutputStream oStream = new ByteArrayOutputStream();
        oStream.write(type);
        oStream.write(name.length());
        oStream.write(name.getBytes());
        writeString(target, oStream);
        writeString(actionList, oStream);
        if (settings != null)
        {
            oStream.write(settings.getBytes());
        }
        return oStream.toByteArray();
    }

    public static SecurityPolicyPermission getFromBytes(byte[] buf, boolean activeSettings)
    {
        int type = buf[SecurityPolicy.index];
        SecurityPolicy.index++;
        int nameLen = buf[SecurityPolicy.index];
        SecurityPolicy.index++;
        String name = new String(buf, SecurityPolicy.index, nameLen);
        SecurityPolicy.index += nameLen;
        String target = readString(buf);
        String actionList = readString(buf);
        if (type == USER_TYPE || type == USER_ASSIGNED_TYPE)
        {
            return new SecurityPolicyPermission(
                name, target, actionList, type,
                SecurityPolicyPermissionSettings.getFromBytes(
                buf, activeSettings));
        }
        return new SecurityPolicyPermission(name, target, actionList, type);
    }

    private void writeString(String str, ByteArrayOutputStream oStream) throws IOException
    {
        if (str == null)
        {
            oStream.write(0);
        }
        else
        {
            oStream.write(str.length());
            oStream.write(str.getBytes());
        }
    }

    private static String readString(byte[] buf)
    {
        String str = null;
        int strLen = buf[SecurityPolicy.index];
        SecurityPolicy.index++;
        if (strLen != 0)
        {
            str = new String(buf, SecurityPolicy.index, strLen);
            SecurityPolicy.index += strLen;
        }
        return str;
    }

}
