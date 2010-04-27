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


package com.nokia.mj.impl.https;

import com.nokia.mj.impl.http.HttpConnectionNative;
import java.io.IOException;
import javax.microedition.io.HttpsConnection;
import javax.microedition.io.SecurityInfo;
import javax.microedition.pki.CertificateException;
import javax.microedition.pki.Certificate;
import com.nokia.mj.impl.gcf.utils.URI;
import com.nokia.mj.impl.https.SecurityInfoImpl;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.connectionmanager.ConnectionManager;
import com.nokia.mj.impl.connectionmanager.AccessPoint;

public class HttpsConnectionNative extends HttpConnectionNative implements
        HttpsConnection
{
    // Constants defined natively in sslerr.h
    private static final int SSL_ERROR_BASE = -7400;
    private static final int SSL_ALERT_BASE = 100;
    private static final int KErrSSLAlertCertificateExpired = SSL_ERROR_BASE
            - SSL_ALERT_BASE - 45;
    private static final int KErrSSLAlertCertificateUnknown = SSL_ERROR_BASE
            - SSL_ALERT_BASE - 46;
    private static final int KErrSSLAlertUnknownCA = SSL_ERROR_BASE
            - SSL_ALERT_BASE - 48;

    // Constants defined here for the start and end of the SSL Alert error codes
    // defined in sslerr.h
    private static final int SSL_ERROR_RANGE_START = SSL_ERROR_BASE
            - SSL_ALERT_BASE;

    private static final int SSL_ERROR_RANGE_END = SSL_ERROR_RANGE_START - 100;

    protected final static int HTTPS_DEFAULT_PORT = 443;

    SecurityInfoImpl iSecurityInfo = null;

    int iMNativeSecureConnectionHandle = 0;

    static
    {
        try
        {
            Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                       "loading https from HttpsConnectionNative.java");
            Jvm.loadSystemLibrary("javahttps");
            Logger.LOG(Logger.ESOCKET, Logger.EInfo, "javahttps loaded");
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.ESOCKET, e.toString());
        }
    }

    public HttpsConnectionNative(int aNativeHttpSessionHandle, String aUri,
                                 int aMode, boolean aTimeouts, ConnectionManager cm,
                                 AccessPoint apnInfo, int aResponseTimeout) throws IOException
    {
        super(aNativeHttpSessionHandle, aUri, aMode, aTimeouts, cm, apnInfo, aResponseTimeout);
    }

    public int getPort()
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ getPort()");
        final String temp = iUri.getPort();
        if (temp.length() != 0)
        {
            try
            {
                final int result = Integer.parseInt(temp);
                Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- getPort() "
                           + result);
                return result;
            }
            catch (NumberFormatException ex)
            {
                // do nothing, return default port
            }
        }
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "- getPort() returns default port" + HTTPS_DEFAULT_PORT);
        return HTTPS_DEFAULT_PORT;
    }

    /**
         * Please refer to Jsr 118
         */
    public SecurityInfo getSecurityInfo() throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ getSecurityInfo()");
        ensureOpen("getSecurityInfo");
        ensureConnected();
        ensureResponse();
        if (iMNativeSecureConnectionHandle == 0)
        {
            // Need to get a pointer to the mixin memory address
            // otherwise the v-table get confused when we cast later on.
            final int handle = _getMSecureConnectionInfo(iNativeTransactionHande);
            if (handle != 0)
            {
                iMNativeSecureConnectionHandle = handle;
            }
            else
            {
                throw new IOException(
                    "Unable to open https connection, getting security info failed");
            }
        }

        if (iSecurityInfo == null)
        {
            Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                       "new SecurityInfoImpl(iMNativeSecureConnectionHandle) "
                       + iMNativeSecureConnectionHandle
                       + " iNativeTransactionHande "
                       + iNativeTransactionHande);
            iSecurityInfo = new SecurityInfoImpl(iMNativeSecureConnectionHandle);
        }

        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- getSecurityInfo()");
        return iSecurityInfo;

    }

    protected void throwIOException(String message) throws IOException
    {
        int error = 0;
        try
        {
            error = Integer.parseInt(message);
        }
        catch (NumberFormatException n)
        {
            // Not an ssl or native error throw the original IOException
            throw new IOException(message);
        }
        // Valid error code from native
        // Parse to see if it was an SSL error or a IOException
        checkError(error);
    }

    public int checkError(int aError) throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   ">>>>>>>> getSecurityInfo() checkError: " + aError);
        if (aError <= SSL_ERROR_RANGE_START && aError >= SSL_ERROR_RANGE_END)
        {
            final Certificate cert = null;// We dont not have a cert at this
            // point
            byte certError = 0;
            switch (aError)
            {
            case KErrSSLAlertCertificateExpired:
                certError = CertificateException.EXPIRED;
                break;
            case KErrSSLAlertCertificateUnknown:
            case KErrSSLAlertUnknownCA:
                certError = CertificateException.UNRECOGNIZED_ISSUER;
                break;
            default:
                certError = CertificateException.VERIFICATION_FAILED;
            }
            throw new CertificateException(
                "Invalid server certificate.Symbian os error code: "
                + aError, cert, certError);
        }
        throw new IOException("Symbian OS error code :" + aError);
    }

    private native int _getMSecureConnectionInfo(int aNativeTransactionHande);
}
