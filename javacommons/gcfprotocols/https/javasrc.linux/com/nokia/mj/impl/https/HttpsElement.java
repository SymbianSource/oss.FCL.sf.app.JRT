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

import javax.microedition.io.SocketConnection;
import java.io.OutputStream;
import java.io.InputStream;
import com.nokia.mj.impl.ssl.SecureConnectionImpl;
import com.nokia.mj.impl.utils.Logger;

/**
 * Https Element calss, used to store the secure connection(tcp connection).
 * which will be later used in case of persistent connection
 *
 * @version 1.1
 * @author Nokia Corporation
 */

class HttpsElement
{

    private InputStream mInputStream;

    private OutputStream mOutputStream;

    private SecureConnectionImpl mSecureObj;

    private boolean mIsProxy;

    /**
     * Constructs the HttpsElement.
     *
     * @param aSecureobj
     *            the SecureConnection object
     * @param aInputStream
     *            the InputStream object
     * @param aOutputStream
     *            the OutputStream object
     */

    HttpsElement(SecureConnectionImpl aSecureobj, InputStream aInputStream,
                 OutputStream aOutputStream, boolean aIsProxy)
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,"Creating a new https element " + aSecureobj + " : " + aInputStream
                   + " : " + aOutputStream);
        mInputStream = aInputStream;
        mOutputStream = aOutputStream;
        mSecureObj = aSecureobj;
        mIsProxy = aIsProxy;
    }

    /**
     * Get the input stream object of this secure connection
     *
     * @return inputstream object
     */

    public InputStream getSslInputStream()
    {
        return mInputStream;
    }

    /**
     * Get the proxy flag of this secure connection
     *
     * @return proxy flag
     */

    public boolean getSslProxyFlag()
    {
        return mIsProxy;
    }

    /**
     * Get the outstream stream object of this secure connection.
     *
     * @return outstream object
     */

    public OutputStream getSslOutputStream()
    {
        return mOutputStream;
    }

    /**
     * Get the secure connection object
     *
     * @return secure connection object
     */

    public SecureConnectionImpl getSecureConnObject()
    {
        return mSecureObj;
    }
}