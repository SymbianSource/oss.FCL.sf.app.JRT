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


package com.nokia.mj.impl.installer;

import com.nokia.mj.impl.comms.CommsListener;
import com.nokia.mj.impl.comms.CommsMessage;
import com.nokia.mj.impl.comms.CommsPermission;
import com.nokia.mj.impl.comms.CommsServerEndpoint;
import com.nokia.mj.impl.installer.captainservice.CaptainService;
import com.nokia.mj.impl.installer.exetable.ExeBall;
import com.nokia.mj.impl.installer.exetable.ExeEngine;
import com.nokia.mj.impl.installer.exetable.ExeTable;
import com.nokia.mj.impl.installer.applicationregistrator.SifRegistrator;
import com.nokia.mj.impl.installer.integrityservice.IntegrityService;
import com.nokia.mj.impl.installer.pushregistrator.PushInfo;
import com.nokia.mj.impl.installer.pushregistrator.PushRegistrator;
import com.nokia.mj.impl.installer.storagehandler.OtaStatusHandler;
import com.nokia.mj.impl.installer.storagehandler.OtaStatusNotification;
import com.nokia.mj.impl.installer.storagehandler.StorageHandler;
import com.nokia.mj.impl.installer.storagehandler.SuiteInfo;
import com.nokia.mj.impl.installer.utils.Args;
import com.nokia.mj.impl.installer.utils.DriveInfo;
import com.nokia.mj.impl.installer.utils.FileRoots;
import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.installer.utils.Platform;
import com.nokia.mj.impl.installer.utils.PlatformUid;
import com.nokia.mj.impl.installer.utils.SysUtil;
import com.nokia.mj.impl.installer.utils.ThreadDumper;
import com.nokia.mj.impl.rt.installer.ApplicationInfoImpl;
import com.nokia.mj.impl.rt.installer.ApplicationUtilsImpl;
import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.rt.support.JvmInternal;
import com.nokia.mj.impl.rt.support.ThreadEventListener;
import com.nokia.mj.impl.rt.utils.ExtensionUtil;
import com.nokia.mj.impl.security.common.Certificate;
import com.nokia.mj.impl.security.midp.authentication.AuthenticationModule;
import com.nokia.mj.impl.storage.StorageNames;
import com.nokia.mj.impl.fileutils.FileOperations;
import com.nokia.mj.impl.fileutils.FileUtility;
import com.nokia.mj.impl.utils.StartUpTrace;
import com.nokia.mj.impl.utils.Tokenizer;
import com.nokia.mj.impl.utils.Uid;

import java.io.IOException;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;

/**
 * JavaInstaller main class. JavaInstaller can be started either with
 * #main(String[]) or #mainWithResult(String[]) methods. Negative return
 * values indicate that execution has failed.
 *
 * @author Nokia Corporation
 * @version $Rev: 10413 $
 */
public class Installer
{
    /** Installer state initial value. */
    public static final int STATE_UNDEFINED = 0;
    /** Installer state indicating that installation is ongoing. */
    public static final int STATE_INSTALLING = 1;
    /** Installer state indicating that uninstallation is ongoing. */
    public static final int STATE_UNINSTALLING = 2;
    /** Installer state indicating that componentinfo operation is ongoing. */
    public static final int STATE_COMPONENT_INFO = 3;

    /** Return value for successful execution. */
    public static final int ERR_NONE = 0;
    /**
     * Return value indicating error when JavaInstaller is started without
     * command line parameters or required system properties are not found.
     */
    public static final int ERR_NOT_FOUND = -1;
    /** Return value indicating general error. */
    public static final int ERR_GENERAL = -2;
    /** Return value indicating that operation was cancelled. */
    public static final int ERR_CANCEL = -3;
    /**
     * Return value which indicates that installer must not exit with
     * System.exit(). This return value is only used for internally.
     */
    static final int RET_NO_SYSTEM_EXIT = 100;

    // ThreadDumper instance.
    private static ThreadDumper iThreadDumper = null;

    // Installer CommsServer.
    private static CommsServerEndpoint iInstallerServer = null;

    // The Uids of the installed Java Applications (MIDlets etc).
    // Used to return information efficiently to preinstaller, appconverter
    // and TCK Runner when Installer is running in poll mode.
    static Uid iInstalledSuite = null;
    static Uid[] iInstalledApps = {};
    public static void setInstalledUids(Uid aSuiteUid, Uid[] aAppUids)
    {
        iInstalledSuite = aSuiteUid;
        iInstalledApps = aAppUids;
    }

    // Installer state.
    private static int iInstallerState = STATE_UNDEFINED;
    public static int getInstallerState()
    {
        return iInstallerState;
    }

    // Static cancel method needs access to current execution ball.
    private static ExeBall iExeBall = null;
    public static void cancel()
    {
        if (iExeBall != null)
        {
            Log.log("Installer.cancel: static cancel");
            iExeBall.cancel();
        }
        else
        {
            Log.log("Installer.cancel: nothing to cancel");
        }
    }

    /** Returns exception which caused installation or uninstallation
        to fail or null if execution was successful. */
    public static Exception getExecuteException()
    {
        if (iExeBall != null)
        {
            return iExeBall.getExecuteException();
        }
        return null;
    }

    /**
     * Installer main program which exits with System.exit() call.
     *
     * @param aArgs command line arguments
     */
    public static void main(String[] aArgs)
    {
        String traceMsg = "Installer.main starts";
        StartUpTrace.doTrace(traceMsg);
        Log.log(traceMsg);

        ExtensionUtil.handleExtensions();

        System.out.println("java.version: " +
                           System.getProperty("java.version"));
        System.out.println("java.fullversion: " +
                           System.getProperty("java.fullversion"));

        try
        {
            JvmInternal.setThreadEventListener(new ThreadEventListener()
            {
                public void threadStarting(Thread newThread, Thread parentThread) {}
                public void threadDied(Thread thread) {}
                public void uncaughtException(Thread thread, Throwable throwable)
                {
                    String threadName = null;
                    if (thread != null)
                    {
                        threadName = thread.getName();
                    }
                    Log.logError("Installer uncaught exception in " +
                                 threadName, throwable);
                }
            });
        }
        catch (Throwable t)
        {
            Log.logError("Exception from setThreadEventListener", t);
        }

        try
        {
            iThreadDumper = ThreadDumper.getInstance();
        }
        catch (Throwable t)
        {
            Log.logError("Exception from ThreadDumper.getInstance", t);
        }

        try
        {
            // Create and start Installer CommsServer.
            iInstallerServer = new CommsServerEndpoint();
            iInstallerServer.start(11100);
            iInstallerServer.registerListener(0, new InstallerListener());
        }
        catch (Throwable t)
        {
            Log.logError("Exception when creating InstallerServer", t);
        }

        int exitCode = ERR_NONE;

        if (Platform.isS60())
        {
            Log.log("Running on S60 platform");
            // Set file.separator and path.separator properties for S60.
            System.setProperty("file.separator", "\\");
            System.setProperty("path.separator", ";");
            // Load JavaInstaller native dll.
            try
            {
                Jvm.loadSystemLibrary("javainstaller");
            }
            catch (Exception ex)
            {
                Log.logError("Loading javainstaller dll failed", ex);
                exitCode = ERR_NOT_FOUND;
            }
        }
        else
        {
            Log.log("Running on Linux platform");
            // Set file.separator and path.separator properties for Linux.
            System.setProperty("file.separator", "/");
            System.setProperty("path.separator", ":");
        }
        Log.log("FileUtils.pathSeparator: " + FileUtils.pathSeparator());
        // Set com.nokia.rt.port system property so that installer
        // specific ApplicationInfoImpl and ApplicationUtilsImpl
        // are used.
        System.setProperty("com.nokia.rt.port", "installer");

        if (exitCode == ERR_NONE)
        {
            exitCode = mainWithResult(aArgs);
        }

        try
        {
            // Stop and destroy Installer CommsServer.
            if (iInstallerServer != null)
            {
                iInstallerServer.stop();
                iInstallerServer.destroy();
                iInstallerServer = null;
            }
        }
        catch (Throwable t)
        {
            Log.logError("Exception when destroying InstallerServer", t);
        }

        if (iThreadDumper != null)
        {
            iThreadDumper.destroy();
            iThreadDumper = null;
        }

        // Notify possible runtime listeners that JavaInstaller
        // is about to exit.
        ApplicationUtilsImpl.doShutdownImpl();
        if (exitCode == RET_NO_SYSTEM_EXIT)
        {
            traceMsg = "Installer.main exits with no code";
            Log.log(traceMsg);
            StartUpTrace.doTrace(traceMsg);
        }
        else
        {
            // System.exit() must be called for the VM to shut down.
            // Note that this is forced shutdown, which means that
            // it is not guaranteed that all finalizers are called.
            traceMsg = "Installer.main exits with code " + exitCode;
            Log.log(traceMsg);
            StartUpTrace.doTrace(traceMsg);
            System.exit(exitCode);
        }
    }

    /**
     * Installer main program which returns status code indicating
     * operation result. Negative status codes indicate errors.
     *
     * @param aArgs command line arguments
     * @return status code indicating operation result
     */
    public static int mainWithResult(String[] aArgs)
    {
        // Clean static variables to ensure they are not left initialized
        // from previous mainWithResult execution.
        iInstalledApps = new Uid[0];
        iInstallerState = STATE_UNDEFINED;
        iExeBall = null;

        if (aArgs.length < 1)
        {
            usage(null);
            return ERR_NOT_FOUND;
        }

        String javaBinRoot = System.getProperty("JAVA_BIN_ROOT");
        if (javaBinRoot == null || javaBinRoot.equals(""))
        {
            Log.log("System property JAVA_BIN_ROOT is not defined.");
            return ERR_NOT_FOUND;
        }
        else
        {
            Log.log("JAVA_BIN_ROOT=" + javaBinRoot);
        }

        StartUpTrace.doTrace("Installer.mainWithResult " + aArgs[0] + " begins");
        StringBuffer buf = new StringBuffer("Java install process started with command: ");
        for (int i = 0; i < aArgs.length; i++)
        {
            buf.append(" ").append(aArgs[i]);
        }
        Log.logInfoPrd(buf.toString());
        Log.logMemory("mainWithResult mem info");

        int exitCode = ERR_NONE;
        Args args = new Args(aArgs);

        if (iThreadDumper != null)
        {
            iThreadDumper.startTimer(10*60*1000); // 10 mins
        }

        try
        {
            // Do IntegrityService rollback every time installer is started.
            if (args.valueContains("log", "is"))
            {
                IntegrityService.setOperationsLogged(true);
            }
            if (!IntegrityService.rollback(
                        FileUtils.getIntegrityServiceRoot()))
            {
                Log.logError(
                    "IntegrityService static rollback failed, isRoot: " +
                    FileUtils.getIntegrityServiceRoot());
                exitCode = ERR_GENERAL;
            }

            boolean silent = (args.get("silent") != null? true: false);
            InstallationNotifier installationNotifier =
                new InstallationNotifier();
            ExeEngine engine = new ExeEngine(installationNotifier);
            if (aArgs[0].equals("install"))
            {
                iInstallerState = STATE_INSTALLING;
                com.nokia.mj.impl.installer.midp2.install.steps.InstallBall
                ball = new
                com.nokia.mj.impl.installer.midp2.install.steps.InstallBall();
                iExeBall = ball;
                ball.iArgs = args;
                ball.setSilent(silent);
                ball.iInstallationNotifier = installationNotifier;
                ExeTable table = getMidp2InstallTable();
                engine.execute(table, ball);
                if (ball.getExecuteException() != null)
                {
                    if (ball.isCancelled())
                    {
                        exitCode = ERR_CANCEL;
                    }
                    else
                    {
                        exitCode = ERR_GENERAL;
                    }
                }
            }
            else if (aArgs[0].equals("uninstall"))
            {
                iInstallerState = STATE_UNINSTALLING;
                com.nokia.mj.impl.installer.midp2.uninstall.steps.UninstallBall
                ball = new
                com.nokia.mj.impl.installer.midp2.uninstall.steps.UninstallBall();
                iExeBall = ball;
                ball.iArgs = args;
                ball.setSilent(silent);
                ball.iInstallationNotifier = installationNotifier;
                ExeTable table = getMidp2UninstallTable();
                engine.execute(table, ball);
                if (ball.getExecuteException() != null)
                {
                    if (ball.isCancelled())
                    {
                        exitCode = ERR_CANCEL;
                    }
                    else
                    {
                        exitCode = ERR_GENERAL;
                    }
                }
            }
            else if (aArgs[0].equals("componentinfo"))
            {
                iInstallerState = STATE_COMPONENT_INFO;
                exitCode = (new GetComponentInfo()).getComponentInfo(args);
            }
            else if (aArgs[0].equals("uninstallall"))
            {
                exitCode = uninstallAll(args, aArgs);
            }
            else if (aArgs[0].equals("poll"))
            {
                exitCode = poll(aArgs);
            }
            else if (aArgs[0].equals("rollback"))
            {
                // IntegrityService rollback is made in the beginning
                // of this try block. Do nothing here.
            }
            else if (aArgs[0].equals("list"))
            {
                list(args);
            }
            else if (aArgs[0].equals("launch"))
            {
                launch(args);
            }
            else if (aArgs[0].equals("test"))
            {
                test(args, aArgs);
            }
            else if (aArgs[0].equals("help"))
            {
                usage(aArgs);
            }
            else if (aArgs[0].equals("version"))
            {
                version(args);
            }
            else if (aArgs[0].equals("register"))
            {
                registerProperties(args);
            }
            else if (aArgs[0].equals("unregister"))
            {
                unregisterProperties(args);
            }
            else if (aArgs[0].equals("removealldata"))
            {
                exitCode = removeAllData(args);
            }
            else
            {
                usage(null);
            }
        }
        catch (Throwable t)
        {
            Log.logError("Unexpected error", t);
            exitCode = ERR_GENERAL;
        }

        // Remove tmp dir from installer private dir if it exists.
        String installerTmpDir = FileUtils.getInstallerRoot() + "tmp";
        if ((new FileUtility(installerTmpDir)).isDirectory())
        {
            try
            {
                boolean result = FileOperations.deleteAll(installerTmpDir);
                if (result)
                {
                    Log.log("Removed " + installerTmpDir);
                }
                else
                {
                    Log.logWarning("Failed to remove " + installerTmpDir);
                }
            }
            catch (IOException ioe)
            {
                Log.logWarning("Failed to remove " + installerTmpDir, ioe);
            }
        }

        if (iThreadDumper != null)
        {
            iThreadDumper.stopTimer();
        }

        if (args.get("nosystemexit") != null)
        {
            Log.log("System.exit() disabled, ignoring exit code " + exitCode);
            exitCode = RET_NO_SYSTEM_EXIT;
        }
        Log.logMemory("mainWithResult mem info");
        if (exitCode == ERR_NONE)
        {
            Log.logInfoPrd("Java install process exits");
        }
        else
        {
            Log.logInfoPrd("Java install process exit code " + exitCode);
        }

        StartUpTrace.doTrace("Installer.mainWithResult " + aArgs[0] + " ends");
        return exitCode;
    }

    private static ExeTable getMidp2InstallTable()
    {
        ExeTable table = new ExeTable();
        com.nokia.mj.impl.installer.midp2.install.steps.ConvertIcons
        convertIconsStep = new
        com.nokia.mj.impl.installer.midp2.install.steps.ConvertIcons();
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  CreateUi());
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  SendInstallerResultMessage());
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  SendOtaStatus());
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  PrepareInstallation());
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  DownloadJad());
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  FindJadJar()); // Either jad or jar must be present in
        // the device for this step.
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  GetAttributes());
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  GetFromStorage());
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  StartProgressNotifications());
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  SelectUids()); // Select suite uid and application uids.
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  AuthenticateJad()); // This must be the first security
        // related step.
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  GrantJadPermissions()); // Permission granting must be
        // made after authentication step.
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  AuthenticateJar());
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  GrantJarPermissions()); // Permission granting must be
        // made after authentication step.
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  CheckDiskSpace());
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  WaitForOcsp()); // Wait for OCSP to be completed before
        // starting download.
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  DownloadJar()); // In case dl can be started before
        // showing confirmation dialog.
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  ConfirmInstallation()); // Show UI confirmation dialog.
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  DownloadJar()); // If dl has not yet been started, start it now.
        table.add(convertIconsStep);
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  WaitForDownload());
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  GetAttributes()); // For reading jar if not yet done.
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  SelectUids()); // Select application uids now that jar is
        // available.
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  CheckDiskSpace()); // Check again now that also jar is available.
        table.add(convertIconsStep);
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  AuthenticateJar()); // If not yet done, check now that
        // jar is available.
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  GrantJarPermissions());  // Permission granting is made
        // after authentication step if needed.
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  ConfirmPermissions()); // Show UI confirmation dialog.
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  HandleCustomAttributes());
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  CheckJarPackages());
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  StopApplication());
        // Update new application info in the following steps.
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  AddToStorage());
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  RegisterPush());
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  RegisterApplication());
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  RegisterApplicationToSif());
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  CopyAppFiles());
        if (Platform.isS60())   // PrepareSplashScreen uses eSWT which is
        {
            // not available in Linux.
            table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                      PrepareSplashScreen()); // Create splash screen images
            // after app dir exists (after CopyAppFiles step).
        }
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  NotifyJsrPlugins());
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  WaitForPermissionsQuery());
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  FinalizeInstallation());
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  SendOtaStatus());
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  SendInstallerResultMessage());
        table.add(new com.nokia.mj.impl.installer.midp2.install.steps.
                  WaitForLaunchAppQuery());
        return table;
    }

    private static ExeTable getMidp2UninstallTable()
    {
        ExeTable table = new ExeTable();
        table.add(new com.nokia.mj.impl.installer.midp2.uninstall.steps.
                  CreateUi());
        table.add(new com.nokia.mj.impl.installer.midp2.uninstall.steps.
                  SendInstallerResultMessage());
        table.add(new com.nokia.mj.impl.installer.midp2.uninstall.steps.
                  PrepareUninstallation());
        table.add(new com.nokia.mj.impl.installer.midp2.uninstall.steps.
                  GetFromStorage());
        table.add(new com.nokia.mj.impl.installer.midp2.uninstall.steps.
                  StartProgressNotifications());
        table.add(new com.nokia.mj.impl.installer.midp2.uninstall.steps.
                  CheckUninstallationAllowed());
        table.add(new com.nokia.mj.impl.installer.midp2.uninstall.steps.
                  CheckDiskSpace());
        table.add(new com.nokia.mj.impl.installer.midp2.uninstall.steps.
                  ConfirmUninstallation()); // Show UI confirmation dialog.
        table.add(new com.nokia.mj.impl.installer.midp2.uninstall.steps.
                  StopApplication());
        table.add(new com.nokia.mj.impl.installer.midp2.uninstall.steps.
                  NotifyJsrPlugins());
        table.add(new com.nokia.mj.impl.installer.midp2.uninstall.steps.
                  UnregisterApplicationFromSif());
        table.add(new com.nokia.mj.impl.installer.midp2.uninstall.steps.
                  UnregisterApplication());
        table.add(new com.nokia.mj.impl.installer.midp2.uninstall.steps.
                  UnregisterPush());
        table.add(new com.nokia.mj.impl.installer.midp2.uninstall.steps.
                  RemoveSecurityData());
        table.add(new com.nokia.mj.impl.installer.midp2.uninstall.steps.
                  RemoveFromStorage());
        table.add(new com.nokia.mj.impl.installer.midp2.uninstall.steps.
                  RemoveAppFiles());
        table.add(new com.nokia.mj.impl.installer.midp2.uninstall.steps.
                  FinalizeUninstallation());
        table.add(new com.nokia.mj.impl.installer.midp2.uninstall.steps.
                  SendOtaStatus());
        table.add(new com.nokia.mj.impl.installer.midp2.uninstall.steps.
                  SendInstallerResultMessage());
        return table;
    }

    /**
     * Main program for listing installed application suites.
     */
    private static void list(Args aArgs)
    {
        boolean verbose = (aArgs.get("v") != null? true: false);
        String otaStatusArg = aArgs.get("otastatus");
        String systemPropertiesArg = aArgs.get("systemproperties");
        String storageArg = aArgs.get("storage");

        if (systemPropertiesArg != null)
        {
            // CDC code for logging system properties.
            Hashtable props = System.getProperties();
            Enumeration e = props.keys();
            Log.logOut("System properties:");
            while (e.hasMoreElements())
            {
                String key = (String)e.nextElement();
                String value = (String)props.get(key);
                Log.logOut(key + ": " + value);
            }
        }

        if (storageArg != null)
        {
            // List contents of Storage tables.
            StorageHandler sh = new StorageHandler();
            String[] tables = getStorageTables(storageArg);
            for (int i = 0; i < tables.length; i++)
            {
                String storageEntriesString =
                    sh.getStorageEntriesString(tables[i]);
                if (storageEntriesString != null)
                {
                    Log.logOut(storageEntriesString);
                }
            }
            sh.close();
        }

        if (otaStatusArg != null)
        {
            // List pending OTA status notifications.
            OtaStatusNotification[] notifications =
                new OtaStatusHandler().getNotifications();
            if (notifications != null)
            {
                for (int i = 0; i < notifications.length; i++)
                {
                    Log.logOut(notifications[i].toString() + "\n");
                }
            }
        }

        if (storageArg == null && otaStatusArg == null &&
                systemPropertiesArg == null)
        {
            // By default list installed application suites.
            SifRegistrator sr = new SifRegistrator();
            sr.startSession(false);
            StorageHandler sh = new StorageHandler();
            Uid[] uids = sh.getSuiteUids();
            if (uids != null)
            {
                for (int i = 0; i < uids.length; i++)
                {
                    SuiteInfo si = new SuiteInfo(uids[i]);
                    if (sh.readSuiteInfo(si))
                    {
                        StringBuffer sb = new StringBuffer();
                        // List SuiteInfo.
                        if (verbose)
                        {
                            sb.append(si.toString());
                        }
                        else
                        {
                            sb.append(si.toShortString());
                        }
                        // List push connections.
                        sb.append(pushConnectionsToString(si.getApplicationUids()));
                        if (verbose)
                        {
                            // List certificate details.
                            Certificate[] certs =
                                AuthenticationModule.getInstance()
                                .getCertificatesDetails(sh.getSession(),
                                                        uids[i]);
                            if (certs != null)
                            {
                                for (int j = 0; j < certs.length; j++)
                                {
                                    sb.append("Certificate[").append(j).append("]:\n");
                                    sb.append(certs[j].toString()).append("\n");
                                }
                            }
                        }
                        Log.logOut(sb.toString());
                        sr.logComponent(si.getGlobalId());
                        Vector apps = si.getApplications();
                        for (int j = 0; j < apps.size(); j++)
                        {
                            sr.logComponent(si.getGlobalId(j));
                        }
                    }
                }
            }
            sh.close();
            sr.closeSession();
        }
    }

    /**
     * Returns string containing push connection information for
     * specified applications.
     */
    private static String pushConnectionsToString(Uid[] aAppUids)
    {
        StringBuffer result = new StringBuffer();
        PushRegistrator pushRegistrator = null;
        try
        {
            // Instantiate PushRegistrator implementation.
            Class clazz = Class.forName(PushInfo.PUSH_REGISTRATOR_CLASS);
            pushRegistrator = (PushRegistrator)clazz.newInstance();
        }
        catch (Exception ex)
        {
            // Ignore silently, just return empty string.
            return result.toString();
        }

        // Found PushRegistrator, ask PushInfos for each application.
        for (int i = 0; aAppUids != null && i < aAppUids.length; i++)
        {
            PushInfo[] pushInfos =
                pushRegistrator.getPushInfos(aAppUids[i]);
            if (pushInfos != null && pushInfos.length > 0)
            {
                result.append("Push connections:\n");
            }
            for (int j = 0; pushInfos != null && j < pushInfos.length; j++)
            {
                result.append("  ").append(pushInfos[j]).append("\n");
            }
        }
        return result.toString();
    }

    /**
     * Returns an array of Storage table names from given
     * string argument. If argument contains no table names
     * then names of all Storage tables are returned.
     */
    private static String[] getStorageTables(String aStorageArg)
    {
        String[] result = null;
        if (aStorageArg == null || aStorageArg.length() == 0)
        {
            result = new String[]
            {
                StorageNames.APPLICATION_PACKAGE_TABLE,
                StorageNames.APPLICATION_TABLE,
                StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE,
                StorageNames.MIDP_PACKAGE_TABLE,
                StorageNames.MIDP_PERMISSIONS_TABLE,
                StorageNames.MIDP_FUNC_GRP_SETTINGS_TABLE,
                StorageNames.PUSH_REGISTRATIONS_TABLE,
                StorageNames.ALARM_REGISTRATIONS_TABLE,
                StorageNames.RUNTIME_SETTINGS_TABLE,
                StorageNames.PREINSTALL_TABLE,
                //StorageNames.OTA_STATUS_TABLE,
            };
        }
        else
        {
            result = Tokenizer.split(aStorageArg, ",");
        }
        return result;
    }

    /**
     * Main program for uninstalling all installed applications.
     */
    private static int uninstallAll(Args aArgs, String[] aStringArgs)
    {
        String uidArg = aArgs.get("uid");
        if (uidArg != null)
        {
            Log.log("Uid must not be specified for uninstallall command.");
            return ERR_GENERAL;
        }
        int result = ERR_NONE;
        StorageHandler sh = new StorageHandler();
        try
        {
            Uid[] uids = sh.getSuiteUids();
            if (uids != null)
            {
                String[] stringArgs = new String[aStringArgs.length + 1];
                for (int i = 0; i < aStringArgs.length; i++)
                {
                    if (aStringArgs[i].equals("uninstallall"))
                    {
                        stringArgs[i] = "uninstall";
                    }
                    else
                    {
                        stringArgs[i] = aStringArgs[i];
                    }
                }

                final int NO_DRIVE = -2;
                int uninstallationDrive = NO_DRIVE;
                String arg = aArgs.get("drive");
                if (arg != null)
                {
                    uninstallationDrive = aArgs.parseDrive(arg);
                }

                int countOk = 0, countNok = 0;
                Log.log("Starting to uninstall " + uids.length +
                        " applications");
                for (int i = 0; i < uids.length; i++)
                {
                    SuiteInfo suite = new SuiteInfo(uids[i]);
                    if (sh.readSuiteInfo(suite))
                    {
                        int suiteDrive = FileUtils.getDrive(suite.getRootDir());
                        if (uninstallationDrive != NO_DRIVE &&
                                uninstallationDrive != suiteDrive)
                        {
                            // Drive option has been specified, but current
                            // suite is from different drive so it must not
                            // be uninstalled.
                            Log.log("Application " + uids[i] + " from drive " +
                                    suiteDrive + " not uninstalled");
                            break;
                        }
                    }

                    stringArgs[stringArgs.length-1] = "-uid=" + uids[i];
                    int newResult = mainWithResult(stringArgs);
                    if (newResult != ERR_NONE)
                    {
                        countNok++;
                        result = newResult;
                        Log.logError("Uninstalling application " + uids[i] +
                                     " failed, result=" + result);
                    }
                    else
                    {
                        Log.log("Application " + uids[i] + " uninstalled");
                        countOk++;
                    }
                }
                Log.log("Successfully uninstalled " + countOk + " applications");
                if (countNok > 0)
                {
                    Log.log("Uninstalling " + countNok + " applications failed");
                }
            }
        }
        finally
        {
            if (sh != null)
            {
                sh.close();
            }
        }
        return result;
    }

    /**
     * Main program for removing OMJ directories from Java runtime
     * directory and removing all data from JavaStorage.
     */
    private static int removeAllData(Args aArgs)
    {
        int result = ERR_NONE;
        // Remove OMJ data directories.
        try
        {
            Vector driveInfos = new Vector();
            SysUtil.getUserVisibleDrives(driveInfos);
            for (int i = 0; i < driveInfos.size(); i++)
            {
                DriveInfo driveInfo = (DriveInfo)driveInfos.elementAt(i);
                int tmpResult = removeAllData(driveInfo.getNumber());
                if (tmpResult != ERR_NONE)
                {
                    result = tmpResult;
                }
            }
        }
        catch (Throwable t)
        {
            Log.logError("Removing data failed", t);
            result = ERR_GENERAL;
        }
        // Remove data from JavaStorage.
        try
        {
            if (!StorageHandler.removeAllData())
            {
                result = ERR_GENERAL;
            }
        }
        catch (Throwable t)
        {
            Log.logError("Removing JavaStorage data failed", t);
            result = ERR_GENERAL;
        }
        // Remove property definitions.
        unregisterProperties(aArgs);
        return result;
    }

    /**
     * Removes OMJ directories from Java runtime directory
     * from specified drive.
     */
    private static int removeAllData(int aDrive)
    {
        int result = ERR_NONE;
        String midpRoot = FileRoots.getMidpRoot();
        String[] privateDirs =
        {
            "apps", "installer", "resource", "security"
        };
        for (int i = 0; i < privateDirs.length; i++)
        {
            String privateDir = FileUtils.setDrive(
                                    midpRoot + privateDirs[i], aDrive);
            try
            {
                FileUtility file = new FileUtility(privateDir);
                if (file.isDirectory())
                {
                    if (FileOperations.deleteAll(privateDir))
                    {
                        Log.log("Removed " + privateDir);
                    }
                    else
                    {
                        Log.logError("Removing " + privateDir + " failed");
                        result = ERR_GENERAL;
                    }
                }
            }
            catch (IOException ioe)
            {
                Log.logError("Removing " + privateDir + " failed", ioe);
                result = ERR_GENERAL;
            }
        }
        return result;
    }

    /**
     * Main program for starting JavaInstaller in polling mode.
     * Installer send Comms messages to get actual install/uninstall tasks.
     */
    private static int poll(String[] aStringArgs)
    {
        CommsInstaller poller = new CommsInstaller(aStringArgs);
        return poller.run();
    }

    /**
     * Main program for launching an application.
     */
    private static void launch(Args aArgs)
    {
        String uidArg = aArgs.get("uid");
        if (uidArg == null)
        {
            Log.logOut("Uid must be specified for launch command.");
            return;
        }
        Uid uid = PlatformUid.createUid(uidArg);
        if (uid == null)
        {
            Log.logOut("Invalid uid " + uidArg);
            return;
        }
        CaptainService captainService = new CaptainService(false);
        Log.logOut("Launching application with uid " + uid);
        captainService.launchApp(uid);
    }

    /**
     * Main program for executing tests within JavaInstaller runtime.
     */
    private static void test(Args aArgs, String[] aStringArgs)
    {
        String mainClass = aArgs.get("mainclass");
        if (mainClass == null)
        {
            Log.logOut("Mainclass must be specified for test command.");
            return;
        }

        String domain = aArgs.get("domain");
        if (domain != null)
        {
            ApplicationInfoImpl appInfoImpl =
                (ApplicationInfoImpl)ApplicationInfo.getInstance();
            appInfoImpl.setProtectionDomain(domain);
            Log.logOut("Set protection domain to " +
                       appInfoImpl.getProtectionDomain());
        }

        String server = aArgs.get("server");
        if (server != null)
        {
            String serverProperty = "com.nokia.mj.impl.installer.test.server";
            System.setProperty(serverProperty, server);
            Log.logOut("Set " + serverProperty + " to " +
                       System.getProperty(serverProperty));
        }

        for (int i = 0; i < 10; i++)
        {
            String param = aArgs.get("param" + i);
            if (param != null)
            {
                String paramProperty =
                    "com.nokia.mj.impl.installer.test.param" + i;
                System.setProperty(paramProperty, param);
                Log.logOut("Set " + paramProperty + " to " +
                           System.getProperty(paramProperty));
            }
        }

        // Disable ExeBall.ForceExitThread so that also long lasting
        // tests can be executed.
        System.setProperty("com.nokia.mj.impl.installer.disableforceexit",
                           "true");

        try
        {
            // Instantiate mainclass and call its main() method.
            Class clazz = Class.forName(mainClass);
            InstallerMain instMain = (InstallerMain)clazz.newInstance();
            Log.logOut("Starting test class " + mainClass);
            instMain.installerMain(aStringArgs);
        }
        catch (Exception ex)
        {
            InstallerException.internalError("Running mainclass failed", ex);
        }
    }

    /**
     * Prints usage information to log and system output.
     */
    private static void usage(String[] aArgs)
    {
        String cmd = "javainstaller";
        String usageMsg =
            "Usage: "+cmd+" <command> [options]\n"+
            "\n"+
            "where command is one of:\n"+
            "  help          Prints help on "+cmd+" usage.\n"+
            "  version       Prints version information.\n"+
            "  componentinfo Get information of an installed or not\n"+
            "                installed Java application.\n"+
            "  install       Install or upgrade Java application suite.\n"+
            "  uninstall     Uninstall Java application suite.\n"+
            "  uninstallall  Uninstall all Java application suites.\n"+
            "  list          List installed Java application suites.\n"+
            "  launch        Launch an installed Java application.\n"+
            "  poll          TCK Runner / preinstaller / appconverter optimised mode.\n"+
            "  rollback      Do only IntegrityService rollback, and nothing else.\n"+
            "  removealldata Removes all OMJ specific data from Java\n"+
            "                runtime private directory and JavaStorage.\n"+
            "  register      Registers all system wide properties installer uses.\n"+
            "  unregister    Unregisters all system wide properties installer uses.\n"+
            "  test          Execute test cases within JavaInstaller runtime.\n"+
            "\n"+
            "Type '"+cmd+" help <command>' for help on a specific command.\n";
        if (aArgs == null || aArgs.length <= 1)
        {
            Log.logOut(usageMsg);
            return;
        }

        String helpCmd =
            "Gives list of available commands or command specific help.\n"+
            "\n"+
            "Usage: "+cmd+" help [command]\n"+
            "\n"+
            "where\n"+
            "  \"command\" is the command to get help for\n";

        String versionCmd =
            "Prints version information.\n"+
            "\n"+
            "Usage: "+cmd+" version\n";

        String componentInfoCmd =
            "Get information of an installed or not installed Java application.\n"+
            "\n"+
            "Usage: "+cmd+" componentinfo [options]\n"+
            "\n"+
            "where options are:\n"+
            "  -jad=jad_file                 Path to Jad file.\n"+
            "  -jar=jar_file                 Path to Jar file.\n"+
            "  -charset=charset              Internet standard character set name telling\n"+
            "                                charset used in Jad file.\n"+
            "  -uid=uid                      Suite or application uid identifying the\n"+
            "                                application suite.\n"+
            "  -commsresult=endpoint         In the end of operation, send\n"+
            "                                InstallerResultMessage to specified comms\n"+
            "                                endpoint.\n"+
            "\n"+
            "Either -jad, -jar or -uid must be specified, other options are optional.\n";

        String installCmd =
            "Installs or upgrades Java application suite.\n"+
            "\n"+
            "Usage: "+cmd+" install [options]\n"+
            "\n"+
            "where options are:\n"+
            "  -jad=jad_file                 Path or URL to Jad file.\n"+
            "  -jar=jar_file                 Path or URL to Jar file.\n"+
            "  -charset=charset              Internet standard character set name telling\n"+
            "                                charset used in Jad file.\n"+
            "  -sourceurl=sourceurl          Source URL where given Jad file was\n"+
            "                                downloaded.\n"+
            "  -iap=iap_id                   Internet access point to be used in\n"+
            "                                downloads and status notification posting\n"+
            "                                during installation.\n"+
            "  -snap=snap_id                 Service network access point to be used in\n"+
            "                                downloads and status notification posting\n"+
            "                                during installation.\n"+
            "  -drive=target_drive           Installation target drive (C,D,E,...).\n"+
            "                                This option is valid only in S60 platform.\n"+
            "  -silent                       Perform silent installation.\n"+
            "  -upgrade=yes|no               Is upgrade allowed, default is yes.\n"+
            "  -upgrade_data=yes|no          Retain or remove application private data\n"+
            "                                in upgrade, default is MIDP specified\n"+
            "                                behaviour.\n"+
            "  -overwrite=yes|no             Allow upgrade with the same version,\n"+
            "                                default is yes.\n"+
            "  -untrusted=yes|no             Allow installing untrusted application,\n"+
            "                                default is yes.\n"+
            "  -ocsp=yes|no                  Perform OCSP, default is yes.\n"+
            "  -ignore_ocsp_warnings=yes|no  Ignore OCSP warnings, default yes.\n"+
            "  -download=yes|no              Is download allowed, default is yes.\n"+
            "  -username=download_username   Username for HTTP authentication.\n"+
            "  -password=download_password   Password for HTTP authentication.\n"+
            "  -proxyhost=host               HTTP proxy host.\n"+
            "  -proxyport=port_number        HTTP proxy port number.\n"+
            "  -log=is                       Write log entries from IntegrityService\n"+
            "                                operations.\n"+
            "  -forcecancel                  Cancel installation before committing\n"+
            "                                anything. This option is for debugging\n"+
            "                                purposes only.\n"+
            "  -captainmsgs=yes|no           If set to no, JavaInstaller will not send\n"+
            "                                any messages to JavaCaptain during\n"+
            "                                installation. Default is yes.\n"+
            "  -skipjarcheck                 Skip application package scanning.\n"+
            "  -skipotastatus                Skip OTA status handling.\n"+
            "  -commsresult=endpoint         In the end of operation, send\n"+
            "                                InstallerResultMessage to specified comms\n"+
            "                                endpoint.\n"+
            "\n"+
            "Either -jad or -jar must be specified, other options are optional.\n";

        String uninstallCmd =
            "Uninstalls Java application suite.\n"+
            "\n"+
            "Usage: "+cmd+" uninstall [options]\n"+
            "\n"+
            "where options are:\n"+
            "  -uid=uid                      Suite or application uid identifying the\n"+
            "                                suite to be uninstalled.\n"+
            "  -cid=component_id             Component id identifying the application\n"+
            "                                to be uninstalled.\n"+
            "  -iap=iap_id                   Internet access point to be used in\n"+
            "                                uninstallation status notification posting.\n"+
            "  -snap=snap_id                 Service network access point to be used in\n"+
            "                                uninstallation status notification posting.\n"+
            "  -silent                       Perform silent uninstallation.\n"+
            "  -log=is                       Write log entries from IntegrityService\n"+
            "                                operations.\n"+
            "  -forceuninstall               Forces uninstallation even if uninstallation\n"+
            "                                is blocked with Nokia-MIDlet-Block-Uninstall\n"+
            "                                attribute.\n"+
            "  -forcecancel                  Cancel uninstallation before committing\n"+
            "                                anything. This option is for debugging\n"+
            "                                purposes only.\n"+
            "  -captainmsgs=yes|no           If set to no, JavaInstaller will not send\n"+
            "                                any messages to JavaCaptain during\n"+
            "                                uninstallation. Default is yes.\n"+
            "  -skipotastatus                Skip OTA status handling.\n"+
            "  -commsresult=endpoint         In the end of operation, send\n"+
            "                                InstallerResultMessage to specified comms\n"+
            "                                endpoint.\n"+
            "\n"+
            "The -uid option must be specified, other options are optional.\n";

        String uninstallAllCmd =
            "Uninstalls all installed Java application suites.\n"+
            "\n"+
            "Usage: "+cmd+" uninstallall [options]\n"+
            "\n"+
            "where options are the same as for uninstall command,\n"+
            "except that -uid option is not allowed.\n";

        String listCmd =
            "Lists installed Java applications.\n"+
            "\n"+
            "Usage: "+cmd+" list [options]\n"+
            "\n"+
            "where options are:\n"+
            "  -v                            More verbose output.\n"+
            "  -otastatus                    List pending OTA status notifications.\n"+
            "  -systemproperties             List system properties.\n"+
            "  -storage[=tbl1[,tbl2]...]     List contents of storage tables.\n"+
            "                                If table names are not specified\n"+
            "                                lists contents of all tables.\n";

        String launchCmd =
            "Launches an installed Java application.\n"+
            "\n"+
            "Usage: "+cmd+" launch [options]\n"+
            "\n"+
            "where options are:\n"+
            "  -uid=uid                      Application uid identifying the\n"+
            "                                application to be launched.\n";

        String pollCmd =
            "Uses Comms to get install/uninstall commands from TCK Runner, preinstaller or appconverter.\n"+
            "\n"+
            "Usage: "+cmd+" poll -address=(tck|preinstall|convert) [options]\n"+
            "\n"+
            "The specified options are used in install/uninstall operations if applicable.\n";

        String rollbackCmd =
            "Perform IntegrityService rollback, and do nothing else.\n"+
            "\n"+
            "Usage: "+cmd+" rollback [options]\n"+
            "\n"+
            "where options are:\n"+
            "  -log=is                       Write log entries from IntegrityService\n"+
            "                                operations.\n";

        String testCmd =
            "Execute test cases within JavaInstaller runtime.\n"+
            "\n"+
            "Usage: "+cmd+" test [options]\n"+
            "\n"+
            "where options are:\n"+
            "  -mainclass=InstallerMainClass Executes the given InstallerMainClass\n"+
            "                                in JavaInstaller runtime.\n"+
            "                                InstallerMainClass must implement\n"+
            "                                com.nokia.mj.impl.installer.utils.InstallerMain\n"+
            "                                interface.\n"+
            "  -domain=protection_domain     Set protection domain in which the tests\n"+
            "                                are executed. Possible values are:\n"+
            "                                MFD, OPD, ITPD, UTPD. Default is UTPD.\n";

        String msg = null;
        for (int i = 1; i < aArgs.length; i++)
        {
            if (aArgs[i].equals("help"))
            {
                msg = helpCmd;
            }
            else if (aArgs[i].equals("version"))
            {
                msg = versionCmd;
            }
            else if (aArgs[i].equals("componentinfo"))
            {
                msg = componentInfoCmd;
            }
            else if (aArgs[i].equals("install"))
            {
                msg = installCmd;
            }
            else if (aArgs[i].equals("uninstall"))
            {
                msg = uninstallCmd;
            }
            else if (aArgs[i].equals("uninstallall"))
            {
                msg = uninstallAllCmd;
            }
            else if (aArgs[i].equals("list"))
            {
                msg = listCmd;
            }
            else if (aArgs[i].equals("launch"))
            {
                msg = launchCmd;
            }
            else if (aArgs[i].equals("poll"))
            {
                msg = pollCmd;
            }
            else if (aArgs[i].equals("rollback"))
            {
                msg = rollbackCmd;
            }
            else if (aArgs[i].equals("removealldata"))
            {
                msg = "Removes all OMJ specific data from Java runtime directory and JavaStorage.\n";
            }
            else if (aArgs[i].equals("register"))
            {
                msg = "Registers all system wide properties installer uses.\n";
            }
            else if (aArgs[i].equals("unregister"))
            {
                msg = "Unregisters all system wide properties installer uses.\n";
            }
            else if (aArgs[i].equals("test"))
            {
                msg = testCmd;
            }
            else
            {
                msg = "Unknown command " + aArgs[i] + "\n";
            }
            Log.logOut(msg);
        }
    }

    /**
     * Prints version information to log and system output.
     */
    private static void version(Args aArgs)
    {
        String versionMsg =
            ApplicationInfo.getInstance().getName() + " " +
            ApplicationInfo.getInstance().getVersion() +
            " for Open Mobile Java\n";
        // Add OS info
        versionMsg += "OS: " + System.getProperty("os.name") +
                      " " + System.getProperty("os.version") + "\n";
        // Add VM info
        versionMsg += "VM name: " + System.getProperty("java.vm.name") + "\n";
        versionMsg += "VM version: " + System.getProperty("java.vm.version") +
                      " (" + System.getProperty("java.vm.info") + ")\n";
        versionMsg += "VM vendor: " + System.getProperty("java.vm.vendor") + "\n";
        // Add runtime info
        versionMsg += "Runtime: " + System.getProperty("java.runtime.name") +
                      " " + System.getProperty("java.runtime.version") + "\n";
        // Add user info
        //versionMsg += getProperty("user.name");
        //versionMsg += getProperty("user.home");
        // Add microedition info
        versionMsg += getProperty("microedition.platform");
        versionMsg += getProperty("microedition.configuration");
        versionMsg += getProperty("microedition.profiles");
        versionMsg += getProperty("microedition.locale");

        Log.logOut(versionMsg);
    }

    private static String getProperty(String aName)
    {
        StringBuffer buf = new StringBuffer();
        buf.append(aName).append(": ")
        .append(System.getProperty(aName)).append("\n");
        return buf.toString();
    }

    /**
     * Registers all system wide properties installer uses.
     */
    private static void registerProperties(Args aArgs)
    {
        InstallationNotifier.defineProperties();
    }

    /**
     * Unregisters all system wide properties installer has registered.
     */
    private static void unregisterProperties(Args aArgs)
    {
        InstallationNotifier.deleteProperties();
    }

    private static class InstallerListener implements CommsListener
    {
        public void processMessage(CommsMessage aMessage)
        {
            try
            {
                int result = ERR_GENERAL;
                Log.log("InstallerListener received " + aMessage);
                if (aMessage.hasPermission(CommsPermission.INSTALL_APPLICATION))
                {
                    int msgId = aMessage.getMessageId();
                    if (msgId == 603)
                    {
                        Installer.cancel();
                        result = ERR_NONE;
                    }
                }
                else
                {
                    Log.logWarning("InstallerListener received message " +
                                   "with no INSTALL_APPLICATION permission");
                }
                CommsMessage response = new CommsMessage();
                response.replyTo(aMessage);
                response.setMessageId(604);
                response.write(result);
                iInstallerServer.send(response);
                Log.log("InstallerListener sent " + response);
            }
            catch (Throwable t)
            {
                Log.logError("InstallerListener error", t);
            }
        }
    }
}
