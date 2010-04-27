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

package com.nokia.mj.impl.rt.midp;

import com.nokia.mj.impl.utils.Logger;

/**
 * A utility class for logging.
 *
 * @author Nokia Corporation
 * @version $Rev$
 */


public final class Log
{

    /** Logger component id */
    private static final int COMPONENT_ID = Logger.EJavaRuntime;

    public static final boolean mOn = false;

    /**
     * Writes info log message.
     * @param msg log message
     */
    public static void logI(String msg)
    {
        Logger.ILOG(COMPONENT_ID, msg);
    }

    /**
     * Writes infoPrd log message.
     * @param msg log message
     */
    public static void logP(String msg)
    {
        Logger.PLOG(COMPONENT_ID, msg);
    }

    /**
     * Writes warning log message.
     * @param msg log message
     */
    public static void logW(String msg)
    {
        Logger.WLOG(COMPONENT_ID, msg);
    }

    /**
     * Writes warning log message.
     * @param msg log message
     * @param t Throwable to be logged
     */
    public static void logW(String msg, Throwable t)
    {
        if (t == null)
        {
            Logger.WLOG(COMPONENT_ID, msg);
        }
        else
        {
            Logger.WLOG(COMPONENT_ID, msg, t);
        }
    }

    /**
     * Writes error log message.
     * @param msg log message
     */
    public static void logE(String msg)
    {
        Logger.ELOG(COMPONENT_ID, msg);
    }

    /**
     * Writes error log message.
     * @param msg log message
     * @param t Throwable to be logged
     */
    public static void logE(String msg, Throwable t)
    {
        if (t == null)
        {
            Logger.ELOG(COMPONENT_ID, msg);
        }
        else
        {
            Logger.ELOG(COMPONENT_ID, msg, t);
        }
    }
}
