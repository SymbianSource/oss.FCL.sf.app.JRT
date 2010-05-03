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
* Description:  Landmark store manager for handling landmark stores
 *
*/


package com.nokia.mj.impl.location;

import java.io.IOException;

/**
 * A utility class for declaring and handling native SymbianOS error codes. All
 * error codes that are used in Java from native code should be declared here
 * and referenced from this class.
 */
public final class NativeError
{
    public static final String NATIVE_ERROR_MESSAGE = "OS error = ";

    // SymbianOS error codes
    public static final int KErrNone = 0;

    public static final int KErrNotFound = -1;

    public static final int KErrGeneral = -2;

    public static final int KErrCancel = -3;

    public static final int KErrNoMemory = -4;

    public static final int KErrNotSupported = -5;

    public static final int KErrArgument = -6;

    public static final int KErrOverflow = -9;

    public static final int KErrAlreadyExists = -11;

    public static final int KErrPathNotFound = -12;

    public static final int KErrDied = -13;

    public static final int KErrNotReady = -18;

    public static final int KErrCorrupt = -20;

    public static final int KErrAccessDenied = -21;

    public static final int KErrWrite = -23;

    public static final int KErrEof = -25;

    public static final int KErrDiskFull = -26;

    public static final int KErrBadName = -28;

    public static final int KErrCommsLineFail = -29;

    public static final int KErrTimedOut = -33;

    public static final int KErrDisconnected = -36;

    public static final int KErrTooBig = -40;

    public static final int KErrDivideByZero = -41;

    public static final int KErrHardwareNotAvailable = -44;

    // Not intended to be constructed
    private NativeError()
    {
    }

    /**
     * Checks for basic native error codes that map to standard Java exceptions
     * and throws the exception if the error code matches. Otherwise throws an
     * IOException.
     *
     * @param aError
     *            Possible error code.
     * @return Value passed in is returned if not an error.
     */
    public static int checkIO(int aError,
                              String aErrorMessage) throws IOException
    {
        if (aError < KErrNone)
        {
            switch (aError)
            {
            case KErrNoMemory:
                throw new OutOfMemoryError(aErrorMessage +
                                           Integer.toString(aError));
                // KErrArgument must throw IllegalArgumentException
                // otherwise lcdui will break, so don't change this.
            case KErrArgument:
                throw new IllegalArgumentException(aErrorMessage +
                                                   Integer.toString(aError));
            case KErrDivideByZero:
                throw new ArithmeticException(aErrorMessage +
                                              Integer.toString(aError));
            default:
                throw new IOException(aErrorMessage + errorMessage(aError));
            }
        }
        return aError;
    }

    /**
     * Checks for basic native error codes that map to standard Java exceptions
     * and throws the exception if the error code matches. Otherwise throws
     * basic Error class.
     *
     * @param aError
     *            Possible error code.
     * @param aThrowAlways
     *            Determines whether a default exception is thrown if the error
     *            code is not recognised.
     * @return Value passed in is returned if not an error.
     * @throws Error
     *             If the error code does not match any exception thrown in
     *             checkExplicitOnly, a default exception is thrown here.
     */
    public static int check(int aError, String aErrorMessage)
    {
        if (aError < KErrNone)
        {
            checkExplicitOnly(aError, aErrorMessage);
            throw new Error(aErrorMessage + errorMessage(aError));
        }
        return aError;
    }

    /**
     * Checks for basic native error codes that map to standard Java exceptions
     * and throws the exception if the error code matches. Otherwise just
     * returns the error.
     *
     * @param aError
     *            Possible error code.
     * @return Value passed in is returned if not an error.
     * @throws OutOfMemoryError
     *             If aError equals KErrNoMemory.
     * @throws IllegalArgumentException
     *             If aError equals KErrArgument
     * @throws ArithmeticException
     *             If aError equals KErrDivideByZero
     */
    public static int checkExplicitOnly(int aError, String aErrorMessage)
    {
        if (aError < KErrNone)
        {
            switch (aError)
            {
            case KErrNoMemory:
                throw new OutOfMemoryError(aErrorMessage +
                                           Integer.toString(aError));
                // KErrArgument must throw IllegalArgumentException
                // otherwise lcdui will break, so don't change this.
            case KErrArgument:
                throw new IllegalArgumentException(aErrorMessage +
                                                   Integer.toString(aError));
            case KErrDivideByZero:
                throw new ArithmeticException((aErrorMessage +
                                               Integer.toString(aError)));
            default:
                // Do nothing
            }
        }
        return aError;
    }

    /**
     * Returns a string formatted with generic text to indicate where the error
     * code comes from and the error code given.
     *
     * @param A
     *            native error code.
     * @return A string containing the error code.
     */
    public static String errorMessage(int aError)
    {
        String result = NATIVE_ERROR_MESSAGE.concat(Integer.toString(aError));
        return result;
    }

}
