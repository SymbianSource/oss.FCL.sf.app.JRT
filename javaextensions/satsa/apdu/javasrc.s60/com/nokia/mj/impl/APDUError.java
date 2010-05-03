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
* Description:  Provides apdu specific error codes
 *
*/


package com.nokia.mj.impl.satsa;

import java.io.IOException;
import com.nokia.mj.impl.utils.OsErrorMessage;
import com.nokia.mj.impl.satsa.SecurityError;

import java.io.InterruptedIOException;
import javax.microedition.io.ConnectionNotFoundException;

/**
 * Native error codes which causes certain Java exception to be thrown. For
 * example ERR_ILLEGAL_ARGUMENT states that IllegalArgumentException must be
 * thrown. Values starts from -3001 to avoid confusing with other error codes.
 * In native side these error codes are defined in STSApduConstants.h file.
 */
public final class APDUError
{
    // CONSTANTS

    public static final int ERR_ILLEGAL_ARGUMENT = -3100;

    public static final int ERR_INTERRUPTED_IO = -3200;

    public static final int ERR_CONNECTION_NOT_FOUND = -3300;

    public static final int ERR_SECURITY = -3400;

    public static final int ERR_IO = -3500;

    public static final int ERR_CANCEL = -3600;

    public static final String ERR_TXT_NO_SPECIFIC_TXT = "General error";

    // illegal argument related detailed errors
    // public static final int ERR_IA_PARAM_NULL = -11;
    public static final String TXT_IA_PARAM_NULL = "Parameter is null";

    public static final int ERR_IA_SELECT_APP = -12;

    public static final String TXT_IA_SELECT_APP = "Card application selection APDU not allowed";

    public static final int ERR_IA_MANAGE_CHANNEL = -13;

    public static final String TXT_IA_MANAGE_CHANNEL = "MANAGE CHANNEL APDU not allowed";

    public static final int ERR_IA_OTHER_CLA = -14;

    public static final String TXT_IA_OTHER_CLA = "CLA byte must be 0x0X, 0x8X, 0x9X or 0xAX if Channel is non-zero";

    public static final int ERR_IA_MALFORMED_APDU = -15;

    public static final String TXT_IA_MALFORMED_APDU = "Parameter contains a malformed APDU";

    public static final int ERR_IA_INVALID_PARAMETER = -16;

    public static final String TXT_IA_INVALID_PARAMETER = "Parameter is invalid";

    public static final int ERR_IA_NOT_AN_ENVELOPE = -17;

    public static final String TXT_IA_NOT_AN_ENVELOPE = "Not an ENVELOPE APDU";

    // Interrupted IO related detailed errors
    public static final int ERR_II_CON_CLOSED_DURING = -21;

    public static final String TXT_II_CON_CLOSED_DURING = "Connection closed during exchange operation";

    public static final int ERR_II_CARD_REMOVED = -22;

    public static final String TXT_II_CARD_REMOVED = "Card was removed";

    public static final int ERR_II_BT_SAP_IN_USE = -23;

    public static final String TXT_II_BT_SAP_IN_USE = "BT SAP is in use";

    // Connection not found related detailed errors
    public static final int ERR_CNT_NO_SLOT = -31;

    public static final String TXT_CNT_NO_SLOT = "Card slot does not exist";

    public static final int ERR_CNT_NO_CARD = -32;

    public static final String TXT_CNT_NO_CARD = "Card is not inserted or powered on";

    public static final int ERR_CNT_NO_APPLICATION = -33;

    public static final String TXT_CNT_NO_APPLICATION = "Card application does not exist or refused selection";

    public static final int ERR_CNT_BT_SAP_IN_USE = -34;

    public static final String TXT_CNT_BT_SAP_IN_USE = "BT Sap is in use";

    // Security related detailed errors
    public static final int ERR_SEC_NO_RIGHTS_PIN = -41;

    public static final String TXT_SEC_NO_RIGHTS_PIN = "No rights to use PIN method";

    public static final int ERR_SEC_ILLEGAL_APDU = -42;

    public static final String TXT_SEC_ILLEGAL_APDU = "Sending to the card application not allowed";

    public static final int ERR_SEC_ACCESS_NOT_ALLOWED = -43;

    public static final String TXT_SEC_ACCESS_NOT_ALLOWED = "Accessing the application not allowed";

    public static final int ERR_SEC_ILLEGAL_FILE_ID = -44;

    public static final String TXT_SEC_ILLEGAL_FILE_ID = "Illegal AID";

    // IO related detailed errors
    public static final int ERR_IO_CON_CLOSED_BEFORE = -51;

    public static final String TXT_IO_CON_CLOSED_BEFORE = "Connection was closed before this method was called";

    public static final int ERR_IO_COMM_PROBLEMS = -52;

    public static final String TXT_IO_COMM_PROBLEMS = "General communication error";

    public static final int ERR_IO_ERROR_IO = -53;

    public static final String TXT_IO_ERROR_IO = "I/O error";

    public static final int ERR_IO_NO_FREE_CHANNELS = -54;

    public static final String TXT_IO_NO_FREE_CHANNELS = "No free channels or other I/O error";

    public static String[] iIllegalArgumentTexts = new String[]
    {
        TXT_IA_PARAM_NULL, TXT_IA_SELECT_APP, TXT_IA_MANAGE_CHANNEL,
        TXT_IA_OTHER_CLA, TXT_IA_MALFORMED_APDU, TXT_IA_INVALID_PARAMETER,
        TXT_IA_NOT_AN_ENVELOPE
    };

    public static String[] iInterruptedIOTexts = new String[]
    {
        TXT_II_CON_CLOSED_DURING, TXT_II_CARD_REMOVED, TXT_II_BT_SAP_IN_USE
    };

    public static String[] iConnectionNotFoundTexts = new String[]
    {
        TXT_CNT_NO_SLOT, TXT_CNT_NO_CARD, TXT_CNT_NO_APPLICATION,
        TXT_CNT_BT_SAP_IN_USE
    };

    public static String[] iSecurityTexts = new String[]
    {
        TXT_SEC_NO_RIGHTS_PIN, TXT_SEC_ILLEGAL_APDU,
        TXT_SEC_ACCESS_NOT_ALLOWED, TXT_SEC_ILLEGAL_FILE_ID
    };

    public static String[] iIOTexts = new String[] { TXT_IO_CON_CLOSED_BEFORE,
            TXT_IO_COMM_PROBLEMS, TXT_IO_ERROR_IO, TXT_IO_NO_FREE_CHANNELS
                                                   };

    /**
     * Do not allow construction.
     */
    private APDUError()
    {
    }

    /**
     * Checks needed error codes and throws correct exception. Out of memory
     * error is checked in all cases.
     */
    public static int check(int aError, boolean aCheckIllegalArg,
                            boolean aCheckSecurity, boolean aCheckConnNotFound,
                            boolean aCheckInterruptedIO, boolean aIOasADefault)
    throws IOException
    {
        if (aError < SecurityError.SUCCESS)
        {
            if (aError == SecurityError.ERR_NO_MEMORY)
            {
                throw new OutOfMemoryError();
            }

            if ((aCheckIllegalArg)
                    && (belongsToErrorCategory(aError, ERR_ILLEGAL_ARGUMENT)))
            {
                throw new IllegalArgumentException(getExceptionText(aError,
                                                   ERR_ILLEGAL_ARGUMENT, iIllegalArgumentTexts));
            }
            else if ((aCheckSecurity)
                     && (belongsToErrorCategory(aError, ERR_SECURITY)))
            {
                throw new SecurityException(getExceptionText(aError,
                                            ERR_SECURITY, iSecurityTexts));
            }
            else if ((aCheckConnNotFound)
                     && (belongsToErrorCategory(aError, ERR_CONNECTION_NOT_FOUND)))
            {
                throw new ConnectionNotFoundException(getExceptionText(aError,
                                                      ERR_CONNECTION_NOT_FOUND, iConnectionNotFoundTexts));
            }
            else if ((aCheckInterruptedIO)
                     && (belongsToErrorCategory(aError, ERR_INTERRUPTED_IO)))
            {
                throw new InterruptedIOException(getExceptionText(aError,
                                                 ERR_INTERRUPTED_IO, iInterruptedIOTexts));
            }
            else if (aIOasADefault)
            {
                // String errorTxt = NativeError.errorMessage( aError );
                String errorTxt = OsErrorMessage.getErrorMessage(aError);
                if (belongsToErrorCategory(aError, ERR_IO))
                {
                    errorTxt = getExceptionText(aError, ERR_IO, iIOTexts);
                }
                throw new IOException(errorTxt);
            }
        }
        return aError;
    }

    /**
     * Checks that does error code belong to gived error category.
     */
    private static boolean belongsToErrorCategory(int aError, int aErrorCategory)
    {
        // hundreds are cut
        return ((aError / 100) == (aErrorCategory / 100));
    }

    /**
     * Finds out detailed error message if it exists.
     */
    private static String getExceptionText(int aError, int aErrorCategory,
                                           String[] aErrTxtArray)
    {
        String exceptionText = ERR_TXT_NO_SPECIFIC_TXT;
        int detailedErrorValue = Math.abs(aError - aErrorCategory);
        int decas = detailedErrorValue / 10;// decimals are cut
        int decasFull = decas * 10;// only Full deca without smaller values
        int index = detailedErrorValue - decasFull;

        if ((index != 0) && (aErrTxtArray.length >= index))
        {
            exceptionText = aErrTxtArray[index - 1];
        }
        return exceptionText;
    }
}