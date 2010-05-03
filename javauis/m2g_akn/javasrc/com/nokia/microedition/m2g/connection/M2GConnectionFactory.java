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
import javax.microedition.m2g.ExternalResourceHandler;
import com.nokia.microedition.m2g.connection.common.M2GConnectionPolicyImpl;

/**
 * Connection factory
 */
public class M2GConnectionFactory
{
    //--------------------------------------------------
    // STATIC CONSTANTS
    //--------------------------------------------------
    public static final String COMMON_PROTOCOL =
        "common";
    public static final String FILE_PROTOCOL =
        "file";
    public static final String CONNECTION_BASE_PACKAGE =
        "com.nokia.microedition.m2g.connection.";
    public static final String CONNECTION_PROXY_CLASS_NAME =
        ".M2GConnectionProxyImpl";


    //--------------------------------------------------
    // METHODS
    //--------------------------------------------------
    /**
     * Ctor
     */
    private M2GConnectionFactory()
    {
    }

    /**
     * Create a connection proxy.
     * If a url is not absolute or if it's beginig with
     * the slash mark then a resource is considered to be opened from a jar file.
     * @param aUrl Locator
     * @return Connection proxy
       * @throws IOException if an io error occurs
       * @throws NullPointerException if the locator is null.
       * @throws IllegalArgumentException if the locator is of unsupported type.
     */
    public static M2GConnectionProxy create(String aUrl) throws IOException
    {
        try
        {
            M2GConnectionProxy proxy = null;
            M2GUrlTokenizer tokenizer = new M2GUrlTokenizer(aUrl);

            // If a url begins with the slash mark then
            // a resource is tryed to open from a jar file.
            if (!M2GUrlTokenizer.isAbsolutURL(aUrl) ||
                    tokenizer.beginWithSlash())
            {
                proxy = new M2GConnectionProxy();
            }
            else
            {
                Class proxyClass =
                    Class.forName(parseClassName(tokenizer.getProtocol()));
                proxy = (M2GConnectionProxy)proxyClass.newInstance();
            }
            proxy.open(tokenizer);
            return proxy;
        }
        catch (InstantiationException e)
        {
            throw new IllegalArgumentException("Internal error: " + e.getMessage());
        }
        catch (IllegalAccessException e)
        {
            throw new IllegalArgumentException("Internal error: " + e.getMessage());
        }
        catch (ClassNotFoundException e)
        {
            throw new IllegalArgumentException("Internal error: " + e.getMessage());
        }
    }

    /**
     * Get default external resource handler
     * @param aUrl URL
     * @return external resource handler
     */
    public static ExternalResourceHandler getExternalResourceHandler(String aUrl)
    {
        return new M2GDefaultExternalResourceHandler(aUrl);
    }

    /**
     * Resolve external resource URL
     * @param aTokenizer Tokenizer
     * @param aUrl URL
     * @return connection proxy
     * @throws IOException
     */
    public static M2GConnectionProxy resolveExternalResourceUrl(
        M2GUrlTokenizer aTokenizer, String aUrl) throws IOException
    {
        // Check if the URL is relative.
        if (!M2GUrlTokenizer.isAbsolutURL(aUrl))
        {
            if (aTokenizer.getBaseUrl() != null)
            {
                aUrl = aTokenizer.resolveUrl(aUrl);
            }
        }

        return M2GConnectionFactory.create(aUrl);
    }

    /**
     * Get default policy
     * @return Connection policy
     */
    public static M2GConnectionPolicy getDefaultPolicy()
    {
        return new M2GConnectionPolicyImpl();
    }

    /**
       * Parses a connection proxy class name.
       * @param aProtocol Protocol
       * @return Connection proxy class name
       */
    private static String parseClassName(String aProtocol)
    {
        if (aProtocol.equals(FILE_PROTOCOL))
        {
            return
                CONNECTION_BASE_PACKAGE + FILE_PROTOCOL + CONNECTION_PROXY_CLASS_NAME;
        }
        else
        {
            return
                CONNECTION_BASE_PACKAGE + COMMON_PROTOCOL + CONNECTION_PROXY_CLASS_NAME;

        }
    }
}
