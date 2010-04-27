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
 * Encapsulation of an optional permission attribute
 */
public class OptionalPermissionAttribute
        extends PermissionAttribute
{
    OptionalPermissionAttribute(String attrName, String attrValue)
    {
        super(attrName, attrValue);
    }

    OptionalPermissionAttribute(String attrValue)
    {
        super(OPTIONAL_LEGACY_ATTRIBUTE_NAME, attrValue);
    }

    public static PermissionAttribute[] HandleLegacyAttribute(String attrValue)
    {
        if (attrValue == null)
        {
            return null;
        }
        OptionalPermissionAttribute[] optPerms = null;
        MIDPPermission[] perms = InitFromLegacyAttribute(
                                     OPTIONAL_LEGACY_ATTRIBUTE_NAME, attrValue, true /* ignore unknown values */);
        if (perms != null)
        {
            optPerms = new OptionalPermissionAttribute[perms.length];
            for (int i=0; i<optPerms.length; i++)
            {
                optPerms[i] = new OptionalPermissionAttribute(perms[i]);
            }
        }
        return optPerms;
    }

    public int getImportance()
    {
        return OPTIONAL_PERMISSION;
    }
    public String toString()
    {
        StringBuffer sb = new StringBuffer();
        sb.append("OPTIONAL\n");
        sb.append("Name: "+getName()+ "\n");
        sb.append("Target: "+getTarget()+ "\n");
        sb.append("ActionList: "+getActionList()+ "\n");
        return sb.toString();
    }


    private OptionalPermissionAttribute(MIDPPermission permission)
    {
        super(permission.getName(),
              permission.getTarget(),
              permission.getActionList(),
              OPTIONAL_LEGACY_ATTRIBUTE_NAME);
    }
}
