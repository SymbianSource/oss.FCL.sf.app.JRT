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

import com.nokia.mj.impl.sensor.SensorManagerImpl;


/**
 * Please refer JSR 256 for more details.
 */
public final class SensorManager
{

    private static SensorManagerImpl sensorManagerImpl = SensorManagerImpl.getInstance();


    /**
     * The default constructor that is used to
     * create a <code>SensorManager</code> instance
     */
    private SensorManager()
    {
    }



    /**
     * Please refer JSR 256 for more details.
     */
    public static void addSensorListener(SensorListener listener, SensorInfo info)
    {
        sensorManagerImpl.addSensorListener(listener, info);
    }



    /**
     * Please refer JSR 256 for more details.
     */
    public static void addSensorListener(SensorListener listener, String quantity)
    {
        sensorManagerImpl.addSensorListener(listener,quantity);
    }




    /**
     * Please refer JSR 256 for more details.
     */
    public static SensorInfo[] findSensors(String quantity, String contextType)
    {
        return sensorManagerImpl.findSensors(quantity,contextType);
    }


    /**
     * Please refer JSR 256 for more details.
     */
    public static SensorInfo[] findSensors(String url)
    {
        return sensorManagerImpl.findSensors(url);
    }





    /**
     * Please refer JSR 256 for more details.
     */
    public static void removeSensorListener(SensorListener listener)
    {
        sensorManagerImpl.removeSensorListener(listener);
    }


}
