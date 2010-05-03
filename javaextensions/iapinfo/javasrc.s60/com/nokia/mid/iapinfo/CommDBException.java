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
* Description:  Exception class
 *
*/

package com.nokia.mid.iapinfo;

import com.nokia.mj.impl.gcf.utils.NativeError;

/**
 * The main Exception class for CommDB API.
 */
class CommDBException extends Exception
{

    protected int errorCode;
    protected int nativeErrorCode;

    /**
     * Default constructor is not allowed.
     */
    protected CommDBException()
    {

    }

    /**
     * Constructs an exception instance with a textual information.
     *
     * @param errorMessage the textual information about the exception
     */
    CommDBException(String errorMessage)
    {
        super(errorMessage);
    }

    /**
     * Constructs an exception instance with a textual information and a Java
     * error code.
     *
     * @param errorCode the numeric error code (see CommsTable class definition)
     */
    CommDBException(String errorMessage, int errorCode)
    {
        super("CommsTable: " + errorMessage);
        this.errorCode = errorCode;
    }

    /**
     * Constructs an exception instance with a textual information a Java and a
     * native error code. At least one error code should be always specified.
     *
     * @param errorCode the numeric error code from Java side (see CommsTable
     *            class definition for error codes)
     *
     * @param nativeErrorCode the numeric error code from native side (see
     *            Symbian documentation for error codes)
     */
    CommDBException(String errorMessage, int errorCode, int nativeErrorCode)
    {
        super("CommsTable: " + errorMessage + " Native Error: "
              + NativeError.errorMessage(nativeErrorCode));
        this.errorCode = errorCode;
        this.nativeErrorCode = nativeErrorCode;
    }

    /**
     * Returns the Java side error code for the current exception.
     *
     * @return the error code
     */
    public int getErrorCode()
    {
        return errorCode;
    }

    /**
     * Returns the native side error code for the current exception. The native
     * error code is not always specified for an exception.
     *
     * @return the native error code
     */
    public int getNativeErrorCode()
    {
        return nativeErrorCode;
    }
}
