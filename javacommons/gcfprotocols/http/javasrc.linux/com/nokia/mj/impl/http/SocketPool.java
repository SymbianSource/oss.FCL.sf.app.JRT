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
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Vector;

/**
 * Socket Pool calss, used to store the pool of tcp connections, which will be
 * later used in case of persistent connection
 *
 * @version 1.1
 * @author Nokia Corporation
 */

class SocketPool
{
    private int mMaxEle;

    private Vector mHosts;

    private Vector mSockElements;

    /**
     * Constructs the SocketPool.
     *
     * @param m
     *            the maximum number of tcp connections that can be stored
     */
    SocketPool(int aMax)
    {
        mMaxEle = aMax;
        mHosts = new Vector();
        mSockElements = new Vector();
    }

    /**
     * Add a new tcp connection into the socket pool
     *
     * @param aHost
     *            the host string
     * @param aSocketObject
     *            the tcp connection established to this host, which is to be
     *            stored.
     * @param aInputStream
     *            the input stream associated with this tcp connection
     * @param aOutputStream
     *            the output strem associated with this tcp connection
     */
    public void addConnection(String aHost, SocketConnection aSocketObject,
                              InputStream aInputStream, OutputStream aOutputStream,
                              boolean aIsProxy)
    {
        int index = mHosts.indexOf(aHost);
        if (index == -1)
        {
            mHosts.addElement(aHost);
            mSockElements.addElement(new SocketElement(aSocketObject,
                                     aInputStream, aOutputStream, aIsProxy));
        }
        else
        {
            mSockElements.setElementAt(new SocketElement(aSocketObject,
                                       aInputStream, aOutputStream, aIsProxy), index);
        }

    }

    /**
     * Remove the tcp connection associated to this host
     *
     * @param aHost
     *            the host string
     */
    public void removeConnection(String aHost)
    {
        int index = mHosts.indexOf(aHost);
        if (index != -1)
        {
            mHosts.removeElementAt(index);
            mSockElements.removeElementAt(index);
        }

    }

    /**
     * Get the tcp connection associated to this host
     *
     * @param aHost
     *            the host string
     * @return socketelement object
     */
    public SocketElement getConnection(String aHost)
    {
        int index = mHosts.indexOf(aHost);
        if (index > -1)
        {
            SocketElement tmp = (SocketElement) mSockElements.elementAt(index);
            return tmp;
        }
        return null;
    }
}
