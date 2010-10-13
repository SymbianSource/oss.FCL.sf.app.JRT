/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

import com.nokia.mj.impl.installer.applicationregistrator.SifNotifier;
import com.nokia.mj.impl.installer.exetable.ExeBall;
import com.nokia.mj.impl.installer.exetable.ExeStep;
import com.nokia.mj.impl.installer.storagehandler.ApplicationInfo;
import com.nokia.mj.impl.installer.utils.FileRoots;
import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;

import java.util.Vector;

public class StartProgressNotifications extends ExeStep
{
    public void execute(ExeBall aBall)
    {
        InstallBall ball = (InstallBall)aBall;
        Log.log("Starting progress notifications...");

        if (!SifNotifier.enabled())
        {
            Log.log("SifNotifier disabled.");
            return;
        }

        try
        {
            ball.iSifNotifier = new SifNotifier();
        }
        catch (Throwable t)
        {
            Log.logError("StartProgressNotifications: creating SifNotifier failed", t);
            // Could not create SifNotifer, let's proceed without it.
            return;
        }

        // Init application names array.
        Vector appNamesVector = ball.iSuite.getApplications();
        if (ball.iOldSuite != null)
        {
            // In update get the names of previously installed applications.
            appNamesVector = ball.iOldSuite.getApplications();
        }
        String[] appNames = new String[appNamesVector.size()];
        for (int i = 0; i < appNamesVector.size(); i++)
        {
            appNames[i] =
                ((ApplicationInfo)appNamesVector.elementAt(i)).getName();
        }

        // Initialize icon filenames and icon dir.
        String iconDir = null;
        String componentIcon = null;
        String[] appIcons = null;
        if (ball.iOldSuite != null)
        {
            // In update get the icons of previously installed applications.
            iconDir = FileRoots.getRegisteredIconDir(
                FileUtils.getDrive(ball.iOldSuite.getRootDir())) +
                ball.iOldSuite.getUid().toString();
            appIcons = ball.iOldSuite.getRegisteredAppIcons();
            if (appIcons != null)
            {
                for (int i = 0; i < appIcons.length; i++)
                {
                    Log.log("Old app icon " + appIcons[i] +
                            " for "+ appNames[i] + " from " + iconDir);
                }
            }
        }

        try
        {
            ball.iSifNotifier.notifyStart(
                (ball.iOldSuite != null?
                 ball.iSifNotifier.OP_UPDATE: ball.iSifNotifier.OP_INSTALL),
                ball.iSuite.getGlobalId(), ball.iSuite.getName(),
                appNames, appIcons, ball.iSuite.calculateInitialSize(),
                iconDir, componentIcon);
        }
        catch (Throwable t)
        {
            Log.logError("StartProgressNotifications: SifNotifier.notifyStart failed", t);
        }

        // Set SifNotifier for the listeners that should use it.
        ball.iInstallationNotifier.setSifNotifier(ball.iSifNotifier);
    }

    public void cancel(ExeBall aBall)
    {
        // nop
    }
}
