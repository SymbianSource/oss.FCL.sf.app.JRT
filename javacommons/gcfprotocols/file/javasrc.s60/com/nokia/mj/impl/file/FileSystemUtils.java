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

import java.util.Vector;
import java.util.Hashtable;

import com.nokia.mj.impl.fileutils.DriveInfo;
import com.nokia.mj.impl.fileutils.DriveUtilities;
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.utils.Tokenizer;
import com.nokia.mj.impl.rt.support.ApplicationInfo;

/**
 * Provides methods to retrieve information that are platform specific. These
 * would include list of roots present and their localized names based on the
 * needs of the platform.
 */
public final class FileSystemUtils
{
    private static String iDefaultRoot;

    private static String iDefaultMemoryCard;

    private static String iDefaultTemporaryDrive;

    private static String iDefaultRomDrive;

    private static String iAppDomain;

    private static ApplicationInfo iAppInfo;

    static
    {
        try
        {
            Jvm.loadSystemLibrary("javafile");
        }
        catch (Exception e)
        {
            FileLogger.ELog("Unable to load javafile native library");
        }

        iDefaultRoot = getDefaultDrive();
        iDefaultTemporaryDrive = getTemporaryDrive();
        iDefaultRomDrive = getRomDrive();
        iDefaultMemoryCard = getMemoryCardDrive();

        // Load application information at startup
        iAppInfo = ApplicationInfo.getInstance();

        // Load application domain
        iAppDomain = iAppInfo.getProtectionDomain();
    }

    public static String getProtectionDomain()
    {
        return iAppDomain;
    }

    /**
     * Lists the currently available drives on the device.
     *
     * @return vector of drives.
     */
    public static Vector listRoots()
    {
        DriveInfo[] drives = DriveUtilities.getAccessibleDrives();

        Vector roots = new Vector();
        for (int index = 0; index < drives.length; index++)
        {
            String toAdd = drives[index].iRootPath.replace('\\', '/');
            if (FileAccessHelper.accessAllowed(toAdd,
                                               FileConstants.INTENT_READ, getProtectionDomain(), false))
            {
                roots.addElement(toAdd);
            }
        }
        return roots;
    }

    public static String getDefaultRoot()
    {
        return iDefaultRoot;
    }

    private static String getDefaultDrive()
    {
        String drive = _getDefaultRootPath();
        drive = drive.replace('\\', '/');
        return drive;
    }

    public static String getTemporaryDrive()
    {
        String drive = _getTemporaryDrivePath();
        drive = drive.replace('\\', '/');
        return drive;
    }

    public static String getRomDrive()
    {
        String drive = _getRomDrivePath();
        drive = drive.replace('\\', '/');
        return drive;
    }

    public static String getMemoryCardDrive()
    {
        String drive = _getMemoryCardDrivePath();
        drive = drive.replace('\\', '/');
        return drive;
    }

    private static boolean isDefaultRoot(String aRoot)
    {
        if (aRoot.equalsIgnoreCase(iDefaultRoot))
        {
            return true;
        }
        return false;
    }

    private static boolean isTemporaryRoot(String aRoot)
    {
        if (aRoot.equalsIgnoreCase(iDefaultTemporaryDrive))
        {
            return true;
        }
        return false;
    }

    private static boolean isRom(String aRoot)
    {
        if (aRoot.equalsIgnoreCase(iDefaultRomDrive))
        {
            return true;
        }
        return false;
    }

    private static boolean isDefaultMemoryCardDrive(String aRoot)
    {
        if (aRoot.equalsIgnoreCase(iDefaultMemoryCard))
        {
            return true;
        }
        return false;
    }

    private static String getRemoteDriveName(final DriveInfo aDrive)
    {
        return FileConnectionTexts.get(FileConnectionTexts.FILE_REMOTEDRIVE_N,
                                       new String[] { "" + aDrive.iRootPath.charAt(0) });
    }

    private static String getMemoryCardDriveName(final DriveInfo aDrive)
    {
        return FileConnectionTexts.get(FileConnectionTexts.FILE_MEMORYCARD_N,
                                       new String[] { "" + aDrive.iRootPath.charAt(0) });
    }

    private static String getInternalDriveName(final DriveInfo aDrive)
    {
        return FileConnectionTexts.get(FileConnectionTexts.FILE_INTERNAL_N,
                                       new String[] { "" + aDrive.iRootPath.charAt(0) });
    }

    /**
     * Gives localized name of a given drive. It is done only in case the drive
     * is one of the default roots present. <br/> C:/ - Internal; D:/ -
     * Temporary; Z: - ROM
     *
     * @param aInfo
     * @return
     */
    private static String getLocalDriveName(final DriveInfo aInfo)
    {
        String driveName = aInfo.iRootPath.replace('\\', '/');
        if (isDefaultRoot(driveName))
        {
            return FileConnectionTexts.get(FileConnectionTexts.FILE_C_DRIVE);
        }

        if (isTemporaryRoot(driveName))
        {
            return FileConnectionTexts.get(FileConnectionTexts.FILE_D_DRIVE);
        }

        if (isRom(driveName))
        {
            return FileConnectionTexts.get(FileConnectionTexts.FILE_Z_DRIVE);
        }
        return null;
    }

    public static Vector getRootNames()
    {
        DriveInfo[] drives = DriveUtilities.getAccessibleDrives();

        // Vector will contain an array of drives that are present and that can
        // be viewed by the application.
        Vector roots = new Vector();

        for (int index = 0; index < drives.length; index++)
        {
            String toAdd = drives[index].iRootPath.replace('\\', '/');
            if (FileAccessHelper.accessAllowed(toAdd,
                                               FileConstants.INTENT_READ, getProtectionDomain(), false))
            {
                roots.addElement(drives[index]);
            }
        }

        Vector rootNames = new Vector();
        for (int index = 0; index < roots.size(); index++)
        {
            DriveInfo info = (DriveInfo) roots.elementAt(index);
            String driveName = info.iRootPath.replace('\\', '/');
            String localName = null;

            // If Remote drive
            if (!info.iIsLocal)
            {
                localName = getRemoteDriveName(info);
            }
            else if (info.iIsRemovable)
            {
                if (isDefaultMemoryCardDrive(driveName))
                {
                    localName = FileConnectionTexts
                                .get(FileConnectionTexts.FILE_MEMORYCARD);
                }
                else
                {
                    localName = getMemoryCardDriveName(info);
                }
            }
            else
                // It is internal drive
            {
                localName = getLocalDriveName(info);
                if (localName == null)
                {
                    localName = getInternalDriveName(info);
                }
            }
            rootNames.addElement(localName);
        }
        return rootNames;
    }

    public static String getPathOfProperty(int aValue)
    {
        return "file:///" + getPathOfProperty(aValue, true);
    }

    public static String getPathOfProperty(int aValue, boolean addRoot)
    {
        String retValue = null;
        int value = aValue;

        retValue = _getPathOfProperty(value, true);

        FileLogger.Log("FileSystemUtils: GetPathOfProperty returned: "
                       + retValue);
        retValue = retValue.replace('\\', '/');

        if (addRoot == false)
        {
            retValue = retValue.substring(2);
        }
        return retValue;
    }

    public static String getLocalizedName(int value)
    {
        String retValue = null;
        String propKey = null;

        switch (value)
        {
        case FileConstants.GRAPHICS_PROPERTY:
            propKey = FileConnectionTexts.FILE_GRAPHICS;
            break;
        case FileConstants.MUSIC_PROPERTY:
            propKey = FileConnectionTexts.FILE_MUSIC;
            break;
        case FileConstants.PHOTOS_PROPERTY:
            propKey = FileConnectionTexts.FILE_IMAGES;
            break;
        case FileConstants.RECORDINGS_PROPERTY:
            propKey = FileConnectionTexts.FILE_RECORDINGS;
            break;
        case FileConstants.TONES_PROPERTY:
            propKey = FileConnectionTexts.FILE_TONES;
            break;
        case FileConstants.VIDEOS_PROPERTY:
            propKey = FileConnectionTexts.FILE_VIDEOS;
            break;
        case FileConstants.MEMORY_CARD_PROPERTY:
            propKey = FileConnectionTexts.FILE_MEMORYCARD;
            break;
        case FileConstants.PRIVATE_PROPERTY:
            propKey = FileConnectionTexts.FILE_PRIVATE;
            break;
        default:
            retValue = null;
        }
        return FileConnectionTexts.get(propKey);
    }

    public static Vector getForbiddenPaths()
    {
        String pathString = _getForbiddenPaths();
        String[] pathArray = Tokenizer.split(pathString, "*");
        Vector paths = new Vector();

        for (int index = 0; index < pathArray.length - 1; index++)
        {
            String toAdd = pathArray[index];
            toAdd = toAdd.replace('\\', '/');
            paths.addElement(toAdd);
        }
        return paths;
    }

    public static Vector getRestrictedPaths()
    {
        String pathString = _getRestrictedPaths();
        String[] pathArray = Tokenizer.split(pathString, "*");
        Vector paths = new Vector();

        for (int index = 0; index < pathArray.length - 1; index++)
        {
            String toAdd = pathArray[index];
            toAdd = toAdd.replace('\\', '/');
            paths.addElement(toAdd);
        }
        return paths;
    }

    public static String getAppPrivateDir()
    {
        String appPrivateDir = iAppInfo.getRootPath();
        appPrivateDir = appPrivateDir.replace('\\', '/');
        appPrivateDir += "scratch/";
        return appPrivateDir;
    }

    private static native String _getMemoryCardDrivePath();

    private static native String _getTemporaryDrivePath();

    private static native String _getRomDrivePath();

    private static native String _getDefaultRootPath();

    private static native String _getPathOfProperty(int value, boolean addRoot);

    private static native String _getForbiddenPaths();

    private static native String _getRestrictedPaths();
}
