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


package com.nokia.mj.impl.http;

import java.io.IOException;
import com.nokia.mj.impl.utils.Logger;

/**
 * Simple MessageParser. Used to parse the Http response line.
 *
 * @version 1.1
 */
public class HttpMessageParser
{
    private static final String WHITESPACE = " ";
    private static final String TAB = "\t";
    private static final String LINEFEED = "\n";

    /**
     * Constructs the OutputStreamAdapter.
     */
    HttpMessageParser()
    {
    }

    /**
     * parse the protocol version.
     *
     * @param aHeaderBuffer
     *            the buffer which contains all the response headers.
     * @param aClientConn
     *            the http client connection object.
     * @throws IOException
     *             if stream is closed
     */
    public final void parseHeader(String aHeaderBuffer,
                                  HttpClientConnection aClientConn) throws IOException
    {
        int count = 0;
        // break down the response message into statusline, body.
        if (aHeaderBuffer == null)
        {
            throw new IOException("response message is null");
        }
        String temp1 = "";

        if (aHeaderBuffer.length() == 0)
        {
            Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                       "++Http parseHeader():no headers present in the response");
            return;
        }
        try
        {
            int x = 0;
            while (true)
            {
                int y = aHeaderBuffer.indexOf('\n', x);
                if ((y == -1) || (y == aHeaderBuffer.length() - 1))
                {
                    break;
                }
                temp1 = aHeaderBuffer.substring(x, y - 1);
                if ((aHeaderBuffer.charAt(y + 1) == ' ')
                        || (aHeaderBuffer.charAt(y + 1) == '\t'))
                {
                    int z = aHeaderBuffer.indexOf('\n', y + 1);
                    StringBuffer temp2 = new StringBuffer();
                    temp2.append(temp1);
                    temp2.append(aHeaderBuffer.substring(y, z - 1));
                    temp1 = temp2.toString();
                }

                x = y + 1;
                parseProperty(temp1, aClientConn);

                if (x >= aHeaderBuffer.length())
                {
                    break;
                }
            }
        }
        catch (IOException e)
        {

        }
    }

    /**
     * parse the protocol version.
     *
     * @param aHeader
     *            the buffer which contains a header in the form name:value.
     * @param aClientConn
     *            the http client connection object.
     * @throws IOException
     *             if stream is closed
     */
    public final void parseProperty(String aHeader,
                                    HttpClientConnection aClientConn) throws IOException
    {
        String line = aHeader;
        String key = null;
        String value = null;
        int colon = line.indexOf(':');
        if (colon == -1)
        {
            throw new IOException("Invalid header: " + aHeader);
        }

        // store the headers in lower case
        key = line.substring(0, colon).toLowerCase();
        if (key.length() == 0)
        {
            throw new IOException("Invalid header field with no key " + line);
        }
        if (line.length() <= colon + 1)
        {
            value = "";
        }
        else
        {
            value = line.substring(colon + 1).trim();
        }
        if ((key.equals("connection")) && (value.equals("close")))
        {
            // server indicated with this header that it is going to close the
            // connection.
            // set the flag, so that this connection will not be re-used.
            aClientConn.mCloseFlag = true;
        }
        if ((key.equals("transfer-encoding"))
                && (value.regionMatches(true, 0, "chunked", 0, value.length())))
        {
            // the response from the server is chunked.
            aClientConn.mChunkedFlag = true;
        }
        if (key.equals("content-length"))
        {
            aClientConn.mContentLength = Long.parseLong(value);
        }
        aClientConn.mHttpResponse.setElement(key, value);
    }

    /**
     * parse the status line.
     *
     * @param aRespline
     *            the status line to be parsed.
     * @param aStatusLine
     *            the result is stored into this object.
     * @throws IOException
     *             in case of any IO errors
     */
    public static final void parseStatusLine(String aRespline,
            HttpStatusLine aStatusLine) throws IOException
    {
        int fromIndex, toIndex, spaceIndex;
        int length = aRespline.length();

        if (length == 0)
        {
            throw new IOException("response may no be null");
        }
        fromIndex = 0;
        toIndex = length;
        spaceIndex = aRespline.indexOf(WHITESPACE, fromIndex);
        if (spaceIndex < 0)
        {
            throw new IOException("response does not have status code");
        }
        aStatusLine.mHttpVersion = aRespline.substring(fromIndex, spaceIndex);
        fromIndex = ignoreWhiteSpace(aRespline, spaceIndex);
        spaceIndex = aRespline.indexOf(WHITESPACE, fromIndex);

        if (spaceIndex == -1)
        {
            spaceIndex = toIndex; // if no response message is present
        }
        String temp = (String) aRespline.substring(fromIndex, spaceIndex);
        try
        {
            aStatusLine.mStatusCode = Integer.parseInt(temp);
        }
        catch (NumberFormatException e)
        {
            throw new IOException("response code is not a number ");
        }
        if (aStatusLine.mStatusCode < 100 || aStatusLine.mStatusCode >= 600)
        {
            throw new IOException("invalid status code");
        }
        // spaceIndex = aRespline.indexOf(WHITESPACE,);
        if (spaceIndex < toIndex)
        {
            fromIndex = ignoreWhiteSpace(aRespline, spaceIndex);
            aStatusLine.mReasonPhrase = aRespline.substring(fromIndex, toIndex);
        }
        else
        {
            aStatusLine.mReasonPhrase = "";
        }
    }

    /**
     * Skips the white space character present in the respline.
     *
     * @param aRespline
     *            the request line to be parsed.
     * @param aIndex
     *            the start index.
     * @return the next index.
     */
    public static final int ignoreWhiteSpace(String aRespline, int aIndex)
    {
        while ((aRespline.charAt(aIndex)) == ' ')
        {
            aIndex++;
        }
        return aIndex;
    }
}
