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
* Description:
*
*/

package com.nokia.mj.impl.rt.javacontrolpanel;

import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.utils.Logger;

import java.security.Permission;
import java.security.AccessControlException;

/**
 * @author Nokia Corporation
 * @version 1.0
 */
public class ApplicationUtilsImpl extends ApplicationUtils
{
    public static void doShutdownImpl()
    {
        // Send shutdown notification to all registered listeners.
        ((ApplicationUtilsImpl)sInstance).doShutdown();
    }


    public void notifyExitCmd()
    {
        Logger.ILOG(Logger.EUtils, "notifyExitCmd() - exiting");
        System.exit(0);
    }

    public void checkPermission(Permission aPermission)
    throws AccessControlException, NullPointerException
    {
        checkPermission(null, aPermission);
    }

    public void checkPermission(Uid aAppUid, Permission aPermission)
    throws AccessControlException, NullPointerException
    {
        return;
    }
}
