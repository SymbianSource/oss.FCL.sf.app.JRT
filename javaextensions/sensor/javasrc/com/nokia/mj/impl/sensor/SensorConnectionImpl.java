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
* Description:  SensorConnection implementation
*
*/


package com.nokia.mj.impl.sensor;

import javax.microedition.sensor.Channel;
import javax.microedition.sensor.ChannelInfo;
import javax.microedition.sensor.Data;
import javax.microedition.sensor.DataListener;
import javax.microedition.sensor.SensorConnection;
import javax.microedition.sensor.SensorInfo;
import javax.microedition.sensor.SensorManager;
import java.io.IOException;
import com.nokia.mj.impl.rt.support.Finalizer;


/**
 * Implementation class for SensorConnection
 * Handles all data synchronization with native counterpart
 */
public class SensorConnectionImpl implements SensorConnection
{
    /**
     * Error message for closed connection
     */
    private static final String CONNECTION_CLOSED = "Connection closed";

    /**
     * Used as a property name,the value will be the required permission either
     * microedition.sensor.PrivateSensor or microedition.sensor.ProtectedSensor
     */
    static final String PROP_PERMISSION = "com.nokia.permission";

    /**
     * Info for Sensor used in this connection
     */
    private SensorInfo iSensorInfo;

    /**
     * Collection of channels used in this connection
     */
    private ChannelImpl[] iChannels = null;

    /**
     * Initial state of the SensorConnection is STATE_OPENED
     */
    private int state = STATE_OPENED;

    /**
     * Finalizer
     */
    private Finalizer iFinalizer;

    /**
     * Handle for native counterpart
     */
    private int iHandle = 0;

    /**
     * Server for continuous listening
     */
    private SensorServer iServer;

    /**
     * Data listener
     */
    private DataListener iDataListener;

    /**
     * getData synchronization
     */
    private class ObjectWait
    {
        protected boolean iIsNotified = false;
    }
    private ObjectWait iGetDataSync = new ObjectWait();
    private boolean iListeningConditions = false;
    /**
     * Constructor, finds a proper sensor with {@link SensorManager}
     * <code>findSensors</code>
     * @throws IOException if connection to sensor cannot be opened
     */
    public SensorConnectionImpl(String aUrl) throws IOException
    {
        iServer = new SensorServer();
        SensorInfo[] infos = SensorManagerImpl.getInstance().findSensors(aUrl);
        if ((infos == null) || (infos.length < 1))
        {
            throw new IllegalArgumentException("Cannot find sensor for URL: " + aUrl);
        }
        // If there is more than one possible sensors, select first
        SensorInfoImpl sensor = (SensorInfoImpl)infos[ 0 ];
        iSensorInfo = sensor;
        iHandle = iServer.startServer(sensor.iIndex);
        iFinalizer = registerForFinalization();
    }

    /**
     * Called when this object is finalized, frees native resources
     */


    public Finalizer registerForFinalization()
    {

        return new Finalizer()
        {
            public void finalizeImpl()
            {
                doFinalize();
            }
        };
    }

    void doFinalize()
    {

        if (iFinalizer == null)
        {
            return;
        }
        iFinalizer = null;

        try
        {
            close();
        }
        catch (IOException e) {}
    }


    /**
     * Closes SensorConnection. Closed SensorConnection cannot perform data
     * collection anymore.
     * @see SensorConnection
     */
    public void close() throws IOException
    {
        if (getState() == STATE_CLOSED)
        {
            return;
        }
        removeDataListener();
        state = STATE_CLOSED;
        _stopServer(iHandle);
    }


    /**
     * @see SensorConnection
     */
    public Channel getChannel(ChannelInfo info)
    {
        if (info == null)
        {
            throw new NullPointerException(
                "Retrieving Channel failed: channelInfo is null");
        }
        ChannelInfo[] channelInfos = getSensorInfo().getChannelInfos();

        if (iChannels == null)
        {
            iChannels = new ChannelImpl[ channelInfos.length ];
        }
        int index = 0;
        int l = 0;
        for (index = 0, l  = channelInfos.length; index < l; index++)
        {
            if (info == channelInfos[ index ])
            {
                break;
            }
            if (index == l - 1)
            {
                throw new IllegalArgumentException(
                    "Retrieving Channel failed: ChannelInfo object not found");
            }
        }

        if (iChannels[index] != null)
        {
            return iChannels[index];
        }

        iChannels[index] = new ChannelImpl(this, info, iHandle, index);
        return iChannels[index];
    }

    /**
     * @see SensorConnection
     */
    public Data[] getData(int bufferSize) throws IOException
    {
        return getData(bufferSize, -1, false, false, false);
    }

    /**
     * <p>
     * The data is collected from the sensor
     * according to the given parameters.
     * </p>
     * <p>
     * If data cannot be retrieved then 0-length Data array is returned.
     * </p>
     *
     * @param bufferSize
     *            - value can be either &gt;0 or -1 (means undefined)
     *            <ul>
     *            <li>if bufferSize is -1 then bufferingPeriod must be &gt; 0
     *            <li>if bufferingPeriod is -1 the bufferSize must be &gt; 0
     *            <li>if bufferSize &gt; 0 and bufferingPeriod &gt; 0 the actual
     *            size of the buffer inside {@link Data} is restricted with the
     *            condition first met. I.e. if sampling rate of the sensor
     *            fluctuates a lot then also the buffer sizes inside
     *            {@link Data} objects differ a lot.
     *            </ul>
     * @param bufferingPeriod
     *            the time to buffer values - given in milliseconds
     * @param isTimestampIncluded
     *            tells if timestamp is included
     * @param isUncertaintyIncluded
     *            tells if uncertainty is included
     * @param isValidityIncluded
     *            tells if validity is included
     * @return collected data of all the channels of this sensor
     * @throws IOException
     * @throws java.lang.IllegalArgumentException
     *             if both of the parameters - bufferSize and bufferingPeriod -
     *             have values less than 1.
     */
    public Data[] getData(int bufferSize, long bufferingPeriod,
                          boolean isTimestampIncluded, boolean isUncertaintyIncluded,
                          boolean isValidityIncluded) throws IOException
    {
        if (state == STATE_CLOSED)
        {
            throw new IOException(
                "Retrieving data failed: sensor is unavailable or SensorConnection is closed");
        }

        if (state != STATE_OPENED)
        {
            throw new IllegalStateException(
                "Retrieving data failed: SensorConnection is in STATE_LISTENING state");
        }

        if (bufferSize < 1)
        {
            if (bufferingPeriod < 1)
            {
                throw new IllegalArgumentException(
                    "Retrieving data failed: bufferSize and bufferPeriod values are less than 1");
            }
            bufferSize = getSensorInfo().getMaxBufferSize();
        }
        // this is to fix the leave that happens on 9.2 for buffering period = -1
        // 0 is considered as default in all the platforms
        else
        {
            if (bufferingPeriod < 0)
            {
                bufferingPeriod = 0;
            }
        }
        if (bufferSize > getSensorInfo().getMaxBufferSize())
        {
            throw new IllegalArgumentException(
                "Retrieving data failed: bufferSize exceeds maximum size "
                + bufferSize);
        }

        ChannelInfo[] channels = iSensorInfo.getChannelInfos();
        DataImpl[] data = new DataImpl[ channels.length ];
        for (int i = 0; i < data.length; i++)
        {
            data[ i ] = new DataImpl(channels[ i ]);
            data[ i ].prepareData(isUncertaintyIncluded);
        }
        iGetDataSync.iIsNotified = false;
        _startDataListening(iHandle, data, bufferSize, bufferingPeriod,
                            isTimestampIncluded, isValidityIncluded, true,
                            channels[ 0 ].getDataType());
        // wait for result
        synchronized (iGetDataSync)
        {
            try
            {
                if (!iGetDataSync.iIsNotified)
                {
                    iGetDataSync.wait();
                }
            }
            catch (InterruptedException e) {}
        }
        return data;
    }

    /**
     * Returns SensorInfo, which contains detailed information of the sensor,
     * information is static and not changing after getting the sensor, except
     * maybe location values
     *
     * @return reference to SensorInfo object of the SensorConnection
     */
    public SensorInfo getSensorInfo()
    {
        return iSensorInfo;
    }

    /**
     * Queries the state of the sensor
     *
     * @return state - possible states are {@link #STATE_OPENED},
     *         {@link #STATE_LISTENING} and {@link #STATE_CLOSED}.
     */
    public synchronized int getState()
    {
        return state;
    }

    /**
     * Removes DataListener, which is receiving notification of collected data.
     * The state is changed to be STATE_OPENED, it's now OK to call getData
     * -methods.
     *
     */
    public synchronized void removeDataListener()
    {
        closeCheck("Removing DataListener ");
        _cancelDataListening(iHandle);
        iDataListener = null;
        state = STATE_OPENED;
    }

    /**
     * <p>
     * Registers DataListener to receive collected data asynchronously.
     * Registered DataListener receives data within
     * {@link DataListener#dataReceived} -notification. With this
     * setDataListener method timestamp, validity and uncertainty values are not
     * included in Data objects.
     * </p>
     *
     * @param listener
     *            - the listener to be registered
     * @param bufferSize
     *            - must be &gt;0
     * @throws java.lang.NullPointerException
     *             if listener is null
     * @throws java.lang.IllegalArgumentException
     *             if bufferSize&lt;1
     */
    public void setDataListener(DataListener listener,
                                int bufferSize)
    {
        if (bufferSize < 1)
        {
            throw new IllegalArgumentException(
                "Setting DataListener failed: bufferSize value is less than 1");
        }
        setDataListener(listener, bufferSize, -1, false, false, false);
    }

    /**
     * <p>
     * Registers listener to receive sensor data asynchronously. Sensor data
     * listener receives sensor data: "dataReceived" notification.
     * </p>
     *
     * @param listener
     *            - the listener to be registered
     * @param bufferSize
     *            - value can be either &gt;0 or -1 (means undefined)
     *            <ul>
     *            <li>if bufferSize is -1 then bufferingPeriod must be > 0
     *            <li>if bufferingPeriod is -1 the bufferSize must be >0
     *            <li>if bufferSize&gt;0 and bufferingPeriod&gt;0 the actual
     *            size of the buffer inside {@link Data} is restricted with the
     *            constraint first met. I.e. if sampling rate of the sensor
     *            fluctuates a lot then also the buffer sizes inside
     *            {@link Data} objects differ a lot.
     *            </ul>
     * @param bufferingPeriod
     *            the time in milliseconds to buffer values inside one Data
     *            object
     * @param isTimestampIncluded
     *            tells if timestamp is included
     * @param isUncertaintyIncluded
     *            tells if uncertainty is included
     * @param isValidityIncluded
     *            tells if validity is included
     * @throws java.lang.NullPointerException
     *             if listener is null
     * @throws java.lang.IllegalArgumentException
     *             if bufferSize and bufferingPeriod both are&lt;1
     */
    public synchronized void setDataListener(DataListener listener,
            int bufferSize, long bufferingPeriod, boolean isTimestampIncluded,
            boolean isUncertaintyIncluded, boolean isValidityIncluded)
    {
        closeCheck("Setting DataListener ");
        if (listener == null)
        {
            throw new NullPointerException();
        }

        if (bufferSize < 1)
        {
            if (bufferingPeriod < 1)
            {
                throw new IllegalArgumentException(
                    "Setting DataListener failed: bufferSize and bufferPeriod are less than 1");
            }
            bufferSize = iSensorInfo.getMaxBufferSize();
        }
        // this is to fix the leave that happens on 9.2 for buffering period = -1
        // 0 is considered as default in all the platforms
        else
        {
            if (bufferingPeriod < 0)
            {
                bufferingPeriod = 0;
            }
        }
        if (bufferSize > getSensorInfo().getMaxBufferSize())
        {
            throw new IllegalArgumentException(
                "Setting DataListener failed: bufferSize exceeds maximum size "
                + bufferSize);
        }
        if (state == STATE_LISTENING)
        {
            _cancelDataListening(iHandle);
        }

        ChannelInfo[] channels = iSensorInfo.getChannelInfos();
        DataImpl[] data = new DataImpl[ channels.length ];
        for (int i = 0; i < data.length; i++)
        {
            data[ i ] = new DataImpl(channels[ i ]);
            data[ i ].prepareData(isUncertaintyIncluded);
        }
        iDataListener = listener;
        _startDataListening(iHandle, data, bufferSize, bufferingPeriod,
                            isTimestampIncluded, isValidityIncluded, false,
                            channels[ 0 ].getDataType());
        state = STATE_LISTENING;
    }

    /**
     * @see SensorConnection
     */
    public int[] getErrorCodes()
    {
        return new int[ 0 ];
    }

    /**
     * @see SensorConnection
     */
    public String getErrorText(int errorCode)
    {
        throw new IllegalArgumentException("No error codes supported");
    }

    /**
     * Helper method for checking is the connection closed
     */
    private void closeCheck(String aReason)
    {
        if (state == STATE_CLOSED)
        {
            throw new IllegalStateException(aReason +
                                            "failed: SensorConnection is already closed");
        }
    }

    /**
     * Starts condition listening
     * @param aListeningType with value 0 condition evaluation is done in native side
     *                          with value 1 all values are passed to Java
     */
    protected void startConditionListening(int aListeningType)
    {
        if (!iListeningConditions)
        {
            iListeningConditions = true;
            _startConditionListening(iHandle, aListeningType);
        }
    }

    /**
     * Stops condition listening
     */
    protected void stopConditionListening()
    {
        boolean stop = true;
        for (int i = 0; i < iChannels.length; i++)
        {
            if (iChannels[i] != null && iChannels[i].hasConditions())
            {
                stop = false;
                break;
            }
        }
        if (stop)
        {
            _stopConditionListening(iHandle);
            iListeningConditions = false;
        }
    }
    /**
     * Creates native peer for this connection
     * @param aSensorList Handle to native sensor list
     * @param aSensorIndex Index for created sensor
     * @return negative Error code or positive handle for sensor connection
     *            if creation was successful
     */
    private native int _createSensorConnection(int aSensorList,
            int aSensorIndex);

    /**
     * Starts data listening from native side, dataReceived method will
     * be called when buffer is filled, or buffering period exceeds
     *
     * @param aSensorConnection Handle to native SensorConnectionImpl
     * @param aData Data objects to be filled
     * @param aBufferSize Buffer size
     * @param aBufferingPeriod Buffering period
     * @param aIsTimestampIncluded Do we add timestamps
     * @param aIsValidityIncluded Do we add validities
     * @param aOneShot do we need just one data (getData)
     * @param aDataType Data type of this sensor
     * @return Error code or 0 if everything is fine and 1 if some data is lost
     */
    private native int _startDataListening(int aSensorConnection,
                                           Data[] aData,
                                           int aBufferSize,
                                           long aBufferingPeriod,
                                           boolean aIsTimestampIncluded,
                                           boolean aIsValidityIncluded,
                                           boolean aOneShot,
                                           int aDataType);

    /**
     * Cancel data listening
     * @param aSensorConnection Handle to native SensorConnectionImpl
     * @return Error code or 0 if everything is fine
     */
    private native int _cancelDataListening(int aSensorConnection);

    /**
     * Clears native parts from this object
     * @param aSensorList Handle to sensorlist
     * @param aSensorConnection Handle to native sensor connection
     */
    private native void _removeConnection(int aSensorList,
                                          int aSensorConnection);


    /**
     * Start native Condition listening
     * @param aConnectionHandle native SensorConnection handle
     * @param aListeningType 0 for native condition checking
     *                       1 for java side condition checking
     */
    protected native void _startConditionListening(int aConnectionHandle,
            int aListeningType);

    /**
     * Stop native condition listening
     * @param aConnectionHandle native SensorConnection handle
     */
    protected native void _stopConditionListening(int aConnectionHandle);

    /**
     * Start server on native side for handling requests from different threads
     * @param aConnectionHandle
     */
    private native void _startServer(int aConnectionHandle);

    /**
     * Stops the server
     * @param aConnectionHandle
     */
    private native void _stopServer(int aConnectionHandle);

    /**
     * Callback from native when condition is met, if all values are listened, then all
     * values comes through this interface
     * @param aChannel
     * @param aCondition
     * @param aValue
     * @param aTimeStamp
     */
    protected void conditionMet(int aChannel, int aCondition, double aValue, long aTimeStamp)
    {
        iChannels[aChannel].conditionMet(aCondition, aValue, aTimeStamp);
    }

    /**
     * Callback from native when data is received
     * @param aData Filled data
     * @param aIsDataLost Information about lost data
     */
    protected void dataReceived(Data[] aData, boolean aIsDataLost)
    {
        if (state == STATE_LISTENING)
        {
            iDataListener.dataReceived(this, aData, aIsDataLost);
        }
        else if (state == STATE_OPENED)
        {
            //release get data
            synchronized (iGetDataSync)
            {
                iGetDataSync.iIsNotified = true;
                iGetDataSync.notify();
            }
        }
    }
    /**
     * Sensor server thread, used for handling all the requests in one thread
     *
     */
    class SensorServer extends Thread
    {
        private int iHandle;
        private int iSensorIndex;
        private Object iLock = new Object();

        /**
         * Starts the server with specified sensor
         * @param aSensorIndex Specified sensor
         * @return Sensor handle, or error code
         * @throws IOException If connection to sensor fails
         */
        public int startServer(int aSensorIndex) throws IOException
        {
            iSensorIndex = aSensorIndex;
            start();
            synchronized (iLock)
            {
                try
                {
                    if (iHandle == 0)
                    {
                        iLock.wait();
                    }
                }
                catch (InterruptedException e) {}
            }
            if (iHandle <= 0)
            {
                throw new IOException("Could not connect to sensor");
            }
            return iHandle;
        }

        /**
         * Actual server behavior, connects to sensor, starts server and finally removes connection
         */
        public void run()
        {
            iHandle =
                _createSensorConnection(SensorManagerImpl.getInstance().sSensorConnectionList,
                                        iSensorIndex);
            synchronized (iLock)
            {
                iLock.notify();
            }
            if (iHandle < 0)
            {
                return;
            }
            _startServer(iHandle);
            _removeConnection(SensorManagerImpl.getInstance().sSensorConnectionList, iHandle);
        }
    }
}
