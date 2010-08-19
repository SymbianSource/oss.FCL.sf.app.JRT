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

package com.nokia.mj.impl.smscbs.utils;

/**
 * Provides some utilities to: 1. Parse an URI to obtain it's main URI
 * components. 2. Concatenates a base URI with a relative URI as specified.
 *
 * @author Nokia Corporation
 */
public final class WmaUrl
{
    public static final boolean SERVER_CONNECTION = true;

    public static final boolean CLIENT_CONNECTION = false;

    private static final String SMS_PROTOCOL = "sms";

    private static final String CBS_PROTOCOL = "cbs";

    private static final String INVALID_URL = "Invalid URL: ";

    private static final int MAX_PORT = 65535;

    private static final int[] RESTRICTED_PORTS =
    {
        2805, // WAP WTA secure connection-less session service
        2923, // WAP WTA secure session service
        2948, // WAP Push connectionless session service (client side)
        2949, // WAP Push secure connectionless session service (client side)
        5502, // Service Card reader
        5503, // Internet access configuration reader
        5508, // Dynamic Menu Control Protocol
        5511, // Message Access Protocol
        5512, // Simple Email Notification
        9200, // WAP connectionless session service
        9201, // WAP session service
        9202, // WAP secure connectionless session service
        9203, // WAP secure session service
        9207, // WAP vCal Secure
        49996,// SyncML OTA configuration
        49999 // WAP OTA configuration
    };

    private int iPortVal;

    private String iHost;

    private String iPort;

    private String iAbsoluteUri;

    private String iProtocol;

    public WmaUrl(String aUri)
    {
        iAbsoluteUri = aUri;
    }

    public WmaUrl(String aUri, String aProtocol)
    {
        iAbsoluteUri = aProtocol + aUri;
    }

    /**
     * Method to retrieve the protocol
     * @return the uri string
     */
    public String getProtocol()
    {
        return iProtocol;
    }

    /**
     * Validates the Uri.
     * @throws IllegalArgumentException it uri is not valid
     */
    public void validateUri() throws IllegalArgumentException
    {
        int index = iAbsoluteUri.indexOf("://");
        int nameLength = iAbsoluteUri.length();
        if ((index == -1) || (index != iAbsoluteUri.indexOf(':'))
                || ((index + 3) >= nameLength))
        {
            throw new IllegalArgumentException(INVALID_URL.concat(iAbsoluteUri));
        }
        iProtocol = iAbsoluteUri.substring(0, index);
        if (iProtocol.compareTo(SMS_PROTOCOL) != 0
                && iProtocol.compareTo(CBS_PROTOCOL) != 0)
        {
            throw new IllegalArgumentException(INVALID_URL
                                               .concat(iAbsoluteUri));
        }
        String subUri = iAbsoluteUri.substring(index + 3, nameLength);
        nameLength = subUri.length();
        index = subUri.indexOf(':');
        if (index >= 0)
        {
            iHost = subUri.substring(0, index);
            iPort = subUri.substring(index + 1, nameLength);
        }
        else if (index < 0)
        {
            iHost = subUri;
        }
        if (0 != iHost.length())
        {
            if (0 == iProtocol.compareTo(CBS_PROTOCOL))
            {
                throw new IllegalArgumentException(INVALID_URL
                                                   .concat(iAbsoluteUri));
            }
            nameLength = iHost.length();
            validateHost(iHost, nameLength);
        }
        if ("".equals(iPort))
        {
            throw new IllegalArgumentException(INVALID_URL
                                               .concat(iAbsoluteUri));
        }
        if (iPort != null)
        {
            validatePort(iPort);
        }
    }

    public String getHost()
    {
        return iHost;
    }

    public String getPort()
    {
        return iPort;
    }

    public int getPortVal()
    {
        return iPortVal;
    }

    public String getAbsoluteUri()
    {
        return iAbsoluteUri;
    }
    /**
    * Returns boolean value indicating whether it is a
    * server/client connection.
    */
    public boolean isServerModeConnection()
    {
        if (iHost.length() == 0)
        {
            return SERVER_CONNECTION;
        }
        return CLIENT_CONNECTION;
    }
    /**
     * Validates the Port
     *
     * @param aPort -Port that to be validated
     * @throws IllegalArgumentException if the Port is invalid
     * @throws SecurityException if the Port is restricted Port
     * @see
     */
    private void validatePort(String aPort)
    {
        for (int p = 0; p < aPort.length(); p++)
        {
            if (!Character.isDigit(aPort.charAt(p)))
            {
                throw new IllegalArgumentException(INVALID_URL
                                                   .concat(iAbsoluteUri));
            }
        }
        iPortVal = Integer.parseInt(aPort);
        if (iPortVal > MAX_PORT || iPortVal < 0)
        {
            throw new IllegalArgumentException("Invalid Port: ".concat(aPort));
        }
    }

    public boolean isRestrictedPort()
    {
        final int aLength = RESTRICTED_PORTS.length;
        if (iProtocol != CBS_PROTOCOL)
        {
            for (int p = 0; p < aLength; p++)
            {
                if (iPortVal < RESTRICTED_PORTS[p])
                {
                    break;
                }
                if (iPortVal == RESTRICTED_PORTS[p])
                {
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * Validates the Host
     *
     * @param aHost - Host to be validated
     * @param aLength - length of the Host
     * @throws IllegalArgumentException if the Host is invalid
     * @see
     */
    private void validateHost(String aHost, int aLength)
    {
        char ch;
        for (int p = 0; p < aLength; p++)
        {
            ch = aHost.charAt(p);
            if (p == 0 && ch == '+')
            {
                continue;
            }
            else if (!Character.isDigit(ch))
            {
                throw new IllegalArgumentException(INVALID_URL
                                                   .concat(iAbsoluteUri));
            }
        }
    }

}