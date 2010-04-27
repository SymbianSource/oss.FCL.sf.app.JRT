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


package com.nokia.mj.impl.installer.midp2.uninstall.steps;

import com.nokia.mj.impl.installer.applicationregistrator.AppRegInfo;
import com.nokia.mj.impl.installer.applicationregistrator.ApplicationRegistrator;
import com.nokia.mj.impl.installer.exetable.ExeBall;
import com.nokia.mj.impl.installer.exetable.ExeStep;
import com.nokia.mj.impl.installer.storagehandler.ApplicationInfo;
import com.nokia.mj.impl.utils.Uid;

import java.util.Vector;

public class UnregisterApplication extends ExeStep
{
    public void execute(ExeBall aBall)
    {
        UninstallBall ball = (UninstallBall)aBall;

        // Unregister all apps
        ball.log("Removing app info from application registry...");

        Vector oldApps = ball.iSuite.getApplications();

        for (int j = 0; j < oldApps.size(); j++)
        {
            ApplicationInfo oldApp = (ApplicationInfo)oldApps.elementAt(j);

            AppRegInfo appRegInfo = new AppRegInfo(
                // Uid aUid,
                oldApp.getUid(),
                // String aGroupName, dummy
                "",
                // String aMIDletName, dummy
                "",
                // String aTargetDrive, dummy
                "",
                // String aIconFileName, dummy
                "",
                // String aJarFileName, dummy
                "",
                // boolean aHidden,
                false,
                // boolean aBackground
                false);

            ball.iApplicationRegistrator.unregisterApplication(appRegInfo);
        }
    }

    public void cancel(ExeBall aBall)
    {
        // nop
    }
}
