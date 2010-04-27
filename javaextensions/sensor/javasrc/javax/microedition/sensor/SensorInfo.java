/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


package javax.microedition.sensor;



/**
 * Please refer JSR 256 for more details.
 */
public interface SensorInfo
{
    /**
     * Please refer JSR 256 for more details.
     */
    public static final int CONN_EMBEDDED = 1;

    /**
     * Please refer JSR 256 for more details.
     */
    public static final int CONN_REMOTE = 2;

    /**
     * Please refer JSR 256 for more details.
     */
    public static final int CONN_SHORT_RANGE_WIRELESS = 4;

    /**
     * Please refer JSR 256 for more details.
     */
    public static final int CONN_WIRED = 8;


    /**<p>
     * This context type defines ambient, environmental sensors.</p>
     */
    public static final String CONTEXT_TYPE_AMBIENT = "ambient";

    /**
     * Please refer JSR 256 for more details.
     */
    public static final String CONTEXT_TYPE_VEHICLE = "vehicle";

    /**
     * Please refer JSR 256 for more details.
     */
    public static final String CONTEXT_TYPE_DEVICE = "device";

    /**
     * Please refer JSR 256 for more details.
     */
    public static final String CONTEXT_TYPE_USER = "user";

    /**
     * Please refer JSR 256 for more details.
     */
    public static final String PROP_IS_CONTROLLABLE="controllable";

    /**
     * Please refer JSR 256 for more details.
     */
    public static final String PROP_IS_REPORTING_ERRORS="errorsReported";

    /**
    * Please refer JSR 256 for more details.
    */
    public static final String PROP_LATITUDE = "latitude";

    /**Please refer JSR 256 for more details.
    *
    */
    public static final String PROP_LOCATION = "location";

    /**Please refer JSR 256 for more details.
    *
    */
    public static final String PROP_LONGITUDE = "longitude";

    /**Please refer JSR 256 for more details.
    *
    */
    public static final String PROP_MAX_RATE = "maxSamplingRate";

    /**
     * Please refer JSR 256 for more details.
     */
    public static final String PROP_PERMISSION = "permission";

    /**
    * Please refer JSR 256 for more details.
    */
    public static final String PROP_VENDOR = "vendor";

    /**
    * Please refer JSR 256 for more details.
    */
    public static final String PROP_VERSION = "version";



    /**
     * Please refer JSR 256 for more details.
     */
    public ChannelInfo[] getChannelInfos();

    /**
     * Please refer JSR 256 for more details.
     */
    public int getConnectionType();

    /**
     * Please refer JSR 256 for more details.
     */
    public String getContextType();


    /**
     * Please refer JSR 256 for more details.
     */
    public String getDescription();


    /**
     * Please refer JSR 256 for more details.
     */
    public int getMaxBufferSize();



    /**
     * Please refer JSR 256 for more details.
     */
    public String getModel();

    /**
     * Please refer JSR 256 for more details.
     */
    public String[] getPropertyNames();

    /**
     * Please refer JSR 256 for more details.
     */
    public Object getProperty(String name);

    /**
     * Please refer JSR 256 for more details.
     */
    public String getQuantity();

    /**
     * Please refer JSR 256 for more details.
     */
    public String getUrl();



    /**
     * Please refer JSR 256 for more details.
     */
    public boolean isAvailable();


    /**
     * Please refer JSR 256 for more details.
     */
    public boolean isAvailabilityPushSupported();

    /**
     * Please refer JSR 256 for more details.
     */
    public boolean isConditionPushSupported();



}