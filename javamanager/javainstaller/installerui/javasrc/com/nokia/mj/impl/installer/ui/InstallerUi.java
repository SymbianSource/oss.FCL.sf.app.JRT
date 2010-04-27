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


package com.nokia.mj.impl.installer.ui;

import com.nokia.mj.impl.utils.exception.InstallerExceptionBase;
import com.nokia.mj.impl.utils.Logger;

/**
 * Base class for JavaInstaller user interfaces.
 *
 * JavaInstaller will instantiate the InstallerUI using InstallerUiFactory
 * and notify InstallerUI that installation/uninstallation has started by
 * calling started() method in the very early phase of
 * installation/uninstallation. In the end of installation/uninstallation
 * JavaInstaller will call ended() method, both in case of successful
 * operation and also in case when an error occurs.
 *
 * Between started() and ended() calls the JavaInstaller can call any
 * other InstallerUI methods in any order. JavaInstaller will update
 * the installation progress by calling updateProgress() with values
 * forwarding from 0 to 100. In case error occurs at any point,
 * JavaInstaller calls updateProgress() with values
 * going backward to 0 and error() in the end.
 * For every started() call there will be
 * corresponding ended() call in every case. Also for every download
 * there will be matching started(DownloadInfo) and ended(Downloadinfo)
 * calls even if error occurs during download.
 *
 * Those InstallerUI methods which are stated to return quickly in
 * their javadocs, must return quickly. InstallerUI must have
 * its own thread where UI updates are processed, so that UI updates
 * caused by these method calls do not block or slow down installer
 * execution.
 *
 * It is up to InstallerUI implmentation to decide how long it waits
 * for the user to answer to those methods which are stated to block
 * until user has answered.
 *
 * InstallerUI methods should not throw exceptions to JavaInstaller.
 * If InstallerUI method throws an exception, Javainstaller will
 * ignore it silently. In this case JavaInstaller will write an
 * error log entry, but will not display anything to user and will
 * not abort installation.
 */
public class InstallerUi
{
    /** Indicates that this UI instance is for installation. */
    public static final int MODE_INSTALL = 1;
    /** Indicates that this UI instance is for uninstallation. */
    public static final int MODE_UNINSTALL = 2;
    /** Indicates that this UI instance is for application conversion. */
    public static final int MODE_APP_CONVERSION = 3;

    protected int iMode = 0;
    protected InstallerUiListener iListener = null;

    /** InstallInfo initialized when confirm() is called. */
    protected InstallInfo iInstallInfo = null;
    /** UninstallInfo initialized when confirm() is called. */
    protected UninstallInfo iUninstallInfo = null;

    /** Index of the currently converted application
        in application conversion. */
    protected int iAppConversionCurrent = 0;
    /** Total number of applications in application conversion. */
    protected int iAppConversionTotal = 0;

    /** Flag telling if user has cancelled confirmation dialogs. */
    protected boolean iConfirmationsCanceled = false;

    /** Flag telling if OCSP indicator is on. */
    private boolean iOcspIndicator = false;

    /** Name of this class instance, used for logging. */
    private static String iThisClassName = null;

    /**
     * Constructor.
     */
    public InstallerUi()
    {
    }

    /**
     * Initialise InstallerUi after construction.
     */
    public void init(int aMode, InstallerUiListener aListener)
    {
        iMode = aMode;
        iListener = aListener;
        iThisClassName = getClassName(this);
    }

    /**
     * Returns InstallerUiListener.
     */
    public InstallerUiListener getListener()
    {
        return iListener;
    }

    /**
     * Returns InstallInfo after confirm() hgas been called.
     */
    public InstallInfo getInstallInfo()
    {
        return iInstallInfo;
    }

    /**
     * Returns UninstallInfo after confirm() hgas been called.
     */
    public UninstallInfo getUninstallInfo()
    {
        return iUninstallInfo;
    }

    /**
     * Notify InstallerUi that user has cancelled installation.
     * This method gets called when user cancels installation from
     * installation progress bar. This method further notifies
     * InstallerUiListener that installation has been canceled.
     */
    public void cancel()
    {
        if (iListener != null)
        {
            // Do cancellation in a separate thread
            // so that UI does not get blocked.
            new Thread(new Runnable()
            {
                public void run()
                {
                    iListener.cancel();
                }
            }, "InstallerUiCancelThread").start();
        }
    }

    /**
     * Cancels all confirmation dialogs that are currently displayed.
     */
    public void cancelConfirmations()
    {
        // This method must be implemented in those InstallerUi
        // subclasses which display confirmation dialogs to user.
        iConfirmationsCanceled = true;
    }

    /**
     * Confirm installation. UI should display an installation
     * confirmation dialog to the user. UI must update
     * aInstallInfo basing on user selections.
     * This method blocks until user has answered to the dialog.
     *
     * @param aInstallInfo installation information
     * @return true if user has accepted installation, false otherwise
     */
    public boolean confirm(InstallInfo aInstallInfo)
    {
        iInstallInfo = aInstallInfo;
        log("Confirm installation: " + iInstallInfo);
        return true;
    }

    /**
     * Confirm permissions. UI should display a permission
     * confirmation dialog to the user.
     * This method blocks until user has answered to the dialog.
     *
     * @param aPermissionInfo permission information
     * @return true if user has accepted the dialog, false if user
     * cancelled the dialog
     */
    public boolean confirmPermissions(PermissionInfo aPermissionInfo)
    {
        log("Confirm permissions: " + aPermissionInfo);
        return true;
    }

    /**
     * Confirm uninstallation. UI should display an uninstallation
     * confirmation dialog to the user.
     * This method blocks until user has answered to the dialog.
     *
     * @param aUninstallInfo uninstallation information
     * @return true if user has accepted uninstallation, false otherwise
     */
    public boolean confirm(UninstallInfo aUninstallInfo)
    {
        iUninstallInfo = aUninstallInfo;
        log("Confirm uninstallation: " + iUninstallInfo);
        return true;
    }

    /**
     * This method is used to notify UI that installation or
     * uninstallation has started. Upon this call UI could
     * for example display progress bar.
     * This method must return quickly.
     */
    public void started()
    {
        log("Started " + getTitle());
    }

    /**
     * This method is used to notify UI that installation or
     * uninstallation has progressed. Upon this call UI can
     * update progress bar.
     * This method must return quickly.
     *
     * @param aProgress progress value between 0 and 100
     */
    public void updateProgress(int aProgress)
    {
        log("Progress: " + aProgress + "%");
    }

    /**
     * This method is used to notify UI that installation or
     * uninstallation has ended. Upon this call UI can
     * stop displaying progress bar.
     * This method must return quickly.
     */
    public void ended()
    {
        log("Ended " + getTitle());
    }

    /**
     * This method is used to notify UI that a download has started.
     * Upon this call UI can for example prepare to display a download
     * progress bar.
     * NOTE: When this method is called, the totalSize in DownlodInfo
     * is not yet initialized.
     * This method must return quickly.
     *
     * @param aDownloadInfo information about download
     */
    public void started(DownloadInfo aDownloadInfo)
    {
        log("Download started: " + aDownloadInfo);
    }

    /**
     * This method is used to notify UI that a download
     * has progressed. Upon this call UI can
     * update download progress bar.
     * This method must return quickly.
     *
     * @param aDownloadInfo information about download
     */
    public void updateProgress(DownloadInfo aDownloadInfo)
    {
        log("Download progress: " + aDownloadInfo.getCurrentSize() + " bytes");
    }

    /**
     * This method is used to notify UI that a download
     * has ended. Upon this call UI can stop
     * displaying download progress bar.
     * This method must return quickly.
     *
     * @param aDownloadInfo information about download
     */
    public void ended(DownloadInfo aDownloadInfo)
    {
        log("Download ended: " + aDownloadInfo);
    }

    /**
     * Set OCSP indicator on or off.
     *
     * @param aOn true when OCSP is started, false when OCSP is stopped
     */
    public void setOcspIndicator(boolean aOn)
    {
        log("setOcspIndicator: " + aOn);
        iOcspIndicator = aOn;
    }

    /**
     * Returns true if OCSP indicator is on.
     */
    public boolean getOcspIndicator()
    {
        return iOcspIndicator;
    }

    /**
     * Update application conversion indicator.
     *
     * @param aCurrent Index of the current application (1..total).
     * @param aTotal Total number of applications. Setting aTotal to
     * zero indicates that application conversion has been completed
     * and ui can be disposed.
     */
    public void updateAppConversionIndicator(int aCurrent, int aTotal)
    {
        log("updateAppConversionIndicator: " + aCurrent + "/" + aTotal);
        iAppConversionCurrent = aCurrent;
        iAppConversionTotal = aTotal;
    }

    /**
     * Notify user that an error has occurred.
     * This method must return quickly.
     *
     * @param aInstallerException exception indicating the error reason
     */
    public void error(InstallerExceptionBase aInstallerException)
    {
        String appName = getAppName();
        if (appName != null)
        {
            log("Error during " + getTitle() + " of " +
                appName + ": " + aInstallerException.toString());
        }
        else
        {
            log("Error: " + aInstallerException.toString());
        }
    }

    /**
     * Ask username and password for http authentication.
     * This method blocks until user has answered to the dialog.
     *
     * @param aUrl url for which authentication is required
     * @return Array of two strings, the first being username
     * and second being password. If username and password
     * cannot be obtained, this method returns null.
     */
    public String[] getUsernamePassword(String aUrl)
    {
        // Subclass should ensure that confirm() is called successfully
        // before displaying UsernamePassword dialog to user.
        log("Get username and password returns empty strings");
        return new String[] { "", "" };
    }

    /**
     * Asks from the user if the installed application should be launched.
     *
     * @param aLaunchAppInfo Information about application that can
     * be launched. Unless the user cancels the query,
     * this data will be filled in with user's answer upon return.
     * @return true if the user has chosen to launch the application,
     * false if the user has canceled the query
     */
    public boolean launchAppQuery(LaunchAppInfo aLaunchAppInfo)
    {
        log("Launch app query: " + aLaunchAppInfo);
        return false;
    }

    /**
     * Writes an info log entry to JavaInstaller log.
     *
     * @param aMsg message to be logged
     */
    public static void log(String aMsg)
    {
        if (Logger.Activated[Logger.EJavaInstaller])
        {
            String msg = iThisClassName + ": " + aMsg;
            Logger.LOG(Logger.EJavaInstaller, Logger.EInfo, msg);
        }
    }

    /**
     * Writes an error log entry to JavaInstaller log.
     *
     * @param aMsg message to be logged
     * @param aThrowable Throwable to be logged
     */
    public static void logError(String aMsg, Throwable aThrowable)
    {
        String msg = iThisClassName + ": " + aMsg;
        if (aThrowable == null)
        {
            Logger.ELOG(Logger.EJavaInstaller, msg);
        }
        else
        {
            Logger.ELOG(Logger.EJavaInstaller, msg, aThrowable);
        }
    }

    /**
     * Returns name of the application to be installed/uninstalled,
     * or null if the name is not available.
     */
    protected String getAppName()
    {
        String appName = null;
        if (iInstallInfo != null)
        {
            appName = iInstallInfo.getName();
        }
        else if (iUninstallInfo != null)
        {
            appName = iUninstallInfo.getName();
        }
        return appName;
    }

    /**
     * Returns string title basing on mode of this InstallerUi.
     */
    protected String getTitle()
    {
        String result = null;
        if (iMode == MODE_INSTALL)
        {
            result = "Installing";
        }
        else if (iMode == MODE_UNINSTALL)
        {
            result = "Uninstalling";
        }
        else if (iMode == MODE_APP_CONVERSION)
        {
            result = "Converting application " +
                     iAppConversionCurrent + " / " + iAppConversionTotal;
        }
        return result;
    }

    /**
     * Get class name without package.
     */
    protected static String getClassName(Object aObject)
    {
        String result = aObject.getClass().getName();
        int dotIndex = result.lastIndexOf('.');
        if (dotIndex > 0)
        {
            result = result.substring(dotIndex + 1);
        }
        return result;
    }
}
