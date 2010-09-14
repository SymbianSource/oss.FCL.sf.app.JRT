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

import com.nokia.mj.impl.installer.applicationregistrator.SifRegistrator;
import com.nokia.mj.impl.installer.storagehandler.ApplicationInfo;
import com.nokia.mj.impl.installer.storagehandler.SuiteInfo;
import com.nokia.mj.impl.installer.storagehandler.StorageHandler;
import com.nokia.mj.impl.installer.integrityservice.IntegrityService;
import com.nokia.mj.impl.installer.jadjarmatcher.JadJarFile;
import com.nokia.mj.impl.installer.jadjarmatcher.JadJarMatcher;
import com.nokia.mj.impl.installer.jadjarmatcher.MidletMessageHandler;
import com.nokia.mj.impl.installer.utils.Args;
import com.nokia.mj.impl.installer.utils.ComponentId;
import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.JadReader;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.installer.utils.MidpAttributeValidator;
import com.nokia.mj.impl.installer.utils.PlatformUid;
import com.nokia.mj.impl.security.midp.authentication.AuthenticationModule;
import com.nokia.mj.impl.security.midp.common.AuthenticationCredentials;
import com.nokia.mj.impl.security.midp.common.SecurityAttributes;
import com.nokia.mj.impl.utils.Attribute;
import com.nokia.mj.impl.utils.JarManifestReader;
import com.nokia.mj.impl.utils.Tokenizer;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.utils.Version;

import java.io.IOException;
import java.util.Hashtable;
import java.util.Vector;

/**
 * This class implements functionlity for getting information of
 * an installed or not installed application.
 */
public class GetComponentInfo
{
    // Constants for install status.
    static final int NEW_COMPONENT = 0;
    static final int UPGRADE = 1;
    static final int ALREADY_INSTALLED = 2;
    static final int NEWER_VERSION_ALREADY_INSTALLED = 3;
    static final int INVALID_PACKAGE = 4;

    // There can be only one instance of IntegrityService at a time
    // so concurrent execution of getComponentInfo must be limited.
    private static final Object iSync = new Object();
    private IntegrityService iIntegrityService = null;

    // Command line arguments
    private Args iArgs = null;
    private String iJarFilename = null;
    private String iJadFilename = null;
    private String iJadCharset = null;
    private Uid iUid = null;
    private int[] iCommsResultEndpoints = null;
    private int iTmpDrive = -1;

    // Jad/Manifest attributes
    private Hashtable iJarAttributes = null;
    private Hashtable iJadAttributes = null;
    private Hashtable iCombinedAttributes = null;

    // Suite info from JavaStorage
    private SuiteInfo iSuite = null;

    // Security information
    private SecurityAttributes iSecurityAttributes = null;
    private boolean iIsTrusted = false;

    // Install status.
    private int iInstallStatus = NEW_COMPONENT;
    private Exception iException = null;

    /**
     * Default constructor.
     */
    public GetComponentInfo()
    {
    }

    /**
     * Main method for getting component info. This method sends
     * information of the requested component using InstallerResultMessage
     * to the Comms endpoint given as an argument. Component can be
     * identifed using either -jad, -jar or -uid argument.
     */
    public int getComponentInfo(Args aArgs)
    {
        int result = Installer.ERR_NONE;
        synchronized (iSync)
        {
            Log.log("getComponentInfo begins");

            // Reset member variables.
            iJarAttributes = null;
            iJadAttributes = null;
            iCombinedAttributes = null;
            iSuite = null;

            // Get IntegrityService instance.
            iIntegrityService = IntegrityService.getInstance(
                                    FileUtils.getIntegrityServiceRoot());

            // Get component info.
            try
            {
                result = parseArgs(aArgs);
                if (result == Installer.ERR_NONE)
                {
                    findJadJar();
                    getAttributes();
                    result = getFromStorage();
                }
                if (result == Installer.ERR_NONE)
                {
                    authenticate();
                    getFromSif();
                }
            }
            catch (Exception e)
            {
                result = Installer.ERR_GENERAL;
                iException = e;
                Log.logError("Getting component info failed", e);
            }

            // Close IntegrityService by making rollback.
            if (iIntegrityService.rollback())
            {
                Log.log("IntegrityService rolled back");
            }
            else
            {
                Log.logError("IntegrityService rollback failed");
            }

            // Send InstallerResultMessage.
            try
            {
                sendResult(result);
            }
            catch (Throwable t)
            {
                Log.logError("Sending component info failed", t);
                result = Installer.ERR_GENERAL;
            }
            Log.log("getComponentInfo returns " + result);
        }
        return result;
    }

    /**
     * Parses command line arguments.
     */
    private int parseArgs(Args aArgs)
    {
        Log.log("parseArgs begins");
        int result = Installer.ERR_NONE;
        iArgs = aArgs;
        iJadFilename = iArgs.get("jad");
        iJarFilename = iArgs.get("jar");
        if (iJadFilename != null && iJarFilename != null)
        {
            InstallerException.internalError(
                "Specify either jad or jar as an argument, not both.");
        }
        iJadCharset = iArgs.get("charset");

        String arg = iArgs.get("uid");
        if (arg != null)
        {
            if (iJadFilename != null || iJarFilename != null)
            {
                InstallerException.internalError(
                    "Specify either jad, jar or uid as an argument.");
            }
            Uid uid = PlatformUid.createUid(arg);
            if (uid == null)
            {
                InstallerException.internalError("Invalid uid " + arg);
            }
            iUid = uid;
        }

        arg = iArgs.get("commsresult");
        if (arg != null)
        {
            iCommsResultEndpoints = InstallerResultMessage.parseEndpoints(arg);
        }

        // Check that file exists and store temporary files to the
        // same drive where the component is located.
        if (iJadFilename != null)
        {
            iTmpDrive = FileUtils.getDrive(iJadFilename);
            if (!FileUtils.exists(iJadFilename) &&
                    !FileUtils.isInboxFile(iJadFilename))
            {
                result = Installer.ERR_NOT_FOUND;
                Log.log("File not found: " + iJadFilename);
            }
        }
        if (iJarFilename != null)
        {
            iTmpDrive = FileUtils.getDrive(iJarFilename);
            if (!FileUtils.exists(iJarFilename) &&
                    !FileUtils.isInboxFile(iJarFilename))
            {
                result = Installer.ERR_NOT_FOUND;
                Log.log("File not found: " + iJarFilename);
            }
        }
        Log.log("parseArgs returns " + result);
        return result;
    }

    /**
     * Finds matching Jad and Jar.
     */
    private void findJadJar()
    {
        Log.log("findJadJar begins");

        // Initialise JadJarMatcher classes.
        JadJarMatcher.setIntegrityService(iIntegrityService);
        MidletMessageHandler.setInstallationDrive(iTmpDrive);

        // Try to find jar basing on local jad file.
        if (iJadFilename != null)
        {
            JadJarFile jadJarFile =
                JadJarMatcher.findJar(iJadFilename, iJadCharset);
            iJadFilename = jadJarFile.iJadFilename;
            iJarFilename = jadJarFile.iJarFilename;
            iJadAttributes = jadJarFile.iJadAttributes;
            iJarAttributes = jadJarFile.iJarAttributes;
            if (iJarFilename != null)
            {
                Log.log("Found matching Jar: " + iJarFilename);
            }
            else
            {
                Log.log("Matching Jar not found locally");
            }
        }
        // Try to find jad basing on local jar file.
        else if (iJarFilename != null)
        {
            JadJarFile jadJarFile =
                JadJarMatcher.findJad(iJarFilename);
            iJadFilename = jadJarFile.iJadFilename;
            iJarFilename = jadJarFile.iJarFilename;
            iJadAttributes = jadJarFile.iJadAttributes;
            iJarAttributes = jadJarFile.iJarAttributes;
            if (iJadFilename != null)
            {
                Log.log("Found matching Jad: " + iJadFilename);
            }
            else
            {
                Log.log("Matching Jad not found locally");
            }
        }

        Log.log("findJadJar returns");
    }

    /**
     * Gets and validates jad and manifest attributes.
     */
    private void getAttributes() throws IOException
    {
        Log.log("getAttributes begins");
        if (iJadFilename != null && iJadAttributes == null)
        {
            Log.log("Reading Jad...");
            iJadAttributes =
                JadReader.getAttributes(iJadFilename, iJadCharset);
            if (iJadAttributes == null)
            {
                throw new IOException(
                    "No Jad attributes found from " + iJadFilename);
            }
        }

        if (iJarFilename != null && iJarAttributes == null)
        {
            Log.log("Reading Jar...");
            iJarAttributes =
                JarManifestReader.getAttributes(iJarFilename);
            if (iJarAttributes == null)
            {
                throw new IOException(
                    "No Jar attributes found from " + iJarFilename);
            }
        }

        try
        {
            // Validate and combine the attributes.
            MidpAttributeValidator attributeValidator =
                new MidpAttributeValidator();
            if (iJadAttributes != null)
            {
                attributeValidator.validate(
                    iJadAttributes, attributeValidator.JAD);
                // Get security attributes.
                iSecurityAttributes = new SecurityAttributes();
                iSecurityAttributes.addDescriptorAttributes(iJadAttributes);
                iIsTrusted = iSecurityAttributes.isTrusted();
            }
            if (iJarAttributes != null)
            {
                attributeValidator.validate(
                    iJarAttributes, attributeValidator.JAR);
            }
            iCombinedAttributes = attributeValidator.combine(
                                      iJadAttributes, iJarAttributes, iIsTrusted);
        }
        catch (Exception e)
        {
            // Attribute validation failure means that
            // application cannot be installed.
            iInstallStatus = INVALID_PACKAGE;
            iException = e;
            Log.log("Attribute validation failed", e);
        }
        Log.log("getAttributes returns");
    }

    /**
     * Gets application suite information from JavaStorage.
     */
    private int getFromStorage()
    {
        Log.log("getFromStorage begins");
        int result = Installer.ERR_NONE;
        StorageHandler storageHandler = null;
        try
        {
            storageHandler = new StorageHandler();
            if (iUid != null)
            {
                // Read from JavaStorage by uid.
                Uid uid = storageHandler.getSuiteUid(iUid);
                if (uid != null)
                {
                    iSuite = new SuiteInfo(uid);
                }
            }
            else
            {
                // Read from JavaStorage by name and vendor.
                iSuite = new SuiteInfo(
                    getAttributeValue("MIDlet-Name"),
                    getAttributeValue("MIDlet-Vendor"));
            }
            if (iSuite != null && storageHandler.readSuiteInfo(iSuite))
            {
                Log.log("Suite found from JavaStorage: " + iSuite.toString());
                iInstallStatus = ALREADY_INSTALLED;
                Version version = Version.getVersion(
                                      getAttributeValue("MIDlet-Version"));
                if (version != null)
                {
                    int versionComparison =
                        version.compareTo(iSuite.getVersion());
                    if (versionComparison > 0)
                    {
                        iInstallStatus = UPGRADE;
                    }
                    else if (versionComparison == 0)
                    {
                        iInstallStatus = ALREADY_INSTALLED;
                    }
                    else if (versionComparison < 0)
                    {
                        iInstallStatus = NEWER_VERSION_ALREADY_INSTALLED;
                    }
                }
            }
            else
            {
                Log.log("Suite not found from JavaStorage");
                iSuite = null;
                if (iUid != null)
                {
                    // If component info is requested by uid
                    // and not found from JavaStorage, return
                    // "not found" error code.
                    result = Installer.ERR_NOT_FOUND;
                }
            }
        }
        finally
        {
            if (storageHandler != null)
            {
                storageHandler.close();
            }
        }
        Log.log("getFromStorage returns " + result);
        return result;
    }

    /**
     * Gets component ids from SIF if application is already installed.
     */
    private void getFromSif()
    {
        if (iSuite != null)
        {
            Log.log("getFromSif begins");
            // Suite exists, now get component ids from SIF.
            SifRegistrator sr = new SifRegistrator();
            sr.startSession(false);
            try
            {
                iSuite.setComponentId(sr.getComponentId(iSuite.getGlobalId()));
                Log.log("getFromSif " + iSuite.getGlobalId() +
                        ", cid: " + iSuite.getComponentId());
                Vector apps = iSuite.getApplications();
                for (int i = 0; i < apps.size(); i++)
                {
                    ApplicationInfo appInfo =
                        (ApplicationInfo)apps.elementAt(i);
                    appInfo.setComponentId(
                        sr.getComponentId(iSuite.getGlobalId(i)));
                    Log.log("getFromSif " + iSuite.getGlobalId(i) +
                            ", cid: " + appInfo.getComponentId());
                }
            }
            finally
            {
                sr.closeSession();
            }
            Log.log("getFromSif returns");
        }
    }

    /**
     * Authenticates Jad and Jar.
     */
    private void authenticate()
    {
        Log.log("authenticate begins");
        if (iSecurityAttributes != null)
        {
            try
            {
                // Use dummy uid in case suite has not been installed yet.
                Uid suiteUid = PlatformUid.createUid(0);
                if (iSuite != null)
                {
                    suiteUid = iSuite.getUid();
                }

                // Authenticate jad.
                AuthenticationCredentials[] authenticationCredentials =
                    AuthenticationModule.getInstance().authenticateJad(
                        suiteUid, null,
                        iSecurityAttributes.getAuthenticationAttributes());

                if (authenticationCredentials != null)
                {
                    for (int i = 0; i < authenticationCredentials.length; i++)
                    {
                        String domain = authenticationCredentials[i]
                            .getProtectionDomainCategory();
                        Log.log("Protection domain: " + domain);
                    }
                }
                if (iJarFilename != null)
                {
                    // Authenticate jar.
                    AuthenticationModule.getInstance().authenticateJar(
                        suiteUid, null, iJarFilename,
                        FileUtils.isDrmProtected(iJarFilename));
                }
            }
            catch (Exception e)
            {
                // If authentication fails it means that
                // application cannot be installed.
                iInstallStatus = INVALID_PACKAGE;
                iException = e;
                Log.log("Authentication failed", e);
            }
        }
        Log.log("authenticate returns");
    }

    /**
     * Sends InstallerResultMessage.
     */
    private void sendResult(int aResult)
    {
        if (iCommsResultEndpoints == null ||
                iCommsResultEndpoints.length == 0)
        {
            return;
        }
        InstallerResultMessage msg = new InstallerResultMessage();
        msg.addValue(msg.NAME_OPERATION, 2);
        msg.addResult(iException);
        msg.addValue(msg.NAME_RESULT, aResult);
        if (aResult == Installer.ERR_NONE)
        {
            // Init msg either using iSuite, iCombinedAttributes or both.
            if (iSuite != null && iCombinedAttributes == null)
            {
                // Init msg from iSuite only.
                msg.init(iSuite);
            }
            else if (iCombinedAttributes != null)
            {
                String[][] midletNAttributes = getMidletNAttributeValues();
                if (iSuite != null)
                {
                    // Init available suite and application uids from iSuite.
                    msg.addValue(msg.NAME_SUITE_UID,
                                 PlatformUid.getIntValue(iSuite.getUid()));
                    if (iSuite.getComponentId() != null)
                    {
                        msg.addValue(msg.NAME_SUITE_CID,
                                     iSuite.getComponentId().getId());
                    }
                    Vector apps = iSuite.getApplications();
                    for (int i = 0; i < midletNAttributes.length; i++)
                    {
                        int index = i+1;
                        if (i < apps.size())
                        {
                            ApplicationInfo app = (ApplicationInfo)apps.elementAt(i);
                            msg.addValue(msg.NAME_MIDLET_UID+index,
                                         PlatformUid.getIntValue(app.getUid()));
                            if (app.getComponentId() != null)
                            {
                                msg.addValue(msg.NAME_MIDLET_CID+index,
                                             app.getComponentId().getId());
                            }
                        }
                    }
                }

                // Init message from attributes read from jad/jar.
                // Note that uids for not installed applications
                // are not returned, unless they are defined with
                // Nokia-MIDlet-UID-n attribute.
                msg.addValue(msg.NAME_SUITE_GID,
                             SuiteInfo.getGlobalId(
                                 getAttributeValue("MIDlet-Vendor"),
                                 getAttributeValue("MIDlet-Name"), null));
                addAttributeValue(msg, msg.NAME_SUITE_NAME, "MIDlet-Name");
                addAttributeValue(msg, msg.NAME_VENDOR, "MIDlet-Vendor");
                addAttributeValue(msg, msg.NAME_VERSION, "MIDlet-Version");
                for (int i = 0; i < midletNAttributes.length; i++)
                {
                    int index = i+1;
                    String midletName = midletNAttributes[i][0].trim();
                    msg.addValue(msg.NAME_MIDLET_NAME+index, midletName);
                    msg.addValue(msg.NAME_MIDLET_GID+index,
                                 SuiteInfo.getGlobalId(
                                     getAttributeValue("MIDlet-Vendor"),
                                     getAttributeValue("MIDlet-Name"),
                                     midletName));
                    String midletUid =
                        getAttributeValue("Nokia-MIDlet-UID-" + index);
                    if (midletUid != null)
                    {
                        msg.addValue(
                            msg.NAME_MIDLET_UID+index,
                            PlatformUid.getIntValue(
                                PlatformUid.createUid(midletUid)));
                    }
                }

                // Calculate size of the installation package.
                int initialSize = 0;
                String dataSize = getAttributeValue("MIDlet-Data-Size");
                String jarSize = getAttributeValue("MIDlet-Jar-Size");
                try
                {
                    if (dataSize != null)
                    {
                        initialSize += Integer.parseInt(dataSize);
                    }
                    if (jarSize != null)
                    {
                        initialSize += Integer.parseInt(jarSize);
                    }
                }
                catch (NumberFormatException nfe)
                {
                    // ignore
                }
                if (initialSize == 0 && iJarFilename != null)
                {
                    // Get initialSize from jar file size.
                    initialSize = (int)FileUtils.getSize(iJarFilename);
                }
                msg.addValue(msg.NAME_COMPONENT_SIZE, initialSize);
            }
            msg.addValue(msg.NAME_INSTALL_STATUS, iInstallStatus);
            if (iInstallStatus != INVALID_PACKAGE)
            {
                // Add authenticity value only when the package is found
                // to be valid.
                msg.addValue(msg.NAME_AUTHENTICITY, (iIsTrusted? 1: 0));
            }
        }
        Log.log("Sending " + msg.toString());
        msg.send(iCommsResultEndpoints);
    }

    /**
     * Adds specified value to given message if the value exists
     * in iCombinedAttributes member. If value does not exist in
     * iCombinedAttributes then value is removed from the message.
     */
    private void addAttributeValue(
        InstallerResultMessage aMsg, String aValueName, String aAttrName)
    {
        String value = getAttributeValue(aAttrName);
        if (value == null)
        {
            aMsg.removeValue(aValueName);
        }
        else
        {
            aMsg.addValue(aValueName, value);
        }
    }

    /**
     * Returns an array of MIDlet-n attribute values from iCombinedAttaributes.
     */
    private String[][] getMidletNAttributeValues()
    {
        String attrName = "MIDlet-";
        String attrValue = "";
        Vector attrs = new Vector();
        int i = 1;
        while (attrValue != null)
        {
            attrValue = getAttributeValue(attrName + i);
            if (attrValue != null)
            {
                String[] attrTokens = Tokenizer.split(attrValue, ",");
                // attrTokens has midlet name, icon, and class.
                attrs.addElement(attrTokens);
            }
            i++;
        }
        String[][] result = new String[attrs.size()][];
        attrs.copyInto(result);
        return result;
    }

    /**
     * Returns a string type attribute value from iCombinedAttributes.
     */
    private String getAttributeValue(String aName)
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
}
