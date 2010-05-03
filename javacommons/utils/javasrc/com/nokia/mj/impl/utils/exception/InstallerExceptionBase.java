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

package com.nokia.mj.impl.utils.exception;

import com.nokia.mj.impl.utils.ErrorMessageBase;
import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;
import com.nokia.mj.impl.utils.OtaStatusCode;

/**
 * Base class for JavaInstaller exceptions having identifiers for
 * localized messages which can be displayed to user.
 *
 * @author Nokia Corporation
 * @version 1.0
 * @see ErrorMessageBase
 */
public class InstallerExceptionBase extends ExceptionBase
{

    // OTA status code.
    private int iOtaStatusCode = 0;

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
     * @param aOtaStatusCode OTA status code
     */
    public InstallerExceptionBase(ErrorMessageBase aShortMsgBase,
                                  int aShortMsgId, String[] aShortMsgParams,
                                  ErrorMessageBase aDetailedMsgBase,
                                  int aDetailedMsgId, String[] aDetailedMsgParams,
                                  int aOtaStatusCode)
    {
        this(aShortMsgBase, aShortMsgId, aShortMsgParams,
             aDetailedMsgBase, aDetailedMsgId, aDetailedMsgParams,
             aOtaStatusCode, null);
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
     * @param aOtaStatusCode OTA status code
     * @param aRootException the exception which caused this exception
     */
    public InstallerExceptionBase(ErrorMessageBase aShortMsgBase,
                                  int aShortMsgId, String[] aShortMsgParams,
                                  ErrorMessageBase aDetailedMsgBase,
                                  int aDetailedMsgId, String[] aDetailedMsgParams,
                                  int aOtaStatusCode, Throwable aRootException)
    {
        super(aShortMsgBase, aShortMsgId, aShortMsgParams,
              aDetailedMsgBase, aDetailedMsgId, aDetailedMsgParams,
              aRootException);
        iOtaStatusCode = aOtaStatusCode;
    }

    /**
     * Returns the OTA status code for this exception.
     *
     * @return the OTA status code for this exception
     */
    public int getOtaStatusCode()
    {
        return iOtaStatusCode;
    }

    /**
     * Indicates whether given object has the same error reason as this one.
     * Comparison is made using short and detailed message ids and
     * OTA status code.
     */
    public boolean equalReason(Object aObj)
    {
        if (!(aObj instanceof InstallerExceptionBase))
        {
            return false;
        }
        InstallerExceptionBase ex = (InstallerExceptionBase)aObj;
        if (super.equalReason(ex) &&
                this.iOtaStatusCode == ex.iOtaStatusCode)
        {
            return true;
        }
        return false;
    }

    /**
     * Returns the string representation of this exception
     *
     * @return a string representation of this exception
     */
    public String toString()
    {
        StringBuffer result = new StringBuffer(super.toString());
        result.append("OtaStatus: ")
        .append(OtaStatusCode
                .getOtaStatusReportBody(iOtaStatusCode)).append("\n");
        return result.toString();
    }

    /*** ---------------------------- PROTECTED --------------------------- */
    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */
    /*** ----------------------------- NATIVE ----------------------------- */
}
