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


package com.nokia.mj.impl.installer.midp2.uninstall.steps;

import com.nokia.mj.impl.installer.exetable.ExeBall;
import com.nokia.mj.impl.installer.exetable.ExeStep;
import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.installer.utils.SysUtil;

/**
 * Checks that there is enough free disk space available for uninstallation.
 */
public class CheckDiskSpace extends ExeStep
{
    public void execute(ExeBall aBall)
    {
        UninstallBall ball = (UninstallBall)aBall;

        // Check that application installation drive has at least
        // initialSize + 100kB free disk space.
        checkFreeSpace(ball.iSuite.calculateInitialSize() + (100 * 1024),
                       FileUtils.getDrive(ball.iSuite.getRootDir()));
    }

    public void cancel(ExeBall aBall)
    {
        // nop
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
}
