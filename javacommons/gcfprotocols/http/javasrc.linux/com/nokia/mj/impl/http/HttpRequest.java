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

import com.nokia.mj.impl.gcf.utils.UrlParser;
import java.util.Vector;

/**
 * Http class responsible for storing and sending the HTTP request message. It
 * stores HTTP headers as a map of key and value pair and a character buffer to
 * store the body of the request.
 */

public class HttpRequest extends HttpMessage
{

    private static final int INITIAL_SIZE = 10;

    // from midp 3.0
    protected static final String HTTP_VERSION_STRING = "HTTP/1.1";

    protected static final String POST = "POST"; // from midp 3.0

    protected static final String PUT = "PUT"; // from midp 3.0

    protected static final String DELETE = "DELETE"; // from midp 3.0

    /**
     * Constructs the HttpRequest
     */
    HttpRequest()
    {
    }

    /**
     * Creates and returns the HTTP/HTTPS request line. Example request line,
     * which will be returned.
     *
     * "GET http://www.google.com HTTP/1.1CRLF Host: www.google.comCRLF
     * Content-Length: 4CRLF CRLF"
     *
     * In the above string, CR(Carrige return) = '\r' LF(Line feed) = '\n'
     *
     * HttpClientConnectionImpl, which calls this function takes this request
     * line and appends it with the message body and sends it to the server
     *
     * @param aHttpurl
     *            the http url object, created from Connector.open() argument.
     * @param aRequestMethod
     *            the http request method. Possible values "GET" , "POST",
     *            "HEAD", "DELETE" ,"PUT"
     * @param aContentLength
     *            the content length (message body length) which will be added
     *            later. this is required because the request must contain the
     *            "Content-Length" header field.
     * @return the http request line as a string.
     */
    protected String createRequestLine(UrlParser aHttpurl,
                                       String aRequestMethod, String aUrlString, int aContentLength,
                                       boolean aIsProxyUsed)
    {
        String filename;
        int numberOfKeys;
        String queryData = null;
        StringBuffer postData = null;
        int queryLen = 0;
        String lenstr = null;
        String bodyData = null;
        StringBuffer headerbuf = new StringBuffer();

        numberOfKeys = numberOfElements();
        for (int i = 0; i < numberOfKeys; i++)
        {
            String key = (String) getNameAt(i);
            headerbuf.append(key);
            headerbuf.append(": ");
            headerbuf.append(getValueAt(i));
            headerbuf.append("\r\n");
        }

        StringBuffer requesttempLine = new StringBuffer(256);

        if (aRequestMethod.compareTo(POST) == 0)
        {
            queryData = aHttpurl.query;
            if (queryData != null)
            {
                postData = new StringBuffer(queryData);
                queryLen = postData.length();
            }
            if (aIsProxyUsed == true)
            {
                requesttempLine
                .append(aRequestMethod + " " + aHttpurl.scheme + ":"
                        + aUrlString + " " + HTTP_VERSION_STRING
                        + "\r\n");
            }
            else
            {
                requesttempLine.append(aRequestMethod + " " + aHttpurl.path
                                       + " " + HTTP_VERSION_STRING + "\r\n");
            }
            requesttempLine.append("Host: " + aHttpurl.host + ":"
                                   + aHttpurl.port + "\r\n");
            requesttempLine.append(headerbuf); // headers
            if (getElement("Content-Length") == null)
            {
                String conLen = Integer.toString(aContentLength);
                requesttempLine.append("Content-Length: " + conLen + "\r\n");
            }
            requesttempLine.append("\r\n");
        }
        else if (aRequestMethod.compareTo(PUT) == 0)
        {
            queryData = aHttpurl.query;
            if (queryData != null)
            {
                postData = new StringBuffer(queryData);
                postData.append("\r\n");
                queryLen = postData.length();
            }
            if (aIsProxyUsed == true)
            {
                requesttempLine
                .append(aRequestMethod + " " + aHttpurl.scheme + ":"
                        + aUrlString + " " + HTTP_VERSION_STRING
                        + "\r\n");
            }
            else
            {
                requesttempLine.append(aRequestMethod + " " + aHttpurl.path
                                       + " " + HTTP_VERSION_STRING + "\r\n");
            }
            requesttempLine.append("Host: " + aHttpurl.host + ":"
                                   + aHttpurl.port + "\r\n");
            requesttempLine.append(headerbuf); // headers
            if (getElement("Content-Length") == null)
            {
                int ct = queryLen + aContentLength;
                String conLen = Integer.toString(ct);
                requesttempLine.append("Content-Length: " + conLen + "\r\n");
            }
            requesttempLine.append("\r\n");
        }
        else if (aRequestMethod.compareTo(DELETE) == 0)
        {
            queryData = aHttpurl.query;
            if (queryData != null)
            {
                postData = new StringBuffer(queryData);
                postData.append("\r\n");
                queryLen = postData.length();
            }
            if (aIsProxyUsed == true)
            {
                requesttempLine
                .append(aRequestMethod + " " + aHttpurl.scheme + ":"
                        + aUrlString + " " + HTTP_VERSION_STRING
                        + "\r\n");
            }
            else
            {
                requesttempLine.append(aRequestMethod + " " + aHttpurl.path
                                       + " " + HTTP_VERSION_STRING + "\r\n");
            }
            requesttempLine.append("Host: " + aHttpurl.host + ":"
                                   + aHttpurl.port + "\r\n");
            requesttempLine.append(headerbuf); // headers
            if (getElement("Content-Length") == null)
            {
                int ct = queryLen + aContentLength;
                String conLen = Integer.toString(ct);
                requesttempLine.append("Content-Length: " + conLen + "\r\n");
            }
            requesttempLine.append("\r\n");
        }
        else
        {
            // GET and HEAD method
            if (aIsProxyUsed == true)
            {
                requesttempLine
                .append(aRequestMethod + " " + aHttpurl.scheme + ":"
                        + aUrlString + " " + HTTP_VERSION_STRING
                        + "\r\n");
            }
            else
            {
                queryData = aHttpurl.query;
                requesttempLine.append(aRequestMethod + " " + aHttpurl.path);
                if (queryData != null)
                {
                    requesttempLine.append("?" + queryData);
                }
                requesttempLine.append(" " + HTTP_VERSION_STRING + "\r\n");
            }
            requesttempLine.append("Host: " + aHttpurl.host);
            if ((aHttpurl.port != 80) && (aHttpurl.port != -1))
            {
                requesttempLine.append(":" + aHttpurl.port);
            }
            requesttempLine.append("\r\n");
            requesttempLine.append(headerbuf); // headers
            requesttempLine.append("\r\n");
        }
        return requesttempLine.toString();
    }
}
