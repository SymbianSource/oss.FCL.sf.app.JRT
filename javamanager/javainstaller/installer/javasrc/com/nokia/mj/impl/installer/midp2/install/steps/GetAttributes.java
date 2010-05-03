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

import java.io.IOException;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;

import com.nokia.mj.impl.installer.exetable.ExeBall;
import com.nokia.mj.impl.installer.exetable.ExeStep;
import com.nokia.mj.impl.installer.storagehandler.SuiteInfo;
import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.installer.utils.JadReader;
import com.nokia.mj.impl.installer.utils.Platform;
import com.nokia.mj.impl.installer.utils.SysUtil;
import com.nokia.mj.impl.security.midp.common.SecurityAttributes;
import com.nokia.mj.impl.utils.Attribute;
import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;
import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.JarManifestReader;
import com.nokia.mj.impl.utils.OtaStatusCode;
import com.nokia.mj.impl.utils.Version;

public class GetAttributes extends ExeStep
{
    public void execute(ExeBall aBall)
    {
        InstallBall ball = (InstallBall)aBall;
        Log.log("Getting attributes...");
        parseJad(ball);
        boolean isTrusted = checkTrusted(ball);
        parseManifest(ball);
        if (!ball.iAttributeValidator.isCombined())
        {
            ball.iCombinedAttributes = ball.iAttributeValidator.combine
                                       (ball.iJadAttributes, ball.iJarAttributes, isTrusted);
        }

        Version firstSupportedVersion = new Version(5,2,0);
        if (Platform.getPlatformVersion().compareTo(firstSupportedVersion) >= 0)
        {
            // Nokia-MIDlet-Name-<locale> and Nokia-MIDlet-<n>-<locale> are
            // supported from sw platform version 5.2.
            setLocalizedNames(ball);
        }

        initSuite(ball, isTrusted);
    }

    public void cancel(ExeBall aBall)
    {
        // nop
    }

    private static void parseManifest(InstallBall aBall)
    {
        if (aBall.iJarFilename == null)
        {
            return;
        }

        if (aBall.iJarAttributes == null)
        {
            try
            {
                aBall.log("Reading Manifest...");
                aBall.iJarAttributes = JarManifestReader.getAttributes(aBall.iJarFilename);
                if (aBall.iJarAttributes == null)
                {
                    throw new IOException("No Manifest attributes found");
                }
            }
            catch (IOException ioe)
            {
                Log.logError("Exception while getting jar attributes: " + ioe);
                throw new InstallerException
                (InstallerErrorMessage.INST_CORRUPT_PKG, null,
                 InstallerDetailedErrorMessage.INTERNAL_ERROR,
                 new String[] { "Reading Manifest failed: " + aBall.iJarFilename },
                 OtaStatusCode.INVALID_JAR, ioe);
            }
        }
        if (!aBall.iAttributeValidator.isValidated(
                    aBall.iAttributeValidator.JAR))
        {
            aBall.iAttributeValidator.validate(
                aBall.iJarAttributes, aBall.iAttributeValidator.JAR);
        }
    }

    private static void parseJad(InstallBall aBall)
    {
        if (aBall.iJadFilename == null)
        {
            return;
        }

        if (aBall.iJadAttributes == null)
        {
            try
            {
                aBall.log("Reading Jad...");
                aBall.iJadAttributes =
                    JadReader.getAttributes(aBall.iJadFilename,
                                            aBall.iJadCharset);
                if (aBall.iJadAttributes == null)
                {
                    throw new IOException("No Jad attributes found");
                }
            }
            catch (IOException ioe)
            {
                Log.logError("Exception while getting jad attributes: " + ioe);
                throw new InstallerException
                (InstallerErrorMessage.INST_CORRUPT_PKG, null,
                 InstallerDetailedErrorMessage.INTERNAL_ERROR,
                 new String[] { "Reading Jad failed: " + aBall.iJadFilename },
                 OtaStatusCode.INVALID_DESCRIPTOR, ioe);
            }
        }
        if (!aBall.iAttributeValidator.isValidated(
                    aBall.iAttributeValidator.JAD))
        {
            aBall.iAttributeValidator.validate(
                aBall.iJadAttributes, aBall.iAttributeValidator.JAD);
        }
    }

    private static boolean checkTrusted(InstallBall aBall)
    {
        if (aBall.iSecurityAttributes != null)
        {
            return aBall.iSecurityAttributes.isTrusted();
        }

        boolean result = false;
        // Create SecurityAttributes and check if suite is trusted.
        if (aBall.iJadAttributes != null)
        {
            aBall.iSecurityAttributes = new SecurityAttributes();
            aBall.iSecurityAttributes
            .addDescriptorAttributes(aBall.iJadAttributes);
        }
        if (aBall.iSecurityAttributes != null &&
                aBall.iSecurityAttributes.isTrusted())
        {
            // Application suite is trusted.
            result = true;
        }
        else if (!aBall.iAllowUntrusted)
        {
            // Application suite is untrusted and untrusted
            // applications are not allowed, let's abort installation.
            InstallerException.internalError
            ("Installing untrusted application is not allowed");
        }
        return result;
    }

    private static void initSuite(InstallBall aBall, boolean aTrusted)
    {
        if (aBall.iSuite == null)
        {
            Log.log("Constructing new SuiteInfo, trusted=" + aTrusted);
            aBall.iSuite = new SuiteInfo
            (aBall.getAttributeValue("MIDlet-Name"),
             aBall.getAttributeValue("MIDlet-Vendor"));
            aBall.iSuite.setVersion
            (Version.getVersion
             (aBall.getAttributeValue("MIDlet-Version")));
            aBall.iSuite.setTrusted(aTrusted);
            aBall.iSuite.setPreinstallation(aBall.iPreinstallation);

            // Uids, paths and applications are initialized in
            // SelectUids step after old suite info is read from storage.

            // Set media id of the installation drive.
            aBall.iSuite.setMediaId(SysUtil.getDriveUniqId
                                    (aBall.iInstallationDrive));
        }

        // This method gets called before and after Jar download,
        // update info which can be changed if download was made.
        aBall.iSuite.setAttributes(aBall.iCombinedAttributes);
        aBall.iSuite.setJadUrl(aBall.iJadUrl);
        aBall.iSuite.setJarUrl(aBall.iJarUrl);

        // Check that MIDlet-Jar-Size attribute value matches to
        // actual jar file size.
        if (aBall.iJarFilename != null)
        {
            checkJarFileExtension(aBall);
            String attrName = "MIDlet-Jar-Size";
            if (aBall.attributeExists(attrName))
            {
                int attrValue = aBall.getAttributeIntValue(attrName);
                long jarFileSize = FileUtils.getSize(aBall.iJarFilename);
                if (attrValue != jarFileSize)
                {
                    // Jar file size is not the same as the size specified
                    // in jad file. The file might be DRM encrypted, so
                    // check if the content size matches to size in jad file.
                    // Note: This check must not be enabled before files
                    // are copied safely, that is copy operation must
                    // guarantee that file is DRM enrcypted after copy.
                    // Installation of DRM protected apps can be prevented
                    // by outcommenting the next line.
                    jarFileSize = FileUtils.getContentSize(aBall.iJarFilename);
                    if (attrValue == jarFileSize)
                    {
                        // Content size matches to the size specified in
                        // jad file, assume that application is DRM protected
                        // and set file type to indicate DRM protected file
                        // because runtime uses filename extension for
                        // checking if file is DRM protected.
                        if (aBall.iJarFileType == null &&
                                aBall.iJarFilename.toLowerCase().endsWith(".jar"))
                        {
                            aBall.iJarFileType = ".dcf";
                        }
                    }
                }
                if (attrValue != jarFileSize)
                {
                    throw new InstallerException
                    (InstallerErrorMessage.INST_CORRUPT_PKG, null,
                     InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                     new String[] { attrName },
                     OtaStatusCode.JAR_SIZE_MISMATCH);
                }
            }
            // If the jar file is DRM protected, set content info accordingly.
            if (aBall.iJarFileType != null &&
                    (aBall.iJarFileType.equals(".dcf") ||
                     aBall.iJarFileType.equals(".dm")))
            {
                aBall.iSuite.setContentInfo(aBall.iSuite.CONTENT_INFO_DRM);
                Log.log("Suite is DRM protected");
            }
        }
    }

    /**
     * If jar file extension is one of the known ones this method
     * saves it to InstallBall.iJarFileType member.
     */
    private static void checkJarFileExtension(InstallBall aBall)
    {
        String lowerCaseJarFilename = aBall.iJarFilename.toLowerCase();
        if (aBall.iJarFileType == null)
        {
            aBall.iJarFileType = matchExtension(lowerCaseJarFilename, ".dcf");
        }
        if (aBall.iJarFileType == null)
        {
            aBall.iJarFileType = matchExtension(lowerCaseJarFilename, ".dm");
        }
    }

    /**
     * Checks if given filename has the specified extension.
     * Returns the matching extension, or null if the extension
     * does not match.
     */
    private static String matchExtension(String aFilename, String aExtension)
    {
        if (aFilename == null || aExtension == null)
        {
            return null;
        }
        if (aFilename.endsWith(aExtension))
        {
            return aExtension;
        }
        return null;
    }

    /**
     * If ball contains attributes Nokia-MIDlet-Name-<locale> or
     * Nokia-MIDlet-<n>-<locale>, store the correct localized name
     * to Nokia-MIDlet-Localized-Name or Nokia-MIDlet-Localized-<n>
     * for later use.
     */
    private static void setLocalizedNames(InstallBall aBall)
    {
        // currentLocale is for example 'es-MX' or 'en-US-Iron'
        String  currentLocale = System.getProperty("microedition.locale");
        Log.log("setLocalizedNames: current microedition.locale is " + currentLocale);
        boolean suiteNameLocalized  = false;
        Vector  midletNamesLocalized = new Vector();
        int idxMinus;
        int n;

        do
        {
            String localizedSuiteName =
                aBall.getAttributeValue("Nokia-MIDlet-Name-" + currentLocale);
            if ((localizedSuiteName != null) &&
                    (localizedSuiteName.length() > 0) &&
                    (suiteNameLocalized == false))
            {
                // Store localized name to new attribute.
                boolean trusted =
                    ((Attribute)aBall.iCombinedAttributes.get(
                         "Nokia-MIDlet-Name-" + currentLocale)).isTrusted();
                Attribute attr = new Attribute("Nokia-MIDlet-Localized-Name",
                                               localizedSuiteName, trusted);
                aBall.iCombinedAttributes.put(attr.getName(), attr);
                suiteNameLocalized = true;
                Log.log("setLocalizedNames: localized MIDlet suite name to " +
                        localizedSuiteName);
            }

            String localizedMidletAttribute = "Nokia-MIDlet-1-" + currentLocale;
            String localizedMidletName = null;
            String midletN = "MIDlet-1";
            n = 1;

            while (aBall.attributeExists(midletN))
            {
                localizedMidletName = aBall.getAttributeValue(localizedMidletAttribute);
                // If midlet N has already been localized do not localize again
                if ((localizedMidletName != null) && (localizedMidletName.length() > 0) &&
                        (!midletNamesLocalized.contains(new Integer(n))))
                {
                    // Store localized name to new attribute.
                    String nokiaMidletN = "Nokia-MIDlet-Localized-" + n;
                    boolean trusted =
                        ((Attribute)aBall.iCombinedAttributes.get(
                             localizedMidletAttribute)).isTrusted();
                    Attribute attr = new Attribute(nokiaMidletN, localizedMidletName, trusted);
                    aBall.iCombinedAttributes.put(nokiaMidletN, attr);
                    // Set information that midlet N has been localized
                    midletNamesLocalized.addElement(new Integer(n));
                    Log.log("setLocalizedNames: localized MIDlet " + n + " name to " +
                            localizedMidletName);
                }
                n++;
                midletN = "MIDlet-" + n;
                localizedMidletAttribute = "Nokia-MIDlet-" + n + "-" + currentLocale;
            }
            // Remember how many midlets the suite contains
            n--;

            // Now strip the most specific part, for example <variant>,
            // from locale and do this again until there are no more parts
            // or both MIDlet suite name and all MIDlet names have been localized
            idxMinus = currentLocale.lastIndexOf('-');
            if (idxMinus != -1)
            {
                currentLocale = currentLocale.substring(0, idxMinus);
            }
        }
        while (!(((midletNamesLocalized.size() == n) && suiteNameLocalized) ||
                 (idxMinus == -1)));
    }

}
