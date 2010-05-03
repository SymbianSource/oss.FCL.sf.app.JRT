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
* Description:  JNI implementation for SensorManagerImpl.java
*
*/

#include <jni.h>
#include <string.h>
#include "com_nokia_mj_impl_sensor_SensorManagerImpl.h"
#include "sensorconnectionlist.h"
#include "sensorjniutils.h"
#include "logger.h"

/*
 * Function for releasing the sensor info array before return
 */
void freeSensorArray(jchar** aSensors, int aCount)
{
    JELOG2(ESensor);
    for (int i = 0; i < aCount; i++)
    {
        delete [] aSensors[i];
    }
    delete aSensors;
}

/*
 * Class:     com_nokia_mj_impl_sensor_SensorManagerImpl
 * Method:    _initSensors
 * Signature: ([Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_sensor_SensorManagerImpl__1initSensors
(JNIEnv* aJniEnv, jobject, jobject aSensorIdVector)
{
    JELOG2(ESensor);
    SensorConnectionList* sensorConnectionList = new SensorConnectionList();
    if (!sensorConnectionList)
    {
        return ERROR_NOMEMORY;
    }

    // ownership of char** is received
    int sensorCount = 0;
    jchar** sensors = sensorConnectionList->InitSensors(&sensorCount);
    if (!sensors)
    {
        freeSensorArray(sensors, sensorCount);
        return ERROR_GENERAL;
    }

    jmethodID addElement =
        aJniEnv->GetMethodID(
            aJniEnv->GetObjectClass(aSensorIdVector),
            "addElement",
            "(Ljava/lang/Object;)V");

    if (HandleException(*aJniEnv) < ERROR_NONE)
    {
        freeSensorArray(sensors, sensorCount);
        return ERROR_GENERAL;
    }

    for (int i = 0; i < sensorCount; i++)
    {
        int strLength = 0;
        while (sensors[ i ][ strLength++ ] != '\0') {};
        jstring javaString = aJniEnv->NewString(sensors[ i ], strLength);

        // If NewStringUTF fails to allocate memory it returns NULL and throws OutOfMemoryError
        if (!javaString)
        {
            freeSensorArray(sensors, sensorCount);
            // OutOfMemoryError thrown on java side
            return ERROR_NOMEMORY;
        }

        aJniEnv->CallVoidMethod(aSensorIdVector, addElement, javaString);
    }

    freeSensorArray(sensors, sensorCount);
    return reinterpret_cast<int>(sensorConnectionList);
}

/*
 * Class:     com_nokia_mj_impl_sensor_SensorManagerImpl
 * Method:    _dispose
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_sensor_SensorManagerImpl__1dispose
(JNIEnv *, jobject, jint aSensorList)
{
    JELOG2(ESensor);
    SensorConnectionList* senList = reinterpret_cast< SensorConnectionList* >(aSensorList);
    delete senList;
}
