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


package com.nokia.mj.impl.installer.utils;

import com.nokia.mj.impl.storage.StorageException;
import com.nokia.mj.impl.utils.exception.InstallerExceptionBase;
import com.nokia.mj.impl.utils.Logger;

public class Log
{
    /** Logger component id */
    private static final int COMPONENT_ID = Logger.EJavaInstaller;

    /**
     * Writes info log message.
     * @param aMsg log message
     */
    public static void log(String aMsg)
    {
        Logger.ILOG(COMPONENT_ID, aMsg);
    }

    /**
     * Writes info log message.
     * @param aMsg log message
     * @param aThrowable Throwable to be logged
     */
    public static void log(String aMsg, Throwable aThrowable)
    {
        Logger.ILOG(COMPONENT_ID, aMsg);
        if (aThrowable != null)
        {
            Logger.LOG(COMPONENT_ID, Logger.EInfo, "Cause", aThrowable);
            Throwable rootException = getRootException(aThrowable);
            while (rootException != null)
            {
                Logger.LOG(COMPONENT_ID, Logger.EInfo,
                           "Root cause", rootException);
                rootException = getRootException(rootException);
            }
        }
    }

    /**
     * Writes info log in products.
     * @param aMsg log message
     */
    public static void logInfoPrd(String aMsg)
    {
        Logger.PLOG(COMPONENT_ID, aMsg);
    }

    /**
     * Writes warning log message.
     * @param aMsg log message
     */
    public static void logWarning(String aMsg)
    {
        //if (Logger.Activated[COMPONENT_ID]) {
        //    Logger.LOG(COMPONENT_ID, Logger.EWarning, aMsg);
        //}
        Logger.WLOG(COMPONENT_ID, aMsg);
    }

    /**
     * Writes warning log message.
     * @param aMsg log message
     * @param aThrowable Throwable to be logged
     */
    public static void logWarning(String aMsg, Throwable aThrowable)
    {
        //if (!Logger.Activated[COMPONENT_ID]) {
        //    return;
        //}
        Logger.WLOG(COMPONENT_ID, aMsg);
        if (aThrowable != null)
        {
            Logger.WLOG(COMPONENT_ID, "Cause", aThrowable);
            Throwable rootException = getRootException(aThrowable);
            while (rootException != null)
            {
                Logger.WLOG(COMPONENT_ID, "Root cause", rootException);
                rootException = getRootException(rootException);
            }
        }
    }

    /**
     * Writes error log message.
     * @param aMsg log message
     */
    public static void logError(String aMsg)
    {
        //if (Logger.Activated[COMPONENT_ID]) {
        //    Logger.LOG(COMPONENT_ID, Logger.EError, aMsg);
        //}
        Logger.ELOG(COMPONENT_ID, aMsg);
    }

    /**
     * Writes error log message.
     * @param aMsg log message
     * @param aThrowable Throwable to be logged
     */
    public static void logError(String aMsg, Throwable aThrowable)
    {
        //if (!Logger.Activated[COMPONENT_ID]) {
        //    return;
        //}
        Logger.ELOG(COMPONENT_ID, aMsg);
        if (aThrowable != null)
        {
            Logger.ELOG(COMPONENT_ID, "Cause", aThrowable);
            Throwable rootException = getRootException(aThrowable);
            while (rootException != null)
            {
                Logger.ELOG(COMPONENT_ID, "Root cause", rootException);
                rootException = getRootException(rootException);
            }
        }
    }

    /**
     * Writes log message to info log and system output.
     * @param aMsg log message
     */
    public static void logOut(String aMsg)
    {
        System.out.println(aMsg);
        if (Logger.Activated[COMPONENT_ID])
        {
            try
            {
                log(aMsg);
            }
            catch (Throwable t)
            {
                t.printStackTrace();
            }
        }
    }

    /**
     * Writes info about memory usage to info log.
     * @param aMsg log message
     */
    public static void logMemory(String aMsg)
    {
        if (Logger.Activated[COMPONENT_ID])
        {
            StringBuffer msg = new StringBuffer();
            if (aMsg != null)
            {
                msg.append(aMsg).append(": ");
            }
            long total = Runtime.getRuntime().totalMemory();
            long free = Runtime.getRuntime().freeMemory();
            msg.append("total=").append(total);
            msg.append(", free=").append(free);
            msg.append(", used=").append(total-free);
            Logger.LOG(COMPONENT_ID, Logger.EInfo, msg.toString());
        }
    }

    /**
     * Returns root exception for given Throwable.
     * @param aThrowable Throwable for which a root exception is returned
     * @return root exception for given Throwable, or null if no root
     * exception is found
     */
    private static Throwable getRootException(Throwable aThrowable)
    {
        Throwable rootException = null;
        if (aThrowable instanceof InstallerExceptionBase)
        {
            rootException = ((InstallerExceptionBase)aThrowable).getRootException();
        }
        else if (aThrowable instanceof StorageException)
        {
            rootException = ((StorageException)aThrowable).getRootException();
        }
        return rootException;
    }
}
