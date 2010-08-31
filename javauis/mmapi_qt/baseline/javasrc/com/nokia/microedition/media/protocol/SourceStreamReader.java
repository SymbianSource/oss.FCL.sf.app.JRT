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
* Description:  SourceStreamReader gets data from java classes and passes
*                stream to CMMASourceStream on C++ side
*
*/


package com.nokia.microedition.media.protocol;

import java.io.IOException;
import javax.microedition.media.protocol.SourceStream;
import javax.microedition.media.Controllable;
import javax.microedition.media.Control;
import com.nokia.microedition.media.SeekControl;
import com.nokia.microedition.media.InputStreamSeekControl;
import com.nokia.mj.impl.utils.Logger;

public class SourceStreamReader extends Thread
{
    Thread t1 = null;
    // these states are the same in c++ side
    private static final int NO_ERROR = 0;
    private static final int ERR_GENERAL = -2;  // KErrGeneral
    private static final int ERR_EOF = -25;   // KErrEof
    private static final int READ_ALL = -1;
    private static final int MORE_DATA = 3;
    private static final int COMPLETED = 4;
    private static final int CLOSED = -45;   // KErrSessionClosed

    // size of the buffer (transferred bytes)
    // MMF currently requests data in 4k blocks
    private static final int BUFFER_SIZE = 4096;

    private static final String SEEK_CONTROL = "SeekControl";

    private SourceStream iSourceStream;
    private byte[] iBuffer;
    private int iLength;            // Length of stream c++ requested
    private int iHandle;            // handle to CMMASourceStream
    private int iStatus = NO_ERROR;       // source stream status
    private int iEventSourceHandle;
    private boolean iIsActive;
    private int iPlayerHandle;

    private Object iWaitObject = new Object();

    // DataSource that SourceStream belongs to
    private Controllable iControllable;

    /**
     * Constructor
     *
     * @param aSourceStream a Stream to be passed to C++ side
     * @param aEventSourceHandle handle to the event source
     */
    public SourceStreamReader(SourceStream aSourceStream,
                              Controllable aControllable,
                              int aEventSourceHandle,
                              int aPlayerHandle)
    {
        iSourceStream = aSourceStream;
        iControllable = aControllable;
        iEventSourceHandle = aEventSourceHandle;
        iBuffer = new byte[ BUFFER_SIZE ];
        iPlayerHandle = aPlayerHandle;
    }

    /**
     * Sets handle to CMMASourceStream
     *
     * @param aHandle handle to CMMASourceStream
     */
    public void setHandle(int aHandle)
    {
        iHandle = aHandle;
    }

    /**
     * Run method for a thread which writes data to C++
     */
    public void run()
    {
        int length = 0;
        do
        {
            Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"Source stream reader run()");
            iStatus = MORE_DATA;
            try
            {
                length = iSourceStream.read(iBuffer, 0, iLength);
            }
            catch (IOException ioe)
            {
                Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"Source stream reader run(), io exception");
                ioe.printStackTrace();
                iStatus = ERR_GENERAL;
            }
            Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"Source stream reader run(), length = "+length);
            if (iStatus == CLOSED)
            {
                return;
            }
            // no more data because end of file has been reach
            if (length == -1)
            {
                Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"no more data because end of file has been reach");
                iStatus = COMPLETED;
            }
            // synchronized (iWaitObject)
            // {
            Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"Source stream reader run(), calling jni _write"+"status =" + iStatus + "length =" + length);

            int err = _write(iHandle,
                             iEventSourceHandle,
                             iBuffer,
                             length,
                             iStatus,
                             iPlayerHandle);
            Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"Source stream reader run(), calling jni _write ret = "+err);
            if (err != 0)
            {
                // error in writing, closing thread
                iIsActive = false;
                iStatus = CLOSED;
                break;
            }
            // wait for next native read
            // try
            // {
            Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"Source stream reader run(), calling wait()");
            Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"Source stream reader run(), calling wait() thread is =" +Thread.currentThread().getName());
            // iWaitObject.wait();
            //}
            /*catch (InterruptedException ex)
            {
                // MIDP's object does not have interrupt
            }*/
            // }
        }
        while (length>0);
    }

    /**
     * Close stops run() thread
     */
    public void close()
    {
        iIsActive = false;
        iStatus = CLOSED;

        // If Controllable has SeekControl it must be closed
        Control control = iControllable.getControl(SEEK_CONTROL);
        if (control != null)
        {
            SeekControl sc = (SeekControl)control;
            sc.close();
        }

        synchronized (iWaitObject)
        {
            iWaitObject.notify();
        }
    }

    /**
     * Resets streams to point start of the media. Player deallocate needs
     * this.
     */
    public void resetStream()
    {
        try
        {
            SeekControl control = (SeekControl)iControllable.getControl(SEEK_CONTROL);
            control.seek(0);
        }
        catch (IOException ioe)
        {
            // Stream may be broken, we can't do anything for it here.
            // It will be noticed by the player when changing state from deallocate
            Logger.ELOG(Logger.EJavaMMAPI,
                        "SourceStreamReader: resetStream: ", ioe);
        }
    }


    /**
     * Native side request data from Java with read method.
     *
     * @param aBytes native side notifies how many bytes should be sent to it
     * @param aStatus Status of the stream reading
     */
    private void read(int aBytes, int aStatus)
    {
        // Try to read but SourceStream is already completed or closed?
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"SourceStreamReader::  read() , callback detected ,iStatus  = "+iStatus+"  aBytes = "+aBytes);
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"thread id = "+Thread.currentThread().getName());
        if (iStatus == CLOSED)
        {
            // Inform the native object; attempting to read pass the end of the stream
            _write(iHandle, iEventSourceHandle, new byte[ 0 ], 0, ERR_EOF, iPlayerHandle);
            return;
        }

        if (aBytes == READ_ALL)
        {
            iLength = BUFFER_SIZE;
        }
        else
        {
            iLength = aBytes;
        }

        if (aBytes > iBuffer.length)
        {
            // native request larger buffer
            iBuffer = new byte[ aBytes ];
        }

        if (iStatus == COMPLETED)
        {
            // Stream is already read ones and need to be seeked to start
            Control control = iControllable.getControl(SEEK_CONTROL);
            if (control != null)
            {
                // start new seak thread, thread will notify iWaitObject
                // when completed
                (new SeekThread(iWaitObject,
                                (SeekControl)control)).start();
            }
            else
            {
                // stream is not seekable, so informing native object
                _write(iHandle, iEventSourceHandle, new byte[ 0 ], 0,
                       ERR_EOF, iPlayerHandle);
            }
        }
        else if (iIsActive)
        {
            // notify to while() write -loop that data is requested
            Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"SourceStreamReader::  read() before sync(iWaitObjet() ");
            synchronized (iWaitObject)
            {
                Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"SourceStreamReader::  read() notifying iWaitObject");
                iWaitObject.notify();
            }
        }
        else
        {
            Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"SourceStreamReader, creating new Thread");
            iIsActive = true;
            t1 = new Thread(this);
            t1.start();
            // start();
        }

    }


    /**
     * _write writes data to the C++ side
     *
     * @param aHandle Handle
     * @param aEventSourceHandle EventSourceHandle
     * @param aData Source stream
     * @param aBytesRead how many bytes is read
     * @param aStatus stream status
     * @return return value
     */
    static private native int _write(int aHandle,
                                     int aEventSourceHandle,
                                     byte[] aData,
                                     int aBytesRead,
                                     int aStatus,
                                     int aPlayerHandle);

}

// end of file
