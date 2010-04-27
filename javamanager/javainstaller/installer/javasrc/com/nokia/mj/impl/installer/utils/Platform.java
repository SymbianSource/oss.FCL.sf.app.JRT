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

package com.nokia.mj.impl.installer.utils;

import com.nokia.mj.impl.utils.Version;

public class Platform
{
    private static String iPlatform = null;
    private static boolean iIsS60 = false;
    private static boolean iIsLinux = false;

    public static boolean isS60()
    {
        if (iPlatform == null)
        {
            setPlatform(FileRoots.getPlatform());
        }
        return iIsS60;
    }

    public static boolean isLinux()
    {
        if (iPlatform == null)
        {
            setPlatform(FileRoots.getPlatform());
        }
        return iIsLinux;
    }

    public static Version getPlatformVersion()
    {
        String platform = System.getProperty("microedition.platform");
        String platVerKey = "sw_platform_version";
        int idx = -1;
        if (platform != null)
        {
            idx = platform.indexOf(platVerKey);
        }
        if (idx == -1)
        {
            Log.log("Version.getPlatformVersion(): System property " +
                    "microedition.platform does not contain sw_platform_version");
            // Returns the version of the oldest platform that can contain
            // Jrt 2
            return new Version(5,0,0);
        }
        // Parsing string "<...>sw_platform_version=X.Y;<...>
        int idxEquals = platform.indexOf('=', idx + platVerKey.length());
        int idxSemicolon = platform.indexOf(';', idxEquals);

        return Version.getVersion(platform.substring(idxEquals + 1, idxSemicolon));
    }

    private static void setPlatform(String aPlatform)
    {
        iPlatform = aPlatform;
        if (iPlatform.equalsIgnoreCase("s60"))
        {
            iIsS60 = true;
            iIsLinux = false;
        }
        else
        {
            iIsS60 = false;
            iIsLinux = true;
        }
    }
}
