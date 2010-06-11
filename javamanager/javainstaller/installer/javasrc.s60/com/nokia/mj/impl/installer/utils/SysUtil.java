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
        int ret = _defineProperty(PlatformUid.getIntValue(aCategory), aKey);
        if (ret < 0)
        {
            InstallerException.internalError("Defining property (" +
                                             format(aCategory, aKey) + ") failed with code " + ret);
        }
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
        int ret = _deleteProperty(PlatformUid.getIntValue(aCategory), aKey);
        if (ret < 0)
        {
            InstallerException.internalError("Deleting property (" +
                                             format(aCategory, aKey) + ") failed with code " + ret);
        }
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
        IntValue value = new IntValue();
        int ret = _getPropertyValue(PlatformUid.getIntValue(aCategory),
                                    aKey, value);
        if (ret < 0)
        {
            InstallerException.internalError("Getting property (" +
                                             format(aCategory, aKey) + ") value failed with code " + ret);
        }
        return value.getValue();
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
        int ret = _setPropertyValue(PlatformUid.getIntValue(aCategory),
                                    aKey, aValue);
        if (ret < 0)
        {
            InstallerException.internalError("Setting property (" +
                                             format(aCategory, aKey, aValue) + ") value failed with code " + ret);
        }
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
        setPropertyValue(aCategory, aKey,
                         (aValue != null? PlatformUid.getIntValue(aValue): 0));
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
        IntValue value = new IntValue();
        int ret = _getRepositoryValue(PlatformUid.getIntValue(aRepository),
                                      aKey, value);
        if (ret < 0)
        {
            InstallerException.internalError("Getting repository (" +
                                             format(aRepository, aKey) +
                                             ") value failed with code " + ret);
        }
        return value.getValue();
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
        String[] value = new String[1];
        int ret = _getRepositoryStringValue(
                      PlatformUid.getIntValue(aRepository), aKey, value);
        if (ret < 0)
        {
            InstallerException.internalError("Getting repository (" +
                                             format(aRepository, aKey) +
                                             ") string value failed with code " + ret);
        }
        return value[0];
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
        int ret = _setRepositoryValue(PlatformUid.getIntValue(aCategory),
                                      aKey, aValue);
        if (ret < 0)
        {
            InstallerException.internalError("Setting repository (" +
                                             format(aCategory, aKey, aValue) + ") value failed with code " + ret);
        }
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
        int ret = _setRepositoryStringValue(PlatformUid.getIntValue(aCategory),
                                            aKey, aValue);
        if (ret < 0)
        {
            InstallerException.internalError(
                "Setting repository (" + format(aCategory, aKey, aValue) +
                ") string value failed with code " + ret);
        }
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
        IntValue value = new IntValue();
        int ret = _getDriveUniqId(aDrive, value);
        if (ret < 0)
        {
            InstallerException.internalError("Getting drive " + aDrive +
                                             " unique id failed with code " + ret);
        }
        return value.getValue();
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
        BooleanValue value = new BooleanValue();
        int ret = _isDrivePresent(aDrive, value);
        if (ret < 0)
        {
            InstallerException.internalError("Getting drive " + aDrive +
                                             " presence failed with code " + ret);
        }
        return value.getValue();
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
        int ret = _isDriveReadOnly(aDrive);
        if (ret < 0)
        {
            InstallerException.internalError("Getting drive " + aDrive +
                                             " read only state failed with code " + ret);
        }

        if (0 == ret)
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    /**
     * Returns the drive number of the default phone memory drive.
     *
     * @return drive number (0..25)
     * @throws InstallerException if error occurs
     */
    public static int getDefaultPhoneMemory()
    {
        int ret = _getDefaultPhoneMemory();
        if (ret < 0)
        {
            InstallerException.internalError("Getting default phone memory " +
                                             " drive failed with code " + ret);
        }
        return ret;
    }

    /**
     * Returns the drive number of the default mass storage drive.
     *
     * @return drive number (0..25)
     * @throws InstallerException if error occurs
     */
    public static int getDefaultMassStorage()
    {
        int ret = _getDefaultMassStorage();
        if (ret < 0)
        {
            InstallerException.internalError("Getting default mass storage " +
                                             " drive failed with code " + ret);
        }
        return ret;
    }

    /**
     * Returns the drives that are visible to the user and where
     * the user can install java applications to. This means that
     * substed and remote drives are not returned.
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
        if (iUserVisibleDrives != null)
        {
            // Drives have already been fetched, just return them.
            for (int i = 0; i < iUserVisibleDrives.size(); i++)
            {
                aVisibleDrives.addElement(iUserVisibleDrives.elementAt(i));
            }
            return;
        }
        // Fetch the drives.
        int ret = _getUserVisibleDrives(aVisibleDrives);
        if (ret < 0)
        {
            InstallerException.internalError("Getting user visible " +
                                             " drives failed with code " + ret);
        }
        // Save the drives so that next time they are not fetched again.
        iUserVisibleDrives = new Vector();
        for (int i = 0; i < aVisibleDrives.size(); i++)
        {
            iUserVisibleDrives.addElement(aVisibleDrives.elementAt(i));
        }
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
        BooleanValue value = new BooleanValue();
        int ret = _isDiskSpaceBelowCriticalLevel(aBytesToWrite, aDrive, value);
        if (ret < 0)
        {
            InstallerException.internalError("Checking disk " + aDrive +
                                             " space failed with code " + ret);
        }
        return value.getValue();
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
        int ret = _getProcessState(PlatformUid.getIntValue(aUid));
        if (ret < 0)
        {
            InstallerException.internalError("Getting process " + aUid +
                                             " state failed with code " + ret);
        }
        return ret;
    }

    /**
     * Get screen width in pixels.
     *
     * @return screen width in pixels
     * @throws InstallerException if error occurs
     */
    public static int getScreenWidth()
    {
        int ret = _getScreenWidth();
        if (ret < 0)
        {
            InstallerException.internalError(
                "Getting screen width failed with code " + ret);
        }
        return ret;
    }

    /**
     * Get screen heigth in pixels.
     *
     * @return screen height in pixels
     * @throws InstallerException if error occurs
     */
    public static int getScreenHeight()
    {
        int ret = _getScreenHeight();
        if (ret < 0)
        {
            InstallerException.internalError(
                "Getting screen height failed with code " + ret);
        }
        return ret;
    }

    /**
     * Maps given ISO language/country code to Symbian TLanguage.
     *
     * @param aLocale ISO language/country code
     * @return Symbian TLanguage value, or -1 if no matching language is found.
     */
    public static int isoToLang(String aLocale)
    {
        // Replace possible dash with underscore.
        aLocale = aLocale.replace('-', '_');
        // Get language and country parts.
        String lang = aLocale.toLowerCase();
        String country = null;
        int sepIndex = aLocale.indexOf("_");
        if (sepIndex >= 0)
        {
            lang = aLocale.substring(0, sepIndex).toLowerCase();
            country = aLocale.substring(sepIndex + 1).toUpperCase();
        }
        // Map locale to Symbian TLanguage using native service.
        int result = -1;
        if (country == null)
        {
            result = _isoToLang(lang);
        }
        else
        {
            result = _isoToLang(lang + "_" + country);
            if (result == -1)
            {
                // No result for language and country, try using language only.
                result = _isoToLang(lang);
            }
        }
        //Log.log("SysUtil.isoToLang: " + aLocale + " ==> " + result);
        return result;
    }

    /*** ---------------------------- PROTECTED --------------------------- */
    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */

    /** User visible drives. Contains DriveInfo objects. */
    private static Vector iUserVisibleDrives = null;

    /**
     * Returns string presentation of given uid and key.
     */
    private static String format(Uid aUid, long aKey)
    {
        return "Uid: " + aUid.toString() + ", key: 0x" + Long.toString(aKey, 16);
    }

    /**
     * Returns string presentation of given uid, key and value.
     */
    private static String format(Uid aUid, long aKey, int aValue)
    {
        return format(aUid, aKey) + ", value: " + aValue;
    }

    /**
     * Returns string presentation of given uid, key and value.
     */
    private static String format(Uid aUid, long aKey, String aValue)
    {
        return format(aUid, aKey) + ", value: " + aValue;
    }

    /*** ----------------------------- NATIVE ----------------------------- */

    /**
     * Define property.
     *
     * @param aCategory uid that defines the property category
     * @param aKey key that identifies the specific property within the category
     * @return Symbian error code (negative number)
     */
    private static native int _defineProperty(int aUid, long aKey);

    /**
     * Delete property.
     *
     * @param aCategory uid that defines the property category
     * @param aKey key that identifies the specific property within the category
     * @return Symbian error code (negative number)
     */
    private static native int _deleteProperty(int aUid, long aKey);

    /**
     * Get property value.
     *
     * @param aCategory uid that defines the property category
     * @param aKey key that identifies the specific property within the category
     * @param aValue returned property value
     * @return Symbian error code (negative number)
     */
    private static native int _getPropertyValue(
        int aUid, long aKey, IntValue aValue);

    /**
     * Set property value.
     *
     * @param aCategory uid that defines the property category
     * @param aKey key that identifies the specific property within the category
     * @param aValue property value
     * @return Symbian error code (negative number)
     */
    private static native int _setPropertyValue(
        int aUid, long aKey, int aValue);

    /**
     * Get repository value.
     *
     * @param aRepository uid that defines the repository
     * @param aKey key that identifies the setting in the repository
     * @param aValue returned repository value
     * @return Symbian error code (negative number)
     */
    private static native int _getRepositoryValue(
        int aRepository, long aKey, IntValue aValue);

    /**
     * Get repository string value.
     *
     * @param aRepository uid that defines the repository
     * @param aKey key that identifies the setting in the repository
     * @param aValueArray array of length one for holding the repository value
     * @return Symbian error code (negative number)
     */
    private static native int _getRepositoryStringValue(
        int aRepository, long aKey, String[] aValueArray);

    /**
     * Set repository value.
     *
     * @param aRepository uid that defines the repository
     * @param aKey key that identifies the setting in the repository
     * @param aValue repository value
     * @return Symbian error code (negative number)
     */
    private static native int _setRepositoryValue(
        int aRepository, long aKey, int aValue);

    /**
     * Set repository string value.
     *
     * @param aRepository uid that defines the repository
     * @param aKey key that identifies the setting in the repository
     * @param aValue repository value
     * @return Symbian error code (negative number)
     */
    private static native int _setRepositoryStringValue(
        int aRepository, long aKey, String aValue);

    /**
     * Get drive's unique identifying number.
     *
     * @param aDrive drive for which unique id is requested (0 = A, 1 = B, etc.)
     * @param aValue returned drive unique id
     * @return Symbian error code (negative number)
     */
    private static native int _getDriveUniqId(int aDrive, IntValue aValue);

    /**
     * Checks if drive is present.
     *
     * @param aDrive drive for which presence is requested (0 = A, 1 = B, etc.)
     * @params aValue true if drive is present, false otherwise
     * @return Symbian error code (negative number)
     */
    private static native int _isDrivePresent(int aDrive, BooleanValue aValue);

    /**
     * Checks if drive is read only drive.
     *
     * @param aDrive drive for which R/O state is requested (0 = A, 1 = B, etc.)
     * @return negative number (Symbian error code) if case of error,
     * 0 if drive is not read only, >0 if drive is read only
     */
    private static native int _isDriveReadOnly(int aDrive);

    /**
     * Get the drive number of the default phone memory drive.
     *
     * @return the drive number (0..25) or Symbian error code (negative number)
     */
    private static native int _getDefaultPhoneMemory();

    /**
     * Get the drive number of the default mass storage drive.
     *
     * @return the drive number (0..25) or Symbian error code (negative number)
     */
    private static native int _getDefaultMassStorage();

    /**
     * Get list of all drives visible to the user.
     *
     * @param aVisibleDrives after return this vector of integers contains
     *  the user visible drives in Integer form (0 == 'A' drive etc)
     * @return 0 or Symbian error code (negative number)
     */
    private static native int _getUserVisibleDrives(Vector aVisibleDrives);

    /**
     * Checks if disk space is below critical level.
     *
     * @param aBytesToWrite Number of bytes the caller is about to write to
     * disk. If value 0 is given, this method checks if the current
     * disk space is already below critical level.
     * @param aDrive drive for which disk space is requested (0 = A, 1 = B, etc.)
     * @param aValue true if disk space is below critical level, false otherwise
     * @return Symbian error code (negative number)
     */
    private static native int _isDiskSpaceBelowCriticalLevel(
        int aBytesToWrite, int aDrive, BooleanValue aValue);

    /**
     * Get process state.
     *
     * @param aUid process uid
     * @return PROC_STATE_ALIVE if process is alive, 0 if process is
     * not alive and Symbian error code (negative number) in case of
     * an error.
     */
    private static native int _getProcessState(int aUid);

    /**
     * Get screen width in pixels.
     *
     * @return screen width in pixels, or Symbian error code
     * (negative number) in case of an error
     */
    private static native int _getScreenWidth();

    /**
     * Get screen height in pixels.
     *
     * @return screen heigth in pixels, or Symbian error code
     * (negative number) in case of an error
     */
    private static native int _getScreenHeight();

    /**
     * Maps given ISO language/country code to Symbian TLanguage.
     *
     * @param aLocale ISO language/country code
     * @return Symbian TLanguage value, or -1 if no matching language is found.
     */
    private static native int _isoToLang(String aLocale);

    /**
     * Class for holding return value from native side.
     */
    private static class IntValue
    {
        private int iValue = 0;
        int getValue()
        {
            return iValue;
        }
        void setValue(int aValue)
        {
            iValue = aValue;
        }
    }

    /**
     * Class for holding return value from native side.
     */
    private static class BooleanValue
    {
        private boolean iValue = false;
        boolean getValue()
        {
            return iValue;
        }
        void setValue(boolean aValue)
        {
            iValue = aValue;
        }
    }
}
