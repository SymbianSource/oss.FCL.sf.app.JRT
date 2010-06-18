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

package com.nokia.mj.impl.rt.unittestrunner;

import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.utils.Uid;

public class ApplicationInfoImpl extends ApplicationInfo
{
    private String iProtectionDomain = null;

    /**
     */
    public String getRuntimeType()
    {
        // See runtime types defined in ApplicationInfo class.
        return "UnitTest_Runner";
    }

    /**
     */
    public String getProtectionDomain()
    {
        return MANUFACTURER_DOMAIN;
    }

    /**
     */
    public Uid getUid()
    {
        return Uid.createUid("[ffffffff]");
    }

    /**
     */
    public Uid getSuiteUid()
    {
        throw new RuntimeException
        ("Get suite uid is not applicable for TckRunner");
    }

    /**
     */
    public String getSuiteName()
    {
        return getName();
    }

    /**
     */
    public String getName()
    {
        return "JavaUnitTester";
    }

    /**
     */
    public String getVendor()
    {
        return "Nokia";
    }

    /**
     */
    public String getVersion()
    {
        return "1.0.0";
    }

    /**
     */
    public String getRootPath()
    {
        return ".";
    }

    /**
     */
    public String getMainClass()
    {
        throw new RuntimeException
        ("Get main class is not applicable for UntiTestRunner");
    }

    /**
     */
    public String getAttribute(String key)
    {
        return null;
    }
}
