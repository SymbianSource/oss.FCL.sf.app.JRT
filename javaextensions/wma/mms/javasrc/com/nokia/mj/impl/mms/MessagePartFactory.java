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


package com.nokia.mj.impl.mms;

import javax.wireless.messaging.MessagePart;

/**
 * Interface for creating instances of MessagePart class. The public class
 * (MessagePart) of JSR205 registers its private services (of creating
 * MessagePart instances) towards the MessagePartImpl
 */
public interface MessagePartFactory
{
    public MessagePart initializeMessagePart(byte[] aContents, String aMimeType,
            String aContentId, String aContentLocation, String aEnc);
}
