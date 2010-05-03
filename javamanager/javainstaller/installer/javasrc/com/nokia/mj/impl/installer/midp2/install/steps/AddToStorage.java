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


package com.nokia.mj.impl.installer.midp2.install.steps;

import com.nokia.mj.impl.fileutils.FileUtility;
import com.nokia.mj.impl.installer.exetable.ExeBall;
import com.nokia.mj.impl.installer.exetable.ExeStep;
import com.nokia.mj.impl.installer.utils.DriveInfo;
import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.installer.utils.SysUtil;

import java.util.Vector;

public class AddToStorage extends ExeStep
{
    public void execute(ExeBall aBall)
    {
        InstallBall ball = (InstallBall)aBall;
        updateJarPath(ball);
        if (ball.iSuite.getContentInfo() == ball.iSuite.CONTENT_INFO_DRM)
        {
            // This is DRM protected application, initialise content id.
            ball.iSuite.setContentId(
                FileUtility.getContentId(ball.iJarFilename));
        }
        if (isInternalDrive(ball.iInstallationDrive))
        {
            // Do not store media id for internal drives.
            ball.iSuite.setMediaId(0);
        }
        if (ball.iOldSuite != null)
        {
            // Old suite exists, this is an upgrade.
            ball.log("Updating info to storage...");
            ball.iStorageHandler.updateSuiteInfo(ball.iOldSuite, ball.iSuite);
        }
        else
        {
            // Old suite does not exists, this is a new installation.
            ball.log("Adding info to storage...");
            ball.iStorageHandler.writeSuiteInfo(ball.iSuite);
        }
    }

    public void cancel(ExeBall aBall)
    {
        // nop
    }

    /**
     * Updates jar path in suite info object.
     */
    private static void updateJarPath(InstallBall aBall)
    {
        if (aBall.iSuite.getJarPath() != null)
        {
            if (!aBall.iPreinstallation)
            {
                String oldPath = aBall.iSuite.getJarPath();
                String newPath = "";
                int index = oldPath.lastIndexOf(FileUtils.pathSeparatorChar());
                if (index != -1)
                {
                    newPath = oldPath.substring(0, index + 1);
                }
                newPath += "midlet";
                if (aBall.iJarFileType != null)
                {
                    newPath += aBall.iJarFileType;
                }
                else
                {
                    // If jar file type is not known,
                    // use the default extension ".jar".
                    newPath += ".jar";
                }
                aBall.iSuite.setJarPath(newPath);
                Log.log("Updated suite jar file name: " + newPath);
            }
        }
    }

    /**
     * Dreturns true if given drive is an internal drive,
     * false otherwise.
     */
    private static boolean isInternalDrive(int aDriveId)
    {
        boolean result = false;
        Vector drives = new Vector();
        SysUtil.getUserVisibleDrives(drives);
        for (int i = 0; i < drives.size(); i++)
        {
            DriveInfo drive = (DriveInfo)drives.elementAt(i);
            int driveId = drive.getNumber();
            if (aDriveId == driveId &&
                    (drive.getDriveType() == DriveInfo.PHONE_MEMORY ||
                     drive.getDriveType() == DriveInfo.INTERNAL_MASS_STORAGE))
            {
                result = true;
                break;
            }
        }
        return result;
    }
}
