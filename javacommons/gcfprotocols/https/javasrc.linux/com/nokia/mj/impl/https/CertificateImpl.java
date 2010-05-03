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

import javax.microedition.pki.Certificate;
import com.nokia.mj.impl.gcf.utils.DateUtil;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.rt.support.Finalizer;

/**
 * Certificate Implementation class.
 *
 */

public class CertificateImpl implements Certificate
{

    private String iIssuer;
    private String iSubject;
    private String iSerialNum;
    private String iVersion;
    private long iTimeNotAfter;
    private long iTimeNotbefore;
    private String iAlgName;
    private String iDateNotAfter;
    private String iDateNotBefore;

    /**
     * Constructs the CertificateImpl.
     */
    public CertificateImpl()
    {
        super();
    }

    /**
     * Constructs the CertificateImpl.
     *
     * @param aResult the certicate information
     */

    public CertificateImpl(String[] aResult)
    {

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
            iSerialNum = t.substring(0, temp.length() - 2); // aResult[2];
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
            e.printStackTrace();
            result = Long.MAX_VALUE;
        }
        return result;
    }

    /**
     * Get the subject field from the certificate
     *
     * @return the Subject field
     */

    public String getSubject()
    {
        return iSubject;
    }

    /**
     * Get the issuer field from the certificate
     *
     * @return the issuer field
     */

    public String getIssuer()
    {
        return iIssuer;
    }

    /**
     * Get the type of the certificate
     *
     * @return the type field
     */

    public String getType()
    {
        String type = "X.509";
        return type;
    }

    /**
     * Get the version of the certificate
     *
     * @return the version field
     */

    public String getVersion()
    {
        return iVersion;
    }

    /**
     * Get the signature algorithm used for the certificate
     *
     * @return the alg name
     */

    public String getSigAlgName()
    {
        return iAlgName;
    }

    /**
     * Gets the time before which this Certificate may not be used from the
     * validity period.
     *
     * @return the time
     */

    public long getNotBefore()
    {
        return iTimeNotbefore;
    }

    /**
     * Gets the time after which this Certificate may not be used from the
     * validity period.
     *
     * @return the time
     */

    public long getNotAfter()
    {
        return iTimeNotAfter;
    }

    /**
     * GGets the printable form of the serial number of this Certificate
     *
     * @return the serial number
     */

    public String getSerialNumber()
    {
        return iSerialNum;
    }

}