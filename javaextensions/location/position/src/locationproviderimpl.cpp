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
* Description:  LocationProviderImpl JNI context
 *
*/


#include <e32def.h>
#include <lbs.h>
#include "com_nokia_mj_impl_location_LocationProviderImpl.h"
#include "clocationprovider.h"
#include "cpositionerbase.h"
#include "s60commonutils.h"
#include "ctrackingpositioner.h"
#include "fs_methodcall.h"
#include "logger.h"
#include "monitor.h"

#ifdef RD_JAVA_PROXIMITY_LISTENER_ENABLED
#include "cproximitylistener.h"
#endif

using namespace java::location;

/*
 * Class:     com_nokia_mj_impl_location_LocationProviderImpl
 * Method:    _createFunctionSource
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_location_LocationProviderImpl__1createFunctionSource
(JNIEnv* aJni, jobject aLocationProvider)
{
    JELOG2(EJavaLocation);

    LocationFunctionServer* mFunctionServer =
        new LocationFunctionServer(*aJni, aLocationProvider);
    return reinterpret_cast<jint>(mFunctionServer);
}

/*
 * Class:     com_nokia_mj_impl_location_LocationProviderImpl
 * Method:    _createProvider
 * Signature: (J)I
 */
JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_location_LocationProviderImpl__1createProvider
(JNIEnv*, jobject, jint aFunctionServerHandle)
{
    JELOG2(EJavaLocation);
    LocationFunctionServer *server =
        reinterpret_cast<LocationFunctionServer *>(aFunctionServerHandle);

    TInt handle(0);

    TRAPD(err, CLocationProvider::NewL(server, &handle));

    if (err != KErrNone)
    {
        return err;
    }

    return handle;
}

#ifdef RD_JAVA_PROXIMITY_LISTENER_ENABLED
/*
 * Class:     com_nokia_mj_impl_location_LocationProviderImpl
 * Method:    _createNativeProximityListener
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_location_LocationProviderImpl__1createNativeProximityListener
(JNIEnv * , jclass, jint aFunctionServerHandle)
{
    JELOG2(EJavaLocation);

    LocationFunctionServer *server =
        reinterpret_cast<LocationFunctionServer *>(aFunctionServerHandle);

    CProximityListener * self = KErrNone;

    TRAPD(error, self = CallMethodL(&CProximityListener::NewL, server, server));

    if (error < KErrNone)
    {
        return error;
    }

    return   reinterpret_cast<int>(self);
}

/*
 * Class:     com_nokia_mj_impl_location_LocationProviderImpl
 * Method:    _addProxmityListener
 * Signature: (IIDDFF)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_location_LocationProviderImpl__1addProxmityListener
(JNIEnv* , jclass, jint aProximityNativeHandle, jint aFunctionServerHandle,
 jdouble aLat, jdouble aLon, jfloat aAlt, jfloat aProximityRadius)
{
    JELOG2(EJavaLocation);

    LocationFunctionServer *server =
        reinterpret_cast<LocationFunctionServer *>(aFunctionServerHandle);

    CProximityListener *proximitylistener =
        reinterpret_cast<CProximityListener *>(aProximityNativeHandle);


    java::util::Monitor* proximityMonitor =
        java::util::Monitor::createMonitor();

    TInt error = KErrNone;
    TInt TrigID = KErrNone;

    TRAP(error, CallMethodL(proximitylistener,
                            &CProximityListener::IntializeTriggerL,
                            aLat, aLon, aAlt, aProximityRadius,
                            proximityMonitor, server));

    if (error < KErrNone)
    {
        return error;
    }

    // Wait for the trigger to get created.
    proximityMonitor->wait();

    TRAPD(err, CallMethodL(TrigID, proximitylistener,
                           &CProximityListener::StartTriggerL,
                           server));

    if (err < KErrNone)
    {
        return err;
    }

    return TrigID;
}

/*
 * Class:     com_nokia_mj_impl_location_LocationProviderImpl
 * Method:    _isMonitoringNotActive
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_com_nokia_mj_impl_location_LocationProviderImpl__1isMonitoringNotActive
(JNIEnv *, jclass, jint aProximityNativeHandle)
{
    CProximityListener *proximitylistener =
        reinterpret_cast<CProximityListener *>(aProximityNativeHandle);

    return proximitylistener->iIsStatusNotActive;
}

/*
 * Class:     com_nokia_mj_impl_location_LocationProviderImpl
 * Method:    _deleteTrigger
 * Signature: (III)V
 */
JNIEXPORT void JNICALL
Java_com_nokia_mj_impl_location_LocationProviderImpl__1deleteTrigger
(JNIEnv *, jclass, jint aFunctionServerHandle, jint aProximityNativeHandle,
 jint aTriggerID)
{
    JELOG2(EJavaLocation);

    LocationFunctionServer *server =
        reinterpret_cast<LocationFunctionServer *>(aFunctionServerHandle);

    CProximityListener *proximitylistener =
        reinterpret_cast<CProximityListener *>(aProximityNativeHandle);

    TRAPD(error, CallMethodL(proximitylistener,
                             &CProximityListener::DeleteTriggerL,
                             aTriggerID, server));

    if (error < KErrNone)
    {
        ELOG1(EJavaLocation,
              "LocationProviderImpl__1deleteTrigger error = %d", error);
    }
}

LOCAL_C void
DisposeProximityListenerObject(CProximityListener* aproximitylistener)
{
    delete aproximitylistener;
}

/*
 * Class:     com_nokia_mj_impl_location_LocationProviderImpl
 * Method:    _disposeNativeProximityListener
 * Signature: (IIZ)V
 */
JNIEXPORT void JNICALL
Java_com_nokia_mj_impl_location_LocationProviderImpl__1disposeNativeProximityListener
(JNIEnv *, jclass, jint aFunctionServerHandle, jint aProximityNativeHandle,
 jboolean aIsFuncSrvCont)
{
    JELOG2(EJavaLocation);

    LocationFunctionServer *server =
        reinterpret_cast<LocationFunctionServer *>(aFunctionServerHandle);

    CProximityListener *proximitylistener =
        reinterpret_cast<CProximityListener *>(aProximityNativeHandle);

    if (aIsFuncSrvCont)
    {
        delete proximitylistener;
    }
    else
    {
        CallMethod(DisposeProximityListenerObject, proximitylistener, server);
    }
}

#endif

/*
 * Class:     com_nokia_mj_impl_location_LocationProviderImpl
 * Method:    _selectModule
 * Signature: (JIIIIII[I)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_location_LocationProviderImpl__1selectModule
(JNIEnv* aJni, jobject, jint aFunctionServerHandle, jint aHandle, jint aHacc,
 jint aVacc, jint aRespTime, jint aPower, jint aRequired,
 jintArray aMethodTimeout)
{
    JELOG2(EJavaLocation);
    jint methodTimeout[2];

    LocationFunctionServer *server =
        reinterpret_cast<LocationFunctionServer *>(aFunctionServerHandle);

    CLocationProvider* locationProvider =
        reinterpret_cast<CLocationProvider *>(aHandle);

    TInt trackingPositionerHandle(0);

    TRAPD(err, CallMethodL(locationProvider, &CLocationProvider::SelectModuleL,
                           aHacc, aVacc, aRespTime, aPower, aRequired,
                           methodTimeout, server));

    trackingPositionerHandle =
        reinterpret_cast<TInt>(locationProvider->iTrackingPositioner);

    if (err != KErrNone)
    {
        return err;
    }

    aJni->SetIntArrayRegion(aMethodTimeout, 0, 2, methodTimeout);

    return trackingPositionerHandle;
}

LOCAL_C void DisposeObject(CLocationProvider* alocationProvider)
{
    delete alocationProvider;
}

JNIEXPORT void JNICALL
Java_com_nokia_mj_impl_location_LocationProviderImpl__1dispose
(JNIEnv*, jobject, jint aFunctionServerHandle, jint aHandle)
{
    JELOG2(EJavaLocation);

    LocationFunctionServer *server =
        reinterpret_cast<LocationFunctionServer *>(aFunctionServerHandle);

    CLocationProvider* locationProvider =
        reinterpret_cast<CLocationProvider *>(aHandle);

    CallMethod(DisposeObject, locationProvider, server);

    delete server;
}

/*
 * Class:     com_nokia_mj_impl_location_LocationProviderImpl
 * Method:    _getQualifiedCoordinates
 * Signature: (JI[D[F[Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL
Java_com_nokia_mj_impl_location_LocationProviderImpl__1getQualifiedCoordinates
(JNIEnv* aJni, jobject, jint aPositionerHandle, jdoubleArray aLatLon,
 jfloatArray aAltHaccVacc, jobjectArray aNmeaData)
{
    JELOG2(EJavaLocation);
    jdouble latlon[2];
    jfloat altHaccVacc[3];
    jlong timestamp;

    CPositionerBase* positioner =
        reinterpret_cast<CPositionerBase *>(aPositionerHandle);;

    CPositionerBase::GetQualifiedCoordinates(positioner, aJni, latlon,
            altHaccVacc, &timestamp, aNmeaData);

    aJni->SetDoubleArrayRegion(aLatLon, 0, 2, latlon);
    aJni->SetFloatArrayRegion(aAltHaccVacc, 0, 3, altHaccVacc);

    return timestamp;
}

/*
 * Class:     com_nokia_mj_impl_location_LocationProviderImpl
 * Method:    _getSpeedAndCourse
 * Signature: (JI[F)Z
 */
JNIEXPORT jboolean JNICALL
Java_com_nokia_mj_impl_location_LocationProviderImpl__1getSpeedAndCourse
(JNIEnv* aJni, jobject, jint aPositionerHandle, jfloatArray aSpeedCourse)
{
    JELOG2(EJavaLocation);
    jfloat speedCourse[2];

    CPositionerBase* positioner =
        reinterpret_cast<CPositionerBase *>(aPositionerHandle);

    TInt err = CPositionerBase::GetSpeedAndCourse(positioner, speedCourse);

    if (err != KErrNone)
    {
        return JNI_FALSE;
    }

    aJni->SetFloatArrayRegion(aSpeedCourse, 0, 2, speedCourse);

    return JNI_TRUE;
}

/*
 * Class:     com_nokia_mj_impl_location_LocationProviderImpl
 * Method:    _startTracking
 * Signature: (JIIII)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_location_LocationProviderImpl__1startTracking
(JNIEnv*, jobject, jint aFunctionServerHandle, jint aHandle, jint aInterval,
 jint aTimeout, jint aMaxAge)
{
    JELOG2(EJavaLocation);
    LocationFunctionServer *server =
        reinterpret_cast<LocationFunctionServer *>(aFunctionServerHandle);

    CLocationProvider* locationProvider =
        reinterpret_cast<CLocationProvider *>(aHandle);

    TInt err = KErrNone;

    CallMethod(err, locationProvider->iTrackingPositioner,
               &CTrackingPositioner::StartTracking,
               aInterval,
               aTimeout,
               aMaxAge,
               server);

    return err;
}

/*
 * Class:     com_nokia_mj_impl_location_LocationProviderImpl
 * Method:    _stopTracking
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL
Java_com_nokia_mj_impl_location_LocationProviderImpl__1stopTracking
(JNIEnv*, jobject, jint aFunctionServerHandle, jint aHandle)
{
    JELOG2(EJavaLocation);
    LocationFunctionServer *server =
        reinterpret_cast<LocationFunctionServer *>(aFunctionServerHandle);

    CLocationProvider* locationProvider =
        reinterpret_cast<CLocationProvider *>(aHandle);

    if (locationProvider->iTrackingPositioner)
    {
        CallMethod(locationProvider->iTrackingPositioner,
                   &CTrackingPositioner::StopTracking, server);
    }
}

/*
 * Class:     com_nokia_mj_impl_location_LocationProviderImpl
 * Method:    _getAddressInfo
 * Signature: (JI[Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_location_LocationProviderImpl__1getAddressInfo
(JNIEnv* aJni, jobject, jint aPositionerHandle, jobjectArray aAddress)
{
    JELOG2(EJavaLocation);

    CPositionerBase* positioner =
        reinterpret_cast<CPositionerBase *>(aPositionerHandle);

    TInt availableFields = CPositionerBase::GetAddressInfo(positioner,
                           aJni, aAddress);

    return availableFields;
}

/*
 * Class:     com_nokia_mj_impl_location_LocationProviderImpl
 * Method:    _getLastKnownLocation
 * Signature: ([D[F[J)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_location_LocationProviderImpl__1getLastKnownLocation
(JNIEnv* aJni, jclass, jdoubleArray aLatLon, jfloatArray aAltHaccVacc,
 jlongArray aTimestamp)
{
    JELOG2(EJavaLocation);
    jdouble latlon[2];
    jfloat altHaccVacc[3];
    jlong timestamp[1];

    RPositionServer server;
    TInt err = server.Connect();
    if (err != KErrNone)
    {
        return err;
    }

    RPositioner positioner;
    err = positioner.Open(server);
    if (err != KErrNone)
    {
        server.Close();
        return err;
    }

    _LIT(KService, "JavaVM");
    positioner.SetRequestor(CRequestor::ERequestorService,
                            CRequestor::EFormatApplication,
                            KService);

    TPositionInfo posInfo;
    TRequestStatus status;
    positioner.GetLastKnownPosition(posInfo, status);
    User::WaitForRequest(status);
    positioner.Close();
    server.Close();

    err = status.Int();

    if (err == KErrNone)
    {
        TPosition pos;
        posInfo.GetPosition(pos);

        latlon[0] = static_cast<jdouble>(pos.Latitude());
        latlon[1] = static_cast<jdouble>(pos.Longitude());
        altHaccVacc[0] = static_cast<jfloat>(pos.Altitude());
        altHaccVacc[1] = static_cast<jfloat>(pos.HorizontalAccuracy());
        altHaccVacc[2] = static_cast<jfloat>(pos.VerticalAccuracy());

        timestamp[0] = java::util::S60CommonUtils::TTimeToJavaTime(pos.Time());

        aJni->SetDoubleArrayRegion(aLatLon, 0, 2, latlon);
        aJni->SetFloatArrayRegion(aAltHaccVacc, 0, 3, altHaccVacc);

        aJni->SetLongArrayRegion(aTimestamp, 0, 1, timestamp);
    }

    return err;
}

