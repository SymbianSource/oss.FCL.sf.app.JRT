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

import java.io.OutputStream;
import java.io.IOException;
import java.util.Vector;
import com.nokia.mj.impl.utils.Logger;

/**
 * Output Stream adapter class. Provides the output stream interfaces
 *
 * @version 1.1
 * @author Nokia Corporation
 */
public class HttpOutputStreamAdapter extends OutputStream
{
    private StringBuffer mBuffer = new StringBuffer();
    private Vector mTmpBuf;
    private boolean mOpenFlag = true;
    private HttpClientConnection mHttpClientConn;
    protected byte[] mDataBytes;

    /**
     * Constructs the OutputStreamAdapter.
     *
     * @param aHttpConn
     *            the HttpConn object
     */
    public HttpOutputStreamAdapter(HttpClientConnection aHttpConn)
    {
        mHttpClientConn = aHttpConn;
        mTmpBuf = new Vector();
    }

    /**
     * Writes the data to the buffer.
     *
     * @param aByte
     *            the byte to be copied
     * @throws IOException
     *             if stream is closed
     */
    public synchronized void write(int aByte) throws IOException
    {
        if (!mOpenFlag)
        {
            throw new IOException("stream closed");
        }
        else
        {
            mTmpBuf.addElement(new Integer(aByte));
        }
    }

    /**
     * Closes the OutputStream .
     *
     * @throws IOException
     *             if stream is closed
     */
    public synchronized void close() throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "+HttpOutputStreamAdapter::close()");
        if (!mOpenFlag)
        {
            return;
        }
        mOpenFlag = false;
        flush();
        mHttpClientConn.mOutputStreamOpen = false;
        mHttpClientConn = null;
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "-HttpOutputStreamAdapter::close()");
    }

    /**
     * Closes the OutputStream .
     *
     * @throws IOException
     *             if stream is closed
     */
    public synchronized void flush() throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "\nHttpOutputStreamAdapter flush() called ");
        copyToByteArray();
        // Reset the internal index
        mTmpBuf.removeAllElements(); // mTmpBuf can be reset, all the data is
        // copied into mDataBytes
        if (mHttpClientConn.mSendChunkedReq == true)
        {
            if (mHttpClientConn.mSockObj == null)
            {
                mHttpClientConn.createSocketConnection();
            }
            mHttpClientConn.sendChunkedRequest(false);
            mDataBytes = null;
        }
        else
        {
            // Send out the request along with the data written on to this output stream
            mHttpClientConn.getConnected();
        }
    }

    /**
     * Checks if OutputStream is in open state.
     *
     * @return true if outputstream open
     */
    public synchronized boolean isOpen()
    {
        return mOpenFlag;
    }

    protected void copyToByteArray() throws IOException
    {
        if (mDataBytes != null)
        {
            return; //already copied
        }

        mDataBytes = new byte[mTmpBuf.size()];

        int k = 0;
        try
        {
            Integer y;
            for (int i = 0; i < mTmpBuf.size(); i++)
            {
                // Copy the data from the Vector to mBuffer memory.
                y = (Integer)(mTmpBuf.elementAt(i));
                int c = y.intValue();
                mBuffer.append((char) c);
                mDataBytes[k] = (byte) c;
                k++;
            }
        }
        catch (Exception ex)
        {
            mHttpClientConn = null;
            throw new IOException(ex.getMessage());
        }
    }
}
