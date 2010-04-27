/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
package com.nokia.mj.impl.rt.legacy;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.security.midp.common.PermissionMappingTable;
import com.nokia.mj.impl.security.midp.common.MIDPPermission;
import com.nokia.mj.impl.security.midp.authorization.ClassInstantiator;
import com.nokia.mj.impl.utils.Logger;
import java.security.Permission;

public final class Security
{
    public static void ensurePermission(String aPermission, String aOperation, String[] aArgs)
    throws
                SecurityException
    {
        if (aPermission != null)
        {
            MIDPPermission internalPermission = PermissionMappingTable.get(
                                                    aPermission);
            if (internalPermission != null)
            {
                // make an instance of the internal permission
                try
                {
                    Permission permission = (Permission)ClassInstantiator
                                            .newInstance(internalPermission.getName(),
                                                         internalPermission.getTarget(),
                                                         internalPermission.getActionList());
                    ApplicationUtils appUtils = ApplicationUtils.getInstance();
                    appUtils.checkPermission(permission);
                    return;
                }
                catch (InstantiationException e)
                {
                    // fall-through
                }
            }
        }
        throw new SecurityException("Permission " + aPermission + " not allowed");
    }

}
