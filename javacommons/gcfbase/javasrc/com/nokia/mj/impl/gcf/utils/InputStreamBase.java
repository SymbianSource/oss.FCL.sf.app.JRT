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
import java.io.InputStream;

/**
 * InputStreamBase is base class for InputStreams that can be used by
 * StreamConnection types. The class is self-sufficient in handling all
 * operations related to InputStreams. If there is a need to specialise it, we
 * can derive change the behaviour.
 */
public class InputStreamBase extends InputStream
{
    protected boolean iIsClosed;

    protected StreamConnectionBase iStreamBase;

    protected final Object iCloseLock;

    protected final byte[] iSingleByte;

    protected final byte[] iBuffer;

    protected int iBufferLength;

    protected int iBufferOffset;

    // Mark and Reset
    private boolean iSupportMarkAndReset;

    private boolean iMarkSet;

    private int iMarkBufferTotal;

    // Holds the amount of data currently present in buffer

    private int iMarkBufferOffset;

    // Holds the offset in the current mark buffer

    private byte[] iMarkBuffer = null;

    private boolean iDataPresentInMark;

    private final int iMarkNotSet = -2;

    // So that it does not clash with EOF

    InputStreamBase(StreamConnectionBase aEndPoint, Object aCloseLock)
    {
        this(aEndPoint, aCloseLock, false);
    }

    protected InputStreamBase(StreamConnectionBase aEndPoint,
                              Object aCloseLock, boolean aSupportMarkAndReset)
    {
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " +   InputStreamBase: Constructor:");
        iStreamBase = aEndPoint;
        iCloseLock = aCloseLock;
        iIsClosed = false;
        iSingleByte = new byte[1];

        // This buffer size is matched on native for receiving data, thus
        // reading data from the native buffer does not support offsets and
        // lengths, but is a simple transfer of data from native to Java
        iBuffer = new byte[iStreamBase.iBufferSize];
        iSupportMarkAndReset = aSupportMarkAndReset;

        iMarkBuffer = null;
        // Set it to some negative value.
        iMarkBufferOffset = iMarkNotSet;
        iMarkBufferTotal = iMarkNotSet;
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " -   InputStreamBase: Constructor:");
    }

    /**
     * Returns the amount of data available currently in the InputStream
     */
    public synchronized int available() throws IOException
    {
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " +   InputStreamBase: available():");
        throwIfClosed();
        int retVal = 0;
        if (iBufferOffset < iBufferLength)
        {
            retVal = iBufferLength - iBufferOffset;
        }
        else
        {
            retVal = iStreamBase.available();
        }
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " -   InputStreamBase: available(): Return: " + retVal);
        return retVal;
    }

    public synchronized int read() throws IOException
    {
        if (read(iSingleByte, 0, 1) == StreamConnectionBase.EOF)
        {
            return StreamConnectionBase.EOF;
        }
        return iSingleByte[0] & 0xff;
    }

    public synchronized int read(byte[] aBytes, int aOffset, int aLength)
    throws IOException
    {
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " +   InputStreamBase: read(): " + " aOffset: " + aOffset
                          + " aLength: " + aLength);
        throwIfClosed();
        StreamConnectionBase.checkIOParams(aBytes, aOffset, aLength);
        if (aLength == 0)
        {
            StreamsLogger.Log(StreamsLogger.Level_2,
                              " -   InputStreamBase: read()");
            return 0;
        }

        int retValue = checkMarkAndResetCase(aBytes, aOffset, aLength);
        if ((StreamConnectionBase.EOF == retValue) || (iMarkNotSet != retValue))
        {
            StreamsLogger.Log(StreamsLogger.Level_2,
                              " -   InputStreamBase: read(): return: " + retValue);
            return retValue;
        }

        StreamsLogger.Log(StreamsLogger.Level_2,
                          " -   InputStreamBase: read(): calling fillBytes");
        return fillBytes(aBytes, aOffset, aLength);
    }

    protected synchronized int fillBytes(byte[] aBytes, int aOffset, int aLength)
    throws IOException
    {
        int bytesRead = 0;

        while (aLength > 0)
        {
            int numRead = readFromStreamBase(aBytes, aOffset, aLength);
            if (numRead <= 0)
            {
                if (bytesRead == 0)
                {
                    return -1;
                }
                break;
            }

            bytesRead += numRead;
            aOffset += numRead;
            aLength -= numRead;
        }
        return bytesRead;
    }

    protected synchronized int readFromStreamBase(byte[] aBytes, int aOffset,
            int aLength) throws IOException
    {
        if (aLength == 0)
        {
            return 0;
        }

        if (iBufferLength == iBufferOffset)
        {
            StreamsLogger
            .Log(StreamsLogger.Level_2,
                 "     InputStreamBase: readFromStreamBase(): Calling readBytes");
            iBufferLength = iStreamBase.readBytes(iBuffer);
            iBufferOffset = 0;
        }

        if (iBufferLength == StreamConnectionBase.EOF)
        {
            return StreamConnectionBase.EOF;
        }

        int nBytes;
        if ((iBufferOffset + aLength) <= iBufferLength)
        {
            nBytes = aLength;
        }
        else
        {
            nBytes = iBufferLength - iBufferOffset;
        }

        System.arraycopy(iBuffer, iBufferOffset, aBytes, aOffset, nBytes);
        iBufferOffset += nBytes;
        return nBytes;
    }

    /**
     * Closes the InputStream and notifies StreamConnectionBase that the stream
     * was closed.
     */
    public void close() throws IOException
    {
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " +   InputStreamBase: Close()");
        synchronized (iCloseLock)
        {
            if (!iIsClosed)
            {
                iIsClosed = true;
                iStreamBase.cancelRead();
                iStreamBase = null;
            }
        }
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " -   InputStreamBase: Close()");
    }

    protected boolean isClosed()
    {
        synchronized (iCloseLock)
        {
            return iIsClosed;
        }
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

    /**
     * returns true if mark and reset is supported, else returns false
     */
    public boolean markSupported()
    {
        return iSupportMarkAndReset;
    }

    /**
     * Marks the current position in this input stream. A subsequent call to the
     * reset method repositions this stream at the last marked position so that
     * subsequent reads re-read the same bytes. The readlimit arguments tells
     * this input stream to allow that many bytes to be read before the mark
     * position gets invalidated.
     * <p>
     * General contract of mark is that, if the method markSupported returns
     * true, the stream somehow remembers all the bytes read after the call to
     * mark and stands ready to supply those same bytes again if and whenever
     * the method reset is called. However, the stream is not required to
     * remember any data at all if more than readlimit bytes are read from the
     * stream before reset is called.
     * <p>
     * Marking a closed stream should not have any effect on the stream.
     * <p>
     *
     * <pre>
     * Scenarios for iMarkSet and iDataPresentInBuffer:
     *   iMarkSet       iDataPresentInBuffer
     *    false          does not matter          Mark is not set
     *    true           false                    Data is yet to be filled
     *    true           true                     Mark is valid
     * </pre>
     */
    public synchronized void mark(int aReadLimit)
    {
        StreamsLogger.Log(StreamsLogger.Level_2, " +   InputStreamBase: mark( "
                          + aReadLimit + " )");

        if (false == iSupportMarkAndReset || aReadLimit <= 0)
        {
            StreamsLogger.Log(StreamsLogger.Level_2,
                              " -   InputStreamBase: mark()");
            return;
        }

        if (iDataPresentInMark || iMarkSet)
        {
            StreamsLogger.Log(StreamsLogger.Level_3,
                              "     InputStreamBase: mark(): Mark Recalled");

            // If mark has already been called, we create a new buffer and fill
            // data into it.
            byte[] tempArray = null;

            // If the current read limit is greater than current mark limit.
            if (aReadLimit >= iMarkBuffer.length)
            {
                StreamsLogger
                .Log(
                    StreamsLogger.Level_3,
                    "     InputStreamBase: mark(): "
                    + "New mark limit > previous one. Creating fresh buffer.");

                // Allocate new buffer
                tempArray = new byte[aReadLimit];
            }
            else
                // new read buffer is less than the old one
            {
                StreamsLogger.Log(StreamsLogger.Level_3,
                                  "     InputStreamBase: mark(): "
                                  + "New mark limit < previous one.");

                StreamsLogger.Log(StreamsLogger.Level_3,
                                  "     InputStreamBase: mark(): Total Present: "
                                  + iMarkBufferTotal + " Buffer Offset: "
                                  + iMarkBufferOffset);

                int unreadBytes = iMarkBufferTotal - iMarkBufferOffset;
                int bufferSize = Math.max(unreadBytes, aReadLimit);
                tempArray = new byte[bufferSize];

                StreamsLogger.Log(StreamsLogger.Level_3,
                                  "     InputStreamBase: mark(): Created buffer of "
                                  + bufferSize + " bytes");
            }

            // Copy from iMarkBuffer to tempArray,
            // from current offset to end of buffer
            System.arraycopy(iMarkBuffer, iMarkBufferOffset, tempArray, 0,
                             iMarkBufferTotal - iMarkBufferOffset);

            iDataPresentInMark = false; // forces a new read next time.

            StreamsLogger.Log(StreamsLogger.Level_3,
                              "     InputStreamBase: mark(): From Offset: "
                              + iMarkBufferOffset + " to " + iMarkBufferTotal);

            iMarkSet = true; // Mark has now been set
            iMarkBuffer = tempArray; // Data being the same previous
            iMarkBufferTotal -= iMarkBufferOffset;
            iMarkBufferOffset = 0; // with offset back to zero
        }
        else
        {
            StreamsLogger.Log(StreamsLogger.Level_3,
                              "     InputStreamBase: mark(): Mark called first time");

            // There is no existing request to mark, so we create fresh buffer
            iMarkBuffer = new byte[aReadLimit];
            iMarkBufferTotal = 0; // Only increases when native read happens
            iMarkBufferOffset = 0;
            iMarkSet = true;
            iDataPresentInMark = false;
        }

        StreamsLogger.Log(StreamsLogger.Level_2,
                          " -    InputStreamBase: mark(): Values: " + " Buffer Length: "
                          + iMarkBuffer.length + " Buffer Offset: "
                          + iMarkBufferOffset + " Total Present: "
                          + iMarkBufferTotal + " Data Present?: "
                          + iDataPresentInMark);
    }

    /**
     * Resets the mark position to its initial state.
     *
     * @throws IOException
     *             if reset is called when there is no valid mark set
     */
    public synchronized void reset() throws IOException
    {
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " +    InputStreamBase: reset(): Mark Set? " + iMarkSet);
        if (!iMarkSet)
        {
            throw new IOException("Point of the mark operation is invalid");
        }

        StreamsLogger.Log(StreamsLogger.Level_2,
                          " -    InputStreamBase: reset()");

        iMarkBufferOffset = 0;
    }

    /**
     * CheckMarkAndReset is mainly responsible for managing mark and reset
     * functionality in InputStreams.
     *
     * @param aBytes
     *            the buffer into which data needs to be filled into
     * @param aOffset
     *            offset from which data needs to be filled into aBytes
     * @param aLength
     *            length of data to be filled from offset
     * @return amount of data filled into aBytes
     * @throws IOException
     *             in case there was problem reading data
     */
    private int checkMarkAndResetCase(byte[] aBytes, int aOffset, int aLength)
    throws IOException
    {
        StreamsLogger.Log(StreamsLogger.Level_3,
                          " +   InputStreamBase: checkMarkAndResetCase()");

        if (!iMarkSet)
        {
            StreamsLogger
            .Log(StreamsLogger.Level_3,
                 " -   InputStreamBase: checkMarkAndResetCase(): Mark not valid");

            return iMarkNotSet;
        }

        StreamsLogger.Log(StreamsLogger.Level_3,
                          "     InputStreamBase: checkMarkAndResetCase(): Values: "
                          + " iMarkSet: " + iMarkSet + " iDataPresentInMark: "
                          + iDataPresentInMark + " iMarkBufferOffset: "
                          + iMarkBufferOffset + " iMarkBufferTotal: "
                          + iMarkBufferTotal + " iMarkBufferLength: "
                          + iMarkBuffer.length);

        // First fill data into buffer if it has not been done already
        if (!iDataPresentInMark)
        {
            StreamsLogger
            .Log(StreamsLogger.Level_3,
                 "     InputStreamBase: checkMarkAndResetCase(): Less data present");

            int retVal = fillBytes(iMarkBuffer, iMarkBufferTotal,
                                   (iMarkBuffer.length - iMarkBufferTotal));

            if (retVal == StreamConnectionBase.EOF)
            {
                return StreamConnectionBase.EOF;
            }

            iMarkBufferTotal += retVal;
            iDataPresentInMark = true;
        }

        if (iMarkBufferOffset == iMarkBufferTotal)
        {
            StreamsLogger
            .Log(StreamsLogger.Level_3,
                 "     InputStreamBase: checkMarkAndResetCase(): reached end of buffer");
            return iMarkNotSet;
        }

        //When data requested goes beyond the read limit set
        if ((iMarkBufferOffset + aLength) > iMarkBufferTotal)
        {
            StreamsLogger
            .Log(
                StreamsLogger.Level_3,
                "     InputStreamBase: checkMarkAndResetCase(): Data asked is more than data present");

            int dataToCopy = iMarkBufferTotal - iMarkBufferOffset;
            System.arraycopy(iMarkBuffer, iMarkBufferOffset, aBytes, aOffset,
                             dataToCopy);
            iMarkBufferOffset += dataToCopy;

            //Rest of the data is fetched from native side
            int retVal = fillBytes(aBytes, aOffset + dataToCopy, aLength
                                   - dataToCopy);

            if (retVal == StreamConnectionBase.EOF)
            {
                retVal = 0;
            }

            StreamsLogger.Log(StreamsLogger.Level_3,
                              "     InputStreamBase: checkMarkAndResetCase(): Fillbytes return: "
                              + retVal + " MarkBufferOffset: "
                              + iMarkBufferOffset + " iMarkBufferTotal: "
                              + iMarkBufferTotal);

            //We should not invalidate the mark position in case less data was
            //retrieved as it might have been set beyond end of file.
            //If that was the case, then current offset would be same as data available
            if ((retVal + iMarkBufferOffset) > iMarkBuffer.length)
            {
                StreamsLogger
                .Log(StreamsLogger.Level_3,
                     "     InputStreamBase: checkMarkAndResetCase(): mark now invalid");

                iMarkBufferTotal = 0; //set it to some negative value
                iMarkBufferOffset = 0;
                iMarkSet = false; //invalidate mark
                iMarkBuffer = null; //let GC take the memory
                iDataPresentInMark = false; //we have gone beyond read limit
            }

            StreamsLogger.Log(StreamsLogger.Level_2,
                              " -   InputStreamBase: checkMarkAndResetCase(): Values: "
                              + " iMarkSet: " + iMarkSet
                              + " iDataPresentInMark: " + iDataPresentInMark
                              + " iMarkBufferOffset: " + iMarkBufferOffset
                              + " iMarkBufferTotal: " + iMarkBufferTotal);

            return retVal + dataToCopy;
        } //end if( (iMarkBufferOffset + aLength) > iMarkBufferTotal)

        //When data requested is less than or equal to data present
        if ((iMarkBufferOffset + aLength) <= iMarkBufferTotal)
        {
            StreamsLogger
            .Log(StreamsLogger.Level_3,
                 "     InputStreamBase: checkMarkAndResetCase(): Requested > total");

            System.arraycopy(iMarkBuffer, iMarkBufferOffset, aBytes, aOffset,
                             aLength);
            iMarkBufferOffset += aLength;

            StreamsLogger.Log(StreamsLogger.Level_3,
                              " -   InputStreamBase: checkMarkAndResetCase(): Values: iMarkSet: "
                              + iMarkSet + " iDataPresentInMark: "
                              + iDataPresentInMark + " iMarkBufferOffset: "
                              + iMarkBufferOffset + " iMarkBufferTotal: "
                              + iMarkBufferTotal);

            return aLength;
        }
        StreamsLogger.Log(StreamsLogger.Level_3,
                          " -   InputStreamBase: checkMarkAndResetCase()");
        return 0;
    }
}
