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

import java.util.Hashtable;
import java.util.Vector;

import com.nokia.mj.impl.installer.InstallationNotifier;
import com.nokia.mj.impl.installer.applicationregistrator.ApplicationRegistrator;
import com.nokia.mj.impl.installer.applicationregistrator.SifNotifier;
import com.nokia.mj.impl.installer.applicationregistrator.SifRegistrator;
import com.nokia.mj.impl.installer.captainservice.CaptainService;
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
import com.nokia.mj.impl.installer.utils.Platform;
import com.nokia.mj.impl.security.midp.authentication.AuthenticationModule;
import com.nokia.mj.impl.security.midp.authorization.PermissionGranter;
import com.nokia.mj.impl.security.packageprotection.PackageProtector;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;
import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.OtaStatusCode;

public final class UninstallBall extends ExeBall
{

    /** All command line arguments. */
    public Args iArgs = null;
    /** InstallationNotifier */
    public InstallationNotifier iInstallationNotifier = null;

    /** Uid from command line. */
    Uid iUid = null;

    /** Suite info */
    SuiteInfo iSuite = null;

    /** Internet access point to be used in uninstallation */
    String iIap = null;
    /** Service network access point to be used in uninstallation */
    String iSnap = null;

    /**
     * Flag telling if and what user has answered to uninstallation
     * confirmation dialog. If null, dialog has not been displayed.
     */
    Boolean iUserConfirmation = null;
    /**
     * If this flag is set Nokia-MIDlet-Block-Uninstall
     * attribute will be ignored.
     */
    boolean iForceUninstall = false;
    /** Tells if installer should send messages to JavaCaptain. */
    boolean iCaptainMsgs = true;
    /** Comms endpoints to which InstallerResultMessage is sent. */
    int[] iCommsResultEndpoints = null;

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
    /** Instance of CaptainService */
    private CaptainService iCaptainService = null;
    /** NotificationPoster instance for posting OTA status notifications. */
    private NotificationPoster iNotificationPoster = null;
    /** InstallerUi instance. */
    private InstallerUi iInstallerUi = null;

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
                getInstallerUi().cancelConfirmations();
            }
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
            throw new InstallerException
            (InstallerErrorMessage.UNINST_CANCEL, null,
             InstallerDetailedErrorMessage.NO_MSG, null,
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
        if (SifRegistrator.getSifMode() > 0)
        {
            // No UI in uinstallation when SifMode > 0.
            return null;
        }
        if (iInstallerUi == null)
        {
            iInstallerUi = InstallerUiFactory.getInstallerUi
                           (InstallerUi.MODE_UNINSTALL, this);
        }
        return iInstallerUi;
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
}
