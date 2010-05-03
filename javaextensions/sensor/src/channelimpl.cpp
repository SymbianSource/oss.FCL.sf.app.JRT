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
* Description:  JNI implementation for ChannelImpl.java
*
*/

#include "sensorjniutils.h"
#include "sensornativeconstants.h"
#include "sensor.h"
#include "sensorconnection.h"
#include "com_nokia_mj_impl_sensor_ChannelImpl.h"
#include "logger.h"

/*
 * Class:     com_nokia_mj_impl_sensor_ChannelImpl
 * Method:    _addCondition
 * Signature: (IIDDII)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_sensor_ChannelImpl__1addCondition
(JNIEnv* /*aJniEnv*/, jobject, jint aConnection, jint aChannel,
 jdouble aLowerLimit, jdouble aUpperLimit, jint aLowerOp, jint aUpperOp)
{
    JELOG2(ESensor);
    SensorConnection* sc = reinterpret_cast< SensorConnection* >(aConnection);
    Sensor* sensor = sc->GetSensor();

    void* conditionHandle = NULL;

    int err = sensor->AddCondition(&conditionHandle,
                                   aChannel, aLowerLimit, aUpperLimit, aLowerOp, aUpperOp);
    if (err != ERROR_NONE)
    {
        return err;
    }

    return reinterpret_cast< int >(conditionHandle);
}

/*
 * Class:     com_nokia_mj_impl_sensor_ChannelImpl
 * Method:    _removeCondition
 * Signature: (I)V
 */
JNIEXPORT int JNICALL Java_com_nokia_mj_impl_sensor_ChannelImpl__1removeCondition
(JNIEnv *, jobject, jint aConnection, jint aConditionId)
{
    JELOG2(ESensor);
    SensorConnection* sc = reinterpret_cast< SensorConnection* >(aConnection);
    Sensor* sensor = sc->GetSensor();

    void* conditionHandle = reinterpret_cast< void* >(aConditionId);
    return sensor->RemoveCondition(conditionHandle);
}
