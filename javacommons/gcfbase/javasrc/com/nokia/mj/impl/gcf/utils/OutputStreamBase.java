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


package com.nokia.mj.impl.gcf.utils;

import java.io.IOException;
import java.io.OutputStream;
import java.io.InterruptedIOException;

/**
 * OutputStreamBase is base class for OutputStreams that can be used by
 * StreamConnection types. The class is self-sufficient in handling all
 * operations related to OutputStreams. If there is a need to specialise it, we
 * can derive change the behaviour.
 */
public class OutputStreamBase extends OutputStream
{
    protected boolean iIsClosed;

    protected StreamConnectionBase iStreamBase;

    protected final Object iCloseLock;

    private final byte[] iSingleByte;

    private final byte[] iBuffer;

    private int iBufferOffset;

    protected OutputStreamBase(StreamConnectionBase aEndPoint,
                               Object aCloseLock, boolean aCreateBuffer)
    {
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " +   OutputStreamBase: Constructor");
        iStreamBase = aEndPoint;
        iIsClosed = false;
        iSingleByte = new byte[1];

        if (aCreateBuffer)
        {
            iBuffer = new byte[iStreamBase.iBufferSize];
        }
        else
        {
            iBuffer = null;
        }
        iCloseLock = aCloseLock;
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " -   OutputStreamBase: Constructor");
    }

    protected OutputStreamBase(StreamConnectionBase aEndPoint, Object aCloseLock)
    {
        iStreamBase = aEndPoint;
        iIsClosed = false;
        iSingleByte = new byte[1];
        iBuffer = new byte[iStreamBase.iBufferSize];
        iCloseLock = aCloseLock;
    }

    public synchronized void write(int aByte) throws IOException
    {
        iSingleByte[0] = (byte)(aByte & 0xFF);
        write(iSingleByte, 0, 1);
    }

    /**
     * Writes the bytes to the local cache or directly to the native socket.
     */
    public synchronized void write(byte[] aBytes, int aOffset, int aLength)
    throws IOException
    {
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " +   OutputStreamBase: write()");
        if (iIsClosed)
        {
            throw new IOException("OutputStream closed");
        }

        StreamConnectionBase.checkIOParams(aBytes, aOffset, aLength);

        // Flush the cache now if length of data to be written is bigger than
        // the space left in the buffer
        if ((iBufferOffset + aLength) > iStreamBase.iBufferSize)
        {
            flush();
        }

        // See if we cache the data here or write it all because it is too big
        // for the cache
        if (aLength > iStreamBase.iBufferSize)
        {
            // If length of data to be written is bigger than the buffer size,
            // we loop until it is all written
            int nLeftToWrite = aLength;
            int offset = aOffset;

            while (nLeftToWrite > 0)
            {
                int nBytesToWrite = Math.min(nLeftToWrite,
                                             iStreamBase.iBufferSize);

                try
                {
                    StreamsLogger
                    .Log(StreamsLogger.Level_3,
                         "     OutputStreamBase: write(): Writing to StreamBase");
                    iStreamBase.writeBytes(aBytes, offset, nBytesToWrite);
                }
                catch (InterruptedIOException ie)
                {
                    ie.bytesTransferred = aLength - nLeftToWrite;
                    throw ie;
                }

                nLeftToWrite -= nBytesToWrite;
                offset += nBytesToWrite;
            }
        }
        else
        {
            StreamsLogger
            .Log(StreamsLogger.Level_3,
                 "     OutputStreamBase: write(): Writing to Internal cache");
            // Otherwise just add the bytes to the end of the cache
            System.arraycopy(aBytes, aOffset, iBuffer, iBufferOffset, aLength);
            iBufferOffset += aLength;
        }
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " -   OutputStreamBase: write()");
    }

    /**
     * This writes all bytes that are currently in the Java-side cache.
     */
    public synchronized void flush() throws IOException
    {
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " +   OutputStreamBase: flush()");
        if (iBufferOffset > 0)
        {
            iStreamBase.writeBytes(iBuffer, 0, iBufferOffset);
            iBufferOffset = 0;
        }
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " -   OutputStreamBase: flush()");
    }

    public void close() throws IOException
    {
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " +   OutputStreamBase: close()");
        // Closing OutputStream without flushing is against spec.
        // See javadoc of DataOutputStream.
        flush();
        synchronized (iCloseLock)
        {
            if (!iIsClosed)
            {
                iIsClosed = true;
                iStreamBase.cancelWrite();
                iStreamBase = null;
            }
        }
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " -   OutputStreamBase: close()");
    }

    public boolean isClosed()
    {
        synchronized (iCloseLock)
        {
            return iIsClosed;
        }
    }
}
