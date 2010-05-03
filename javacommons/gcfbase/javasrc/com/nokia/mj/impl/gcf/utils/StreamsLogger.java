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


package com.nokia.mj.impl.gcf.utils;

import com.nokia.mj.impl.utils.Logger;

/**
 * Streamslogger is used by Common Streams implementation to log all messages.
 * Contains various levels of logging. Needed because if there might be timing
 * issues if too many logs are printed
 */
public class StreamsLogger
{
    /**
     * Component ID of Common Streams implementation in Logger.
     * Change the component ID to the the component you wish to log to.
     */
    private static final int Component_Id = Logger.EGcf;

    // -----------------------------------------------------
    // Different logging levels within Log
    // -----------------------------------------------------
    public static final int Level_1 = 1; //High Priority
    public static final int Level_2 = 2; //Medium Priority
    public static final int Level_3 = 3; //Low Priority

    /**
     * CurrentLoggingLevel controls the amount of info logs that are logged.
     * Current logging level can be changed during build time.
     * Setting this to higher level prints logs of lower levels as well.
     * Lowest level logs should be of highest priority.
     */
    private static final int CurrentLoggingLevel = Level_3;

    /**
     * Writes info log messages
     * @param aMsg log message
     */
    public static void Log(int aLevel, String aMsg)
    {
        if (aLevel > CurrentLoggingLevel)
            return;

        if (Logger.Activated[Component_Id])
        {
            Logger.LOG(Component_Id, Logger.EInfo, aMsg);
        }
    }

    /**
     * Writes warning log messages
     * @param aMsg log message
     */
    public static void WLog(String aMsg)
    {
        Logger.WLOG(Component_Id, aMsg);
    }

    /**
     * Writes error log messages
     * @param aMsg log message
     */
    public static void ELog(String aMsg)
    {
        Logger.ELOG(Component_Id, aMsg);
    }
}