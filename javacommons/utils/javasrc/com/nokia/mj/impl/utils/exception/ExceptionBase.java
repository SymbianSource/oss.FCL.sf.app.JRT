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

package com.nokia.mj.impl.utils.exception;

import com.nokia.mj.impl.utils.ErrorMessageBase;

/**
 * Base class for exceptions having identifiers for
 * localized messages which can be displayed to user.
 *
 * @author Nokia Corporation
 * @version 1.0
 * @see ErrorMessageBase
 */
public class ExceptionBase extends RuntimeException
{

    // Reference to the exception which caused this exception.
    private Throwable iRootException = null;

    // ErrorMessageBases for fetching short and detailed error messages.
    private ErrorMessageBase iShortMsgBase = null;
    private ErrorMessageBase iDetailedMsgBase = null;
    // Identifiers for short and detailed error messages.
    private int iShortMsgId = 0;
    private int iDetailedMsgId = 0;
    // Parameters for short and detailed error messages.
    private String[] iShortMsgParams = null;
    private String[] iDetailedMsgParams = null;

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Constructor
     *
     * @param aShortMsgBase ErrorMessageBase for short error message
     * @param aShortMsgId id for short error message
     * @param aShortMsgParams parameters for short error message
     * @param aDetailedMsgBase ErrorMessageBase for detailed error message
     * @param aDetailedMsgId id for detailed error message
     * @param aDetailedMsgParams parameters for detailed error message
     */
    public ExceptionBase(ErrorMessageBase aShortMsgBase,
                         int aShortMsgId, String[] aShortMsgParams,
                         ErrorMessageBase aDetailedMsgBase,
                         int aDetailedMsgId, String[] aDetailedMsgParams)
    {
        this(aShortMsgBase, aShortMsgId, aShortMsgParams,
             aDetailedMsgBase, aDetailedMsgId, aDetailedMsgParams, null);
    }

    /**
     * Constructor
     *
     * @param aShortMsgBase ErrorMessageBase for short error message
     * @param aShortMsgId id for short error message
     * @param aShortMsgParams parameters for short error message
     * @param aDetailedMsgBase ErrorMessageBase for detailed error message
     * @param aDetailedMsgId id for detailed error message
     * @param aDetailedMsgParams parameters for detailed error message
     * @param aRootException the exception which caused this exception
     */
    public ExceptionBase(ErrorMessageBase aShortMsgBase,
                         int aShortMsgId, String[] aShortMsgParams,
                         ErrorMessageBase aDetailedMsgBase,
                         int aDetailedMsgId, String[] aDetailedMsgParams,
                         Throwable aRootException)
    {
        super(getLogMsg(aShortMsgBase, aShortMsgId, aShortMsgParams,
                        aDetailedMsgBase, aDetailedMsgId, aDetailedMsgParams));
        iShortMsgBase = aShortMsgBase;
        iShortMsgId = aShortMsgId;
        iShortMsgParams = aShortMsgParams;
        iDetailedMsgBase = aDetailedMsgBase;
        iDetailedMsgId = aDetailedMsgId;
        iDetailedMsgParams = aDetailedMsgParams;
        iRootException = aRootException;
    }

    /**
     * Returns the short error message id for this exception.
     *
     * @return the short error message id for this exception
     */
    public int getShortMessageId()
    {
        return iShortMsgId;
    }

    /**
     * Returns the detailed error message id for this exception.
     *
     * @return the detailed error message id for this exception
     */
    public int getDetailedMessageId()
    {
        return iDetailedMsgId;
    }

    /**
     * Returns the short error message for this exception.
     *
     * @return the short error message for this exception
     */
    public String getShortMessage()
    {
        return iShortMsgBase.get(iShortMsgId, iShortMsgParams);
    }

    /**
     * Returns the detailed error message for this exception.
     *
     * @return the detailed error message for this exception
     */
    public String getDetailedMessage()
    {
        if (iDetailedMsgId == ErrorMessageBase.NO_MSG)
        {
            return null;
        }
        return iDetailedMsgBase.get(iDetailedMsgId, iDetailedMsgParams);
    }

    /**
     * Returns the root exception associated with this exception
     *
     * @return The root exception associated with this exception
     */
    public Throwable getRootException()
    {
        return iRootException;
    }

    /**
     * Indicates whether given object has the same error reason as this one.
     * Comparison is made using short and detailed message ids.
     */
    public boolean equalReason(Object aObj)
    {
        if (!(aObj instanceof ExceptionBase))
        {
            return false;
        }
        ExceptionBase ex = (ExceptionBase)aObj;
        if (this.iShortMsgId == ex.iShortMsgId &&
                this.iDetailedMsgId == ex.iDetailedMsgId)
        {
            return true;
        }
        return false;
    }

    /*** ---------------------------- PROTECTED --------------------------- */
    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */

    /**
     * Constructs localised message that can be used as a message in
     * parent exception.
     */
    private static String getLogMsg
    (ErrorMessageBase aShortMsgBase,
     int aShortMsgId, String[] aShortMsgParams,
     ErrorMessageBase aDetailedMsgBase,
     int aDetailedMsgId, String[] aDetailedMsgParams)
    {
        StringBuffer result = new StringBuffer("\n");
        result.append("ShortMsg: ").append
        (aShortMsgBase.get(aShortMsgId, aShortMsgParams))
        .append("\n");
        if (aDetailedMsgId != ErrorMessageBase.NO_MSG)
        {
            result.append("DetailedMsg: ").append
            (aDetailedMsgBase.get(aDetailedMsgId, aDetailedMsgParams))
            .append("\n");
        }
        return result.toString();
    }

    /**
     * Constructs non-localised message that can be used as a message in
     * parent exception.
     */
    private static String getLogMsg2
    (ErrorMessageBase aShortMsgBase,
     int aShortMsgId, String[] aShortMsgParams,
     ErrorMessageBase aDetailedMsgBase,
     int aDetailedMsgId, String[] aDetailedMsgParams)
    {
        StringBuffer result = new StringBuffer("\n");
        result.append("ShortMsg: ").append(aShortMsgId).append(": ")
        .append(msgParamsToString(aShortMsgParams)).append("\n");
        result.append("DetailedMsg: ").append(aDetailedMsgId).append(": ")
        .append(msgParamsToString(aDetailedMsgParams)).append("\n");
        return result.toString();
    }

    /**
     * Returns string containing given message parameters
     * separated with commas.
     */
    private static String msgParamsToString(String[] aParams)
    {
        if (aParams == null)
        {
            return null;
        }
        StringBuffer result = new StringBuffer();
        for (int i = 0; i < aParams.length; i++)
        {
            result.append(aParams[i]);
            if (i < aParams.length - 1)
            {
                result.append(", ");
            }
        }
        return result.toString();
    }

    /*** ----------------------------- NATIVE ----------------------------- */
}
