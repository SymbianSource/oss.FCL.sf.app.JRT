/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.fileutils;
import com.nokia.mj.impl.utils.Logger;

public final class DriveUtilities
{
    public static boolean registerListener(DriveListener aListener)
    {
        Logger.LOG(Logger.EJavaFile, Logger.EInfo, "--> DriveUtilities.registerListener()");
        boolean regSuccess = DriveListenerImpl.registerListenerStatic(aListener);
        Logger.LOG(Logger.EJavaFile, Logger.EInfo, "<-- DriveUtilities.registerListener() returns "
                   + regSuccess);
        return regSuccess;
    }

    public static boolean unregisterListener(DriveListener aListener)
    {
        Logger.LOG(Logger.EJavaFile, Logger.EInfo, "--> DriveUtilities.unregisterListener()");
        boolean unregSuccess = DriveListenerImpl.unregisterListenerStatic(aListener);
        Logger.LOG(Logger.EJavaFile, Logger.EInfo, "<-- DriveUtilities.unregisterListener() returns "
                   + unregSuccess);
        return unregSuccess;
    }

    /**
    * Returns the list of all defined drives/roots within the system.
    */
    public static DriveInfo[] getAllDrives()
    {
        Logger.LOG(Logger.EJavaFile, Logger.EInfo, "--> DriveUtilities.getAllDrives()");
        DriveInfo[] drives = DriveObserverMessages.getDrives(true);
        Logger.LOG(Logger.EJavaFile, Logger.EInfo, "<-- DriveUtilities.getAllDrives()");
        return drives;
    }

    /**
    * Returns the list of available drives/roots wich are present and accesible.
    */
    public static DriveInfo[] getAccessibleDrives()
    {
        Logger.LOG(Logger.EJavaFile, Logger.EInfo, "--> DriveUtilities.getAccessibleDrives()");
        DriveInfo[] drives = DriveObserverMessages.getDrives(false);
        Logger.LOG(Logger.EJavaFile, Logger.EInfo, "<-- DriveUtilities.getAccessibleDrives()");
        return drives;
    }
}
