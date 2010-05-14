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

import javax.wireless.messaging.TextMessage;

/**
 * This class implements TextMessage and extends from SMSMessageImpl
 * @see
 */
public final class SMSTextMessageImpl extends SMSMessageImpl implements
        TextMessage
{
    private String iPayload;

    public SMSTextMessageImpl(String aAddress, long aMsgTimestamp)
    {
        super(aAddress, aMsgTimestamp);
    }
    /**
     * Method to get the message payload text
     * @return Returns the message payload data as a String
     *         or null if it is not set
     */
    String getPayloadTextData()
    {
        return getPayloadText();
    }
    /**
     * Method to get the message payload Data
     * @returns null if it is not supported
     */
    byte[] getPayloadBinaryData()
    {
        return null;
    }
    /**
     * Method to get the message payload text
     * @return Returns the message payload data as a String
     *         or null if it is not set
     */
    public String getPayloadText()
    {
        return iPayload;
    }
    /**
     * Sets the payload data of this message.The payload may be set to null.
     * @param aData payload Data as a string.
     */
    public void setPayloadText(String aMsgData)
    {
        iPayload = aMsgData;
    }
    /**
     * Method to get the type of this message
     * @return returns message type
     */
    public int getType()
    {
        return MESSAGE_TEXT;
    }
}