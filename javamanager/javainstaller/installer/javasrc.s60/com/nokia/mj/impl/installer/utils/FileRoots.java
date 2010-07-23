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

    private static int iResourceDriveNumber = -1;

    /**
     * Returns string describing current platform.
     */
    public static String getPlatform()
    {
        return "s60";
    }

    /**
     * Returns the directory that contains the Java applications
     * to be preinstalled in S60.
     */
    public static String getPreinstallDir()
    {
        return "\\resource\\java\\preinstall\\";
    }

    /**
     * Returns preinstallation drive for given filename.
     * Filename must contain full path including drive letter.
     */
    public static int getPreinstallDrive(String aFilename)
    {
        int drive = FileUtils.getDrive(aFilename);
        if (SysUtil.isDriveReadOnly(drive))
        {
            // if the drive is ReadOnly drive (e.g. Z:),
            // use system default installation drive (usually C:)
            drive = SysUtil.getDefaultPhoneMemory();
        }
        return drive;
    }

    /**
     * Returns directory where the icons registered to
     * platform should be copied in S60.
     */
    public static String getRegisteredIconDir(int aDrive)
    {
        String result = getAppsRoot();
        result = FileUtils.setDrive(result, aDrive);
        String replace = "\\private\\";
        int i = result.indexOf(replace);
        if (i == -1)
        {
            return FileUtils.getDriveName(aDrive) + ":\\data\\java\\";
        }
        return result.substring(0, i) + "\\public\\" +
            result.substring(i + replace.length());
    }

    /**
     * Returns the directory that contains the resources
     * needed by Java Installer, e.g. 'trusted' icon
     */
    public static String getResourceDir()
    {
        return FileUtils.setDrive(
                   System.getProperty("JAVA_RES_ROOT"), getResourceDrive());
    }

    /**
     * Returns root folder for MIDP runtime in S60.
     */
    public static String getMidpRoot()
    {
        int resourceDrive = getResourceDrive();
        if (resourceDrive == 25)
        {
            // If MIDP is installed to Z: drive,
            // use C: drive as MIDP root.
            resourceDrive = 2;
        }
        String midpRoot = _getMidpRoot();
        return ((char)(resourceDrive + 'A')) + ":" + midpRoot;
    }

    /**
     * Returns root folder for applications in S60.
     */
    static String getAppsRoot()
    {
        // Note that drive letter does not matter here,
        // installer will replace it with correct one.
        return getMidpRoot() + "apps\\";
    }

    /**
     * Returns installer root folder in S60.
     */
    static String getInstallerRoot()
    {
        return getMidpRoot() + "installer\\";
    }

    /**
     * Returns the drive where OMJ was installed.
     */
    private static int getResourceDrive()
    {
        if (iResourceDriveNumber == -1)
        {
            iResourceDriveNumber = _getResourceDrive();
        }
        return iResourceDriveNumber;
    }

    /*** ----------------------------- NATIVE ----------------------------- */

    /**
     * Returns the drive number of the drive where the installer resources are.
     * In case of error logs the error and returns default drive number
     * that can be build specific but that is usually 2 (C: drive)
     *
     * @return the drive (0..25) where the installer resources are
     */
    private static native int _getResourceDrive();

    /**
     * Returns the path (without drive letter) to the MIDP runtime directory,
     * for example \private\102033E6\
     *
     * @return root directory where OMJ binary has been installed to
     */
    private static native String _getMidpRoot();
}
