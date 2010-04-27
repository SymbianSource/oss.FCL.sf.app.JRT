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

import com.nokia.mj.impl.installer.customisationproperties.CustomisationProperties;
import com.nokia.mj.impl.installer.exetable.ExeBall;
import com.nokia.mj.impl.installer.exetable.ExeStep;
import com.nokia.mj.impl.installer.ui.ApplicationInfo;
import com.nokia.mj.impl.installer.ui.InstallInfo;
import com.nokia.mj.impl.installer.utils.DriveInfo;
import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.installer.utils.Platform;
import com.nokia.mj.impl.installer.utils.SysUtil;
import com.nokia.mj.impl.security.midp.common.SigningCertificate;
import com.nokia.mj.impl.security.midp.authentication.AuthenticationModule;

import java.io.InputStream;
import java.io.IOException;
import java.util.Vector;
import java.util.jar.JarFile;
import java.util.jar.JarEntry;

/**
 * ConfirmInstallation shows installation confirmation dialog to user.
 *
 * @author Nokia Corporation
 * @version $Rev:  $ $Date:  $
 */
public class ConfirmInstallation extends ExeStep
{
    public void execute(ExeBall aBall)
    {
        InstallBall ball = (InstallBall)aBall;

        // Get possible certificates for the application.
        SigningCertificate[] certs = AuthenticationModule.getInstance()
                                     .getCertificatesDetails(ball.iStorageHandler.getSession(),
                                                             ball.iSuite.getUid());
        if (certs != null)
        {
            Log.log("Possible certificates for this application:");
            for (int i = 0; i < certs.length; i++)
            {
                Log.log("Certificate[" + i + "]:\n" + certs[i]);
            }
        }
        // Check if RMS data should be retained in case of an
        // upgrade as specified in MIDP 2.1 spec chapter
        // 2.1.5 "MIDlet Suite Update".
        if (ball.iOldSuite != null)
        {
            // Check from signer if private data should
            // be retained in application upgrade.
            checkSameSignerForUpgradeData(ball, ball.iOldCertificates, certs);
            // Check from jad and jar urls if private data should
            // be retained in application upgrade.
            checkUrlsForUpgradeData(ball);
        }

        if (ball.isSilent() || Platform.isLinux())
        {
            // Silent installation, assume positive answer from user.
            // Linux does not have UI, assume positive answer from user.
            ball.iUserConfirmation = new Boolean(true);
            return;
        }

        // Collect all the info needed in confirmation dialog,
        // display it to user and based on user response
        // initialise ball.iUserConfirmation and other data.
        // Use localized suite name if it exists
        String suiteName = ball.getAttributeValue("Nokia-MIDlet-Localized-Name");
        if (suiteName == null)
        {
            suiteName = ball.iSuite.getName();
        }
        InstallInfo installInfo = new InstallInfo(ball.iSuite.getUid(),
                suiteName, ball.iSuite.getVendor(),
                ball.iSuite.getVersion().toString());

        // In case of an update, set old suite version.
        if (ball.iOldSuite != null)
        {
            installInfo.setOldVersion(ball.iOldSuite.getVersion().toString());
        }

        // Set Jar size and data size from Jad/Manifest attributes.
        installInfo.setJarSize(getAttributeLongValue(ball, "MIDlet-Jar-Size"));
        installInfo.setDataSize(getAttributeLongValue(ball, "MIDlet-Data-Size"));
        if (installInfo.getJarSize() <= 0 &&
                ball.iJarFilename != null && ball.iDownloader == null)
        {
            // Get jar file size.
            installInfo.setJarSize(FileUtils.getSize(ball.iJarFilename));
        }

        // Set certificates.
        installInfo.setCertificates(certs);

        // Set ApplicationInfos.
        installInfo.setApplications(getApplicationInfos(ball));

        // Set RetainData flag if this is an application update and
        // retain user data decision has not been made yet.
        if (ball.iOldSuite != null && ball.iUpgradeData == null)
        {
            installInfo.setRetainData(true);
        }

        // Set installation drive.
        installInfo.setDriveId(ball.iInstallationDrive);
        Vector drives = new Vector();
        SysUtil.getUserVisibleDrives(drives);
        int[] driveIds = new int[drives.size()];
        int[] driveTypes = new int[drives.size()];
        long[] driveFreeSpaces = new long[drives.size()];
        for (int i = 0; i < drives.size(); i++)
        {
            DriveInfo driveInfo = (DriveInfo)drives.elementAt(i);
            // Note that this trusts that DriveInfo constant values
            // are identical to drive type constants in InstallInfo.
            driveIds[i] = driveInfo.getNumber();
            driveTypes[i] = driveInfo.getDriveType();
            driveFreeSpaces[i] = driveInfo.getFreeSpace();
        }
        installInfo.setDriveIdsTypes(driveIds, driveTypes);
        installInfo.setDriveFreeSpaces(driveFreeSpaces);

        // Set icon paths.
        JarFile jarFile = null;
        InputStream iconInputStream = null;
        if (ball.iJarFilename != null && ball.iDownloader == null)
        {
            // We have jar filename and are not downloading it,
            // so jar file must be available.
            // Get icon InputStream for correct icon in jar file.
            String suiteIconPath = ball.iSuite.getAttributeValue("MIDlet-Icon");
            if (suiteIconPath == null)
            {
                suiteIconPath = ball.iSuite.getFirstIconPath();
            }
            Log.log("SuiteIconPath for the UI resource: " + suiteIconPath);
            if (suiteIconPath != null)
            {
                try
                {
                    jarFile = new JarFile(ball.iJarFilename);
                    iconInputStream = jarFile.getInputStream
                                      (new JarEntry(FileUtils.trimJarEntry(suiteIconPath)));
                    installInfo.setIconInputStream(iconInputStream);
                    installInfo.setIconPath(suiteIconPath);
                }
                catch (IOException ioe)
                {
                    Log.logWarning("Getting InputStream for icon failed", ioe);
                }
            }
        }

        // Ask user confirmation.
        boolean userConfirmation = true;
        try
        {
            if (ball.getInstallerUi() != null)
            {
                userConfirmation = ball.getInstallerUi().confirm(installInfo);
            }
        }
        catch (Throwable t)
        {
            InstallerException.internalError(
                "Exception from InstallerUi.confirm: " + t, t);
            userConfirmation = false;
        }

        if (!userConfirmation)
        {
            ball.cancel();
        }
        ball.iUserConfirmation = new Boolean(userConfirmation);

        // Close the jarFile and iconInputStream.
        if (jarFile != null)
        {
            try
            {
                jarFile.close(); // Closes also input stream
                jarFile = null;
            }
            catch (IOException ioe)
            {
                Log.logWarning("Closing icon InputStream failed", ioe);
            }
        }

        // Now that user has answered to confirmation dialog, take into
        // use user selections: driveId and retainData.
        int oldInstallationDrive = ball.iInstallationDrive;
        ball.iInstallationDrive = installInfo.getDriveId();
        if (ball.iUpgradeData == null)
        {
            // Get RetainData flag.
            ball.iUpgradeData = installInfo.getRetainData();
        }
        Log.log("UserConfirmation: " + ball.iUserConfirmation);
        Log.log("InstallationDrive: " + ball.iInstallationDrive);
        Log.log("UpgradeData: " + ball.iUpgradeData);
        if (oldInstallationDrive != ball.iInstallationDrive)
        {
            try
            {
                // Save user selection.
                SysUtil.setRepositoryStringValue(
                    SysUtil.REPO_ID_JAVA_INST_VARIATION,
                    SysUtil.REPO_KEY_JAVA_INST_DEF_INST_DRIVE,
                    FileUtils.getDriveName(ball.iInstallationDrive));
                Log.log("Updated user chosen drive to repository: " +
                        ball.iInstallationDrive);
            }
            catch (Throwable t)
            {
                Log.log("Updating user chosen drive to repository failed", t);
            }
        }

        // Log all suite info.
        //ball.log(ball.iSuite.toString());
    }

    public void cancel(ExeBall aBall)
    {
        // nop
    }

    /**
     * Get ApplicationInfos to be passed to InstallerUi.
     */
    protected static ApplicationInfo[] getApplicationInfos(InstallBall aBall)
    {
        Vector appInfos = aBall.iSuite.getApplications();
        ApplicationInfo[] result = null;
        if (appInfos != null)
        {
            result = new ApplicationInfo[appInfos.size()];
            for (int i = 0; i < result.length; i++)
            {
                com.nokia.mj.impl.installer.storagehandler.ApplicationInfo
                appInfo =
                    (com.nokia.mj.impl.installer.storagehandler.ApplicationInfo)
                    appInfos.elementAt(i);
                // Use localized application names if defined
                String attrName = "Nokia-MIDlet-Localized-" + (i + 1);
                String midletName = aBall.getAttributeValue(attrName);
                if (midletName == null)
                {
                    midletName = appInfo.getName();
                }
                result[i] = new ApplicationInfo(appInfo.getUid(),
                                                midletName,
                                                appInfo.getIconPath());
            }
        }
        return result;
    }

    /**
     * Checks from certificate signers if the application private data
     * needs to be retained during application upgrade.
     */
    private void checkSameSignerForUpgradeData(InstallBall aBall,
            SigningCertificate[] aOldCerts,
            SigningCertificate[] aNewCerts)
    {
        if (aBall.iUpgradeData != null)
        {
            // iUpgradeData flag has already been set.
            return;
        }
        if (aOldCerts == null && aNewCerts != null)
        {
            // Old and new suites have different signers, do not set
            // iUpgradeData flag.
            return;
        }
        if (aOldCerts != null && aNewCerts != null)
        {
            for (int i = 0;
                    i < aOldCerts.length && aBall.iUpgradeData == null;
                    i++)
            {
                for (int j = 0;
                        j < aNewCerts.length && aBall.iUpgradeData == null;
                        j++)
                {
                    if (aOldCerts[i].isSameSigner(aNewCerts[j]))
                    {
                        // Old and new suites have the same signer
                        // so data must be retained.
                        Log.log("Retain user data, same signer for old and new suite: " +
                                aNewCerts[j].getOrganization());
                        aBall.iUpgradeData = new Boolean(true);
                    }
                }
            }
        }
    }

    /**
     * Checks from jad and jar urls if the application private data
     * needs to be retained during application upgrade.
     */
    private void checkUrlsForUpgradeData(InstallBall aBall)
    {
        if (aBall.iUpgradeData != null)
        {
            // iUpgradeData flag has already been set.
            return;
        }
        if (aBall.iOldSuite != null)
        {
            checkUrlsForUpgradeData(aBall,
                                    aBall.iOldSuite.getJadUrl(),
                                    aBall.iJadUrl);
            checkUrlsForUpgradeData(aBall,
                                    aBall.iOldSuite.getJarUrl(),
                                    aBall.iJarUrl);
        }
    }

    /**
     * Checks from given urls if the application private data
     * needs to be retained during application upgrade.
     */
    private void checkUrlsForUpgradeData
    (InstallBall aBall, String aOldUrl, String aNewUrl)
    {
        if (aBall.iUpgradeData == null)
        {
            // Upgrade data selection has not been made yet,
            // check from given urls.
            if (aOldUrl != null && aNewUrl != null && aOldUrl.equals(aNewUrl))
            {
                // Old and new urls are identical so data must be retained.
                Log.log("Retain user data, same url for old and new suite: " +
                        aNewUrl);
                aBall.iUpgradeData = new Boolean(true);
            }
        }
    }

    /**
     * Get long type Jad/Manifest attribute value, or -1 if attribute
     * is not found.
     */
    private long getAttributeLongValue(InstallBall aBall, String aAttrName)
    {
        String attrValue = aBall.getAttributeValue(aAttrName);
        long size = -1;
        if (attrValue != null)
        {
            try
            {
                size = Long.parseLong(attrValue);
            }
            catch (NumberFormatException nfe)
            {
                Log.logWarning("Parsing " + aAttrName + " value " +
                               attrValue + " failed", nfe);
            }
        }
        return size;
    }
}
