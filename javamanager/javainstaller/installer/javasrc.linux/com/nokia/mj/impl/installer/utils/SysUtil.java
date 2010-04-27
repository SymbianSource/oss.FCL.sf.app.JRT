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


package com.nokia.mj.impl.installer.utils;

import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.installer.utils.PlatformUid;
import com.nokia.mj.impl.utils.Uid;

import java.util.Vector;

/**
 * SysUtil provides system utility services.
 *
 * @author Nokia Corporation
 * @version $Rev: 10289 $
 */
public final class SysUtil
{
    /*** ----------------------------- PUBLIC ------------------------------ */

    // Property categories.
    public static final Uid PROP_CATEGORY_SYSTEM =
        PlatformUid.createUid("0x101f75b6");

    // Property keys.
    public static final long PROP_KEY_JAVA_LATEST_INSTALLATION = 0x10282567;
    public static final long PROP_KEY_JAVA_LATEST_INSTALLATION_PROGRESS = 0x20019546;
    public static final long PROP_KEY_JAVA_LATEST_INSTALLATION_STATE = 0x20019547;

    // Repository ids.
    public static final Uid REPO_ID_JAVA_INST_VARIATION =
        PlatformUid.createUid("0x102824CE");

    // Repository keys.
    public static final long REPO_KEY_JAVA_INST_SHELL_FOLDER_ATTR = 0x00000001;
    public static final long REPO_KEY_JAVA_INST_SHELL_FOLDER_VALUE = 0x00000002;
    //public static final long REPO_KEY_JAVA_PREINST_DEF_DRIVE = 0x00000003;
    public static final long REPO_KEY_JAVA_INST_DEF_INST_DRIVE = 0x00000004;

    // Process states.
    public static final int PROC_STATE_ALIVE = 1;

    /**
     * Define property. Property must be defined before it
     * can be used. It is enough to define property once.
     * If property has already been defined, this method does
     * not have any effect.
     *
     * @param aCategory uid that defines the property category
     * @param aKey key that identifies the specific property within the category
     * @throws InstallerException if error occurs
     */
    public static void defineProperty(Uid aCategory, long aKey)
    {
        // nop
    }

    /**
     * Delete property. After deletion property cannot be used
     * before it is defined again. If property has not been defined,
     * this method does not have any effect.
     *
     * @param aCategory uid that defines the property category
     * @param aKey key that identifies the specific property within the category
     * @throws InstallerException if error occurs
     */
    public static void deleteProperty(Uid aCategory, long aKey)
    {
        // nop
    }

    /**
     * Get property value.
     *
     * @param aCategory uid that defines the property category
     * @param aKey key that identifies the specific property within the category
     * @return property value
     * @throws InstallerException if error occurs
     */
    public static int getPropertyValue(Uid aCategory, long aKey)
    {
        return 0;
    }

    /**
     * Set property value.
     *
     * @param aCategory uid that defines the property category
     * @param aKey key that identifies the specific property within the category
     * @param aValue property value
     * @throws InstallerException if error occurs
     */
    public static void setPropertyValue(Uid aCategory, long aKey, int aValue)
    {
        // nop
    }

    /**
     * Set property value from given Uid.
     *
     * @param aCategory uid that defines the property category
     * @param aKey key that identifies the specific property within the category
     * @param aValue property value
     * @throws InstallerException if error occurs
     */
    public static void setPropertyValue(Uid aCategory, long aKey, Uid aValue)
    {
        // nop
    }

    /**
     * Get repository value.
     *
     * @param aRepository uid that defines the repository
     * @param aKey key that identifies the setting in the repository
     * @return repository value
     * @throws InstallerException if error occurs
     */
    public static int getRepositoryValue(Uid aRepository, long aKey)
    {
        return 0;
    }

    /**
     * Get repository string value.
     *
     * @param aRepository uid that defines the repository
     * @param aKey key that identifies the setting in the repository
     * @return repository value
     * @throws InstallerException if error occurs
     */
    public static String getRepositoryStringValue(Uid aRepository, long aKey)
    {
        return "";
    }

    /**
     * Set repository value.
     *
     * @param aCategory uid that defines the repository
     * @param aKey key that identifies the setting in the repository
     * @param aValue repository value
     * @throws InstallerException if error occurs
     */
    public static void setRepositoryValue(Uid aCategory, long aKey, int aValue)
    {
        // nop
    }

    /**
     * Set repository string value.
     *
     * @param aCategory uid that defines the repository
     * @param aKey key that identifies the setting in the repository
     * @param aValue repository value
     * @throws InstallerException if error occurs
     */
    public static void setRepositoryStringValue(Uid aCategory, long aKey, String aValue)
    {
        // nop
    }

    /**
     * Get drive's unique identifying number.
     *
     * @param aDrive drive for which unique id is requested (0 = A, 1 = B, etc.)
     * @return drive unique id
     * @throws InstallerException if error occurs
     */
    public static int getDriveUniqId(int aDrive)
    {
        return -1;
    }

    /**
     * Checks if drive is present.
     *
     * @param aDrive drive for which presence is requested (0 = A, 1 = B, etc.)
     * @return true if drive is present, false otherwise
     * @throws InstallerException if error occurs
     */
    public static boolean isDrivePresent(int aDrive)
    {
        return true;
    }

    /**
     * Checks if drive is read only drive.
     *
     * @param aDrive drive for which R/O state is requested (0 = A, 1 = B, etc.)
     * @return true if drive is read only, false otherwise
     * @throws InstallerException if error occurs
     */
    public static boolean isDriveReadOnly(int aDrive)
    {
        return false;
    }

    /**
     * Returns the drives that are visible to the user and where
     * the user can install java applications to.
     *
     * @param aVisibleDrives After return contains the user visible
     *  drives as DriveInfo objects
     * @throws InstallerException if error occurs
     * @see DriveInfo
     */
    public static void getUserVisibleDrives(Vector aVisibleDrives)
    {
        // Make sure the vector does not contain any old values
        aVisibleDrives.removeAllElements();
    }

    /**
     * Checks if disk space is below critical level.
     *
     * @param aBytesToWrite Number of bytes the caller is about to write to
     * disk. If value 0 is given, this method checks if the current
     * disk space is already below critical level.
     * @param aDrive drive for which disk space is requested (0 = A, 1 = B, etc.)
     * @return true if disk space is below critical level, false otherwise
     * @throws InstallerException if error occurs
     */
    public static boolean isDiskSpaceBelowCriticalLevel(
        int aBytesToWrite, int aDrive)
    {
        return false;
    }

    /**
     * Returns state of the process with given uid.
     *
     * @param aUid process uid
     * @return PROC_STATE_ALIVE if process is alive, 0 otherwise.
     * @throws InstallerException if error occurs
     */
    public static int getProcessState(Uid aUid)
    {
        return 0;
    }

    /**
     * Get screen width in pixels.
     *
     * @return screen width in pixels
     * @throws InstallerException if error occurs
     */
    public static int getScreenWidth()
    {
        // Return proper screen width in Linux.
        return 360;
    }

    /**
     * Get screen heigth in pixels.
     *
     * @return screen height in pixels
     * @throws InstallerException if error occurs
     */
    public static int getScreenHeight()
    {
        // Return proper screen height in Linux.
        return 640;
    }
    /*** ---------------------------- PROTECTED --------------------------- */
    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */
    /*** ----------------------------- NATIVE ----------------------------- */
}
