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
import com.nokia.mj.impl.installer.jsrpluginnotifier.InstallerExtensionInfo;
import com.nokia.mj.impl.installer.jsrpluginnotifier.JsrPluginNotifier;
import com.nokia.mj.impl.utils.Uid;

/**
 * MIDP uninstallation step NotifyJsrPlugins.
 * Added to MIDP2 Uninstall table in com.nokia.mj.impl.installer.Installer
 */
public class NotifyJsrPlugins extends ExeStep
{
    public void execute(ExeBall aBall)
    {
        UninstallBall ball = (UninstallBall)aBall;

        // If any Jsr plugins, notify plugins.
        if (ball.iJsrPluginNotifier.anyJsrPlugins())
        {
            ball.iJsrPluginNotifier.notifyUninstallation(
                createInstallerExtensionInfo(ball));
        }
    }

    public void cancel(ExeBall aBall)
    {
        UninstallBall ball = (UninstallBall)aBall;

        // Notify possible plugins that uninstallation has been cancelled.
        if (ball.iJsrPluginNotifier.anyJsrPlugins())
        {
            ball.iJsrPluginNotifier.notifyRollbackUninstall(
                createInstallerExtensionInfo(ball));
        }
    }

    private InstallerExtensionInfo createInstallerExtensionInfo(UninstallBall aBall)
    {
        InstallerExtensionInfo info = new InstallerExtensionInfo();
        // Beware, it is possible that aBall has not been fully initialized.
        if (aBall.iSuite != null)
        {
            info.iUid = aBall.iSuite.getUid();
            info.iAppUids = aBall.iSuite.getApplicationUids();
            info.iAttributes = aBall.iSuite.getAttributes();
            info.iRootPath = aBall.iSuite.getRootDir();
        }
        info.iUpgrade = false;
        info.iSilent = aBall.isSilent();
        if (aBall.iStorageHandler != null)
        {
            info.iStorageSession = aBall.iStorageHandler.getSession();
        }
        return info;
    }
}
