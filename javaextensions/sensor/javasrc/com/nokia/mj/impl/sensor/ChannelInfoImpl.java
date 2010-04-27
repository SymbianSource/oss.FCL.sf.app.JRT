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
* Description:  ChannelInfo implementation
*
*/


package com.nokia.mj.impl.sensor;

import javax.microedition.sensor.ChannelInfo;
import javax.microedition.sensor.Data;
import javax.microedition.sensor.MeasurementRange;
import javax.microedition.sensor.Unit;

/**
 * <p>
 * This class wraps up static information about data properties of one channel
 * of the sensor. In addition to the properties, it also maintains Condition
 * objects related to this particular channel.
 * </p>
 *
 * <p>
 * If the sensor is measuring different values simultanously, like the three
 * axis magnetometer giving heading, pitch and roll values, each axis is
 * presenting one channel. Data from each channel is put in separate
 * {@link Data} object. {@link Data#getChannel} will return the channel from
 * which the data values was fetched from. Synonyms for the concept could be
 * "axis" or "degree of freedom". Purpose of this class is to standardize the
 * most obvious channels to make the computational handling easier; an example
 * of established channels is 3d accelerometer with channels: axis_x, axis_y and
 * axis_z.
 * </p>
 *
 * <p>
 * With sensors, which are measuring values related to the orientation of the
 * device, like accelerometers and magnetometers, it's good to agree of the
 * conventions of how the axis are typically oriented towards device.
 * </p>
 *
 * <p>
 * It is generally recommended that the longitudinal axis is aligned with the
 * bottom-to-top direction of the screen. This means that the pitch is positive
 * when the top of the screen is up and the bottom of the screen down (when roll
 * is zero). The roll is positive when the device is tilted clockwise looking
 * from the direction of the bottom of the screen, i.e. when the left side of
 * the screen is up and the right side of the screen is down (when pitch is
 * zero).
 * </p>
 *
 * <p>
 * X- and Y-axis orientation is already defined in MIDP Graphics class, and
 * those agreed conventions must be used here also:<br>
 *"The X-axis direction is positive towards the right, and the Y-axis direction
 * is positive downwards." Z-axis direction is orthogonal to the plane of the
 * screen and move towards user is positive.
 * </p>
 *
 *<p>
 * A joystick can also be understood as one sensor.
 *</p>
 *
 *<p>
 * Channel contains following mandatory member variables:
 * <ul>
 * <li>a name, table of standard names below</li>
 * <li>an accuracy</li>
 * <li>a data type</li>
 * <li>ranges</li>
 * <li>a scale</li>
 * <li>a unit</li>
 * </ul>
 *</p>
 *
 *<p>
 * Measured data values are checked against the maximum range. Range can change
 * during the run; range and resolution are related values, it's probable that
 * when range is changing, resolution is also changing; resolution can be found
 * inside {@link MeasurementRange}.
 *</p>
 *
 * <p>
 * Channel also maintains Conditions. Application can monitor measured data
 * values of SensorConnection by setting conditions. By default no conditions
 * are set for monitoring and method {@link #getConditions} returns a
 * zero-length array. Channel offers methods to add, get and remove Condition
 * objects. Overlapping Condition objects can be added. There will be
 * notification for each met condition, thus for one measured value there can be
 * several notifications. After condition is met, it is removed.
 * </p>
 *
 *<p>
 * Channel should do all the needed checks when the Condition object is added.
 * For Channels having Object data type, the only valid Condition type is
 * ObjectCondition.
 * </p>
 *
 * <p>
 * Channel names listed in the table 1 below must be used, if they are suited.
 * Names are case-sensitive and must follow the given format. If names listed
 * here don't match the needs, new names can be defined. The name must be chosen
 * to be unique, and be named using standard Java package naming conventions,
 * for example "com.nokia.phone.ri.sensor.quaternion".
 *</p>
 *<p>
 * Table 1: Channel names to be used
 * <table border="1">
 * <tr>
 * <td>axis_x</td>
 * <td>
 * Acceleration/velocity to axis x direction</td>
 * </tr>
 * <tr>
 * <td>axis_y</td>
 * <td>
 * Acceleration/velocity to axis y direction</td>
 * </tr>
 * <tr>
 * <td>axis_z</td>
 * <td>
 * Acceleration/velocity to axis z direction</td>
 * </tr>
 * <tr>
 * <td>
 * default</td>
 * <td>
 * The default channel, which is expected to be used in the case of one-channel
 * sensors like a thermometer, a barometer or a humiditymeter</td>
 * </tr>
 * <tr>
 * <td>
 * heading</td>
 * <td>
 * The angle between the direction of a compass needle and a specified meridian,
 * due north</td>
 * </tr>
 * <tr>
 * <td>
 * pitch</td>
 * <td>
 * The deviation from a stable attitude by movement of the longitudinal axis
 * about the lateral axis</td>
 * </tr>
 * <tr>
 * <td>
 * roll</td>
 * <td>
 * The deviation of display plane of the horizontal plane</td>
 * </tr>
 * <tr>
 * <td>rot_x</td>
 * <td>The rotation around axis x</td>
 * </tr>
 * <tr>
 * <td>rot_y</td>
 * <td>The rotation around axis y</td>
 * </tr>
 * <tr>
 * <td>rot_z</td>
 * <td>The rotation around axis z</td>
 * </tr>
 *
 * <tr>
 * <td>button_1</td>
 * <td>A custom button 1, fire 1 in joystick</td>
 * </tr>
 * <tr>
 * <td>button_2</td>
 * <td>A custom button 2, fire 2 in joystick</td>
 * </tr>
 * <tr>
 * <td>button_3</td>
 * <td>A custom button 3</td>
 * </tr>
 * <tr>
 * <td>button_4</td>
 * <td>A custom button 4</td>
 * </tr>
 *
 * <tr>
 * <td>
 * throttle</td>
 * <td>
 * </td>
 * </tr>
 *
 * <tr>
 * <td>
 * slider</td>
 * <td>
 * </td>
 * </tr>
 *
 * <tr>
 * <td>
 * rudder</td>
 * <td>
 * </td>
 * </tr>
 *
 * </table>
 * </p>
 */
public class ChannelInfoImpl implements ChannelInfo
{
    private static final String SCHEME_SEP = "#";
    private static final String INVALID_CHANNELINFO = "Invalid channel info string";


    /**
     * Name of the channel
     */
    private String iName;
    private MeasurementRange[] iMeasurementRanges;
    private int iScale;
    private Unit iUnit;
    private float iAccuracy;
    private int iDataType;

    /**
     **Constructor for making new Channel object.
     *
     * @param iName
     *            the name of the channel
     * @param iScale
     *            the scale of the data values of this channel
     * @param iUnit
     *            the unit of the data values of this channel
     * @param iAccuracy
     *            the accuracy of the data values
     * @param iDataType
     *            the type of the data of the data values
     * @param ranges
     *            the ranges of this channel
     * @throws NullPointerException
     *             if any of the parameters is null
     * @throws IllegalArgumentException
     *             if the dataType or the accuracy is not valid
     */
    public ChannelInfoImpl(String aChannelInfo)
    {
        // name
        int start = 0;
        int end = aChannelInfo.indexOf(SCHEME_SEP);
        if (end <= 0)
        {
            throw new IllegalArgumentException(INVALID_CHANNELINFO);
        }
        iName = aChannelInfo.substring(start, end);

        // accuracy
        start = end + 1;
        end = aChannelInfo.indexOf(SCHEME_SEP, start);
        if (end <= 0)
        {
            throw new IllegalArgumentException(INVALID_CHANNELINFO);
        }
        iAccuracy = Float.parseFloat(aChannelInfo.substring(start, end));

        // dataType
        start = end + 1;
        end = aChannelInfo.indexOf(SCHEME_SEP, start);
        if (end <= 0)
        {
            throw new IllegalArgumentException(INVALID_CHANNELINFO);
        }
        int dataType = Integer.parseInt(aChannelInfo.substring(start, end));
        if (dataType != ChannelInfo.TYPE_DOUBLE
                && dataType != ChannelInfo.TYPE_INT
                && dataType != ChannelInfo.TYPE_OBJECT)
        {
            throw new IllegalArgumentException(INVALID_CHANNELINFO);
        }
        iDataType = dataType;

        // scale
        start = end + 1;
        end = aChannelInfo.indexOf(SCHEME_SEP, start);
        if (end <= 0)
        {
            throw new IllegalArgumentException(INVALID_CHANNELINFO);
        }
        iScale = Integer.parseInt(aChannelInfo.substring(start, end));

        // unit
        start = end + 1;
        end = aChannelInfo.indexOf(SCHEME_SEP, start);
        if (end <= 0)
        {
            throw new IllegalArgumentException(INVALID_CHANNELINFO);
        }
        iUnit = Unit.getUnit(aChannelInfo.substring(start, end));

        // measurement ranges (multiple)
        start = end + 1;
        end = aChannelInfo.indexOf(SCHEME_SEP, start);
        if (end <= 0)
        {
            throw new IllegalArgumentException(INVALID_CHANNELINFO);
        }
        int measurementRangeCount = Integer.parseInt(aChannelInfo.substring(start, end));
        iMeasurementRanges = new MeasurementRange[ measurementRangeCount ];
        for (int i = 0; i < measurementRangeCount; i++)
        {
            // measurement range: smallest
            start = end + 1;
            end = aChannelInfo.indexOf(SCHEME_SEP, start);
            if (end <= 0)
            {
                throw new IllegalArgumentException(INVALID_CHANNELINFO);
            }
            double msrSmallest = Double.parseDouble(aChannelInfo.substring(start, end));

            // measurement range: largest
            start = end + 1;
            end = aChannelInfo.indexOf(SCHEME_SEP, start);
            if (end <= 0)
            {
                throw new IllegalArgumentException(INVALID_CHANNELINFO);
            }
            double msrLargest = Double.parseDouble(aChannelInfo.substring(start, end));

            // measurement range: resolution
            start = end + 1;
            end = aChannelInfo.indexOf(SCHEME_SEP, start);
            if (end <= 0)
            {
                throw new IllegalArgumentException(INVALID_CHANNELINFO);
            }
            double msrResolution = Double.parseDouble(aChannelInfo.substring(start, end));
            iMeasurementRanges[ i ] =
                new MeasurementRange(msrSmallest, msrLargest, msrResolution);
        }
    }

    /**
     * <p>
     * Returns accuracy of the Channel, which is the estimate needed for a
     * comparison of different sensors. The accuracy is given as unitless
     * percentage:
     * </p>
     * <p>
     * 0 &lt; accuracy &lt; 1
     * </p>
     * <p>
     * There can be several accuracy classes for different ranges. A relation
     * between an uncertainty of the {@link Data} and an accuracy is:<br>
     * <code>
     * accuracy * data value &gt;= uncertainty
     * </code> This method returns the accuracy, which results the maximum
     * uncertainty.
     * </p>
     *
     * @return an accuracy
     */
    public float getAccuracy()
    {
        return iAccuracy;
    }

    /**
     * Returns the data type of the channel. Possible data types are
     * int/double/Object. Constants for types are listed in description of
     * {@link ChannelInfoImpl}.
     *
     * @return the data type of the channel
     */
    public int getDataType()
    {
        return iDataType;
    }

    /**
     * Method to return the name of the Channel.
     *
     * @return the name
     */
    public String getName()
    {
        return iName;
    }

    /**
     * The ranges of the possible measured values; defined with smallest and
     * largest possible value and resolution, for more information see class
     * {@link MeasurementRange}. If the data type of the Channel object is
     * {@link #TYPE_OBJECT} method returns a 0-length Range array
     *
     * @return all ranges of the data values
     */
    public MeasurementRange[] getMeasurementRanges()
    {
        return iMeasurementRanges;
    }

    /**
     * <p>
     * Returns scale, relates with unit, for example -3 means scaling with
     * 10<sup>-3</sup>. If unit is meters and scale is -3, the unit can be
     * interpreted as mm.
     * </p>
     *
     * <p>
     * The most probable usage scenario is that fixed-point presentation is
     * used, which means that amount of decimal numbers remains always the same.
     * Also {@link Data#getUncertainty} and {@link #getRanges} have to have the
     * same scale as data values.
     * </p>
     *
     * @return scale, the exponent of ten
     */
    public int getScale()
    {
        return iScale;
    }

    /**
     * Returns unit, data values presented in.
     *
     * @return unit, data values of one channel are presented in
     */
    public Unit getUnit()
    {
        return iUnit;
    }
}
