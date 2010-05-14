/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SourceStream that buffers data from another SourceStream.
*
*/


package com.nokia.microedition.media;

import java.io.IOException;
import javax.microedition.media.protocol.SourceStream;

/**
 * SourceStream that buffers data from another SourceStream.
 *
 */
public class BufferSourceStream extends InputStreamSourceStream
{
    // Deferred buffer size
    private static final int BUFFER_SIZE = 5120;

    // Stream to buffer
    SourceStream iSourceStream;

    byte[] iBuffer;

    // stream read position
    int iPos;

    /**
     * Constructor.
     * @param aInputStream An InputStream
     */
    public BufferSourceStream(SourceStream aSourceStream)
    {
        iSourceStream = aSourceStream;
    }

    /**
     * Return header data.
     * @return header data
     */
    public byte[] getHeader() throws IOException
    {
        // Read header if this method is called for the first time
        if (iBuffer == null)
        {
            byte[] tmpBuffer = new byte[ BUFFER_SIZE ];

            int bytesInBuffer = iSourceStream.read(tmpBuffer, 0, BUFFER_SIZE);

            // bytesInBuffer is the total number of bytes read into the buffer,
            // or -1 if there is no more data because the end of the stream has
            // been reached.
            if (bytesInBuffer >= 0)
            {
                iBuffer = new byte[ bytesInBuffer ];

                System.arraycopy(tmpBuffer, 0,
                                 iBuffer, 0, bytesInBuffer);
            }
            else
            {
                throw new IOException();
            }
        }
        return iBuffer;
    }

    /**
     * Read from Input Stream
     * @param buffer Input Stream
     * @param offset where reading starts
     * @param length length of read data
     * @return bytes read
     * @throws IOException
     */
    public int read(byte[] aBuffer, int aOffset, int aLength) throws IOException
    {
        // bytes read from internal buffer
        int bytesFromBuffer = 0;


        if ((iBuffer != null) &&   // read from iBuffer if getHeader is called
                (iPos < iBuffer.length))
        {
            // Need to read from buffer
            if (aLength < iBuffer.length - iPos)
            {
                // aLength bytes can be read from buffer
                bytesFromBuffer = aLength;
            }
            else
            {
                // aLength cannot be read from buffer
                // read all there is available
                bytesFromBuffer = iBuffer.length - iPos;
            }

            System.arraycopy(iBuffer, iPos,
                             aBuffer, aOffset, bytesFromBuffer);

            // move stream position
            iPos += bytesFromBuffer;
            return bytesFromBuffer;
        }

        // bytes read from iSourceStream
        int bytesFromStream = 0;
        // Check if bytes are needed from SourceStream
        if (bytesFromBuffer < aLength)
        {
            bytesFromStream = iSourceStream.read(aBuffer,
                                                 bytesFromBuffer,
                                                 aLength - bytesFromBuffer);
            if (bytesFromStream != -1)
            {
                // move stream position, if not end of stream
                iPos += bytesFromStream;
            }
        }

        return bytesFromStream;
    }
}

// End of File
