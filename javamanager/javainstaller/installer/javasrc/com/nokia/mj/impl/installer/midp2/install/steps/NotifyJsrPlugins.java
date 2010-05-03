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


package com.nokia.mj.impl.installer.midp2.install.steps;

import com.nokia.mj.impl.installer.exetable.ExeBall;
import com.nokia.mj.impl.installer.exetable.ExeStep;
import com.nokia.mj.impl.installer.jsrpluginnotifier.InstallerExtensionInfo;
import com.nokia.mj.impl.installer.jsrpluginnotifier.JsrPluginNotifier;
import com.nokia.mj.impl.utils.Uid;

/**
 * MIDP installation step NotifyJsrPlugins.
 * Added to MIDP2 Install table in com.nokia.mj.impl.installer.Installer
 *
 * @author Nokia Corporation
 * @version $Rev: 9041 $ $Date: 2009-12-07 18:36:08 +0200 (Mon, 07 Dec 2009) $
 */
public class NotifyJsrPlugins extends ExeStep
{
    public void execute(ExeBall aBall)
    {
        InstallBall ball = (InstallBall)aBall;

        // If any Jsr plugins, notify plugins
        if (ball.iJsrPluginNotifier.anyJsrPlugins())
        {
            // Create info object to be passed to plugins
            InstallerExtensionInfo info = new InstallerExtensionInfo();
            info.iUid = ball.iSuite.getUid();
            info.iUpgrade = (ball.iOldSuite != null);
            info.iSilent = ball.isSilent();
            info.iAttributes = ball.iCombinedAttributes;

            ball.iJsrPluginNotifier.notifyInstallation(info);
        }
    }

    public void cancel(ExeBall aBall)
    {
        InstallBall ball = (InstallBall)aBall;

        // Notify possible plugins that installation can been cancelled
        if (ball.iJsrPluginNotifier.anyJsrPlugins())
        {
            // Create info object to be passed to plugins
            InstallerExtensionInfo info = new InstallerExtensionInfo();
            // Beware, it is possible that aBall has not been fully
            // initialized
            if (null == ball.iSuite)
            {
                info.iUid = null;
            }
            else
            {
                info.iUid = ball.iSuite.getUid();
            }
            info.iUpgrade = (ball.iOldSuite != null);
            info.iSilent = ball.isSilent();
            info.iAttributes = ball.iCombinedAttributes;

            ball.iJsrPluginNotifier.notifyRollbackInstall(info);
        }
    }
}
