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

package com.nokia.mj.impl.bluetooth;

import java.io.IOException;

import javax.microedition.io.Connector;
import javax.microedition.io.ServerSocketConnection;
import javax.microedition.io.StreamConnection;

public class ServerSocketConnectionWrapper implements ServerSocketConnection
{
    private ServerSocketConnection iServerSocket;

    public ServerSocketConnectionWrapper(int port) throws IOException
    {
        iServerSocket = (ServerSocketConnection) Connector.open("socket://:"
                        + port);
    }

    public String getLocalAddress() throws IOException
    {
        return iServerSocket.getLocalAddress();
    }

    public int getLocalPort() throws IOException
    {
        return iServerSocket.getLocalPort();
    }

    public StreamConnection acceptAndOpen() throws IOException
    {
        return iServerSocket.acceptAndOpen();
    }

    public void close() throws IOException
    {
        iServerSocket.close();
    }
}
