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

import com.nokia.mj.impl.utils.Logger;

/**
 * FileLogger is used by FileConnection API implementation to log all messages.
 * Contains various levels of logging and option to log all messages to standard
 * out if needed.
 */
final class FileLogger
{
    /**
     * Component ID of Common Streams implementation in Logger. Change the
     * component ID to the the component you wish to log to.
     */
    private static final int Component_Id = Logger.EJavaFile;

    /**
     * Writes info log messages
     *
     * @param aMsg
     *            log message
     */
    public static void Log(String aMsg)
    {
        if (Logger.Activated[Component_Id])
        {
            Logger.LOG(Component_Id, Logger.EInfo, aMsg);
        }
    }

    /**
     * Writes warning log messages
     *
     * @param aMsg
     *            log message
     */
    public static void WLog(String aMsg)
    {
        Logger.WLOG(Component_Id, aMsg);
    }

    /**
     * Writes error log messages
     *
     * @param aMsg
     *            log message
     */
    public static void ELog(String aMsg)
    {
        Logger.ELOG(Component_Id, aMsg);
    }
}
