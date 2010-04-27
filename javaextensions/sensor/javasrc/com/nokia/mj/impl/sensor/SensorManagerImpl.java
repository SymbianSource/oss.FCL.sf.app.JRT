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
* Description:  SensorManager implementation
*
*/


package com.nokia.mj.impl.sensor;

import javax.microedition.sensor.SensorConnection;
import javax.microedition.sensor.SensorInfo;
import javax.microedition.sensor.SensorListener;
import java.util.Hashtable;
import java.util.Vector;
import com.nokia.mj.impl.rt.support.Finalizer;
import com.nokia.mj.impl.rt.support.Jvm;


/**
 *
 */
public class SensorManagerImpl
{
    static
    {
        try
        {
            Jvm.loadSystemLibrary("javasensor");
        }
        catch (Exception e)
        {

        }
    }


    static boolean isDebug = /*
                                 * System.getProperty("microedition.sensor.debug") ==
                                 * null ? false :
                                 */true;

    /**
     * Help constant to be returned, when no SensorConnection is found with
     * given parameters
     */
    private static final SensorInfoImpl[] NO_INFOS = new SensorInfoImpl[ 0 ];

    /*
     * private static final String[] PERMISSIONS = {
     * "javax.microedition.sensor.ProtectedSensor",
     * "javax.microedition.sensor.PrivateSensor" };
     */

    private static final char[] RESERVED = { ';', '/', '?', ':', '@', '&', '=',
                                           '+', '$', ','
                                           };

    /**
     * Help constant used when sensor url is parsed
     */
    private static final String SCHEME_CONTEXT = "contextType";

    /**
     * Help constant used when sensor url is parsed
     */
    private static final String SCHEME_LOCATION = "location";

    /**
     * Help constant used when sensor url is parsed
     */
    private static final String SCHEME_MODEL = "model";

    /**
     * Help constant used when sensor url is parsed
     */
    private static final String SCHEME_QUERY_SEP = "?";

    /**
     * Help constant used when sensor url is parsed
     */
    private static final String SCHEME_SENSOR = "sensor:";

    /**
     * Help constant used when sensor url is parsed
     */
    private static final char SCHEME_COLON = ':';

    /**
     * Help constant used when sensor url is parsed
     */
    private static final char SCHEME_SEMICOLON = ';';

    /**
     * Help constant used when sensor url is parsed
     */
    private static final char SCHEME_EQUALS = '=';

    /**
     * URL parsing exception message prefix
     */
    private static final String INVALID_URL_PREFIX = "Invalid URL: ";

    /**
     * Singleton pattern: the one and only SensorManager instance created
     */
    private static SensorManagerImpl _instance;

    /**
     * SensorListeners monitoring changes in states of the registered sensors
     */
    private Vector sensorListeners = new Vector();

    /*
     * Native SensorConnectionList handle
     */
    int sSensorConnectionList;

    /*
     * SensorInfos, cached when SensorManager is created
     */
    private SensorInfo[] iSensorInfos;

    /**
     * Finalizer
     */
    private Finalizer iFinalizer;

    /**
     * Default constructor that is used to create SensorManager instance.
     */
    private SensorManagerImpl()
    {
        iFinalizer = registerForFinalization();

        Vector sensorIdentifiers = new Vector();
        sSensorConnectionList = _initSensors(sensorIdentifiers);

        if (sSensorConnectionList < 0)
        {
            throw new OutOfMemoryError();
        }

        if (sensorIdentifiers.isEmpty())
        {
            throw new OutOfMemoryError();
        }

        int sensorCount = sensorIdentifiers.size();
        iSensorInfos = new SensorInfo[ sensorCount ];
        for (int i = 0; i < sensorCount; i++)
        {
            iSensorInfos[ i ] = new SensorInfoImpl(
                (String) sensorIdentifiers.elementAt(i), i);
        }
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

        if (sSensorConnectionList > 0)
        {
            _dispose(sSensorConnectionList);
        }
    }

    /**
     * This method is used to get an actual SensorManagerImpl implementation.
     *
     * @return SensorManager implementation
     */
    public static SensorManagerImpl getInstance()
    {
        if (_instance == null)
        {
            _instance = new SensorManagerImpl();
        }
        return _instance;
    }

    /**
     * <p>
     * Registers SensorListener to monitor state of selected sensor.
     * </p>
     *
     * <p>
     * SensorListener is notified when
     * <ul>
     * <li>SensorConnection matching given SensorInfo is getting either
     * available or unavailable.</li>
     * </ul>
     * </p>
     *
     * @param listener SensorListener to be registered
     * @param info SensorInfo instance
     * @throws java.lang.NullPointerException if either of the listener or info
     *             is null
     */
    public synchronized void addSensorListener(SensorListener listener,
            SensorInfo info)
    {

        if (listener == null)
        {
            throw new NullPointerException(
                "Adding sensor listener failed, listener is null");
        }
        if (info == null)
        {
            throw new NullPointerException(
                "Adding sensor listener failed, SensorInfo is null");
        }

        int i = 0;
        int l = iSensorInfos.length;
        for (; i < l; i++)
        {
            if (iSensorInfos[ i ] == info)
                break;
        }

        if (i == l)
            throw new IllegalArgumentException(
                "Adding Sensor Listener failed, unknown SensorInfo");

        if (!sensorListeners.contains(listener))
        {
            if (info.isAvailable())
            {
                listener.sensorAvailable(info);
            }
            else
            {
                listener.sensorUnavailable(info);
            }
            sensorListeners.addElement(listener);
        }
    }

    /**
     * <p>
     * Registers SensorListener to monitor changes in the state of any
     * {@link SensorConnection} defined with quantity.
     * </p>
     *
     * <p>
     * SensorListener is notified when any SensorConnection of defined Quantity
     * is getting available or unavailable.
     * </p>
     *
     * @param listener SensorListener to be registered
     * @param quantity Quantity of the SensorConnection to be listened
     * @throws java.lang.NullPointerException if listener or quantity is null
     */
    public void addSensorListener(SensorListener listener, String quantity)
    {

        if (listener == null)
            throw new NullPointerException(
                "Adding sensor listener failed, listener is null");

        if (quantity == null)
            throw new NullPointerException(
                "Adding sensor listener failed, quantity is null");

        SensorInfo[] infos = findSensors(quantity, null);

        for (int i = 0, l = infos.length; i < l; i++)
        {
            addSensorListener(listener, infos[ i ]);
        }
    }

    /**
     * Returns array of {@link SensorInfoImpl} objects of such sensors, which
     * are matching specified quantity and context type. Possible quantities are
     * defined in {@link Quantity}. If both parameters are null returns
     * SensorInfos of all supported sensors
     *
     * @param quantity - the quantity of the sensor
     * @param contextType - the context type of the sensor is telling the
     *            purpose of use of the sensor
     * @return array of {@link SensorInfoImpl} objects of all supported sensors
     *         with specified quantity and contextType - returns 0-length array
     *         of SensorInfos if no match is found
     */
    public SensorInfo[] findSensors(String quantity, String contextType)
    {
        // when both values are null return all
        if (quantity == null && contextType == null)
        {
            return iSensorInfos;
        }

        // if quantity is null, we can't use findSensors( url )
        if (quantity == null)
        {
            if (isValidContext(contextType))
            {
                Vector matchingSensorInfos = new Vector();
                for (int i = 0; i < iSensorInfos.length; i++)
                {
                    if (iSensorInfos[ i ].getContextType()
                            .equals(contextType))
                    {
                        matchingSensorInfos.addElement(iSensorInfos[ i ]);
                    }
                }
                SensorInfo[] result = new SensorInfo[ matchingSensorInfos
                                                      .size()];
                matchingSensorInfos.copyInto(result);
                return result;
            }
            else
            {
                throw new IllegalArgumentException(
                    "Finding sensor failed: invalid contextType  "
                    + contextType);
            }
        }
        else
        {
            String url = SCHEME_SENSOR + quantity;
            if (contextType != null)
            {
                url += SCHEME_SEMICOLON + SCHEME_CONTEXT + SCHEME_EQUALS + contextType;
            }
            return findSensors(url);
        }
    }

    /**
     * Method that checks if the given context type is Valid
     *
     * @param contextType context type string
     * @return true if context type is valid, othewise false
     */
    private boolean isValidContext(String contextType)
    {
        if (contextType == null)
        {
            return false;
        }

        if (!contextType.equals(SensorInfo.CONTEXT_TYPE_AMBIENT)
                && !contextType.equals(SensorInfo.CONTEXT_TYPE_DEVICE)
                && !contextType.equals(SensorInfo.CONTEXT_TYPE_USER)
                && !contextType.equals(SensorInfo.CONTEXT_TYPE_VEHICLE))
        {
            return false;
        }
        return true;
    }

    /**
     * Returns array of {@link SensorInfoImpl} objects of all sensors, which are
     * matching the criteria specified in url. Sensor url scheme is defined in
     * {@link SensorConnection}, the only mandatory field being quantity.
     *
     * @param url - sensor url to be parsed; retrieved information is used as
     *            search criteria to find proper SensorConnections
     *
     * @return array of {@link SensorInfoImpl} objects of all supported sensors
     *         with specified type
     * @throws java.lang.NullPointerException if url is null
     * @throws java.lang.IllegalArgumentException if url is malformed according
     *             to the sensor url scheme
     */
    public SensorInfo[] findSensors(String url)
    {
        if (url == null)
        {
            throw new NullPointerException("Finding sensors failed: URL is null");
        }

        // cut first "sensor:" away
        if (url.startsWith(SCHEME_SENSOR))
        {
            url = url.substring(SCHEME_SENSOR.length());
        }
        else
        {
            throw new IllegalArgumentException(
                INVALID_URL_PREFIX + "URL does not start with " + SCHEME_SENSOR);
        }

        // if there is still colon left throw IllegalArgumentException
        if (url.indexOf(SCHEME_COLON) > -1)
        {
            throw new IllegalArgumentException(
                INVALID_URL_PREFIX + "colon seperator not allowed after" + SCHEME_SENSOR);
        }

        boolean isOnlyQuantity = false;
        int start = 0;
        int end = 0;

        end = url.indexOf(SCHEME_SEMICOLON, start);
        if (end < 0)
            end = url.indexOf(SCHEME_QUERY_SEP, start);

        // only quantity given
        if (end < 0)
        {
            end = url.length();
            isOnlyQuantity = true;
        }

        String quantity = url.substring(start, end);

        if (quantity == null || quantity.length() == 0)
        {
            throw new IllegalArgumentException(
                INVALID_URL_PREFIX + "quantity is null");
        }

        // check no forbidden letters
        if (containsIllegalCharacters(quantity))
        {
            throw new IllegalArgumentException(
                INVALID_URL_PREFIX + "quantity " + quantity + " contains illegal characters");
        }

        String contextType = null;
        String location = null;
        String model = null;

        // parse other values
        if (!isOnlyQuantity)
        {
            // strip quantity
            url = url.substring(quantity.length() + 1);
            Hashtable properties = parseProperty(url);

            contextType = (String) properties.remove(SCHEME_CONTEXT);
            if (contextType != null && !isValidContext(contextType))
            {
                throw new IllegalArgumentException(
                    INVALID_URL_PREFIX + "contextType is invalid");
            }
            location = (String) properties.remove(SCHEME_LOCATION);
            model = (String) properties.remove(SCHEME_MODEL);
            if (properties.size() != 0)
            {
                throw new IllegalArgumentException(
                    INVALID_URL_PREFIX + "URL contains invalid property name-value pair(s)");
            }
        }

        // Filter the matching Sensors
        Vector matchingInfos = new Vector();

        for (int i = 0; i < iSensorInfos.length; i++)
        {
            if (!iSensorInfos[ i ].getQuantity().equals(quantity))
            {
                continue;
            }

            if (contextType != null
                    && !contextType.equals(iSensorInfos[ i ].getContextType()))
            {
                continue;
            }

            if (model != null
                    && !model.equals(iSensorInfos[ i ].getModel()))
            {
                continue;
            }

            if (location != null)
            {
                Object sensorLoc = null;
                try
                {
                    sensorLoc = iSensorInfos[ i ]
                                .getProperty(SensorInfo.PROP_LOCATION);
                }
                catch (IllegalArgumentException iae)
                {
                    // sensorinfo did not have location property
                }
                if (!location.equals(sensorLoc))
                {
                    continue;
                }
            }
            // If all the given parameters matched, add sensor to matchingInfos
            matchingInfos.addElement(iSensorInfos[ i ]);
        }

        if (matchingInfos.size() > 0)
        {
            SensorInfo[] result = new SensorInfo[ matchingInfos.size()];
            matchingInfos.copyInto(result);
            return result;
        }
        else
        {
            return NO_INFOS;
        }
    }

    /**
     * Method to check if given string contains characters not allowed in URL
     * string values
     *
     * @param aValue string to check
     * @return true if string contains illegal characters, otherwise false
     * @throws NullPointerException if given string is null
     */
    private boolean containsIllegalCharacters(String aValue)
    {
        if (aValue == null)
        {
            throw new NullPointerException();
        }
        for (int i = 0, ll = RESERVED.length; i < ll; i++)
        {
            if (aValue.indexOf(RESERVED[ i ]) > -1)
            {
                return true;
            }
        }
        return false;
    }

    /**
     * Parses a property value of given property name from properties string.
     * Properties string should be in formant:
     *
     * @param aProperties string containing the properties in
     *            "name1=value1;name2=value2;..." format
     * @param aName name of the wanted property
     * @return property value as string or null if not found
     */
    private Hashtable parseProperty(String aProperties)
    {
        Hashtable result = new Hashtable();
        int start = 0;
        int end = -1;
        int length = aProperties.length();
        while (start >= 0 && start < length)
        {
            end = aProperties.indexOf(SCHEME_EQUALS, start);
            if (-1 == end)
            {
                throw new IllegalArgumentException(INVALID_URL_PREFIX +
                                                   "property name " + aProperties + " not found");
            }
            String key = aProperties.substring(start, end);

            start = end + 1;
            end = aProperties.indexOf(SCHEME_SEMICOLON, start);
            if (-1 == end && start < length)
            {
                end = length;
            }
            if (-1 == end)
            {
                throw new IllegalArgumentException(start + INVALID_URL_PREFIX +
                                                   " value not found for property name " + aProperties);
            }
            String value = aProperties.substring(start, end);
            result.put(key, value);
            start = end + 1;
        }
        return result;
    }

    /**
     * Removes SensorListener from the list of listeners, which are monitoring
     * defined sensor.
     *
     * @param listener the SensorListener to be removed.
     * @throws java.lang.NullPointerException if listener is null
     */
    public synchronized void removeSensorListener(SensorListener listener)
    {
        if (listener == null)
        {
            throw new NullPointerException();
        }
        synchronized (sensorListeners)
        {
            sensorListeners.removeElement(listener);
        }
    }

    /**
     * Initializes sensors and fills given Vector with sensor information
     * @param aSensorIdentifiers Sensor information vector
     * @return Handle to native counterpart, or error code
     */
    private native int _initSensors(Vector aSensorIdentifiers);

    /**
     * Deletes native counterpart of this class
     * @param sensorConnectionList Handle to native counterpart
     */
    private native void _dispose(int sensorConnectionList);
}
