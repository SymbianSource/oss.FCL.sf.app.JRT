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


package com.nokia.mj.impl.datagram;

import java.io.*;
import javax.microedition.io.*;
import com.nokia.mj.impl.gcf.utils.UrlParser;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.rt.support.Finalizer;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.rt.support.ShutdownListener;
import com.nokia.mj.impl.gcf.protocol.datagram.DatagramPermissionImpl;
import com.nokia.mj.impl.connectionmanager.ConnectionManager;
import com.nokia.mj.impl.connectionmanager.AccessPoint;
import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.utils.Uid;

/**
 * UDP DatagramConnection Implementation class.
 *
 */

public class UDPDatagramConnectionImpl implements UDPDatagramConnection
{
    private int iNativePeerHandle;
    private UrlParser iUri;
    private boolean iConnectionOpen = true;
    private Finalizer iFinalizer;
    private static final String DATAGRAM_PROTOCOL = "datagram";
    private static final int MAX_DATAGRAM_SIZE = 65535;
    private static final int NOMINAL_DATAGRAM_SIZE = 256;
    private ConnectionManager iCmInstance = null;
    private AccessPoint iApn = null;
    private String iFullUrl = null;

    /**
     * Constructs the UDPDatagramConnectionImpl.
     *
     * @param aUri the uri to which udp connection is opened
     * @param aMode the with which udp datagram connection is opened
     * @param aTimeout the with period for the connection
     * @throws IOException in case of any IO errors
     */

    public UDPDatagramConnectionImpl(String aUri, int aMode, boolean aTimeout)
    throws IOException
    {

        Logger.LOG(Logger.ESOCKET,Logger.EInfo," +UDPDatagramConnectionImpl(): aUri = " + aUri);
        int retVal = 0;

        int retval[] = new int[1];

        String apnstring = null;
        String urls = null;
        int apnid = -1;
        int index = -1;
        iFullUrl = "datagram:"+aUri;

        index = aUri.indexOf(";nokia_netid=");
        if (index != -1)
        {
            apnstring = aUri.substring(index);
            int equalindex = apnstring.indexOf('=');
            if (equalindex != -1)
            {
                String temp = apnstring.substring(equalindex+1);
                try
                {
                    apnid = Integer.parseInt(temp);
                }
                catch (NumberFormatException e)
                {
                    throw new IllegalArgumentException("Invalid URL: "+iFullUrl);
                }
            }
            else
            {
                throw new IllegalArgumentException("Invalid URL: "+iFullUrl);
            }
            urls = aUri.substring(0, index);
        }
        else
        {
            index = aUri.indexOf(";nokia_apnid=");
            if (index != -1)
            {
                apnstring = aUri.substring(index);
                int equalindex = apnstring.indexOf('=');
                if (equalindex != -1)
                {
                    String temp = apnstring.substring(equalindex+1);
                    try
                    {
                        apnid = Integer.parseInt(temp);
                    }
                    catch (NumberFormatException e)
                    {
                        throw new IllegalArgumentException("Invalid URL: "+iFullUrl);
                    }
                }
                else
                {
                    throw new IllegalArgumentException("Invalid URL: "+iFullUrl);
                }
                urls = aUri.substring(0, index);
            }
            else
            {
                urls = aUri;
            }
        }
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,"UDPDatagramConnectionImpl -after removing nokia_apnid : "+urls);

        if (urls.equals("//:"))
        {
            String temp = "datagram:"+urls;

            /* security related stuff */
            ApplicationUtils appUtils = ApplicationUtils.getInstance();
            DatagramPermissionImpl per = new DatagramPermissionImpl("datagram://");
            Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                       "calling check permissions from UDPDatagramConnectionImpl");
            appUtils.checkPermission(per);
            /* security related stuff */

            iNativePeerHandle =  _createNativePeer(temp,aMode,"",0);
        }
        else
        {
            iUri = new UrlParser(DATAGRAM_PROTOCOL, urls);

            if (iUri.port == -1)
            {
                /* client connecction should have a port number */
                throw new IllegalArgumentException("Invalid URL: "+iFullUrl);
            }
            /* check for restricted ports */
            Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                       "port = "+iUri.port);

            if (iUri.getPort() == 9200 || iUri.getPort() == 9201 || iUri.getPort() == 9202 || iUri.getPort() == 9203)
            {
                if (ApplicationInfo.getInstance().getRuntimeType().equals(ApplicationInfo.RUNTIME_MIDP) == true)
                {
                    if ((ApplicationInfo.getInstance().getProtectionDomain().equals(ApplicationInfo.UNIDENTIFIED_THIRD_PARTY_DOMAIN)) == true)
                    {
                        Logger.LOG(Logger.ESOCKET, Logger.EInfo,"Throwing security exception for restricted port");
                        throw new SecurityException("Unable to open datagram connection: port " +iUri.getPort()+ " is restricted");
                    }
                }
            }

            if (iUri.host == null)
            {
                /* security related stuff, server mode datagram connection */
                ApplicationUtils appUtils = ApplicationUtils.getInstance();
                DatagramPermissionImpl per = new DatagramPermissionImpl("datagram://");
                Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                           "calling check permissions from UDPDatagramConnectionImpl");
                appUtils.checkPermission(per);
                /* security related stuff */

                iNativePeerHandle =  _createNativePeer(iUri.toString(),aMode,"",iUri.port);
            }
            else
            {
                /* security related stuff, client mode datagram connection */
                ApplicationUtils appUtils = ApplicationUtils.getInstance();
                DatagramPermissionImpl per = new DatagramPermissionImpl("datagram://*");
                Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                           "calling check permissions from UDPDatagramConnectionImpl");
                appUtils.checkPermission(per);
                /* security related stuff */

                iNativePeerHandle =  _createNativePeer(iUri.toString(),aMode,iUri.host,iUri.port);
            }
        }
        if (iNativePeerHandle == 0)
        {
            throw new IOException("Unable to open datagram connection.Creating native peer failed");
        }

        // remove comments if open c patch available
        iCmInstance = ConnectionManager.getInstance();
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,"after getting ConnectionManager instance");
        // get application suite uid
        try
        {
            Uid appSuite = ApplicationInfo.getInstance().getSuiteUid();
            try
            {
                // remove comments if open c patch available
                iApn = iCmInstance.getNetworkAccessPoint(appSuite, aUri);
                Logger.LOG(Logger.ESOCKET, Logger.EInfo,"iApn.getType = "+iApn.getType()+ " iApn.getNapId= "+iApn.getNapId());
            }
            catch (Exception e)
            {
                Logger.LOG(Logger.ESOCKET, Logger.EInfo,"getNetworkAccessPoint threw Exception");
            }

        }
        catch (Exception e)
        {
            Logger.LOG(Logger.ESOCKET, Logger.EInfo,"getSuiteUid exception ");
        }

        if (iApn != null)
        {
            retVal = _openConnection(iNativePeerHandle,iApn.getType(),iApn.getNapId(), retval);
        }
        else
        {
            retVal = _openConnection(iNativePeerHandle, -1, -1, retval);
        }
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "UDPDatagramConnectionImpl::UDPDatagramConnectionImpl(): _openConnection return value "
                   + retVal);
        if (iCmInstance != null)
        {
            if (retval[0] < 0)
            {
                //reset connection manager
                iCmInstance.reset();
            }
        }

        if (retVal < 0)
        {
            throw new IOException("Unable to open datagram connection. POSIX error code: "+retVal);
        }

        iFinalizer = registerforFinalization();

        setShutdownListener();
        Logger.LOG(Logger.ESOCKET,Logger.EInfo," --UDPDatagramConnectionImpl()");
    }

    /**
     * Registers for shutdown listener
     */
    private void setShutdownListener()
    {
        // Get the instance of ApplicationUtils.
        ApplicationUtils appUtils = ApplicationUtils.getInstance();

        // Get the name of the application.
        appUtils.addShutdownListener(new ShutdownListener()
        {
            public void shuttingDown()
            {
                try
                {
                    close();// Do cleaning...
                }
                catch (IOException ex)
                {
                    // catch the exception and call dispose
                }
                if (iNativePeerHandle != 0)
                {
                    _dispose(iNativePeerHandle);
                    iNativePeerHandle = 0;
                }
            }

        });
    }
    /**
     * Get the local socket address of this UDPDatagram connection.
     *
     * @return the local address
     * @throws IOException in case of any IO errors
     */

    public String getLocalAddress() throws IOException
    {
        String[] address = new String[1];
        int retVal;
        if (!iConnectionOpen)
        {
            throw new IOException("getLocalAddress failed: connection is already closed");
        }
        int apId = -1;
        int apType = -1;
        try
        {

            Uid appSuite = ApplicationInfo.getInstance().getSuiteUid();
            iCmInstance = ConnectionManager.getInstance();
            AccessPoint apn = iCmInstance.getApplicationDefault(appSuite);
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
            }
        }
        catch (Exception e)
        {

        }
        retVal = _getLocalAddress(iNativePeerHandle, address, apId, apType);
        if (retVal < 0)
        {
            throw new IOException("getLocalAddress method failed.Posix error code: " + retVal);
        }
        else
        {
            return address[0];
        }
    }

    /**
     * Get the local port of this UDPDatagram connection.
     *
     * @return the local port
     * @throws IOException in case of any IO errors
     */

    public int getLocalPort() throws IOException
    {
        int retVal;
        if (!iConnectionOpen)
        {
            throw new IOException("getLocalPort failed: connection is already closed");
        }
        retVal = _getLocalPort(iNativePeerHandle);
        if (retVal < 0)
        {
            throw new IOException("getLocalPort method failed.Posix error code: " + retVal);
        }
        else
        {
            return retVal;
        }
    }

    /**
     * Close the secure connection.
     *
     * @throws IOException in case of any IO errors
     */

    public synchronized void close() throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "+UDPDatagramConnectionImpl::Close()");
        if (iConnectionOpen == true)
        {
            iConnectionOpen = false;
            int ret = _close(iNativePeerHandle);
            if (ret != 0)
            {
                if (ret == -4)
                {
                    // if recieve is blocked when close is called
                    throw new InterruptedIOException();
                }
                else
                {
                    throw new IOException("close method failed.Posix error code: " + ret);
                }
            }
        }
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "-UDPDatagramConnectionImpl::Close()");
    }

    /**
     * Please refer JSR 118.
     */

    public int getMaximumLength() throws IOException
    {
        if (!iConnectionOpen)
        {
            throw new IOException("getMaximumLength failed: connection is already closed");
        }
        return MAX_DATAGRAM_SIZE;
    }

    /**
     * Please refer JSR 118.
     */

    public int getNominalLength() throws IOException
    {
        if (!iConnectionOpen)
        {
            throw new IOException("getNominalLength failed: connection is already closed");
        }
        return NOMINAL_DATAGRAM_SIZE;
    }

    /**
     * Please refer JSR 118.
     */

    public void send(Datagram aDgram) throws IOException
    {

        int retVal;
        if (!iConnectionOpen)
        {
            throw new IOException("send failed: connection is already closed");
        }
        String addr = aDgram.getAddress();
        UrlParser url1;
        if (addr == null)
        {
            url1 = iUri; // address from the connector.open uri
        }
        else
        {
            url1 = new UrlParser(addr);

            if (url1.getPort() == 9200 || url1.getPort() == 9201 || url1.getPort() == 9202 || url1.getPort() == 9203)
            {
                if ((ApplicationInfo.getInstance().getProtectionDomain().equals(ApplicationInfo.UNIDENTIFIED_THIRD_PARTY_DOMAIN)) == true)
                {
                    Logger.LOG(Logger.ESOCKET, Logger.EInfo,"Throwing secuirty exception for restricted port");
                    throw new SecurityException("Unable to open datagram connection: port " +url1.getPort()+ " is restricted");
                }
            }
        }

        retVal = _send(iNativePeerHandle, aDgram.getData(), aDgram
                       .getOffset(), aDgram.getLength(), url1.host, url1.port);

        if (retVal < 0)
        {
            throw new IOException("send method failed.Posix error code: " + retVal);
        }

    }

    /**
     * Please refer JSR 118.
     */

    public void receive(Datagram aDgram) throws IOException
    {
        String addr = new String("  ");
        String[] senderAddr = new String[1];
        int[] senderPort = new int[1];
        int port = 0;
        int retVal;

        if (!iConnectionOpen)
        {
            throw new IOException("receive failed: connection is already closed");
        }


        retVal = _receive(iNativePeerHandle, aDgram.getData(), aDgram
                          .getOffset(), aDgram.getLength(), senderAddr, senderPort);

        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "UDPDatagramConnectionImpl.java recieve returned : "+retVal);
        if (retVal < 0)
        {
            throw new IOException("receive method failed.Posix error code: " + retVal);
        }
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "addr = "
                   + senderAddr[0] + "port = " + senderPort[0]
                   + "recieve retval = " + retVal);
        StringBuffer recvurl = new StringBuffer();
        recvurl.append(DATAGRAM_PROTOCOL + "://");
        recvurl.append(senderAddr[0]);
        recvurl.append(":");

        recvurl.append(senderPort[0]);
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "recvurl = " + recvurl);
        aDgram.setAddress(recvurl.toString());
        aDgram.setLength(retVal);

    }

    /**
     * Please refer JSR 118.
     */

    public Datagram newDatagram(int aSize) throws IOException
    {
        return newDatagram(aSize, null);
    }

    /**
     * Please refer JSR 118.
     */
    public Datagram newDatagram(int aSize, String aAddr) throws IOException
    {
        if (aSize < 0)
        {
            throw new IllegalArgumentException("Invalid argument in newDatagram method");
        }
        byte[] buf = new byte[aSize];
        return newDatagram(buf, aSize, aAddr);
    }

    /**
     * Please refer JSR 118.
     */

    public Datagram newDatagram(byte[] aBuf, int aSize) throws IOException
    {
        return newDatagram(aBuf, aSize, null);
    }

    /**
     * Please refer JSR 118.
     */

    public Datagram newDatagram(byte[] aBuf, int aSize, String aAddr)
    throws IOException
    {
        DatagramImpl tmp = null;
        if (!iConnectionOpen)
        {
            throw new IOException("newDatagram failed: connection is already closed");
        }
        if (aBuf == null)
        {
            throw new IllegalArgumentException("Invalid argument in newDatagram method");
        }
        if (aAddr != null)
        {
            UrlParser urlvalidation = new UrlParser(aAddr);
            tmp = new DatagramImpl(aBuf, aSize, aAddr);
        }
        else
        {
            String tmpAddr = null;
            if (iUri != null)   // server datagram if iUri = null
            {
                if (iUri.host != null) // server datagram is host is null
                {
                    tmpAddr = iUri.toString();
                }
            }
            tmp = new DatagramImpl(aBuf, aSize, tmpAddr);
        }
        return tmp;
    }

    Finalizer registerforFinalization()
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "creating a datagram finalizer object ");
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
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "datagram doFinalize() called :");
        if (iConnectionOpen == true)
        {
            try
            {
                close();
            }
            catch (IOException e)
            {
                // catch the exception and call dispose
            }
        }
        if (iNativePeerHandle != 0)
        {
            _dispose(iNativePeerHandle);
            iNativePeerHandle = 0;
        }
    }
    // Native calls
    private native int _createNativePeer(String aUri, int aMode, String host,
                                         int port);
    private native int _openConnection(int iNativePeerHandle, int aType, int aApn, int[] retval);
    private native int _send(int iNativePeerHandle, byte[] buf, int offset,
                             int length, String host, int port);
    private native int _receive(int iNativePeerHandle, byte[] buf, int offset,
                                int length, String[] senderAddr, int[] senderPort);
    private native int _getLocalAddress(int iNativePeerHandle, String[] address, int aIapId, int aType);
    private native int _getLocalPort(int iNativePeerHandle);
    private native int _close(int iNativePeerHandle);
    private native void _dispose(int iNativePeerHandle);

}
