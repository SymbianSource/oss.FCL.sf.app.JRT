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
* Description:  Common JNI implementations
 *
*/


#ifndef LAPIJNICOMMON_H
#define LAPIJNICOMMON_H
#include <jni.h>
#include <e32cmn.h>
#include <e32base.h>
#include <badesca.h>
// EXTERNAL INCLUDES

// FORWARD DECLARATIONS
class CLAPILandmark;
class TLocality;

/**
 * Sets an error code to an error array
 * @param aJniEnv A reference to the JNI environment
 * @param aTo The array to which the error is to be added
 * @param aFrom The value which will be added to the array
 */
void SetJavaErrorCode(JNIEnv& aJniEnv, jintArray aTo, TInt aFrom);

/**
 * Creates a position from the passed coordinate information
 * @param aJniEnv A reference to the JNI environment
 * @param aCoordinates Latitude and longitude values
 * @param aCoordinateInfo Altitude, horizontal accuracy and vertical accurazy
 */
TLocality CreatePosition(JNIEnv& aJniEnv, jdoubleArray aCoordinates,
                         jfloatArray aCoordinateInfo);

/**
 * Creates a position information from the passed position
 * @param aJniEnv A reference to the JNI environment
 * @param aCoordinates Latitude and longitude values
 * @param aCoordinateInfo Altitude, horizontal accuracy and vertical accurazy
 */
void CreateCoordinateInfos(JNIEnv& aJniEnv, const TLocality& aPosition,
                           jdoubleArray aCoordinates, jfloatArray aCoordinateInfo);

/**
 * Creates a new native string array from java string array
 * @param aJniEnv A reference to the JNI environment
 * @param aJavaArray The Java string array from which the new array is created
 */
RPointerArray<HBufC> CreateNativeStringArrayL(JNIEnv& aJniEnv,
        jobjectArray aJavaArray);

/**
 * Creates a new java string array from a native string array
 * @param aJniEnv A reference to the JNI environment
 * @param aNativeArray The native string array from which the new array is created
 */
jobjectArray CreateJavaStringArray(JNIEnv& aJniEnv,
                                   RPointerArray<HBufC>& aNativeArray);

/**
 * Creates an item handle array from the paased landmarks
 * @param aJniEnv A pointer to the JNI environment
 * @param aLandmarks The landmarks from which the handles are created
 * @return Java integer array. Ownership is transferred to the caller
 */
jintArray CreateHandleArray(JNIEnv& aJniEnv,
                            const CArrayPtr<CLAPILandmark>& aLandmarks);

jobjectArray CopyToNewJavaStringArrayL(JNIEnv& aJni,
                                       const CDesCArray& aNativeArray);

#endif // LAPIJNICOMMON_H
// End of file
