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

import java.io.InputStream;
import java.io.IOException;

/**
 * Input Stream adapter class. Provides the input stream interfaces
 *
 * @version 1.1
 * @author Nokia Corporation
 */
public class HttpInputStreamAdapter extends InputStream
{
    private boolean mOpenFlag = true;

    private HttpClientConnection mHttpClientConn;

    private byte[] mSingleByteBuf = new byte[1];

    private static final int MASK = 0xff;

    /**
     * Constructs the HttpInputStreamAdapter.
     *
     * @param aHttpConn
     *            the HttpConn object
     */
    public HttpInputStreamAdapter(HttpClientConnection aHttpConn)
    {
        mHttpClientConn = aHttpConn;
    }

    /**
     * Read the data from the buffer.
     *
     * @return byte read
     * @throws IOException
     *             if stream is closed
     */
    public synchronized int read() throws IOException
    {
        if (read(mSingleByteBuf, 0, 1) > 0)
        {
            return (mSingleByteBuf[0] & MASK);
        }
        return -1;
    }

    /**
     * Reads the data to the buffer.
     *
     * @param buf
     *            the byte to be copied
     * @param offset
     *            the start offset inside the buf
     * @param len
     *            number of bytes to be read
     * @return byte read
     * @throws IOException
     *             if stream is closed
     */
    public synchronized int read(byte[] buf, int offset, int len)
    throws IOException
    {
        int j, k, tmplen;
        int ret = -1;

        if ((offset < 0) || (len < 0) || (offset + len > buf.length))
        {
            throw new IOException("illegal argument exception ");
        }

        if (mOpenFlag == false)
        {
            throw new IOException("Connection Closed");
        }

        if (mHttpClientConn.mChunkedFlag == false)
        {
            // read directly from the underlying socket input stream
            if (mHttpClientConn.mContentLength >= 0)
            {
                mHttpClientConn.mChunkSize = (int) mHttpClientConn.mContentLength;
                int rc = mHttpClientConn.getBytes(buf, offset, len);
                ret = rc;
            }
            else
            {
                tmplen = mHttpClientConn.mSocketInStream.read(buf, offset, len);
                ret = tmplen;
            }
        }
        else
        {
            // chunked response case
            int rc = mHttpClientConn.getBytes(buf, offset, len);
            ret = rc;
        }
        return ret;
    }

    /**
     * Closes the OutputStream .
     *
     * @throws IOException
     *             if stream is closed
     */
    public synchronized void close() throws IOException
    {
        // mHttpClientConn.inStream.close();
        mOpenFlag = false;
        mHttpClientConn.mInputStreamOpen = false;
    }

    /**
     * Check if input stream is in open state.
     *
     * @return true if inputstream open
     */
    public synchronized boolean isOpen()
    {
        return mOpenFlag;
    }
}
