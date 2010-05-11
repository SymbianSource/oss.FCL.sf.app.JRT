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


// INTERNAL INCLUDES
#include "lapijnicommon.h"
#include "clapilandmark.h"
#include "javasymbianoslayer.h"
#include "lapipanics.h"
#include "jstringutils.h"
#include "s60commonutils.h"
#include "logger.h"

// EXTERNAL INCLUDES
#include <lbsposition.h>

// UNNAMED LOCAL NAMESPACE
namespace
{
// Size of the coordinates array (latitude and longitude)
const TInt KLAPINumCoordinates = 2;
// Size of the coordinate information array (altitude, horizontal
// accuracy and vertical accuracy
const TInt KLAPINumCoordinateInfos = 3;
}

/**
 * Sets an error code to an error array
 * @param aJniEnv Pointer to the JNI environment
 * @param aTo The array to which the error is to be added
 * @param aFrom The value which will be added to the array
 */
void SetJavaErrorCode(JNIEnv& aJniEnv, jintArray aTo, TInt aFrom)
{
    JELOG2(EJavaLocation);
    jint javaError[1] =
        { aFrom };
    aJniEnv.SetIntArrayRegion(aTo, 0, 1, javaError);
}

/**
 * Creates a position from the passed coordinate information
 * @param aJniEnv A reference to the JNI environment
 * @param aCoordinates Latitude and longitude values
 * @param aCoordinateInfo Altitude, horizontal accuracy and vertical accurazy
 */
TLocality CreatePosition(JNIEnv& aJniEnv, jdoubleArray aCoordinates,
                         jfloatArray aCoordinateInfo)
{
    JELOG2(EJavaLocation);
    __ASSERT_DEBUG(aCoordinates && aCoordinateInfo, LAPIError::Panic(
                       ELAPIPanicNullArgument));

    jdouble dArray[KLAPINumCoordinates];
    jfloat fArray[KLAPINumCoordinateInfos];

    // Get latitude and longitude
    aJniEnv.GetDoubleArrayRegion(aCoordinates, 0, KLAPINumCoordinates, dArray);
    // Get altitude, horizontal accuracy and vertical accuracy
    aJniEnv.GetFloatArrayRegion(aCoordinateInfo, 0, KLAPINumCoordinateInfos,
                                fArray);

    TCoordinate coordinates(dArray[0], dArray[1], fArray[0]);
    TLocality position(coordinates, fArray[1], fArray[2]);
    return position;
}

/**
 * Creates a position information from the passed position
 * @param aJniEnv A reference to the JNI environment
 * @param aCoordinates Latitude and longitude values
 * @param aCoordinateInfo Altitude, horizontal accuracy and vertical accurazy
 */
void CreateCoordinateInfos(JNIEnv& aJniEnv, const TLocality& aPosition,
                           jdoubleArray aCoordinates, jfloatArray aCoordinateInfo)
{
    JELOG2(EJavaLocation);
    __ASSERT_DEBUG(
        (aJniEnv.GetArrayLength(aCoordinates) >= KLAPINumCoordinates),
        LAPIError::Panic(ELAPIPanicJniIncorrectArrayLength));
    __ASSERT_DEBUG((aJniEnv.GetArrayLength(aCoordinateInfo)
                    >= KLAPINumCoordinateInfos), LAPIError::Panic(
                       ELAPIPanicJniIncorrectArrayLength));

    jdouble coordinates[KLAPINumCoordinates] =
        { aPosition.Latitude(), aPosition.Longitude() };

    jfloat coordinateInfo[KLAPINumCoordinateInfos] =
        { aPosition.Altitude(), aPosition.HorizontalAccuracy(),
          aPosition.VerticalAccuracy()
        };

    // Set coordinate values to the java array
    aJniEnv.SetDoubleArrayRegion(aCoordinates, 0, KLAPINumCoordinates,
                                 coordinates);
    // Set coordinate information to the java array
    aJniEnv.SetFloatArrayRegion(aCoordinateInfo, 0, KLAPINumCoordinateInfos,
                                coordinateInfo);
}

/**
 * Creates a new native string array from java string array
 * @param aJniEnv A reference to the JNI environment
 * @param aJavaArray The Java string array from which the new array is created
 */
RPointerArray<HBufC> CreateNativeStringArrayL(JNIEnv& aJniEnv,
        jobjectArray aJavaArray)
{
    JELOG2(EJavaLocation);
    TInt count = 0;
    if (aJavaArray)
    {
        count = aJniEnv.GetArrayLength(aJavaArray);
    }
    // Create pointer array holding HBufCs so that null java elements
    // can be represented as NULL pointers in the array
    RPointerArray<HBufC> nativeArray(10);
    CleanupResetAndDestroyPushL(nativeArray);

    for (TInt i = 0; i < count; i++)
    {
        // Creates a new local reference
        jobject joe = aJniEnv.GetObjectArrayElement(aJavaArray, i);
        jstring javaString = static_cast<jstring>(joe);
        if (!javaString)
        {
            nativeArray.AppendL(NULL);
        }
        else
        {
            // The value returned by GetObjectArrayElement is not really
            // a pointer to the object, but it is a "local reference".
            // In practice, it is a pointer to a pointer which points to the
            // object. If the object is null, we get a pointer which points
            // to a pointer which points to null.
            TAny** objPointer = reinterpret_cast<TAny**>(joe);
            TAny* obj = *objPointer;
            if (!obj)
            {
                nativeArray.AppendL(NULL);
            }
            else
            {
                JStringUtils nativeElem(aJniEnv, javaString);
                HBufC* element = nativeElem.AllocLC();
                nativeArray.AppendL(element);
                CleanupStack::Pop(element);
            }
        }
        // The VM runs out of temporary local references, unless the
        // elements are not released here
        aJniEnv.DeleteLocalRef(joe);
    }

    CleanupStack::Pop(&nativeArray);
    return nativeArray;
}

/**
 * Creates a new java string array from a native string array
 * @param aJniEnv A reference to the JNI environment
 * @param aNativeArray The native string array from which the new array is created
 */
jobjectArray CreateJavaStringArray(JNIEnv& aJniEnv,
                                   RPointerArray<HBufC>& aNativeArray)
{
    JELOG2(EJavaLocation);
    return java::util::S60CommonUtils::NativeToJavaStringArray(aJniEnv,
            aNativeArray);
}

jobjectArray CopyToNewJavaStringArrayL(JNIEnv& aJni,
                                       const CDesCArray& aNativeArray)
{
    JELOG2(EJavaLocation);
    jclass stringClass = aJni.FindClass("java/lang/String");
    User::LeaveIfNull(stringClass);
    //
    jobjectArray result = aJni.NewObjectArray(aNativeArray.Count(),
                          stringClass, NULL);
    if (result != NULL)
    {
        TPtrC item;
        for (int i = 0; i < aNativeArray.Count(); ++i)
        {
            item.Set(aNativeArray[i]);
            jstring javaString =
                java::util::S60CommonUtils::NativeToJavaString(aJni, item);
            User::LeaveIfNull(javaString);
            //
            aJni.SetObjectArrayElement(result, i, javaString);
            aJni.DeleteLocalRef(javaString);
        }
    }
    return result;
}

/**
 * Creates an item handle array from the paased landmarks
 * @param aJniEnv A reference to the JNI environment
 * @param aLandmarks The landmarks from which the handles are created
 * @return Java integer array. Ownership is transferred to the caller
 */
jintArray CreateHandleArray(JNIEnv& aJniEnv,
                            const CArrayPtr<CLAPILandmark>& aLandmarks)
{
    JELOG2(EJavaLocation);
    TInt count = aLandmarks.Count();
    jintArray handles = aJniEnv.NewIntArray(count);

    if (handles && count > 0)
    {
        jint* elems = aJniEnv.GetIntArrayElements(handles, NULL);
        if (!elems)
        {
            return NULL; // Error
        }
        // Make handles from the native side peer objects
        for (TInt i = 0; i < count; i++)
        {
            elems[i] = reinterpret_cast<jint>(aLandmarks[i]);
        }
        // Elemenet must be released since it is local
        aJniEnv.ReleaseIntArrayElements(handles, elems, 0);
    }

    return handles;
}

// End of file
