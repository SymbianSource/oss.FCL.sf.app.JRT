/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.https;

import java.io.IOException;
import javax.microedition.io.*;
import com.nokia.mj.impl.gcf.utils.UrlParser;
import com.nokia.mj.impl.gcf.utils.StreamConnectionBase;
import com.nokia.mj.impl.http.HttpClientConnection;
import com.nokia.mj.impl.ssl.SecureConnectionImpl;
import com.nokia.mj.impl.utils.Logger;

import java.io.*;

/**
 * Https Connection Implementation class.
 *
 */
public class HttpsConnectionImpl extends HttpClientConnection implements
        HttpsConnection
{
    private UrlParser iUri;

    private SecureConnectionImpl mSecureConnobj;

    private static final int DEFAULT_HTTPS_PORT = 443;

    protected static SecureConnectionPool mSSLConnectionPool;

    static
    {
        int max = 2; // maximum number of persistent connection
        mSSLConnectionPool = new SecureConnectionPool(max);
    }

    /**
     * Constructs the HttpsConnectionImpl with default request method - GET.
     *
     * @param url
     *            url string
     * @param mode
     *            mode of connection
     */
    public HttpsConnectionImpl(String uri, int mode)
    {
        super(uri);
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "HttpsConnectionImpl() "
                   + uri);
        iUri = new UrlParser("https", uri);
        if (iUri.port == -1)
        {
            iUri.port = DEFAULT_HTTPS_PORT;
        }
        mHttpurl = iUri;
    }

    /**
     * Creates the ssl connection
     *
     * @throws IOException
     *             in case of IOException
     */
    public void createSocketConnection() throws IOException
    {
        int socket_desc = 0;
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "HttpsConnectionImpl: calling overloaded https connect method");
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "sock_dess " + socket_desc);

        HttpsElement https_elm = mSSLConnectionPool
                                 .getConnection((iUri.host + iUri.port));

        if (https_elm == null)
        {
            String tmpproxy = null;
            tmpproxy = getProxy();
            Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                       " https , get proxy returned " + tmpproxy);
            if (tmpproxy == null)
            {
                mIsProxyUsed = false;
                socket_desc = -1;
            }
            else
            {
                // in case of proxy
                mIsProxyUsed = true;
                int colon = tmpproxy.indexOf(':');
                String proxyHost = tmpproxy.substring(0, colon);
                String proxyPortString = tmpproxy.substring(colon + 1, tmpproxy
                                         .length());
                int proxyPort = Integer.parseInt(proxyPortString);
                socket_desc = _createNativePeer(proxyHost, proxyPort,
                                                iUri.host, iUri.port);
                if (socket_desc < 0)
                {
                    throw new IOException(
                        "Could not connect to host, error code: "
                        + socket_desc);
                }
            }
            mSecureConnobj = new SecureConnectionImpl(iUri, 3, socket_desc,
                    null, null);

            // ssl read and write
            mSocketInStream = mSecureConnobj.openInputStream();

            // ssl read and write
            mSocketOutStream = mSecureConnobj.openOutputStream();
        }
        else
        {
            Logger.LOG(Logger.ESOCKET, Logger.EInfo, "old socket obj");
            mSocketOutStream = https_elm.getSslOutputStream();
            mSecureConnobj = https_elm.getSecureConnObject();
            mSocketInStream = mSecureConnobj.openInputStream();
            mIsProxyUsed = https_elm.getSslProxyFlag();
            mOldSocketFlag = true;
            Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                       "Using the old ssl connection, mSecureConnobj");
        }
    }

    public void sendRequest() throws IOException
    {
        // creates request, sends request to server and reads response recieved
        // from server.
        if (mHttpConnectionState == HTTP_CONNECTED)
        {
            // request has already been sent
            return;
        }
        mHttpConnectionState = HTTP_CONNECTED;
        if (mSecureConnobj == null)
        {
            createSocketConnection();
        }

        if (mSendChunkedReq == true)
        {
            sendChunkedRequest(true);
        }
        else
        {
            // send request
            sendRequestLine();
        }

        if (mResentFlag == false)
        {
            if (mSocketInStream == null)
            {
                mSocketInStream = mSecureConnobj.openInputStream();
            }
            readResponse(mSocketInStream);
        }

        // Logger.LOG(Logger.ESOCKET,
        // Logger.EInfo,"httpClientConnection-sendRequest()");
    }

    /**
     * resend_request is an internal function , which will be called when either
     * the send request or read response fails, because of using an old tcp
     * connection from the connection pool. This failure is not reported to the
     * application since it is not aware of this persistent connection. So the
     * request will be resent using a new tcp connection.
     */
    public void resend_request() throws IOException
    {
        mHttpConnectionState = HTTP_SETUP;
        mOldSocketFlag = false;
        mSecureConnobj = null;
        mSSLConnectionPool.removeConnection((iUri.host + iUri.port));
        sendRequest();
        mResentFlag = true;
    }

    /**
     * closes the https connection.
     */
    public void close()
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "https close() method called closeflag = " + mCloseFlag
                   + " mEof == " + mEof);
        if (mHttpConnectionState == HTTP_CLOSED)
        {
            return;
        }

        mHttpConnectionState = HTTP_CLOSED;
        try
        {
            if ((mCloseFlag == false) && (mEof == true))
            {
                // if the first respone is read completely and server has not
                // closed the connection
                // then add this tcp connection to the connection pool
                mSSLConnectionPool.addConnection((iUri.host + iUri.port),
                                                 mSecureConnobj, mSocketInStream, mSocketOutStream,
                                                 mIsProxyUsed);
            }
            else
            {
                // first response not read completely or server closed
                // so this tcp connection cannot be re-used, close it
                mSSLConnectionPool.removeConnection((iUri.host + iUri.port));
                if (mSocketInStream != null)
                {
                    mSocketInStream.close();
                }
                if (mSocketOutStream != null)
                {
                    mSocketOutStream.close();
                }
                if (mSecureConnobj != null)
                {
                    mSecureConnobj.close();
                }
                mSocketInStream = null;
                mSocketOutStream = null;
                mSecureConnobj = null;
            }
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                       "exception while closing https connection ");
        }
    }

    /**
     * Get the security info
     *
     * @return SecurityInfo instance
     * @throws IOException
     *             in case of IOException
     */
    public SecurityInfo getSecurityInfo() throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "HttpsConnection :: getSecurityInfo() called ");
        getConnected();
        return mSecureConnobj.getSecurityInfo();
    }

    private native int _createNativePeer(String proxyAddr, int proxyPort, String host, int port);
}
