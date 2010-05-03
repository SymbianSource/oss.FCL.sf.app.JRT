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

/**
 * Encapsulation of a mandatory permission attribute
 */
public class MandatoryPermissionAttribute
        extends PermissionAttribute
{
    /**
     * Constructor for legacy attributes
     */
    MandatoryPermissionAttribute()
    {
        super(MANDATORY_LEGACY_ATTRIBUTE_NAME);
    }

    MandatoryPermissionAttribute(String attrName, String attrValue)
    {
        super(attrName, attrValue);
    }

    MandatoryPermissionAttribute(String attrValue)
    {
        super(MANDATORY_LEGACY_ATTRIBUTE_NAME, attrValue);
    }

    public static PermissionAttribute[] HandleLegacyAttribute(String attrValue)
    {
        if (attrValue == null)
        {
            return null;
        }
        MandatoryPermissionAttribute[] mandatoryPerms = null;
        MIDPPermission[] perms = InitFromLegacyAttribute(
                                     MANDATORY_LEGACY_ATTRIBUTE_NAME, attrValue, false /* ignore unknown values */);
        if (perms != null)
        {
            mandatoryPerms = new MandatoryPermissionAttribute[perms.length];
            for (int i=0; i<mandatoryPerms.length; i++)
            {
                mandatoryPerms[i] = new MandatoryPermissionAttribute(perms[i]);
            }
        }
        return mandatoryPerms;
    }

    public int getImportance()
    {
        return MANDATORY_PERMISSION;
    }

    public String toString()
    {
        StringBuffer sb = new StringBuffer();
        sb.append("MANDATORY\n");
        sb.append("Name: "+getName()+ "\n");
        sb.append("Target: "+getTarget()+ "\n");
        sb.append("ActionList: "+getActionList()+ "\n");
        return sb.toString();
    }

    private MandatoryPermissionAttribute(MIDPPermission permission)
    {
        super(permission.getName(),
              permission.getTarget(),
              permission.getActionList(),
              MANDATORY_LEGACY_ATTRIBUTE_NAME);
    }
}
