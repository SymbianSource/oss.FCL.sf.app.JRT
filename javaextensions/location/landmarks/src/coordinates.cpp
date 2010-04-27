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
* Description:  JNI implementation of Coordinates class
 *
*/


// INTERNAL INCLUDES
#include "javax_microedition_location_Coordinates.h"
#include <lbsposition.h>
#include "logger.h"

LOCAL_C void CalculateBearingToL(TCoordinate* aSrc, TCoordinate* aDest,
                                 TReal32* aBearing)
{
    TReal32 bearing;
    User::LeaveIfError(aSrc->BearingTo(*aDest, bearing));
    *aBearing = bearing;
}

/*
 * Class:     javax_microedition_location_Coordinates
 * Method:    _azimuthTo
 * Signature: (IDDDD)F
 */
JNIEXPORT jfloat JNICALL Java_javax_microedition_location_Coordinates__1azimuthTo(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jdouble aSrcLatitude,
    jdouble aSrcLongitude,
    jdouble aDestLatitude,
    jdouble aDestLongitude)
{
    JELOG2(EJavaLocation);

    TReal32 azimuth = 0.0f;

    TCoordinate src(aSrcLatitude, aSrcLongitude);
    TCoordinate dest(aDestLatitude, aDestLongitude);

    TRAPD(error, CalculateBearingToL(
              &src,
              &dest,
              &azimuth));

    if (error != KErrNone)
    {
        ELOG1(EJavaLocation,
              "Java_javax_microedition_location_Coordinates__1azimuthTo - error %d", error);
    }

    return azimuth;
}

LOCAL_C void CalculateDistanceL(TCoordinate* aSrc, TCoordinate* aDest,
                                TReal32* aDistance)
{
    TReal32 distance;
    User::LeaveIfError(aSrc->Distance(*aDest, distance));
    *aDistance = distance;
}

/*
 * Class:     javax_microedition_location_Coordinates
 * Method:    _distance
 * Signature: (IDDDD)F
 */
JNIEXPORT jfloat JNICALL Java_javax_microedition_location_Coordinates__1distance(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jdouble aSrcLatitude,
    jdouble aSrcLongitude,
    jdouble aDestLatitude,
    jdouble aDestLongitude)
{
    JELOG2(EJavaLocation);

    TReal32 distance = -1.0f;

    TCoordinate src(aSrcLatitude, aSrcLongitude);
    TCoordinate dest(aDestLatitude, aDestLongitude);

    //   TInt error = src->Distance( *dest,distance);
    TRAPD(error, CalculateDistanceL(
              &src,
              &dest,
              &distance));

    if (error != KErrNone)
    {
        ELOG1(EJavaLocation,
              "Java_javax_microedition_location_Coordinates__1distance - error %d", error);
    }

    return distance;
}

// End of file
