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

package com.nokia.mj.impl.file;

import java.util.Hashtable;
import java.util.Vector;
import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.fileutils.DriveInfo;
import com.nokia.mj.impl.fileutils.DriveUtilities;


public class FileAccessHelper implements FileConstants
{
    public static int PATHS_EQUAL = 0;
    public static int PATHS_NO_MATCH = 1;
    public static int PATH_ABOVEIN_HIERARCHY = 2;
    public static int PATH_BELOWIN_HIERARCHY = 3;
    private static String PATH_PRIVATE = "private";
    private static String PATH_SYSTEM = "system";
    private static String[] iRestrictedPathList;
    private static String[] iForbiddenPathList;

    // getMidpRoot get the midp private directory such as "/private/102033E6"
    private static String iMidpRoot = FileSystemUtils.getMidpRoot().toLowerCase();

    /**
     * Checks to see if the application has access to a specific path.
     *
     * @param aPath
     *            path which the application is trying to access.
     * @param aDomain
     *            domain of the application
     * @param aIsOpening
     *            if the operation being performed is equivalent to opening a
     *            connection.
     * @return true in case access is allowed. False otherwise
     */
    public static boolean accessAllowed(String aPath, String aDomain)
    {
        FileLogger.Log("FileAccessHelper.accessAllowed: Checking access: \n");
               
        if(aPath.endsWith("/") == false)
            aPath += "/";

        if (isHomeDir(aPath))
        {
            return true;
        }

        // The basic assumption for this check is 
        // always drive name is a single character.
        if (aPath.substring(2).toLowerCase().startsWith(iMidpRoot))
        {
            // Allowed only for Manufacturer domain.
            if (aDomain.equals(ApplicationInfo.MANUFACTURER_DOMAIN) == false)
            {
                return false;
            }
            
        }

        return true;
    }

    /**
     * Checks to see if the specified path is same as application's private
     * directory.
     */
    private static boolean isHomeDir(String aPath)
    {
        String appPrivateDir = FileSystemUtils.getAppPrivateDir();

        if (aPath.toLowerCase().startsWith(appPrivateDir.toLowerCase()))
        {
            return true;
        }

        return false;
    }
}
