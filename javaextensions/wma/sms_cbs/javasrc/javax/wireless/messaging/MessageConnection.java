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


package javax.wireless.messaging;

import java.io.IOException;
import java.io.InterruptedIOException;

import javax.microedition.io.Connection;

public interface MessageConnection extends Connection
{
    String TEXT_MESSAGE = "text";
    String BINARY_MESSAGE = "binary";
    String MULTIPART_MESSAGE = "multipart";


    Message newMessage(String aType);

    Message newMessage(String aType, String aAddress);

    Message receive()throws IOException, InterruptedIOException;

    void send(Message aMsg) throws IOException, InterruptedIOException;

    void setMessageListener(MessageListener aListener) throws IOException;

    int numberOfSegments(Message aMessage);
}