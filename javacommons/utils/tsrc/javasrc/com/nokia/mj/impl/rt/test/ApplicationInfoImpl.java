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

package com.nokia.mj.impl.rt.test;

import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.rt.support.ApplicationInfo;

/**
 *
 * @author Nokia Corporation
 * @version 1.0
 */

public class ApplicationInfoImpl extends ApplicationInfo
{
    public static final String TEST_RT_TYPE = "TEST RT";
    public static final String TEST_PROTECTION_DOMAIN = "TEST DOMAIN";
    public static final Uid    TEST_APP_UID = Uid.createUid("[12345678]");
    public static final Uid    TEST_APP_SUITE_UID = Uid.createUid("[87654321]");
    public static final String TEST_APP_NAME = "TEST NAME";
    public static final String TEST_APP_SUITE_NAME = "TEST SUITE NAME";
    public static final String TEST_APP_VENDOR = "TEST VENDOR";
    public static final String TEST_APP_VERSION = "TEST VERSION";
    public static final String TEST_PATH = "TEST PATH";
    public static final String MAIN_CLASS = "TEST CLASS";
    public static final String TEST_KEY = "TEST KEY";
    public static final String TEST_VALUE = "TEST VALUE";

    public String getRuntimeType()
    {
        return TEST_RT_TYPE;
    }

    public String getProtectionDomain()
    {
        return TEST_PROTECTION_DOMAIN;
    }

    public Uid getUid()
    {
        return TEST_APP_UID;
    }

    public Uid getSuiteUid()
    {
        return TEST_APP_SUITE_UID;
    }

    public String getSuiteName()
    {
        return TEST_APP_SUITE_NAME;
    }

    public String getName()
    {
        return TEST_APP_NAME;
    }

    public String getVendor()
    {
        return TEST_APP_VENDOR;
    }

    public String getVersion()
    {
        return TEST_APP_VERSION;
    }

    public String getRootPath()
    {
        return TEST_PATH;
    }

    public String getMainClass()
    {
        return MAIN_CLASS;
    }

    public String getAttribute(String key)
    {
        if (TEST_KEY.equals(key))
        {
            return TEST_VALUE;
        }
        return null;
    }
}
