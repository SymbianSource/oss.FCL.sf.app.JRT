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
* Description:  Orientation JNI context
 *
*/


// INTERNAL INCLUDES
#include "javax_microedition_location_Orientation.h"
#include "corientation.h"
#include "sensrvmagneticnorthsensor.h"
#include "locationfunctionserver.h"
#include "fs_methodcall.h"

// EXTERNAL INCLUDES
#include <e32def.h>

using namespace java::location;

/*
 * Class:     javax_microedition_location_Orientation
 * Method:    _createNativeHandle
 * Signature: (I)I
 */
JNIEXPORT jint
JNICALL Java_javax_microedition_location_Orientation__1createNativeHandle(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint /*aFunctionServerHandle*/
)
{
    TInt handle(0);
    TRAPD(err, COrientation::NewL(&handle));

    if (err < KErrNone)
    {
        handle = err;
    }

    return handle;
}

/*
 * Class:     javax_microedition_location_Orientation
 * Method:    _getAzimuthData
 * Signature: (JI)I
 */
JNIEXPORT jint
JNICALL Java_javax_microedition_location_Orientation__1getAzimuthData(
    JNIEnv* /*aJniEnv*/,
    jclass,
    jint aFunctionServerHandle,
    jint aHandle)
{
    LocationFunctionServer *server =
        reinterpret_cast<LocationFunctionServer *>(aFunctionServerHandle);

    // Native Handle
    COrientation* Orientation = reinterpret_cast< COrientation *>(aHandle);

    Orientation->ichannelInfo.iChannelType =
        KSensrvChannelTypeIdMagneticNorthData;

    Orientation->mFunctionServer = server;
    TInt error = KErrNone;
    TRAPD(err,CallMethodL(error, Orientation,&COrientation::GetOrientationL,server));

    if (error < KErrNone)
    {
        err = error;
    }

    return err;
}

/*
 * Class:     javax_microedition_location_Orientation
 * Method:    _getData
 * Signature: (I)F
 */
JNIEXPORT jfloat JNICALL Java_javax_microedition_location_Orientation__1getData(
    JNIEnv*,
    jclass,
    jint aHandle)
{
    // Native Handle
    COrientation* iOrientation = reinterpret_cast< COrientation *>(aHandle);

    jfloat azimuth = static_cast<jfloat>(iOrientation->mAzimuth);

    return azimuth;
}

LOCAL_C void DisposeOrientationObject(COrientation* aOrientation)
{
    delete aOrientation;
}

/*
 * Class:     javax_microedition_location_Orientation
 * Method:    _dispose
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_javax_microedition_location_Orientation__1dispose(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aFunctionServerHandle,
    jint aHandle)
{
    // Native Handle
    COrientation* iOrientation = reinterpret_cast< COrientation *>(aHandle);

    // Function Server
    LocationFunctionServer *server =
        reinterpret_cast<LocationFunctionServer *>(aFunctionServerHandle);

    CallMethod(DisposeOrientationObject, iOrientation, server);
}
//  End of File
