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

import com.nokia.mj.impl.installer.ui.ApplicationInfo;
import com.nokia.mj.impl.installer.ui.LaunchAppInfo;
import com.nokia.mj.impl.installer.applicationregistrator.ApplicationRegistrator;
import com.nokia.mj.impl.installer.captainservice.CaptainService;
import com.nokia.mj.impl.installer.exetable.ExeBall;
import com.nokia.mj.impl.installer.exetable.ExeStep;
import com.nokia.mj.impl.installer.Installer;
import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.installer.utils.Platform;
import com.nokia.mj.impl.utils.Uid;

import java.io.InputStream;
import java.io.IOException;
import java.util.Vector;
import java.util.jar.JarFile;
import java.util.jar.JarEntry;

public class FinalizeInstallation extends ExeStep
{
    // JarFile instance, used for getting icon InputStreams.
    private JarFile iJarFile = null;

    public void execute(ExeBall aBall)
    {
        InstallBall ball = (InstallBall)aBall;
        ball.log("Finalizing installation...");

        // If posting OTA status notifications is still going on,
        // stop it now and continue when next installation is made.
        ball.getNotificationPoster().stop();

        // Cleanup security components.
        ball.cleanupSecurity();

        // If -forcecancel option was specified, abort execution
        // before committing anything.
        if (ball.iArgs.get("forcecancel") != null)
        {
            InstallerException.internalError("FORCED CANCEL");
        }

        // User is not allowed to cancel installation after this step.
        ball.setCanCancel(false);

        checkWaitAttribute(ball, "before storage commit");

        // StorageHandler must be committed before ApplicationRegistrator
        // so that platform specific application registry can use the data
        // in Java Storage (S60 AppArc application list re-scan)
        ball.iStorageHandler.commitSession();

        // StorageHandler commit is asynchronous. In case of
        // conversion installation or preinstallation wait for
        // a while so that the commit can be done before the
        // application registrator commit causes accesses to
        // Storage (S60 AppArc application list re-scan)
        if (ball.iConversionInstallation ||
                ball.iPreinstallation)
        {
            try
            {
                Thread.sleep(1000);
            }
            catch (InterruptedException ie)
                { }
        }

        Log.log("StorageHandler committed");

        checkWaitAttribute(ball, "after storage commit");

        ball.iSifRegistrator.commitSession();
        Log.log("SifRegistrator committed");

        checkWaitAttribute(ball, "after sif commit");

        // ApplicationRegistrator session must be commited before
        // IntegrityService session so that temp icon files
        // can be removed.
        // Make synchronous commit so that the application
        // becomes visible to all parts of the system before
        // installer finishes.
        ball.iApplicationRegistrator.commitSession(true);

        Log.log("ApplicationRegistrator committed");

        checkWaitAttribute(ball, "after apparc commit");

        boolean result = ball.iIntegrityService.commit();
        if (!result)
        {
            InstallerException.internalError("IntegrityService commit failed");
        }
        Log.log("IntegrityService committed");

        checkWaitAttribute(ball, "after integrityservice commit");

        ball.iInstallationNotifier.finish(
            ball.iSuite.getUid(), ball.iInstallationNotifier.INSTALL_OK);

        // All sessions have been committed, do not throw
        // exceptions anymore after this point!

        Uid[] uids = ball.iSuite.getApplicationUids();

        try
        {
            // Notify platform that applications have been added or updated.
            ball.iSifRegistrator.notifyAppChange(
                uids, (ball.iOldSuite != null?
                       ball.iSifRegistrator.APP_UPDATED:
                       ball.iSifRegistrator.APP_ADDED));
        }
        catch (Throwable t)
        {
            Log.logError("SifRegistrator.notifyAppChange failed", t);
        }

        if (ball.iCaptainMsgs)
        {
            // Notify JavaCaptain that application has been installed.
            Uid[] oldUids = null;
            if (ball.iOldSuite != null)
            {
                oldUids = ball.iOldSuite.getApplicationUids();
            }
            ball.getCaptainService().appUpdated(oldUids, uids);
            Log.log("JavaCaptain notified");
        }

        // Add an entry to platform installation log.
        ball.iApplicationRegistrator.addInstallLogEntry(ball.iSuite, 0);

        String midletName = ball.getAttributeValue("MIDlet-Name");
        ball.log("Application " + midletName + " successfully installed.");
        ball.log(ball.iSuite.toShortString());

        // Store the uids to Installer static variable
        // in case CommsInstaller has to send them to
        // TCK Runner or preinstaller
        Installer.setInstalledUids(ball.iSuite.getUid(), uids);

        if (!ball.isSilent())
        {
            if (ball.getInstallerUi() != null && Platform.isS60())
            {
                if (ball.LAUNCH_APP_QUERY)
                {
                    // Set installation progress to 100% before
                    // displaying application launch query.
                    ball.iInstallationNotifier.set(
                        ball.iInstallationNotifier.getMax());
                    // Display launch application query to the user.
                    createLaunchAppQueryThread(ball);
                }
            }
            if (Platform.isLinux() && ball.iCaptainMsgs)
            {
                // Launch the installed application
                try
                {
                    // CaptainService.launchApp() uses RLC message.
                    //ball.getCaptainService().launchApp(uids[0]);
                    // CaptainService.startApp() uses installer specific
                    // message.
                    ball.getCaptainService().startApp(new Uid[] { uids[0] });
                }
                catch (InstallerException ie)
                {
                    Log.log("Launching application failed: " + ie.toString());
                }
            }
        }
    }

    public void cancel(ExeBall aBall)
    {
        // nop
    }

    /**
     * Constructs a new LaunchAppInfo object basing on given InstallBall.
     */
    private LaunchAppInfo createLaunchAppInfo(InstallBall aBall)
    {
        // Get suite icon InputStream.
        InputStream suiteIconInputStream = null;
        String suiteIconPath = aBall.iSuite.getAttributeValue("MIDlet-Icon");
        if (suiteIconPath != null)
        {
            try
            {
                if (iJarFile == null)
                {
                    // Temp files have already been deleted,
                    // get the icon from installed jar file.
                    iJarFile = new JarFile(aBall.iSuite.getJarPath());
                }
                suiteIconInputStream =
                    iJarFile.getInputStream
                    (new JarEntry(FileUtils.trimJarEntry(suiteIconPath)));
                Log.log("LaunchAppInfo: suite icon " + suiteIconPath);
            }
            catch (IOException ioe)
            {
                Log.logWarning("Getting InputStream for suite icon failed", ioe);
            }
        }

        // Get application icon InputStreams.
        InputStream[] appIconInputStreams = null;
        Vector appInfos = aBall.iSuite.getApplications();
        if (appInfos != null)
        {
            appIconInputStreams = new InputStream[appInfos.size()];
            try
            {
                if (iJarFile == null)
                {
                    // Temp files have already been deleted,
                    // get the icon from installed jar file.
                    iJarFile = new JarFile(aBall.iSuite.getJarPath());
                }
                for (int i = 0; i < appIconInputStreams.length; i++)
                {
                    String iconPath =
                        ((com.nokia.mj.impl.installer.storagehandler.ApplicationInfo)
                         appInfos.elementAt(i)).getIconPath();
                    if (iconPath != null && iconPath.length() > 0)
                    {
                        appIconInputStreams[i] =
                            iJarFile.getInputStream
                            (new JarEntry(FileUtils.trimJarEntry(iconPath)));
                        Log.log("LaunchAppInfo: app icon " + i + ": " + iconPath);
                    }
                    else
                    {
                        appIconInputStreams[i] = null;
                        Log.log("LaunchAppInfo: app icon " + i + ": null");
                    }
                }
            }
            catch (IOException ioe)
            {
                Log.logWarning("Getting InputStream for application icon failed", ioe);
            }
        }

        return new LaunchAppInfo
               (ConfirmInstallation.getApplicationInfos(aBall),
                appIconInputStreams, suiteIconInputStream, suiteIconPath);
    }

    /**
     * Creates and starts a new thread for asking launch
     * application query from the user.
     */
    private void createLaunchAppQueryThread(InstallBall aBall)
    {
        final InstallBall ball = aBall;
        ball.iDialogOpen = true;
        new Thread(new Runnable()
        {
            public void run()
            {
                // Create and init LaunchAppinfo object.
                LaunchAppInfo launchAppInfo = createLaunchAppInfo(ball);
                // Display the query.
                boolean launchApp =
                    ball.getInstallerUi().launchAppQuery(launchAppInfo);

                // Close the jarFile and icon InputStreams.
                if (iJarFile != null)
                {
                    try
                    {
                        iJarFile.close(); // Closes also InputStreams
                        iJarFile = null;
                    }
                    catch (IOException ioe)
                    {
                        Log.logWarning("Closing icon InputStreams failed", ioe);
                    }
                }

                if (ball.iSifRegistrator.getSifMode() > 0 && launchApp)
                {
                    ball.getInstallerUi().syncExec(new Runnable()
                    {
                        // launchAppView() must be called in the UI thread.
                        public void run()
                        {
                            ball.iSifRegistrator.launchAppView();
                        }
                    });
                }
                else
                if (ball.iCaptainMsgs && launchApp &&
                        launchAppInfo.getApplications() != null &&
                        launchAppInfo.getApplications().length > 0)
                {
                    int selection = launchAppInfo.getSelection();
                    Log.log("Launching application " + selection + ": " +
                            launchAppInfo.getApplications()[selection]
                            .getName());
                    ball.getCaptainService().startApp
                    (new Uid[] { launchAppInfo
                                 .getApplications()[selection]
                                 .getUid()
                               });
                }
                else
                {
                    Log.log("No application launch");
                }

                synchronized (ball)
                {
                    ball.iDialogOpen = false;
                    // Notify WaitForLaunchAppQuery step that
                    // we are finished with launch app query.
                    ball.notify();
                }
            }
        }, "LaunchAppQueryThread").start();
    }

    /**
     * Wait a moment if Nokia-MIDlet-Install-Commit-Wait atribute has been set.
     */
    private static void checkWaitAttribute(InstallBall aBall, String aWaitMsg)
    {
        String commitWaitEnabled = System.getProperty(
                                       "com.nokia.mj.impl.installer.commitwaitenabled");
        if (commitWaitEnabled != null &&
                commitWaitEnabled.equalsIgnoreCase("true"))
        {
            Log.log("commitWaitEnabled: " + commitWaitEnabled);
            String waitValue = aBall.getAttributeValue(
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
