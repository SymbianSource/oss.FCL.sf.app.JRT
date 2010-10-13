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

import com.nokia.mj.impl.installer.applicationregistrator.ApplicationRegistrator;
import com.nokia.mj.impl.installer.exetable.ExeBall;
import com.nokia.mj.impl.installer.exetable.ExeStep;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.utils.Uid;

public class FinalizeUninstallation extends ExeStep
{
    public void execute(ExeBall aBall)
    {
        UninstallBall ball = (UninstallBall)aBall;
        ball.log("Finalizing uninstallation...");

        // Cleanup security components.
        ball.cleanupSecurity();

        if (ball.iArgs.get("forcecancel") != null)
        {
            InstallerException.internalError("FORCED CANCEL");
        }

        // User is not allowed to cancel uninstallation after this step.
        ball.setCanCancel(false);

        checkWaitAttribute(ball, "before storage commit");

        // StorageHandler must be committed before ApplicationRegistrator
        // so that platform specific application registry can use the data
        // in Java Storage (S60 AppArc re-scan)
        ball.iStorageHandler.commitSession();
        Log.log("StorageHandler committed");

        checkWaitAttribute(ball, "after storage commit");

        boolean result = ball.iIntegrityService.commit();
        //ball.log("IntegrityService commit result: " + result);
        if (!result)
        {
            InstallerException.internalError("IntegrityService commit failed");
        }
        Log.log("IntegrityService committed");

        checkWaitAttribute(ball, "after integrityservice commit");

        ball.iSifRegistrator.commitSession();
        Log.log("SifRegistrator committed");

        checkWaitAttribute(ball, "after sif commit");

        ball.iApplicationRegistrator.commitSession(false);
        Log.log("ApplicationRegistrator committed");

        checkWaitAttribute(ball, "after apparc commit");

        ball.iInstallationNotifier.finish(
            ball.iSuite.getUid(), ball.iInstallationNotifier.UNINSTALL_OK);

        Uid[] appUids = ball.iSuite.getApplicationUids();
        if (appUids != null)
        {
            try
            {
                // Notify platform that applications have been removed.
                ball.iSifRegistrator.notifyAppChange(
                    appUids, ball.iSifRegistrator.APP_REMOVED);
            }
            catch (Throwable t)
            {
                Log.logError("SifRegistrator.notifyAppChange failed", t);
            }

            if (ball.iCaptainMsgs)
            {
                // Notify JavaCaptain that application has been uninstalled.
                ball.getCaptainService().appUpdated(appUids, null);
                Log.log("JavaCaptain notified");
            }

            for (int i = 0; i < appUids.length; i++)
            {
                ball.log("Application with uid " + appUids[i] + " uninstalled");
            }
        }
        ball.log("Application suite with uid " + ball.iSuite.getUid() +
                 " successfully uninstalled.");
    }

    public void cancel(ExeBall aBall)
    {
        // nop
    }

    /**
     * Wait a moment if Nokia-MIDlet-Install-Commit-Wait atribute has been set.
     */
    private static void checkWaitAttribute(UninstallBall aBall, String aWaitMsg)
    {
        String commitWaitEnabled = System.getProperty(
                                       "com.nokia.mj.impl.installer.commitwaitenabled");
        if (commitWaitEnabled != null &&
                commitWaitEnabled.equalsIgnoreCase("true"))
        {
            Log.log("commitWaitEnabled: " + commitWaitEnabled);
            String waitValue = aBall.iSuite.getAttributeValue(
                                   "Nokia-MIDlet-Install-Commit-Wait");
            if (waitValue != null && waitValue.equalsIgnoreCase(aWaitMsg))
            {
                try
                {
                    int waitTime = 30; // secs
                    Log.log("Waiting " + waitTime + " secs " + aWaitMsg);
                    Thread.sleep(waitTime * 1000);
                }
                catch (InterruptedException ie)
                {
                }
            }
        }
    }
}
