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

package com.nokia.mj.impl.security.common;

import java.util.Date;
import java.util.Calendar;

/**
 * MIDP authentication is build around X.509 Public Key Infrastructure so that
 * MIDlet suites are signed using public key certificates.
 * This class encapsulates a certificate info used in MIDP authentication
 */
public class Certificate implements javax.microedition.pki.Certificate
{
    private String issuer = null;
    private String subject = null;
    private String organization = null;
    private String notBefore = null;
    private String notAfter = null;
    private String serialNumber = null;
    private String fingerprint = null;
    private String algName = null;
    private String certType = null;
    private String version = null;
    private long timeNotAfter;
    private long timeNotBefore;

    public Certificate(String issuer, String subject, String organization, String notBefore, String notAfter, String serialNumber, String fingerprint)
    {
        this.issuer = issuer;
        this.subject = subject;
        this.organization = organization;
        this.notBefore = notBefore;
        this.notAfter = notAfter;
        this.serialNumber = serialNumber;
        this.fingerprint = fingerprint;
    }

    public Certificate(String issuer, String subject, long timenotBefore, long timenotAfter, String serialNumber, String algName, String version, String certType)
    {
        this.issuer = issuer;
        this.subject = subject;
        this.notBefore = notBefore;
        this.notAfter = notAfter;
        this.serialNumber = serialNumber;
        this.algName = algName;
        this.timeNotAfter = timenotAfter;
        this.timeNotBefore = timenotBefore;
        this.certType = certType;
        this.version = version;
    }

    public Certificate(Certificate otherCert)
    {
        if (otherCert != null)
        {
            this.issuer = otherCert.getIssuer();
            this.subject = otherCert.getSubject();
            this.organization = otherCert.getOrganization();
            this.notBefore = otherCert.getDateNotBefore();
            this.notAfter = otherCert.getDateNotAfter();
            this.serialNumber = otherCert.getSerialNumber();
            this.fingerprint = otherCert.getFingerprint();
        }
    }

    /**
     * Getter for the issuer of the certificate
     *
     * @return The name of the entity which issued the certificate
     */
    public String getIssuer()
    {
        return issuer;
    }

    /**
     * Getter for the certificate validity's start date
     *
     * @return The start date of the certificate's validity
     */
    public Date getValidFrom()
    {
        return str2Date(notBefore);
    }

    /**
     * Getter for the certificate validity's end date
     *
     * @return The end date of the certificate's validity
     */
    public Date getValidUntil()
    {
        return str2Date(notAfter);
    }

    /**
     * Getter for the serial number of the certificate
     *
     * @return The serial number of the certificate
     */
    public String getSerialNumber()
    {
        return serialNumber;
    }

    /**
     * Getter for the fingerprint of the certificate
     *
     * @return The fingerprint of the certificate
     */
    public String getFingerprint()
    {
        return fingerprint;
    }

    /**
     * Getter for the subject of the certificate
     *
     * @return The name of the entity to which the certificate has been issued
     */
    public String getSubject()
    {
        return subject;
    }

    /**
     * Getter for the organization of the certificate
     *
     * @return The name of the organization to which the certificate has been
     *         issued
     */
    public String getOrganization()
    {
        return organization;
    }

    /*
     * The format of the returned string is YYYYMMDDHHMMSS,
     * where YYYY represents the year,
     *       MM is the month (counting January as 01), and
     *       DD is the day of the month (01 to 31)
     *       HH is the hour (0-24)
     *       MM is the minutes (0-60)
     *       SS is the seconds (0-60)
     */
    public String getDateNotAfter()
    {
        return notAfter;
    }

    /*
     * The format of the returned string is YYYYMMDDHHMMSS,
     * where YYYY represents the year,
     *       MM is the month (counting January as 01), and
     *       DD is the day of the month (01 to 31)
     *       HH is the hour (0-24)
     *       MM is the minutes (0-60)
     *       SS is the seconds (0-60)
     */
    public String getDateNotBefore()
    {
        return notBefore;
    }

    /**
     * Gets the time before which this Certificate may not be used from the
     * validity period.
     *
     * @return the time
     */
    public long getNotBefore()
    {
        return timeNotBefore;
    }

    /**
     * Gets the time after which this Certificate may not be used from the
     * validity period.
     *
     * @return the time
     */
    public long getNotAfter()
    {
        return timeNotAfter;
    }

    /**
     * Get the type of the certificate
     *
     * @return the type field
     */
    public String getType()
    {
        return certType;
    }

    /**
     * Get the version of the certificate
     *
     * @return the version field
     */
    public String getVersion()
    {
        return version;
    }

    /**
     * Get the signature algorithm used for the certificate
     *
     * @return the alg name
     */
    public String getSigAlgName()
    {
        return algName;
    }

    /*
     * Returns the content of the fingerprint split into groups of 4 characters
     * separated by ' '
     *
     * @return The formatted fingerprint
     */
    public String getFormattedFingerprint()
    {
        if (fingerprint == null)
        {
            return null;
        }
        StringBuffer sb = new StringBuffer();
        for (int i=0; i<fingerprint.length(); i++)
        {
            sb.append(fingerprint.charAt(i));
            if ((i+1)%4 == 0)
            {
                // insert the separator
                sb.append(' ');
            }
        }
        return sb.toString();
    }

    /*
     * Formats the issuer according to following syntax:
     * CN", "O", "OU", "C", "ST", "L
     * where CN = common name
     *        O = organization name
     *       OU = organization unit name
     *        C = country name
     *       ST = state or province name
     *        L = locality name
     * e.g. Issuer, Org, Sub-org, FI, Unknown, Tampere
     */
    public String getFormattedIssuer()
    {
        return formatDN(getIssuer());
    }

    /*
     * Formats the subject according to following syntax:
     * CN", "O", "OU", "C", "ST", "L
     * where CN = common name
     *        O = organization name
     *       OU = organization unit name
     *        C = country name
     *       ST = state or province name
     *        L = locality name
     * e.g. subject, Org, Sub-org, FI, Unknown, Tampere
     */
    public String getFormattedSubject()
    {
        return formatDN(getSubject());
    }

    public String toString()
    {
        StringBuffer sb = new StringBuffer();
        sb.append("\nIssuer:").append(getIssuer())
        .append("\nSubject:").append(getSubject())
        .append("\nOrganization:").append(getOrganization())
        .append("\nValidFrom:").append(getDateNotBefore())
        .append("\nValidUntil:").append(getDateNotAfter())
        .append("\nSerialNumber:").append(getSerialNumber())
        .append("\nFingerprint:").append(getFingerprint());
        return sb.toString();
    }

    /*
     * Builds a Date object out of a string represented as YYYYMMDDHHMMSS
     * where YYYY represents the year,
     *       MM is the month (counting January as 01), and
     *       DD is the day of the month (01 to 31)
     *       HH is the hour (0-24)
     *       MM is the minutes (0-60)
     *       SS is the seconds (0-60)
     */
    private Date str2Date(String str)
    {
        if (str != null
                && str.length() == 14)
        {
            try
            {
                Calendar cal = Calendar.getInstance();
                cal.set(Calendar.YEAR, Integer.parseInt(str.substring(0,4)));
                cal.set(Calendar.MONTH, Integer.parseInt(str.substring(4,6)));
                cal.set(Calendar.DAY_OF_WEEK, Integer.parseInt(str.substring(6,8)));
                cal.set(Calendar.HOUR, Integer.parseInt(str.substring(8,10)));
                cal.set(Calendar.MINUTE, Integer.parseInt(str.substring(10,12)));
                cal.set(Calendar.SECOND, Integer.parseInt(str.substring(12,14)));
                return cal.getTime();
            }
            catch (NumberFormatException e)
            {
            }
            catch (ArrayIndexOutOfBoundsException e) {}
        }
        return null;
    }

    /*
     * Formats a distinguished name (DN) according to following syntax:
     * CN", "O", "OU", "C", "ST", "L
     * where CN = common name
     *        O = organization name
     *       OU = organization unit name
     *        C = country name
     *       ST = state or province name
     *        L = locality name
     */
    private String formatDN(String DN)
    {
        if (DN != null)
        {
            StringBuffer buf = new StringBuffer();
            // common name
            String tmp = getToken(DN, "/CN=");
            if (tmp != null)
            {
                buf.append(tmp);
            }
            // organization name
            tmp = getToken(DN, "/O=");
            if (tmp != null)
            {
                if (buf.length() > 0)
                {
                    buf.append(", ");
                }
                buf.append(tmp);
            }
            // organization unit name
            tmp = getToken(DN, "/OU=");
            if (tmp != null)
            {
                if (buf.length() > 0)
                {
                    buf.append(", ");
                }
                buf.append(tmp);
            }
            // country name
            tmp = getToken(DN, "/C=");
            if (tmp != null)
            {
                if (buf.length() > 0)
                {
                    buf.append(", ");
                }
                buf.append(tmp);
            }
            // state or province name
            tmp = getToken(DN, "/ST=");
            if (tmp != null)
            {
                if (buf.length() > 0)
                {
                    buf.append(", ");
                }
                buf.append(tmp);
            }
            // locality name
            tmp = getToken(DN, "/L=");
            if (tmp != null)
            {
                if (buf.length() > 0)
                {
                    buf.append(", ");
                }
                buf.append(tmp);
            }
            return buf.toString();
        }
        return null;
    }

    private String getToken(String str, String sep)
    {
        int startIndex = str.indexOf(sep);
        if (startIndex != -1)
        {
            startIndex += sep.length();
            int endIndex = str.indexOf("/", startIndex);
            if (endIndex == -1)
            {
                endIndex = str.length();
            }
            return str.substring(startIndex, endIndex);
        }
        return null;
    }
}
