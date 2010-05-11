/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.mj.impl.file;

import java.util.Vector;
import java.util.Enumeration;
import javax.microedition.io.file.FileSystemListener;

import com.nokia.mj.impl.fileutils.DriveInfo;
import com.nokia.mj.impl.fileutils.DriveListener;
import com.nokia.mj.impl.fileutils.DriveUtilities;
import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.rt.support.Jvm;

public final class FileSystemRegistryImpl implements DriveListener
{
    private static Vector iListeners = new Vector();

    private static Object iListenerLock = new Object();

    private static FileSystemRegistryImpl iDriveListener;

    static
    {
        try
        {
            Jvm.loadSystemLibrary("javafile");
        }
        catch (Exception e)
        {
            FileLogger.ELog("Unable to load javafile native library");
        }
    }

    private FileSystemRegistryImpl()
    {
    }

    /**
     * Please refer JSR 75 for more details.
     */
    public static Enumeration listRoots()
    {
        FileLogger.Log("--> FileSystemRegistry.listRoots()");

        // Access Check
        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        FileInternalPermission perm = new FileInternalPermission("",
                FileAccessHelper.INTENT_READ);
        appUtils.checkPermission(perm);
        String domain = FileSystemUtils.getProtectionDomain();

        // Fetch the roots.
        Vector rootVector = FileSystemUtils.listRoots();
        Vector retValue = new Vector();

        for (int index = 0; index < rootVector.size(); index++)
        {
                retValue.addElement(rootVector.elementAt(index));
        }

        FileLogger.Log("<-- FileSystemRegistry.listRoots()");
        return retValue.elements();
    }

    /**
     * Please refer JSR 75 for more details.
     */
    public static boolean addFileSystemListener(FileSystemListener aListener)
    {
        FileLogger.Log("--> FileSystemregistryImpl.addFileSystemListener()");
        if (aListener == null)
        {
            throw new NullPointerException("Argument passed was null");
        }

        // Access Check
        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        FileInternalPermission perm = new FileInternalPermission("",
                FileAccessHelper.INTENT_READ);
        appUtils.checkPermission(perm);

        boolean regSuccess = true;
        synchronized (iListenerLock)
        {
            iListeners.addElement(aListener);
            // If it is the first time, register with DriveUtilities.
            if (1 == iListeners.size())
            {
                iDriveListener = new FileSystemRegistryImpl();
                regSuccess = DriveUtilities.registerListener(iDriveListener);
            }
        }

        FileLogger.Log("<-- FileSystemregistryImpl.addFileSystemListener() "
                       + " returns: " + regSuccess);
        return regSuccess;
    }

    /**
     * Please refer JSR 75 for more details.
     */
    public static boolean removeFileSystemListener(FileSystemListener aListener)
    {
        FileLogger.Log("--> FileSystemregistryImpl.removeFileSystemListener()");
        if (aListener == null)
        {
            throw new NullPointerException("Argument passed was null");
        }

        boolean unregStatus = true;
        synchronized (iListenerLock)
        {
            iListeners.removeElement(aListener);
            if (0 == iListeners.size())
            {
                unregStatus = DriveUtilities.unregisterListener(iDriveListener);
                iDriveListener = null;
            }
        }

        FileLogger.Log("<-- FileSystemregistryImpl.removeFileSystemListener()"
                       + " returns: " + unregStatus);
        return unregStatus;
    }

    /**
     * Notifies all the Listeners on root change event.
     *
     * @param aStatus
     *            status of the root that has changed
     * @param aRoot
     *            root which has either been added or removed
     */
    static void notifyListeners(final int aStatus, final String aRoot)
    {
        FileLogger.Log("--> FileSystemregistryImpl.notifyListeners()");
        FileSystemListener tmpObj = null;
        for (int index = 0; index < iListeners.size(); ++index)
        {
            tmpObj = (FileSystemListener) iListeners.elementAt(index);
            tmpObj.rootChanged(aStatus, aRoot);
        }
        FileLogger.Log("<-- FileSystemregistryImpl.notifyListeners()");
    }

    /**
     * Callback from DriveUtilities once there is change in drive status.
     */
    public void driveChanged(final int aOperation, final DriveInfo aDriveInfo)
    {
        FileLogger.Log("--> FileSystemregistryImpl.driveChanged()");
        if (aOperation == DriveListener.REMOVABLE_MEDIA_INSERTED)
        {
            FileSystemRegistryImpl.notifyListeners(
                FileSystemListener.ROOT_ADDED, aDriveInfo.iRootPath
                .replace('\\', '/'));
        }

        if (aOperation == DriveListener.REMOVABLE_MEDIA_REMOVED)
        {
            FileSystemRegistryImpl.notifyListeners(
                FileSystemListener.ROOT_REMOVED, aDriveInfo.iRootPath
                .replace('\\', '/'));
        }
        FileLogger.Log("<-- FileSystemregistryImpl.driveChanged()");
    }
}
