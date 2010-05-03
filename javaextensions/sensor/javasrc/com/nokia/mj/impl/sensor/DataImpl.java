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
* Description:  DataImpl implementation
*
*/


package com.nokia.mj.impl.sensor;

import javax.microedition.sensor.ChannelInfo;
import javax.microedition.sensor.Data;
import javax.microedition.sensor.DataListener;
import javax.microedition.sensor.SensorConnection;

/**
 * <p>
 * This class represents measurement value/values retrieved from the sensor.
 * Although there might be many channels, this class contains data of only one
 * channel. Also related information - like an accuracy, a timestamp and a
 * validity - is encapsulated inside the class.
 * </p>
 *
 * <p>
 * Sensors can send values from multiple channels in the same time; number of
 * the channels depends on the sensor nature. For example, the thermometer
 * returns only one value representing temperature. The 3D accelerometer returns
 * collection of three values each representing an acceleration in one
 * particular direction. Each DataI mpl object can be identified by the Channel
 * object not to mix measurements from different channels. All the channels of
 * the sensor can be queried with the method {@link SensorInfoImpl#getChannels}.
 * </p>
 *
 * <p>
 * The content of the Data depends of the parameters specified in initiating
 * data retrieval method whether {@link SensorConnection#getData(int)}/
 * {@link SensorConnection#getData(int, long, boolean, boolean, boolean)} or
 * {@link SensorConnection#setDataListener}/. Buffering is recommended if the
 * application is performance critical. Buffering has also favourable effect of
 * easing garbage collection; if the sampling rate is 1000 Hz - instead of
 * creating 1000 objects, by setting the size of buffer to 100 - only 10 objects
 * are created per a second.
 * </p>
 *
 * <p>
 * Possible primitive data types are int/double and if other types are wanted
 * then Object type has to be used, e.g. boolean values are recommended to be
 * converted to values 0 and 1. The need to use Object type could come with
 * maybe not so common data types like character strings or bitmaps. Functions
 * to retrieve arrays of values are {@link #getObjectValues}/
 * {@link #getIntValues}/{@link #getDoubleValues}. Supported data type of the
 * measurement can be asked with funtion {@link ChannelInfo#getDataType} method
 * returns data types of requested channel.
 * </p>
 *
 * <p>
 * The presence of the timestamp and the accuracy and the validity depends on
 * the given parameters when data retrieval is initiated. Omitting either
 * timestamp or accuracy or validity or all can optimize the performance of the
 * data fetching.
 * </p>
 *
 * <p>
 * Data values are returned as an array of ints/doubles/Objects inside Data
 * objects; either synchronously with method {@link SensorConnection#getData} or
 * asynchronously by listening {@link DataListener#dataReceived} events, an
 * array is returned even if there is only one channel. The platform
 * implementation MUST ensure that it only returns objects where the parameters
 * have values according to their semantics.
 * </p>
 *
 */

final class DataImpl implements Data
{
    private final ChannelInfo iChannel;

    private double[] iDoubleValues = null;
    private int[] iIntValues = null;
    private Object[] iObjectValues = null;

    private long[] iTimestamps = null;
    private boolean[] iValidities = null;

    private boolean iIsUncertainties = true;
    /**
     *
     *
     */
    DataImpl(ChannelInfo aChannel)
    {
        iChannel = aChannel;
    }

    DataImpl(ChannelInfo aChannel, double[] aValues, long[] aTimeStamps, boolean[] aValidities)
    {
        iChannel = aChannel;
        iDoubleValues = aValues;
        iTimestamps = aTimeStamps;
        iValidities = aValidities;
    }

    DataImpl(ChannelInfo aChannel, int[] aValues, long[] aTimeStamps, boolean[] aValidities)
    {
        iChannel = aChannel;
        iIntValues = aValues;
        iTimestamps = aTimeStamps;
        iValidities = aValidities;
    }

    void prepareData(boolean aIsUncertainties)
    {
        iIsUncertainties = aIsUncertainties;
    }
    /**
     * Returns the name used to identify the measurement, name is the same as
     * the channel - channel can be asked with function
     * {@link SensorInfoImpl#getChannels}.
     *
     * @return name - channel of the measurement
     */
    public ChannelInfo getChannelInfo()
    {
        return iChannel;
    }

    /**
     * <p>
     * Returns the values of the sensor measurements as double array if the
     * method {@link ChannelInfo#getDataType} of this channel returns data type,
     * which should equal {@link ChannelInfo#TYPE_DOUBLE}.
     * </p>
     *
     * @return sensor data values as array of double values or a 0-length array
     *         if the values cannot be retrieved or represented as double array.
     */
    public double[] getDoubleValues()
    {

        if (iChannel.getDataType() != ChannelInfo.TYPE_DOUBLE)
        {
            throw new IllegalStateException(
                "Data type 'double' is invalid for this channel. Call "
                + getRightMethod() + " instead");
        }

        return iDoubleValues;
    }

    /**
     * <p>
     * Returns the values of the sensor measurements as int array if the method
     * {@link ChannelInfo#getDataType} of this channel returns data type, which
     * should equal to {@link ChannelInfo#TYPE_INT}.
     *
     * @return sensor data values or 0-length int array if the values cannot be
     *         retrieved or represented as int array.
     */
    public int[] getIntValues()
    {
        if (getChannelInfo().getDataType() != ChannelInfo.TYPE_INT)
        {
            throw new IllegalStateException(
                "Data type 'int' is invalid for this channel. Call "
                + getRightMethod() + " instead");
        }

        return iIntValues;
    }

    /**
     * <p>
     * Returns measurement timestamp values as an array. Timestamp is time when
     * single measurement was captured, prevailing measurement time in
     * milliseconds, function <code>{@link
     * java.lang.System#currentTimeMillis}</code> can be
     * used to get the value, which returns the difference, measured in
     * milliseconds, between the current time and midnight, January 1, 1970 UTC.
     * </p>
     *
     * @return timestamps when the each measurement was captured
     * @throws java.lang.IndexOutOfBoundsException
     *             if the index is out of range
     * @throws java.lang.IllegalStateException
     *             if timestamp is not supposed to be got according to the
     *             settings defined, when the data retrieval is initiated.
     */
    public long getTimestamp(int index)
    {
        if (iTimestamps == null)
        {
            throw new IllegalStateException("Timestamp information not available");
        }
        return iTimestamps[ index ];
    }

    /**
     * <p>
     * Returns accuracies of the measurements. Unit of the accuracy must be the
     * same as the actual value. Also the scale of the accuracy must be the same
     * as scale of actual value. Accuracy is the standard deviation of the
     * measurement.
     * </p>
     *
     * <p>
     * If measurements are normally distributed, then 68% of measured values
     * should reside within ± 1 sigma (standard deviation).
     * </p>
     *
     * <p>
     * For example if real measured value is 5.24 s, value is presented as int
     * value 524. Unit is Unit.s (second), scale is -2, so that value of actual
     * measurement can be calculated in following manner:
     * </p>
     *
     *<p>
     * 524 * 10<sup>-2</sup> = 5.24
     *</p>
     *
     * <p>
     * This can be understood as fixed point presentation, amount of decimals is
     * supposed to be constant. If accuracy of the measurement is e.g. 8, unit
     * is the same as of the actual measurement i.e. seconds, scale is same as
     * well. If measurements are normally distributed - then probability of
     * actual value to reside between 5.24 ± 0.08 is around 68%.
     * </p>
     *
     * @return uncertainty of the measurements
     * @throws java.lang.IndexOutOfBoundsException
     *             if the index is out of range
     * @throws java.lang.IllegalStateException
     *             if the uncertainty is not supposed to be got according to
     *             settings.
     */
    public float getUncertainty(int index)
    {
        if (!iIsUncertainties)
        {
            throw new IllegalStateException("Uncertainty information not available");
        }

        int length = iIntValues != null ? iIntValues.length : iDoubleValues.length;

        if ((index >= length) || (index < 0))
        {
            throw new IndexOutOfBoundsException(
                "Index of data value is out of range: " + index);
        }

        // If sensor is not providing accuracy, then uncertainty is zero
        if (iChannel.getAccuracy() < 0)
        {
            return 0;
        }

        float value = 0;
        if (iIntValues != null)
        {
            value = iIntValues[ index ] * iChannel.getAccuracy();
        }
        else
        {
            value = (float)(iDoubleValues[ index ] * iChannel.getAccuracy());
        }
        return Math.abs(value);
    }

    /**
     * <p>
     * Returns the values of the sensor measurements as array of Objects if the
     * method {@link ChannelInfo#getDataType} of this channel returns data type,
     * which should equal {@link ChannelInfo#TYPE_OBJECT} and size of the buffer
     * &gt; 0 .
     * </p>
     *
     * @return sensor data values as array of Objects or 0-length Object array
     *         if the values cannot be retrieved or represented as Object array.
     */
    public Object[] getObjectValues()
    {
        if (iChannel.getDataType() != ChannelInfo.TYPE_OBJECT)
        {
            throw new IllegalStateException(
                "Data type 'object' is invalid for this channel. Call "
                + getRightMethod() + " instead.");
        }
        return iObjectValues;
    }

    /**
     * Returns validities of the measurements. Provides a way to inform of
     * invalid measurements like "no data received".
     *
     * @return validities of the measurements
     * @throws java.lang.IndexOutOfBoundsException
     *             if the index is out of range
     * @throws IllegalStateException
     *             if the validity is not supposed to be got according to the
     *             settings. Validity is provided if e.g.
     *             {@link SensorConnection#getData(int, long, boolean, boolean, boolean)}
     *             is called, with the parameter isValidityIncluded = true.
     */
    public boolean isValid(int index)
    {
        if (iValidities == null)
        {
            throw new IllegalStateException("Validity information not available");
        }
        return iValidities[ index ];
    }


    /**
     * Helper method for getting string to exception string
     * @return Suitable string for exception
     */
    private String getRightMethod()
    {
        switch (iChannel.getDataType())
        {
        case ChannelInfo.TYPE_INT:
            return "getIntValues()";
        case ChannelInfo.TYPE_DOUBLE:
            return "getDoubleValues()";
        case ChannelInfo.TYPE_OBJECT:
            return "getObjectValues()";
        }
        return "";
    }

};
