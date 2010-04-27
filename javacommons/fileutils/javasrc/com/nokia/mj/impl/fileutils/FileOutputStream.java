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
import java.io.OutputStream;

class FileOutputStream extends OutputStream
{
    private FileStreamHandler iDataSink;
    private final Object iCloseLock;
    private boolean iIsClosed;
    private byte[] iSingleByte;

    private FileOutputStream()
    {
        iCloseLock = null;
    }

    public FileOutputStream(FileStreamHandler aDataSink)
    {
        iDataSink = aDataSink;
        iCloseLock = new Object();
        iIsClosed = false;
        iSingleByte = new byte[1];
    }

    public void write(int aByte) throws IOException
    {
        iSingleByte[0] = (byte)(aByte & 0xFF);
        write(iSingleByte, 0, 1);
    }

    public synchronized void write(byte[] aBytes, int aOffset, int aLength)
    throws IOException
    {
        synchronized (iCloseLock)
        {
            if (iIsClosed)
            {
                throw new IOException("OutputStream closed");
            }
            FileStreamHandler.checkIOParams(aBytes, aOffset, aLength);

            // Data given by user is immediately sent to be written.
            iDataSink.writeBytes(aBytes, aOffset, aLength);
        }
    }

    public synchronized void flush() throws IOException
    {
        // Data written to native side immediately. Nothing to flush.
    }

    public void close() throws IOException
    {
        // Closing OutputStream without flushing is against spec.
        // See javadoc of DataOutputStream.
        flush();
        synchronized (iCloseLock)
        {
            if (!iIsClosed)
            {
                iIsClosed = true;
                iDataSink.stopWrite();
                iDataSink = null;
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
}