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
* Description:  SensorInfo implementation
*
*/


package com.nokia.mj.impl.sensor;

import java.util.Enumeration;
import java.util.Hashtable;

import javax.microedition.sensor.ChannelInfo;
import javax.microedition.sensor.Data;
import javax.microedition.sensor.DataListener;
import javax.microedition.sensor.SensorConnection;
import javax.microedition.sensor.SensorInfo;
import javax.microedition.sensor.SensorManager;

import com.nokia.mj.impl.utils.ResourceLoader;

/**
 * <p>
 * SensorInfoImpl contains static information describing properties of the
 * physical sensor and SensorConnection implementation. While trying to find the
 * best possible choice of the given quantity, a SensorInfo array is returned to
 * ease the selection - see e.g. method
 * {@link SensorManager#findSensors(Quantity, ContextType)}. There is thought to
 * be a repository of sensors from which the needed information can be
 * retrieved. After selecting the proper implementation the SensorConnection can
 * be constructed with Connector.open - by giving a url as a parameter; the url
 * can be got with method {@link SensorInfoImpl#getUrl()}.
 * </p>
 *
 * <p>
 * The SensorInfo object contains Channels, which contain all the information
 * related to data provided by the channel, like the unit, the scale and the
 * range. SensorInfo contains information of the sensor as a whole.
 *
 */
final class SensorInfoImpl implements SensorInfo
{

    private static final int MAX_BUFFER_SIZE = 256;
    /**
     * Permission constant for the "public" sensor, which doesn't require any
     * permissions
     */
    final static int PUBLIC_SENSOR = 1;
    /**
     * Permission constant for the "private" sensor
     */
    final static int PRIVATE_SENSOR = 2;
    final static int PROTECTED_SENSOR = 4;

    private static final String[] NO_PROPERTIES = new String[ 0 ];
    private static final String SCHEME_SENSOR = "sensor:";
    private static final String SCHEME_CONTEXT_TYPE = "contextType=";
    private static final String SCHEME_MODEL = "model=";
    private static final String SCHEME_LOCATION = "location=";
    private static final String SCHEME_SEP_EXTERNAL = ";";
    private static final String SCHEME_SEP_INTERNAL = "#";
    private static final String SCHEME_SEP_EQUAL ="=";
    private static final String INVALID_SENSORINFO = "Invalid sensor info string";

    private static final String CHANNEL_END_SEP = "@ch_end#";
    private static final String FILE_RES_START = "qtn_sensor_description_";
    private static final String FILE_RES_NAME = "javasensor";

    private ChannelInfo[] iChannelInfos;
    /**
     * connection type of the sensor
     */
    private int connectionType;

    /**
     * availability of the sensor
     */
    private boolean iAvailable;

    /** ContextType of the sensor to qualify the type of the sensor more */
    private String contextType;
    private String description;

    private String model;

    private Hashtable properties;

    /**
     * Quantity
     */
    private String quantity;

    /**
     * Url to instantiate new SensorConnection instance
     */
    private String url;

    /**
     * Index number for this sensor, used for creating new SensorConnections
     */
    int iIndex;

    private static ResourceLoader iRes = null;

    /**
     * Constructor of SensorInfo object, contains also some additional
     * parameters, like className and innerUrl and permission, which are set to
     * properties
     */
    SensorInfoImpl(String aSensorInfo, int aIndex)
    {
        iIndex = aIndex;
        properties = new Hashtable();

        // Quantity
        int start = 0;
        int end = aSensorInfo.indexOf(SCHEME_SEP_INTERNAL);
        if (end <= 0)
        {
            throw new IllegalArgumentException(INVALID_SENSORINFO);
        }
        quantity = aSensorInfo.substring(start, end);

        // Context type
        start = end + 1;
        end = aSensorInfo.indexOf(SCHEME_SEP_INTERNAL, start);
        if (end <= start)
        {
            throw new IllegalArgumentException(INVALID_SENSORINFO);
        }
        contextType = aSensorInfo.substring(start, end);

        // Location
        start = end + 1;
        end = aSensorInfo.indexOf(SCHEME_SEP_INTERNAL, start);
        if (end <= start)
        {
            throw new IllegalArgumentException(INVALID_SENSORINFO);
        }
        properties.put(PROP_LOCATION, aSensorInfo.substring(start, end));

        // Vendor
        start = end + 1;
        end = aSensorInfo.indexOf(SCHEME_SEP_INTERNAL, start);
        if (end <= start)
        {
            throw new IllegalArgumentException(INVALID_SENSORINFO);
        }
        properties.put(PROP_VENDOR, aSensorInfo.substring(start, end));

        // Description
        start = aSensorInfo.indexOf(SCHEME_SEP_EQUAL, end) + 1;
        end = aSensorInfo.indexOf(SCHEME_SEP_INTERNAL, start);
        if (end <= start)
        {
            throw new IllegalArgumentException(INVALID_SENSORINFO);
        }
        String textId = aSensorInfo.substring(start, end);
        if (iRes == null)
        {
            iRes = ResourceLoader.getInstance(FILE_RES_NAME, FILE_RES_START);
        }
        description = iRes.format(textId, (Object[]) null);

        // Model
        start = aSensorInfo.indexOf(SCHEME_SEP_EQUAL, end) + 1;
        end = aSensorInfo.indexOf(SCHEME_SEP_INTERNAL, start);
        if (end <= start)
        {
            throw new IllegalArgumentException(INVALID_SENSORINFO);
        }
        model = aSensorInfo.substring(start, end);

        // Connection type
        start = aSensorInfo.indexOf(SCHEME_SEP_EQUAL, end) + 1;
        end = aSensorInfo.indexOf(SCHEME_SEP_INTERNAL, start);
        if (end <= start)
        {
            throw new IllegalArgumentException(INVALID_SENSORINFO);
        }
        String conn = aSensorInfo.substring(start, end);
        connectionType = Integer.parseInt(conn);

        url = createUrl();

        // isAvailable
        start = end + 1;
        end = aSensorInfo.indexOf(SCHEME_SEP_INTERNAL, start);
        if (end <= start)
        {
            throw new IllegalArgumentException(INVALID_SENSORINFO);
        }
        String avail = aSensorInfo.substring(start, end);
        if (Integer.parseInt(avail) != 0)
        {
            iAvailable = true;
        }
        else
        {
            iAvailable = false;
        }

        // Next create ChannelInfos

        // Channel count
        start = end + 1;
        end = aSensorInfo.indexOf(SCHEME_SEP_INTERNAL, start);
        if (end <= start)
        {
            throw new IllegalArgumentException(INVALID_SENSORINFO);
        }
        String chCountStr = aSensorInfo.substring(start, end);
        int channelCount = Integer.parseInt(chCountStr);

        iChannelInfos = new ChannelInfo[ channelCount ];
        start = end + 1;
        for (int i = 0; i < channelCount; i++)
        {
            end = aSensorInfo.indexOf(CHANNEL_END_SEP, start);
            String channelString = aSensorInfo.substring(start, end);
            iChannelInfos[ i ] = new ChannelInfoImpl(channelString);
            start = end + CHANNEL_END_SEP.length();
        }
    }

    /**
     * <p>
     * SensorConnection implementation can provide data from different channels
     * - for example sensor measuring Quantity.DIRECTION could return "heading",
     * "pitch" and "roll" values in one shot. Heading, pitch and roll are
     * understood as different channels.
     * </p>
     *
     * <p>
     * Majority of sensors still provide information from one channel only. For
     * example, thermometer returns only one value representing temperature,
     * which means that sensor has only one channel. Application gets data with
     * request {@link SensorConnection#getData} or if application is registered
     * as {@link DataListener} the values are sent with dataReceived()
     * notification. In the case of one channel only the length of Data array
     * will be 1.
     * </p>
     *
     * <p>
     * Each {@link Data} object is identified by the {@link ChannelInfoImpl} not
     * to mix measurements from different channels. All the channels of the
     * sensor can be queried with the this method.
     * </p>
     *
     * @return channels (Channel is used for identification of {@link Data}).
     */
    public ChannelInfo[] getChannelInfos()
    {
        return iChannelInfos;
    }

    /**
     * Return the connection type of the sensor. Possible values are
     * {@link #CON_EMBEDDED}, {@link #CON_WIRED},
     * {@link #CON_SHORT_RANGE_WIRELESS} and {@link #CON_REMOTE}.
     * <ul>
     * <li>CON_EMBEDDED means sensor which is inside the phone</li>
     * <li>CON_WIRED means that sensor is plugged to the device with wire, e.g.
     * into the serial or USB port</li>
     * <li>CON_SHORT_RANGE_WIRELESS means sensors which are connected e.g. via
     * Bluetooth or infrared or WLAN.</li>
     * <li>CON_REMOTE sensor can be accessed with e.g. HTTP connection</li>
     * </ul>
     *
     * @return the type of sensor connection
     */
    public int getConnectionType()
    {
        return connectionType;
    }

    /**
     * Returns the context type of the sensor. Possible context types are
     * specified in {@link ContextType}.
     *
     * @return context type of the sensor
     */
    public String getContextType()
    {
        return contextType;
    }

    /**
     * Method returns human readable and localized description of the sensor.
     * Description tells essentials of the sensor, like at least sensor type
     * (accelerometer,microphone, thermometer) and meaning of channels
     *
     * @return description of the SensorConnection
     */
    public String getDescription()
    {
        return description;
    }

    public int getMaxBufferSize()
    {
        return MAX_BUFFER_SIZE;
    }

    /**
     * Returns the model of the sensor, model is vendor specific string, which
     * format is free.
     *
     * @return model of the sensor
     */
    public String getModel()
    {
        return model;
    }

    /**
     * <p>
     * Method will return all the keys of the properties, which are set. Meant
     * to be used in conjunction with the method {@link #getProperty(String)}
     * once the key is got.
     * </p>
     *
     * @return keys of the properties set as String array - returns 0-length
     *         array if no properties are set
     */
    public synchronized String[] getPropertyNames()
    {
        if (properties == null)
            return NO_PROPERTIES;

        String[] keys = new String[ properties.size()];

        int i = 0;
        for (Enumeration e = properties.keys(); e.hasMoreElements();)
        {
            keys[ i ] = (String)e.nextElement();
            i++;
        }

        return keys;

    }

    /**
     * Method provided to be able to query additional property values set for
     * this specific {@link SensorConnection}
     *
     * @param key
     *            - property name
     * @return - property value
     * @throws java.lang.NullPointerException
     *             if key is null
     * @throws java.lang.IllegalArgumentException
     *             if the key is not any item of String array returned with
     *             getProperties() -method.
     */
    public synchronized Object getProperty(String key)
    {
        if (key == null)
        {
            throw new NullPointerException(
                "Retrieving property name failed: property name is null");
        }

        if (!properties.containsKey(key))
        {
            throw new IllegalArgumentException("Retrieving property name failed: "
                                               + key + " is not found");
        }
        return properties.get(key);
    }

    /**
     * Returns the quantity of the sensor. Quantities are specified in
     * {@link Quantity}.
     *
     * @return quantity of the sensor
     */
    public String getQuantity()
    {
        return quantity;
    }

    /**
     * The url to make {@link SensorConnection} with
     * {@link javax.microedition.io.Connector#open} call. Quantity, context type
     * and model MUST be present in the returned string. If defined, location is
     * presented, too.
     *
     * @return sensor url
     */
    public final String getUrl()
    {
        return url;
    }

    public boolean isAvailable()
    {
        return iAvailable;
    }

    /*
     * (non-Javadoc)
     *
     * @see javax.microedition.sensor.SensorInfo#isAvailabilityPushSupported()
     */
    public boolean isAvailabilityPushSupported()
    {
        return false;
    }

    /*
     * (non-Javadoc)
     *
     * @see javax.microedition.sensor.SensorInfo#isConditionPushSupported()
     */
    public boolean isConditionPushSupported()
    {
        return false;
    }

    /**
     * Returns proper Channel object with the name of the channel
     *
     * @param name
     *            name of the channel
     * @return Channel object with given name, null if the Channel cannot be
     *         found
     */
    ChannelInfo getChannelInfo(String name)
    {
        for (int i = 0, l = iChannelInfos.length; i < l; i++)
        {
            if (iChannelInfos[ i ].getName().equals(name))
                return iChannelInfos[ i ];
        }
        return null;
    }


    /**
     * Url generated when all the variables are set
     *
     * @return url based on class members
     */
    private String createUrl()
    {
        if (!properties.containsKey(PROP_LOCATION))
            return SCHEME_SENSOR + quantity + SCHEME_SEP_EXTERNAL + SCHEME_CONTEXT_TYPE
                   + contextType + SCHEME_SEP_EXTERNAL + SCHEME_MODEL + model;

        return SCHEME_SENSOR + quantity + SCHEME_SEP_EXTERNAL + SCHEME_CONTEXT_TYPE
               + contextType + SCHEME_SEP_EXTERNAL + SCHEME_MODEL + model + SCHEME_SEP_EXTERNAL
               + SCHEME_LOCATION + properties.get(PROP_LOCATION);
    }
}
