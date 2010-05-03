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


package com.nokia.mj.impl.installer.captainservice;

import com.nokia.mj.impl.comms.CommsMessage;
import com.nokia.mj.impl.installer.utils.PlatformUid;
import com.nokia.mj.impl.utils.Uid;

/**
 * Response to CaptainRequest message.
 */
public class CaptainResponse
{
    /** Status indicating successful operation. */
    public static final int STAT_OK = 0;
    /** Status indicating failed operation. */
    public static final int STAT_NOK = 1;

    /** Uids to which requested operation was performed. */
    private Uid[] iUids = null;
    /** Statuses of operations. */
    private int[] iStats = null;

    /**
     * Construct a CaptainResponse message.
     */
    public CaptainResponse(CommsMessage aMsg)
    {
        parseResponse(aMsg);
    }

    /**
     * Returns Uids.
     */
    public Uid[] getUids()
    {
        return iUids;
    }

    /**
     * Returns statuses.
     */
    public int[] getStats()
    {
        return iStats;
    }

    /**
     * Returns true if all statuses indicate successful operation, false otherwise.
     */
    public boolean statusOk()
    {
        for (int i = 0; i < iStats.length; i++)
        {
            if (iStats[i] != STAT_OK)
            {
                return false;
            }
        }
        return true;
    }

    /**
     * Initialize members of this object from data read from received message.
     */
    private void parseResponse(CommsMessage aMsg)
    {
        int count = aMsg.readInt();
        iUids = new Uid[count];
        iStats = new int[count];
        for (int i = 0; i < count; i++)
        {
            iUids[i] = PlatformUid.createUid(aMsg.readString());
            iStats[i] = aMsg.readInt();
        }
    }

    /**
     * Returns string representation of this object.
     */
    public String toString()
    {
        StringBuffer buf = new StringBuffer();
        buf.append("CaptainResponse: uids=");
        if (iUids != null)
        {
            for (int i = 0; i < iUids.length; i++)
            {
                buf.append(iUids[i].toString());
                if (i < iUids.length-1)
                {
                    buf.append(",");
                }
            }
        }
        buf.append(", stats=");
        if (iStats != null)
        {
            for (int i = 0; i < iStats.length; i++)
            {
                buf.append(iStats[i]);
                if (i < iStats.length-1)
                {
                    buf.append(",");
                }
            }
        }
        return buf.toString();
    }
}
