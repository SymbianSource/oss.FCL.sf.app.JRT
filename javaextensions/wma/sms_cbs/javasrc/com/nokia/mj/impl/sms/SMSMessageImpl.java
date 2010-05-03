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

package com.nokia.mj.impl.sms;

import java.util.Date;
import javax.wireless.messaging.Message;

/**
 * This class implements Message Interface
 */
public abstract class SMSMessageImpl implements Message
{
    public static final int MESSAGE_TEXT = 0;

    public static final int MESSAGE_BINARY = 1;

    private String iMsgAddress;

    private final long iMsgTimestamp;

    SMSMessageImpl(String aAddress, long aMsgTimestamp)
    {
        iMsgAddress = aAddress;
        iMsgTimestamp = aMsgTimestamp;
    }

    /**
     * Method to retrieve the address set in the message
     * @return the message address
     * @see
     */
    public final String getAddress()
    {
        return iMsgAddress;
    }

    /**
     * sets the message address
     * @param aAddress an absolute message address.
     */
    public final void setAddress(String aAddress)
    {
        iMsgAddress = aAddress;
    }

    /**
     * Method to retrieve the time stamp
     * @return the TimeStamp of the message or NULL if the timestamp is not set
     */
    public final Date getTimestamp()
    {
        if (iMsgTimestamp == 0)
        {
            return null;
        }
        return new Date(iMsgTimestamp);
    }


    public abstract int getType();

    public abstract String getPayloadTextData();

    public abstract byte[] getPayloadBinaryData();

}
