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
        byte[] tmpBuffer = new byte[ BUFFER_SIZE ];
        int bytesInBuffer = BUFFER_SIZE;

        // Read into iBuffer first if iBuffer doesn't already contain
        // first 256 bytes
        if (iBuffer == null)
        {
            bytesInBuffer = readAndBuffer(tmpBuffer, 0, 256);

            if (bytesInBuffer < 0)
            {
                throw new IOException();
            }
        }
        else if (iBuffer.length < 256)
        {
            bytesInBuffer = readAndBuffer(tmpBuffer, iBuffer.length, (256 - iBuffer.length));

            if (bytesInBuffer == -1)
            {
                bytesInBuffer = iBuffer.length;
            }
        }

        // tmpBuffer will be overwritten here
        System.arraycopy(iBuffer, 0,
                         tmpBuffer, 0, bytesInBuffer);

        return tmpBuffer;
    }


    public int readAndBuffer(byte[] aBuffer, int aOffset, int aLength) throws IOException
    {
        int bytesInBuffer = iSourceStream.read(aBuffer, aOffset, aLength);

        if (iBuffer == null)
        {
            iBuffer = new byte[bytesInBuffer];

            System.arraycopy(aBuffer, aOffset,
                             iBuffer, 0, bytesInBuffer);
        }
        else if (bytesInBuffer >= 0)
        {
            byte[] tempBuffer = iBuffer;
            iBuffer = new byte[iBuffer.length + bytesInBuffer];

            System.arraycopy(tempBuffer, 0,
                             iBuffer, 0, tempBuffer.length);
            System.arraycopy(aBuffer, aOffset,
                             iBuffer, tempBuffer.length, bytesInBuffer);
            tempBuffer = null; // relese tempBuffer
        }

        return bytesInBuffer;
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
