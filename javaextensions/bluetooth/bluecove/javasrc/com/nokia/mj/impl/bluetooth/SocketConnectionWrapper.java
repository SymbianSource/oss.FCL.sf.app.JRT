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

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import javax.microedition.io.Connector;
import javax.microedition.io.SocketConnection;

public class SocketConnectionWrapper implements SocketConnection
{
    private SocketConnection iSocket;

    private SocketConnectionWrapper()
    {
    }

    public SocketConnectionWrapper(String host, int port) throws IOException
    {
        iSocket = (SocketConnection) Connector.open("socket://" + host + ":"
                  + port);
    }

    public String getAddress() throws IOException
    {
        return iSocket.getAddress();
    }

    public String getLocalAddress() throws IOException
    {
        return iSocket.getLocalAddress();
    }

    public int getLocalPort() throws IOException
    {
        return iSocket.getLocalPort();
    }

    public int getPort() throws IOException
    {
        return iSocket.getPort();
    }

    public int getSocketOption(byte option) throws IllegalArgumentException,
                IOException
    {
        return iSocket.getSocketOption(option);

    }

    public void setSocketOption(byte option, int value)
    throws IllegalArgumentException, IOException
    {
        iSocket.setSocketOption(option, value);
    }

    public void close() throws IOException
    {
        iSocket.close();
    }

    public InputStream openInputStream() throws IOException
    {
        return iSocket.openInputStream();
    }

    public DataInputStream openDataInputStream() throws IOException
    {
        return iSocket.openDataInputStream();
    }

    public OutputStream openOutputStream() throws IOException
    {
        return iSocket.openOutputStream();
    }

    public DataOutputStream openDataOutputStream() throws IOException
    {
        return iSocket.openDataOutputStream();
    }
}
