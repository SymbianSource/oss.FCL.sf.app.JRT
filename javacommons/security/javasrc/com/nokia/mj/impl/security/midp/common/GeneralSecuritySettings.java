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

import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.security.midp.storage.*;
import com.nokia.mj.impl.storage.StorageSession;

public class GeneralSecuritySettings
{
    static
    {
        Jvm.loadSystemLibrary("javasecurity");
    }

    // constants for the security warnings mode
    public static final int UNINITIALIZED_SECURITY_MODE = -1;
    public static final int USER_SECURITY_MODE = 1;
    public static final int DEFAULT_SECURITY_MODE = 2;

    public static void setSecurityWarningsMode(StorageSession aStorageSession,Uid appUid, int securityWarningsMode)
    {
        SecurityStorage storage = new SecurityStorage(aStorageSession);
        try
        {
            storage.writeSecurityWarningsMode(appUid, securityWarningsMode);
        }
        finally
        {
            storage.close();
        }
    }

    public static int getSecurityWarningsMode(StorageSession aStorageSession, Uid appUid)
    {
        SecurityStorage storage = new SecurityStorage(aStorageSession);
        try
        {
            return storage.readSecurityWarningsMode(appUid);
        }
        finally
        {
            storage.close();
        }
    }

    public static String getSecurityPolicy()
    {
        return _getSecurityPolicy();
    }

    public static int getDefaultSecurityWarningsMode()
    {
        int tmp = _getDefaultSecurityWarningsMode();
        if (tmp != USER_SECURITY_MODE && tmp != DEFAULT_SECURITY_MODE)
        {
            tmp = UNINITIALIZED_SECURITY_MODE;
        }
        return tmp;
    }

    private native static String _getSecurityPolicy();
    private native static int _getDefaultSecurityWarningsMode();
}
