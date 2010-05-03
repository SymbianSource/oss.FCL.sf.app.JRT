/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

public class FileRoots
{
    /**
     * Returns string describing current platform.
     */
    public static String getPlatform()
    {
        return "linux";
    }

    /**
     * Returns the folder that contains the Java applications
     * to be preinstalled in Linux.
     * Currently preinstallation is supported only in
     * S60.
     */
    public static String getPreinstallDir()
    {
        return "";
    }

    /**
     * Returns preinstallation drive for given filename.
     * Filename must contain full path including drive letter.
     */
    public static int getPreinstallDrive(String aFilename)
    {
        // Drive letters are not used in Linux.
        return -1;
    }

    /**
     * Returns directory where the icons registered to
     * platform should be copied in linux.
     */
    public static String getRegisteredIconDir(int aDrive)
    {
        return getResourceDir();
    }

    /**
     * Returns the directory that contains the resources
     * needed by Java Installer, e.g. 'trusted' icon
     */
    public static String getResourceDir()
    {
        return System.getProperty("JAVA_BIN_ROOT") + "/resources/";
    }

    /**
     * Returns root folder for MIDP runtime in linux.
     */
    public static String getMidpRoot()
    {
        return System.getProperty("JAVA_BIN_ROOT");
    }

    /**
     * Returns root folder for applications in linux.
     */
    static String getAppsRoot()
    {
        return getMidpRoot() + "apps/";
    }

    /**
     * Returns installer root folder in linux.
     */
    static String getInstallerRoot()
    {
        return getMidpRoot() + "installer/";
    }

}
