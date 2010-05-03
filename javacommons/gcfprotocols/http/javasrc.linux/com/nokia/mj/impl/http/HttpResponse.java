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

import com.nokia.mj.impl.http.HttpMessageParser;
import java.io.IOException;

/**
 * Http class responsible for storing HTTP response message.
 */
public class HttpResponse extends HttpMessage
{
    private HttpStatusLine mStatusLine;

    HttpResponse()
    {
        mStatusLine = new HttpStatusLine();
    }

    /**
     * Parse the status line of the http resposne.
     *
     * @param aStatusLine
     *            the response status line.
     * @throws IOException
     *             incase of any errors
     */
    public void parseStatusLine(String aStatusLine) throws IOException
    {
        HttpMessageParser.parseStatusLine(aStatusLine, mStatusLine);
    }

    /**
     * Return the http response code.
     *
     * @return the response code
     */
    public int getStatusCode()
    {
        return mStatusLine.mStatusCode;
    }

    /**
     * Return the http response msg.
     *
     * @return the response msg
     */
    public String getResponseMsg()
    {
        return mStatusLine.mReasonPhrase;
    }
}