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


package com.nokia.mj.impl.utils;

/**
 * Class which provides utility functions for handling Java resources
 *
 * @author Nokia Corporation
 * @version 1.0
 */
public class ResourceUtil
{

    /*** ----------------------------- PUBLIC ------------------------------ */

    /** Name for icon which is used to indicate trusted application. */
    public static final String TRUSTED_ICON_NAME = "java_trusted.png";
    /** Name for icon which is used to indicate untrusted application. */
    public static final String UNTRUSTED_ICON_NAME = "java_untrusted.png";

    /**
     * Get full path to platform specific Java resource dir.
     * There can be more than one resource dirs and they
     * are indexed starting from zero.
     *
     * @param aIndex index for the resource dir
     * @return resource dir, or null if resource dir for given
     * index does not exist
     */
    public static String getResourceDir(int aIndex)
    {
        if (aIndex < 0)
        {
            return null;
        }
        String resourceDir = null;
        String osName = System.getProperty("os.name");
        if (osName != null && osName.toLowerCase().startsWith("linux"))
        {
            // Linux has only one resource dir.
            if (aIndex == 0)
            {
                resourceDir = System.getProperty("JAVA_RES_ROOT");
            }
        }
        else
        {
            // S60 resource dirs.
            switch (aIndex)
            {
            case 0:
                // C-drive
                resourceDir = setDrive(System.getProperty("JAVA_RES_ROOT"), 2);
                break;
            case 1:
                // Z-drive
                resourceDir = setDrive(System.getProperty("JAVA_RES_ROOT"), 25);
                break;
            }
        }
        return resourceDir;
    }

    /*** ---------------------------- PROTECTED --------------------------- */
    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */

    /**
     * Returns given filename from given drive, or unmodified filename
     * if drive id is -1.
     */
    private static String setDrive(String aFilename, int aDrive)
    {
        if (aFilename != null && aDrive >= 0)
        {
            if (aFilename.charAt(1) == ':')
            {
                aFilename = (char)('a' + aDrive) + aFilename.substring(1);
            }
        }
        return aFilename;
    }

    /*** ----------------------------- NATIVE ----------------------------- */
}
