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


package com.nokia.mj.impl.bluetooth.utils;

import com.nokia.mj.impl.utils.Logger;

public class DebugLog
{
    private static final int COMPONENT_ID = Logger.EJavaBluetooth;

    public static interface LoggerAppender
    {
        public void appendLog(int level, String message, Throwable throwable);
    }

    public static interface LoggerAppenderExt extends LoggerAppender
    {
        public boolean isLogEnabled(int level);
    }

    /**
     * Writes info log message.
     *
     * @param aMsg
     *            log message
     */
    public static void log(String aMsg)
    {
        if (Logger.Activated[COMPONENT_ID])
        {
            Logger.LOG(COMPONENT_ID, Logger.EInfo, aMsg);
        }
    }

    /**
     * Writes warning log message.
     *
     * @param aMsg
     *            log message
     */
    public static void logWarning(String aMsg)
    {
        Logger.WLOG(COMPONENT_ID, aMsg);
    }

    /**
     * Writes warning log message.
     *
     * @param aMsg
     *            log message
     * @param aThrowable
     *            Throwable to be logged
     */
    public static void logWarning(String aMsg, Throwable aThrowable)
    {
        logThrowable(Logger.EWarning, aMsg, aThrowable);
    }

    /**
     * Writes error log message.
     *
     * @param aMsg
     *            log message
     */
    public static void logError(String aMsg)
    {
        Logger.ELOG(COMPONENT_ID, aMsg);
    }

    /**
     * Writes error log message.
     *
     * @param aMsg
     *            log message
     * @param aThrowable
     *            Throwable to be logged
     */
    public static void logError(String aMsg, Throwable aThrowable)
    {
        logThrowable(Logger.EError, aMsg, aThrowable);
    }

    /**
     * Writes log message and Throwable with given log level. If Throwable
     * contains root exceptions they will be logged also.
     *
     * @param aLogLevel
     *            log level
     * @param aMsg
     *            log message
     * @param aThrowable
     *            Throwable to be logged
     */
    private static void logThrowable(int aLogLevel, String aMsg,
                                     Throwable aThrowable)
    {
        if (!Logger.Activated[COMPONENT_ID])
        {
            return;
        }
        if (aThrowable == null)
        {
            Logger.LOG(COMPONENT_ID, aLogLevel, aMsg);
        }
        else
        {
            Logger.LOG(COMPONENT_ID, aLogLevel, aMsg, aThrowable);
            // Throwable rootException = null;
            // if (aThrowable instanceof InstallerExceptionBase) {
            // rootException =
            // ((InstallerExceptionBase)aThrowable).getRootException();
            // } else if (aThrowable instanceof StorageException) {
            // rootException =
            // ((StorageException)aThrowable).getRootException();
            // }
            // if (rootException != null) {
            // Logger.LOG(COMPONENT_ID, aLogLevel, "Root exception",
            // rootException);
            // }
        }
    }

    // Bluecove Methods
    public static boolean isDebugEnabled()
    {
        return Logger.Activated[COMPONENT_ID];
    }

    public static void setDebugEnabled()
    {
    }

    public static void debug(String message)
    {
        log(message);
    }

    public static void debug(String message, String v)
    {
        log(message + " " + v);
    }

    public static void debug(String message, Throwable t)
    {
        logError(message, t);
    }

    public static void debug(String message, Object obj)
    {
        log(message + " " + obj.toString());
    }

    public static void debug(String message, String v, String v2)
    {
        log(message + " " + v + " " + v2);
    }

    public static void debug(String message, long v)
    {
        log(message + " " + String.valueOf(v));
    }

    public static void debug0x(String message, long v)
    {
        log(message + " 0x" + toHexString(v));
    }

    public static void debug0x(String message, String s, long v)
    {
        log(message + " " + s + " 0x" + toHexString(v));
    }

    public static void debug(String message, boolean v)
    {
        log(message + " " + v);
    }

    public static void debug(String message, byte[] data)
    {
        debug(message, data, 0, (data == null) ? 0 : data.length);
    }

    public static void debug(String message, byte[] data, int off, int len)
    {
        StringBuffer buf = new StringBuffer();
        if (data == null)
        {
            buf.append(" null byte[]");
        }
        else
        {
            buf.append(" [");
            for (int i = off; i < off + len; i++)
            {
                if (i != off)
                {
                    buf.append(", ");
                }
                buf.append((new Byte(data[i])).toString());
            }
            buf.append("]");
            if (len > 4)
            {
                buf.append(" ").append(len);
            }
        }
        log(message + buf.toString());
    }

    public static void debug(String message, int[] data)
    {
        debug(message, data, 0, (data == null) ? 0 : data.length);
    }

    public static void debug(String message, int[] data, int off, int len)
    {
        StringBuffer buf = new StringBuffer();
        if (data == null)
        {
            buf.append(" null int[]");
        }
        else
        {
            buf.append(" [");
            for (int i = off; i < off + len; i++)
            {
                if (i != off)
                {
                    buf.append(", ");
                }
                buf.append(Integer.toString(data[i]));
            }
            buf.append("]");
            if (len > 4)
            {
                buf.append(" ").append(len);
            }
        }
        log(message + buf.toString());
    }

    public static void nativeDebugCallback(String fileName, int lineN,
                                           String message)
    {
        try
        {
            if ((fileName != null) && fileName.startsWith(".\\"))
            {
                fileName = fileName.substring(2);
            }
            DebugLog.debugNative(fileName + ":" + lineN, message);
        }
        catch (Throwable e)
        {
        }
    }

    public static void debugNative(String location, String message)
    {
        log(message + "\n\t  " + location);
    }

    public static void error(String message)
    {
        logError(message);
    }

    public static void error(String message, long v)
    {
        logError(message + " " + v);
    }

    public static void error(String message, String v)
    {
        logError(message + " " + v);
    }

    public static void error(String message, Throwable t)
    {
        logError(message, t);
    }

    public static void fatal(String message)
    {
        logError(message);
    }

    public static void fatal(String message, Throwable t)
    {
        logError(message, t);
    }

    public static void addAppender(LoggerAppender newAppender)
    {
    }

    public static void removeAppender(LoggerAppender newAppender)
    {
    }

    private static String toHexString(long l)
    {
        StringBuffer buf = new StringBuffer();
        String lo = Integer.toHexString((int) l);
        if (l > 0xffffffffl)
        {
            String hi = Integer.toHexString((int)(l >> 32));
            buf.append(hi);
            for (int i = lo.length(); i < 8; i++)
            {
                buf.append('0');
            }
        }
        buf.append(lo);
        return buf.toString();
    }

}
