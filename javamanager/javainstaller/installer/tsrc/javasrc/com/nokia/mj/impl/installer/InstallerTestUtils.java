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

import com.nokia.mj.impl.installer.applicationregistrator.ApplicationRegistrator;
import com.nokia.mj.impl.installer.storagehandler.StorageHandler;
import com.nokia.mj.impl.installer.storagehandler.SuiteInfo;
import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.JadReader;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.installer.utils.Platform;
import com.nokia.mj.impl.utils.Attribute;
import com.nokia.mj.impl.utils.JarManifestReader;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.utils.exception.InstallerExceptionBase;

import java.io.IOException;
import java.util.Hashtable;

/**
 * Utilities for InstallerEngine unit tests.
 */
public class InstallerTestUtils
{
    private static ApplicationRegistrator iApplicationRegistrator = null;

    public static void init()
    {
        iApplicationRegistrator = new ApplicationRegistrator();
        iApplicationRegistrator.startSession();
    }

    public static void destroy()
    {
        iApplicationRegistrator.closeSession();
        iApplicationRegistrator = null;
    }

    /**
     * Returns path for the test applications.
     */
    public static String getTestDir()
    {
        return FileUtils.getInstallerRoot() +
               "jiutdata" +
               FileUtils.pathSeparator() +
               "installerengine" +
               FileUtils.pathSeparator();
    }

    /**
     * Returns Suiteinfo initialized with Name and Vendor information
     * from given jad or jar file.
     *
     * @param aFilename name of jad or jar file
     * @param aIsJad true if file is a jad file, false if file is a jar file
     * @returns SuiteInfo initialized with name and vendor
     * @throws IOException if an error occurs when reading file or
     * name and vendor information is not found
     */
    public static SuiteInfo getNameVendor(String aFilename, boolean aIsJad)
    throws IOException
    {
        Hashtable attrs = null;
        if (aIsJad)
        {
            attrs = JadReader.getAttributes(aFilename);
        }
        else
        {
            attrs = JarManifestReader.getAttributes(aFilename);
        }
        Attribute name = (Attribute)attrs.get("MIDlet-Name");
        Attribute vendor = (Attribute)attrs.get("MIDlet-Vendor");
        if (name == null)
        {
            throw new IOException
            ("Couldn't find name attribute from " + aFilename);
        }
        if (vendor == null)
        {
            throw new IOException
            ("Couldn't find vendor attribute from " + aFilename);
        }
        return new SuiteInfo(name.getValue(), vendor.getValue());
    }

    /**
     * Returns true if application identified with name and vendor
     * in given SuiteInfo exist in JavaStorage and disk and is registered
     * to ApplicationRegistrator.
     */
    public boolean exists(SuiteInfo aSuiteInfo)
    {
        boolean exists = existsInStorage(aSuiteInfo);
        if (exists)
        {
            exists = existsInDisk(aSuiteInfo);
        }
        if (exists && Platform.isS60())
        {
            // ApplicationRegistrator is only implemented for S60 platform.
            exists = existsInAppReg(aSuiteInfo);
        }
        return exists;
    }

    /**
     * Returns true if application identified with name and vendor
     * in given SuiteInfo does not exist in JavaStorage and disk and is
     * not registered to ApplicationRegistrator.
     * This method should be called only after SuiteInfo has been
     * initialized to contain jad/jar paths and ApplicationInfos.
     */
    public boolean doesNotExist(SuiteInfo aSuiteInfo)
    {
        boolean result = !existsInStorage(aSuiteInfo);
        if (result && aSuiteInfo.getJarPath() != null)
        {
            result = !existsInDisk(aSuiteInfo);
        }
        if (result && Platform.isS60())
        {
            // ApplicationRegistrator is only implemented for S60 platform.
            result = !existsInAppReg(aSuiteInfo);
        }
        return result;
    }

    /**
     * Returns true if application identified with name and vendor
     * in given SuiteInfo exist in JavaStorage. If application exists
     * this method fills SuiteInfo with data from storage.
     */
    public static boolean existsInStorage(SuiteInfo aSuiteInfo)
    {
        if (aSuiteInfo == null)
        {
            return false;
        }
        boolean exists = new StorageHandler().readSuiteInfo(aSuiteInfo);
        if (exists)
        {
            Log.logOut("InstallerTestUtils.existsInStorage=true: " +
                       aSuiteInfo.getName() + ", " + aSuiteInfo.getVendor());
        }
        else
        {
            Log.logOut("InstallerTestUtils.existsInStorage=false: " +
                       aSuiteInfo.getName() + ", " + aSuiteInfo.getVendor());
        }
        return exists;
    }

    /**
     * Returns true if application jar and jar specified in given
     * SuiteInfo exist in disk.
     */
    public static boolean existsInDisk(SuiteInfo aSuiteInfo)
    {
        boolean exists = true;
        if (aSuiteInfo != null)
        {
            if (aSuiteInfo.getJadPath() != null)
            {
                exists = existsInDisk(aSuiteInfo.getJadPath());
            }
            if (exists)
            {
                exists = existsInDisk(aSuiteInfo.getJarPath());
            }
        }
        return exists;
    }

    /**
     * Returns true if file with given path exists in disk.
     */
    public static boolean existsInDisk(String aPath)
    {
        if (aPath != null)
        {
            if (!FileUtils.exists(aPath))
            {
                Log.logOut("InstallerTestUtils.existsInDisk=false: " + aPath);
                return false;
            }
        }
        Log.logOut("InstallerTestUtils.existsInDisk=true: " + aPath);
        return true;
    }

    /**
     * Returns true if application suite is registered
     * to ApplicationRegistrator.
     */
    public static boolean existsInAppReg(SuiteInfo aSuiteInfo)
    {
        if (aSuiteInfo != null)
        {
            Uid[] appUids = aSuiteInfo.getApplicationUids();
            if (appUids != null)
            {
                for (int i = 0; i < appUids.length; i++)
                {
                    if (!iApplicationRegistrator.uidInUse(appUids[i]))
                    {
                        Log.logOut("InstallerTestUtils.existsInAppReg=false: " +
                                   appUids[i]);
                        return false;
                    }
                }
            }
        }
        return true;
    }

    /**
     * Returns true if given exception is an instance of
     * InstallerExceptionBase and exception reason equals
     * to given error codes.
     */
    public static boolean isExceptionReason(Exception aException,
                                            int aShortMsgId,
                                            int aDetailedMsgId,
                                            int aOtaStatusCode)
    {
        if (aException instanceof InstallerExceptionBase)
        {
            InstallerExceptionBase ieb = (InstallerExceptionBase)aException;
            return ieb.equalReason(new InstallerException(aShortMsgId, null,
                                   aDetailedMsgId, null,
                                   aOtaStatusCode));
        }
        return false;
    }
}
