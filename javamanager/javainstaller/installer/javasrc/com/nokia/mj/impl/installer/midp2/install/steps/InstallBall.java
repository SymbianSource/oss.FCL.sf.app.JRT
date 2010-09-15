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

import java.util.Hashtable;

import com.nokia.mj.impl.installer.Installer;
import com.nokia.mj.impl.installer.InstallationNotifier;
import com.nokia.mj.impl.installer.applicationregistrator.ApplicationRegistrator;
import com.nokia.mj.impl.installer.applicationregistrator.SifNotifier;
import com.nokia.mj.impl.installer.applicationregistrator.SifRegistrator;
import com.nokia.mj.impl.installer.captainservice.CaptainService;
import com.nokia.mj.impl.installer.customisationproperties.CustomisationProperties;
import com.nokia.mj.impl.installer.downloader.Downloader;
import com.nokia.mj.impl.installer.downloader.DownloadListener;
import com.nokia.mj.impl.installer.downloader.GcfDownloader;
import com.nokia.mj.impl.installer.downloader.GcfNotificationPoster;
import com.nokia.mj.impl.installer.downloader.NotificationPoster;
import com.nokia.mj.impl.installer.exetable.ExeBall;
import com.nokia.mj.impl.installer.integrityservice.IntegrityService;
import com.nokia.mj.impl.installer.jsrpluginnotifier.JsrPluginNotifier;
import com.nokia.mj.impl.installer.storagehandler.StorageHandler;
import com.nokia.mj.impl.installer.storagehandler.SuiteInfo;
import com.nokia.mj.impl.installer.ui.InstallerUi;
import com.nokia.mj.impl.installer.ui.InstallerUiFactory;
import com.nokia.mj.impl.installer.utils.Args;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.installer.utils.MidpAttributeValidator;
import com.nokia.mj.impl.security.midp.authentication.AuthenticationModule;
import com.nokia.mj.impl.security.midp.authorization.PermissionGranter;
import com.nokia.mj.impl.security.midp.common.AuthenticationCredentials;
import com.nokia.mj.impl.security.midp.common.SecurityAttributes;
import com.nokia.mj.impl.security.midp.common.SigningCertificate;
import com.nokia.mj.impl.security.packageprotection.PackageProtector;
import com.nokia.mj.impl.utils.Attribute;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;
import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.OtaStatusCode;

public final class InstallBall extends ExeBall
{
    /** Flag for enabling launch app query in the end of installation. */
    static final boolean LAUNCH_APP_QUERY = true;

    /** All command line arguments. */
    public Args iArgs = null;
    /** InstallationNotifier */
    public InstallationNotifier iInstallationNotifier = null;

    // Command line arguments.
    String iJarFilename = null;
    String iJadFilename = null;
    String iJarUrl = null;
    String iJadUrl = null;
    String iSourceUrl = null;

    // Jad/Manifest attributes
    Hashtable iJarAttributes = null;
    Hashtable iJadAttributes = null;
    Hashtable iCombinedAttributes = null;
    MidpAttributeValidator iAttributeValidator = null;

    /**
     * Flag telling if there is a dialog being displayed
     * to the user. This flag is used in those execution
     * steps which must wait for the user answer.
     */
    boolean iDialogOpen = false;

    /**
     * Flag telling if installation is done as preinstallation.
     * In preinstallation case the jad and jar files are not
     * copied to apps directory.
     */
    boolean iPreinstallation = false;

    /**
     * Flag telling if the installation is done by converting
     * an existing java application.
     */
    boolean iConversionInstallation = false;

    /**
     * Path to the private dir of legacy application
     * in case of conversion installation.
     */
    String iConversionRoot = null;

    /**
     * Flag telling if the installation file should be moved or copied.
     * By default the file is copied. If installation is made from midlet
     * message package or if JavaInstaller downloads jar file, then
     * installation file is moved.
     */
    boolean iMoveInstallationFile = false;

    /** Installation drive */
    int iInstallationDrive = 2; // default is C-drive
    /** Installation group */
    String iInstallationGroup = null;
    /** Internet access point to be used in installation */
    String iIap = null;
    /** Service network access point to be used in installation */
    String iSnap = null;
    /** Character set used in Jad file */
    String iJadCharset = null;
    /** Tells if installer should send messages to JavaCaptain. */
    boolean iCaptainMsgs = true;
    /** Is upgrade allowed */
    boolean iAllowUpgrade = true;
    /** Retain or remove private data in upgrade.
        If null, private data is handled according
        to rules defined in MIDP spec. */
    Boolean iUpgradeData = null;
    /** Allow upgrade with the same version */
    boolean iAllowOverwrite = true;
    /** Allow untrusted applications to be installed */
    boolean iAllowUntrusted = true;
    /** Tells if OCSP should be performed */
    Boolean iOcsp = null;
    /** Tells if OCSP warnings should be ignored */
    Boolean iIgnoreOcspWarnings = null;
    /** Exception that has occurred during OCSP, or null. */
    Throwable iOcspException = null;
    /** Is download allowed */
    boolean iAllowDownload = true;
    /** Username used in download */
    String iDownloadUsername = null;
    /** Password used in download */
    String iDownloadPassword = null;
    /** Comms endpoints to which InstallerResultMessage is sent. */
    int[] iCommsResultEndpoints = null;

    /** Old suite info, used in upgrade case */
    SuiteInfo iOldSuite = null;
    /** Suite info */
    SuiteInfo iSuite = null;

    /** Downloader instance for jad and jar download. */
    Downloader iDownloader = null;
    /** DlListener instance for jad and jar download. */
    DlListener iDlListener = null;
    /**
     * Flag telling if there has been successful download during
     * this installation.
     */
    boolean iDownloadSucceeded = false;
    /** Downloaded jar file type basing on its MIME type.
        This can be null if a file with unknown MIME type is downloaded. */
    String iJarFileType = null;

    /**
     * Certificates for previous version of this application,
     * used in case of an upgrade.
     */
    SigningCertificate[] iOldCertificates = null;
    /** SecurityAttributes for this application suite. */
    SecurityAttributes iSecurityAttributes = null;
    /** AuthenticationCredentials from Jad authentication. */
    AuthenticationCredentials[] iAuthenticationCredentials = null;
    /** Flag telling if Jar has been authenticated. */
    boolean iJarAuthenticated = false;
    /** Flag telling if Jar permissions have been granted. */
    boolean iJarPermissionsGranted = false;
    /** Flag telling if blanket permissions have been granted. */
    boolean iBlanketPermissionsGranted = false;

    /**
     * Flag telling if and what user has answered to installation
     * confirmation dialog. If null, dialog has not been displayed.
     */
    Boolean iUserConfirmation = null;

    /** IntegrityService session */
    IntegrityService iIntegrityService = null;
    /** StorageHandler session */
    StorageHandler iStorageHandler = null;
    /** ApplicationRegistrator session */
    ApplicationRegistrator iApplicationRegistrator = null;
    /** SifRegistrator session */
    SifRegistrator iSifRegistrator = null;
    /** SifNotifier instance */
    SifNotifier iSifNotifier = null;
    /** JsrPluginNotifier */
    JsrPluginNotifier iJsrPluginNotifier = null;
    /** CustomisationPropeerties */
    CustomisationProperties iCustomisationProperties = null;
    /** Instance of CaptainService */
    private CaptainService iCaptainService = null;
    /** NotificationPoster instance for posting OTA status notifications. */
    private NotificationPoster iNotificationPoster = null;

    /**
     * InstallerUi instance. Call getInstallerUi() at least once
     * to get this variable initialized.
     */
    private InstallerUi iInstallerUi = null;

    /**
     * Called just before InstallerUi is about to be opened.
     * This call can be used to hide the "preparing installation"
     * indicator.
     */
    public void uiIsReady()
    {
        if (iApplicationRegistrator != null)
        {
            Log.log("InstallBall.uiIsReady");
            // Ask javainstalllauncher to stop displaying
            // "Preparing installation" indicator. This is done also
            // in ConvertIcons to make sure that the notification
            // really is passed to launcher process.
            iApplicationRegistrator.notifyLauncherThatUiIsReady();
        }
    }

    /**
     * Called when InstallerUi is hidden or unhidden.
     *
     * @param aHidden true if UI was hidden, false if UI was unhidden.
     */
    public void uiIsHidden(boolean aHidden)
    {
        log("InstallBall.uiIsHidden " + aHidden);
        if (iSifNotifier == null)
        {
            Log.logWarning("InstallBall.uiIsHidden(" + aHidden +
                           ") called when SifNotifier does not exist");
            return;
        }
        iSifNotifier.setInstallerUi(getInstallerUi());
        if (aHidden)
        {
            iSifNotifier.activateIndicator();
        }
        else
        {
            iSifNotifier.deactivateIndicator();
        }
    }

    /**
     * Called when user cancels the execution from the InstallerUi.
     * This method must return quickly.
     */
    public void cancel()
    {
        super.cancel();
        if (isCancelled())
        {
            if (getInstallerUi() != null)
            {
                if (getInstallerUi().getOcspIndicator())
                {
                    try
                    {
                        // Cancel OCSP.
                        Log.log("User cancelled, cancelling OCSP...");
                        AuthenticationModule.getInstance()
                        .cancelOcspCheck(iSuite.getUid());
                        Log.log("OCSP cancelled");
                    }
                    catch (Throwable t)
                    {
                        Log.logWarning("Cancelling OCSP failed", t);
                    }
                }
            }
            if (iDownloader != null)
            {
                try
                {
                    Log.log("User cancelled, stopping downloader...");
                    iDownloader.stop();
                    iDownloader = null;
                    Log.log("Downloader stopped");
                }
                catch (Throwable t)
                {
                    Log.logWarning("Stopping downloader failed", t);
                }
            }
        }
        if (getInstallerUi() != null)
        {
            Log.log("User cancelled, closing confirmation dialogs...");
            getInstallerUi().cancelConfirmations();
        }
    }

    /**
     * This method must throw appropriate exception when
     * user has cancelled the execution.
     */
    public void checkForCancel() throws Exception
    {
        super.checkForCancel();
        if (isCancelled())
        {
            if (iDownloader != null)
            {
                try
                {
                    Log.log("checkForCancel: User cancelled, stopping downloader...");
                    iDownloader.stop();
                    iDownloader = null;
                    Log.log("checkForCancel: Downloader stopped");
                }
                catch (Throwable t)
                {
                    Log.logWarning("checkForCancel: Stopping downloader failed", t);
                }
            }
            throw new InstallerException(
                Installer.ERR_CANCEL,
                InstallerErrorMessage.INST_CANCEL, null,
                InstallerDetailedErrorMessage.INST_CANCEL, null,
                OtaStatusCode.USER_CANCELLED);
        }
    }

    /**
     * Returns CaptainService instance.
     */
    CaptainService getCaptainService()
    {
        if (iCaptainService == null)
        {
            iCaptainService = new CaptainService(isSilent());
        }
        return iCaptainService;
    }

    /**
     * Returns a new Downloader. This method always creates
     * a new Downloader instance.
     */
    Downloader getDownloader
    (DownloadListener aDownloadListener)
    {
        // By default use GcfDownloader.
        return GcfDownloader.getDownloader(aDownloadListener, iIap, iSnap);
    }

    /**
     * Returns NotificationPoster instance.
     */
    NotificationPoster getNotificationPoster()
    {
        if (iNotificationPoster == null)
        {
            // By default use GcfNotificationPoster.
            iNotificationPoster = GcfNotificationPoster.
                                  getNotificationPoster(iIap, iSnap);
        }
        return iNotificationPoster;
    }

    /**
     * Returns InstallerUi instance. In silent mode this method returns null.
     */
    InstallerUi getInstallerUi()
    {
        if (isSilent())
        {
            // No UI in silent mode.
            return null;
        }
        if (iInstallerUi == null)
        {
            iInstallerUi = InstallerUiFactory.getInstallerUi
                           (InstallerUi.MODE_INSTALL, this);
        }
        return iInstallerUi;
    }

    /**
     * Starts posting pending OTA status notifications
     * if posting has not yet been started.
     * A separate thread for posting pending
     * OTA status notifications is created. Thread is stopped
     * in FinalizeInstallation step if thread is still
     * running at that point.
     */
    void startToPostPendingNotifications()
    {
        if (!getNotificationPoster().isStarted())
        {
            // Enable notification posting by uncommenting next line
            getNotificationPoster().notifyPendingStatuses();
        }
    }

    /**
     * Calls cleanup for AuthenticationModule, PermissionGranter and
     * PackageProtector to ensure that no security related data from
     * installation is left to memory upon installation completion.
     */
    void cleanupSecurity()
    {
        AuthenticationModule.getInstance().cleanup();
        PermissionGranter.getInstance().cleanup();
        PackageProtector.getInstance().cleanup();
    }

    // Begin helper methods for accessing jad/manifest attributes.

    boolean attributeExists(String aName)
    {
        if (iCombinedAttributes == null)
        {
            return false;
        }
        return (iCombinedAttributes.get(aName) != null);
    }

    boolean attributeExistsInJad(String aName)
    {
        if (iJadAttributes == null)
        {
            return false;
        }
        return (iJadAttributes.get(aName) != null);
    }

    boolean attributeExistsInJar(String aName)
    {
        if (iJarAttributes == null)
        {
            return false;
        }
        return (iJarAttributes.get(aName) != null);
    }

    String getAttributeValue(String aName)
    {
        if (iCombinedAttributes == null)
        {
            return null;
        }
        Attribute attr = (Attribute)iCombinedAttributes.get(aName);
        if (attr != null)
        {
            return attr.getValue();
        }
        return null;
    }

    int getAttributeIntValue(String aName)
    {
        if (iCombinedAttributes == null)
        {
            return 0;
        }
        Attribute attr = (Attribute)iCombinedAttributes.get(aName);
        if (attr != null)
        {
            return attr.getIntValue();
        }
        return 0;
    }

    // End helper methods for accessing jad/manifest attributes.
}
