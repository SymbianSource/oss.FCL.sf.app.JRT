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


package com.nokia.mj.impl.installer.midp2.install.steps;

import com.nokia.mj.impl.installer.exetable.ExeBall;
import com.nokia.mj.impl.installer.exetable.ExeStep;
import com.nokia.mj.impl.installer.utils.DriveInfo;
import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.installer.utils.Platform;
import com.nokia.mj.impl.installer.utils.SysUtil;
import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;
import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.OtaStatusCode;

import java.util.Vector;

/**
 * Checks available disk space and chooses the default installation drive.
 * This step is called twice: before installation confirmation dialog is
 * displayed to user and after the user has answered to the dialog.
 */
public class CheckDiskSpace extends ExeStep
{
    public void execute(ExeBall aBall)
    {
        InstallBall ball = (InstallBall)aBall;
        int initialSize = ball.iSuite.calculateInitialSize();
        int requiredSize = getRequiredSize(ball);

        if (ball.iUserConfirmation == null)
        {
            // Before installation confirmation dialog is displayed,
            // choose the default installation drive.
            // Do not change the already selected installation drive
            // in case of preinstallation or if the -drive option
            // has been specified from command line.
            if (!ball.iPreinstallation &&
                    ball.iArgs.get("drive") == null &&
                    !Platform.isLinux())
            {
                Log.log("Choosing default installation drive");
                Vector drives = new Vector();
                SysUtil.getUserVisibleDrives(drives);
                logDrives("User visible drives:", drives);
                if (ball.iOldSuite == null ||
                        !SysUtil.isDrivePresent(ball.iInstallationDrive))
                {
                    // Either this is a new installation, or this is an
                    // update but the old installation drive is not present,
                    // so choose the default installation drive.
                    ball.iInstallationDrive = getDefaultInstallationDrive(
                                                  drives, requiredSize);
                }
            }
        }
        else
        {
            Log.log("Checking available disk space from drive " +
                    ball.iInstallationDrive);
            // After user has selected installation drive, check
            // free space from user selected drive.
            checkFreeSpace(requiredSize, ball.iInstallationDrive);
            // Update paths in the suite info to point to
            // user selected installation drive.
            FileUtils.setAppsRoot(ball.iInstallationDrive);
            updateSuitePaths(ball);
            ball.iSuite.setMediaId(SysUtil.getDriveUniqId
                                   (ball.iInstallationDrive));
        }

        // Add initial size to suite.
        ball.iSuite.setInitialSize(initialSize);
    }

    public void cancel(ExeBall aBall)
    {
        // nop
    }

    /**
     * Returns amount of disk space this application requires.
     */
    static int getRequiredSize(InstallBall aBall)
    {
        int initialSize = aBall.iSuite.getInitialSize();
        if (initialSize <= 0)
        {
            initialSize = aBall.iSuite.calculateInitialSize();
        }
        if (initialSize == 0 && aBall.iJarFilename != null)
        {
            // Get initialSize from jar file size.
            initialSize = (int)FileUtils.getSize(aBall.iJarFilename);

        }
        return initialSize + (100 * 1024); // +100kB
    }

    /**
     * Checks if given drive has enough free disk space. Throws
     * InstallerException if there is not enough free disk space.
     */
    private static void checkFreeSpace(int aSizeInBytes, int aDrive)
    {
        if (SysUtil.isDiskSpaceBelowCriticalLevel(aSizeInBytes, aDrive))
        {
            Log.logError("Disk space below critical level, required space " +
                         aSizeInBytes + " bytes, drive " + aDrive);
            throw InstallerException.getOutOfDiskSpaceException(
                aSizeInBytes, null);
        }
    }

    /**
     * Chooses the default installation drive from given DriveInfo vector.
     * Default installation drive is the first INTERNAL_MASS_STORAGE,
     * MEMORY_CARD or PHONE_MEMORY drive that has enough free space for
     * the application.
     * @param aDrives DriveInfo objects.
     * @param aSizeInBytes application size.
     * @return Default installation drive id.
     * @throws InstallerException if none of the drives has enough free
     * space for the application.
     */
    private static int getDefaultInstallationDrive(
        Vector aDrives, int aSizeInBytes)
    {
        sortDrives(aDrives);
        logDrives("Sorted drives:", aDrives);
        for (int i = 0; i < aDrives.size(); i++)
        {
            DriveInfo drive = (DriveInfo)aDrives.elementAt(i);
            int driveId = drive.getNumber();
            if (SysUtil.isDiskSpaceBelowCriticalLevel(aSizeInBytes, driveId))
            {
                Log.log("Drive " + driveId +
                        " space below critical level, required space " +
                        aSizeInBytes + " bytes");
            }
            else
            {
                Log.log("Drive " + driveId +
                        " has enough free space, required space " +
                        aSizeInBytes + " bytes");
                return driveId;
            }
        }
        // None of the available drives has enough space, throw an exception.
        throw InstallerException.getOutOfDiskSpaceException(
            aSizeInBytes, null);
    }

    /**
     * Sorts DriveInfos in given vector to drive priority order.
     * Priority order for the drives is CONFIGURED_DEFAULT,
     * INTERNAL_MASS_STORAGE, MEMORY_CARD, PHONE_MEMORY.
     * If there is more than one drive of the same type,
     * the ones which have more free space have higher priority.
     */
    private static void sortDrives(Vector aDrives)
    {
        for (int i = 0; i < aDrives.size(); i++)
        {
            for (int j = i+1; j < aDrives.size(); j++)
            {
                DriveInfo d1 = (DriveInfo)aDrives.elementAt(i);
                DriveInfo d2 = (DriveInfo)aDrives.elementAt(j);
                if (hasHigherPriority(d1, d2))
                {
                    Log.log(d1.getNumber() + " < " + d2.getNumber() +
                            ", swap [" + i + "] and [" + j + "]");
                    aDrives.removeElementAt(j);
                    aDrives.insertElementAt(d2, i);
                }
            }
        }
        // Move the configured default installation drive to be the first.
        int configuredDefault = getConfiguredDefaultDrive();
        if (configuredDefault != -1)
        {
            for (int i = 0; i < aDrives.size(); i++)
            {
                DriveInfo d = (DriveInfo)aDrives.elementAt(i);
                if (d.getNumber() == configuredDefault)
                {
                    aDrives.removeElementAt(i);
                    aDrives.insertElementAt(d, 0);
                }
            }
        }
    }

    /**
     * Logs list of drives to info log.
     */
    private static void logDrives(String aMsg, Vector aDrives)
    {
        StringBuffer sortedDrives = new StringBuffer();
        for (int i = 0; i < aDrives.size(); i++)
        {
            DriveInfo drive = (DriveInfo)aDrives.elementAt(i);
            sortedDrives.append(" ").append(drive.getNumber())
                .append(" (").append(drive.getDriveType())
                .append(", ").append(drive.getFreeSpace()).append(")");

        }
        Log.log(aMsg + sortedDrives);
    }

    /**
     * Returns the configured default installation drive.
     * If it is not available, returns -1.
     */
    private static int getConfiguredDefaultDrive()
    {
        int result = -1;
        try
        {
            String driveName = SysUtil.getRepositoryStringValue(
                                   SysUtil.REPO_ID_JAVA_INST_VARIATION,
                                   SysUtil.REPO_KEY_JAVA_INST_DEF_INST_DRIVE);
            if (driveName != null && driveName.length() > 0)
            {
                result = driveName.toLowerCase().charAt(0) - 'a';
            }
        }
        catch (Throwable t)
        {
            Log.log("Getting default installation drive from repository failed", t);
        }
        Log.log("Default installation drive from repository: " + result);
        return result;
    }

    /**
     * Returns true if aD2 has higher priority than aD1.
     */
    private static boolean hasHigherPriority(
        DriveInfo aD1, DriveInfo aD2)
    {
        boolean result = false;
        int type1 = aD1.getDriveType();
        int type2 = aD2.getDriveType();
        if (type1 == type2)
        {
            // Check which drive has more free space.
            if (aD2.getFreeSpace() > aD1.getFreeSpace())
            {
                result = true;
            }
        }
        // Check priority basing on drive type.
        if (!result &&
                type1 != DriveInfo.INTERNAL_MASS_STORAGE &&
                type2 == DriveInfo.INTERNAL_MASS_STORAGE)
        {
            result = true;
        }
        if (!result &&
                type1 != DriveInfo.INTERNAL_MASS_STORAGE &&
                type1 != DriveInfo.MEMORY_CARD &&
                type2 == DriveInfo.MEMORY_CARD)
        {
            result = true;
        }
        if (!result &&
                type1 != DriveInfo.INTERNAL_MASS_STORAGE &&
                type1 != DriveInfo.MEMORY_CARD &&
                type1 != DriveInfo.PHONE_MEMORY &&
                type2 == DriveInfo.PHONE_MEMORY)
        {
            result = true;
        }
        return result;
    }

    /**
     * Updates installation drive into root, jad and jar
     * paths in suite info object.
     */
    private static void updateSuitePaths(InstallBall aBall)
    {
        aBall.iSuite.setRootDir
        (FileUtils.setDrive
         (aBall.iSuite.getRootDir(), aBall.iInstallationDrive));
        if (aBall.iSuite.getJadPath() != null)
        {
            if (!aBall.iPreinstallation)
            {
                aBall.iSuite.setJadPath
                (FileUtils.setDrive
                 (aBall.iSuite.getJadPath(), aBall.iInstallationDrive));
            }
        }
        if (aBall.iSuite.getJarPath() != null)
        {
            if (!aBall.iPreinstallation)
            {
                aBall.iSuite.setJarPath
                (FileUtils.setDrive
                 (aBall.iSuite.getJarPath(), aBall.iInstallationDrive));
            }
        }
    }
}
