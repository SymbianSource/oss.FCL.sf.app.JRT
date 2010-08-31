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
* Description:  This class gets data from java classes and passes stream
*                to CMMASourceStream on C++ side
*
*/


package com.nokia.microedition.media.protocol;

import java.io.IOException;
import java.io.OutputStream;

public class OutputStreamWriter extends Thread
{
    /**
     * Private helper class to handle case when notify comes before
     * wait
     */
    private class WaitObject
    {
        static final int NOTIFIED = 1;
        public int iStatus = NO_ERROR;

        public synchronized void doWait()
        {
            if (iStatus == OutputStreamWriter.NO_ERROR)
            {
                try
                {
                    super.wait();
                }
                catch (InterruptedException ex)
                {
                    iStatus = OutputStreamWriter.GENERAL_ERROR;
                }
            }
        }

        public synchronized void doNotify()
        {
            super.notify();
            iStatus = NOTIFIED;
        }
    }
    /**
     * Size of the buffer (transferred bytes) the same size as in c++ side
     */
    private static final int BUFFER_MAXSIZE = 4096;

    /**
     * Status code. Negative values are error codes.
     * Values >= NO_ERROR are status codes.
     */
    private static final int NO_ERROR = 0;

    /**
     * Status code. Stream is ended.
     */
    private static final int COMPLETED = 2;

    /**
     * Status code. Stream is closed.
     */
    private static final int CLOSED = -2002;

    /**
     * Error status code.
     */
    private static final int GENERAL_ERROR = -2003;

    /**
     * IO error status code.
     */
    private static final int IO_ERROR = -2004;

    /**
     * This constant indicates native commit.
     */
    private static final int COMMIT = -10000;

    /**
     * Stream status.
     * >= 0 indicates no error
     * < 0 indicates error
     */
    private int iStatus = NO_ERROR;

    /**
     * Stream where output will be written.
     */
    private OutputStream iOutputStream;

    /**
     * Native handle.
     */
    private int iHandle;

    /**
     * Native event source handle.
     */
    private int iEventSourceHandle;

    /**
     * Used to wait native object to start writing.
     */
    private WaitObject iWaitObject = new WaitObject();

    /**
     * Used to wait in commit method until all data is read.
     */
    private WaitObject iCommitWaitObject = new WaitObject();

    /**
     * Runnable object that does stream commit.
     */
    private Runnable iRunnable;

    /**
     * Player handle is needed to check if native player object is deleted.
     */
    private int iPlayerHandle;

    /**
     * Constructor
     *
     * @param aOutputStream a Stream to be passed to C++ side
     * @param aEventsourceHandle handle to event source
     * @param aRunnable is called when native commit is done
     * @param aPlayerHandle Used to check if player is already deleted.
     */
    public OutputStreamWriter(OutputStream aOutputStream,
                              int aEventSourceHandle,
                              Runnable aRunnable,
                              int aPlayerHandle)
    {
        iOutputStream = aOutputStream;
        iEventSourceHandle = aEventSourceHandle;
        iRunnable = aRunnable;
        iPlayerHandle = aPlayerHandle;
    }

    /**
     * Sets handle to native object (CMMAOutputStream) and starts read thread.
     *
     * @param aHandle handle (CMMAOutputStream)
     */
    public void start(int aHandle)
    {
        if (aHandle <= 0)
        {
            // Handle must be positive integer.
            throw new IllegalArgumentException("Could not start player, " + aHandle);
        }
        iHandle = aHandle;
        start();
    }

    /**
     * From Thread
     */
    public void run()
    {
        // buffer for the data copied from native
        byte[] buffer = new byte[ BUFFER_MAXSIZE ];

        // used get read size from native
        int[] outputStreamSize = new int[ 1 ];

        // Wait until native side starts writing or
        // stream is closed.
        iWaitObject.doWait();
        if (iWaitObject.iStatus != WaitObject.NOTIFIED)
        {
            // some error has occured
            iStatus = iWaitObject.iStatus;
        }
        // Read data until stream is completed or error occures.
        while (iStatus >= NO_ERROR &&
                iStatus != COMPLETED)
        {
            // Read data from native.
            iStatus = _readData(iHandle,
                                iEventSourceHandle,
                                buffer,
                                outputStreamSize,
                                BUFFER_MAXSIZE,
                                iPlayerHandle);

            // Write data to Java stream.
            try
            {
                // Don't write if no bytes was read or error occured
                if (outputStreamSize[ 0 ] > 0 &&
                        iStatus >= NO_ERROR)
                {
                    iOutputStream.write(buffer, 0, outputStreamSize[ 0 ]);
                }
            }
            catch (IOException ioe)
            {
                // Error writing to stream
                iStatus = IO_ERROR;
            }
            catch (Throwable t)
            {
                // For example when OutputStream is closed
                iStatus = IO_ERROR;
            }
        }

        // Notify object waiting commit to complete.
        iCommitWaitObject.doNotify();
    }

    /**
     * Close stops run() thread
     */
    public void close()
    {
        iStatus = CLOSED;
        // release wait
        iWaitObject.doNotify();
    }

    /**
     * Close the OutputStream.
     * @throws IOException If an I/O error occurs
     */
    public void closeStream() throws IOException
    {
        iOutputStream.close();
    }

    /**
     * Wait until all data is written from native side and flush the stream.
     */
    public void commit() throws IOException
    {
        if (iStatus < NO_ERROR)
        {
            throw new IOException(
                "Current recording cannot be completed, Symbian OS error: " + iStatus);
        }
        iCommitWaitObject.doWait();
        if (iCommitWaitObject.iStatus != WaitObject.NOTIFIED)
        {
            // error happened in waiting
            iStatus = iCommitWaitObject.iStatus;
        }
        if (iStatus < NO_ERROR)
        {
            throw new IOException(
                "Current recording cannot be completed, Symbian OS error: " + iStatus);
        }
        iOutputStream.flush();
        iStatus = CLOSED;
    }

    /**
     * C++ side notifies that data is available. Java makes a byte array
     * and passes it to c++ side. Native side fills the byte array.
     *
     * @param alength native side notifies how many bytes are ready to be read
     * @param aStatus Status of the stream reading
     *
     */
    private void write(int aLength, int aStatus)
    {
        if (aStatus == COMMIT && iStatus >= NO_ERROR)
        {
            (new Thread(iRunnable)).start();
        }
        else if (this != null &&
                 iStatus >= NO_ERROR)
        {
            iStatus = aStatus;

            // notify the write-loop that data is available
            iWaitObject.doNotify();
        }
    }

    /**
     * _readData passes c++ side a byte array and native side
     * fills it with output stream data.
     *
     * @param aHandle Handle
     * @param aEventSourceHandle Event source
     * @param aData byte array for output stream data
     * @param aLength length of byte array
     * @param aStatus status
     * @return return value
     */
    static private native int _readData(int aHandle,
                                        int aEventSourceHandle,
                                        byte[] aData,
                                        int[] aOutputStreamSize,
                                        int aLength,
                                        int aPlayerHandle);

}

//end of file
