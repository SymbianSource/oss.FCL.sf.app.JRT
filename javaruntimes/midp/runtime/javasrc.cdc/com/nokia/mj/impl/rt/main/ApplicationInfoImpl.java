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

package com.nokia.mj.impl.rt.main;

import java.util.Hashtable;

import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.rt.support.ApplicationInfo;

/**
 * A Main runtime specific implementation of ApplicationInfo class of the
 * runtime support API.
 */

public class ApplicationInfoImpl extends ApplicationInfo
{
    /**
     * For storing the MIDlet specific info.
     */
    private static Uid       sAppUid;
    private static String    sAppName;
    private static Hashtable sManifestArgs;
    private static String    sRootPath;

    /*** ----------------------------- PACKAGE ---------------------------- */

    static void setUid(Uid appUid)
    {
        sAppUid = appUid;
    }

    static void setName(String appName)
    {
        sAppName = appName;
    }

    static void setManifestArgs(Hashtable manifestArgs)
    {
        sManifestArgs = manifestArgs;
    }
    static void setRootPath(String rootPath)
    {
        sRootPath = rootPath;
    }

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * @see com.nokia.mj.impl.rt.support.ApplicationInfo#
     *      getRuntimeType().
     */
    public String getRuntimeType()
    {
        return ApplicationInfo.RUNTIME_NON_MANAGED;
    }

    /**
     * @see com.nokia.mj.impl.rt.support.ApplicationInfo#
     *      getProtectionDomain().
     */
    public String getProtectionDomain()
    {
        return ApplicationInfo.MANUFACTURER_DOMAIN;
    }

    /**
     * @see com.nokia.mj.impl.rt.support.ApplicationInfo#
     *      getUid().
     */
    public Uid getUid()
    {
        return sAppUid;
    }

    /**
     * @see com.nokia.mj.impl.rt.support.ApplicationInfo#
     *      getSuiteUid().
     */
    public Uid getSuiteUid()
    {
        return sAppUid;
    }

    /**
     * @see com.nokia.mj.impl.rt.support.ApplicationInfo#
     *      getSuiteName().
     */
    public String getSuiteName()
    {
        return sAppName;
    }

    /**
     * @see com.nokia.mj.impl.rt.support.ApplicationInfo#
     *      getName().
     */
    public String getName()
    {
        return sAppName;
    }

    /**
     * @see com.nokia.mj.impl.rt.support.ApplicationInfo#
     *      getVendor().
     */
    public String getVendor()
    {
        throw new RuntimeException("getVendor() is not supported!");
    }

    /**
     * @see com.nokia.mj.impl.rt.support.ApplicationInfo#
     *      getVersion().
     */
    public String getVersion()
    {
        throw new RuntimeException("getVendor() is not supported!");
    }

    /**
     * @see com.nokia.mj.impl.rt.support.ApplicationInfo#
     *      getRootPath().
     */
    public String getRootPath()
    {
        return sRootPath;
    }

    /**
     * @see com.nokia.mj.impl.rt.support.ApplicationInfo#
     *      getMainClass().
     */
    public String getMainClass()
    {
        throw new RuntimeException("getMainClass() is not supported!");
    }

    /**
     * @see com.nokia.mj.impl.rt.support.ApplicationInfo#
     *      getAttribute(java.lang.String).
     */
    public String getAttribute(String key)
    {
        if (sManifestArgs != null)
        {
            return (String)sManifestArgs.get(key);
        }
        return null;
    }
}
