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

import com.nokia.mj.impl.installer.InstallerResultMessage;
import com.nokia.mj.impl.installer.applicationregistrator.ApplicationRegistrator;
import com.nokia.mj.impl.installer.applicationregistrator.SifRegistrator;
import com.nokia.mj.impl.installer.customisationproperties.CustomisationProperties;
import com.nokia.mj.impl.installer.downloader.Downloader;
import com.nokia.mj.impl.installer.exetable.ExeBall;
import com.nokia.mj.impl.installer.exetable.ExeStep;
import com.nokia.mj.impl.installer.integrityservice.IntegrityService;
import com.nokia.mj.impl.installer.jsrpluginnotifier.JsrPluginNotifier;
import com.nokia.mj.impl.installer.storagehandler.StorageHandler;
import com.nokia.mj.impl.installer.utils.Args;
import com.nokia.mj.impl.installer.utils.FileRoots;
import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.installer.utils.MidpAttributeValidator;
import com.nokia.mj.impl.installer.utils.SysUtil;
import com.nokia.mj.impl.security.midp.authentication.AuthenticationModule;
import com.nokia.mj.impl.security.midp.authentication.OcspSettings;
import com.nokia.mj.impl.utils.Uid;

public class PrepareInstallation extends ExeStep
{

    public void execute(ExeBall aBall)
    {
        InstallBall ball = (InstallBall)aBall;
        ball.log("Preparing installation...");

        ball.iCustomisationProperties = new CustomisationProperties();
        // Get default installation drive from CustomisationProperties...
        ball.iInstallationDrive =
            ((Integer)(ball.iCustomisationProperties.getIntegerProperty
                       (CustomisationProperties.DefaultInstallationDrive).
                       firstElement())).intValue();
        Log.log("CustomisationProperties called");
        // ...then check if we are making preinstallation
        // and set installation target drive accordingly...
        parseJadJarArgs(ball);
        checkForPreinstallation(ball);
        // ...and finally override installation drive with possible value
        // from command line.
        parseArgs(ball);
        FileUtils.setAppsRoot(ball.iInstallationDrive);
        Log.log("Args parsed");
        String isRoot = FileUtils.getIntegrityServiceRoot();
        ball.iIntegrityService = IntegrityService.getInstance(isRoot);
        Log.log("IntegrityService instantiated");
        ball.iStorageHandler = new StorageHandler();
        ball.iStorageHandler.startSession();
        Log.log("StorageSession created");
        ball.iApplicationRegistrator = new ApplicationRegistrator();
        ball.iApplicationRegistrator.startSession();
        Log.log("ApplicationRegistrator session created");
        ball.iSifRegistrator = new SifRegistrator();
        ball.iSifRegistrator.startSession(true);
        Log.log("SifRegistrator session created");
        ball.iJsrPluginNotifier = new JsrPluginNotifier(ball.iIntegrityService);
        Log.log("JsrPluginNotifier created");
        ball.iAttributeValidator = new MidpAttributeValidator();
        Log.log("MidpAttributeValidator created");
        AuthenticationModule.getInstance().setOCSPFlags(getOcspSettings(ball));

        ball.iInstallationNotifier.start(
            ball.iInstallationNotifier.INSTALLING);
    }

    public void cancel(ExeBall aBall)
    {
        InstallBall ball = (InstallBall)aBall;
        ball.log("Installation failed.");
        if (ball.getExecuteException() != null)
        {
            ball.log(ball.getExecuteException().toString());
        }

        // If posting OTA status notifications is still going on,
        // stop it now and continue when next installation is made.
        ball.getNotificationPoster().stop();

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
            uid, ball.iInstallationNotifier.INSTALL_FAIL);
    }

    private static void parseJadJarArgs(InstallBall aBall)
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

        String jadArg = args.get("jad");
        String jarArg = args.get("jar");
        if (jadArg != null && jarArg != null)
        {
            InstallerException.internalError
            ("Specify either Jad or Jar as an argument, not both.");
        }
        if (jarArg != null)
        {
            if (Downloader.isDownloadUrl(jarArg))
            {
                aBall.iJarUrl = jarArg;
            }
            else
            {
                aBall.iJarFilename = jarArg;
            }
        }
        if (jadArg != null)
        {
            if (Downloader.isDownloadUrl(jadArg))
            {
                aBall.iJadUrl = jadArg;
            }
            else
            {
                aBall.iJadFilename = jadArg;
            }
        }

        aBall.iSourceUrl = args.get("sourceurl", aBall.iSourceUrl);
    }

    private static void parseArgs(InstallBall aBall)
    {
        Args args = aBall.iArgs;

        // Parse string type arguments
        aBall.iIap = args.get("iap", aBall.iIap);
        aBall.iSnap = args.get("snap", aBall.iSnap);
        aBall.iJadCharset = args.get("charset", aBall.iJadCharset);
        aBall.iDownloadUsername = args.get("username", aBall.iDownloadUsername);
        aBall.iDownloadPassword = args.get("password", aBall.iDownloadPassword);

        String arg = args.get("proxyhost");
        if (arg != null)
        {
            System.setProperty("http.proxyHost", arg);
            aBall.log("HTTP proxy host set to " + arg);
        }

        arg = args.get("proxyport");
        if (arg != null)
        {
            System.setProperty("http.proxyPort", arg);
            aBall.log("HTTP proxy port set to " + arg);
        }

        // Parse boolean type arguments
        aBall.iCaptainMsgs = args.getBoolean("captainmsgs", aBall.iCaptainMsgs);
        aBall.iAllowUpgrade = args.getBoolean("upgrade", aBall.iAllowUpgrade);
        aBall.iUpgradeData = args.getBoolean("upgrade_data", aBall.iUpgradeData);
        aBall.iAllowOverwrite = args.getBoolean("overwrite", aBall.iAllowOverwrite);
        aBall.iAllowUntrusted = args.getBoolean("untrusted", aBall.iAllowUntrusted);
        aBall.iOcsp = args.getBoolean("ocsp", aBall.iOcsp);
        aBall.iIgnoreOcspWarnings = args.getBoolean("ignore_ocsp_warnings", aBall.iIgnoreOcspWarnings);
        aBall.iAllowDownload = args.getBoolean("download", aBall.iAllowDownload);
        aBall.iConversionInstallation = args.getBoolean("convert", aBall.iConversionInstallation);
        if (aBall.iConversionInstallation)
        {
            String filename = aBall.iJadFilename;
            if (filename == null)
            {
                filename = aBall.iJarFilename;
            }
            if (filename != null)
            {
                aBall.iConversionRoot = FileUtils.getParent(filename);
                int drive = FileUtils.getDrive(filename);
                if (drive != -1)
                {
                    // Make conversion installation to the same
                    // drive where the application already exists.
                    aBall.iInstallationDrive = drive;
                }
            }
            if (aBall.iConversionRoot == null ||
                    !FileUtils.exists(aBall.iConversionRoot))
            {
                InstallerException.internalError(
                    "Root dir for conversion installation does not exist: " +
                    aBall.iConversionRoot);
            }
            else
            {
                Log.log("Conversion installation from " + aBall.iConversionRoot);
            }
            // Conversion installation cannot be canceled.
            aBall.setCanCancel(false);
            // Uncomment the next line to disable conversion installation support.
            //InstallerException.internalError("Conversion installation not supported");
        }

        // Process "drive" argument after "convert" argument so that it can
        // be used to override the target installation drive.
        arg = args.get("drive");
        if (arg != null)
        {
            aBall.iInstallationDrive = args.parseDrive(arg);
        }
    }

    private static void checkForPreinstallation(InstallBall aBall)
    {
        String preinstallDir = FileRoots.getPreinstallDir();
        if (preinstallDir == null || preinstallDir.length() == 0)
        {
            return;
        }
        checkForPreinstallation(aBall, aBall.iJadFilename, preinstallDir);
        checkForPreinstallation(aBall, aBall.iJarFilename, preinstallDir);
    }

    private static void checkForPreinstallation(InstallBall aBall,
            String aFilename,
            String aPreinstallDir)
    {
        if ((aFilename != null) &&
                (aFilename.indexOf(aPreinstallDir) != -1) &&
                aBall.isSilent())
        {
            aBall.iPreinstallation = true;
            int drive = FileRoots.getPreinstallDrive(aFilename);
            if (drive != -1)
            {
                aBall.iInstallationDrive = drive;
            }
        }
    }

    private static OcspSettings getOcspSettings(InstallBall aBall)
    {
        int ocspMode = OcspSettings.OCSP_MODE_UNDEFINED;
        if (aBall.iOcsp != null)
        {
            if (aBall.iOcsp.booleanValue())
            {
                ocspMode = OcspSettings.OCSP_MODE_ENABLED;
            }
            else
            {
                ocspMode = OcspSettings.OCSP_MODE_DISABLED;
            }
        }
        int drive = -1;
        if (aBall.iJadFilename != null)
        {
            drive = FileUtils.getDrive(aBall.iJadFilename);
        }
        else if (aBall.iJarFilename != null)
        {
            drive = FileUtils.getDrive(aBall.iJarFilename);
        }
        if (drive != -1 && SysUtil.isDriveReadOnly(drive))
        {
            // If installation is started from read-only drive
            // OCSP must never be made.
            ocspMode = OcspSettings.OCSP_MODE_DISABLED;
        }

        int ocspWarning = OcspSettings.OCSP_WARNING_UNDEFINED;
        if (aBall.iIgnoreOcspWarnings != null)
        {
            if (aBall.iIgnoreOcspWarnings.booleanValue())
            {
                ocspWarning = OcspSettings.OCSP_WARNING_IGNORE;
            }
            else
            {
                ocspWarning = OcspSettings.OCSP_WARNING_CONFIRM;
            }
        }

        OcspSettings ocspSettings =
            new OcspSettings(ocspMode, ocspWarning, aBall.isSilent(),
                             aBall.iIap, aBall.iSnap);
        Log.log("ocspSettings: " + ocspSettings);
        return ocspSettings;
    }
}
