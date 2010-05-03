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

import com.nokia.mj.impl.installer.Installer;
import com.nokia.mj.impl.installer.InstallerResultMessage;
import com.nokia.mj.impl.installer.applicationregistrator.ApplicationRegistrator;
import com.nokia.mj.impl.installer.applicationregistrator.SifRegistrator;
import com.nokia.mj.impl.installer.exetable.ExeBall;
import com.nokia.mj.impl.installer.exetable.ExeStep;
import com.nokia.mj.impl.installer.integrityservice.IntegrityService;
import com.nokia.mj.impl.installer.jsrpluginnotifier.JsrPluginNotifier;
import com.nokia.mj.impl.installer.storagehandler.StorageHandler;
import com.nokia.mj.impl.installer.utils.Args;
import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.installer.utils.PlatformUid;
import com.nokia.mj.impl.utils.Uid;

public class PrepareUninstallation extends ExeStep
{
    public void execute(ExeBall aBall)
    {
        UninstallBall ball = (UninstallBall)aBall;
        ball.log("Preparing uninstallation...");

        parseArgs(ball);
        if (ball.iUid == null)
        {
            InstallerException.internalError("Uid parameter missing.");
        }
        ball.log("Uninstalling application with uid: " + ball.iUid);

        String isRoot = FileUtils.getIntegrityServiceRoot();
        ball.iIntegrityService = IntegrityService.getInstance(isRoot);
        ball.iStorageHandler = new StorageHandler();
        ball.iStorageHandler.startSession();
        ball.iApplicationRegistrator = new ApplicationRegistrator();
        ball.iApplicationRegistrator.startSession();
        ball.iSifRegistrator = new SifRegistrator();
        ball.iSifRegistrator.startSession(true);
        ball.iJsrPluginNotifier = new JsrPluginNotifier(ball.iIntegrityService);

        ball.iInstallationNotifier.start(
            ball.iInstallationNotifier.UNINSTALLING);
    }

    public void cancel(ExeBall aBall)
    {
        UninstallBall ball = (UninstallBall)aBall;
        ball.log("Uninstallation failed.");
        if (ball.getExecuteException() != null)
        {
            ball.log(ball.getExecuteException().toString());
        }

        // Cleanup security components.
        ball.cleanupSecurity();

        boolean result = ball.iIntegrityService.rollback();
        if (!result)
        {
            Log.logError("IntegrityService rollback failed");
        }
        else
        {
            Log.log("IntegrityService rolled back");
        }
        try
        {
            ball.iStorageHandler.rollbackSession();
            Log.log("StorageHandler rolled back");
        }
        catch (Exception ex)
        {
            Log.logError("StorageHandler rollback failed", ex);
        }
        try
        {
            ball.iSifRegistrator.rollbackSession();
            Log.log("SifRegistrator rolled back");
        }
        catch (Exception ex)
        {
            Log.logError("SifRegistrator rollback failed", ex);
        }
        try
        {
            ball.iApplicationRegistrator.rollbackSession();
            Log.log("ApplicationRegistrator rolled back");
        }
        catch (Exception ex)
        {
            Log.logError("ApplicationRegistrator rollback failed", ex);
        }

        Uid uid = (ball.iSuite != null? ball.iSuite.getUid(): null);
        ball.iInstallationNotifier.finish(
            uid, ball.iInstallationNotifier.UNINSTALL_FAIL);
    }

    private static void parseArgs(UninstallBall aBall)
    {
        Args args = aBall.iArgs;

        // Parse commsresult option first so that if parsing other
        // options fails the InstallerResultMesage will be correctly
        // sent.
        String arg = args.get("commsresult");
        if (arg != null)
        {
            aBall.iCommsResultEndpoints =
                InstallerResultMessage.parseEndpoints(arg);
        }

        String uidArg = args.get("uid");
        String cidArg = args.get("cid");
        if (uidArg != null && cidArg != null)
        {
            InstallerException.internalError(
                "Specify either uid or cid as an argument, not both.");
        }
        if (uidArg != null)
        {
            Uid uid = PlatformUid.createUid(uidArg);
            if (uid == null)
            {
                InstallerException.internalError("Invalid uid " + uidArg);
            }
            aBall.iUid = uid;
        }
        else if (cidArg != null)
        {
            Uid uid = null;
            try
            {
                int cid = Integer.parseInt(cidArg);
                uid = SifRegistrator.getUid(cid);
            }
            catch (NumberFormatException nfe)
            {
                InstallerException.internalError("Invalid cid " + cidArg, nfe);
            }
            if (uid == null)
            {
                InstallerException.internalError(
                    Installer.ERR_NOT_FOUND,
                    "Application not found for cid " + cidArg);
            }
            aBall.iUid = uid;
        }

        aBall.iIap = args.get("iap", aBall.iIap);
        aBall.iSnap = args.get("snap", aBall.iSnap);

        arg = args.get("proxyhost");
        if (arg != null)
        {
            System.setProperty("http.proxyHost", arg);
        }

        arg = args.get("proxyport");
        if (arg != null)
        {
            System.setProperty("http.proxyPort", arg);
        }

        arg = args.get("forceuninstall");
        if (arg != null)
        {
            aBall.iForceUninstall = true;
            aBall.log("ForceUninstall set");
        }

        aBall.iCaptainMsgs = args.getBoolean("captainmsgs", aBall.iCaptainMsgs);
    }
}
