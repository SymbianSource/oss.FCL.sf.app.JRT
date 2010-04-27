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


package com.nokia.mj.impl.installer.storagehandler;

import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.utils.OtaStatusCode;
import com.nokia.mj.impl.utils.Uid;

import java.util.Date;

/**
 * OtaStatusNotification contains information of one Ota status
 * notification that needs to be sent.
 *
 * @author Nokia Corporation
 * @version $Rev: 9041 $ $Date: 2009-12-07 18:36:08 +0200 (Mon, 07 Dec 2009) $
 */
public class OtaStatusNotification
{
    // OTA status notification types
    public static final int TYPE_INSTALL = 0;
    public static final int TYPE_UNINSTALL = 1;

    // Maximum number of retries.
    private static final int MAX_RETRY_COUNT = 5;

    private Uid iUid = null; // Suite uid
    private long iCreationTime = 0;
    private int iType = TYPE_INSTALL; // install or uninstall
    private int iOtaCode = OtaStatusCode.SUCCESS; // OTA status code
    private String iUrl = null; // URL to be notified
    private long iLatestRetryTime = 0;
    private int iRetryCount = 0; // How many times sending has been retried

    /** Constructor. */
    private OtaStatusNotification()
    {
    }

    /** Constructor. */
    public OtaStatusNotification(
        Uid aUid, long aCreationTime, int aType, int aOtaCode,
        String aUrl, long aLatestRetryTime, int aRetryCount)
    {
        iUid = aUid;
        iCreationTime = aCreationTime;
        iType = aType;
        iOtaCode = aOtaCode;
        iUrl = aUrl;
        iLatestRetryTime = aLatestRetryTime;
        iRetryCount = aRetryCount;
    }

    /** Get suite uid. */
    public Uid getUid()
    {
        return iUid;
    }

    /** Get creation time. */
    public long getCreationTime()
    {
        return iCreationTime;
    }

    /** Get notification type. */
    public int getType()
    {
        return iType;
    }

    /** Get OTA status code. */
    public int getOtaCode()
    {
        return iOtaCode;
    }

    /** Get notification URL. */
    public String getUrl()
    {
        return iUrl;
    }

    /** Get latest retry time. */
    public long getLatestRetryTime()
    {
        return iLatestRetryTime;
    }

    /** Set latest retry time. */
    public void setLatestRetryTime(long aTime)
    {
        iLatestRetryTime = aTime;
    }

    /** Get retry count. */
    public int getRetryCount()
    {
        return iRetryCount;
    }

    /** Set retry count. */
    public void setRetryCount(int aCount)
    {
        iRetryCount = aCount;
    }

    /**
     * Returns OTA status report message body.
     */
    public String getStatusReportBody()
    {
        return getStatusReportBody(getOtaCode());
    }

    /**
     * Returns OTA status report message body.
     */
    public static String getStatusReportBody(int aOtaCode)
    {
        String result = OtaStatusCode.getOtaStatusReportBody(aOtaCode);
        if (result == null)
        {
            InstallerException.internalError
            ("Unrecognized OTA status code: " + aOtaCode);
        }
        return result;
    }

    /**
     * Increase retry count and set the latest retry time
     * to current time.
     *
     * @return false if maximum number of retries have already
     * been made, true if sending this notification can still
     * be tried
     */
    public boolean increaseRetryCount()
    {
        if (iRetryCount >= MAX_RETRY_COUNT)
        {
            return false;
        }
        iLatestRetryTime = System.currentTimeMillis();
        iRetryCount++;
        return true;
    }

    /**
     * Compare this OtaStatusNotification to given one.
     * Comparison is based only on Uid and CreationTime.
     * @return a negative integer, zero, or a positive integer
     * if this instance is less than, equal to, or greater than
     * the given version.
     */
    public int compareTo(OtaStatusNotification aNotification)
    {
        int result = getUid().compareTo(aNotification.getUid());
        if (result == 0)
        {
            if (getCreationTime() < aNotification.getCreationTime())
            {
                result = -1;
            }
            else if (getCreationTime() > aNotification.getCreationTime())
            {
                result = 1;
            }
        }
        return result;
    }

    /**
     * Indicates whether given object is equal to this one.
     * Comparison is based only on Uid and CreationTime.
     */
    public boolean equals(Object aObj)
    {
        if (!(aObj instanceof OtaStatusNotification))
        {
            return false;
        }
        OtaStatusNotification osn = (OtaStatusNotification)aObj;
        if (this.getUid().compareTo(osn.getUid()) == 0 &&
                this.getCreationTime() == osn.getCreationTime())
        {
            return true;
        }
        return false;
    }

    /**
     * Returns a hash code value for this object.
     */
    public int hashCode()
    {
        return getUid().hashCode() + (int)getCreationTime();
    }

    /** Returns string representation of this object. */
    public String toString()
    {
        StringBuffer buf = new StringBuffer("OtaStatusNotification:\n");
        buf.append("  Uid: ").append(getUid()).append("\n");
        buf.append("  Type: ").append(getTypeStr()).append(" (")
        .append(getType()).append(")\n");
        buf.append("  OtaCode: ").append(getOtaCode()).append("\n");
        buf.append("  StatusReport: ").append(getStatusReportBody()).append("\n");
        buf.append("  URL: ").append(getUrl()).append("\n");
        buf.append("  CreationTime: ")
        .append(new Date(getCreationTime())).append(" (")
        .append(getCreationTime()).append(")\n");
        buf.append("  LatestRetryTime: ")
        .append(new Date(getLatestRetryTime())).append(" (")
        .append(getLatestRetryTime()).append(")\n");
        buf.append("  RetryCount: ").append(getRetryCount());
        return buf.toString();
    }

    /** Get notification type as a string. */
    private String getTypeStr()
    {
        String result = null;
        switch (getType())
        {
        case TYPE_INSTALL:
            result = "INSTALL";
            break;
        case TYPE_UNINSTALL:
            result = "UNINSTALL";
            break;
        }
        return result;
    }
}
