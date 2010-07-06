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


package com.nokia.mj.impl.installer.ui.eswt;

import com.nokia.mj.impl.fileutils.FileUtility;
import com.nokia.mj.impl.installer.ui.ApplicationInfo;
import com.nokia.mj.impl.installer.ui.DownloadInfo;
import com.nokia.mj.impl.installer.ui.InstallerUi;
import com.nokia.mj.impl.installer.ui.InstallerUiListener;
import com.nokia.mj.impl.installer.ui.InstallInfo;
import com.nokia.mj.impl.installer.ui.LaunchAppInfo;
import com.nokia.mj.impl.installer.ui.PermissionInfo;
import com.nokia.mj.impl.installer.ui.UninstallInfo;
import com.nokia.mj.impl.rt.ui.RuntimeUi;
import com.nokia.mj.impl.rt.ui.RuntimeUiFactory;
import com.nokia.mj.impl.utils.ResourceUtil;
import com.nokia.mj.impl.utils.StartUpTrace;
import com.nokia.mj.impl.utils.exception.InstallerExceptionBase;

import java.io.InputStream;
import java.io.IOException;
import java.util.Hashtable;

import org.eclipse.ercp.swt.midp.UIThreadSupport;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ControlEvent;
import org.eclipse.swt.events.ControlListener;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.ImageData;
import org.eclipse.swt.graphics.ImageLoader;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Shell;

/**
 * JavaInstaller eSWT UI.
 */
public class InstallerUiEswt extends InstallerUi
{
    /** Default shell style. */
    private static final int SHELL_STYLE =
        SWT.BORDER | SWT.APPLICATION_MODAL | SWT.ON_TOP;

    private Shell iParent = null;
    private Shell iDialog = null;
    private ProgressView iProgressView = null;
    private ProgressView iDlProgressView = null;
    private ProgressView iOcspProgressView = null;
    private InstallConfirmationView iInstallConfirmationView = null;
    private UninstallConfirmationView iUninstallConfirmationView = null;
    private PermissionConfirmationView iPermissionConfirmationView = null;
    private UsernamePasswordView iUsernamePasswordView = null;
    private LaunchAppQueryView iLaunchAppQueryView = null;
    /** Synchronization object for waiting for the UI initialization. */
    private Object iInitWaitObject = new Object();
    /** Synchronization object for waiting for the UI termination. */
    private Object iExitWaitObject = new Object();
    /**
     * Synchronization object for synchronizing progress updates
     * and confirmation dialogs. Used to guard disabling and
     * getting iDisplayProgress variable.
     */
    private Object iProgressSyncObject = new Object();
    /** Flag telling if UI main thread exists. */
    private boolean iUiThreadExists = false;
    /** Name of the application to be installed. */
    private String iAppName = null;
    /** Security icon.  */
    private Image iSecurityIcon = null;
    /** Flag telling if progress bar should be displayed. */
    private boolean iDisplayProgress = false;
    /** Flag telling if the first progress bar update has been traced. */
    private boolean iProgressBarUpdateTraced = false;
    /** Flag telling if MinimalUi should be used if UI creation fails. */
    private boolean iMinimalUiEnabled = true;

    /** Hashtable for storing the loaded icons. */
    private static Hashtable iImageTable = null;

    /** Default shell bounds. */
    private Rectangle iDefaultShellBounds = null;
    private Rectangle iDefaultShellClientBounds = null;

    /** Bold font for views. */
    private Font iBoldFont = null;

    private ViewBase iActiveView = null;

    /**
     * Constructor.
     */
    public InstallerUiEswt()
    {
        super();
    }

    /**
     * Initialise InstallerUi after construction.
     */
    public void init(int aMode, InstallerUiListener aListener)
    {
        super.init(aMode, aListener);
        StartUpTrace.doTrace("InstallerUiEswt init");
        // Create a hashtable for icons.
        iImageTable = new Hashtable();
        // Create a new thread to be the UI main thread.
        UIThreadSupport.startInUIThread(new Runnable()
        {
            public void run()
            {
                uiMain();
            }
        });
        // To wait InstallerUi to be ready before installer main thread
        // continues, uncomment the following line.
        //waitForUi();
    }

    /**
     * This method is executed in UI main thread.
     */
    private void uiMain()
    {
        log("uiMain: thread started");
        iUiThreadExists = true;
        try
        {
            // Create the necessary views.
            Display display = new Display();
            StartUpTrace.doTrace("InstallerUiEswt display created");
            display.setAppName(""); // Remove display title.
            iParent = new Shell(display);
            iDialog = new Shell(iParent, SHELL_STYLE);
            iDefaultShellBounds = iDialog.internal_getDefaultBounds();
            iDefaultShellClientBounds = iDialog.getClientArea();
            iBoldFont = getBoldFont();
            StartUpTrace.doTrace("InstallerUiEswt shell created");
            iProgressView = new ProgressView(this, iDialog, getTitle());

            iParent.addControlListener(new CListener(this));
            log("InstallerUiEswt CListener added");

            display.addListener(SWT.Dispose, new Listener()
            {
                public void handleEvent(Event aEvent)
                {
                    log("Dispose event for display");
                    // Prevent UI from being automatically disposed.
                    //aEvent.doit = false;
                }
            });

            synchronized (iInitWaitObject)
            {
                // Notify that UI is now ready.
                iInitWaitObject.notify();
            }
            StartUpTrace.doTrace("InstallerUiEswt ready");

            // If there has been ui update requests, update the ui right
            // away.
            if (iMode == MODE_INSTALL && getOcspIndicator())
            {
                setOcspIndicator(getOcspIndicator());
            }
            else if (iMode == MODE_APP_CONVERSION &&
                     iAppConversionTotal > 0)
            {
                updateAppConversionIndicator(
                    iAppConversionCurrent, iAppConversionTotal);
            }

            // UI event loop must be executed in UI main thread,
            // which is the thread where Display is created.
            while (isUiReady())
            {
                if (!display.readAndDispatch())
                {
                    display.sleep();
                }
            }
            if (iBoldFont != null && !iBoldFont.isDisposed())
            {
                iBoldFont.dispose();
            }
            display.dispose();
            log("uiMain: display disposed");
            synchronized (iExitWaitObject)
            {
                // Notify that UI main thread has been terminated.
                iExitWaitObject.notify();
            }
        }
        catch (Throwable t)
        {
            logError("Exception in uiMain", t);
            // Release wait object in case someone is waiting for them.
            synchronized (iInitWaitObject)
            {
                iUiThreadExists = false;
                iInitWaitObject.notify();
            }
            synchronized (iExitWaitObject)
            {
                iExitWaitObject.notify();
            }
        }
        iUiThreadExists = false;
        log("uiMain: thread ended");
    }

    /**
     * Cancels all confirmation views that are currently displayed.
     */
    public void cancelConfirmations()
    {
        super.cancelConfirmations();
        if (iInstallConfirmationView != null)
        {
            iInstallConfirmationView.confirmCancel();
        }
        if (iUninstallConfirmationView != null)
        {
            iUninstallConfirmationView.confirmCancel();
        }
        if (iPermissionConfirmationView != null)
        {
            iPermissionConfirmationView.confirmCancel();
        }
        if (iUsernamePasswordView != null)
        {
            iUsernamePasswordView.confirmCancel();
        }
        if (iLaunchAppQueryView != null)
        {
            iLaunchAppQueryView.confirmCancel();
        }
        // Remove download progress bar if it visible.
        if (iDlProgressView != null && !iDlProgressView.isDisposed())
        {
            iDlProgressView.dispose();
            iDlProgressView = null;
        }
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
        super.confirm(aInstallInfo);

        waitForUi();
        boolean result = true;
        if (!isUiReady())
        {
            result = false;
            if (iMinimalUiEnabled)
            {
                result = MinimalUi.confirmStatic(aInstallInfo);
                log("MinimalUi installation confirmation returns " + result);
                return result;
            }
            else
            {
                // If UI is not ready by the time confirmation is requested,
                // throw an exception.
                throw new RuntimeException("JavaInstallerUi not ready");
            }
        }

        if (result)
        {
            StartUpTrace.doTrace("InstallerUiEswt confirm");
            if (iInstallConfirmationView == null)
            {
                final Display display = iParent.getDisplay();
                final InstallerUiEswt self = this;
                display.syncExec
                (new Runnable()
                {
                    public void run()
                    {
                        iInstallConfirmationView =
                            new InstallConfirmationView(self, iDialog);
                    }
                });
            }
            result = iInstallConfirmationView.confirm(aInstallInfo);
            iInstallConfirmationView.dispose();
            iInstallConfirmationView = null;
        }
        if (result)
        {
            iAppName = aInstallInfo.getName();
            if (iUsernamePasswordView != null)
            {
                // UsernamePasswordView blocks prompting until
                // app name is set so that username/password
                // prompt will not be displayed if the user
                // does not confirm installation.
                iUsernamePasswordView.setAppName(iAppName);
            }
            else
            {
                // UsernamePasswordView is not being displayed,
                // let's allow progress to be displayed.
                iDisplayProgress = true;
                // If download progress view is in use,
                // display it to user only after confirmation.
                if (iDlProgressView != null && !iDlProgressView.isVisible())
                {
                    iDlProgressView.setVisible(true);
                }
            }
        }
        else
        {
            // The install confirmation has been rejected,
            // nothing to display anymore.
            iParent.getDisplay().syncExec
            (new Runnable()
            {
                public void run()
                {
                    iParent.dispose();
                }
            });
        }
        log("Installation confirmation returns " + result);
        return result;
    }

    /**
     * Confirm permissions. UI should display a permission
     * confirmation dialog to the user.
     * This method blocks until user has answered to the dialog.
     *
     * @param aPermissionInfo permission information
     * @return true if user has accepted permissions, false otherwise
     */
    public boolean confirmPermissions(PermissionInfo aPermissionInfo)
    {
        super.confirmPermissions(aPermissionInfo);

        waitForUi();
        if (!isUiReady())
        {
            aPermissionInfo.setPermissionAllowed(false);
            return true;
        }

        synchronized (iProgressSyncObject)
        {
            // Do not display progress bar during dialog.
            iDisplayProgress = false;
        }
        if (iPermissionConfirmationView == null)
        {
            final Display display = iParent.getDisplay();
            final InstallerUiEswt self = this;
            display.syncExec
            (new Runnable()
            {
                public void run()
                {
                    iPermissionConfirmationView =
                        new PermissionConfirmationView(self, iDialog);
                }
            });
        }
        boolean result = iPermissionConfirmationView.confirm(
                             iInstallInfo, aPermissionInfo);
        iPermissionConfirmationView.dispose();
        iPermissionConfirmationView = null;
        iDisplayProgress = true;
        iProgressView.setVisible(true);
        log("Permission confirmation returns " + result +
            ", user selection " + aPermissionInfo.isPermissionAllowed());
        return result;
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
        super.confirm(aUninstallInfo);

        waitForUi();
        boolean result = true;
        if (!isUiReady())
        {
            result = false;
            if (iMinimalUiEnabled)
            {
                result = MinimalUi.confirmStatic(aUninstallInfo);
                log("MinimalUi uninstallation confirmation returns " + result);
                return result;
            }
            else
            {
                // If UI is not ready by the time confirmation is requested,
                // throw an exception.
                throw new RuntimeException("JavaInstallerUi not ready");
            }
        }
        if (result)
        {
            StartUpTrace.doTrace("InstallerUiEswt confirm");
            if (iUninstallConfirmationView == null)
            {
                final Display display = iParent.getDisplay();
                final InstallerUiEswt self = this;
                display.syncExec
                (new Runnable()
                {
                    public void run()
                    {
                        iUninstallConfirmationView =
                            new UninstallConfirmationView(self, iDialog);
                    }
                });
            }
            result = iUninstallConfirmationView.confirm(aUninstallInfo);
            iUninstallConfirmationView.dispose();
            iUninstallConfirmationView = null;
        }
        if (result)
        {
            iDisplayProgress = true;
        }
        else
        {
            // The uninstall confirmation has been rejected,
            // nothing to display anymore.
            iParent.getDisplay().syncExec
            (new Runnable()
            {
                public void run()
                {
                    iParent.dispose();
                }
            });
        }
        log("Uninstallation confirmation returns " + result);
        return result;
    }

    /**
     * This method is used to notify UI that installation or
     * uninstallation has started. Upon this call UI could
     * for example display progress bar.
     * This method must return quickly.
     */
    public void started()
    {
        super.started();
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
        super.updateProgress(aProgress);
        if (!isUiReady())
        {
            return;
        }
        // UI is created asynchronously, so it might be that
        // UI was not yet ready when started() was called.
        // Ensure that iProgressView has been opened before
        // updating it.
        synchronized (iProgressSyncObject)
        {
            if (iDlProgressView != null && iDlProgressView.isVisible())
            {
                // If download progress is being displayed,
                // do not display installation progress.
                return;
            }
            if (iDisplayProgress && !iProgressView.isVisible())
            {
                iProgressView.setVisible(true);
            }
            if (iDisplayProgress && !iProgressBarUpdateTraced)
            {
                StartUpTrace.doTrace(
                    "InstallerUiEswt progress " + aProgress + " %");
                iProgressBarUpdateTraced = true;
            }
        }
        iProgressView.updateProgress(aProgress);

    }

    /**
     * This method is used to notify UI that installation or
     * uninstallation has ended. Upon this call UI can
     * stop displaying progress bar.
     * This method must return quickly.
     */
    public void ended()
    {
        super.ended();
        if (!isUiReady())
        {
            return;
        }
        Display display = iParent.getDisplay();
        display.syncExec
        (new Runnable()
        {
            public void run()
            {
                iParent.dispose();
            }
        });
        log("ended: parent disposed");
        // Let's wait for ui to be properly terminated before returning.
        synchronized (iExitWaitObject)
        {
            try
            {
                if (!display.isDisposed())
                {
                    iExitWaitObject.wait();
                }
            }
            catch (InterruptedException ie)
            {
                // Ignore silently.
            }
        }
        log("ended returns");
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
        super.started(aDownloadInfo);
        if (!isUiReady())
        {
            return;
        }
        // Ensure that download progress bar is displayed and
        // updated to zero progress.
        long oldCurrentSize = aDownloadInfo.getCurrentSize();
        long oldTotalSize = aDownloadInfo.getTotalSize();
        aDownloadInfo.setCurrentSize(0);
        aDownloadInfo.setTotalSize(100);
        updateProgress(aDownloadInfo);
        aDownloadInfo.setCurrentSize(oldCurrentSize);
        aDownloadInfo.setTotalSize(oldTotalSize);
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
        super.updateProgress(aDownloadInfo);
        if (!isUiReady())
        {
            return;
        }

        if (iDlProgressView == null)
        {
            final boolean indeterminate =
                (aDownloadInfo.getTotalSize() <= 0);
            final InstallerUiEswt self = this;
            iParent.getDisplay().syncExec(new Runnable()
            {
                public void run()
                {
                    iDlProgressView = new ProgressView(
                        self, iDialog,
                        InstallerUiTexts.get(InstallerUiTexts.DOWNLOADING),
                        indeterminate);
                }
            });
            iDlProgressView.addCancelCommand();
        }

        synchronized (iProgressSyncObject)
        {
            if (iDisplayProgress && !iDlProgressView.isVisible())
            {
                iDlProgressView.setVisible(true);
            }
        }
        if (aDownloadInfo.getTotalSize() > 0)
        {
            int progress = (int)((aDownloadInfo.getCurrentSize()*100)/
                                 aDownloadInfo.getTotalSize());
            iDlProgressView.updateProgress(progress);
        }
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
        super.ended(aDownloadInfo);
        if (!isUiReady())
        {
            return;
        }
        if (iDlProgressView != null && !iDlProgressView.isDisposed())
        {
            iDlProgressView.dispose();
            iDlProgressView = null;
        }
        synchronized (iProgressSyncObject)
        {
            if (iDisplayProgress && !iProgressView.isVisible())
            {
                iProgressView.setVisible(true);
            }
        }
    }

    /**
     * Set OCSP indicator on or off.
     *
     * @param aOn true when OCSP is started, false when OCSP is stopped
     */
    public void setOcspIndicator(boolean aOn)
    {
        super.setOcspIndicator(aOn);
        waitForUi();
        if (!isUiReady())
        {
            log("UI not ready, could not set OCSP indicator to " + aOn);
            return;
        }
        if (aOn)
        {
            if (iOcspProgressView == null)
            {
                final InstallerUiEswt self = this;
                iParent.getDisplay().syncExec
                (new Runnable()
                {
                    public void run()
                    {
                        iOcspProgressView = new ProgressView(self, iDialog,
                                                             InstallerUiTexts.get(InstallerUiTexts.OCSP_CHECK_PROGRESS),
                                                             true);
                    }
                });
                iOcspProgressView.addCancelCommand();
            }
            if (iOcspProgressView != null)
            {
                if (!iOcspProgressView.isVisible())
                {
                    iOcspProgressView.setVisible(true);
                }
            }
        }
        else
        {
            if (iOcspProgressView != null)
            {
                if (iOcspProgressView.isVisible())
                {
                    iOcspProgressView.setVisible(false);
                }
                iOcspProgressView.dispose();
                iOcspProgressView = null;
            }
        }
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
        super.updateAppConversionIndicator(aCurrent, aTotal);
        if (!isUiReady())
        {
            log("UI not ready, could not update app conversion indicator");
            return;
        }
        int progress = 101;
        if (aTotal > 0)
        {
            progress = (aCurrent*100)/aTotal;
        }
        if (progress <= 100)
        {
            iProgressView.setText(getTitle());
            iProgressView.updateProgress(progress);
            if (!iProgressView.isVisible())
            {
                iProgressView.removeCancelCommand();
                iProgressView.setVisible(true);
            }
            iParent.getDisplay().syncExec(new Runnable()
            {
                public void run()
                {
                    // Ensure that conversion indicator
                    // is drawn to the foreground.
                    iParent.setMaximized(true);
                }
            });
        }
        else
        {
            iProgressView.updateProgress(100);
            // Call ended() method which will dispose the ui.
            ended();
        }
    }

    /**
     * Notify user that an error has occurred.
     * This method must return quickly.
     *
     * @param aInstallerException exception indicating the error reason
     */
    public void error(InstallerExceptionBase aInstallerException)
    {
        super.error(aInstallerException);

        waitForUi();
        // InstallerUi does not have to be ready as long as
        // RuntimeUi is used to display error messages.
        //if (!isUiReady()) {
        //    return;
        //}

        boolean identified = false;
        //String tmpAppName = null;
        if (iInstallInfo != null)
        {
            //tmpAppName = iInstallInfo.getName();
            if (iInstallInfo.getCertificates() != null)
            {
                identified = true;
            }
        }
        else if (iUninstallInfo != null)
        {
            //tmpAppName = iUninstallInfo.getName();
            if (iUninstallInfo.getCertificates() != null)
            {
                identified = true;
            }
        }
        String tmpTitle = "";
        if (iMode == MODE_INSTALL)
        {
            tmpTitle = InstallerUiTexts.get(InstallerUiTexts.INSTALL_FAILED);
        }
        else if (iMode == MODE_UNINSTALL)
        {
            tmpTitle = InstallerUiTexts.get(InstallerUiTexts.UNINSTALL_FAILED);
        }

        // Ensure that no confirmations are being displayed.
        cancelConfirmations();
        // Hide progress view before displaying error message.
        if (iProgressView != null)
        {
            iProgressView.setVisible(false);
        }
        // Use RuntimeUi to display error message.
        RuntimeUi runtimeUi = RuntimeUiFactory.getRuntimeUi(identified);
        runtimeUi.error(tmpTitle, aInstallerException);
        runtimeUi.destroy();

        /*
        // Display error message using eSWT MessageBox.
        final String appName = tmpAppName;
        final String title = tmpTitle;
        final String shortMsg = aInstallerException.getShortMessage();
        final String detailedMsg = aInstallerException.getDetailedMessage();
        // UI updates must be executed in UI thread.
        iParent.getDisplay().syncExec
            (new Runnable() {
                    public void run() {
                        if (detailedMsg == null || detailedMsg.length() == 0) {
                            // No detailed msg, display only short msg.
                            MessageBox messageBox = new MessageBox
                                (iParent, SWT.ICON_ERROR | SWT.OK);
                            messageBox.setText(title);
                            messageBox.setMessage
                                (getMessage(title, appName, shortMsg, false));
                            messageBox.open();
                        } else {
                            // Display both short and detailed msgs.
                            MessageBox messageBox = new MessageBox
                                (iParent, SWT.ICON_ERROR | SWT.YES | SWT.NO);
                            messageBox.setText(title);
                            messageBox.setMessage
                                (getMessage(title, appName, shortMsg, true));
                            int answer = messageBox.open();
                            if ((answer & SWT.YES) != 0) {
                                // User wants to see details, display them.
                                messageBox = new MessageBox
                                    (iParent, SWT.ICON_ERROR | SWT.OK);
                                messageBox.setText(title);
                                messageBox.setMessage
                                    (getMessage(title, appName, detailedMsg, false));
                                messageBox.open();
                            }
                        }
                    }
                    private String getMessage(String aTitle, String aAppName,
                                              String aMsg, boolean aDetailsQuery) {
                        //String result = aTitle + "\n\n";
                        String result = "";
                        if (aAppName == null) {
                            result += aMsg;
                        } else {
                            result += aAppName + "\n\n" + aMsg;
                        }
                        if (aDetailsQuery) {
                            result += "\n\n";
                            result += InstallerUiTexts.get
                                (InstallerUiTexts.DETAILS_QUERY);
                        }
                        return result;
                    }
                });

        */
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
        waitForUi();
        if (!isUiReady())
        {
            return null;
        }

        synchronized (iProgressSyncObject)
        {
            // Do not display progress bar during dialog.
            iDisplayProgress = false;
        }
        if (iUsernamePasswordView == null)
        {
            final Display display = iParent.getDisplay();
            final InstallerUiEswt self = this;
            display.syncExec(new Runnable()
            {
                public void run()
                {
                    iUsernamePasswordView =
                        new UsernamePasswordView(self, iDialog);
                }
            });
        }
        iUsernamePasswordView.setAppName(iAppName);
        final String[] usernamePassword =
            iUsernamePasswordView.getUsernamePassword(aUrl);
        iUsernamePasswordView.dispose();
        iUsernamePasswordView = null;
        iDisplayProgress = true;

        if (usernamePassword != null)
        {
            log("Username: " + usernamePassword[0]);
            // Do not write confidential information (password) to log.
            //log("Password: " + usernamePassword[1]);
        }
        else
        {
            log("No username and password provided");
        }
        return usernamePassword;
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
        waitForUi();
        if (!isUiReady() || iConfirmationsCanceled)
        {
            // Either UI is not yet ready, or user has cancelled
            // installation, in both cases do nothing.
            return false;
        }

        if (iLaunchAppQueryView == null)
        {
            final Display display = iParent.getDisplay();
            final InstallerUiEswt self = this;
            display.syncExec
            (new Runnable()
            {
                public void run()
                {
                    iLaunchAppQueryView =
                        new LaunchAppQueryView(self, iDialog);
                }
            });
        }

        boolean result = iLaunchAppQueryView.launchAppQuery(aLaunchAppInfo);
        iParent.getDisplay().syncExec
        (new Runnable()
        {
            public void run()
            {
                iParent.dispose();
            }
        });
        iLaunchAppQueryView = null;
        log("LaunchAppQuery returns " + result + " for " + aLaunchAppInfo);
        return result;
    }

    /**
     * Executes given Runnable synchronously in the UI thread.
     */
    public void syncExec(Runnable aRunnable)
    {
        iParent.getDisplay().syncExec(aRunnable);
    }

    /**
     * Returns string title basing on mode of this InstallerUi.
     */
    protected String getTitle()
    {
        String result = null;
        if (iMode == MODE_INSTALL)
        {
            result = InstallerUiTexts.get(InstallerUiTexts.INSTALLING);
        }
        else if (iMode == MODE_UNINSTALL)
        {
            result = InstallerUiTexts.get(InstallerUiTexts.UNINSTALLING);
        }
        else if (iMode == MODE_APP_CONVERSION)
        {
            result = InstallerUiTexts.get(
                         InstallerUiTexts.APP_CONVERSION_PROGRESS,
                         new Object[] { new Integer(iAppConversionCurrent),
                                        new Integer(iAppConversionTotal)
                                      });
        }
        return result;
    }

    /**
     * Returns icon for indicating identified or unidentified application.
     *
     * @param aDisplay display to which the icon will be added
     * @param aIdentified if true returns icon for identified application,
     * otherwise returns icon for unidentified application
     * @return icon for identified or unidentified application, or null
     * if icon cannot be loaded
     */
    protected Image getSecurityIcon(Display aDisplay, boolean aIdentified)
    {
        if (iSecurityIcon != null)
        {
            return iSecurityIcon;
        }
        String iconFilename = ResourceUtil.UNTRUSTED_ICON_NAME;
        if (aIdentified)
        {
            iconFilename = ResourceUtil.TRUSTED_ICON_NAME;
        }
        String resourceDir = ResourceUtil.getResourceDir(0);
        for (int i = 1; iSecurityIcon == null && resourceDir != null; i++)
        {
            iSecurityIcon = loadImage(aDisplay, resourceDir + iconFilename, false);
            resourceDir = ResourceUtil.getResourceDir(i);
        }
        return iSecurityIcon;
    }

    /**
     * Loads image from specified InputStream. This method scales the image
     * to optimum size.
     *
     * @param aDisplay display to which the icon will be added
     * @param aInputStream InputStream where the image is loaded
     * @param aImageName name of the image to be loaded
     * @return image from InputStream or null if image cannot be loaded
     */
    protected static Image loadImage(
        Display aDisplay, InputStream aInputStream, String aImageName)
    {
        return loadImage(aDisplay, aInputStream, aImageName, true);
    }

    /**
     * Loads image from specified file.
     *
     * @param aDisplay display to which the icon will be added
     * @param aImageFilename name of the image file to be loaded
     * @param aScaleImage flag telling if the loaded image should be scaled
     * @return image from file or null if image cannot be loaded
     */
    private static Image loadImage(
        Display aDisplay, String aImageFilename, boolean aScaleImage)
    {
        Image result = null;
        InputStream imageInputStream = null;
        try
        {
            FileUtility imageFile = new FileUtility(aImageFilename);
            if (imageFile.exists())
            {
                imageInputStream = imageFile.openInputStream();
                result = loadImage(aDisplay, imageInputStream,
                                   aImageFilename, aScaleImage);
            }
        }
        catch (IOException ioe)
        {
            log("Can not get InputStream for " + aImageFilename + ": " + ioe);
        }
        finally
        {
            if (imageInputStream != null)
            {
                try
                {
                    imageInputStream.close();
                    imageInputStream = null;
                }
                catch (IOException ioe)
                {
                    logError("Closing image InputStream failed", ioe);
                }
            }
        }
        return result;
    }

    /**
     * Loads image from specified InputStream.
     *
     * @param aDisplay display to which the icon will be added
     * @param aInputStream InputStream where the image is loaded
     * @param aImageName name of the image to be loaded
     * @param aScaleImage flag telling if the loaded image should be scaled
     * @return image from InputStream or null if image cannot be loaded
     */
    private static Image loadImage(
        Display aDisplay, InputStream aInputStream,
        String aImageName, boolean aScaleImage)
    {
        Image result = null;
        if (aImageName != null)
        {
            result = (Image)iImageTable.get(aImageName);
        }
        if (result != null)
        {
            log("Using already loaded image " + aImageName);
            return result;
        }
        try
        {
            long startTime = System.currentTimeMillis();
            ImageData[] imageDatas = new ImageLoader().load(aInputStream);
            ImageData imageData = imageDatas[0];
            if (aScaleImage)
            {
                Point bestSize = getBestImageSize(
                                     imageData.width, imageData.height);
                if (bestSize.x != imageData.width ||
                        bestSize.y != imageData.height)
                {
                    imageData = imageData.scaledTo(bestSize.x, bestSize.y);
                    log("Image " + aImageName + " scaled from " +
                        imageDatas[0].width + "x" + imageDatas[0].height +
                        " to " + bestSize.x + "x" + bestSize.y);
                }
            }
            result = new Image(aDisplay, imageData);
            long endTime = System.currentTimeMillis();
            log("Loaded image " + aImageName + " (load time " +
                (endTime - startTime) + " ms)");
            iImageTable.put(aImageName, result);
        }
        catch (Throwable t)
        {
            log("Can not load image " + aImageName + ": " + t);
            //logError("Exception while loading image " + aImageName, t);
        }
        return result;
    }

    /**
     * Determines the best image size for the image of given size.
     */
    private static Point getBestImageSize(int aWidth, int aHeight)
    {
        // Actually maximum image width and height should be obtained with
        // org.eclipse.swt.internal.extension.DisplayExtension
        // getBestImageWidth() and getBestImageHeight().
        final int MAX_WIDTH = 50; // max width in pixels
        final int MAX_HEIGHT = 50; // max height in pixels
        Point result = new Point(aWidth, aHeight);
        if (result.x > MAX_WIDTH || result.y > MAX_HEIGHT)
        {
            if (result.x >= MAX_WIDTH)
            {
                result.x = MAX_WIDTH;
                result.y = MAX_WIDTH * aHeight / aWidth;
            }
            if (result.y >= MAX_HEIGHT)
            {
                result.x = MAX_HEIGHT * aWidth / aHeight;
                result.y = MAX_HEIGHT;
            }
        }
        return result;
    }

    /** Returns true if UI has been created and can be used. */
    protected boolean isUiReady()
    {
        if (iProgressView == null)
        {
            log("ui not ready, iProgressView is null");
            return false;
        }
        if (iProgressView.isDisposed())
        {
            log("ui not ready, iProgressView is disposed");
            return false;
        }
        return true;
    }

    /**
     * Blocks until UI has became ready.
     */
    private void waitForUi()
    {
        synchronized (iInitWaitObject)
        {
            try
            {
                if (iUiThreadExists && iProgressView == null)
                {
                    iInitWaitObject.wait();
                }
            }
            catch (InterruptedException ie)
            {
                // Ignore silently.
            }
        }
    }

    /**
     * Called when screen orientation changes.
     */
    private void screenOrientationChanged()
    {
        iDefaultShellBounds = iDialog.internal_getDefaultBounds();

        if (iActiveView != null)
        {
            iActiveView.screenOrientationChanged();
        }
    }

    private static class CListener implements ControlListener
    {
        private InstallerUiEswt iInstallerUi = null;

        CListener(InstallerUiEswt aInstallerUi)
        {
            iInstallerUi = aInstallerUi;
        }

        public void controlMoved(ControlEvent aEvent)
        {
        }

        public void controlResized(ControlEvent aEvent)
        {
            iInstallerUi.screenOrientationChanged();
        }
    }

    Rectangle defaultShellBounds()
    {
        return iDefaultShellBounds;
    }

    Rectangle defaultShellClientBounds()
    {
        return iDefaultShellClientBounds;
    }

    /** Get bold font for the current view. */
    Font getBoldFont()
    {
        if (iBoldFont == null)
        {
            FontData[] fontDatas = iParent.getFont().getFontData();
            FontData[] boldFontDatas = new FontData[fontDatas.length];
            for (int i = 0; i < boldFontDatas.length; i++)
            {
                boldFontDatas[i] = new FontData
                (fontDatas[i].getName(), fontDatas[i].getHeight()+1, SWT.BOLD);
            }
            iBoldFont = new Font(iParent.getDisplay(), boldFontDatas);
        }
        return iBoldFont;
    }

    void setActiveView(ViewBase aView)
    {
        if (iActiveView != null && iActiveView != aView &&
                !iActiveView.isDisposed())
        {
            iActiveView.setVisible(false);
        }
        iActiveView = aView;
    }

    ViewBase getActiveView()
    {
        return iActiveView;
    }
}
