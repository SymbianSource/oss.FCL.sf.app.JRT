/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

import com.nokia.mj.impl.installer.applicationregistrator.ApplicationRegistrator;
import com.nokia.mj.impl.installer.exetable.ExeBall;
import com.nokia.mj.impl.installer.exetable.ExeStep;
import com.nokia.mj.impl.installer.storagehandler.ApplicationInfo;
import com.nokia.mj.impl.installer.storagehandler.SuiteInfo;
import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.installer.utils.PlatformUid;
import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;
import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.OtaStatusCode;
import com.nokia.mj.impl.utils.Tokenizer;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.utils.exception.InvalidAttributeException;

import java.io.InputStream;
import java.io.IOException;
import java.util.Vector;

public class SelectUids extends ExeStep
{
    private static final String MIDLET_UID_ATTR_NAME = "Nokia-MIDlet-UID-";

    // Uids read from 'uids' file, this is used only when
    // converting midlets from old java to OMJ.
    private Uid[] iUidsFromFile = null;

    public void execute(ExeBall aBall)
    {
        InstallBall ball = (InstallBall)aBall;
        //ball.log("Selecting uids...");
        // SelectUids step is called in two phases because suite uid
        // must be allocated before security framework is used,
        // but application uids must be selected after jar file has
        // been downloaded because jar can have more MIDlet-n
        // attributes than jad.
        if (ball.iSuite.getUid() == null)
        {
            // First phase: select suite uid and application uids
            // basing on attributes that are available.
            readUidsFromFile(ball);
            selectSuiteUid(ball);
            selectApplicationUids(ball);
            // Set application root, jad and jar paths.
            updateSuitePaths(ball);
        }
        else
        {
            // Second phase: allocate application uids for those
            // applications which don't yet have uids allocated.
            selectApplicationUids(ball);
            // Check that all Nokia-MIDlet-UID-n attributes are
            // present in jar for trusted applications.
            checkNokiaMidletUids(ball);
        }
        // Log all suite info.
        //ball.log(ball.iSuite.toString());
    }

    public void cancel(ExeBall aBall)
    {
        // nop
    }

    /**
     * Selects suite uid.
     */
    private void selectSuiteUid(InstallBall ball)
    {
        // Select suite uid.
        Log.log("Selecting suite uid...");
        if (ball.iConversionInstallation &&
                iUidsFromFile != null && iUidsFromFile.length > 0)
        {
            // Conversion installation, use uid from 'uids' file.
            ball.iSuite.setUid(iUidsFromFile[0]);
            ball.log("Selected suite uid from file: " + ball.iSuite.getUid());
        }
        else if (ball.iOldSuite != null)
        {
            // Old suite exists, use its uid.
            ball.iSuite.setUid(ball.iOldSuite.getUid());
            ball.log("Selected old suite uid: " + ball.iSuite.getUid());
        }
        else
        {
            // Allocate a new suite uid.
            ball.iSuite.setUid
            (allocateUid(ball, ball.getAttributeValue("MIDlet-Name")));
            ball.log("Allocated new suite uid: " + ball.iSuite.getUid());
        }
    }

    /**
     * Selects application uids.
     */
    private void selectApplicationUids(InstallBall ball)
    {
        // Select application uids.
        int startIndex = ball.iSuite.getApplications().size() + 1;
        Log.log("Selecting application uids, startIndex " + startIndex);
        for (int i = startIndex; true; i++)
        {
            String appAttrName = "MIDlet-" + i;
            String appAttrValue = ball.getAttributeValue(appAttrName);
            if (appAttrValue != null)
            {
                Uid uid = null;
                // Split MIDlet-n attribute into tokens.
                String[] tokens = Tokenizer.split(appAttrValue, ",");
                if (tokens == null || tokens.length != 3)
                {
                    Log.logError("Invalid " + appAttrName +
                                 " attribute value: " + appAttrValue);
                    throw new InvalidAttributeException
                    (InstallerErrorMessage.INST_CORRUPT_PKG, null,
                     InstallerDetailedErrorMessage.ATTR_BAD_SYNTAX,
                     new String[] { appAttrName },
                     (ball.attributeExistsInJad(appAttrName)?
                      OtaStatusCode.INVALID_DESCRIPTOR:
                      OtaStatusCode.INVALID_JAR));
                }
                if (ball.iConversionInstallation &&
                        iUidsFromFile != null && iUidsFromFile.length > i)
                {
                    // Conversion installation, use uid from 'uids' file.
                    uid = iUidsFromFile[i];
                    ball.log("Selected app uid from file: " + uid);
                }
                if (uid == null)
                {
                    // Check if uid is specified with Nokia-MIDlet-UID attribute.
                    uid = selectAppUidFromAttribute(ball, i);
                }
                if (uid == null && ball.iOldSuite != null)
                {
                    // No uid yet, check from old suite.
                    Vector oldApps = ball.iOldSuite.getApplications();
                    if (oldApps.size() >= i)
                    {
                        // Use old application uid.
                        uid = ((ApplicationInfo)oldApps.elementAt(i-1)).getUid();
                        // Check that uid is not already in use in this
                        // suite. This could happen if Nokia-MIDlet-UID
                        // attribute was used to select uid for different
                        // app in the previous version of this suite.
                        if (appUidInUse(uid, ball.iSuite))
                        {
                            // Uid is already in use in this suite,
                            // it cannot be used twice.
                            Log.log("Old uid already in use: " + uid);
                            uid = null;
                        }
                        if (uid != null)
                        {
                            ball.log("Selected old app uid: " + uid);
                        }
                    }
                }
                if (uid == null)
                {
                    // If no uid chosen yet, then allocate a new application uid.
                    uid = allocateUid(ball, tokens[0].trim());
                    ball.log("Allocated new app uid: " + uid);
                }
                // Add a new ApplicationInfo to suite.
                ball.iSuite.addApplication
                (new ApplicationInfo
                 (uid, tokens[0].trim(), tokens[1].trim(),
                  tokens[2].trim(), ApplicationInfo.AUTOSTART_FALSE));
            }
            else
            {
                break;
            }
        }
    }

    /**
     * Selects application uid from Nokia-MIDlet-UID attribute.
     * @return uid if attribute specifies it, null otherwise.
     * @throws InstallerException if attribute exists and is not valid
     * or specified uid is already in use.
     */
    private Uid selectAppUidFromAttribute(InstallBall aBall, int aIndex)
    {
        Uid uid = null;
        String appUidAttrName = MIDLET_UID_ATTR_NAME + aIndex;
        String appUidAttrValue = aBall.getAttributeValue(appUidAttrName);
        if (appUidAttrValue != null)
        {
            uid = PlatformUid.createUid(appUidAttrValue);
            if (uid == null)
            {
                Log.logError("Invalid " + appUidAttrName +
                             " attribute value: " + appUidAttrValue);
                throw new InvalidAttributeException
                (InstallerErrorMessage.INST_CORRUPT_PKG, null,
                 InstallerDetailedErrorMessage.ATTR_BAD_SYNTAX,
                 new String[] { appUidAttrName },
                 (aBall.attributeExistsInJad(appUidAttrName)?
                  OtaStatusCode.INVALID_DESCRIPTOR:
                  OtaStatusCode.INVALID_JAR));
            }
            boolean uidInJad = aBall.attributeExistsInJad(appUidAttrName);
            // If jar file is not yet present, assume that uid
            // will be present in jar.
            boolean uidInJar = true;
            if (aBall.iJarFilename != null)
            {
                uidInJar = aBall.attributeExistsInJar(appUidAttrName);
            }
            if (uidInJar && !uidInJad && aBall.iJadAttributes != null)
            {
                // If Uid is specified in jar but not in jad
                // it must be ignored. Getting applications
                // through Java Verified process requires
                // that applications using protected Uids
                // can be installed as untrusted (without jad).
                Log.logWarning(
                    appUidAttrName +
                    " attribute present in jar but not in jad, ignoring it");
                uid = null;
            }
            if (uid != null)
            {
                // Check that uid is valid.
                PlatformUid.checkValidity
                (appUidAttrName, uid,
                 aBall.iSuite.isTrusted(),
                 uidInJad, uidInJar);
                // Check that uid is not already in use.
                if (appUidInUse(uid, aBall) ||
                        appUidInUse(uid, aBall.iSuite))
                {
                    Log.logError("Uid " + uid + " is already in use");
                    throw new InvalidAttributeException
                    (InstallerErrorMessage.INST_CORRUPT_PKG, null,
                     InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                     new String[] { appUidAttrName },
                     (aBall.attributeExistsInJad(appUidAttrName)?
                      OtaStatusCode.INVALID_DESCRIPTOR:
                      OtaStatusCode.INVALID_JAR));
                }
            }
            aBall.log("Selected app uid from " + appUidAttrName +
                      " attribute: " + uid);
        }
        return uid;
    }

    /**
     * Updates root, jad and jar paths to suite info object.
     */
    private void updateSuitePaths(InstallBall aBall)
    {
        aBall.iSuite.setRootDir
        (FileUtils.getAppsRoot() +
         FileUtils.getAppRootPath(aBall.iSuite.getUid()));
        if (aBall.iJadFilename != null)
        {
            if (aBall.iPreinstallation)
            {
                // In preinstallation case use file without copying it to
                // suite root dir.
                aBall.iSuite.setJadPath(aBall.iJadFilename);
            }
            else
            {
                // Set jar path to point under suite root dir.
                aBall.iSuite.setJadPath
                (aBall.iSuite.getRootDir() +
                 FileUtils.getName(aBall.iJadFilename));
            }
        }
        if (aBall.iJarFilename != null)
        {
            if (aBall.iPreinstallation)
            {
                // In preinstallation case use file without copying it to
                // suite root dir.
                aBall.iSuite.setJarPath(aBall.iJarFilename);
            }
            else
            {
                // Set jar path to point under suite root dir.
                aBall.iSuite.setJarPath
                (aBall.iSuite.getRootDir() +
                 FileUtils.getName(aBall.iJarFilename));
            }
        }
    }

    /**
     * Returns true if given application uid is used by someone else
     * than previous version of this application suite.
     *
     * @param aUid uid to be checked
     * @param aBall InstallBall object
     * @return true if uid is already in use, false if this suite can
     * use given uid
     */
    private boolean appUidInUse(Uid aUid, InstallBall aBall)
    {
        boolean result = aBall.iApplicationRegistrator.uidInUse(aUid);
        if (result)
        {
            // Uid is in use, check if it is used by previous version
            // of this suite.
            if (appUidInUse(aUid, aBall.iOldSuite))
            {
                // Uid is in use by previous version of
                // this suite, we can still use it so
                // let's return false.
                result = false;
            }
        }
        return result;
    }

    /**
     * Returns true if given uid is already in use in given suite.
     *
     * @param aUid uid to be checked
     * @param aSuite application suite info object
     * @return true if uid is already in use, false otherwise
     */
    private boolean appUidInUse(Uid aUid, SuiteInfo aSuite)
    {
        if (aSuite == null)
        {
            return false;
        }
        boolean result = false;
        Uid[] uids = aSuite.getApplicationUids();
        if (uids != null)
        {
            for (int i = 0; !result && i < uids.length; i++)
            {
                if (aUid.equals(uids[i]))
                {
                    // Uid is already in use.
                    result = true;
                }
            }
        }
        return result;
    }

    /**
     * Allocate a new uid which is not in use in JavaStorage or used by
     * some non-Java applications.
     */
    private Uid allocateUid(InstallBall aBall, String aAppName)
    {
        Uid uid = null;
        String uidSeed =
            aAppName + ":" + aBall.getAttributeValue("MIDlet-Vendor");
        do
        {
            if (uid == null)
            {
                uid = aBall.iStorageHandler.allocateUid(uidSeed);
            }
            else
            {
                //Log.log("Generated uid " + uid +
                //    " was already in use, allocating a new one");
                uid = aBall.iStorageHandler.allocateUid(uid);
            }
            // Uid returned by storage handler is not in use
            // in Java Storage, but check if the platform
            // is already using it for a non-Java application,
            // or if it has already been allocated for this suite.
        }
        while (aBall.iApplicationRegistrator.uidInUse(uid) ||
                uid.equals(aBall.iSuite.getUid()) ||
                appUidInUse(uid, aBall.iSuite));
        return uid;
    }

    /**
     * Reads uids from 'uids' file during conversion of
     * midlets from old java to OMJ.
     */
    private void readUidsFromFile(InstallBall aBall)
    {
        if (!aBall.iConversionInstallation)
        {
            return;
        }
        String uidsFilename = aBall.iConversionRoot + "uids";
        if (FileUtils.exists(uidsFilename))
        {
            Log.log("Reading uids file: " + uidsFilename);
            Vector v = new Vector();
            InputStream is = null;
            try
            {
                is = FileUtils.getInputStream(uidsFilename);
                while (is.available() > 0)
                {
                    v.addElement(readUid(is));
                }
            }
            catch (IOException ioe)
            {
                InstallerException.internalError
                ("Error while reading uids file: "  + uidsFilename, ioe);
            }
            finally
            {
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
            }
            iUidsFromFile = new Uid[v.size()];
            v.copyInto(iUidsFromFile);
            for (int i = 0; i < iUidsFromFile.length; i++)
            {
                Log.log("Read uid[" + i + "]: " + iUidsFromFile[i]);
            }
        }
        else
        {
            InstallerException.internalError("No uids file: "  + uidsFilename);
        }
    }

    /**
     * Reads a Uid from specified InputStream.
     */
    private static Uid readUid(InputStream aInputStream) throws IOException
    {
        int uid = 0;
        for (int i = 0; i < 4; i++)
        {
            uid |= (aInputStream.read() << i*8);
        }
        return PlatformUid.createUid(uid);
    }

    /**
     * Checks that all specified  Nokia-MIDlet-UID-n attributes are
     * present in jar for trusted application. Throws InstallerException
     * if attributes are not present.
     */
    private static void checkNokiaMidletUids(InstallBall aBall)
    {
        if (aBall == null || !aBall.iSuite.isTrusted())
        {
            return;
        }
        for (int i = 1; true; i++)
        {
            // Check if uid is specified with Nokia-MIDlet-UID attribute.
            String appUidAttrName = MIDLET_UID_ATTR_NAME + i;
            String appUidAttrValue = aBall.getAttributeValue(appUidAttrName);
            if (appUidAttrValue != null)
            {
                // Uid is specified with attribute, check that attribute
                // is present in jar.
                if (!aBall.attributeExistsInJar(appUidAttrName))
                {
                    Log.logError(
                        "For trusted application Uid must be specified in Jar: " +
                        appUidAttrName);
                    throw new InvalidAttributeException
                    (InstallerErrorMessage.INST_CORRUPT_PKG, null,
                     InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                     new String[] { appUidAttrName },
                     OtaStatusCode.INVALID_JAR);
                }
            }
            else
            {
                break;
            }
        }
    }
}
