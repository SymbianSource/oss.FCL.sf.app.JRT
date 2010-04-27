/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.microedition.m2g.connection;

import java.io.IOException;
import java.io.InputStream;
import javax.microedition.io.StreamConnection;
import com.nokia.mj.impl.utils.Logger;


/**
 * Base connection proxy class
 */
public class M2GConnectionProxy
{
    //--------------------------------------------------
    // STATIC CONSTANTS
    //--------------------------------------------------
    // Error constants
    static final String RESOURCE_NOT_FOUND_ESTR =
        "Given resource is not found.";

    //--------------------------------------------------
    // VARIABLES
    //--------------------------------------------------
    protected String iBaseUrl             = null;
    protected String iSuffixUrl           = null;
    protected InputStream iInputStream        = null;
    protected StreamConnection iConnection      = null;
    protected M2GConnectionPolicy iConnectionPolicy = null;

    //--------------------------------------------------
    // METHODS
    //--------------------------------------------------

    /**
     * Ctor.
     */
    public M2GConnectionProxy()
    {
    }


    /**
     * Close the connection.
     * When a connection has been closed, access to any of its
     * methods that involve an I/O operation will cause an
     * IOException  to be thrown.
     * Closing an already closed connection has no effect.
     */
    public void close() throws IOException
    {
        if (iInputStream != null)
        {
            iInputStream.close();
            iInputStream = null;
        }
        if (iConnection != null)
        {
            iConnection.close();
            iConnection = null;
        }
        iConnectionPolicy = null;
    }

    /**
     * Get base url
     * @return Base url
     */
    public String getBaseUrl()
    {
        return iBaseUrl;
    }

    /**
     * Get the suffix url
     * @note The suffix is calculated in m2g.connection.file.M2GConnectionProxyImpl.open()
     * @return Suffix url including '?' if there is a suffix, and null otherwise
     */
    public String getSuffixUrl()
    {
        return iSuffixUrl;
    }

    /**
     * Get an input stream for a connection.
     * @return An input stream
       * @throws IOException If the input stream is null.
     */
    public InputStream getInputStream() throws IOException
    {
        if (iInputStream == null)
        {
            throw new IOException();
        }
        return iInputStream;
    }

    /**
     * Get a connection policy.
     * @return A connection policy
       * @throws IOException If the connection policy is null.
     */
    public M2GConnectionPolicy getConnectionPolicy() throws IOException
    {
        if (iConnectionPolicy == null)
        {
            throw new IOException();
        }
        return iConnectionPolicy;
    }


    /**
     * Open a connection to a file located in a jar file..
     * @param aTokenizer Url tokenizer
       * @throws IOException If an error occurs while loading a content.
       * @throws IllegalArgumentException If no resource is found according to an URL.
       */
    public void open(M2GUrlTokenizer aTokenizer) throws IOException
    {
        iBaseUrl    = aTokenizer.getBaseUrl();
        iSuffixUrl  = null; // can't have DRM mode within a JAR

        iInputStream =
            Runtime.getRuntime().getClass().getResourceAsStream(aTokenizer.getUrl());

        if (iInputStream == null)
        {
            Logger.ELOG(Logger.EJavaUI, RESOURCE_NOT_FOUND_ESTR);
            throw new IllegalArgumentException(RESOURCE_NOT_FOUND_ESTR);
        }
        iConnectionPolicy = M2GConnectionFactory.getDefaultPolicy();
    }
}
