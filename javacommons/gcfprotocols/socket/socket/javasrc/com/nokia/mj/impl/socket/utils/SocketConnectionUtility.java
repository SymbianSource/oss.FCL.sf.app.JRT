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


package com.nokia.mj.impl.socket.utils;

import java.io.IOException;
import javax.microedition.io.SocketConnection;
import com.nokia.mj.impl.socket.SocketConnectionImpl;
import com.nokia.mj.impl.gcf.utils.UrlParser;
import com.nokia.mj.impl.connectionmanager.ConnectionManager;
import com.nokia.mj.impl.connectionmanager.AccessPoint;

public class SocketConnectionUtility
{
    public static final int DEFAULT_MODE = 3; // default mode is read/write

    public static final int SOCKET_DESC = -1; // no opened socket is passed

    protected SocketConnectionImpl sockObj = null;

    private ConnectionManager cminstance = null;

    private AccessPoint apn = null;

    public SocketConnectionUtility(int host, int port) throws IOException
    {
        StringBuffer sockurl = new StringBuffer();
        sockurl.append("socket://" + host + ":" + port);
        UrlParser url1 = new UrlParser(sockurl.toString());

        sockObj = new SocketConnectionImpl(url1, DEFAULT_MODE, SOCKET_DESC,
                                           cminstance, apn);
    }

    public SocketConnectionUtility(String aSockUrl) throws IOException
    {
        UrlParser url = new UrlParser("socket", aSockUrl);
        sockObj = new SocketConnectionImpl(url, DEFAULT_MODE, SOCKET_DESC,
                                           cminstance, apn);
    }

    public SocketConnectionImpl getSocketInstance()
    {
        return sockObj;
    }

    public String getAddress() throws IOException
    {
        return sockObj.getAddress();
    }

    public String getLocalAddress() throws IOException
    {
        return sockObj.getLocalAddress();
    }

    public int getLocalPort() throws IOException
    {
        return sockObj.getLocalPort();
    }

    public int getPort() throws IOException
    {
        return sockObj.getPort();
    }

    public int getSocketOption(byte aOption) throws IllegalArgumentException,
                IOException
    {
        return sockObj.getSocketOption(aOption);
    }

    public void setSocketOption(byte aOption, int aValue)
    throws IllegalArgumentException, IOException
    {
        sockObj.setSocketOption(aOption, aValue);
    }

    public synchronized void close() throws IOException
    {
        sockObj.close();
    }
};