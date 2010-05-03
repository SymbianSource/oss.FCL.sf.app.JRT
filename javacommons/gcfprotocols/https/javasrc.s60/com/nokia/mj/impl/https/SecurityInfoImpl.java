/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.https;

import javax.microedition.io.SecurityInfo;
import javax.microedition.pki.Certificate;

public class SecurityInfoImpl implements SecurityInfo
{

    // data constants :

    private static final int DATA_STRING_NUMBER = 9;

    private static final int DATA_LONG_NUMBER = 2;

    /**
     * the following constant is also used by CertificateImpl
     */
    static final int CERTIFICATE_DATA_OFFSET = 3;

    // data containers :

    /**
     * The ownership of the two arrays containing data is shared with
     * CertificateImpl
     */
    protected String[] iStringArray = new String[DATA_STRING_NUMBER];

    protected long[] iLongArray = new long[DATA_LONG_NUMBER];

    // data indexes in arrays:
    private static final int CYPHER_SUITE_NAME = 0;

    private static final int PROTOCOL_NAME = 1;

    private static final int PROTOCOL_VERSION = 2;

    private static final int CERT_SUBJECT = 0;
    private static final int CERT_ISSUER = 1;
    private static final int CERT_TYPE = 2;
    private static final int CERT_VERSION = 3;
    private static final int CERT_SIGALGNAME = 4;
    private static final int CERT_SERIALNUMBER = 5;

    private static final int CERT_NOT_BEFORE = 0;
    private static final int CERT_NOT_AFTER = 1;

    private Certificate iCertificate;

    /**
     * SecurityInfo and corresponding Certificate are created by retrieving,
     * once and for all, all relevant info in one big native call. The native
     * side is tailored to return all data into 2 arrays. - array of String
     * contains in order : cypher suite name, protocol name, protocol version,
     * certificate's subject, certificate's issuer, type of the Certificate,
     * version number of the Certificate, name of the algorithm used to sign the
     * Certificate, serial number of the Certificate. - array of long contains
     * in order : time before which the Certificate may not be used, time after
     * which the Certificate may not be used.
     */
    public SecurityInfoImpl(int aNativeSecureConnectionHandle)
    {
        int ret = _createSecurityInfo(aNativeSecureConnectionHandle,
                                      iStringArray, iLongArray);

        iCertificate = createCertificate(iStringArray, iLongArray);
    }

    public Certificate createCertificate(String[] aStringArray, long[] aLongArray)
    {
        String issuer = aStringArray[CERT_ISSUER + CERTIFICATE_DATA_OFFSET];
        String subject = aStringArray[CERT_SUBJECT + CERTIFICATE_DATA_OFFSET];
        String version = aStringArray[CERT_VERSION + CERTIFICATE_DATA_OFFSET];
        String algName = aStringArray[CERT_SIGALGNAME + CERTIFICATE_DATA_OFFSET];
        String serialNum = aStringArray[CERT_SERIALNUMBER + CERTIFICATE_DATA_OFFSET];
        String certType =  aStringArray[CERT_TYPE + CERTIFICATE_DATA_OFFSET];
        return new com.nokia.mj.impl.security.common.Certificate(issuer,subject,aLongArray[CERT_NOT_BEFORE],aLongArray[CERT_NOT_AFTER],serialNum,algName,version,certType);
    }

    private native int _createSecurityInfo(int aHandle, String[] aStringArray,
                                           long[] aLongArray);

    public Certificate getServerCertificate()
    {
        return iCertificate;
    }

    public String getCipherSuite()
    {
        return iStringArray[CYPHER_SUITE_NAME];
    }

    public String getProtocolName()
    {
        return iStringArray[PROTOCOL_NAME];
    }

    public String getProtocolVersion()
    {
        return iStringArray[PROTOCOL_VERSION];
    }
}
