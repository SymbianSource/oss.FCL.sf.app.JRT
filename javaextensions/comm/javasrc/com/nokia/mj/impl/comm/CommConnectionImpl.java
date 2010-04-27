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


package com.nokia.mj.impl.comm;

import java.io.DataOutputStream;
import java.io.OutputStream;
import java.io.DataInputStream;
import java.io.InputStream;
import java.io.IOException;

import javax.microedition.io.CommConnection;

import com.nokia.mj.impl.utils.OsErrorMessage;
import com.nokia.mj.impl.gcf.utils.StreamConnectionBase;
import com.nokia.mj.impl.rt.support.Finalizer;
import com.nokia.mj.impl.utils.Logger;


/**
 * Comm Connection Implementation class.
 */

public final class CommConnectionImpl extends StreamConnectionBase implements
        CommConnection
{
    private int[] mParamterValues = new int[7];

    private boolean mConnectionIsOpen;

    private CommConnectionPort mCommPort;

    private Finalizer mFinalizer;

    private boolean mOutputStreamOpenedOnce = false;

    private boolean mDataOutputStreamOpenedOnce = false;

    private boolean iInputStreamOpenedOnce = false;

    private boolean iDataInputStreamOpenedOnce = false;

    protected static final int COMM_BAUDRATE_INDEX = 0;

    protected static final int COMM_BITSPERCHAR_INDEX = 1;

    protected static final int COMM_STOPBITS_INDEX = 2;

    protected static final int COMM_PARITY_INDEX = 3;

    protected static final int COMM_BLOCKING_INDEX = 4;

    protected static final int COMM_AUTORTS_INDEX = 5;

    protected static final int COMM_AUTOCTS_INDEX = 6;

    /**
        * Constructs the CommConnectionImpl.
        *
        * @param uri
        *            the uri to which serial connection is opened
        * @param mode
        *            the with which serial connection is opened
        * @param aTimeout
        *            the timeout
        */

    public CommConnectionImpl(String aUri, int mode, boolean aTimeout)
    throws IOException
    {
        super(aUri, mode, false, false); // prefetch not used
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "++CommConnectionImpl(), uri = "+aUri);
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,"CommConnectionImpl() :: uri = "+aUri);
        mCommPort = new CommConnectionPort();
        CommConnectionUrlParser.parseURI(aUri,mParamterValues,mCommPort);
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "++CommConnectionImpl, url parsed successfully");
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "++CommConnectionImpl,baud rate = "+mParamterValues[COMM_BAUDRATE_INDEX]);
        int ret = _openCommportConnection(iNativePeerHandle, mCommPort.mCommPortName,
                                          mCommPort.mCommPortNumber, mParamterValues);
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "++CommConnectionImpl(), _openCommConnection returned  "+ret);
        if (ret != 0)
        {
            throw new IOException("Unable to open CommConnection: Symbian OS error code: "+ret);
        }
        mFinalizer = createFinalizer();
        mConnectionIsOpen = true;
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "--CommConnectionImpl()");
    }

    /**
     * Creates a finalizer instance.
     *
     * @return an instance of Finalizer class
     */

    private Finalizer createFinalizer()
    {
        return new Finalizer()
        {
            public void finalizeImpl()
            {
                doFinalize();
            }
        };
    }

    /**
     * This function is called by the runtime during finalization. all native
     * resources are released
     */
    private void doFinalize()
    {
        if (mConnectionIsOpen == true)
        {
            try
            {
                close();
            }
            catch (Exception e)
            {
                // catch the exception and call dispose
            }
        }
        if (iNativePeerHandle != 0)
        {
            _dispose(iNativePeerHandle);
            iNativePeerHandle = 0;
        }
    }



    /**
     * Please refer JSR 118.
     */

    public int getBaudRate()
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "CommConnectionImpl::getBaudRate()");
        return _getBaudRate(iNativePeerHandle);
    }

    /**
     * Please refer JSR 118.
     */

    public int setBaudRate(int aRate)
    {
        // sets the rate to aRate
        // returns the old rate
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "CommConnectionImpl::setBaudRate()");
        return _setBaudRate(iNativePeerHandle, aRate);
    }

    /**
    * Creates an OutputStream object. This overrides the Streams OpenOutputStream
    * For TCK Compliance - open, close and open of outputstream should throw IOException
    * @return a new instance on OutputStreamBase.
    * @throws IOException
    *             in case there is an attempt to open outputstream more than once for a connection
    */
    public OutputStream openOutputStream() throws IOException
    {
        if (mOutputStreamOpenedOnce == true)
        {
            throw new IOException("OutputStream cannot be opened multiple times");
        }
        else
        {
            mOutputStreamOpenedOnce = true;
            return super.openOutputStream();
        }
    }

    /**
     * Creates an DataOutputStream object. This overrides the Streams OpenDataOutputStream
     * For TCK Compliance - open, close and open of DataOutputStream should throw IOException
     * @return a new instance on DataOutputStreamBase.
     * @throws IOException
     *             in case there is an attempt to open DataOutputStream more than once for a connection
     */
    public DataOutputStream openDataOutputStream() throws IOException
    {
        if (mDataOutputStreamOpenedOnce == true)
        {
            throw new IOException("DataOutputStream cannot be opened multiple times");
        }
        else
        {
            mDataOutputStreamOpenedOnce = true;
            return super.openDataOutputStream();
        }
    }

    /**
    * Creates an DataInputStream object. This overrides the Streams openDataInputStream
    * For TCK Compliance - open, close and open of DatainputStream should throw IOException
    * @return a new instance on DataInputStreamBase.
    * @throws IOException
    *             in case there is an attempt to open openDataInputStream more than once for a connection
    */
    public DataInputStream openDataInputStream() throws IOException
    {
        if (iDataInputStreamOpenedOnce == true)
        {
            throw new IOException("DataInputStream cannot be opened multiple times");
        }
        else
        {
            iDataInputStreamOpenedOnce = true;
            return super.openDataInputStream();
        }
    }

    /**
    * Creates an InputStream object. This overrides the Streams openInputStream
    * For TCK Compliance - open, close and open of InputStream should throw IOException
    * @return a new instance on InputStreamBase.
    * @throws IOException
    *             in case there is an attempt to open openInputStream more than once for a connection
    */
    public InputStream openInputStream() throws IOException
    {
        if (iInputStreamOpenedOnce == true)
        {
            throw new IOException("InputStream cannot be opened multiple times");
        }
        else
        {
            iInputStreamOpenedOnce = true;
            return super.openInputStream();
        }
    }

    /**
     * Create the native peer handle
     *
     * @param uri
     *            the uri to which serial connection is opened
     * @param mode
     *            the with which serial connection is opened
     * @param aTimeout
     *            the timeout
     * @return native peer handle
     */

    public int createNativePeer(String aUri, int aMode, boolean timeout)
    {
        return _createNativePeer();
    }

    /**
     * Close the serial connection
     */
    public void close() throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "CommConnectionImpl::close(), flag = "+mConnectionIsOpen);
        if (!mConnectionIsOpen)
        {
            return; // already closed
        }
        mConnectionIsOpen = false;
        super.close(); // streamconnectionbase close
        if (isStreamsClosed() == true)
        {
            Logger.LOG(Logger.ESOCKET, Logger.EInfo, "CommConnectionImpl::close(), doing native close");
            _close(iNativePeerHandle);

        }
    }

    private native int _createNativePeer();

    private native int _openCommportConnection(int iNativePeerHandle,
            String aPort, int aPortNumber, int[] aIntegerOptions);

    private native int _setBaudRate(int iNativePeerHandle, int aBaudRate);

    private native int _getBaudRate(int iNativePeerHandle);

    private native void _close(int iNativePeerHandle);

    private native void _dispose(int iNativePeerHandle);
}

/**
 * CommConnectionPort class stores the port name and the number of the serial
 * port.
 */

class CommConnectionPort
{
    protected String mCommPortName = "";
    protected int mCommPortNumber = -1;

}


