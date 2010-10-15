/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;
import javax.microedition.io.HttpConnection;
import com.nokia.mj.impl.gcf.utils.URI;
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.gcf.utils.DateUtil;
import com.nokia.mj.impl.gcf.utils.NativeError;
import com.nokia.mj.impl.http.BlockingOperation;
import com.nokia.mj.impl.rt.support.Finalizer;
import com.nokia.mj.impl.connectionmanager.ConnectionManager;
import com.nokia.mj.impl.connectionmanager.AccessPoint;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.rt.support.ShutdownListener;

/**
 * An HttpConection class implementing an HTTP 1.1 client connection using the
 * native symbian http client ap
 *
 */
public class HttpConnectionNative implements HttpConnection,
        NativeHttpByteSource
{
    protected int iMode;
    protected int iState;
    protected boolean iClosed;
    protected URI iUri;
    protected String iRef;
    protected String iRequestMethod;
    protected Hashtable iRequestProperties;
    protected int iResponseCode;
    protected String iResponseMessage;
    protected Hashtable iReplyHeaders;
    protected Vector iReplyHeaderKeys;
    protected long iLength;
    protected DataInputStream iInputStream;
    protected boolean iInputStreamOpened = false;
    protected NativeHttpInputStream iNativeHttpInputStream;
    protected ByteArrayOutputStream iPostedDataStream;
    protected DataOutputStream iOutputStream;
    protected boolean iOutputStreamOpened = false;
    protected char[] iLineBuffer;
    protected int iLineBufferLength;
    protected int iLineLength;

    // States
    protected final static int INVALID_STATE = 0;
    protected final static int SETUP = 1;
    protected final static int CONNECTED = 2;
    protected final static int REQUEST_HEADERS_SENT = 3;
    protected final static int REQUEST_SENT = 4;
    protected final static int REPLY_RECEIVED = 5;
    protected final static int PARTIAL_REQUEST_SENT = 6;

    // Character constants
    protected final static char HASH = '#';
    protected final static int QUESTION_MARK = 63;
    protected final static int CR = 13;
    protected final static int LF = 10;
    protected final static int SP = 32;
    protected final static int EOF = -1;

    // Misc. Constants
    protected final static byte[] CRLF = { CR, LF };
    protected final static int DEFAULT_LINE_BUFFER_SIZE = 128;
    protected final static String VERSION_1_1 = "1.1";
    protected final static String VERSION_1_0 = "1.0";
    protected final static String NATIVESEPERATOR = ";;";
    protected final static String DEFAULT_PATH = "/"; // NOI8N
    protected final static int DEFAULT_PORT = 80;
    protected final static String PROTOCOL_SUFFIX = "://";

    // 'Well-known' Header names
    protected final static String CONNECTION = "Connection"; // NOI8N
    protected final static String CONTENT_ENCODING = "Content-Encoding"; // NOI8N
    protected final static String CONTENT_LENGTH = "Content-Length"; // NOI8N
    protected final static String CONTENT_TYPE = "Content-Type"; // NOI8N
    protected final static String DATE = "Date"; // NOI8N
    protected final static String HOST = "Host"; // NOI8N
    protected final static String LAST_MODIFIED = "Last-Modified"; // NOI8N
    protected final static String EXPIRES = "Expires"; // NOI8N
    protected final static String TRANSFER_ENCODING = "Transfer-Encoding"; // NOI8N
    protected final static String USER_AGENT = "User-Agent";

    // 'Well-known' Header values
    protected final static String UNTRUSTED_1_0 = "UNTRUSTED/1.0";

    // Connection value
    protected final static String CLOSE = "close"; // NOI8N

    // Http-native handle
    private static int iNativeHttpSessionHandle = 0;

    protected int iNativeTransactionHande = 0;
    final BlockingOperation iTransactionBlock;

    // Custom lock for synchronizing all read operations.
    protected final BlockingOperation iNativeDataReadyForRead;

    private Finalizer iFinalizer;
    protected ConnectionManager iCmInstance = null;
    protected AccessPoint iApn = null;
    private static boolean iIsSessionDeleted = false;
    private boolean iTrustedSuite = true;
    private int iRespTimeOut = -1;
    private int iRetries = 0;
    private int iRequestBodyLen = -1;
    private int iTotalPostDataLen = 0;
    private boolean iEndOfRequest = false;
    private boolean iChunkedTransfer = false;

    protected final BlockingOperation iPostDataBlock;

    static
    {
        try
        {
            Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                       "loading http from HttpConnectionNative.java");
            Jvm.loadSystemLibrary("javahttp");
            Logger.LOG(Logger.ESOCKET, Logger.EInfo, "javahttp loaded");
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.ESOCKET, e.toString());
        }
    }

    public HttpConnectionNative(int aNativeHttpSessionHandle, String aName,
                                int aMode, boolean aTimeouts, ConnectionManager cm,
                                AccessPoint apnInfo, int aResponseTimeout) throws IOException
    {
        Logger.PLOG(Logger.ESOCKET, "New HTTP connection to URL: " + aName);
        Logger.ILOG(Logger.ESOCKET, "+++++ HttpConnectionNative new, this ="
                    + this);

        int index;
        iCmInstance = cm;
        iApn = apnInfo;
        index = aName.indexOf(HASH);
        iRespTimeOut = aResponseTimeout; // setting of timeout value for http response.
        // specific scenario identified in case of java installer.

        // Handle to shared native RHttpSession
        if (iNativeHttpSessionHandle == 0)
        {
            // only one session per application
            creatHttpSession();
        }

        if (index != -1)
        {
            iRef = aName.substring(index + 1);
            aName = aName.substring(0, index);
        }

        iUri = new URI(aName);
        final String host = iUri.getHost();
        if (host == null || host.equals(""))
        {
            throw new IllegalArgumentException("Invalid url: " + aName);
        }

        iMode = aMode;
        iState = SETUP;
        iRequestMethod = GET;
        iRequestProperties = new Hashtable();
        iReplyHeaders = new Hashtable();
        iReplyHeaderKeys = new Vector();
        iLength = -1;

        // Blocking operations to allow async native transactions
        iTransactionBlock = new BlockingOperation();
        iNativeDataReadyForRead = new BlockingOperation();
        iNativeDataReadyForRead.setResult(BlockingOperation.BLOCKED);
        iPostDataBlock = new BlockingOperation();
        iFinalizer = registerForFinalization();
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- HttpConnectionNative new ");

        ApplicationInfo appInfo = ApplicationInfo.getInstance();
        String runtime = appInfo.getRuntimeType();
        boolean  midpRuntime = true;

        if (runtime == ApplicationInfo.RUNTIME_INSTALLER)
        {
            midpRuntime = false;
        }
        else
        {
            // add the untrusted header for untrusted midlet suite
            if ((ApplicationInfo.getInstance().getProtectionDomain()
                    .equals(ApplicationInfo.UNIDENTIFIED_THIRD_PARTY_DOMAIN)) == true)
            {
                iTrustedSuite = false;
                iRequestProperties.put(USER_AGENT, UNTRUSTED_1_0);
            }
        }
        // get the default User-Agent headers and add to hashtable
        final String agent = _getUserAgentHeaderValue(midpRuntime);
        if (agent != null)
        {
            if (iTrustedSuite == false)
            {
                String result = agent + ' ' + UNTRUSTED_1_0;
                iRequestProperties.put(USER_AGENT, result);
            }
            else
            {
                iRequestProperties.put(USER_AGENT, agent);
            }
        }
    }

    Finalizer registerForFinalization()
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "++HttpConnectionNative::registerForFinalization ");
        return new Finalizer()
        {
            public void finalizeImpl()
            {
                doFinalize();
            }
        };
    }


    public void doFinalize()
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "++HttpConnectionNative::doFinalize ");
        try
        {
            close();
        }
        catch (Exception e)
        {

        }
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "--HttpConnectionNative::doFinalize ");
    }

    public void creatHttpSession() throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "++HttpConnectionNative::creatHttpSession()");
        int[] retval = new int[2];
        int handle;
        int err;
        if (iApn != null)
        {
            handle = _createHttpSession(0, iApn.getType(), iApn.getNapId(),
                                        retval);
        }
        else
        {
            handle = _createHttpSession(0, -1, -1, retval);
        }
        iNativeHttpSessionHandle = NativeError.check(handle);
        if (retval[0] < 0)
        {
            throw new IOException(
                "Unable to open http connection.Creation of Native peer failed.");
        }
        if (retval[1] < 0)
        {
            // reset connection manager
            iCmInstance.reset();
        }

        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "--HttpConnectionNative::creatHttpSession()");
    }

    /**
     * Please refer to Jsr 118
     */
    public synchronized void close() throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "++++++++++ close()");
        if (iClosed)
        {
            return;
        }
        iClosed = true;
        if ((iInputStream == null) && (iOutputStream == null)
                && (iState != SETUP))
        {
            if (iNativeHttpInputStream != null)
            {
                iNativeHttpInputStream.close();
                iNativeHttpInputStream = null;
            }
            closeConnection();
        }
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "------- close()");
        return;
    }

    /**
     * Please refer to Jsr 118
     */
    public synchronized DataInputStream openDataInputStream()
    throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ openDataInputStream()");
        // prevents multiple read
        synchronized (iNativeDataReadyForRead.getLock())
        {
            ensureOpen("openDataInputStream");
            if (iInputStreamOpened)
            {
                throwIOException("Input stream is already opened");
            }
            ensureResponse();
            iInputStream = makeDataInputStream();
            iInputStreamOpened = true;
            // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "-
            // openDataInputStream()" );
            return iInputStream;
        }
    }

    /**
     * Please refer to Jsr 118
     */
    public synchronized InputStream openInputStream() throws IOException
    {
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ openInputStream()" );
        final InputStream result = openDataInputStream();
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- openInputStream()" );
        return result;
    }


    /**
     * Please refer to Jsr 118
     */

    public synchronized DataOutputStream openDataOutputStream()
    throws IOException
    {
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ openDataOutputStream()"
        // );
        ensureOpen("openDataOutputStream");
        if (iOutputStreamOpened)
        {
            throwIOException("Output stream is already opened");
        }
        makeDataOutputStream();
        iOutputStreamOpened = true;
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- openDataOutputStream()"
        // );
        return iOutputStream;
    }

    /**
     * Please refer to Jsr 118
     */
    public synchronized OutputStream openOutputStream() throws IOException
    {
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ openOutputStream()" );
        final OutputStream result = openDataOutputStream();
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- openOutputStream()" );
        return result;
    }

    /**
     * Please refer to Jsr 118
     */
    public synchronized String getEncoding()
    {
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ getEncoding()" );
        String result = null;
        try
        {
            result = getHeaderField(CONTENT_ENCODING);
        }
        catch (IOException ex)
        {
            // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- getEncoding() returns
            // null" );
            return null;
        }
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- getEncoding()" );
        return result;
    }

    /**
     * Please refer to Jsr 118
     */
    public synchronized long getLength()
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ getLength()");
        try
        {
            ensureOpen("getLength");
            ensureResponse();
            // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- getLength() : " +
            // iLength);
            return iLength;
        }
        catch (IOException ex)
        {
            // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- getLength() set -1
            // IOException: "+ ex.toString() );
            return -1;
        }
    }

    /**
     * Please refer to Jsr 118
     */
    public synchronized String getType()
    {
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ getType()" );
        String result = null;
        try
        {
            result = getHeaderField(CONTENT_TYPE);
        }
        catch (IOException ex)
        {
            // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- getType() returns
            // null" );
            // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- getType() NULL");
            return null;
        }
        //Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- getType()" + result);
        return result;
    }

    // HttpConnection interface implementation

    public synchronized long getDate() throws IOException
    {
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ getDate()" );
        final long result = getHeaderFieldDate(DATE, 0);
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- getDate()" );
        return result;
    }

    public synchronized long getExpiration() throws IOException
    {
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ getExpiration()" );
        final long result = getHeaderFieldDate(EXPIRES, 0);
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- getExpiration()" );
        return result;
    }

    public String getFile()
    {
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ getFile()" );
        String result = iUri.getPath();
        if (result.length() == 0)
            result = null;
        else
            result = "/" + result;
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- getFile: " + result );
        return result;
    }

    public synchronized String getHeaderField(int aIndex) throws IOException
    {
        ensureOpen("getHeaderField");
        ensureResponse();
        if (aIndex >= iReplyHeaderKeys.size() || aIndex < 0)
        {
            // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- getHeaderField(int)
            // returns null" );
            return null;
        }
        final String result = (String) iReplyHeaders.get(iReplyHeaderKeys
                              .elementAt(aIndex));
        if (result != null)
        {
            int length = result.length();
            String keys = (String) iReplyHeaderKeys.elementAt(aIndex);
            if (keys.equalsIgnoreCase("Set-Cookie") == true)
            {
                // first cookie is discarded
                if ((result.charAt(0)) == ',')
                {
                    throw new IOException(
                        "cookie size too large - hence discarded");
                }

                // last cookie is discarded
                if ((result.charAt(length - 2)) == ',')
                {
                    throw new IOException(
                        "cookie size too large - hence discarded");
                }

                int index = result.indexOf(", ,", 0);
                if (index != -1)
                {
                    throw new IOException(
                        "cookie size too large - hence discarded");
                }
            }
        }
        //Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- getHeaderField(int)" );
        //Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ getHeaderField(int): "  + result);
        return result;
    }

    public synchronized String getHeaderField(String aName) throws IOException
    {
        ensureOpen("getHeaderField");
        ensureResponse();
        if (aName == null)
        {
            // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "--
            // getHeaderField(String) returns null" );
            return null;
        }
        final String result = (String) iReplyHeaders.get(aName.toLowerCase());

        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ getHeaderField(String): "
                   + result);
        if (result != null)
        {
            int length = result.length();
            if (aName.equalsIgnoreCase("Set-Cookie") == true)
            {
                // first cookie is discarded
                if ((result.charAt(0)) == ',')
                {
                    throw new IOException(
                        "cookie size too large - hence discarded");
                }

                // last cookie is discarded
                if ((result.charAt(length - 2)) == ',')
                {
                    throw new IOException(
                        "cookie size too large - hence discarded");
                }

                int index = result.indexOf(", ,", 0);
                if (index != -1)
                {
                    throw new IOException(
                        "cookie size too large - hence discarded");
                }
            }
        }
        return result;
    }

    public synchronized long getHeaderFieldDate(String aName, long aDfault)
    throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "+ getHeaderFieldDate(String, long)");
        String value;

        value = getHeaderField(aName);
        if (value != null)
        {
            final long result = parseDate(value);
            Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                       "- getHeaderFieldDate(String, long)");
            return result;
        }
        else
        {
            Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                       "- getHeaderFieldDate(String, long) return aDfault");
            return aDfault;
        }
    }

    public synchronized int getHeaderFieldInt(String aName, int aDfault)
    throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "+ getHeaderFieldInt(String, int) , name = " + aName
                   + "default = " + aDfault);
        String value;

        value = getHeaderField(aName);
        if (value == null)
        {
            Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                       "- getHeaderFieldInt(String, int) returns aDfault");
            return aDfault;
        }
        else
        {
            try
            {
                final int result = Integer.parseInt(value);
                // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "-
                // getHeaderFieldInt(String, int)" );
                return result;
            }
            catch (NumberFormatException nfe)
            {
                //Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- getHeaderFieldInt (String, int): " + aDfault);
                return aDfault;
            }
        }
    }

    public synchronized String getHeaderFieldKey(int aIndex) throws IOException
    {
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ getHeaderFieldKey(int)"
        // );
        ensureOpen("getHeaderFieldKey");
        ensureResponse();
        if (aIndex >= iReplyHeaderKeys.size() || aIndex < 0)
        {
            // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "-
            // getHeaderFieldKey(int) returns null" );
            return null;
        }
        final String result = (String) iReplyHeaderKeys.elementAt(aIndex);
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- getHeaderFieldKey(int)"
        // );
        return result;
    }

    public String getHost()
    {
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ getHost()" );
        final String result = iUri.getHost();
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- getHost()" );
        return result;
    }

    public synchronized long getLastModified() throws IOException
    {
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ getLastModified()" );
        final long result = getHeaderFieldDate(LAST_MODIFIED, 0);
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- getLastModified()" );
        return result;
    }

    public int getPort()
    {
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ getPort()" );
        final String temp = iUri.getPort();
        if (temp.length() != 0)
        {
            try
            {
                final int result = Integer.parseInt(temp);
                // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- getPort()" );
                return result;
            }
            catch (NumberFormatException ex)
            {
                // do nothing, return default port
            }
        }
        //Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- getPort() returns default port" );
        return DEFAULT_PORT;
    }

    public String getProtocol()
    {
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ getProtocol()" );
        final String result = iUri.getProtocol();
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- getProtocol()" );
        return result;
    }

    public String getQuery()
    {
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ getQuery()" );
        String result = iUri.getQuery();
        if (result.length() == 0)
            result = null;
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- getQuery()" );
        return result;
    }

    public String getRef()
    {
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "* getRef()" );
        return iRef;
    }

    public final String getRequestMethod()
    {
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "* getRequestMethod()" );
        return iRequestMethod;
    }

    public final String getRequestProperty(String aKey)
    {
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+
        // getRequestProperty(String)" );
        final String result = (String) iRequestProperties.get(aKey);

        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "-
        // getRequestProperty(String)" );
        return result;
    }

    public synchronized final int getResponseCode() throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ getResponseCode()");
        ensureOpen("getResponseCode");
        ensureResponse();
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- getResponseCode() : "
                   + iResponseCode);
        return iResponseCode;
    }

    public synchronized String getResponseMessage() throws IOException
    {
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ getResponseMessage()" );
        ensureOpen("getResponseMessage");
        ensureResponse();
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- getResponseMessage() : "
        // + iResponseMessage);
        return iResponseMessage;
    }

    public String getURL()
    {
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ getURL()" );
        final String result = iUri.toString();
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- getURL()" );
        return result;
    }

    /**
         * Please refer to Jsr 118
         */
    public synchronized void setRequestMethod(String aMethod)
    throws IOException
    {
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ setRequestMethod(String)"
        // );
        try
        {
            if (aMethod == null)
            {
                throw new NullPointerException("Request method is null");
            }
            ensureOpen("setRequestMethod");
            if (iState != SETUP)
            {
                throwIOException("setRequestMethod method failed.Connection state is already connected");
            }
            if ((GET.equals(aMethod) == false)
                    && (HEAD.equals(aMethod) == false)
                    && (POST.equals(aMethod) == false))
            {
                throwIOException("Invalid or Unsupported request method: "
                                 + aMethod);
            }
            // ignore if an output stream has already been open
            if (iPostedDataStream == null)
                iRequestMethod = aMethod;
        }
        catch (IOException ex)
        {
            Logger.WLOG(Logger.ESOCKET, "setRequestMethod failed", ex);
            throw ex;
        }
        //Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- setRequestMethod(String)" );
        return;
    }

    /**
         * Please refer to Jsr 118
         */
    public synchronized void setRequestProperty(String aKey, String aValue)
    throws IOException
    {
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ setRequestProperty( " +
        // aKey + " , " + aValue );
        ensureOpen("setRequestProperty");
        if (iState != SETUP)
        {
            throwIOException("setRequestProperty method failed.Connection state is already connected");
        }

        // ignore if an outputstream has been open
        if (iPostedDataStream == null)
        {
            if (aKey.equals(USER_AGENT) && !iTrustedSuite)
            {
                String val = aValue + ' ' + UNTRUSTED_1_0;
                iRequestProperties.put(aKey, val);
            }
            else
            {
                iRequestProperties.put(aKey, aValue);
            }
            if (aKey.equals(TRANSFER_ENCODING))
            {
                String tmp = aValue.toLowerCase();
                if (tmp.equals("chunked"))
                {
                    iChunkedTransfer = true;
                    iRequestBodyLen = -1;    // as per RFC, precedence should be give to "transfer-encoding" when "content-length" is also set.
                }
            }
            if (aKey.equals(CONTENT_LENGTH))
            {
                iRequestBodyLen = Integer.parseInt(aValue);

            }
        }
        //Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- setRequestProperty(String, String)" );
        return;
    }

    protected void closeConnection() throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "++++++++++++++ Java closeConnection()");
        if ((iNativeTransactionHande != 0) &&(iNativeHttpSessionHandle!=0))
        {
            Logger.LOG(Logger.ESOCKET, Logger.EInfo, "########## Valid handle");
            _closeTransaction(iNativeTransactionHande);
            iNativeTransactionHande = 0;

        }
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "-------------- Java closeConnection()");
    }

    protected void addHeader(String aKey, String aValue)
    {
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ addHeader to reply enum(
        // " + aKey + " , " + aValue );
        // Should check for duplicate keys
        aKey = aKey.toLowerCase();
        iReplyHeaders.put(aKey, aValue);
        iReplyHeaderKeys.addElement(aKey);
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- addHeader(String,
        // String)" );
        return;
    }

    protected synchronized void inputStreamClosed() throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "++++++ inputStreamClosed()");
        synchronized (iNativeDataReadyForRead.getLock())
        {
            iInputStream = null;
            iNativeHttpInputStream = null;
            if (iClosed && (iOutputStream == null))
            {
                closeConnection();
            }
        }
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "-------- inputStreamClosed()");
    }


    private void waitForPostDataCompletion()
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+waitForPostDataCompletion");
        iPostDataBlock.waitForCompletion();
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "-waitForPostDataCompletion");

    }

    private synchronized void sendPartialRequest() throws IOException
    {
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "HTTP sendPartialRequest() ");
        switch (iState)
        {
        case SETUP:
        case CONNECTED:

            synchronized (iPostDataBlock.getLock())
            {
                synchronized (iPostDataBlock)
                {
                    sendRequest(true);
                    iPostedDataStream.reset();
                    waitForPostDataCompletion();
                }
            }
            break;

        case PARTIAL_REQUEST_SENT :

            byte[] postData = iPostedDataStream.toByteArray();
            iPostedDataStream.reset();
            int len = -1;
            if (postData != null)
            {
                len = postData.length;
                iTotalPostDataLen = iTotalPostDataLen + len;
            }
            // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "HTTP content-len = "+iRequestBodyLen + " totalflushed = "+ iTotalPostDataLen +" os len = "+ postData.length);
            if (iTotalPostDataLen == iRequestBodyLen)
            {
                iEndOfRequest = true;
            }

            synchronized (iPostDataBlock.getLock())
            {
                synchronized (iPostDataBlock)
                {
                    int err = _postData(iNativeTransactionHande, postData, len, iEndOfRequest);

                    if (err != 0)
                    {
                        throwIOException("Unable to write data .Symbian os error code: "
                                         + err);
                    }
                    if (iEndOfRequest == false)
                    {
                        // wait until http stack consumes the data
                        waitForPostDataCompletion();
                    }
                    else
                    {
                        // request complete, wait until the first chunk of response header arrives
                        synchronized (iTransactionBlock.getLock())
                        {
                            synchronized (iTransactionBlock)
                            {
                                waitForTransaction();
                            }
                        }

                    }  // end else
                } // end syn

            } // end syn getlock

            break;
        }

    }

    protected synchronized void outputStreamFlushed() throws IOException
    {
        int postDataLength = iPostedDataStream.size();


        if ((iRequestBodyLen > postDataLength) || (iChunkedTransfer == true))
        {
            // chunked http request will be sent in two cases
            // 1) application sets "Content-Lenght" request header and writes to output stream multiple times using flush
            //     like a sequence of os.write(); os.flush() operations
            // 2) application sets "Transfer-Encoding" request header and writes to ouput stream multiple times using flush
            sendPartialRequest();

        }
        else
        {
            switch (iState)
            {
            case SETUP:
            case CONNECTED:
            case REQUEST_HEADERS_SENT:
                ensureResponse();
                break;

            default:
                // No-op
                ;
            }
        }
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- outputStreamFlushed()" );
    }

    protected synchronized void outputStreamClosed() throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "++++++++ outputStreamClosed()");
        iOutputStream = null;
        iPostedDataStream = null;
        if (iClosed && iInputStream == null)
        {
            closeConnection();
        }
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "-------- outputStreamClosed()");
    }

    private synchronized void connect() throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+++++++ Java connect()");
        if (iNativeTransactionHande <= 0)
        {
            final int handle = _createNativeTransaction(
                                   iNativeHttpSessionHandle, iUri.toString(), iRequestMethod);
            iNativeTransactionHande = NativeError.check(handle);
        }
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "-------- connect()");
    }

    protected void ensureConnected() throws IOException
    {
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ ensureConnected()" );
        switch (iState)
        {
        case SETUP:

            connect();
            iState = CONNECTED;
            break;

        default:
            ;
        }
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- ensureConnected()" );
        return;
    }

    protected void ensureOpen(String aMethodName) throws IOException
    {
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ ensureOpen()" );
        if (iClosed)
        {
            throwIOException(aMethodName
                             + " method failed.Connection state is already closed");
        }
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- ensureOpen()" );
        return;
    }

    private void waitForResponse() throws IOException
    {
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "HTTP flush case, waitForResponse: total len =  "+iRequestBodyLen + " totalflushed = "+ iTotalPostDataLen );
        if (iEndOfRequest == false)
        {
            // chunked http request was sent, but end of request not sent
            // only in case of "transfer-encoding" request header, control will come here
            iEndOfRequest = true;
            byte[] postData = iPostedDataStream.toByteArray();
            iPostedDataStream.reset();
            int len = -1;
            if (postData != null)
            {
                len = postData.length;
                // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "More data left in os, len = "+len);
                iTotalPostDataLen = iTotalPostDataLen + len;
            }
            int err = _postData(iNativeTransactionHande, postData, len , iEndOfRequest);
            if (err != 0)
            {
                throwIOException("Unable to write data .Symbian os error code: "
                                 + err);
            }
            synchronized (iTransactionBlock.getLock())
            {
                synchronized (iTransactionBlock)
                {
                    waitForTransaction();
                }
            }
        }
        getResponse();
        iState = REPLY_RECEIVED;

    }

    protected void ensureResponse() throws IOException
    {
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ ensureResponse()" );
        ensureConnected();
        switch (iState)
        {
        case PARTIAL_REQUEST_SENT:
            waitForResponse();
            break;

        case CONNECTED:

            sendRequest(false);      // sendRequest() will block until the first chunk of http response arrives from the server.
            // Fall Through
        case REQUEST_HEADERS_SENT:
        case REQUEST_SENT:
            getResponse();
            iState = REPLY_RECEIVED;
            break;

        default:
            // No-op
            ;
        }
        //Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- ensureResponse()" );
        return;
    }

    protected synchronized void getResponse() throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ getResponse()");

        boolean flag = true;
        String[] rawHeaders = _getResponse(iNativeTransactionHande);
        if (rawHeaders == null || rawHeaders.length == 0)
            throwIOException("Invalid http response");

        // First element is the Status Line
        readStatusLine(rawHeaders[0]);
        readHeaders(rawHeaders);
        processReplyHeaders();
        rawHeaders = null;
        //Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- getResponse()" );
        return;
    }

    protected void readStatusLine(String aStatusLine) throws IOException
    {
        // Status-Line = HTTP-Version ;; Status-Code ;; Reason-Phrase
        Logger.PLOG(Logger.ESOCKET, "HTTP response, status line: "
                    + aStatusLine);
        // HTTP-Version
        final int fisrtSepIndex = aStatusLine.indexOf(NATIVESEPERATOR);
        if (fisrtSepIndex <= 0)
        {
            throwIOException("Invalid http response");
        }
        String parse = aStatusLine.substring(0, fisrtSepIndex);

        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, " Server Vesion : " + parse
        // );

        // Check the server we are connecting to is a http v1.0 or v1.1
        if (parse.compareTo(VERSION_1_1) != 0
                && parse.compareTo(VERSION_1_0) != 0)
        {
            throwIOException("Invalid http response.Http-version mismatch");
        }

        // Status Code
        int nextSepIndex = 0;
        nextSepIndex = aStatusLine.indexOf(NATIVESEPERATOR, fisrtSepIndex + 2);
        if (nextSepIndex <= 0)
        {
            throwIOException("Invalid http response");
        }
        parse = null;
        parse = aStatusLine.substring(fisrtSepIndex + 2, nextSepIndex);
        iResponseCode = Integer.parseInt(parse);

        // Reason-Phrase

        iResponseMessage = aStatusLine.substring(nextSepIndex + 2, aStatusLine
                           .length());
        if (iResponseMessage.length() <= 0)
        {
            throwIOException("Invalid http response");
        }
        //Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- readStatusLine(InputStream)" );
    }

    protected long parseDate(String aDateString)
    {
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ parseDate(String)" );
        final long result = DateUtil.epocDifference(aDateString);
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- parseDate(String)" );
        return result;
    }

    protected void processReplyHeaders()
    {
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ processReplyHeaders()" );
        String value;
        value = (String) iReplyHeaders.get(CONTENT_LENGTH.toLowerCase());
        if (value != null)
        {
            iLength = Long.parseLong(value);
        }
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- processReplyHeaders()" );
    }

    protected void readHeaders(String[] aHeaders) throws IOException
    {
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ readHeaders()" );
        // Headers are of the form
        // valueKey + NATIVESEPERATOR + property
        String key;
        String value;
        String rawHeader = null;
        final int headersCount = aHeaders.length;

        // First element is status line which we ignore (parsed in
        // readStatusLine)
        int sepIndex = -1;
        for (int ii = 1; ii < headersCount; ++ii)
        {
            rawHeader = aHeaders[ii];
            sepIndex = rawHeader.indexOf(NATIVESEPERATOR);
            key = rawHeader.substring(0, sepIndex);
            value = rawHeader.substring(sepIndex + 2, rawHeader.length());
            addHeader(key, value);
            rawHeader = null;
            key = null;
            value = null;
            sepIndex = -1;
        }
        //Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- readHeaders(InputStream)" );
    }

    protected synchronized void sendRequest(boolean aPartialDataFlag) throws IOException
    {
        ensureConnected();
        Logger.PLOG(Logger.ESOCKET, "HTTP sendRequest() , Flag : "  + aPartialDataFlag);
        final int count = iRequestProperties.size();
        int headerCount = count;

        final Enumeration keyEnum = iRequestProperties.keys();
        String[] headers = null;
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- headerCount: " +
        // headerCount );
        if (headerCount > 0)
            headers = new String[headerCount];

        String valueKey = null;
        String property = null;
        String concat = null;
        boolean untrustedAdded = false;
        for (int ii = 0; ii < count; ++ii)
        {
            valueKey = (String) keyEnum.nextElement();
            property = getRequestProperty(valueKey);
            concat = valueKey + NATIVESEPERATOR + property;
            headers[ii] = concat;
            // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "submit : " + ii + " : "
            // + concat + "concat length:" + concat.length() );
        }

        byte[] postData = null;
        int postDataLength = 0;
        // Data to be sent native
        if (iPostedDataStream != null)
        {
            postData = iPostedDataStream.toByteArray();
            postDataLength = postData.length;

        }

        synchronized (iTransactionBlock.getLock())
        {
            // Submit the headers and the data at the same time
            // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ _submitTransaction
            // headers count : " + (headers!=null ?headers.length:-1 ) );
            synchronized (iTransactionBlock)
            {
                Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                           "before _submitTransaction ");
                final int err = _submitTransaction(iNativeTransactionHande,
                                                   headers, postData, postDataLength,iRespTimeOut, aPartialDataFlag);

                Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                           "-_submitTransaction +err " + err);

                // Clear tempory byte array and headers array
                postData = null;
                headers = null;
                if (err != 0)
                    throwIOException("Unable to connect to server.Symbian os error code: "
                                     + err);
                if (aPartialDataFlag == true)
                {
                    iState = PARTIAL_REQUEST_SENT;
                    iTotalPostDataLen = iTotalPostDataLen + postDataLength;
                }
                else
                {
                    // block and wait for response headers only when it a complete request
                    iState = REQUEST_SENT;
                    waitForTransaction();
                }
                Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                           "- sendRequest _submitTransaction");
            }
        }
        if (aPartialDataFlag == false)
        {
            if ((iTransactionBlock.getResult() == -18) &&(iRetries < 2))
            {
                iRetries++;
                Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                           "- sendRequest KErrNotReady erroi, calling recursive sendRequest()");
                sendRequest(false);
                Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                           "- sendRequest KErrNotReady erroi, calling recursive sendRequest() returned");
            }
        }
    }

    protected void throwIOException(String aMessage) throws IOException
    {
        throw new IOException(aMessage);
    }

    protected void waitForTransaction() throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+waitForTransaction");

        iTransactionBlock.waitForCompletion();
        final int result = iTransactionBlock.getResult();
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "waitForTransaction result :"
                   + result);
        if ((result < 0))
        {
            final Integer error = new Integer(result);
            if ((result == -18))
            {
                if (iRetries >=2)
                {
                    throwIOException(error.toString());
                }
            }
            else
            {
                throwIOException(error.toString());
            }

        }

        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "-waitForTransaction");
    }

    protected DataInputStream makeDataInputStream() throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> makeDataInputStream()<<<<<<<<<<<<<<<<<<<<<<<");
        DataInputStream result = null;
        final String transferEncoding;

        final int bytesTotal = (int) getLength();
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "++++++ makeDataInputStream()  getLength():" + bytesTotal);

        if (iNativeHttpInputStream == null)
            iNativeHttpInputStream = new NativeHttpInputStream(this);
        result = new PlainDataInputStream(iNativeHttpInputStream);
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, " makeDataInputStream()");
        return result;
    }

    protected void makeDataOutputStream() throws IOException
    {
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ makeDataOutputStream()"
        // );
        iPostedDataStream = new ByteArrayOutputStream();
        iOutputStream = new PlainDataOutputStream(iPostedDataStream);
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- makeDataOutputStream()"
        // );
    }

    /**
     * From NativeHttpByteSoure
     */
    public void closeByteSource()
    {
    }

    /*
     * Returns the total number of bytes read into the buffer, or -1 if there is
     * no more data because the end of the stream has been reached.
     */
    public synchronized int getBytes(byte[] aBytes, int aLength)
    throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+getBytes()");
        int result = -1;

        // Prevents multiple read
        synchronized (iNativeDataReadyForRead.getLock())
        {
            synchronized (iNativeDataReadyForRead)
            {
                if (iNativeDataReadyForRead.getResult() == BlockingOperation.BLOCKED)
                {
                    Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                               "+ getBytes()- ReadOperation.waitForCompletion()");
                    iNativeDataReadyForRead.waitForCompletion();
                    Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                               "- getBytes()- ReadOperation.waitForCompletion()");
                }
            }

            final int callBackResult = iNativeDataReadyForRead.getResult();
            if (callBackResult == EOF)
                return 0; // All data has been read
            if (callBackResult < 0)
                throwIOException("Unable to read http response.Symbian os error code: "
                                 + result);
            // Block until data has been read from native
            iNativeDataReadyForRead.setResult(BlockingOperation.BLOCKED);
            if (iNativeHttpSessionHandle!=0)
            {
                result = _getBytes(iNativeTransactionHande, aBytes, aLength);
                Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ 2 getBytes()- result "
                           + result);
            }
        }
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "-getBytes() + result "
                   + result);
        return result;
    }

    public int available() throws IOException
    {
        if ((iClosed == true) && (iInputStream == null)
                && (iOutputStream == null))
        {
            throw new IOException(
                "Available method failed.Connection is already closed");
        }
        return _available(iNativeTransactionHande);
    }

    /*
     * Native call back methods
     */
    protected void transactionSubmitCallback(int aStatus)
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "+ transactionSubmitCallback Callback");
        iTransactionBlock.notifyCompleted(aStatus);
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "- transactionSubmitCallback Callback");
    }

    protected void dataReadyForReadCallBack(int aStatus)
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ dataReadyForReadCallBack  "
                   + aStatus);
        iNativeDataReadyForRead.notifyCompleted(aStatus);
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- dataReadyForReadCallBack");
    }

    protected void postDataConsumedCallback()
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ postDataconsumedCallback  ");
        iPostDataBlock.notifyCompleted(0);

    }

    /*
     * Native Calls
     */
    private native int _createHttpSession(int a, int aType, int aApn,
                                          int[] retval);
    private native int _createNativeTransaction(int aNativeHttpSessionHandle,
            String aUri, String aRequestMethod);
    private native int _submitTransaction(int aNativeTransactionHande,
                                          String[] aHeaders, byte[] aPostData, int aPostLength,
                                          int aRespTimeOut, boolean aPartialData);
    private native String[] _getResponse(int aNativeTransactionHande);
    private native int _getBytes(int aNativeTransactionHande, byte[] aBuffer,
                                 int aLength);
    private native void _closeTransaction(int aNativeTransactionHande);
    private native int _available(int aNativeTransactionHande);
    private native String _getUserAgentHeaderValue(boolean aMidpRuntime);
    private native int _postData(int aNativeTransactionHandle, byte[] aPostData, int aLength, boolean iEndOfRequest);

    /*
     *
     * class PlainDataInputStream
     *
     */
    protected class PlainDataInputStream extends DataInputStream
    {
        public void close() throws IOException
        {
            super.close();
            inputStreamClosed();
        }

        PlainDataInputStream(InputStream aIs)
        {
            super(aIs);
        }
    }

    /*
     *
     * class PlainDataOutputStream
     *
     */
    protected class PlainDataOutputStream extends DataOutputStream
    {
        private boolean iFlushed = false;

        public void close() throws IOException
        {
            if (!iFlushed)
                flush();
            super.close();
            outputStreamClosed();
        }

        public void flush() throws IOException
        {
            super.flush();
            outputStreamFlushed();
            iFlushed = true;
        }

        PlainDataOutputStream(OutputStream aOs)
        {
            super(aOs);
        }
    }

    class NativeHttpInputStream extends InputStream
    {
        private NativeHttpByteSource iByteSource;
        private byte[] iBuffer;
        private int iOffset;
        private int iLength;
        // 6K buffer to read from native http
        private final static int BUFFER_SIZE = 6144;
        private boolean iIsClosed;

        NativeHttpInputStream(NativeHttpByteSource aByteSource)
        {
            // Bytesource length may be -1 indicating CHUNKED (size unknown)
            iByteSource = aByteSource;
            iBuffer = new byte[BUFFER_SIZE];
            iIsClosed = false;
        }

        public synchronized void close() throws IOException
        {
            iIsClosed = true;
            iByteSource.closeByteSource();
        }

        public synchronized int read() throws IOException
        {
            final int BYTEPADDER = 0xFF;
            // Logger.LOG(Logger.ESOCKET, Logger.EInfo,"NativeInputStream.read()
            // iOffset, "+ iOffset + "iLength " + iLength);
            throwIfClosed();

            if (iOffset == iLength)
            {
                // Logger.LOG(Logger.ESOCKET,
                // Logger.EInfo,"NativeInputStream.read() iOffset == iLength
                // getBytes: iLength " + iLength );
                getBytes();
            }

            // Logger.LOG(Logger.ESOCKET, Logger.EInfo,"NativeInputStream.read()
            // 2 iOffset, "+ iOffset + " iLength " + iLength);
            if (iLength > 0)
            {
                return iBuffer[iOffset++] & BYTEPADDER;
            }
            else
            {
                Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                           "NativeInputStream.read(): return -1 ");
                return EOF;
            }
        }

        public synchronized int read(byte[] aBytes, int aOffset, int aLength)
        throws IOException
        {
            // Logger.LOG(Logger.ESOCKET,
            // Logger.EInfo,"+NativeInputStream.read(,,,) aOffset, "+ aOffset +
            // "aLength" + aLength);

            throwIfClosed();

            int nBytes = 0;
            while (nBytes < aLength)
            {
                // Logger.LOG(Logger.ESOCKET,
                // Logger.EInfo,"NativeInputStream.read(,,,) nBytes, "+ nBytes);
                int count = readInner(aBytes, aOffset + nBytes, aLength
                                      - nBytes);
                // Logger.LOG(Logger.ESOCKET,
                // Logger.EInfo,"NativeInputStream.read(,,,) count = readInner,
                // "+ count);
                if (count < 0)
                {
                    nBytes = (nBytes == 0) ? -1 : nBytes;
                    break;
                }
                nBytes += count;
            }
            Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                       "- NativeInputStream.read(,,,) return nBytes, " + nBytes);
            return nBytes;
        }

        private int readInner(byte[] aBytes, int aOffset, int aLength)
        throws IOException
        {
            // Logger.LOG(Logger.ESOCKET,
            // Logger.EInfo,"NativeInputStream.readInner() iOffset: "+ iOffset +
            // "iLength: " + iLength);
            if (iOffset == iLength)
            {
                getBytes();
            }
            if (iLength == 0)
            {
                return EOF;
            }

            int nBytesLeft = iLength - iOffset;
            // Logger.LOG(Logger.ESOCKET,
            // Logger.EInfo,"NativeInputStream.readInner() : nBytesLeft "+
            // nBytesLeft);
            int nBytesToCopy = aLength > nBytesLeft ? nBytesLeft : aLength;
            System.arraycopy(iBuffer, iOffset, aBytes, aOffset, nBytesToCopy);
            iOffset += nBytesToCopy;
            return nBytesToCopy;
        }

        private void getBytes() throws IOException
        {
            int nBytes = iByteSource.getBytes(iBuffer, iBuffer.length);

            if (nBytes == -1)
            {
                throw new IOException();
            }
            iLength = nBytes;
            iOffset = 0;
        }


        /**
         * Returns the number of bytes that can be read (or skipped over) from
         * this input stream without blocking by the next caller of a method for
         * this input stream. The next caller might be the same thread or
         * another thread.
         *
         * @return the number of bytes that can be read from this input stream
         *         without blocking.
         * @exception IOException
         *                if an I/O error occurs.
         */
        public int available() throws IOException
        {
            throwIfClosed();
            return iByteSource.available();
        }

        private void throwIfClosed() throws IOException
        {
            if (iIsClosed)
            {
                throw new IOException("Connection is already closed");
            }
        }
    }
}
