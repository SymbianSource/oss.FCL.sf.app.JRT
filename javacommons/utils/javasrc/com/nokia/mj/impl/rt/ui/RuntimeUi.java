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

package com.nokia.mj.impl.rt.ui;

import com.nokia.mj.impl.utils.exception.ExceptionBase;
import com.nokia.mj.impl.utils.Logger;

/**
 * Base class for RuntimeUI implementation.
 */
public class RuntimeUi
{
    /** Id for the log file where log entries are written. */
    protected static int iLogId = Logger.EJavaRuntime;
    /** Identified flag. */
    private boolean iIdentified = false;

    /**
     * Constructor
     */
    protected RuntimeUi()
    {
        log("construct");
    }

    /**
     * Seeks confirmation from the user.
     *
     * @param aAppName     the name of the application on behalf of which the
     *                     confirmation is requested
     * @param aConfirmData the data to be confirmed. Unless the user has
     *                     canceled the confirmation, this data will be filled
     *                     in with user's answer upon return
     * @return             true if the user has answered, false if the user has
     *                     canceled the confirmation
     */
    public boolean confirm(String aAppName, ConfirmData aConfirmData)
    {
        if (aConfirmData != null)
        {
            aConfirmData.setAnswer(aConfirmData.getAnswerSuggestion());
            log("Confirm data on behalf of " + aAppName  + ": \n"
                + aConfirmData.toString());
        }
        return true;
    }

    /**
     * Notifies the user that an error has occurred.
     * This method must return quickly.
     *
     * @param aAppName   the name of the application which generated the error
     *                   situation
     * @param aException exception indicating the error reason
     */
    public void error(String aAppName, ExceptionBase aException)
    {
        log("Error caused by " + aAppName + ": " + aException.toString());
    }

    /**
     * Destroys this RuntimeUi instance.
     * After this method has been called
     * the RuntimeUi instance can no longer be used.
     */
    public void destroy()
    {
        log("destroy");
    }

    /**
     * Set the identified flag.
     */
    protected void setIdentified(boolean aIdentified)
    {
        iIdentified = aIdentified;
    }

    /**
     * Returns true if this UI instance corresponds to an identified
     * application, false otherwise.
     */
    protected boolean isIdentified()
    {
        return iIdentified;
    }

    /**
     * Writes an info log entry to JavaRuntime log.
     *
     * @param aMsg message to be logged
     */
    protected static void log(String aMsg)
    {
        Logger.ILOG(iLogId, "RuntimeUi: " + aMsg);
    }

    /**
     * Writes an error log entry to JavaRuntime log.
     *
     * @param aMsg message to be logged
     * @param aThrowable Throwable to be logged
     */
    protected static void logError(String aMsg, Throwable aThrowable)
    {
        Logger.ELOG(iLogId, "RuntimeUi: " + aMsg, aThrowable);
    }
}
