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

import com.nokia.mj.impl.ssl.SecureConnectionImpl;

import java.io.InputStream;
import java.io.OutputStream;
import java.util.Vector;
import com.nokia.mj.impl.utils.Logger;

/**
 * SecureSocket Pool calss, used to store tcp connections, which will be later
 * used in case of persistent connection
 */
class SecureConnectionPool
{
    int max;

    private Vector mHosts = null;

    private Vector mHttpsElements = null;

    /**
     * Constructs the SecureSocketPool.
     *
     * @param m
     *            the maximum number of tcp connections that can be stored
     */
    SecureConnectionPool(int m)
    {
        max = m;
        mHosts = new Vector();
        mHttpsElements = new Vector();
    }

    /**
     * Add a new tcp connection into the securesocket pool
     *
     * @param aHost
     *            the host string
     * @param aSecureConnobj
     *            the secure tcp connection established to this host
     * @param aInputStream
     *            the input stream associated with this tcp connection
     * @param aOutputStream
     *            the output strem associated with this tcp connection
     */
    public void addConnection(String aHost,
                              SecureConnectionImpl aSecureConnobj, InputStream aInputStream,
                              OutputStream aOutputStream, boolean aIsProxy)
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "Adding a new socket connection to SecureConnectionPool: "
                   + aHost + ":os: " + aOutputStream + ":ins: "
                   + aInputStream);

        int index = mHosts.indexOf(aHost);
        if (index == -1)
        {
            mHosts.addElement(aHost);
            mHttpsElements.addElement(new HttpsElement(aSecureConnobj,
                                      aInputStream, aOutputStream, aIsProxy));
        }
        else
        {
            // Replace the old connection
            mHttpsElements.setElementAt(new HttpsElement(aSecureConnobj,
                                        aInputStream, aOutputStream, aIsProxy), index);
        }
    }

    /**
     * Remove the secure tcp connection associated to this host
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
            mHttpsElements.removeElementAt(index);
        }
    }

    /**
     * Get the secure tcp connection associated to this host
     *
     * @param aHost
     *            the host string
     * @return HttpsElement object
     */
    public HttpsElement getConnection(String aHost)
    {
        int index = mHosts.indexOf(aHost);
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "SecureConnectionPool getConnection called " + aHost);

        if (index > -1)
        {
            HttpsElement tmp = (HttpsElement) mHttpsElements.elementAt(index);
            return tmp;
        }
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "getConnection called returning null");
        return null;
    }
}