/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.mj.impl.utils;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Enumeration;
import java.util.Vector;


/**
 * A utility class for stream operations.
 */
public class StreamUtils
{
    private static final int mDefaultBufferSize = 1024;

    /**
     * Reads all bytes from given stream until EOF, and returns the bytes in
     * an array. The method will block until EOF is reached. The method uses
     * efficiently buffers in order to avoid extra copying. User can provide
     * a hint how large first buffer should be allocated to optimize buffer
     * copying. The user must note that it is only a hint - the result buffer
     * might also be smaller or bigger. If hint is <=0, then value of
     * stream.available() is used as hint.
     *
     * @param stream The stream from where to read data from.
     * @param byteCountHint A hint about the total amount of bytes in the
     *                      stream. Value <=0 means that caller has no hint to
     *                      give.
     * @return The data read from the given stream in byte array.
     * @throws IOException on I/O error cases.
     */

    public static byte[] readBytesFromStream(InputStream stream,
            int byteCountHint)
    throws IOException
    {
        // Set the first buffer size.
        int bufferSize = Math.max(byteCountHint, stream.available());
        if (bufferSize < 1)
        {
            bufferSize = mDefaultBufferSize;
        }

        // A vector for buffers.
        Vector bufferVector = new Vector();

        // Amount of total read bytes.
        int totalCount = 0;

        while (true)
        {
            // Internal buffer where to read the content.
            byte[] currentBuffer = new byte[bufferSize];

            // Read from stream.
            int len = readBytesFromStream(stream, currentBuffer);
            if (len < 1)
            {
                // There were no more data in the stream.
                break;
            }
            totalCount += len; // Increase total amount of read bytes.

            // Add the buffer to vector.
            bufferVector.addElement(new ByteBuffer(currentBuffer, len));
            if (len < currentBuffer.length)
            {
                // We have read all the data. Buffer contains less data
                // than the size of the buffer.
                break;
            }
            bufferSize = mDefaultBufferSize;
        }
        return createResultBuffer(totalCount, bufferVector);
    }

    /**
     * Fills the given buffer with bytes from given stream. The method will
     * not read to the end of stream if the buffer is filled up.
     *
     * @param stream The stream from where to read data from.
     * @param buffer A byte array where to copy the data.
     * @return The amount of data read into byte array. If return value <
     *         len(buffer), then EOF has been reached.
     * @throws IOException on I/O error cases.
     */
    public static int readBytesFromStream(InputStream stream,
                                          byte[] buffer) throws IOException
    {
        int readByteCount = 0;
        int len;
        while (readByteCount < buffer.length &&
                (len = stream.read(buffer, readByteCount,
                                   buffer.length - readByteCount)) >= 0)
        {
            // Increase the start position where to write into current buffer.
            readByteCount += len;
        }
        return readByteCount;
    }

    /**
     * Creates a result buffer from given args.
     *
     * @param totalByteCount Total count of read bytes.
     * @param firstBuffer A byte array containing first set of data.
     * @return The amount of data read into byte array.
     * @throws IOException on I/O error cases.
     */
    private static byte[] createResultBuffer(int totalByteCount,
            Vector bufferVector)
    {
        if (totalByteCount == 0)
        {
            return new byte[0];
        }

        byte[] resultBuffer = null;
        // The stream contained data.
        if (bufferVector.size() == 1)
        {
            // There is only one buffer.
            ByteBuffer firstBuffer = (ByteBuffer)bufferVector.elementAt(0);
            if (firstBuffer.isFull())
            {
                // The first buffer was full.
                resultBuffer = firstBuffer.mBuffer;
            }
        }

        if (resultBuffer == null)
        {
            // Need to combine the result from several buffers or the
            // first buffer wasn't full.
            resultBuffer = new byte[totalByteCount];

            // A destination position in the result buffer.
            int dst = 0;

            // Loop and copy all the buffers.
            Enumeration byteBufs = bufferVector.elements();
            while (byteBufs.hasMoreElements())
            {
                ByteBuffer buf = (ByteBuffer)byteBufs.nextElement();
                System.arraycopy(buf.mBuffer, 0, resultBuffer, dst, buf.mCount);
                dst +=  buf.mCount;
            }
        }
        return resultBuffer;
    }

    /**
     * A utility class for storing the byte array and how much data it
     * contains.
     */
    private static class ByteBuffer
    {
        byte[] mBuffer;
        int    mCount;

        private ByteBuffer(byte[] buffer, int count)
        {
            mBuffer = buffer;
            mCount = count;
        }

        private boolean isFull()
        {
            return mBuffer.length == mCount;
        }
    }
}
