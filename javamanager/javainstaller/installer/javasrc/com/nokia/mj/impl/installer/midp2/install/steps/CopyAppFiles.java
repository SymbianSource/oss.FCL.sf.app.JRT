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

import com.nokia.mj.impl.installer.applicationregistrator.SifRegistrator;
import com.nokia.mj.impl.installer.exetable.ExeBall;
import com.nokia.mj.impl.installer.exetable.ExeStep;
import com.nokia.mj.impl.installer.integrityservice.IntegrityService;
import com.nokia.mj.impl.installer.storagehandler.ApplicationInfo;
import com.nokia.mj.impl.installer.utils.FileRoots;
import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.installer.utils.SysUtil;

import java.util.Vector;

public class CopyAppFiles extends ExeStep
{
    public void execute(ExeBall aBall)
    {
        InstallBall ball = (InstallBall)aBall;
        ball.log("Copying application files...");

        final String PRIVATE_DATA_DIR = "scratch";
        final String RMS_DATA_DIR = "rms";
        String privateDataDir = ball.iSuite.getRootDir() + PRIVATE_DATA_DIR;
        String rmsDataDir = ball.iSuite.getRootDir() + RMS_DATA_DIR;
        String oldPrivateDataDir = null;
        String oldRmsDataDir = null;

        if (!FileUtils.exists(ball.iSuite.getRootDir()))
        {
            // Ensure that suite root directory exists.
            ball.iIntegrityService.createDir(ball.iSuite.getRootDir());
        }

        if (ball.iOldSuite != null)
        {
            oldPrivateDataDir = ball.iOldSuite.getRootDir() + PRIVATE_DATA_DIR;
            oldRmsDataDir = ball.iOldSuite.getRootDir() + RMS_DATA_DIR;

            // In case of an upgrade, remove old jad and jar files,
            // but do not remove anything from preinstall directory.
            String preinstallDir = FileRoots.getPreinstallDir();
            if (preinstallDir == null || preinstallDir.length() == 0 ||
                    ball.iOldSuite.getJarPath().indexOf(preinstallDir) == -1)
            {
                removeFromAppsRoot(ball.iIntegrityService, ball.iOldSuite.getJadPath());
                removeFromAppsRoot(ball.iIntegrityService, ball.iOldSuite.getJarPath());
            }

            // If necessary, remove private data.
            if (ball.iUpgradeData != null && !ball.iUpgradeData.booleanValue())
            {
                // Private data removal is needed.
                removeFromAppsRoot(ball.iIntegrityService, oldPrivateDataDir);
                removeFromAppsRoot(ball.iIntegrityService, oldRmsDataDir);
            }
            else
            {
                // If upgrade is made to different drive, move private data,
                // otherwise leave private data as it is.
                int oldDrive = FileUtils.getDrive(ball.iOldSuite.getRootDir());
                if (!FileUtils.isSameDrive(ball.iOldSuite.getRootDir(),
                                           ball.iSuite.getRootDir()) &&
                        oldDrive != -1 && SysUtil.isDrivePresent(oldDrive))
                {
                    boolean result = ball.iIntegrityService.move
                                     (ball.iOldSuite.getRootDir(), ball.iSuite.getRootDir());
                    if (!result)
                    {
                        // Moving private data failed, do not abort installation
                        // but write a warning to log.
                        Log.logWarning("Moving private data failed.");
                    }
                }
            }
        }
        else
        {
            // In case of a new installation, ensure that installation
            // drive does not have application private data directories.
            removeFromAppsRoot(ball.iIntegrityService, privateDataDir);
            removeFromAppsRoot(ball.iIntegrityService, rmsDataDir);
        }

        // In case of conversion installation, copy private data and RMS data.
        if (ball.iConversionInstallation)
        {
            oldPrivateDataDir = ball.iConversionRoot + PRIVATE_DATA_DIR;
            if (FileUtils.exists(oldPrivateDataDir))
            {
                ball.log("Conversion installation, copying " +
                         PRIVATE_DATA_DIR + " dir...");
                boolean result = ball.iIntegrityService.copy(
                                     oldPrivateDataDir, privateDataDir);
                if (!result)
                {
                    Log.logWarning(
                        "Copying legacy application private data failed.");
                }
            }
            oldRmsDataDir = ball.iConversionRoot + RMS_DATA_DIR;
            if (FileUtils.exists(oldRmsDataDir))
            {
                ball.log("Conversion installation, copying " +
                         RMS_DATA_DIR + " dir...");
                boolean result = ball.iIntegrityService.copy(
                                     oldRmsDataDir, rmsDataDir);
                if (!result)
                {
                    Log.logWarning(
                        "Copying legacy application rms data failed.");
                }
            }
        }

        // Copy jad and jar files to correct place,
        // except in preinstallation case do not copy files.
        if (!ball.iPreinstallation)
        {
            copyToAppsRoot(ball.iIntegrityService,
                           ball.iJadFilename, ball.iSuite.getJadPath());
            if (ball.iMoveInstallationFile)
            {
                moveToAppsRoot(ball.iIntegrityService,
                               ball.iJarFilename, ball.iSuite.getJarPath());
            }
            else
            {
                copyToAppsRoot(ball.iIntegrityService,
                               ball.iJarFilename, ball.iSuite.getJarPath());
            }
        }

        if (SifRegistrator.getSifMode() > 0)
        {
            // Copy application icons.
            copyRegisteredIcons(ball);
        }

        // Ensure that application private data folder exists.
        if (!FileUtils.exists(privateDataDir))
        {
            ball.iIntegrityService.createDir(privateDataDir);
        }
    }

    public void cancel(ExeBall aBall)
    {
        // nop
    }

    private static void removeFromAppsRoot(IntegrityService aIs, String aFile)
    {
        if (aFile != null && aFile.length() > 0 && FileUtils.exists(aFile))
        {
            boolean result = aIs.delete(aFile);
            if (!result)
            {
                // File removal failed, do not abort installation
                // but write a warning to log.
                Log.logWarning("Removing " + aFile + " failed.");
            }
        }
    }

    private static void copyToAppsRoot(IntegrityService aIs, String aFrom, String aTo)
    {
        if (aFrom != null && aTo != null && !aFrom.equals(aTo))
        {
            boolean result = aIs.copy(aFrom, aTo);
            if (!result)
            {
                InstallerException.internalError(
                    "Copying file " + aFrom + " to " + aTo +  " failed.");
            }
        }
    }

    private static void moveToAppsRoot(IntegrityService aIs, String aFrom, String aTo)
    {
        if (aFrom != null && aTo != null && !aFrom.equals(aTo))
        {
            boolean result = aIs.move(aFrom, aTo);
            if (!result)
            {
                InstallerException.internalError(
                    "Moving file " + aFrom + " to " + aTo +  " failed.");
            }
        }
    }

    private void copyRegisteredIcons(InstallBall aBall)
    {
        Vector apps = aBall.iSuite.getApplications();
        if (apps == null || apps.size() == 0)
        {
            return;
        }
        ApplicationInfo appInfo = null;
        String iconFilename = null;
        String iconDir = null;
        boolean result = true;
        for (int i = 0; i < apps.size(); i++)
        {
            appInfo = (ApplicationInfo)apps.elementAt(i);
            iconFilename = aBall.iSuite.getRegisteredIconPath(i);
            iconDir = FileUtils.getParent(iconFilename);
            if (!FileUtils.exists(iconDir))
            {
                // Ensure that icon directory exists.
                result = aBall.iIntegrityService.createDir(iconDir);
                if (!result)
                {
                    InstallerException.internalError(
                        "Creating icon directory " + iconDir + " failed.");
                }
            }
            result = aBall.iIntegrityService.copy(
                         appInfo.getConvertedIconPath(), iconFilename);
            if (!result)
            {
                InstallerException.internalError(
                    "Copying application icon from " +
                    appInfo.getConvertedIconPath() +
                    " to " + iconFilename + " failed.");
            }
        }
    }
}
