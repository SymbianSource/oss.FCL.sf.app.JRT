/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.mj.impl.rt.installer;

import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.PlatformUid;
import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.utils.Uid;

/**
 * @author Nokia Corporation
 * @version 1.0
 */

public class ApplicationInfoImpl extends ApplicationInfo
{
    private String iProtectionDomain = null;

    /**
     */
    public String getRuntimeType()
    {
        return ApplicationInfo.RUNTIME_INSTALLER;
    }

    /**
     */
    public String getProtectionDomain()
    {
        if (iProtectionDomain == null)
        {
            return UNIDENTIFIED_THIRD_PARTY_DOMAIN;
        }
        return iProtectionDomain;
    }

    /**
     */
    public void setProtectionDomain(String aProtectionDomain)
    {
        iProtectionDomain = aProtectionDomain;
    }

    /**
     */
    public Uid getUid()
    {
        return PlatformUid.createUid("0x2001843A"); // JavaInstaller uid.
    }

    /**
     */
    public Uid getSuiteUid()
    {
        // Return JavaInstaller application uid also as suite uid.
        return getUid();
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
        return "JavaInstaller";
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
        return "1.0.1";
    }

    /**
     */
    public String getRootPath()
    {
        return FileUtils.getInstallerRoot();
    }

    /**
     */
    public String getMainClass()
    {
        throw new RuntimeException
        ("Get main class is not applicable for JavaInstaller");
    }

    /**
     */
    public String getAttribute(String key)
    {
        return null;
    }
}
