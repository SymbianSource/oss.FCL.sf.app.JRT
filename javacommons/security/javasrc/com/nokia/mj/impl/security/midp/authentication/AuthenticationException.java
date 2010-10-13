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

package com.nokia.mj.impl.security.midp.authentication;

/**
 * Internal exception used to signal errors from JNI when doing the
 * authentication
 *
 * @author Nokia Corporation
 * @version 1.0
 */
public class AuthenticationException extends RuntimeException
{
    // error codes
    public static final int CONNECTION_TO_CAPTAIN_FAILED = 1;
    public static final int SENDING_MSG_TO_CAPTAIN_FAILED = 2;
    public static final int CERT_VERIFICATION_FAILED = 3;
    public static final int SIG_VERIFICATION_FAILED = 4;
    public static final int MISSING_ROOT = 5;
    public static final int MISSING_DOMAIN_MAPPING = 6;
    public static final int CERT_NOT_YET_VALID = 7;
    public static final int CERT_EXPIRED = 8;
    public static final int ROOT_CERT_IN_CHAIN = 9;
    public static final int UNKNOWN_EXT_KEY_USAGE = 10;

    // the error code encapsulated in this exception
    private int errorCode;

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Constructor
     *
     * @param errorcode One of the error codes defined by this class
     */
    public AuthenticationException(int errorCode)
    {
        this.errorCode = errorCode;
    }

    /**
     * Retrieves the internal error code associated with this exception
     *
     * @return The internal error code associated with this exception
     */
    public int getErrorCode()
    {
        return errorCode;
    }

    /*** ---------------------------- PROTECTED --------------------------- */
    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */
    /*** ----------------------------- NATIVE ----------------------------- */
}
