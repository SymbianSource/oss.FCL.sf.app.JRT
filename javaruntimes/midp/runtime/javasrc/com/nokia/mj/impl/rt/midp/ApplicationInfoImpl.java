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

package com.nokia.mj.impl.rt.midp;

import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.rt.support.ApplicationInfo;

/**
 * A MIDP runtime specific implementation of ApplicationInfo class of the
 * runtime support API.
 */

public class ApplicationInfoImpl extends ApplicationInfo
{
    /**
     * For storing the MIDlet specific info.
     */
    private static MidletInfo sMidletInfo;

    /*** ----------------------------- PACKAGE ---------------------------- */

    /**
     * A static setter for setting MidletInfo instance.
     * @param midletInfo Midlet info to be set.
     */
    static void setMidletInfo(MidletInfo midletInfo)
    {
        sMidletInfo = midletInfo;
    }

    /**
     * A static getter for getting the MidletInfo instance.
     * @return The stored instnce of MidletInfo class.
     */
    static MidletInfo getMidletInfo()
    {
        return sMidletInfo;
    }

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * @see com.nokia.mj.impl.rt.support.ApplicationInfo#
     *      getRuntimeType().
     */
    public String getRuntimeType()
    {
        return ApplicationInfo.RUNTIME_MIDP;
    }

    /**
     * @see com.nokia.mj.impl.rt.support.ApplicationInfo#
     *      getProtectionDomain().
     */
    public String getProtectionDomain()
    {
        return sMidletInfo.getProtectionDomain();
    }

    /**
     * @see com.nokia.mj.impl.rt.support.ApplicationInfo#
     *      getUid().
     */
    public Uid getUid()
    {
        return sMidletInfo.getUid();
    }

    /**
     * @see com.nokia.mj.impl.rt.support.ApplicationInfo#
     *      getSuiteUid().
     */
    public Uid getSuiteUid()
    {
        return sMidletInfo.getSuiteUid();
    }

    /**
     * @see com.nokia.mj.impl.rt.support.ApplicationInfo#
     *      getSuiteName().
     */
    public String getSuiteName()
    {
        return sMidletInfo.getSuiteName();
    }

    /**
     * @see com.nokia.mj.impl.rt.support.ApplicationInfo#
     *      getName().
     */
    public String getName()
    {
        return sMidletInfo.getName();
    }

    /**
     * @see com.nokia.mj.impl.rt.support.ApplicationInfo#
     *      getLocalizedName().
     */
    public String getLocalizedName()
    {
        return sMidletInfo.getLocalizedName();
    }

    /**
     * @see com.nokia.mj.impl.rt.support.ApplicationInfo#
     *      getVendor().
     */
    public String getVendor()
    {
        return sMidletInfo.getVendor();
    }

    /**
     * @see com.nokia.mj.impl.rt.support.ApplicationInfo#
     *      getVersion().
     */
    public String getVersion()
    {
        return sMidletInfo.getVersion();
    }

    /**
     * @see com.nokia.mj.impl.rt.support.ApplicationInfo#
     *      getRootPath().
     */
    public String getRootPath()
    {
        return sMidletInfo.getRootPath();
    }

    /**
     * @see com.nokia.mj.impl.rt.support.ApplicationInfo#
     *      getMainClass().
     */
    public String getMainClass()
    {
        return sMidletInfo.getMainClass();
    }

    /**
     * @see com.nokia.mj.impl.rt.support.ApplicationInfo#
     *      getAttribute(java.lang.String).
     */
    public String getAttribute(String key)
    {
        return sMidletInfo.getAttribute(key);
    }
}
