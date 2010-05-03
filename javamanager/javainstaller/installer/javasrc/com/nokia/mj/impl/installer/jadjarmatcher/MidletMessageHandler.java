/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.installer.jadjarmatcher;

import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.FileWriter;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.utils.Attribute;
import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;
import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.OtaStatusCode;
import com.nokia.mj.impl.utils.exception.InvalidAttributeException;

import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.Hashtable;
import java.util.jar.JarFile;
import java.util.jar.JarEntry;

public class MidletMessageHandler
{
    private static final String ATTR_PACKAGE = "Distribution-Package";
    private static final String ATTR_DESCRIPTOR = "Distribution-Descriptor-Filename";
    private static final String ATTR_CONTENT = "Distribution-Content-Filename";
    private static final String ATTR_FORWARD_LOCK = "Nokia-Forward-Lock";

    /** Installation drive where the midlet message will be extracted to. */
    private static int iInstallationDrive = -1;

    /**
     * Set installation drive where the midlet mesage will be extracted to.
     */
    public static void setInstallationDrive(int aDrive)
    {
        iInstallationDrive = aDrive;
    }

    /**
     * Returns true if given aJadJarFile.iJarAttributes indicate
     * that package is a midlet message, false otherwise. If package
     * is a midlet message, this method initializes jad and jar
     * filenames from midlet message attributes into aJadJarFile
     * parameter.
     */
    static boolean checkMidletMessage(JadJarFile aJadJarFile)
    {
        // Check if installation package is in midlet message format.
        String dPackage = getAttributeValue(
                              ATTR_PACKAGE, aJadJarFile.iJarAttributes);
        String ddFilename = getAttributeValue(
                                ATTR_DESCRIPTOR, aJadJarFile.iJarAttributes);
        String dcFilename = getAttributeValue(
                                ATTR_CONTENT, aJadJarFile.iJarAttributes);
        if (dPackage != null && dPackage.equals("1.0") &&
                ddFilename != null && ddFilename.length() > 0 &&
                dcFilename != null && dcFilename.length() > 0)
        {
            // Installation package is in midlet message format.
            Log.log("MidletMessage: " + aJadJarFile.iJarFilename);
            aJadJarFile.iJadFilename = ddFilename;
            aJadJarFile.iJarFilename = dcFilename;
            aJadJarFile.iMidletMessage = true;
        }
        return aJadJarFile.iMidletMessage;
    }

    /**
     * Handles midlet message package. This method assumes that the jad and
     * jar filename members in aJadJarFile parameter have the names of jad
     * and jar file entries in aJarFilename midlet message. Unzips the
     * midlet message contents to temporary files on disk and initializes
     * JadJarFile members to point to the temporary files.
     */
    static void handleMidletMessage(String aJarFilename, JadJarFile aJadJarFile)
    {
        Log.log("MidletMessage descriptor: " + aJadJarFile.iJadFilename +
                ", content: " + aJadJarFile.iJarFilename);
        String jadExt = FileUtils.getExtension(aJadJarFile.iJadFilename);
        if (jadExt == null || jadExt.length() == 0)
        {
            jadExt = ".jad";
        }
        String jarExt = FileUtils.getExtension(aJadJarFile.iJarFilename);
        if (jarExt == null || jarExt.length() == 0)
        {
            jarExt = ".jar";
        }
        // Check if application should be installed as Forward Locked.
        String frwdLockAttr = getAttributeValue(
                                  ATTR_FORWARD_LOCK, aJadJarFile.iJarAttributes);
        boolean frwdLock = false;
        if (frwdLockAttr != null && frwdLockAttr.toLowerCase().equals("true"))
        {
            frwdLock = true;
            jarExt = ".dcf";
        }
        // Extract jad and jar files from midlet message package.
        String tmpJadFilename = JadJarMatcherBase.getIntegrityService()
                                .createTempFile(iInstallationDrive, jadExt);
        String tmpJarFilename = JadJarMatcherBase.getIntegrityService()
                                .createTempFile(iInstallationDrive, jarExt);
        tmpJadFilename = extract(
                             aJarFilename, ATTR_DESCRIPTOR, aJadJarFile.iJadFilename,
                             "text/vnd.sun.j2me.app-descriptor", tmpJadFilename, false);
        tmpJarFilename = extract(
                             aJarFilename, ATTR_CONTENT, aJadJarFile.iJarFilename,
                             "application/java-archive", tmpJarFilename, frwdLock);
        // Initialize both iJadFilename and iJarFilename.
        aJadJarFile.iJadFilename = tmpJadFilename;
        aJadJarFile.iJarFilename = tmpJarFilename;
        // Set jad and jar attributes to be null so that they
        // will be initialised later.
        aJadJarFile.iJadAttributes = null;
        aJadJarFile.iJarAttributes = null;
    }

    /**
     * Extracts specifed aJarEntry from iJarFilename to aFilename.
     */
    private static String extract(
        String aJarFilename, String aAttrName, String aJarEntry,
        String aMimeType, String aFilename, boolean aLocked)
    {
        String resultFilename = aFilename;
        JarFile jarFile = null;
        InputStream is = null;
        FileWriter fw = null;
        try
        {
            jarFile = new JarFile(aJarFilename);
            JarEntry jarEntry = jarFile.getJarEntry(
                                    FileUtils.trimJarEntry(aJarEntry));
            if (jarEntry == null)
            {
                Log.logError("Entry " + aJarEntry + " not found from " +
                             aJarFilename);
                throw new InvalidAttributeException(
                    InstallerErrorMessage.INST_CORRUPT_PKG, null,
                    InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                    new String[] { aAttrName },
                    OtaStatusCode.INVALID_JAR);
            }
            is = jarFile.getInputStream(jarEntry);
            fw = new FileWriter(aMimeType, aFilename, aLocked);
            fw.create();
            // Copy the file from InputStream to FileWriter.
            byte[] buf = new byte[16384];
            int i = 0;
            while ((i = is.read(buf)) != -1)
            {
                fw.write(buf, 0, i);
            }
            // Close the FileWriter and get the result filename.
            fw.close();
            resultFilename = fw.getOutputFilename();
            fw = null;
            Log.log("Extracted " + aJarEntry + " from " +
                    aJarFilename + " to " + resultFilename);
        }
        catch (IOException ioe)
        {
            Log.logError("Exception while extracting " + aJarEntry + " from " +
                         aJarFilename + " to " + aFilename + ": " + ioe);
            throw new InstallerException
            (InstallerErrorMessage.INST_CORRUPT_PKG, null,
             InstallerDetailedErrorMessage.INTERNAL_ERROR,
             new String[] { "Extracting " + aJarEntry + " from " +
                            aJarFilename + " failed"
                          },
             OtaStatusCode.INVALID_JAR, ioe);
        }
        finally
        {
            // Close streams and jar file.
            if (fw != null)
            {
                try
                {
                    fw.close();
                    resultFilename = fw.getOutputFilename();
                    fw = null;
                }
                catch (IOException ioe)
                {
                    Log.logWarning("Closing FileWriter failed", ioe);
                }
            }
            if (is != null)
            {
                try
                {
                    is.close();
                    is = null;
                }
                catch (IOException ioe)
                {
                    Log.logWarning("Closing InputStream failed", ioe);
                }
            }
            if (jarFile != null)
            {
                try
                {
                    jarFile.close();
                    jarFile = null;
                }
                catch (IOException ioe)
                {
                    Log.logWarning("Closing " + aJarFilename + " failed", ioe);
                }
            }
        }
        return resultFilename;
    }

    private static String getAttributeValue(String aName, Hashtable aAttrs)
    {
        Attribute attr = (Attribute)aAttrs.get(aName);
        if (attr != null)
        {
            return attr.getValue();
        }
        return null;
    }
}
