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

package com.nokia.mj.impl.rt.midptest;

import java.security.Permission;
import java.security.AccessControlException;

import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.rt.support.PromptTextProvider;

/**
 * @author Nokia Corporation
 * @version 1.0
 */
public class ApplicationUtilsImpl extends ApplicationUtils
{
    public static void doShutdownImpl()
    {
//        sInstance.doShutdown();
        ((ApplicationUtilsImpl)sInstance).doShutdown();
    }


    public void notifyExitCmd()
    {
        System.out.println("(test) ApplicationUtilsImpl.notifyExitCmd()");
    }

    public void checkPermission(Permission p)
    throws AccessControlException,
                NullPointerException
    {
        System.out.println("(test) ApplicationUtilsImpl.checkPermission()");
    }

    public void checkPermission(Permission p, String textPrompt)
    throws AccessControlException,
                NullPointerException
    {
        System.out.println("(test) ApplicationUtilsImpl.checkPermission(2)");
        throw new AccessControlException("Denied ");
    }

    public void checkPermission(Permission p, PromptTextProvider provider)
    throws AccessControlException,
                NullPointerException
    {
        System.out.println("(test) ApplicationUtilsImpl.checkPermission(3)");
    }
}
