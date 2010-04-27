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


package com.nokia.mj.impl.gcf;

import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.utils.Uid;

/**
 * This class provides simple utility methods to show auto-innvocation
 * security prompt to the user.
 */
public final class PushSecurityUtils
{
    /**
     * This operation shows auto-invocation prompt(if needed) to the user.
     * @param aAction Value of this can be staticregistration,dynamicregistration
     *                or autoinvocation.
     * @param aBtDeviceName Name of the bluetooth device(which takes connection).
     *                      Other cases null or empty string is passed as argument.
     * @throws SecurityException if user does not accept auto-launch.
     */
    public static void ensurePermission(String aAction,String aBtDeviceName)
    {
        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        PushRegistryPermissionImpl permissionObj =
            new PushRegistryPermissionImpl("*",aAction);
        permissionObj.mBtDeviceName = aBtDeviceName;
        appUtils.checkPermission(permissionObj);
    }

    /**
     * This operation shows auto-invocation prompt(if needed) to the user.
     * @param aAction Value of this can be staticregistration,dynamicregistration
     *                or autoinvocation.
     * @param aUid UID of the application.
     * @throws SecurityException if user does not accept auto-launch.
     */
    public static void ensurePermission(Uid aUid,String aAction)
    {
        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        PushRegistryPermissionImpl permissionObj =
            new PushRegistryPermissionImpl("*",aAction);
        appUtils.checkPermission(aUid,permissionObj);
    }
}//end class PushSecurityUtils

