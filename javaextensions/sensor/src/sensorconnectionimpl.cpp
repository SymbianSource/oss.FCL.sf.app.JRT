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
* Description:  JNI implementation for SensorConnectionImpl.java
*
*/


#include "com_nokia_mj_impl_sensor_SensorConnectionImpl.h"
#include "sensorjniutils.h"
#include "sensornativeconstants.h"
#include "sensor.h"
#include "sensorconnection.h"
#include "sensorconnectionlist.h"
#include "logger.h"

void ConditionCallback(void* aJniEnv, void* aJavaPeer, void* aHandle, int aChannel,
                       double aValue, long aTimeStamp)
{
    JELOG2(ESensor);
    JNIEnv* jniEnv = (JNIEnv*)aJniEnv;
    jobject peer = (jobject) aJavaPeer;
    jclass cls = jniEnv->GetObjectClass(peer);
    jmethodID mid = jniEnv->GetMethodID(cls, "conditionMet", "(IIDJ)V");
    int condId = reinterpret_cast< int >(aHandle);
    jniEnv->CallVoidMethod(peer, mid, aChannel, condId, aValue, aTimeStamp);
    // Handle possible exception in callback
    HandleException(*jniEnv);
}

void FillData(DataFillerParams* aParams)
{
    JELOG2(ESensor);
    JNIEnv* jniEnv = (JNIEnv*)aParams->iJniEnv;
    jobjectArray destData = (jobjectArray)aParams->iDataObjects;

    // Assign data
    for (int i = 0; i < aParams->iDataCount; i++)
    {
        jobject data = jniEnv->GetObjectArrayElement(destData, i);
        jclass dataClass = jniEnv->GetObjectClass(data);

        if (aParams->iDatas[ i ]->iIntValues)
        {
            jfieldID field = jniEnv->GetFieldID(dataClass, "iIntValues", "[I");
            jintArray intValues = jniEnv->NewIntArray(aParams->iDatas[ i ]->iNumOfValues);
            jniEnv->SetIntArrayRegion(intValues, 0, aParams->iDatas[ i ]->iNumOfValues,
                                      aParams->iDatas[ i ]->iIntValues);
            jniEnv->SetObjectField(data, field, intValues);
            jniEnv->DeleteLocalRef(intValues);
            HandleException(*jniEnv);
        }
        else if (aParams->iDatas[ i ]->iDoubleValues)
        {
            jfieldID field = jniEnv->GetFieldID(dataClass, "iDoubleValues", "[D");
            jdoubleArray doubleValues = jniEnv->NewDoubleArray(aParams->iDatas[ i ]->iNumOfValues);
            jniEnv->SetDoubleArrayRegion(doubleValues, 0, aParams->iDatas[ i ]->iNumOfValues,
                                         aParams->iDatas[ i ]->iDoubleValues);
            jniEnv->SetObjectField(data, field, doubleValues);
            jniEnv->DeleteLocalRef(doubleValues);
            HandleException(*jniEnv);
        }
        if (aParams->iDatas[ i ]->iTimeStampsIncluded)
        {
            jfieldID field = jniEnv->GetFieldID(dataClass, "iTimestamps", "[J");
            jlongArray timestamps = jniEnv->NewLongArray(aParams->iDatas[ i ]->iNumOfValues);
            jniEnv->SetLongArrayRegion(timestamps, 0, aParams->iDatas[ i ]->iNumOfValues,
                                       reinterpret_cast<jlong*>(aParams->iDatas[ i ]->iTimeStamps));
            jniEnv->SetObjectField(data, field, timestamps);
            jniEnv->DeleteLocalRef(timestamps);
            HandleException(*jniEnv);
        }
        if (aParams->iDatas[ i ]->iValiditiesIncluded)
        {
            jfieldID field = jniEnv->GetFieldID(dataClass, "iValidities", "[Z");
            jbooleanArray validities = jniEnv->NewBooleanArray(aParams->iDatas[ i ]->iNumOfValues);
            jniEnv->SetBooleanArrayRegion(validities, 0, aParams->iDatas[ i ]->iNumOfValues,
                                          reinterpret_cast<jboolean*>(aParams->iDatas[ i ]->iValidities));
            jniEnv->SetObjectField(data, field, validities);
            jniEnv->DeleteLocalRef(validities);
            HandleException(*jniEnv);
        }
    }
    jobject peer = (jobject) aParams->iJavaPeer;
    jclass cls = jniEnv->GetObjectClass(peer);
    jmethodID mid = jniEnv->GetMethodID(cls, "dataReceived", "([Ljavax/microedition/sensor/Data;Z)V");
    jniEnv->CallVoidMethod(peer, mid, destData, aParams->iIsDataLost);
    // Handle possible exception in callback
    HandleException(*jniEnv);
}

/*
 * Class:     com_nokia_mj_impl_sensor_SensorConnectionImpl
 * Method:    _createSensorConnection
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_sensor_SensorConnectionImpl__1createSensorConnection
(JNIEnv* aJniEnv, jobject aJobj, jint aSensorList, jint aIndex)
{
    JELOG2(ESensor);
    SensorConnectionList* senList = reinterpret_cast< SensorConnectionList* >(aSensorList);
    SensorConnection* conn = senList->CreateSensorConnection(aIndex);
    if (!conn)
    {
        return ERROR_NOMEMORY;
    }
    int err = conn->GetSensor()->OpenChannel(conn);
    if (err)
    {
        return err;
    }
    conn->SetJavaPeer(aJniEnv->NewWeakGlobalRef(aJobj));
    conn->SetCallbacks(&FillData, &ConditionCallback);
    return reinterpret_cast< int >(conn);
}

/*
 * Class:     com_nokia_mj_impl_sensor_SensorConnectionImpl
 * Method:    _startDataListening
 * Signature: ([Ljavax/microedition/sensor/Data;IJZZ)I
 */

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_sensor_SensorConnectionImpl__1startDataListening
(JNIEnv* aJniEnv, jobject /*aPeer*/, jint aSensorConnection, jobjectArray aData,
 jint aBufferSize, jlong aBufferingPeriod, jboolean aIsTimestampsIncluded,
 jboolean aIsValidityIncluded, jboolean aIsOneShot, jint aDataType)
{
    JELOG2(ESensor);
    SensorConnection* conn = reinterpret_cast< SensorConnection* >(aSensorConnection);
    void* oldData = 0;
    void* dataObjects = aJniEnv->NewGlobalRef(aData);
    int err = conn->PrepareDataListening(dataObjects,
                                         aJniEnv->GetArrayLength(aData),
                                         oldData,
                                         aDataType);
    // Remove possible old data objects
    if (oldData)
    {
        aJniEnv->DeleteGlobalRef((jobject)oldData);
    }

    if (!err)
    {
        err = conn->StartDataListening(aBufferSize, aBufferingPeriod,
                                       aIsTimestampsIncluded, aIsValidityIncluded,
                                       aIsOneShot);
    }
    return err;
}

/*
 * Class:     com_nokia_mj_impl_sensor_SensorConnectionImpl
 * Method:    _cancelDataListening
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_sensor_SensorConnectionImpl__1cancelDataListening
(JNIEnv *, jobject, jint aSensorConnection)
{
    JELOG2(ESensor);
    SensorConnection* conn = reinterpret_cast< SensorConnection* >(aSensorConnection);
    Sensor* sensor = conn->GetSensor();
    return sensor->CancelDataListening();
}

/*
 * Class:     com_nokia_mj_impl_sensor_SensorConnectionImpl
 * Method:    _removeConnection
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_sensor_SensorConnectionImpl__1removeConnection
(JNIEnv* aJniEnv, jobject, jint aSensorList, jint aSensorConnection)
{
    JELOG2(ESensor);
    SensorConnectionList* senList = reinterpret_cast< SensorConnectionList* >(aSensorList);
    SensorConnection* conn = reinterpret_cast< SensorConnection* >(aSensorConnection);
    jobject dataObjects = (jobject)conn->DataObjects();
    if (dataObjects)
    {
        aJniEnv->DeleteGlobalRef(dataObjects);
    }
    aJniEnv->DeleteWeakGlobalRef((jweak)conn->GetJavaPeer());
    senList->RemoveConnection(conn);
}

/*
 * Class:     com_nokia_mj_impl_sensor_SensorConnectionImpl
 * Method:    _startConditionListening
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_sensor_SensorConnectionImpl__1startConditionListening
(JNIEnv *, jobject, jint aConnectionHandle , jint aListeningType)
{
    JELOG2(ESensor);
    SensorConnection* conn = reinterpret_cast< SensorConnection* >(aConnectionHandle);
    Sensor* sensor = conn->GetSensor();
    sensor->StartConditionListening(aListeningType);
}

/*
 * Class:     com_nokia_mj_impl_sensor_SensorConnectionImpl
 * Method:    _stopConditionListening
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_sensor_SensorConnectionImpl__1stopConditionListening
(JNIEnv *, jobject, jint aConnectionHandle)
{
    JELOG2(ESensor);
    SensorConnection* conn = reinterpret_cast< SensorConnection* >(aConnectionHandle);
    Sensor* sensor = conn->GetSensor();
    sensor->StopConditionListening();
}

/*
 * Class:     com_nokia_mj_impl_sensor_SensorConnectionImpl
 * Method:    _startServer
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_sensor_SensorConnectionImpl__1startServer
(JNIEnv* aJniEnv, jobject, jint aConnectionHandle)
{
    JELOG2(ESensor);
    SensorConnection* conn = reinterpret_cast< SensorConnection* >(aConnectionHandle);
    Sensor* sensor = conn->GetSensor();
    // Store JNI env from this thread
    conn->SetJni(aJniEnv);
    sensor->StartServer();
}

/*
 * Class:     com_nokia_mj_impl_sensor_SensorConnectionImpl
 * Method:    _startServer
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_sensor_SensorConnectionImpl__1stopServer
(JNIEnv *, jobject, jint aConnectionHandle)
{
    JELOG2(ESensor);
    SensorConnection* conn = reinterpret_cast< SensorConnection* >(aConnectionHandle);
    Sensor* sensor = conn->GetSensor();
    sensor->StopServer();
}
