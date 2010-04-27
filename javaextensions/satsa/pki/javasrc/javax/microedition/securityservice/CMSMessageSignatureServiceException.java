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


package javax.microedition.securityservice;

/**
 * Refer JSR-177 Specification for details
 */
public final class CMSMessageSignatureServiceException
        extends java.lang.Exception
{

    /**
    * Error code returned if a cyptographic error occured.
    */
    public static final byte CRYPTO_FAILURE = 1;

    /**
    * Error code returned if an error occurs when formatting a result.
    */
    public static final byte CRYPTO_FORMAT_ERROR = 2;

    /**
    * Error code returned if a certificate is not available on the
    * device for the selected public key.
    */
    public static final byte CRYPTO_NO_CERTIFICATE = 8;

    /**
    * Error code returned if detached signatures are not supported.
    */
    public static final byte CRYPTO_NO_DETACHED_SIG = 3;

    /**
    * Error code returned if opaque signatures are not supported.
    */
    public static final byte CRYPTO_NO_OPAQUE_SIG = 4;


    /**
    * Error code returned if security element is busy.
    */
    public static final byte SE_BUSY = 5;

    /**
    * Error code returned if a cryptographic operation failed in a
    * security element.
    */
    public static final byte SE_CRYPTO_FAILURE = 7;

    /**
    * Error code returned if an operation involving the security element fails.
    */
    public static final byte SE_FAILURE = 6;

    private byte iReason;

    /**
    * Refer JSR-177 Specification for details
    */
    public CMSMessageSignatureServiceException(byte aCode)
    {
        iReason = aCode;
    } // CMSMessageSignatureServiceException constructor

    /**
    * Refer JSR-177 Specification for details
    */
    public byte getReason()
    {
        return iReason;
    }

} // CMSMessageSignatureServiceException
