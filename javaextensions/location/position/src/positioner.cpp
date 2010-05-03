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
* Description:  Positioner JNI context
 *
*/


#include <e32def.h>
#include <lbs.h>
#include "com_nokia_mj_impl_location_Positioner.h"
#include "clocationprovider.h"
#include "cpositioner.h"
#include "fs_methodcall.h"
#include "logger.h"

using namespace java::location;

/*
 * Class:     com_nokia_mj_impl_location_Positioner
 * Method:    _getLocation
 * Signature: (JII)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_location_Positioner__1getLocation(
    JNIEnv* /*aJni*/,
    jobject /*aPositioner*/,
    jint aFunctionServerHandle,
    jint aPositionerHandle,
    jint aTimeout)
{
    JELOG2(EJavaLocation);
    LocationFunctionServer *server =
        reinterpret_cast<LocationFunctionServer *>(aFunctionServerHandle);

    CPositioner* positioner = reinterpret_cast< CPositioner *>(aPositionerHandle);

    TInt error = KErrNone;

    TRAPD(err, CallMethod(error, positioner, &CPositioner::GetLocation, aTimeout, server));

    if (err < KErrNone)
    {
        error = err;
    }

    return error;
}

/*
 * Class:     com_nokia_mj_impl_location_Positioner
 * Method:    _createPositioner
 * Signature: (JI)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_location_Positioner__1createPositioner(
    JNIEnv* aJni,
    jobject aPositioner,
    jint aFunctionServerHandle,
    jint aLocationProviderHandle)
{
    JELOG2(EJavaLocation);

    LocationFunctionServer *server =
        reinterpret_cast<LocationFunctionServer *>(aFunctionServerHandle);

    CLocationProvider* locationProvider =
        reinterpret_cast<CLocationProvider *>(aLocationProviderHandle);

    TInt positionerHandle = KErrNotFound;
    jobject jpositioner = aJni->NewWeakGlobalRef(aPositioner);

    TRAPD(err, CLocationProvider::StaticCreatePositionerL(aJni, jpositioner, locationProvider, &positionerHandle, server));

    if (err != KErrNone)
    {
        aJni->DeleteGlobalRef(jpositioner);
        return err;
    }

    return positionerHandle;
}

/*
 * Class:     com_nokia_mj_impl_location_Positioner
 * Method:    _cancel
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_location_Positioner__1cancel
(JNIEnv*, jobject, jint aFunctionServerHandle, jint aPositionerHandle)
{
    JELOG2(EJavaLocation);

    LocationFunctionServer *server =
        reinterpret_cast<LocationFunctionServer *>(aFunctionServerHandle);

    CPositioner* positioner = reinterpret_cast< CPositioner *>(aPositionerHandle);

    CallMethod(positioner, &CPositioner::StaticCancel, positioner, server);
}

LOCAL_C void DisposeObject(CPositioner* aPositioner)
{
    delete aPositioner;
}

/*
 * Class:     com_nokia_mj_impl_location_Positioner
 * Method:    _dispose
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_location_Positioner__1dispose
(JNIEnv*, jobject, jint aFunctionServerHandle, jint aPositionerHandle)
{
    JELOG2(EJavaLocation);

    LocationFunctionServer *server =
        reinterpret_cast<LocationFunctionServer *>(aFunctionServerHandle);

    CPositioner* positioner = reinterpret_cast< CPositioner *>(aPositionerHandle);

    CallMethod(DisposeObject, positioner, server);

}
