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

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InterruptedIOException;
import java.io.OutputStream;

import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.rt.support.Finalizer;
import com.nokia.mj.impl.rt.support.Jvm;

/**
 * FileStreamHandler helps in creation of InputStream and OutputStream and
 * provides data to them from the native side.
 */
class FileStreamHandler
{
    protected int iNativeHandle;

    protected FileInputStream iInputStream;

    protected FileOutputStream iOutputStream;

    protected DataInputStream iDataInputStream;

    protected DataOutputStream iDataOutputStream;

    // Synchronizes close and open operations.
    protected final Object iCloseOperation;

    protected boolean iIsClosed;

    private Finalizer iFinalizer;

    private FileStreamHandler()
    {
        iCloseOperation = new Object();
    }

    protected FileStreamHandler(String aFileName)
    {
        iNativeHandle = createNativePeer(aFileName);
        iCloseOperation = new Object();
        iIsClosed = false;
        iFinalizer = registerFinalize();
    }

    protected int createNativePeer(String aFileName)
    {
        return _createNativePeer(aFileName);
    }

    /**
     * Registers with Finalizer to call a method when the object gets collected
     * by GC
     *
     * @return Finalizer object that will be notified when GC happens
     */
    private Finalizer registerFinalize()
    {
        return new Finalizer()
        {
            public void finalizeImpl()
            {
                doFinalize();
            }
        };
    }

    private synchronized void doFinalize()
    {
        _dispose(iNativeHandle);
    }

    protected void stopRead()
    {
        synchronized (iCloseOperation)
        {
            _stopReading(iNativeHandle);
            closeInputStreams();
        }
    }

    protected void stopWrite()
    {
        synchronized (iCloseOperation)
        {
            closeOutputStreams();
            _stopWriting(iNativeHandle);
        }
    }

    protected InputStream openInputStream() throws IOException
    {
        synchronized (iCloseOperation)
        {
            if (null == iInputStream)
            {
                iInputStream = new FileInputStream(this);
            }
            else
            {
                throw new IOException("InputStream already open");
            }
        }
        return iInputStream;
    }

    protected DataInputStream openDataInputStream() throws IOException
    {
        synchronized (iCloseOperation)
        {
            if (null == iDataInputStream)
            {
                iDataInputStream = new DataInputStream(openInputStream());
            }
            else
            { // TCK2 compliance
                throw new IOException("InputStream already open");
            }
        }
        return iDataInputStream;
    }

    /**
     * This method is called by InputStreamBase whenever it needs to fill its
     * internal buffer with the next block of data.
     *
     * @param aBuffer
     *            is the buffer that needs to be filled with data.
     * @return the number of bytes read
     * @throws InterruptedIOException
     *             in case connection was closed while read was in progress.
     * @throws IOException
     *             in case where there were problems during read.
     */
    protected synchronized int readBytes(byte[] aBuffer, int aOffset,
                                         int aLength) throws InterruptedIOException, IOException
    {
        return _readData(iNativeHandle, aBuffer, aOffset, aLength);
    }

    /**
     * Closes all input streams
     *
     */
    protected void closeInputStreams()
    {
        synchronized (iCloseOperation)
        {
            try
            {
                if (iInputStream != null)
                {
                    iInputStream.close();
                    iInputStream = null;
                }

                if (iDataInputStream != null)
                {
                    iDataInputStream.close();
                    iDataInputStream = null;
                }
            }
            catch (IOException ex)
            {
                // Do nothing. Deleting Streams anyhow
            }
        }
    }

    protected OutputStream openOutputStream() throws IOException
    {
        synchronized (iCloseOperation)
        {
            if (null == iOutputStream)
            {
                iOutputStream = new FileOutputStream(this);
            }
            else
            { // TCK2 compliance
                throw new IOException("OutputStream already open");
            }
        }
        return iOutputStream;
    }

    protected DataOutputStream openDataOutputStream() throws IOException
    {
        synchronized (iCloseOperation)
        {
            if (null == iDataOutputStream)
            {
                iDataOutputStream = new DataOutputStream(openOutputStream());
            }
            else
            {
                throw new IOException("OutputStream already open");
            }
        }
        return iDataOutputStream;
    }

    /**
     * This method is called by OutputStreamBase whenever its internal buffer
     * overruns or when flush is called or when output stream is closed.
     *
     * @param aBytes
     *            the buffer that has to be written to native.
     * @param aOffset
     *            offset from which data needs to be written in aBytes.
     * @param aLength
     *            the length of data from offset that has to be written.
     * @throws IOException
     *             in case write operation fails.
     */
    protected synchronized void writeBytes(byte[] aBytes, int aOffset,
                                           int aLength) throws IOException
    {
        _writeData(iNativeHandle, aBytes, aOffset, aLength);
    }

    protected void flush()
    {
        _flush(iNativeHandle);
    }
    
    /**
     * Closes all output streams.
     *
     */
    protected void closeOutputStreams()
    {
        synchronized (iCloseOperation)
        {
            try
            {
                if (iOutputStream != null)
                {
                    iOutputStream.close();
                    iOutputStream = null;
                }

                if (iDataOutputStream != null)
                {
                    iDataOutputStream.close();
                    iDataOutputStream = null;
                }
            }
            catch (IOException ex)
            {
                // Do nothing. Deleting Streams anyhow
            }
        }
    }

    /**
     * Checks the validity of the parameters passed.
     *
     * @param aBytes
     *            byte array with data
     * @param aOffset
     *            offset from which operations have to start
     * @param aLength
     *            length of data from offset
     */
    static void checkIOParams(byte[] aBytes, int aOffset, int aLength)
    {
        if (aOffset < 0 || aLength < 0 || (aOffset + aLength) > aBytes.length)
        {
            throw new IndexOutOfBoundsException();
        }
    }

    protected void closeFileStream()
    {
        _closeFileStream(iNativeHandle);
    }

    protected void openFileForReading()
    {
        _openFileForReading(iNativeHandle);
    }

    protected void openFileForWriting(long aOffset)
    {
        _openFileForWriting(iNativeHandle, aOffset);
    }

    /**
     * Moves the current read position on the native side forward and back.
     * NOTE: Method needs to be called by InputStream only. Native only sets
     * read position forward or backward.
     *
     * @param aOffset
     *            relative offset to the current native position. Positive moves
     *            the offset forward and negative moves it back.
     * @return actual number of bytes that can be skipped.
     */
    protected long skip(long aOffset)
    {
        return _skip(iNativeHandle, aOffset);
    }

    protected long available()
    {
        return _available(iNativeHandle);
    }

    private native long _available(int aNativePeer);

    private native long _skip(int aNativePeer, long aOffset);

    private native int _readData(int aNativePeer, byte[] aBuffer, int aOffset,
                                 int aLength);

    private native void _writeData(int aNativePeer, byte[] aBytes, int aOffset,
                                   int aLength);

    private native void _flush(int aNativePeer);

    private native void _closeFileStream(int aHandle);

    private native void _openFileForReading(int aHandle);

    private native void _openFileForWriting(int aHandle, long aOffset);

    private native void _dispose(int aNativeHandle);

    private native void _stopReading(int iNativeHandle);

    private native void _stopWriting(int iNativeHandle);

    private native int _createNativePeer(String aFileName);

}
