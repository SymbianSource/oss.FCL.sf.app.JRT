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

import com.nokia.mj.impl.installer.exetable.ExeBall;
import com.nokia.mj.impl.installer.exetable.ExeStep;
import com.nokia.mj.impl.installer.ui.ApplicationInfo;
import com.nokia.mj.impl.installer.ui.UninstallInfo;
import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.installer.utils.Platform;
import com.nokia.mj.impl.security.midp.common.SigningCertificate;
import com.nokia.mj.impl.security.midp.authentication.AuthenticationModule;
import com.nokia.mj.impl.utils.Tokenizer;

import java.io.InputStream;
import java.io.IOException;
import java.util.Vector;
import java.util.jar.JarFile;
import java.util.jar.JarEntry;

/**
 * ConfirmUninstallation shows uninstallation confirmation dialog to user.
 *
 * @author Nokia Corporation
 * @version $Rev:  $ $Date:  $
 */
public class ConfirmUninstallation extends ExeStep
{
    public void execute(ExeBall aBall)
    {
        UninstallBall ball = (UninstallBall)aBall;
        if (ball.isSilent() || Platform.isLinux())
        {
            // Silent installation, assume positive answer from user.
            // Linux does not have UI, assume positive answer from user.
            ball.iUserConfirmation = new Boolean(true);
            return;
        }

        // Collect all the info needed in uninstallation confirmation dialog,
        // display it to user and basing on user response initialise
        // ball.iUserConfirmation. Use localized suite name if it exists.
        String suiteName = ball.iSuite.getAttributeValue(
                               "Nokia-MIDlet-Localized-Name");
        if (suiteName == null)
        {
            suiteName = ball.iSuite.getName();
        }
        UninstallInfo uninstallInfo = new UninstallInfo
        (ball.iSuite.getUid(), suiteName,
         ball.iSuite.getVendor(), ball.iSuite.getVersion().toString());

        // Set MIDlet-Delete-Confirm value.
        String propName = "microedition.locale";
        String locale = System.getProperty(propName);
        Log.log(propName + ": " + locale);
        String attrName = "MIDlet-Delete-Confirm";
        String attrValue = null;
        if (locale != null)
        {
            int idxMinus = -1;
            do
            {
                attrValue = ball.iSuite.getAttributeValue(attrName + "-" + locale);
                Log.log(attrName + "-" + locale + ": " + attrValue);
                idxMinus = locale.lastIndexOf('-');
                // If attribute is not found using full locale, e.g. 'en-GB',
                // strip the most specific part of the locale out
                // (so that locale will be e.g. 'en') and try again.
                if (idxMinus != -1)
                {
                    locale = locale.substring(0, idxMinus);
                }
            }
            while ((attrValue == null) && (idxMinus != -1));
        }
        if (attrValue == null)
        {
            attrValue = ball.iSuite.getAttributeValue(attrName);
            Log.log(attrName + ": " + attrValue);
        }
        uninstallInfo.setDeleteConfirm(attrValue);

        // Set certificates.
        SigningCertificate[] certs = AuthenticationModule.getInstance()
                                     .getCertificatesDetails(ball.iStorageHandler.getSession(),
                                                             ball.iSuite.getUid());
        if (certs != null)
        {
            Log.log("Certificates for this application:");
            for (int i = 0; i < certs.length; i++)
            {
                Log.log("Certificate[" + i + "]:\n" + certs[i]);
            }
        }
        uninstallInfo.setCertificates(certs);

        // Set ApplicationInfos.
        uninstallInfo.setApplications(getApplicationInfos(ball));

        // Get icon InputStream for correct icon in jar file.
        String suiteIconPath = ball.iSuite.getAttributeValue("MIDlet-Icon");
        if (suiteIconPath == null)
        {
            initIconPaths(ball);
            suiteIconPath = ball.iSuite.getFirstIconPath();
        }
        Log.log("SuiteIconPath for the UI: " + suiteIconPath);
        JarFile jarFile = null;
        InputStream iconInputStream = null;
        if (suiteIconPath != null)
        {
            try
            {
                jarFile = new JarFile(ball.iSuite.getJarPath());
                iconInputStream = jarFile.getInputStream
                                  (new JarEntry(FileUtils.trimJarEntry(suiteIconPath)));
                uninstallInfo.setIconInputStream(iconInputStream);
                uninstallInfo.setIconPath(suiteIconPath);
            }
            catch (IOException ioe)
            {
                Log.logWarning("Getting InputStream for icon failed", ioe);
            }
        }

        // Ask user confirmation.
        boolean userConfirmation = true;
        try
        {
            if (ball.getInstallerUi() != null)
            {
                userConfirmation = ball.getInstallerUi().confirm(uninstallInfo);
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

        ball.iUserConfirmation = new Boolean(userConfirmation);
    }

    public void cancel(ExeBall aBall)
    {
        // nop
    }

    /**
     * Initializes application icon paths to SuiteInfo
     * using icon information in Jad/Manifest attributes.
     */
    private void initIconPaths(UninstallBall aBall)
    {
        Vector appInfos = aBall.iSuite.getApplications();
        for (int i = 0; i < appInfos.size(); i++)
        {
            com.nokia.mj.impl.installer.storagehandler.ApplicationInfo
            appInfo =
                (com.nokia.mj.impl.installer.storagehandler.ApplicationInfo)
                appInfos.elementAt(i);
            String attrName = "MIDlet-" + (i+1);
            String attrValue = aBall.iSuite.getAttributeValue(attrName);
            if (attrValue != null)
            {
                // Split MIDlet-n attribute into tokens.
                String[] tokens = Tokenizer.split(attrValue, ",");
                if (tokens == null || tokens.length != 3)
                {
                    Log.logWarning("Invalid " + attrName +
                                   " attribute value: " + attrValue);
                    break;
                }
                appInfo.setIconPath(tokens[1].trim());
            }
        }
    }

    /**
     * Get ApplicationInfos to be passed to InstallerUi.
     */
    private ApplicationInfo[] getApplicationInfos(UninstallBall aBall)
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
                String midletName = aBall.iSuite.getAttributeValue(attrName);
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
}
