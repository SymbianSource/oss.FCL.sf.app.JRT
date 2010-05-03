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

import javax.wireless.messaging.BinaryMessage;

/**
 * This class implements BinaryMessage and extends from SMSMessageImpl
 */
public final class SMSBinaryMessageImpl extends SMSMessageImpl implements
        BinaryMessage
{
    private byte[] iPayload;

    public SMSBinaryMessageImpl(String aAddress, long aTimestamp)
    {
        super(aAddress, aTimestamp);
    }

    /**
     * Method to get the message payload text
     * @returns null if not supported
     * @see
     */
    public String getPayloadTextData()
    {
        return null;
    }

    /**
    *Refer About this in JSR-205(Wireless Messaging API 2.0)specification
    */

    public byte[] getPayloadBinaryData()
    {
        return getPayloadData();
    }

    /**
      *Refer About this in JSR-205(Wireless Messaging API 2.0)specification
      */
    public byte[] getPayloadData()
    {
        return iPayload;
    }

    /**
      *Refer About this in JSR-205(Wireless Messaging API 2.0)specification
      */
    public void setPayloadData(byte[] aData)
    {
        iPayload = aData;
    }

    /**
     * Method to get the type of this message
     * @return returns message type
     */
    public int getType()
    {
        return MESSAGE_BINARY;
    }
}