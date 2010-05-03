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
import com.nokia.mj.impl.gcf.utils.StreamConnectionBase;
import java.io.*;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.rt.support.Finalizer;
import com.nokia.mj.impl.connectionmanager.ConnectionManager;
import com.nokia.mj.impl.connectionmanager.AccessPoint;

/**
 * Socket Connection Implementation class.
 */

public class SocketConnectionImpl extends StreamConnectionBase implements
        SocketConnection
{

    private UrlParser iUri;

    private Finalizer iFinalizer;

    private boolean iConnectionOpen = false;

    private boolean iOutputStreamOpenedOnce = false;

    private boolean iInputStreamOpenedOnce = false;

    private boolean iDataOutputStreamOpenedOnce = false;

    private boolean iDataInputStreamOpenedOnce = false;

    private ConnectionManager iCmInstance = null;

    private AccessPoint iApn = null;

    /**
     * Constructs the SocketConnectionImpl.
     *
     * @param uri
     *            the uri to which socket connection is opened
     * @param mode
     *            the with which socket connection is opened
     */
    public SocketConnectionImpl(UrlParser aUri, int aMode, int aSock,
                                ConnectionManager cm, AccessPoint apnInfo) throws IOException
    {

        super(aUri.toString(), aMode, false);
        iCmInstance = cm;
        iApn = apnInfo;

        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "+SocketConnectionImpl::SocketConnectionImpl()");

        int ret = 0;
        if (iNativePeerHandle == 0)
        {
            throw new IOException(
                "Unable to open socket connection: failed to create native peer ");
        }
        int retval[] = new int[1];
        if (iApn != null)
        {
            ret = _openSocket(iNativePeerHandle, aSock, iApn.getType(), iApn
                              .getNapId(), retval);
        }
        else
        {
            ret = _openSocket(iNativePeerHandle, aSock, -1, -1, retval);
        }
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "setting access point return value =" + retval[0]);
        if (iCmInstance != null)
        {
            if (retval[0] < 0)
            {
                // reset connection manager
                iCmInstance.reset();
            }
        }
        if (ret < 0)
        {
            throw new IOException(
                "Unable to open socket connection. POSIX error code: " + ret);
        }

        iConnectionOpen = true;
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "SocketConnectionImpl::SocketConnectionImpl(): _openSocket return value "
                   + ret);
        iFinalizer = createFinalizer();
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "-SocketConnectionImpl::SocketConnectionImpl()");
    }

    /**
     * Get the peer socket address of this connection.
     *
     * @return the peer address
     * @throws IOException
     *             in case of any IO errors
     */

    public String getAddress() throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "+SocketConnectionImpl::getAddress()");
        String addr[] = new String[1];
        if (iConnectionOpen == false)
        {
            throw new IOException(
                "getAddress failed: connection is already closed");
        }
        int retValue = _getAddress(iNativePeerHandle, addr);
        if (retValue < 0)
        {
            throw new IOException("getAddress failed. POSIX error code: "
                                  + retValue);
        }
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "-SocketConnectionImpl::getAddress()");
        return addr[0];
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
                   "+SocketConnectionImpl::getLocalAddress()");
        String addr[] = new String[1];
        if (iConnectionOpen == false)
        {
            throw new IOException(
                "getLocalAddress failed: connection is already closed");
        }
        int retValue = _getLocalAddress(iNativePeerHandle, addr);
        if (retValue < 0)
        {
            throw new IOException(
                "getLocalAddress failed. POSIX error code: "
                + retValue);
        }
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "-SocketConnectionImpl::getLocalAddress()");
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
                   "+SocketConnectionImpl::getLocalPort()");
        if (iConnectionOpen == false)
        {
            throw new IOException(
                "getLocalPort failed: connection is already closed");
        }
        int result = _getLocalPort(iNativePeerHandle);
        if (result < 0)
        {
            throw new IOException(
                "getLocalPort failed. POSIX error code: " + result);
        }
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "-SocketConnectionImpl::getLocalPort()");
        return result;
    }

    /**
     * Get the peer port of this connection.
     *
     * @return the local address
     * @throws IOException
     *             in case of any IO errors
     */

    public int getPort() throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "+SocketConnectionImpl::getPort()");
        if (iConnectionOpen == false)
        {
            throw new IOException(
                "getPort failed: connection is already closed");
        }
        int retVal = _getport(iNativePeerHandle);
        if (retVal < 0)
        {
            throw new IOException("getPort failed. POSIX error code: "
                                  + retVal);
        }
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "-SocketConnectionImpl::getPort()");
        return retVal;
    }

    /**
     * Please refer JSR 118.
     *
     * @return the socket option value
     * @param aOption
     *            the socket option
     * @throws IOException
     *             in case of any IO errors
     * @throws IllegalArgumentException
     *             in case of invalid argument
     */
    public int getSocketOption(byte aOption) throws IllegalArgumentException,
                IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "+SocketConnectionImpl::getSocketOption()");
        if (iConnectionOpen == false)
        {
            throw new IOException(
                "getSocketOption failed: connection is already closed");
        }
        checkOption(aOption);
        int retVal = _getSocketOption(iNativePeerHandle, aOption);
        if (retVal < 0)
        {
            throw new IOException(
                "getSocketOption failed. POSIX error code: " + retVal);
        }
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "-SocketConnectionImpl::getSocketOption()");
        return retVal;
    }

    /**
     * Please refer JSR 118.
     *
     * @param aValue
     *            the socket option value
     * @param aOption
     *            the socket option
     * @throws IOException
     *             in case of any IO errors
     * @throws IllegalArgumentException
     *             in case of invalid argument
     */

    public void setSocketOption(byte aOption, int aValue)
    throws IllegalArgumentException, IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "+SocketConnectionImpl::setSocketOption()");

        if (iConnectionOpen == false)
        {
            throw new IOException(
                "setSocketOption failed: connection is already closed");
        }
        checkOption(aOption);

        if (aValue < 0)
        {
            throw new IllegalArgumentException(
                "Invalid value for setSocketOption method");
        }
        int retVal = _setSocketOption(iNativePeerHandle, aOption, aValue);
        if (retVal < 0)
        {
            throw new IOException(
                "setSocketOption failed. POSIX error code: " + retVal);
        }
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "-SocketConnectionImpl::setSocketOption()");
    }

    private void checkOption(byte option) throws IllegalArgumentException
    {
        if ((option != DELAY) && (option != KEEPALIVE) && (option != LINGER)
                && (option != RCVBUF) && (option != SNDBUF))
        {
            throw new IllegalArgumentException("Unsupported socket option");
        }
    }

    /**
     * Creates an OutputStream object. This overrides the Streams
     * OpenOutputStream For TCK Compliance - open, close and open of
     * outputstream should throw IOException
     *
     * @return a new instance on OutputStreamBase.
     * @throws IOException
     *             in case there is an attempt to open outputstream more than
     *             once for a connection
     */
    public OutputStream openOutputStream() throws IOException
    {
        if (iOutputStreamOpenedOnce == true)
        {
            throw new IOException(
                "OutputStream cannot be opened multiple times");
        }
        else
        {
            iOutputStreamOpenedOnce = true;
            return super.openOutputStream();
        }
    }

    /**
     * Creates an DataOutputStream object. This overrides the Streams
     * OpenDataOutputStream For TCK Compliance - open, close and open of
     * DataOutputStream should throw IOException
     *
     * @return a new instance on DataOutputStreamBase.
     * @throws IOException
     *             in case there is an attempt to open DataOutputStream more
     *             than once for a connection
     */
    public DataOutputStream openDataOutputStream() throws IOException
    {
        if (iDataOutputStreamOpenedOnce == true)
        {
            throw new IOException(
                "DataOutputStream cannot be opened multiple times");
        }
        else
        {
            iDataOutputStreamOpenedOnce = true;
            return super.openDataOutputStream();
        }
    }

    /**
     * Creates an InputStream object. This overrides the Streams openInputStream
     * For TCK Compliance - open, close and open of InputStream should throw
     * IOException
     *
     * @return a new instance on InputStreamBase.
     * @throws IOException
     *             in case there is an attempt to open openInputStream more than
     *             once for a connection
     */
    public InputStream openInputStream() throws IOException
    {
        if (iInputStreamOpenedOnce == true)
        {
            throw new IOException("InputStream cannot be opened multiple times");
        }
        else
        {
            iInputStreamOpenedOnce = true;
            return super.openInputStream();
        }
    }

    /**
     * Creates an DataInputStream object. This overrides the Streams
     * openDataInputStream For TCK Compliance - open, close and open of
     * DatainputStream should throw IOException
     *
     * @return a new instance on DataInputStreamBase.
     * @throws IOException
     *             in case there is an attempt to open openDataInputStream more
     *             than once for a connection
     */
    public DataInputStream openDataInputStream() throws IOException
    {
        if (iDataInputStreamOpenedOnce == true)
        {
            throw new IOException(
                "DataInputStream cannot be opened multiple times");
        }
        else
        {
            iDataInputStreamOpenedOnce = true;
            return super.openDataInputStream();
        }
    }

    /**
     * Close the socket connection.
     */

    public synchronized void close() throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "+SocketConnectionImpl::Close()");
        if (iConnectionOpen == true)
        {
            iConnectionOpen = false;
            super.close(); // closes the input and outputstreams.
            // If streams is closed only then the native is closed
            if (isStreamsClosed() == true)
            {
                int ret = _close(iNativePeerHandle);
                if (ret != 0)
                {
                    throw new IOException(
                        "close failed. POSIX error code: " + ret);
                }
            }
        }
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "-SocketConnectionImpl::Close()");
    }

    /**
     * Creatives the native side handle for this socket connection.
     *
     * @param aUri
     *            the uri to which socket connection is opened
     * @param aMode
     *            the with which socket connection is opened
     * @param aTimeout
     *            the timeout value for this connection
     * @return the native handle
     */
    public int createNativePeer(String aUri, int aMode, boolean aTimeout)
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "+SocketConnectionImpl::createNativePeer()");
        int ret = 0;
        iUri = new UrlParser(aUri);
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "SocketConnectionImpl::createNativePeer() aUri = " + aUri
                   + "aMode = " + aMode + "iUri.host = " + iUri.host);
        if (iUri.host == null)
        {
            ret = _createNativePeer(aUri, aMode, "", iUri.port);
        }
        ret = _createNativePeer(aUri, aMode, iUri.host, iUri.port);
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "-SocketConnectionImpl::createNativePeer()");
        return ret;
    }

    Finalizer createFinalizer()
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "creating a socket finalizer object ");
        return new Finalizer()
        {
            public void finalizeImpl()
            {
                doFinalize();
            }
        };
    }

    public void doFinalize()
    {
        Logger
        .LOG(Logger.ESOCKET, Logger.EInfo,
             "socket doFinalize() called :");
        if (iConnectionOpen == true)
        {
            iConnectionOpen = false;
            if (iNativePeerHandle != 0)
            {
                int ret = _close(iNativePeerHandle);
            }
        }
        if (iNativePeerHandle != 0)
        {
            _dispose(iNativePeerHandle);
            iNativePeerHandle = 0;
        }
    }

    private native int _createNativePeer(String aUri, int aMode, String aHost,
                                         int aPort);

    private native int _openSocket(int iNativePeerHandle, int aSock, int aType,
                                   int aApn, int[] retval);

    private native int _getport(int iNativePeerHandle);

    private native int _getLocalPort(int iNativePeerHandle);

    private native int _getSocketOption(int iNativePeerHandle, int aOption);

    private native int _setSocketOption(int iNativePeerHandle, int aOption,
                                        int aValue);

    private native int _getLocalAddress(int iNativePeerHandle, String[] addr);

    private native int _getAddress(int iNativePeerHandle, String[] addr);

    private native int _close(int iNativePeerHandle);

    private native void _dispose(int iNativePeerHandle);

};
