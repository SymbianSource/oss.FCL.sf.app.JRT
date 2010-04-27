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


package com.nokia.mj.impl.http;

import javax.microedition.io.SocketConnection;
import java.io.OutputStream;
import java.io.InputStream;

/**
 * Socket Element calss, used to store the tcp connection, which will be later
 * used in case of persistent connection
 *
 * @version 1.1
 * @author Nokia Corporation
 */
class SocketElement
{
    private InputStream mInpStr;

    private OutputStream mOutStr;

    private SocketConnection mSockObj;

    private boolean mIsProxy = false;

    private boolean mIsActive = false;

    /**
     * Constructs the SocketElement.
     *
     * @param aSockObject
     *            the SocketConnection object
     * @param aInputStream
     *            the InputStream object of this socket connection
     * @param aOutputStream
     *            the OutputStream object of this socket connection.
     */
    SocketElement(SocketConnection aSockObject, InputStream aInputStream,
                  OutputStream aOutputStream, boolean aIsProxy)
    {
        mInpStr = aInputStream;
        mOutStr = aOutputStream;
        mSockObj = aSockObject;
        mIsProxy = aIsProxy;
    }

    /**
     * Get the input stream object of this socket object
     *
     * @return inputstream object
     */
    public InputStream get_input_stream()
    {
        return mInpStr;
    }

    /**
     * Get the proxy flag of this socket object
     *
     * @return proxy flag
     */
    public boolean get_Proxy_Flag()
    {
        return mIsProxy;
    }

    /**
     * Get the outstream stream object of this socket object
     *
     * @return outstream object
     */
    public OutputStream get_output_stream()
    {
        return mOutStr;
    }

    /**
     * Get the socket object
     *
     * @return socket object
     */
    public SocketConnection get_socket_object()
    {
        return mSockObj;
    }
    /**
     * sets flag if the socket object is in use or not
     * true - if socket object in use
     * false - when socket object is not in use
     */
    public void setIsActiveFlag(boolean flag)
    {
        mIsActive = flag;
    }

    /**
     * returns whether the socket object is in use or not
     * true - if socket object in use
     * false - when socket object is not in use
     */
    public boolean getIsActiveFlag()
    {
        return mIsActive;
    }
}
