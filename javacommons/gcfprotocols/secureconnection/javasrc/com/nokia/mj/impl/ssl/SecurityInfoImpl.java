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

import com.nokia.mj.impl.gcf.utils.DateUtil;
import com.nokia.mj.impl.utils.Logger;
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
        iCertificate = createCertifcate(aResult);

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


    public Certificate createCertifcate(String[] aResult)
    {
        String iIssuer,iSubject,iSerialNum = null,iVersion;
        String iAlgName,iDateNotAfter,iDateNotBefore;
        String certType = "X.509";
        long iTimeNotAfter,iTimeNotbefore;

        StringBuffer temp = new StringBuffer("");
        StringBuffer t1 = new StringBuffer("");
        int tmpindex = 0;
        String t = aResult[0];
        while (true)
        {
            tmpindex = t.lastIndexOf('/');
            if (tmpindex < 0)
            {
                break;
            }
            temp.append(t.substring(tmpindex + 1, t.length()) + ";");
            t1.append(t.substring(0, tmpindex));
            t = t1.toString();
            t1.setLength(0);
        }
        t = temp.toString();
        iSubject = t.substring(0, t.length() - 1);
        // -------------
        t1.setLength(0);
        t = aResult[1];
        temp.setLength(0);
        while (true)
        {
            tmpindex = t.lastIndexOf('/');
            if (tmpindex < 0)
            {
                break;
            }
            temp.append(t.substring(tmpindex + 1, t.length()) + ";");
            t1.append(t.substring(0, tmpindex));
            t = t1.toString();
            t1.setLength(0);
        }
        t = temp.toString();
        iIssuer = t.substring(0, t.length() - 1);

        // -------------
        temp.setLength(0);
        temp.append(aResult[2]);
        for (int i = 1; i < temp.length(); i += 3)
        {
            temp.insert(i + 1, ":");
        }
        t = temp.toString();
        if (temp.length() >= 2)
        {
            iSerialNum = t.substring(0, temp.length() - 1); // aResult[2];
        }
        else if (temp.length() == 1)
        {
            iSerialNum = "0"+t.substring(0);
        }

        iDateNotBefore = aResult[3];
        iTimeNotbefore = parseString(iDateNotBefore);
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "iTimeNotbefore = "
                   + iTimeNotbefore);
        iDateNotAfter = aResult[4];
        iTimeNotAfter = parseString(iDateNotAfter);
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "iTimeNotAfter = "
                   + iTimeNotAfter);
        iAlgName = aResult[5];
        iVersion = aResult[6];

        return new com.nokia.mj.impl.security.common.Certificate(iIssuer,iSubject,iTimeNotbefore,iTimeNotAfter,iSerialNum,iAlgName,iVersion,certType);

    }

    long parseString(String s)
    {
        String tmp = null;
        long result;
        try
        {
            tmp = s.substring(0, 2);
            int yr = 2000 + Integer.parseInt(tmp);
            tmp = s.substring(2, 4);
            int mnt = Integer.parseInt(tmp);
            tmp = s.substring(4, 6);
            int dy = Integer.parseInt(tmp);
            tmp = s.substring(6, 8);
            int hr = Integer.parseInt(tmp);
            tmp = s.substring(8, 10);
            int mn = Integer.parseInt(tmp);
            tmp = s.substring(10, 12);
            int secnd = Integer.parseInt(tmp);
            result = DateUtil.epocDifference(yr, mnt, dy, hr, mn, secnd);
        }
        catch (Exception e)
        {
            Logger.WLOG(Logger.ESOCKET,
                        "CertificateImpl: parseString() exception: "+e);
            result = Long.MAX_VALUE;
        }
        return result;
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