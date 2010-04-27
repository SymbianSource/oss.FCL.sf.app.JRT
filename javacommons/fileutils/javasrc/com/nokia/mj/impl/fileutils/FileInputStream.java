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

package com.nokia.mj.impl.fileutils;

import java.io.IOException;
import java.io.InputStream;

/**
 * Provides an InputStream to a given file target. FileInputStream cannot be
 * used independently. It needs to be created using FileUtility.
 *
 * <pre>
 * try
 * {
 *     FileUtility target = new FileUtility("C:/Data/Images/myFile.txt");
 *     InputStream stream = target.openInputStream();
 * }
 * catch(IOException ex)
 * {
 *     ...
 * }
 * </pre>
 *
 * @see FileUtility
 */
class FileInputStream extends InputStream
{
    private static int EOF = -1;

    private FileStreamHandler iDataSource;

    private boolean iIsClosed;

    private final Object iCloseLock;

    private final byte[] iSingleByte;

    private byte[] iMarkBuffer;

    private int iMarkBufferOffset;

    private int iMarkLength;

    private boolean iReadFromMarkBuffer;

    private int iMarkReadOffset;

    public FileInputStream(FileStreamHandler aDataSource)
    {
        iDataSource = aDataSource;
        iIsClosed = false;
        iCloseLock = new Object();
        iSingleByte = new byte[1];

        iMarkBuffer = null;
        iMarkBufferOffset = 0;
        iMarkLength = 0;
        iReadFromMarkBuffer = false;
        iMarkReadOffset = 0;
    }

    /**
     *
     */
    public int read() throws IOException
    {
        if (read(iSingleByte, 0, 1) == EOF)
        {
            return EOF;
        }
        return iSingleByte[0] & 0xff;
    }

    /**
     * Reads up to aLength bytes of data from the input stream into an array of
     * bytes. An attempt is made to read as many as aLength bytes, but a smaller
     * number may be read. The number of bytes actually read is returned as an
     * integer.
     * <p>
     * This method blocks until input data is available, end of file is
     * detected, or an exception is thrown.
     * <p>
     * If aLength is zero, then no bytes are read and 0 is returned; otherwise,
     * there is an attempt to read at least one byte. If no byte is available
     * because the stream is at end of file, the value -1 is returned;
     * otherwise, at least one byte is read and stored into aBytes.
     * <p>
     * The first byte read is stored into element aBytes[aOffset], the next one
     * into aBytes[aOffset+1], and so on. The number of bytes read is, at most,
     * equal to aLength. Let k be the number of bytes actually read; these bytes
     * will be stored in elements aBytes[aOffset] through aBytes[aOffset+k-1],
     * leaving elements aBytes[aOffset+k] through aBytes[aOffset+aLength-1]
     * unaffected.
     * <p>
     *
     * @param aBytes
     *            the buffer into which the data is read.
     * @param aOffset
     *            the start offset in array aBytes at which the data is written.
     * @param aLength
     *            the maximum number of bytes to read.
     * @return the total number of bytes read into the buffer, or -1 if there is
     *         no more data because the end of the stream has been reached.
     * @throws IOException
     *             If the bytes cannot be read for any reason other than end of
     *             file, or if the input stream has been closed, or if some
     *             other I/O error occurs.
     */
    public synchronized int read(byte[] aBytes, int aOffset, int aLength) throws IOException
    {
        throwIfClosed();

        FileStreamHandler.checkIOParams(aBytes, aOffset, aLength);
        if (aLength == 0)
        {
            return 0;
        }

        // In case of any exceptions, native throws the exception.
        int retVal = handleMarkAndReset(aBytes, aOffset, aLength);
        retVal = (0 == retVal) ? (retVal = EOF) : retVal;
        return retVal;
    }

    /**
     * Fills data into aBytes while taking care of mark and reset case.
     * <p>
     * In case mark point has not been set, it translates into a simple read
     * from the file.
     * <p>
     * In case mark point has been set, then the method decides whether the data
     * has to be read from the native side or mark buffer. Also, in case mark is
     * active, data read from file is appended to the mark buffer. On call to
     * reset, data is read from this buffer.
     *
     * @param aBytes
     *            the buffer into which the data is read
     * @param aOffset
     *            the start offset in the array aBytes at which the data is
     *            written
     * @param aLength
     *            the maximum number of bytes to read
     * @return the total number of bytes read into aBytes, or -1 if there is no
     *         more data because the end of the stream has been reached.
     * @throws IOException
     *             in case there was an I/O error.
     */
    private int handleMarkAndReset(byte[] aBytes, int aOffset, int aLength)
    throws IOException
    {
        if (iMarkLength > 0)
        {
            // Mark has been called and is active at the moment.
            // This leads to 2 situations.
            // if reset() has been called, we need to read from mark buffer.
            // if reset() has not been called, we just need to append data
            // to mark buffer.

            if (false == iReadFromMarkBuffer)
            {
                // reset() has not been called. Read from native directly and
                // append the data to mark buffer.
                int retVal = iDataSource.readBytes(aBytes, aOffset, aLength);

                if ((iMarkBufferOffset + retVal) > iMarkLength)
                {
                    // If data read is more than space left in mark buffer.
                    // Mark point becomes invalid.
                    iMarkLength = 0;
                    iMarkBuffer = null;
                    iMarkBufferOffset = 0;
                }
                else
                {
                    // Data read from native can be put into mark buffer.
                    System.arraycopy(aBytes, aOffset, iMarkBuffer,
                                     iMarkBufferOffset, retVal);
                    iMarkBufferOffset += retVal;
                }
                return retVal;
            }
            else
            {
                // Mark is valid and reset has been called. Read from mark
                // buffer.
                if (aLength <= (iMarkBufferOffset - iMarkReadOffset))
                {
                    // Required data requested is already in the buffer.
                    System.arraycopy(iMarkBuffer, iMarkReadOffset, aBytes,
                                     aOffset, aLength);
                    iMarkReadOffset += aLength;
                    return aLength;
                }
                else
                {
                    // Required data is less than available in mark buffer.
                    int readableData = iMarkBufferOffset - iMarkReadOffset;
                    System.arraycopy(iMarkBuffer, iMarkReadOffset, aBytes,
                                     aOffset, readableData);
                    iMarkReadOffset += readableData;

                    int nativeRead = iDataSource.readBytes(aBytes,
                                                           (aOffset + readableData), (aLength - readableData));

                    if (nativeRead > 0)
                    {
                        // More data has been read from read limit.
                        // Invalidate mark buffer.
                        if (iMarkBufferOffset == iMarkLength)
                        {
                            iMarkBuffer = null;
                            iMarkLength = 0;
                            iMarkBufferOffset = 0;
                            iReadFromMarkBuffer = false;
                        }
                        else
                        {
                            if ((iMarkBufferOffset + nativeRead) <= iMarkLength)
                            {
                                System.arraycopy(aBytes,
                                                 aOffset + readableData, iMarkBuffer,
                                                 iMarkBufferOffset, nativeRead);
                                iMarkBufferOffset += nativeRead;
                                iMarkReadOffset += nativeRead;
                            }
                            else
                            {
                                iMarkBuffer = null;
                                iMarkLength = 0;
                                iMarkBufferOffset = 0;
                                iReadFromMarkBuffer = false;
                            }
                        }
                    }
                    else
                    {
                        iMarkBuffer = null;
                        iMarkLength = 0;
                        iMarkBufferOffset = 0;
                        iReadFromMarkBuffer = false;
                    }

                    return (readableData + nativeRead);
                }
            }
        }

        // Mark has not been called. So, just read.
        return iDataSource.readBytes(aBytes, aOffset, aLength);
    }

    /**
     * Returns an estimate of the number of bytes that can be read (or skipped
     * over) from this input stream without blocking by the next invocation of a
     * method for this input stream. The next invocation might be the same
     * thread or another thread. A single read or skip of this many bytes will
     * not block, but may read or skip fewer bytes.
     *
     * NOTE: For DRM protected files, available returns incorrect value.
     * No workaround at present.
     */
    public synchronized int available() throws IOException
    {
        throwIfClosed();
        return (int)iDataSource.available();
    }

    public void close() throws IOException
    {
        synchronized (iCloseLock)
        {
            if (!iIsClosed)
            {
                iIsClosed = true;
                iDataSource.stopRead();
            }
        }
    }

    boolean isClosed()
    {
        synchronized (iCloseLock)
        {
            return iIsClosed;
        }
    }

    /**
     * Tests if this input stream supports the mark and reset methods.
     *
     * @return true as file input stream must support mark and reset.
     */
    public boolean markSupported()
    {
        return true;
    }

    /**
     * Marks the current position in this input stream. A subsequent call to the
     * reset method repositions this stream at the last marked position so that
     * subsequent reads re-read the same bytes.
     *
     * The aReadlimit arguments tells this input stream to allow that many bytes
     * to be read before the mark position gets invalidated. The general
     * contract of mark is that, if the method markSupported returns true, the
     * stream somehow remembers all the bytes read after the call to mark and
     * stands ready to supply those same bytes again if and whenever the method
     * reset is called. However, the stream is not required to remember any data
     * at all if more than aReadlimit bytes are read from the stream before
     * reset is called.
     *
     * Marking a closed stream should not have any effect on the stream.
     *
     * @param aReadLimit
     *            amount of data to be remembered until the marked position
     *            becomes invalid.
     */
    public synchronized void mark(int aReadLimit)
    {
        if (aReadLimit <= 0)
        {
            // Mark limit is invalid. Return without doing anything.
            return;
        }

        // If data has already been read into mark buffer and point has been
        // reset.
        if (iReadFromMarkBuffer)
        {
            int readableData = (iMarkBufferOffset - iMarkReadOffset);
            int lengthToCopy = (readableData < aReadLimit) ? readableData
                               : aReadLimit;

            byte[] tempBuffer = new byte[aReadLimit];
            System.arraycopy(iMarkBuffer, iMarkReadOffset, tempBuffer, 0,
                             lengthToCopy);

            iMarkBuffer = tempBuffer;
            iMarkBufferOffset = lengthToCopy;
            iMarkLength = aReadLimit;
            iMarkReadOffset = 0;

            // In case the new mark buffer has become smaller.
            if (readableData > aReadLimit)
            {
                // Move file offset back so many bytes. It has moved forward.
                iDataSource.skip(aReadLimit - readableData);
            }
        }
        else
        {
            // Point of mark is valid. Allocate buffer for it.
            iMarkBuffer = new byte[aReadLimit];
            iMarkBufferOffset = 0;
            iMarkLength = aReadLimit;
            iReadFromMarkBuffer = false;
        }
    }

    /**
     * Repositions this stream to the position at the time the mark method was
     * last called on this input stream. The general contract of reset is:
     * <p>
     * <ul>
     * <li>If the method markSupported returns true, then:
     * <ul>
     * <li> If the method mark has not been called since the stream was created,
     * or the number of bytes read from the stream since mark was last called is
     * larger than the argument to mark at that last call, then an IOException
     * might be thrown.
     * <li> If such an IOException is not thrown, then the stream is reset to a
     * state such that all the bytes read since the most recent call to mark (or
     * since the start of the file, if mark has not been called) will be
     * resupplied to subsequent callers of the read method, followed by any
     * bytes that otherwise would have been the next input data as of the time
     * of the call to reset.
     * </ul>
     * <li>If the method markSupported returns false, then:
     * <ul>
     * <li>The call to reset may throw an IOException.
     * <li>If an IOException is not thrown, then the stream is reset to a fixed
     * state that depends on the particular type of the input stream and how it
     * was created. The bytes that will be supplied to subsequent callers of the
     * read method depend on the particular type of the input stream.
     * </ul>
     * </ul>
     *
     * @throws java.io.IOException
     */
    public synchronized void reset() throws IOException
    {
        if (iMarkLength <= 0)
        {
            throw new IOException("Point of mark operation is invalid");
        }

        // Mark is set. Push back to previous mark point
        iReadFromMarkBuffer = true;
        iMarkReadOffset = 0;
    }

    /**
     * Checks if the InputStream has been closed.
     *
     * @throws IOException
     *             if the stream has been closed
     */
    protected synchronized void throwIfClosed() throws IOException
    {
        if (iIsClosed)
        {
            throw new IOException("InputStream closed");
        }
    }
}
