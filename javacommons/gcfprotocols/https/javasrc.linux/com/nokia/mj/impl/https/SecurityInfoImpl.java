/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.ssl;
import javax.microedition.io.SecurityInfo;
import javax.microedition.pki.Certificate;

/**
 * Security Info Implementation class.
 */

public class SecurityInfoImpl implements SecurityInfo
{
    private Certificate iCertificate;
    private String iCipher;
    private String iProtocolName;
    private String iProtocolVersion;

    public SecurityInfoImpl()
    {
        super();
    }

    public SecurityInfoImpl(String[] aResult)
    {
        iCertificate = new CertificateImpl(aResult);

        if (aResult[7].indexOf("TLS") >= 0)
            iProtocolName = "TLS";
        else
            iProtocolName = "SSL";

        if (aResult[7].compareTo("TLSv1") == 0)
            iProtocolVersion = "3.1";
        else
            iProtocolVersion = "3.0";

        if (aResult[8].indexOf("MD5") >= 0)
        {
            iCipher = new String("TLS_RSA_WITH_RC4_128_MD5");
        }
        else
        {
            iCipher = new String("TLS_RSA_WITH_RC4_128_SHA");
        }

    }

    /**
     * Gets the handle to the servercertificate
     *
     * @returns a Certificate class
     */

    public Certificate getServerCertificate()
    {
        return iCertificate;
    }

    /**
     * Gets the SSL protocol version
     *
     * @returns protocolversion
     */

    public String getProtocolVersion()
    {
        return iProtocolVersion;
    }

    /**
     * Gets the Protocol Name
     *
     * @returns protocol name
     */

    public String getProtocolName()
    {
        return iProtocolName;
    }

    /**
     * Gets the name of the cipher suite in use
     *
     * @returns cipher suite name
     */

    public String getCipherSuite()
    {
        return iCipher;
    }
}