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


package com.nokia.mj.impl.socket;

import java.io.IOException;
import javax.microedition.io.*;
import com.nokia.mj.impl.gcf.utils.*;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.connectionmanager.ConnectionManager;
import com.nokia.mj.impl.connectionmanager.AccessPoint;
import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.utils.Uid;

/**
 * Socket Server Connection Implementation class.
 */

public class ServerSocketConnectionImpl implements ServerSocketConnection
{
    private int iMode;

    private int iNativeHandle;

    private int iPort;

    private UrlParser iUri;

    private boolean iConnectionOpen = false;

    private int iSockDesc;

    private int iSocketServerFactoryHandle;

    private ConnectionManager cminstance = null;

    private AccessPoint apn = null;

    /**
     * Constructs the ServerSocketConnectionImpl.
     *
     * @param uri
     *            the uri on which this server socket is opened
     * @param mode
     *            the with which server connection is opened
     */

    public ServerSocketConnectionImpl(UrlParser aUri, int aMode)
    throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   " + ServerSocketConnectionImpl: ServerSocketConnectionImpl");
        iMode = aMode;
        if (aUri.port == -1) // socket://:
            iPort = 0;
        else
            iPort = aUri.port;
        iUri = aUri;
        int ret[] = new int[1];
        iNativeHandle = _createSocketServerNativePeer(iUri.toString(), aMode,
                        iPort, ret);

        iSocketServerFactoryHandle = ret[0];

        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   " iSocketServerFactory returned is "
                   + iSocketServerFactoryHandle);
        if (iNativeHandle == 0)
        {
            // push exception was thrown since for this url, push connection is
            // already registered
            throw new IOException(
                "Unable to open serversocket connection: failed to create native peer");
        }

        iSockDesc = _openServerSocket(iNativeHandle,
                                      iSocketServerFactoryHandle, iUri.toString());
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "SocketDescriptor returned is " + iSockDesc);
        if (iSockDesc < 0)
        {
            throw new IOException(
                "Unable to open serversocket connection. POSIX error code: "
                + iSockDesc);
        }

        iConnectionOpen = true;
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   " - ServerSocketConnectionImpl: ServerSocketConnectionImpl");
    }

    /**
     * Get the local socket address of this connection.
     *
     * @return the local address
     * @throws IOException
     *             in case of any IO errors
     */

    public String getLocalAddress() throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "+SocketServerConnectionImpl::getLocalAddress \n");
        String addr[] = new String[1];
        if (iConnectionOpen == false)
        {
            throw new IOException(
                "getLocalAddress failed: connection is already closed");
        }
        int apId = -1;
        int apType = -1;
        try
        {

            Uid appSuite = ApplicationInfo.getInstance().getSuiteUid();
            cminstance = ConnectionManager.getInstance();
            AccessPoint apn = cminstance.getApplicationDefault(appSuite);
            if (apn != null)
            {
                apId = apn.getNapId();
                apType = apn.getType();

                if ((apType!=AccessPoint.NAP_SNAP) && (apType!=AccessPoint.NAP_IAP))
                {
                    apType = -1;
                    apId = -1;
                }
                Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                           "+SocketServerConnectionImpl:: getApplicationDefault returned,  \n"+apn.getNapId()+" type = "+apn.getType());
                System.out.println(" type = "+apn.getType()+"  id = "+apId);
            }
        }
        catch (Exception e)
        {

        }
        int retValue = _getLocalAddress(iNativeHandle, iSockDesc, apId, apType, addr);
        if (retValue < 0)
        {
            throw new IOException(
                "getLocalAddress failed. POSIX error code: "
                + retValue);
        }
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "-SocketServerConnectionImpl::getLocalAddress()");
        return addr[0];
    }

    /**
     * Get the local port of this connection.
     *
     * @return the local port
     * @throws IOException
     *             in case of any IO errors
     */

    public int getLocalPort() throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "+SocketServerConnectionImpl::getLocalPort\n");
        if (iConnectionOpen == false)
        {
            throw new IOException(
                "getLocalPort failed: connection is already closed");
        }
        int result = _getLocalPort(iNativeHandle, iSockDesc);
        if (result < 0)
        {
            throw new IOException(
                "getLocalPort failed. POSIX error code: " + result);
        }
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "-SocketServerConnectionImpl::getLocalPort\n");
        return result;
    }

    /**
     * Wait for an incoming connection.
     *
     * @return the accepted incoming stream connection
     * @throws IOException
     *             in case of any IO errors
     */

    synchronized public StreamConnection acceptAndOpen() throws IOException
    {

        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "+ServerSocketConnectionImpl::acceptAndOpen \n");
        if (iConnectionOpen == false)
        {
            throw new IOException(
                "acceptAndOpen failed: connection is already closed");
        }
        int sockDesc = -1;
        sockDesc = _accept(iNativeHandle);
        if (sockDesc < 0)
        {
            throw new IOException(
                "acceptAndOpen failed. POSIX error code: " + sockDesc);
        }
        if (iPort == 0)
            iPort = getLocalPort();
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "+ServerSocketConnectionImpl::acceptAndOpen - dynamic port = "
                   + iPort);
        SocketConnectionImpl con = new SocketConnectionImpl(iUri, iMode,
                sockDesc, cminstance, apn);
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "sockDesc:" + sockDesc);
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "-SocketServerConnectionImpl::acceptAndOpen \n");
        return con;
    }

    /**
     * Close the socket connection.
     */

    public void close() throws IOException
    {

        if (iConnectionOpen == false)
        {
            return;
        }
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "+SocketServerConnectionImpl::Close \n");
        int ret = _close(iNativeHandle, iSocketServerFactoryHandle, iUri
                         .toString(), iSockDesc);
        if (ret != 0)
        {
            throw new IOException("Close failed. POSIX error code:: "
                                  + ret);
        }
        iConnectionOpen = false;
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "-SocketServerConnectionImpl::Close \n");
    }

    private native int _createSocketServerNativePeer(String uri, int mode,
            int port, int[] sd);

    private native int _getLocalPort(int iNativeHandle, int iSockDesc);

    private native int _accept(int iNativeHandle);

    private native int _getLocalAddress(int iNativeHandle, int iSockDesc,int midletIapId,int apType,
                                        String[] addr);

    private native int _close(int iNativeHandle,
                              int iSocketServerFactoryHandle, String urls, int iSockDesc);

    private native int _openServerSocket(int iNativeHandle,
                                         int iSocketServerFactoryHandle, String url);
}
