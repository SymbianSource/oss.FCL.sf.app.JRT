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


package javax.microedition.io;


import java.io.IOException;
import javax.microedition.io.ContentConnection;


public interface HttpConnection extends ContentConnection
{

    public final static String HEAD = "HEAD";
    public final static String GET = "GET";
    public final static String POST = "POST";
    public final static String PUT = "PUT"; //from midp 3.0
    public final static String DELETE = "DELETE"; //from midp 3.0
    public static final int HTTP_OK = 200;
    public static final int HTTP_CREATED = 201;
    public static final int HTTP_ACCEPTED = 202;
    public static final int HTTP_NOT_AUTHORITATIVE = 203;
    public static final int HTTP_NO_CONTENT = 204;
    public static final int HTTP_RESET = 205;
    public static final int HTTP_PARTIAL = 206;
    public static final int HTTP_MULT_CHOICE = 300;
    public static final int HTTP_MOVED_PERM = 301;
    public static final int HTTP_MOVED_TEMP = 302;
    public static final int HTTP_SEE_OTHER = 303;
    public static final int HTTP_NOT_MODIFIED = 304;
    public static final int HTTP_USE_PROXY = 305;
    public static final int HTTP_TEMP_REDIRECT = 307;
    public static final int HTTP_BAD_REQUEST = 400;
    public static final int HTTP_UNAUTHORIZED = 401;
    public static final int HTTP_PAYMENT_REQUIRED = 402;
    public static final int HTTP_FORBIDDEN = 403;
    public static final int HTTP_NOT_FOUND = 404;
    public static final int HTTP_BAD_METHOD = 405;
    public static final int HTTP_NOT_ACCEPTABLE = 406;
    public static final int HTTP_PROXY_AUTH = 407;
    public static final int HTTP_CLIENT_TIMEOUT = 408;
    public static final int HTTP_CONFLICT = 409;
    public static final int HTTP_GONE = 410;
    public static final int HTTP_LENGTH_REQUIRED = 411;
    public static final int HTTP_PRECON_FAILED = 412;
    public static final int HTTP_ENTITY_TOO_LARGE = 413;
    public static final int HTTP_REQ_TOO_LONG = 414;
    public static final int HTTP_UNSUPPORTED_TYPE = 415;
    public static final int HTTP_UNSUPPORTED_RANGE = 416;
    public static final int HTTP_EXPECT_FAILED = 417;
    public static final int HTTP_INTERNAL_ERROR = 500;
    public static final int HTTP_NOT_IMPLEMENTED = 501;
    public static final int HTTP_BAD_GATEWAY = 502;
    public static final int HTTP_UNAVAILABLE = 503;
    public static final int HTTP_GATEWAY_TIMEOUT = 504;
    public static final int HTTP_VERSION = 505;

    public long getDate() throws IOException;

    public long getExpiration() throws IOException;

    public String getFile();

    public String getHeaderField(String name) throws IOException;

    public String getHeaderField(int n)  throws IOException;

    public long getHeaderFieldDate(String name, long def) throws IOException;

    public int getHeaderFieldInt(String name, int def) throws IOException;

    public String getHeaderFieldKey(int n) throws IOException;

    public String getHost();

    public long getLastModified() throws IOException;

    public int getPort();

    public String getProtocol();

    public String getQuery();

    public String getRef();

    public String getRequestMethod();

    public String getRequestProperty(String key);

    public int getResponseCode() throws IOException;

    public String getResponseMessage() throws IOException;

    public String getURL();

    public void setRequestMethod(String method) throws IOException;

    public void setRequestProperty(String key, String value) throws IOException;

}

