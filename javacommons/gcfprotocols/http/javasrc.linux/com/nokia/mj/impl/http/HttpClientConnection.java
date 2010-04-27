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

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.io.InputStream;
import javax.microedition.io.SocketConnection;
import javax.microedition.io.HttpConnection;
import com.nokia.mj.impl.gcf.utils.UrlParser;
import com.nokia.mj.impl.gcf.utils.DateUtil;
import com.ibm.oti.util.Util;
import com.nokia.mj.impl.rt.support.Finalizer;
import com.nokia.mj.impl.rt.support.JvmInternal; // to be removed when Finalizer is ready
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.gcf.protocol.socket.Protocol;
import com.nokia.mj.impl.socket.utils.SocketConnectionUtility;

/**
 *Http Implementation class - supports the midp 3.0 specification.
 */
public class HttpClientConnection implements HttpConnection
{
    private Finalizer mFinalizer;

    protected UrlParser mHttpurl;

    private HttpOutputStreamAdapter mOutputStream = null;

    private HttpInputStreamAdapter mInputStream = null;

    private SocketElement mSelm = null;

    private static final int DEFAULT_HTTP_PORT = 80;

    private static final int DEFAULT_HTTPS_PORT = 443;

    private int mybuf_index = 0;

    private int mybuf_length = 0;

    private static final int BUFFER_SIZE = 512;

    private byte[] myBuf = new byte[BUFFER_SIZE];

    private static SocketPool mSockpool;

    private String mStatusLine = null;

    private int mByt_rd_frmchunk = 0;

    protected InputStream mSocketInStream;

    protected OutputStream mSocketOutStream;

    protected SocketConnection mSockObj = null;

    protected static final String HTTP_VERSION_STRING = "HTTP/1.1";

    protected static final byte HTTP_SETUP = 0;

    protected static final byte HTTP_CONNECTED = 1;

    protected static final byte HTTP_SENDING = 2;

    protected static final byte HTTP_CLOSED = 3;

    protected String mUrlString;

    protected String mRequestMethod;

    protected StringBuffer mResponseBuffer;

    protected byte mHttpConnectionState = HTTP_SETUP;

    protected boolean mCloseFlag = false;

    protected boolean mChunkedFlag;

    protected boolean mSendChunkedReq;

    protected boolean mLastChunkSent = false;

    protected long mContentLength = -1;

    protected int msgBufindex = 0;

    protected boolean mOldSocketFlag = false;

    protected boolean mResentFlag = false;

    protected boolean mEof = false;

    protected int mChunkSize = 0;

    public boolean mInputStreamOpen = false;

    protected HttpRequest mHttpRequest = null;

    protected HttpResponse mHttpResponse = null;

    protected boolean mIsProxyUsed = false;

    protected boolean mOutputStreamOpen = false;

    static
    {
        int max = 2; // maximum number of persistent connection
        mSockpool = new SocketPool(max);
    }

    /**
     * Constructs the HttpClientConnection with default request method - GET.
     *
     * @param url -
     *            url string
     */
    public HttpClientConnection(String url)
    {
        // return an object of type httpconection
        // this method gets called when connector.open is called
        // parameter - url. store the url for further use
        // set default port - 80 and default method -GET

        // Logger.LOG(Logger.ESOCKET, Logger.EInfo,"HttpClientConnection()");
        mHttpurl = new UrlParser("http", url);
        mUrlString = url;
        if (mHttpurl.port == -1)
        {
            mHttpurl.port = DEFAULT_HTTP_PORT;
        }
        mRequestMethod = GET;
        mHttpRequest = new HttpRequest();
        mHttpResponse = new HttpResponse();
        mResponseBuffer = new StringBuffer();
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "  HttpClientConnection:  registering for finalization");
        mFinalizer = registerForFinalization();
    }

    Finalizer registerForFinalization()
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "creating a finalizer object this = " + this + "mSockObj = "
                   + mSockObj);
        return new Finalizer()
        {
            public void finalizeImpl()
            {
                doFinalize();
            }
        };

    }

    /**
     * This function is called by the runtime during finalization. all native
     * resources are released
     */
    public void doFinalize()
    {
        mSelm = mSockpool.getConnection(mHttpurl.host + mHttpurl.port);
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "Http doFinalize() called : this :" + this + "+mSockObj = "
                   + mSockObj);
        if (mSelm != null)
        {
            if (mSelm.getIsActiveFlag() != true)
            {
                Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                           "socket is active - hence not finalizing");
                mSockpool.removeConnection(mHttpurl.host + mHttpurl.port);
                try
                {
                    if (mSocketInStream != null)
                    {
                        mSocketInStream.close();
                        mSocketInStream = null;
                    }
                    if (mSocketOutStream != null)
                    {
                        mSocketOutStream.close();
                        mSocketOutStream = null;
                    }
                    if (mSockObj != null)
                    {
                        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                                   "Http doFinalize() called :sockobj:"+ mSockObj);
                        mSockObj.close();
                        mSockObj = null;
                    }
                }
                catch (Exception e)
                {

                }
            }
        }
    }

    /**
     * This function returns the proxy address.
     *
     */
    protected String getProxy()
    {
        int colon = 0;
        String proxyHost = null;
        String proxyPortString = null;
        int proxyPort = 0;
        String tmpproxy = System.getProperty("com.nokia.mj.impl.http.proxy");
        return tmpproxy;
    }

    /**
     * createSocketConnection method opens socket connection if not open.
     */
    public void createSocketConnection() throws IOException
    {
        String socketUrl = null;
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+createSocketConnection()");

        mSelm = mSockpool.getConnection(mHttpurl.host + mHttpurl.port);
        if (mSelm == null)
        {
            // if proxy present, connect to proxy or else make direct socket
            // connection to destination
            String proxyAddress = null;
            proxyAddress = getProxy();
            if (proxyAddress != null)
            {
                // if proxy is present connect to it
                socketUrl = "//" + proxyAddress;
                mIsProxyUsed = true;
            }
            else
            {
                socketUrl = "//" + mHttpurl.host + ":" + mHttpurl.port;
                mIsProxyUsed = false;
            }
            Logger.LOG(Logger.ESOCKET, Logger.EInfo, "socket conn to "
                       + socketUrl);
            SocketConnectionUtility sock1 = new SocketConnectionUtility(
                socketUrl);
            mSockObj = (SocketConnection) sock1.getSocketInstance();
            Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                       "before 1st setsocketoption");
            mSocketOutStream = mSockObj.openOutputStream();
        }
        else
        {
            Logger.LOG(Logger.ESOCKET, Logger.EInfo, "using persistent socket");
            mSockObj = mSelm.get_socket_object();
            mSocketOutStream = mSelm.get_output_stream();
            mIsProxyUsed = mSelm.get_Proxy_Flag();
            mSelm.setIsActiveFlag(true);
            mSocketInStream = mSelm.get_input_stream();
            mOldSocketFlag = true;
        }
        if (mSockObj != null)
        {
            Logger
            .LOG(Logger.ESOCKET, Logger.EInfo,
                 "socket creation success ");
        }
        else
        {
            // ideally mSockObj should not be null
            throw new IOException("Connection failed");
        }
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "--createSocketConnection()");
    }

    /**
     * getURL returns the URL passed to the Connector.open() function.
     *
     * @return URL
     */
    public String getURL()
    {
        // the stored url is returned
        return mHttpurl.scheme + ":" + mUrlString;
    }

    /**
     * returns the protocol name of this url eg : http or https.
     *
     * @return protocol
     */
    public String getProtocol()
    {
        return mHttpurl.scheme;
    }

    /**
     * returns the host name of the url for this Httpconnection eg : host name
     * or IPv4 address.
     *
     * @return host address
     */
    public String getHost()
    {
        return mHttpurl.host;
    }

    /**
     * returns the file portion of the url for this HttpConnection returns file
     * portion of the url or null if there is no file.
     *
     * @return file portion of url
     */
    public String getFile()
    {
        return mHttpurl.path;
    }

    /**
     * @return the ref portion of the url. usually after the # mark null is
     *         returned if no value.
     */
    public String getRef()
    {
        return mHttpurl.fragment;
    }

    /**
     * @return the query portion of the url RFC 2396 defines the quesry
     *         component as the text after the forst question mark(?) in the
     *         url. null is returned if no value
     */
    public String getQuery()
    {
        return mHttpurl.query;
    }

    /**
     * returns the network port number in the url for this HttpConnection
     * returns the defalut port number (80) if no port number was passed in the
     * url string to connector.open.
     *
     * @return port
     */
    public int getPort()
    {
        return mHttpurl.port;

    }

    /**
     * returns the http request method - GET/POST/HEAD/PUT/DELETE default method -
     * GET.
     *
     * @return request method
     */
    public synchronized String getRequestMethod()
    {
        return mRequestMethod;

    }

    /**
     * returns the value of the named request property key identifies the named
     * request property whose value is returned if key does not exist null is
     * returned.
     *
     * @param key
     * @return request property
     */
    public synchronized String getRequestProperty(String key)
    {
        return mHttpRequest.getElement(key);
    }

    /**
     * set the HTTP method for the url - GET/POST/HEAD/PUT/DELETE.
     *
     * @param request
     *            method
     * @throws IOException
     *             if it is in connected state
     */
    public synchronized void setRequestMethod(String method) throws IOException
    {
        if (method == null)
        {
            throw new NullPointerException();
        }
        throwIfClosed();
        if (mHttpConnectionState != HTTP_SETUP)
        {
            throw new IOException("Illegal state to call setRequestMethod");
        }
        if (!((method.equals(GET)) || (method.equals(POST))
                || (method.equals(HEAD)) || (method.equals(PUT)) || (method
                        .equals(DELETE))))
        {
            throw new IOException("setRequestMethod : Unsupported HTTP method"
                                  + method);
        }

        if (mOutputStreamOpen)
        {
            // After an output stream has been opened by the openOutputStream or
            // openDataOutputStream
            // attempts to change the request parameters are ignored.
            return;
        }
        mRequestMethod = method;
    }

    /**
     * set the HTTP request properties.
     *
     * @param key
     * @param value
     * @throw IOException if in connected state if the key already exists it
     *        should be overwritten with the new value Http requires that those
     *        properties which can multiple values, the values should be
     *        separated by comma for the same key which enable multiple
     *        propertied to be appended into a single property.
     */
    public synchronized void setRequestProperty(String key, String value)
    throws IOException
    {
        throwIfClosed();
        if (mHttpConnectionState != HTTP_SETUP)
        {
            throw new IOException("Illegal state to call setRequestProperty");
        }
        if ((key == null) || (value == null))
        {
            throw new IOException("Null string passed to setRequestProperty");
        }
        if (mOutputStreamOpen)
        {
            // After an output stream has been opened by the openOutputStream or
            // openDataOutputStream
            // attempts to change the request parameters are ignored.
            return;
        }
        String tmp = key.toLowerCase();
        if (tmp.equals("transfer-encoding"))
        {
            mSendChunkedReq = true;
        }
        mHttpRequest.setElement(key, value);
    }

    /*
     * the following methods cause the transition from setup to connected state.
     */

    /**
     * returns the HTTP response code parses the status line like HTTP/1.0 200
     * OK HTTP/1.0 401 Unauthorized and extracts the status code or -1 if no
     * status code can be discerned
     *
     * @return response code
     * @throws IOException
     *             if an error occurred connecting to server.
     */
    public synchronized int getResponseCode() throws IOException
    {
        // opensocket, sendrequest, readrequest
        // send read buffer to headerparser
        // LineParser.parseStatusLine(string)
        // return LineParser.statusCode
        throwIfClosed();
        getConnected();
        return mHttpResponse.getStatusCode();
    }

    /**
     * returns the response message from the HTTP response parses the status
     * line and extracts message like - OK, Unauthorized.
     *
     * @return message or null if message cant be discerned
     * @throws IOException
     *             if error occurred while connecting to server
     */
    public synchronized String getResponseMessage() throws IOException
    {
        throwIfClosed();
        getConnected();
        return mHttpResponse.getResponseMsg();
    }

    /**
     * returns the value of the expires header field it is the number of
     * miilisecons passed from jan 1, 1970 it is the expiration time for the
     * reource the url refers.
     *
     * @return value or 0 if not known
     * @throws IOException
     *             if error occurred while connecting to server
     */
    public synchronized long getExpiration() throws IOException
    {
        throwIfClosed();
        getConnected();
        final long result = getHeaderFieldDate("Expires", 0);
        return result;
    }

    /**
     * returns the value of the date header field or 0 if not known it is the
     * sending date of the resource the url references it is the number of
     * miilseconds passed since jan 1. 1970.
     *
     * @return date header field
     * @throws IOException
     *             if error occurred while connecting to server
     */
    public synchronized long getDate() throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "++Http getDate()");
        throwIfClosed();
        getConnected();
        final long result = getHeaderFieldDate("Date", 0);
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "--Http getDate()");
        return result;
    }

    /**
     * returns the value of last-modified header field it is the date of the
     * resource the url references when it was last modified.
     *
     * @return last-modified header field or 0 if not known
     * @throws IOException
     *             if error occurred while connecting to server
     */
    public synchronized long getLastModified() throws IOException
    {
        throwIfClosed();
        getConnected();
        final long result = getHeaderFieldDate("Last-Modified", 0);
        return result;
    }

    /**
     * Returns the value of the named header field.
     *
     * @param name -
     *            of a header field.
     * @return the value of the named header field, or null if there is no such
     *         field in the header.
     * @throws IOException -
     *             if an error occurred connecting to the server.
     */
    public synchronized String getHeaderField(String name) throws IOException
    {
        throwIfClosed();
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+getHeaderField()");
        getConnected();
        return mHttpResponse.getElement(name);
    }

    /**
     * Returns the value of the named field parsed as a number.
     *
     * @param name -
     *            the name of the header field.
     * @param def -
     *            the default value.
     * @return: the value of the named field, parsed as an integer or The def
     *          value is returned if the field is missing or malformed.
     * @throws IOException -
     *             if an error occurred connecting to the server.
     */
    public synchronized int getHeaderFieldInt(String name, int def)
    throws IOException
    {
        String value;
        throwIfClosed();
        getConnected();
        value = mHttpResponse.getElement(name);
        if (value == null)
        {
            return def;
        }
        else
        {
            int result;
            try
            {
                result = Integer.parseInt(value);
            }
            catch (Exception e)
            {
                result = def; // default value is returned
            }
            return result;
        }
    }

    /**
     * Gets a header field value by index.
     *
     * @param aIndex -
     *            the index of the header field
     * @return : the value of the nth header field or null if the array index is
     *         out of range. An empty String is returned if the field does not
     *         have a value
     * @throws IOException -
     *             if an error occurred connecting to the server.
     */
    public synchronized String getHeaderField(int aIndex) throws IOException
    {
        throwIfClosed();
        getConnected();
        if (aIndex >= mHttpResponse.numberOfElements() || aIndex < 0)
        {
            return null;
        }
        final String result = (String) mHttpResponse.getValueAt(aIndex);
        return result;
    }

    /**
     * Gets a header field key by index.
     *
     * @param aIndex -
     *            the index of the header field
     * @return: the key of the nth header field or null if the array index is
     *          out of range.
     * @throws IOException -
     *             if an error occurred connecting to the server
     */
    public synchronized String getHeaderFieldKey(int aIndex) throws IOException
    {
        throwIfClosed();
        getConnected();
        if (aIndex >= mHttpResponse.numberOfElements() || aIndex < 0)
        {
            return null;
        }
        final String result = (String) mHttpResponse.getNameAt(aIndex);
        return result;
    }

    /**
     * @return the length of the content which is being provided E.g. if the
     *         connection is via HTTP, then the value of the content-length
     *         header field is returned or -1 if it is not known.
     */
    public synchronized long getLength()
    {
        try
        {
            getConnected();
        }
        catch (Exception e)
        {
            Logger.WLOG(Logger.ESOCKET, "getLength() failed", e);
        }
        return mContentLength;
    }

    /**
     * Returns a string describing the encoding of the content which the
     * resource connected to is providing E.g. if the connection is via HTTP,
     * the value of the content-encoding header field is returned.
     *
     * @return encoding string used or null if not known
     */
    public String getEncoding()
    {
        try
        {
            getConnected();
        }
        catch (Exception e)
        {
            Logger.WLOG(Logger.ESOCKET, "getEncoding() failed", e);
        }
        return mHttpResponse.getElement("Content-Encoding");
    }

    /**
     * Returns the type of content that the resource connected to is providing
     * For instance, if the connection is via HTTP, then the value of the
     * content-type header field is returned. or null if it is not known
     */
    public synchronized String getType()
    {
        try
        {
            getConnected();
        }
        catch (Exception e)
        {
            Logger.WLOG(Logger.ESOCKET, "getType() failed, e");
        }
        return mHttpResponse.getElement("Content-Type");
    }

    /**
     * Open and return an input stream for a connection.
     *
     * @return InputStream
     * @throws IOExcpetion
     *             if an IO error occurs
     */
    public synchronized InputStream openInputStream() throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ openInputStream()");
        if (mHttpConnectionState != HTTP_CONNECTED)
        {
            getConnected();
        }
        throwIfClosed();
        if (mInputStream == null)
        {
            mInputStream = new HttpInputStreamAdapter(this);
            mInputStreamOpen = true;
        }
        else
        {
            throw new IOException("InputStream already opened");
        }
        return (mInputStream);
    }

    /**
     * Open and return a data input stream for a connection.
     *
     * @return DataInputStream
     * @throws IOExcpetion
     *             if an IO error occurs
     */
    public synchronized DataInputStream openDataInputStream()
    throws IOException
    {
        if (mHttpConnectionState != HTTP_CONNECTED)
        {
            getConnected();
        }
        throwIfClosed();
        if (mInputStream == null)
        {
            mInputStream = new HttpInputStreamAdapter(this);
            mInputStreamOpen = true;
        }
        else
        {
            throw new IOException("DataInputStream already opened");
        }
        return (new DataInputStream(mInputStream));
    }

    /**
     * Open and return an output stream for a connection.
     *
     * @return OutputStream
     * @throws IOExcpetion
     *             if an IO error occurs
     */
    public synchronized OutputStream openOutputStream() throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "++ Http::openOutputStream()");
        throwIfClosed();
        if (mOutputStream == null)
        {
            mOutputStream = new HttpOutputStreamAdapter(this);
            mOutputStreamOpen = true;
        }
        else
        {
            throw new IOException("OutputStream already opened");
        }
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "-- Http::openOutputStream()");
        return mOutputStream;
    }

    /**
     * Open and return a data output stream for a connection.
     *
     * @return DataOutputStream
     * @throws IOExcpetion
     *             if an IO error occurs
     */
    public synchronized DataOutputStream openDataOutputStream()
    throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "++ Http::openDataOutputStream()");
        throwIfClosed();
        if (mOutputStream == null)
        {
            mOutputStream = new HttpOutputStreamAdapter(this);
            mOutputStreamOpen = true;
        }
        else
        {
            throw new IOException("OutputStream already opened");
        }
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "-- Http::openDataOutputStream()");
        return (new DataOutputStream(mOutputStream));
    }

    /**
     * Throw an exception if the connection is close
     *
     * @throws IOExcpetion
     *             if an IO error occurs
     */
    void throwIfClosed() throws IOException
    {
        if (mHttpConnectionState == HTTP_CLOSED)
        {
            throw new IOException("Connection close");
        }
    }

    /**
     * closes the http connection.
     */
    public synchronized void close()
    {
        if (mHttpConnectionState == HTTP_CLOSED)
        {
            return;
        }
        mHttpConnectionState = HTTP_CLOSED;
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "Http close() called "
                   + mCloseFlag + ";;" + mEof + "this = " + this + " sockobj = "
                   + mSockObj);
        try
        {
            if ((mCloseFlag == false) && (mEof == true))
            {
                Logger.LOG(Logger.ESOCKET, Logger.EInfo, "Adding connection : "
                           + mHttpurl.host);
                if (mInputStream != null)
                {
                    mInputStream.close();
                }
                if (mOutputStream != null)
                {
                    mOutputStream.close();
                }
                mSockpool.addConnection((mHttpurl.host + mHttpurl.port),
                                        mSockObj, mSocketInStream, mSocketOutStream,
                                        mIsProxyUsed);
            }
            else
            {
                mSockpool.removeConnection(mHttpurl.host + mHttpurl.port);
                if (mSocketInStream != null)
                {
                    mSocketInStream.close();
                }
                if (mSocketOutStream != null)
                {
                    mSocketOutStream.close();
                }
                if (mSockObj != null)
                {
                    Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                               "Http::close(),calling Socket close");
                    mSockObj.close();
                }
                mSocketInStream = null;
                mSocketOutStream = null;
                mSockObj = null;
            }
        }
        catch (Exception e)
        {
            Logger.WLOG(Logger.ESOCKET,
                        "Error while closing the http connection", e);
        }

    }

    /**
     * creates and sends a chunked request to the server and reads response from
     * server after sending of last chunk.
     */
    public void sendChunkedRequest(boolean isLastChunk) throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "http:: sendChunkedRequest() isLastChunk = " + isLastChunk);
        try
        {
            if (isLastChunk == true)
            {
                Logger.LOG(Logger.ESOCKET, Logger.EInfo, "Sending last chunk "
                           + mLastChunkSent);

                // Send 0\r\n , marking end of chunked request
                if (mLastChunkSent == true)
                {
                    // already sent, either by os.close() or some get() call
                    return;
                }
                String t1 = "0;\r\n\r\n";
                mSocketOutStream.write(t1.getBytes());
                mSocketOutStream.flush();
                mLastChunkSent = true;
                return;
            }
            else
            {
                if (mHttpConnectionState != HTTP_SENDING)
                {
                    String requesttempLine = mHttpRequest.createRequestLine(
                                                 mHttpurl, mRequestMethod, mUrlString, 0,
                                                 mIsProxyUsed);
                    Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                               "\n chunked request sent:" + requesttempLine
                               + ":end");
                    mSocketOutStream.write(requesttempLine.getBytes());
                    mHttpConnectionState = HTTP_SENDING;
                }
                if (mOutputStream != null)
                {
                    if (mOutputStream.mDataBytes != null)
                    {
                        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                                   "inside bytarr");
                        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                                   " out stream size = "
                                   + mOutputStream.mDataBytes.length);
                        // write mChunkSize\r\ndata
                        String t2 = Integer
                                    .toString(mOutputStream.mDataBytes.length)
                                    + ";\r\n";
                        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                                   "\nRequest Body:\n" + t2
                                   + new String(mOutputStream.mDataBytes));
                        mSocketOutStream.write(t2.getBytes());
                        mSocketOutStream.write(mOutputStream.mDataBytes);
                        String t3 = "\r\n";
                        mSocketOutStream.write(t3.getBytes());
                    }
                }
                mSocketOutStream.flush();
            }
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.ESOCKET, Logger.EInfo, "outstream.write failed"
                       + e);
            if (mOldSocketFlag == true)
            {
                // if we have used an old tcp connection ( from connection pool)
                // and send fails, then send the second request afresh.
                resend_request();
            }
            else
            {
                Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                           "---------SendRequestLine()");
                throw new IOException(e.getMessage());
            }
        }
    }

    public void sendRequest() throws IOException
    {
        /**
         * creates request, sends request to server and reads response recieved
         * from server.
         */
        if (mHttpConnectionState == HTTP_CONNECTED)
        {
            // request has already been sent
            return;
        }
        mHttpConnectionState = HTTP_CONNECTED;
        if (mSockObj == null)
        {
            createSocketConnection();
        }

        if (mSendChunkedReq == true)
        {
            sendChunkedRequest(true);
        }
        else
        {
            // send request
            sendRequestLine();
        }
        if (mResentFlag == false)
        {
            if (mSocketInStream == null)
            {
                mSocketInStream = mSockObj.openInputStream();
            }
            readResponse(mSocketInStream);
        }
        // Logger.LOG(Logger.ESOCKET,
        // Logger.EInfo,"httpClientConnection-sendRequest()");

    }

    /**
     * send the request line eg : GET www.google.com HTTP/1.1.
     *
     * @throws IOException
     */
    public void sendRequestLine() throws IOException
    {
        int tmplen = 0;
        if (mOutputStream != null)
        {
            mOutputStream.copyToByteArray();
            if (mOutputStream.mDataBytes != null)
            {
                tmplen = mOutputStream.mDataBytes.length;
            }
        }
        String requesttempLine = mHttpRequest.createRequestLine(mHttpurl,
                                 mRequestMethod, mUrlString, tmplen, mIsProxyUsed);
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "\n request sent:"
                   + requesttempLine + ":end");
        try
        {
            mSocketOutStream.write(requesttempLine.getBytes());
            if (mOutputStream != null)
            {
                if (mOutputStream.mDataBytes != null)
                {
                    Logger.LOG(Logger.ESOCKET, Logger.EInfo, "inside bytarr");
                    Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                               " out stream size = "
                               + mOutputStream.mDataBytes.length);
                    mSocketOutStream.write(mOutputStream.mDataBytes);
                }
            }
            mSocketOutStream.flush();
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.ESOCKET, Logger.EInfo, "outstream.write failed"
                       + e);
            if (mOldSocketFlag == true)
            {
                resend_request();
            }
            else
            {
                Logger.LOG(Logger.ESOCKET, Logger.EInfo, "--SendRequestLine()");
                throw new IOException(e.getMessage());
            }
        }
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "---------SendRequestLine()");
    }

    /**
     * read the response from inputstream.
     *
     * @param is -
     *            inputstream
     */
    public void readResponse(InputStream is) throws IOException
    {
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo,"+ readResponse()");
        readStatusLine(is);
        if (mResentFlag == false)
        {
            readResponseHeaders(is);
        }
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo,"- readResponse()");
    }

    public void readResponseHeaders(InputStream is) throws IOException
    {
        // read response headers and body.
        int total = 0;
        int count = -1;
        try
        {
            int tmpcnt = 0;
            int bytesread = 0;
            String ln;
            while (true)
            {
                ln = readLine(is);
                if (ln == null)
                {
                    break;
                }
                mResponseBuffer.append(ln);
                mResponseBuffer.append("\r\n");
                bytesread++;
                tmpcnt++;
            }
            String headerBuf = mResponseBuffer.toString();
            parseResponse(headerBuf);
        }
        catch (Exception e)
        {
            Logger.WLOG(Logger.ESOCKET, "readResponseHeaders() failed", e);
        }
    }

    public void readStatusLine(InputStream is) throws IOException
    {
        // read statusline from server
        try
        {
            mStatusLine = readLine(is);
            Logger.LOG(Logger.ESOCKET, Logger.EInfo, "status line is "
                       + mStatusLine);
            if ((mStatusLine == null) && (mOldSocketFlag == true))
            {
                // If the CRLF of the previous response is not read, read it
                // read again to get the status line of the new response.

                mStatusLine = readLine(is);
                Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                           "status line in trial2 is " + mStatusLine);
            }
            if ((mStatusLine == null) && (mOldSocketFlag == true))
            {
                /*
                 * the second request has failed. reconstruct and send the
                 * second request on a new socket connection
                 */
                resend_request();
            }
            if (mStatusLine == null)
            {
                // server closed/terminated abnormally
                throw new IOException("Server error");
            }

            Logger.LOG(Logger.ESOCKET, Logger.EInfo, "status line is "
                       + mStatusLine);
        }
        catch (IOException e)
        {
            throw new IOException("No response received" + e.toString());
        }
        mHttpResponse.parseStatusLine(mStatusLine);
    }

    public String readLine(InputStream is) throws IOException
    {
        // read data line by line from inputstream.
        // read one line at a time from inputstream
        int c;
        StringBuffer resBuffer = new StringBuffer();
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo,"+readLine()");
        try
        {
            for (;;)
            {
                c = is.read();
                // Logger.LOG1(Logger.ESOCKET, Logger.EInfo,"+readLine()-c:"+c);
                if (c < 0)
                {
                    return null;
                }
                if (c == '\r')
                {
                    continue;
                }
                if (c == '\n')
                {
                    // Logger.LOG(Logger.ESOCKET,
                    // Logger.EInfo,"+readLine()-\n");
                    break;
                }
                resBuffer.append((char) c);
            }

            // Return a whole line and reset the string buffer.
            // Logger.LOG(Logger.ESOCKET,
            // Logger.EInfo,"+readLine()- resBuffer :" + resBuffer);
            String line;
            if (resBuffer.length() == 0)
            {
                line = null;
            }
            else
            {
                line = resBuffer.toString();
            }
            // Logger.LOG(Logger.ESOCKET,
            // Logger.EInfo,"+readLine()- responseline :" + line);
            return line;
        }
        catch (Exception e)
        {
            return null;
        }
    }

    public void parseResponse(String buf) throws IOException
    {
        HttpMessageParser msgParser = new HttpMessageParser();
        msgParser.parseHeader(buf, this);
    }

    public void resend_request() throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "resend request called ");
        mHttpConnectionState = HTTP_SETUP;
        mOldSocketFlag = false;
        mSockObj = null;
        mSockpool.removeConnection(mHttpurl.host + mHttpurl.port);
        sendRequest();
        mResentFlag = true;

    }

    /**
     * getHeaderFieldDate
     *
     * @param aName
     * @param aDfault
     * @return header field date
     * @throws IOException
     */
    public synchronized long getHeaderFieldDate(String aName, long aDfault)
    throws IOException
    {
        throwIfClosed();
        getConnected();
        String value;
        value = mHttpResponse.getElement(aName);
        if (value != null)
        {
            final long result = parseDate(value);
            return result;
        }
        else
        {
            return aDfault;
        }
    }

    protected long parseDate(String aDateString)
    {
        final long result = DateUtil.epocDifference(aDateString);
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "parseDate : result = "
                   + result);
        return result;
    }

    protected void getConnected() throws IOException
    {
        try
        {
            sendRequest();
        }
        catch (Exception e)
        {
            throw new IOException(e.getMessage());
        }
    }

    protected void getChunkSize()
    {
        // read new chunk size
        try
        {
            String tmplen = readLine(mSocketInStream);
            // Logger.LOG(Logger.ESOCKET, Logger.EInfo,"line = "+tmplen);
            if (tmplen == null)
            {
                tmplen = readLine(mSocketInStream);
            }
            int t = tmplen.indexOf(';'); // the line containing size may have
            // some extra stuff after ';' character
            if (t > 0)
            {
                tmplen = tmplen.substring(0, t);
            }
            mChunkSize = Integer.parseInt(tmplen, 16);

            Logger.LOG(Logger.ESOCKET, Logger.EInfo, "\nChunk_size = "
                       + mChunkSize);
        }
        catch (Exception e)
        {
            Logger.WLOG(Logger.ESOCKET, "Reading chunk size failed", e);
        }
    }

    protected synchronized int getBytes(byte[] aBytes, int aOffset, int aLength)
    throws IOException
    {
        int bytesRead = 0;
        while (aLength > 0)
        {
            int numRead = myRead(aBytes, aOffset, aLength);
            if (numRead <= 0)
            {
                if (bytesRead == 0)
                {
                    return -1;
                }
                break;
            }
            bytesRead += numRead;
            aOffset = aOffset + numRead;
            aLength = aLength - numRead;
        }

        return bytesRead;
    }

    protected int myRead(byte[] buf, int offset, int len) throws IOException
    {
        if (mybuf_index == mybuf_length)
        {
            // everything was read from the internal mybuffer
            mybuf_length = get_data_into_myBuffer();
            mybuf_index = 0;
        }
        if (mybuf_length == -1)
        {
            return -1;
        }

        int nBytes;

        if ((mybuf_index + len) <= mybuf_length)
        {
            nBytes = len;
        }
        else
        {
            nBytes = mybuf_length - mybuf_index;
        }
        System.arraycopy(myBuf, mybuf_index, buf, offset, nBytes);
        mybuf_index += nBytes;
        return nBytes;
    }

    protected int get_data_into_myBuffer() throws IOException
    {
        int rc = -1;
        int cnt;

        if (mByt_rd_frmchunk == mChunkSize)
        {
            // before getting the chunk_size read the CRLF present
            if (mChunkedFlag == false)
            {
                // Data was not chunked, we had assuemd the whole content as
                // one big chunk
                mEof = true;
                return -1;
            }
            getChunkSize();
            if (mChunkSize == 0)
            {
                mEof = true;
                return -1;
            }
            // This is a new chunk, nothing is read still
            mByt_rd_frmchunk = 0;
        }

        int chunk_remaining = mChunkSize - mByt_rd_frmchunk;
        int inputBufferSize = BUFFER_SIZE;
        // Logger.LOG(Logger.ESOCKET,
        // Logger.EInfo,"get_data_into_myBuffer calling stream base read : "+
        // chunk_remaining);
        if (inputBufferSize >= chunk_remaining)
        {
            rc = mSocketInStream.read(myBuf, 0, chunk_remaining);
        }
        else
        {
            rc = mSocketInStream.read(myBuf, 0, inputBufferSize);
        }
        // Logger.LOG(Logger.ESOCKET,
        // Logger.EInfo,"get_data_into_myBuffer return : "+ rc);

        // we now have read "rc" bytes from the chunk
        mByt_rd_frmchunk += rc;
        return rc;
    }
}
