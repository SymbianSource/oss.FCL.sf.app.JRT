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
import java.lang.RuntimeException;

import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.rt.support.ApplicationInfo;

/**
 * Provides methods to retrieve information that are platform specific. These
 * would include list of roots present and their localized names based on the
 * needs of the platform.
 */
public final class FileSystemUtils
{
    private static String iAppDomain;

    private static ApplicationInfo iAppInfo;

    static
    {
        try
        {
            Jvm.loadSystemLibrary("javafile");
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.EJavaFile, "Unable to load javafile dll");
        }

        // Load application information at startup
        iAppInfo = ApplicationInfo.getInstance();

        // Load application domain
        iAppDomain = iAppInfo.getProtectionDomain();
    }

    public static String getAppPrivateDir()
    {
        String appPrivateDir = iAppInfo.getRootPath();
        appPrivateDir = appPrivateDir.replace('\\', '/');
        appPrivateDir += "scratch/";
        return appPrivateDir;
    }

    public static String getProtectionDomain()
    {
        return iAppDomain;
    }

    public static String getDefaultRoot()
    {
        return _listRoots();
    }

    public static Vector listRoots()
    {
        String rootList = _listRoots();
        Vector roots = new Vector();
        roots.addElement(rootList + "/");
        return roots;
    }

    public static Vector getRootNames()
    {
        Vector roots = new Vector();
        roots.addElement("Internal/");
        return roots;
    }

    public static String getPathOfProperty(int aValue)
    {
        String retValue = null;
        Vector root = listRoots();

        switch (aValue)
        {
        case FileConstants.GRAPHICS_PROPERTY:
            retValue = "file:///" + root.elementAt(0) + "Images/";
            break;
        case FileConstants.MUSIC_PROPERTY:
            retValue = "file:///" + root.elementAt(0) + "Sounds/";
            break;
        case FileConstants.PHOTOS_PROPERTY:
            retValue = "file:///" + root.elementAt(0) + "Images/";
            break;
        case FileConstants.RECORDINGS_PROPERTY:
            retValue = "file:///" + root.elementAt(0) + "Sounds/";
            break;
        case FileConstants.TONES_PROPERTY:
            retValue = "file:///" + root.elementAt(0) + "Sounds/";
            break;
        case FileConstants.VIDEOS_PROPERTY:
            retValue = "file:///" + root.elementAt(0) + "Videos/";
            break;
        case FileConstants.MEMORY_CARD_PROPERTY:
            throw new RuntimeException(
                "fileconn.dir.memorycard is not support in linux");
        default:
            retValue = null;
        }
        return retValue;
    }

    public static String getLocalizedName(int aValue)
    {
        return "";
    }

    private static native String _listRoots();
}
