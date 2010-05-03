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

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InterruptedIOException;
import java.io.OutputStream;

import javax.microedition.io.Connection;
import javax.microedition.io.Connector;

import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.utils.OsErrorMessage;

/**
 * StreamConnectionBase is an abstract class for all StreamConnection types
 * which allows them to manage InputStreams and OutputStreams. This class also
 * takes care of interacting with native objects which are designated to give
 * data to respective streams.
 * <p>
 * Derived classes must implement createNativePeer method. The return value is
 * expected to return a handle to the native object that will provide data in
 * furture calls.
 * <p>
 * Implementation also provides the facility to prefetch some amount of data
 * from the native side as soon as an InputStream is opened. This feature can be
 * disabled during contruction.
 */
public abstract class StreamConnectionBase
{
    public static final int EOF = -1;

    // States
    protected static final int INITIAL = 0;

    protected static final int OPEN = 1;

    protected static final int CLOSED = 2;

    protected static final int ERROR = 3;

    protected static final int PENDING_CLOSED = 4;

    protected int iState;

    protected boolean iSupportMarkAndReset;

    protected int iMode;

    protected InputStreamBase iInputStream;

    protected OutputStreamBase iOutputStream;

    protected DataInputStream iDataInputStream;

    protected DataOutputStream iDataOutputStream;

    protected Object iWriteOperation;

    protected Object iCloseOperation;

    protected Object iReadOperation;

    protected boolean iDoPrefetch;

    protected Prefetch iPrefetch;

    protected int iNativePeerHandle;

    // BUFFER_SIZE is configurable.
    protected int iBufferSize = 512;

    static
    {
        try
        {
            // Load the native libraries
            Jvm.loadSystemLibrary("javagcf");
        }
        catch (Exception e)
        {
            StreamsLogger.ELog("Unable to load native libraries!");
        }
    }

    public StreamConnectionBase(String aUri, int aMode, boolean aTimeouts)
    {
        this(aUri, aMode, aTimeouts, true, 512, true);
    }

    public StreamConnectionBase(String aUri, int aMode, boolean aTimeouts,
                                boolean aDoPrefetch)
    {
        this(aUri, aMode, aTimeouts, aDoPrefetch, 512, true);
    }

    public StreamConnectionBase(String aUri, int aMode, boolean aTimeouts,
                                boolean aDoPrefetch, int aReadBufferSize)
    {
        this(aUri, aMode, aTimeouts, aDoPrefetch, aReadBufferSize, true);
    }

    public StreamConnectionBase(String aUri, int aMode, boolean aTimeouts,
                                boolean aDoPrefetch, int aReadBufferSize, boolean aMarkSupported)
    {
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " +  StreamConnectionBase: Constructor");
        iCloseOperation = new Object();
        iWriteOperation = new Object();
        iReadOperation = new Object();
        iPrefetch = null;

        iState = INITIAL;
        iMode = aMode;
        iSupportMarkAndReset = aMarkSupported;
        iDoPrefetch = aDoPrefetch;
        iNativePeerHandle = 0;

        iBufferSize = aReadBufferSize;

        StreamsLogger.Log(StreamsLogger.Level_3,
                          "    StreamConnectionBase: Creating Native Peer");
        iNativePeerHandle = createNativePeer(aUri, aMode, aTimeouts);

        // If no native peer is created, there is no need to set its buffer
        if (iNativePeerHandle != 0)
        {
            StreamsLogger.Log(StreamsLogger.Level_3,
                              "    StreamConnectionBase: Setting native buffer size to "
                              + iBufferSize);
            _setNativeBufferSize(iNativePeerHandle, iBufferSize);
        }
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " -  StreamConnectionBase: Constructor");
    }

    /**
     * createNativePeer method needs to be implemented by every derived class.
     * Used to create a native peer to which calls to read and write are made.
     * JNI calls from StreamConnectionBase are directly translated to calls to
     * methods of NativeStreamBase object on the native side and native
     * peer is handle to object of this class.
     *
     * @param aUri
     *            contains the url passed to Connector.open call.
     * @param aMode
     *            is the mode with which the connection was opened. This is the
     *            second parameter in Connector.open.
     * @param aTimeouts
     *            is the timeout value that might be needed for some connection
     *            types. Passed as third parameter to Connector.open.
     * @return handle to the native peer created should be returned. If there is
     *         a need such that there must be a pure java implementation and
     *         native peer should be avoided, then the method must return 0.
     *
     * Note: In case of pure java implementations, Developer must ensure that
     * the methods that go to native side to fetch data are overwritten.
     *
     * @see nativeRead() nativeWrite() cancelRead() cancelWrite()
     */
    protected abstract int createNativePeer(String aUri, int aMode,
                                            boolean aTimeouts);

    /**
     * Creates an InputStream object. Also starts prefetch.
     *
     * @return a new instance on InputStreamBase.
     * @throws IOException
     *             in case there is an attempt to create multiple InputStreams
     *             from the same connection. Only one InputStream is allowed for
     *             every StreamConnection type.
     */
    public InputStream openInputStream() throws IOException
    {
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " +  StreamConnectionBase: openInputStream()");
        checkMode(Connector.READ);

        synchronized (iReadOperation)
        {
            synchronized (iCloseOperation)
            {
                ensureOpen();
                if (iInputStream == null)
                {
                    iInputStream = new InputStreamBase(this, iCloseOperation,
                                                       iSupportMarkAndReset);
                }
                else
                { // TCK2 compliance
                    StreamsLogger
                    .WLog("    StreamConnectionBase: openInputStream() "
                          + "Attempt to open second InputStream. IOException");
                    throw new IOException("InputStream already open");
                }

                if (iDoPrefetch)
                {
                    StreamsLogger.Log(StreamsLogger.Level_2,
                                      "    StreamConnectionBase: openInputStream(): "
                                      + "Starting Prefetch");
                    startPrefetch();
                }
            }
        }
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " -  StreamConnectionBase: openInputStream()");
        return iInputStream;
    }

    /**
     * Creates a DataInputStream object. Also starts prefetch.
     *
     * @return a new instance on DataInputStreamBase.
     * @throws IOException
     *             in case there is an attempt to create multiple InputStreams
     *             from the same connection. Only one InputStream is allowed for
     *             every StreamConnection type.
     */
    public DataInputStream openDataInputStream() throws IOException
    {
        checkMode(Connector.READ);
        synchronized (iCloseOperation)
        {
            ensureOpen();
            if (iDataInputStream == null)
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
     * Creates an OutputStream object. Also starts prefetch.
     *
     * @return a new instance on OutputStreamBase.
     * @throws IOException
     *             in case there is an attempt to create multiple OutputStreams
     *             from the same connection. Only one OutputStream is allowed
     *             for every StreamConnection type.
     */
    public OutputStream openOutputStream() throws IOException
    {
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " +  StreamConnectionBase: openOutputStream()");
        checkMode(Connector.WRITE);
        synchronized (iCloseOperation)
        {
            ensureOpen();
            if (iOutputStream == null)
            {
                iOutputStream = new OutputStreamBase(this, iCloseOperation,
                                                     true);
            }
            else
            { // TCK2 compliance
                StreamsLogger
                .WLog("    StreamConnectionBase: openOutputStream() "
                      + "Attempt to open second OutputStream. IOException");
                throw new IOException("OutputStream already open");
            }
        }
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " -  StreamConnectionBase: openOutputStream()");
        return iOutputStream;
    }

    /**
     * Creates an DataOutputStream object. Also starts prefetch.
     *
     * @return a new instance on DataOutputStream.
     * @throws IOException
     *             in case there is an attempt to create multiple
     *             DataOutputStream from the same connection. Only one
     *             DataOutputStream is allowed for every StreamConnection type.
     */
    public DataOutputStream openDataOutputStream() throws IOException
    {
        checkMode(Connector.WRITE);
        synchronized (iCloseOperation)
        {
            ensureOpen();
            if (iDataOutputStream == null)
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
     * Checks for the Connection mode to be correct. This method is called when
     * opening either Input or Output streams.
     *
     * <pre>
     * Values of modes:
     *     Connector.READ       (1) =&gt; 0 1
     *     Connector.WRITE      (2) =&gt; 1 0
     *     Connector.READ_WRITE (3) =&gt; 1 1
     * </pre>
     *
     * <p>
     *
     * <pre>
     *
     * Usage:
     *     checkMode(Connector.READ) to check if the connection has been opened
     *     with read permissions. There is no need to check explicity for
     *     READ_WRITE mode as it implies READ as well.
     * </pre>
     *
     * @param aAction
     *            is the minimum valid mode the the Connection must have been
     *            opened in. Connector.READ should be given instead of
     *            Connector.READ_WRITE.
     * @throws IOException
     *             when the current connection mode does not match the required
     *             mode.
     */
    protected final void checkMode(int aAction) throws IOException
    {
        if ((iMode & aAction) == 0)
        {
            throw new IOException("Bad action for access mode");
        }
    }

    /**
     * Checks if the Connection is still open.
     *
     * @throws IOException
     *             is the connection has been closed.
     */
    protected final void ensureOpen() throws IOException
    {
        synchronized (iCloseOperation)
        {
            if (iState == CLOSED)
            {
                throw new IOException("Connection Closed");
            }
        }
    }

    /**
     * Closes all input streams
     *
     */
    protected void closeInputStreams()
    {
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " +  StreamConnectionBase: closeInputStreams()");
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
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " -  StreamConnectionBase: closeInputStreams()");
    }

    /**
     * Closes all output streams.
     *
     */
    protected void closeOutputStreams()
    {
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " +  StreamConnectionBase: closeOutputStreams()");
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
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " -  StreamConnectionBase: closeOutputStreams()");
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
    public int readBytes(byte[] aBuffer) throws InterruptedIOException,
                IOException
    {
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " +  StreamConnectionBase: readBytes()");
        int result = 1;
        synchronized (iReadOperation)
        {
            // Prevents multiple reads
            // Check if the connection is still open.
            if (iDoPrefetch)
            {
                // If prefetch is turned on
                StreamsLogger.Log(StreamsLogger.Level_3,
                                  "    StreamConnectionBase: readBytes(): "
                                  + "Getting Prefetched Data");
                result = iPrefetch.getPrefetched(aBuffer);
            }
            else
            {
                StreamsLogger.Log(StreamsLogger.Level_3,
                                  "    StreamConnectionBase: readBytes(): "
                                  + "Getting data directly from native");
                // If prefetch is turned off
                result = readData(aBuffer);
            }

            if (result == 0)
            {
                StreamsLogger.Log(StreamsLogger.Level_3,
                                  "    StreamConnectionBase: readBytes(): "
                                  + "Read result = EOF");
                return EOF;
            }

            if (result < 0)
            {
                throw new IOException(OsErrorMessage.getErrorMessage(result));
            }
        }

        StreamsLogger.Log(StreamsLogger.Level_2,
                          " -  StreamConnectionBase: readBytes()");
        return result;
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
    public void writeBytes(byte[] aBytes, int aOffset, int aLength)
    throws IOException
    {
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " +  StreamConnectionBase: writeBytes()");
        synchronized (iWriteOperation)
        {
            int nLeftToWrite = aLength;
            int written = 0;
            int totalWritten = 0;

            while (nLeftToWrite > 0)
            {
                StreamsLogger.Log(StreamsLogger.Level_3,
                                  "   StreamConnectionBase: writeBytes(): "
                                  + " Calling writeData(): " + " DataLength: "
                                  + aBytes.length + " Offset: " + aOffset
                                  + totalWritten + " Length: " + nLeftToWrite);

                written = writeData(aBytes, aOffset + totalWritten,
                                    nLeftToWrite);

                if (written < 0)
                {
                    throw new IOException(OsErrorMessage
                                          .getErrorMessage(written));
                }

                // check error status here
                nLeftToWrite -= written;
                totalWritten += written;

            }
        }
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " -  StreamConnectionBase: writeBytes()");
    }

    /**
     * writeBytes calls this method when it has to write some data to native
     * side. In case we decide to have pure java implementation, we need to
     * override this method and manage the buffer sent.
     *
     * @param aBytes
     *            the buffer with data that needs to be written
     * @param aOffset
     *            offset from which data needs to be written in aBytes.
     * @param aLength
     *            the length of data from offset that has to be written.
     * @return the amount of data successfully written or error code.
     */
    public int writeData(byte[] aBytes, int aOffset, int aLength)
    {
        int err = _writebytes(iNativePeerHandle, aBytes, aOffset, aLength);
        return err;
    }

    /**
     * readBytes calls this method when it has to read some data from native
     * side. In case we decide to have pure java implementation, we need to
     * override this method and manage the buffer sent.
     *
     * @param aBytes
     *            the buffer with data that needs to be written
     * @return the amount of data successfully read or error code.
     */
    public int readData(byte[] aBytes)
    {
        int err = _readbytes(iNativePeerHandle, aBytes);
        return err;
    }

    /**
     * Called when the read operations have stopped i.e when InputStream is
     * closed. This is done to notify native peer that read operations have been
     * cancelled and native needs to take appropriate actions.
     *
     * In case we plan to have pure java implementation in jsr, then we must
     * override this method to prevent it from going to native side.
     *
     */
    public void cancelRead()
    {
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " +  StreamConnectionBase: cancelRead()");

        if (iDoPrefetch)
        {
            iPrefetch.close();
            iPrefetch = null;
        }

        stopReading();

        closeInputStreams();
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " -  StreamConnectionBase: cancelRead()");
    }

    public void cancelWrite()
    {
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " +  StreamConnectionBase: cancelWrite()");
        stopWriting();
        closeOutputStreams();

        StreamsLogger.Log(StreamsLogger.Level_2,
                          " -  StreamConnectionBase: cancelWrite()");
    }

    public void stopWriting()
    {
        // To be overridden by client if there is a need to get notification in
        // java when there is no native peer.
        _stopwriting(iNativePeerHandle);
    }

    public void stopReading()
    {
        // To be overridden by client if there is a need to get notification in
        // java when there is no native peer.
        _stopreading(iNativePeerHandle);
    }

    /**
     * Checks to see if streams are closed
     *
     * @return true in case all streams have been closed. False otherwise.
     */
    protected boolean isStreamsClosed()
    {
        if (iInputStream == null && iOutputStream == null)
        {
            return true;
        }

        return false;
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

    /**
     * Called in case application calls available() method on an InputStream
     * before first read. InputStream will not have fetched the first chunk of
     * data. If data has been prefetched, then we return the amount prefetched
     *
     * @return number of bytes available in prefetch buffer. Zero in case
     *         prefetch was not enabled.
     * @throws IOException
     *             in case there was an error in previous prefetch
     */
    final int available() throws IOException
    {
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " +  StreamConnectionBase: available()");
        int readOperationResult = 0;

        if (iDoPrefetch)
        {
            readOperationResult = iPrefetch.getResult();
            if (readOperationResult < 0)
            {
                throw new IOException(OsErrorMessage
                                      .getErrorMessage(readOperationResult));
            }
        }
        else
        {
            StreamsLogger.Log(StreamsLogger.Level_2,
                              " StreamConnectionBase: callling _available()");
            readOperationResult = _available(iNativePeerHandle);
        }
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " -  StreamConnectionBase: available()");
        return readOperationResult;
    }

    /**
     * Close called on the StreamConnection type. It closes all open input and
     * output streams.
     *
     * @throws IOException
     *             when there was a problem in closing.
     */
    public void close() throws IOException
    {
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " +  StreamConnectionBase: close()");

        synchronized (iCloseOperation)
        {
            if (iState != CLOSED)
            {
                iState = CLOSED;
            }
        }

        StreamsLogger.Log(StreamsLogger.Level_2,
                          " -  StreamConnectionBase: close()");
    }

    /**
     * Creates and starts a new prefetch thread.
     *
     */
    public void startPrefetch()
    {
        if (iPrefetch == null)
        {
            iPrefetch = new Prefetch(this);
        }
    }

    // First call to readBytes indicates a fresh start.
    protected native int _readbytes(int aHandle, byte[] aBuffer);

    protected native int _writebytes(int aHandle, byte[] aBuffer, int aOffset,
                                     int aLength);

    // Stop reading asks native peer to stop all reading and reset its offsets.
    // So that next call of readBytes that it encounters will start afresh
    protected native void _stopreading(int aHandle);

    protected native void _stopwriting(int aHandle);

    protected native void _setNativeBufferSize(int aHandle, int aSize);

    protected native int _available(int aHandle);
}

/**
 * Prefetch is a thread that is started as soon as an InputStream is opened.
 * Purpose of the thread is to prefetch data from the native side before the
 * first read happens.
 * <p>
 * Exposes 2 methods, getPrefetched() and getResult() to query for the results.
 */
class Prefetch implements Runnable
{
    private final byte[] iBuffer;

    private final int STARTED = 0;

    private final int RUNNING = 1;

    private final int CANCELLED = 2;

    private final int CLOSED = 3;

    private int iState;

    private int iReadResult;

    private StreamConnectionBase iEndpoint;

    private Object iPrefetchLock = new Object();

    private Object iStartLock = new Object();

    private Object iPrefetchDoneLock = new Object();

    private Thread me;

    private boolean iWaitingToPick = false;

    Prefetch(StreamConnectionBase aEndpoint)
    {
        StreamsLogger.Log(StreamsLogger.Level_2, " +  Prefetch: Constructor");
        iEndpoint = aEndpoint;
        iBuffer = new byte[iEndpoint.iBufferSize];
        me = new Thread(this);
        iState = STARTED;
        iReadResult = 0;

        synchronized (iStartLock)
        {
            try
            {
                me.start();
                StreamsLogger.Log(StreamsLogger.Level_3,
                                  "    Prefetch: Constructor: "
                                  + "Waiting for Thread to start");

                iStartLock.wait();
                // Wait untill the thread has started properly
            }
            catch (Exception e)
            {
                StreamsLogger.ELog("Prefetch Constructor! " + e);
            }
        }
        iState = RUNNING;
        StreamsLogger.Log(StreamsLogger.Level_2, " -  Prefetch: Constructor");
    }

    /**
     * Reads next chunk of data from the native side and waits until it is taken
     * from it. Thread runs until the InputStream has not been closed.
     */
    public void run()
    {
        StreamsLogger.Log(StreamsLogger.Level_2, " +   Prefetch: run()");
        while (iState != CLOSED)
        {
            synchronized (iPrefetchLock)
            {
                synchronized (iPrefetchDoneLock)
                {
                    StreamsLogger.Log(StreamsLogger.Level_3,
                                      "     Prefetch: run(): "
                                      + "Notifying iPrefetchDoneLock");
                    iPrefetchDoneLock.notify();
                }

                if (iState == STARTED)
                {
                    synchronized (iStartLock)
                    {
                        StreamsLogger.Log(StreamsLogger.Level_3,
                                          "     Prefetch: run(): Notifying iStartLock");
                        // Thread has not started. So constructor can exit
                        iStartLock.notify();
                    }
                    iState = RUNNING;
                }

                if (iState != CANCELLED)
                {
                    // StreamsLogger .Log(StreamsLogger.Level_3," Prefetch:
                    // run(): Calling StreamConnectionBase.readData,this =
                    // "+this+"state );
                    if (iEndpoint != null)
                        iReadResult = iEndpoint.readData(iBuffer);
                }

                if (iState == CANCELLED)
                {
                    break;
                }

                try
                {
                    StreamsLogger.Log(StreamsLogger.Level_3,
                                      "     Prefetch: run(): "
                                      + "Waiting for someone to pick up data");
                    iWaitingToPick = true;
                    iPrefetchLock.wait();
                    iWaitingToPick = false;
                    // Wait until the buffer is taken from inputstream
                    StreamsLogger.Log(StreamsLogger.Level_3,
                                      "     Prefetch: run(): Data taken");
                }
                catch (InterruptedException ex)
                {
                    StreamsLogger.ELog("Prefetch: Run()! " + ex);
                }
            }
        }
        StreamsLogger.Log(StreamsLogger.Level_2, " -   Prefetch: run()");
    }

    /**
     * Method that returns the prefetched data. This unlocks the iPrefetchLock
     * in run() method so that the next block of data can be prefetched from
     * native side.
     *
     * @param aBytes
     *            the buffer that needs to be filled with prefetched data.
     * @return the amount of data read.
     * @throws InterruptedIOException
     *             in case the connection was in between.
     */
    public synchronized int getPrefetched(byte[] aBytes)
    throws InterruptedIOException
    {
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " +   Prefetch: getPrefetched()");
        int result = 0;
        synchronized (iPrefetchDoneLock)
        {
            synchronized (iPrefetchLock)
            {
                if (iState == CANCELLED)
                {
                    StreamsLogger.WLog("     Prefetch: getPrefetched(): "
                                       + "Read was cancelled");
                    throw new InterruptedIOException();
                }

                result = iReadResult;
                if (result >= 0)
                {
                    System.arraycopy(iBuffer, 0, aBytes, 0,
                                     iEndpoint.iBufferSize);
                    iReadResult = 0;
                }

                StreamsLogger.Log(StreamsLogger.Level_3,
                                  "     Prefetch: getPrefetched(): "
                                  + "Notifying iPrefetchLock");
                iPrefetchLock.notifyAll();
            }

            try
            {
                StreamsLogger.Log(StreamsLogger.Level_3,
                                  "     Prefetch: getPrefetched(): "
                                  + "Waiting for prefetch to resume");
                iPrefetchDoneLock.wait();
            }
            catch (Exception e)
            {
                StreamsLogger.ELog("Prefetch: getPrefetched()!: " + e);
            }
        }
        StreamsLogger.Log(StreamsLogger.Level_2,
                          " -   Prefetch: getPrefetched(): result: " + result);
        return result;
    }

    /**
     * Called only when available is called before first InputStream.read In
     * that case we need to return the value of the previously fetched block.
     *
     * @return read result of prefetched data
     */
    public int getResult()
    {
        return iReadResult;
    }

    /**
     * Changes the state of the thread to Cancelled. This causes the thread to
     * stop.
     *
     */
    public void close()
    {
        StreamsLogger.Log(StreamsLogger.Level_2, " +   Prefetch: close()");
        iState = CANCELLED;
        if (iWaitingToPick)
        {
            StreamsLogger.Log(StreamsLogger.Level_3,
                              "     Prefetch: close(): Waiting for block");
            synchronized (iPrefetchLock)
            {
                StreamsLogger.Log(StreamsLogger.Level_3,
                                  "     Prefetch: close(): Notified");
                iPrefetchLock.notify();
            }
        }
        me = null;
        iEndpoint = null;
        StreamsLogger.Log(StreamsLogger.Level_2, " -   Prefetch: close()");
    }
}
