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


package javax.microedition.pki;

import java.io.IOException;

public class CertificateException
        extends IOException
{
    public static final byte BAD_EXTENSIONS = 1;
    public static final byte CERTIFICATE_CHAIN_TOO_LONG = 2;
    public static final byte EXPIRED = 3;
    public static final byte UNAUTHORIZED_INTERMEDIATE_CA = 4;
    public static final byte MISSING_SIGNATURE = 5;
    public static final byte NOT_YET_VALID = 6;
    public static final byte SITENAME_MISMATCH = 7;
    public static final byte UNRECOGNIZED_ISSUER = 8;
    public static final byte UNSUPPORTED_SIGALG = 9;
    public static final byte INAPPROPRIATE_KEY_USAGE = 10;
    public static final byte BROKEN_CHAIN = 11;
    public static final byte ROOT_CA_EXPIRED = 12;
    public static final byte UNSUPPORTED_PUBLIC_KEY_TYPE = 13;
    public static final byte VERIFICATION_FAILED = 14;



    private final Certificate iCert;
    private final byte iReason;



    public CertificateException(Certificate aCert, byte aReason)
    {
        this(null, aCert, aReason);
    }



    public CertificateException(String aMessage, Certificate aCert, byte aReason)
    {
        super(aMessage);
        iCert = aCert;
        switch (aReason)
        {
        case BAD_EXTENSIONS:
        case CERTIFICATE_CHAIN_TOO_LONG:
        case EXPIRED:
        case UNAUTHORIZED_INTERMEDIATE_CA:
        case MISSING_SIGNATURE:
        case NOT_YET_VALID:
        case SITENAME_MISMATCH:
        case UNRECOGNIZED_ISSUER:
        case UNSUPPORTED_SIGALG:
        case INAPPROPRIATE_KEY_USAGE:
        case BROKEN_CHAIN:
        case ROOT_CA_EXPIRED:
        case UNSUPPORTED_PUBLIC_KEY_TYPE:
        case VERIFICATION_FAILED:
            iReason = aReason;
            break;
        default:
            throw new IllegalArgumentException();
        }
    }



    public Certificate getCertificate()
    {
        return iCert;
    }



    public byte getReason()
    {
        return iReason;
    }
}
