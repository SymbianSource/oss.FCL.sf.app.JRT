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


package com.nokia.mj.impl.ssl;

import java.io.IOException;
import javax.microedition.pki.CertificateException;
import javax.microedition.io.*;
import com.nokia.mj.impl.gcf.utils.UrlParser;
import com.nokia.mj.impl.gcf.utils.StreamConnectionBase;
import java.io.*;
// import com.nokia.mj.impl.socket.*;
import com.nokia.mj.impl.gcf.utils.DateUtil;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.rt.support.Finalizer;
import com.nokia.mj.impl.rt.support.ApplicationUtils;

import com.nokia.mj.impl.rt.ui.RuntimeUiFactory;
import com.nokia.mj.impl.rt.ui.RuntimeUi;
import com.nokia.mj.impl.rt.ui.ConfirmData;
import com.nokia.mj.impl.connectionmanager.ConnectionManager;
import com.nokia.mj.impl.connectionmanager.AccessPoint;
import com.nokia.mj.impl.utils.Id;
import com.nokia.mj.impl.utils.ResourceLoader;

/**
 * Secure Connection Implementation class.
 */

public class SecureConnectionImpl extends StreamConnectionBase implements
        SecureConnection
{
    private UrlParser iUri; // the uri of the secure connection

    private int iSocketDesc; // the tcp socket to be secured

    private Finalizer iFinalizer;

    private boolean iConnectionOpen = false;

    private static final int X509_V_OK = 0;

    private boolean iOutputStreamOpenedOnce = false;

    private boolean iInputStreamOpenedOnce = false;

    private boolean iDataOutputStreamOpenedOnce = false;

    private boolean iDataInputStreamOpenedOnce = false;

    private boolean iInput = false;

    private int iChoice = -1;

    private ConnectionManager iCmInstance = null;

    private AccessPoint iApn = null;

    private static final String SSL_RES_NAME = "javassl";

    private static final String LOCALISED_STRING_ID = "qtn_ssl_prompt_";

    private static final Id UNTRUSTED_CERTIFICATE_WARNING = new Id("untrusted_certificate", "N/A");

    /**
     * Constructs the SecureConnectionImpl.
     *
     * @param aUri
     *            the uri to which secure connection is opened
     * @param aMode
     *            the with which secure connection is opened
     * @param aSock
     *            the tcp socket, which has to be made secure
     * @throws IOException
     *             in case of any IO errors
     */
    public void showUI()
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "display ui called ");
        RuntimeUi rui = RuntimeUiFactory.getRuntimeUi();
        String[] ans = { "Yes", "No" };
        String localisedWarning = ResourceLoader.getInstance(SSL_RES_NAME,
                                  LOCALISED_STRING_ID).format(UNTRUSTED_CERTIFICATE_WARNING,
                                                              (Object[]) null);
        ConfirmData conf = new ConfirmData(localisedWarning, ans, 0);
        iInput = rui.confirm("SecureConnection", conf);
        iChoice = conf.getAnswer();
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "confirmdata returned value : " + iInput + "selected button :"
                   + iChoice);
    }

    public SecureConnectionImpl(UrlParser aUri, int aMode, int aSock,
                                ConnectionManager cm, AccessPoint apnInfo) throws IOException
    {

        super(aUri.toString(), aMode, false);
        int ret;
        int[] retval = new int[2];
        iCmInstance = cm;
        iApn = apnInfo;

        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "+SecureConnectionImpl::SecureConnectionImpl()");
        iSocketDesc = aSock;
        if (iApn != null)
        {
            ret = _doHandshake(iNativePeerHandle, iSocketDesc, iApn.getType(),
                               iApn.getNapId(), retval);
        }
        else
        {
            ret = _doHandshake(iNativePeerHandle, iSocketDesc, -1, -1, retval);
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
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "ssl connect return value ="
                   + ret);
        if (ret < 0)
        {
            throw new IOException(
                "Unable to open a secure connection. Posix error code: " + ret);
        }
        if (retval[1] < 0)
        {
            // validation of certificate failed
            iConnectionOpen = true;
            SecurityInfo tmpSecurityInfo = getSecurityInfo();
            throw new CertificateException(
                "Invalid server certificate. Symbian OS error code: "
                + retval[1],
                tmpSecurityInfo.getServerCertificate(),
                CertificateException.VERIFICATION_FAILED);
        }
        if (ret != X509_V_OK)
        {
            iConnectionOpen = true;
            if (ret == 18) // open c error code for self signed certificate
            {
                showUI();
                if (iInput == true)
                {
                    if (iChoice == 0) // "Yes" option selected
                    {
                        Logger
                        .LOG(Logger.ESOCKET, Logger.EInfo,
                             "SecureConnectionImpl()- untrusted certificate accepted");
                        // Continue
                    }
                    else
                    {
                        // "No" option selected
                        SecurityInfo tmpSecurityInfo = getSecurityInfo();
                        if (tmpSecurityInfo.getServerCertificate() == null)
                            Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                                       "tmpSecurityInfo.getServerCertificate() is null");
                        else
                            Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                                       "tmpSecurityInfo.getServerCertificate() is not null");
                        throw new CertificateException(
                            "Untrusted server certificate", tmpSecurityInfo
                            .getServerCertificate(),
                            CertificateException.VERIFICATION_FAILED); // untrusted
                        // certificate
                    }
                }
                else
                {
                    SecurityInfo tmpSecurityInfo = getSecurityInfo();
                    throw new CertificateException(
                        "Untrusted server certificate", tmpSecurityInfo
                        .getServerCertificate(),
                        CertificateException.VERIFICATION_FAILED); // untrusted
                    // certificate
                }
            }
            else
            {
                SecurityInfo tmpSecurityInfo = getSecurityInfo();
                throw new CertificateException("Untrusted server certificate",
                                               tmpSecurityInfo.getServerCertificate(),
                                               CertificateException.VERIFICATION_FAILED); // untrusted
                // certificate
            }
            /*
             * if(ret > 14) { ret = CertificateException.VERIFICATION_FAILED; }
             */
        }
        else
        {
            iConnectionOpen = true;
        }
        iFinalizer = createFinalizer();
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "--SecureConnectionImpl(), socketdescrip = " + iSocketDesc);
    }

    /**
     * Creatives the native side handle for this secure connection.
     *
     * @param aUri
     *            the uri to which secure connection is opened
     * @param aMode
     *            the with which secure connection is opened
     * @param aTimeOut
     *            the timeout value for this connection
     * @return the native handle
     */
    public int createNativePeer(String aUri, int aMode, boolean aTimeOut)
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "+SecureConnectionImpl::createNativePeer()");
        iUri = new UrlParser(aUri);
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "SecureConnectionImpl::createNativePeer() aUri = " + aUri
                   + "aMode = " + aMode + "iUri.host = " + iUri.host);
        int ret = _createNativePeer(aUri, aMode, iUri.host, iUri.port);
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "-SecureConnectionImpl::createNativePeer()");
        return ret;
    }

    /**
         * Please refer to Jsr 118
         */
    public String getAddress() throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "+SecureConnectionImpl::getAddress()");
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
                   "-SecureConnectionImpl::getAddress()");
        return addr[0];
    }

    /**
         * Please refer to Jsr 118
         */
    public String getLocalAddress() throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "+SecureConnectionImpl::getLocalAddress()");
        String addr[] = new String[1];
        if (iConnectionOpen == false)
        {
            throw new IOException(
                "getLocalAddress failed: connection is already closed");
        }
        int retValue = _getLocalAddress(iNativePeerHandle, addr);
        if (retValue < 0)
        {
            throw new IOException("getLocalAddress failed. POSIX error code: "
                                  + retValue);
        }
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "-SecureConnectionImpl::getLocalAddress()");
        return addr[0];
    }

    /**
         * Please refer to Jsr 118
         */
    public int getLocalPort() throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "+SecureConnectionImpl::getLocalPort()");
        if (iConnectionOpen == false)
        {
            throw new IOException(
                "getLocalPort failed: connection is already closed");
        }
        int localPort = _getLocalPort(iNativePeerHandle);
        if (localPort < 0)
        {
            throw new IOException("getLocalPort failed. POSIX error code: "
                                  + localPort);
        }
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "-SecureConnectionImpl::getLocalPort()");
        return localPort;
    }

    /**
         * Please refer to Jsr 118
         */
    public int getPort() throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "+SecureConnectionImpl::getPort()");
        if (iConnectionOpen == false)
        {
            throw new IOException("getPort failed: connection is already closed");
        }
        int result = _getport(iNativePeerHandle);
        if (result < 0)
        {
            throw new IOException("getPort failed. POSIX error code: " + result);
        }
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "-SecureConnectionImpl::getPort()");
        return result;
    }

    /**
         * Please refer to Jsr 118
         */
    public int getSocketOption(byte aOption) throws IllegalArgumentException,
                IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "+SecureConnectionImpl::getSocketOption()");
        if (iConnectionOpen == false)
        {
            throw new IOException(
                "getSocketOption failed: connection is already closed");
        }
        checkOption(aOption);
        int retVal = _getSocketOption(iNativePeerHandle, aOption);
        if (retVal < 0)
        {
            throw new IOException("getSocketOption failed. POSIX error code: "
                                  + retVal);
        }
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "-SecureConnectionImpl::getSocketOption()");

        return retVal;
    }

    /**
         * Please refer to Jsr 118
         */
    public void setSocketOption(byte aOption, int aValue)
    throws IllegalArgumentException, IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "+SecureConnectionImpl::setSocketOption()");
        if (iConnectionOpen == false)
        {
            throw new IOException(
                "setSocketOption failed: connection is already closed");
        }
        checkOption(aOption);

        if (aValue < 0)
        {
            throw new IllegalArgumentException(
                "Incorrect value for setSocketOption");
        }
        int retval = _setSocketOption(iNativePeerHandle, aOption, aValue);

        if (retval < 0)
        {
            throw new IOException("setSocketOption failed. POSIX error code: "
                                  + retval);
        }
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "-SecureConnectionImpl::setSocketOption()");
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
         * Please refer to Jsr 118
         */
    public SecurityInfo getSecurityInfo() throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "+SecureConnectionImpl::getSecurityInfo()");
        if (iConnectionOpen == false)
        {
            throw new IOException(
                "getSecurityInfo failed: connection is already closed");
        }
        String[] iStringArray = new String[9];
        long version = 0;
        int res = _getSecurityInfo(iNativePeerHandle, iStringArray);
        SecurityInfo securityInfo = new SecurityInfoImpl(iStringArray);
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "-SecureConnectionImpl::getSecurityInfo()");
        return securityInfo;
    }

    /**
         * Please refer to Jsr 118
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
         * Please refer to Jsr 118
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
         * Please refer to Jsr 118
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
         * Please refer to Jsr 118
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
         * Please refer to Jsr 118
         */

    public void close() throws IOException
    {
        Logger
        .LOG(Logger.ESOCKET, Logger.EInfo,
             "+SecureConnectionImpl::close() open flag = "
             + iConnectionOpen);
        if (iConnectionOpen == true)
        {
            iConnectionOpen = false;
            super.close(); // closes the input and outputstreams.

            // If streams is closed only then the native is closed
            if (isStreamsClosed() == true)
            {
                Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                           "+SecureConnectionImpl::close() calling _close "
                           + iConnectionOpen);
                int ret = _close(iNativePeerHandle);
                Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                           "-SecureConnectionImpl::close() = close returned "
                           + ret);
                if (ret != 0)
                {
                    throw new IOException("Posix error code : " + ret);
                }
            }
        }
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "-SecureConnectionImpl::close()");
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
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "securesocket doFinalize() called :");
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

    // Native calls

    private native int _createNativePeer(String aUri, int aMode, String aHost,
                                         int aPort);

    private native int _doHandshake(int iNativePeerHandle, int iSocketDesc,
                                    int type, int apn, int[] retval);

    //private native int _addCertificate(int iNativePeerHandle);

    private native int _getSecurityInfo(int iNativePeerHandle,
                                        String[] aStringArray);

    private native int _getAddress(int iNativePeerHandle, String[] aAddr);

    private native int _getLocalAddress(int iNativePeerHandle, String[] aAddr);

    private native int _getLocalPort(int iNativePeerHandle);

    private native int _getport(int iNativePeerHandle);

    private native int _setSocketOption(int iNativePeerHandle, int aOption,
                                        int aValue);

    private native int _getSocketOption(int iNativePeerHandle, int aOption);

    private native int _close(int iNativePeerHandle);

    private native void _dispose(int iNativePeerHandle);
}
