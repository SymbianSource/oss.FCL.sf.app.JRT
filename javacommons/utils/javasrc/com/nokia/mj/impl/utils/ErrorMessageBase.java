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

package com.nokia.mj.impl.utils;

import com.nokia.mj.impl.utils.ResourceLoader;

import java.util.Hashtable;

/**
 * Base class for classes defining localized error messages.
 *
 * @author Nokia Corporation
 * @version 1.0
 * @see InstallerExceptionBase
 */
abstract public class ErrorMessageBase
{
    /*** ----------------------------- PUBLIC ------------------------------ */

    public static final int NO_MSG = 0; // Used when the error message is not available.

    /**
     * Method for retrieving the message of a certain error.
     *
     * @param errorCode The error code whose message is queried
     * @param errorMessageParameters The parameters to be filled into the
     * message
     * @return The corresponding message including the provided
     * parameters
     */
    public String get(int errorCode, String[] errorMessageParameters)
    {
        Hashtable messageTable = getMessageTable();
        String msgId = ((String)messageTable.get(new Integer(errorCode)));
        if (msgId == null)
        {
            return this.getClass().getName() + ": No message found for error " + errorCode;
        }
        return getResourceLoader().format(msgId, errorMessageParameters);
    }

    /*** ---------------------------- PROTECTED --------------------------- */

    /**
     * Method for getting message table where key is an Integer value for
     * the error code, and value is an error message String id.
     * This method must be overriden in inheriting class.
     */
    abstract protected Hashtable getMessageTable();

    /**
     * Method for retrieving the ResourceLoader instance that is used
     * to localise error messages.
     * This method must be overriden in inheriting class.
     */
    abstract protected ResourceLoader getResourceLoader();

    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */
    /*** ----------------------------- NATIVE ----------------------------- */
}