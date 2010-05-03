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
* Description:  JNI implementation of Landmark class
 *
*/


// INTERNAL INCLUDES
#include "javax_microedition_location_Landmark.h"
#include "jstringutils.h"
#include "s60commonutils.h"
#include "lapijnicommon.h"
#include "logger.h"
#include "clapilandmark.h"
#include "clapiaddressinfo.h"
#include "cleanupresetanddestroy.h"
#include "locationfunctionserver.h"

using namespace java::location;

LOCAL_C void CreateLandmarkL(TInt* aHandle)
{
    CLAPILandmark::TCtorParams params;
    params.iLandmark = NULL;
    params.iLandmarkStore = NULL;

    CLAPILandmark* landmark = CLAPILandmark::NewLC(params);
    // Add the created object to event source
    TInt handle = reinterpret_cast<TInt>(landmark);
    CleanupStack::Pop(landmark);
    *aHandle = handle;
}

/*
 * Class:     javax_microedition_location_Landmark
 * Method:    _createNativePeer
 * Signature: (I)I
 */
JNIEXPORT jint
JNICALL Java_javax_microedition_location_Landmark__1createNativePeer(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aEventSourceHandle)
{
    JELOG2(EJavaLocation);

    LocationFunctionServer* eventSource =
        reinterpret_cast< LocationFunctionServer*>(aEventSourceHandle);

    TInt handle = KErrGeneral;

    TInt error = eventSource->ExecuteTrap(
                     CreateLandmarkL,
                     &handle);

    // Return the handle if it was successfully obtained

    return error == KErrNone ? handle : error;

}

LOCAL_C void LandmarkNameL(JNIEnv* aJniEnv, CLAPILandmark* aLandmark,
                           jstring* aName)
{

    const TDesC& name = aLandmark->NameL();
    *aName = java::util::S60CommonUtils::NativeToJavaString(*aJniEnv, name);
}

/*
 * Class:     javax_microedition_location_Landmark
 * Method:    _getName
 * Signature: (II)Ljava/lang/String;
 */
JNIEXPORT jstring
JNICALL Java_javax_microedition_location_Landmark__1getName(
    JNIEnv* , jobject /*aPeer*/, jint aEventSourceHandle, jint aLandmarkHandle)
{
    JELOG2(EJavaLocation);

    LocationFunctionServer* eventSource =
        reinterpret_cast< LocationFunctionServer*>(aEventSourceHandle);
    CLAPILandmark* landmark =
        reinterpret_cast< CLAPILandmark*>(aLandmarkHandle);

    jstring name(NULL);

    TInt error = eventSource->ExecuteTrap(
                     LandmarkNameL,
                     eventSource->getValidJniEnv(),
                     landmark,
                     &name);

    // Null indicates an error. Landmark should always have a name
    return name;
}

LOCAL_C void LandmarkDescriptionL(CLAPILandmark* aLandmark,
                                  const TDesC** aDescription)
{
    *aDescription = aLandmark->DescriptionL();
}

/*
 * Class:     javax_microedition_location_Landmark
 * Method:    _getDescription
 * Signature: (II)Ljava/lang/String;
 */
JNIEXPORT jstring
JNICALL Java_javax_microedition_location_Landmark__1getDescription(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aEventSourceHandle,
    jint aLandmarkHandle)
{
    JELOG2(EJavaLocation);

    LocationFunctionServer* eventSource =
        reinterpret_cast< LocationFunctionServer*>(aEventSourceHandle);
    CLAPILandmark* landmark =
        reinterpret_cast< CLAPILandmark*>(aLandmarkHandle);

    const TDesC* description(NULL);

    TInt error = eventSource->ExecuteTrap(
                     LandmarkDescriptionL,
                     landmark,
                     &description);

    jstring javaDescription(NULL);
    // Zero length descriptions are not allowed
    if (description && description->Length()> 0)
    {
        javaDescription = java::util::S60CommonUtils::NativeToJavaString(*aJniEnv, *description);
    }

    // Null indicates an error or that the landmark didn't have a description
    return javaDescription;
}

LOCAL_C void LandmarkCoordinatesL(CLAPILandmark* aLandmark,
                                  const TLocality** aPosition)
{
    *aPosition = aLandmark->CoordinatesL();
}

/*
 * Class:     javax_microedition_location_Landmark
 * Method:    _getCoordinates
 * Signature: (II[D[F)I
 */
JNIEXPORT jint
JNICALL Java_javax_microedition_location_Landmark__1getCoordinates(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aEventSourceHandle,
    jint aLandmarkHandle,
    jdoubleArray aCoordinates,
    jfloatArray aCoordinateInfo)
{
    JELOG2(EJavaLocation);

    LocationFunctionServer* eventSource =
        reinterpret_cast< LocationFunctionServer*>(aEventSourceHandle);
    CLAPILandmark* landmark =
        reinterpret_cast< CLAPILandmark*>(aLandmarkHandle);

    const TLocality* position(NULL);

    TInt error = eventSource->ExecuteTrap(
                     LandmarkCoordinatesL,
                     landmark,
                     &position);

    LOG1(EJavaLocation,EInfo, "Java_javax_microedition_location_Landmark__1getCoordinates - error %d", error);

    if (error == KErrNone)
    {
        if (position)
        {
            // The position was successfully received from the landmark. Create
            // position data arrays from the native landmark position
            CreateCoordinateInfos(*aJniEnv, *position, aCoordinates, aCoordinateInfo);
        }
        else
        {
            // Position has not been set to the landmark.
            error = KErrNotFound;
        }
    }

    return error;
}

LOCAL_C void AddressInfosL(JNIEnv* aJniEnv, CLAPILandmark* aLandmark,
                           jobjectArray* aJavaStringArray)
{

    // Read all address information from the landmark
    const CLAPIAddressInfo* info = aLandmark->AddressInfoL();
    if (info && info->ValueCount() > 0)
    {
        RPointerArray<HBufC> values(ELAPINumAddressInfos);
        CleanupResetAndDestroyPushL(values);

        for (TInt i = 1; i <= ELAPINumAddressInfos; i++)
        {
            const TDesC* value = info->Value(i);
            // Null elements indicate an empty address info field
            HBufC* newValue = value ? value->AllocLC() : NULL;
            values.AppendL(newValue);
            // Pop the value if it was added to the cleanup stack
            if (newValue)
            {
                CleanupStack::Pop(newValue);
            }
        }

        // Create a java string array from the address info array
        *aJavaStringArray = CreateJavaStringArray(*aJniEnv, values);
        CleanupStack::PopAndDestroy(&values);
    }
}

/*
 * Class:     javax_microedition_location_Landmark
 * Method:    _getAddressInfo
 * Signature: (II)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray
JNICALL Java_javax_microedition_location_Landmark__1getAddressInfo(
    JNIEnv* ,
    jobject /*aPeer*/,
    jint aEventSourceHandle,
    jint aLandmarkHandle)
{
    JELOG2(EJavaLocation);

    LocationFunctionServer* eventSource =
        reinterpret_cast< LocationFunctionServer*>(aEventSourceHandle);
    CLAPILandmark* landmark =
        reinterpret_cast< CLAPILandmark*>(aLandmarkHandle);

    // Ownership of the name array is transferred to Java side
    jobjectArray javaStringArray(NULL);

    TInt error = eventSource->ExecuteTrap(
                     AddressInfosL,
                     eventSource->getValidJniEnv(),
                     landmark,
                     &javaStringArray);

    return javaStringArray; // NULL indicates an error
}

LOCAL_C void SetNameL(CLAPILandmark* aLandmark, const TDesC* aName)
{
    aLandmark->SetNameL(*aName);
}

/*
 * Class:     javax_microedition_location_Landmark
 * Method:    _setName
 * Signature: (IILjava/lang/String;)I
 */
JNIEXPORT jint
JNICALL Java_javax_microedition_location_Landmark__1setName(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aEventSourceHandle,
    jint aLandmarkHandle,
    jstring aName)
{
    JELOG2(EJavaLocation);
    LocationFunctionServer* eventSource =
        reinterpret_cast< LocationFunctionServer*>(aEventSourceHandle);
    CLAPILandmark* landmark =
        reinterpret_cast< CLAPILandmark*>(aLandmarkHandle);

    // Convert the description to a native descriptor. NULL is ok
    const JStringUtils name(*aJniEnv, aName);
    const TDesC* nativeName = (aName ? &name : NULL);

    TInt error = eventSource->ExecuteTrap(
                     SetNameL,
                     landmark,
                     nativeName);
    return error;
}

LOCAL_C void SetDescriptionL(CLAPILandmark* aLandmark,
                             const TDesC* aDescription)
{
    aLandmark->SetDescriptionL(aDescription);
}

/*
 * Class:     javax_microedition_location_Landmark
 * Method:    _setDescription
 * Signature: (IILjava/lang/String;)I
 */
JNIEXPORT jint
JNICALL Java_javax_microedition_location_Landmark__1setDescription(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aEventSourceHandle,
    jint aLandmarkHandle,
    jstring aDescription)
{
    JELOG2(EJavaLocation);

    LocationFunctionServer* eventSource =
        reinterpret_cast< LocationFunctionServer*>(aEventSourceHandle);
    CLAPILandmark* landmark =
        reinterpret_cast< CLAPILandmark*>(aLandmarkHandle);

    // Convert the description to a native descriptor. NULL is ok
    const JStringUtils description(*aJniEnv, aDescription);
    const TDesC* nativeDesc = (aDescription ? &description : NULL);

    TInt error = eventSource->ExecuteTrap(
                     SetDescriptionL,
                     landmark,
                     nativeDesc);

    LOG1(EJavaLocation,EInfo, "Java_javax_microedition_location_Landmark__1setDescription - error %d", error);
    return error;
}

LOCAL_C void SetCoordinatesL(CLAPILandmark* aLandmark, TLocality* aCoordinates)
{
    aLandmark->SetCoordinatesL(aCoordinates);
}

/*
 * Class:     javax_microedition_location_Landmark
 * Method:    _setCoordinates
 * Signature: (II[D[F)I
 */
JNIEXPORT jint
JNICALL Java_javax_microedition_location_Landmark__1setCoordinates(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aEventSourceHandle,
    jint aLandmarkHandle,
    jdoubleArray aCoordinates,
    jfloatArray aCoordinateInfo)
{
    JELOG2(EJavaLocation);

    LocationFunctionServer* eventSource =
        reinterpret_cast< LocationFunctionServer*>(aEventSourceHandle);
    CLAPILandmark* landmark =
        reinterpret_cast< CLAPILandmark*>(aLandmarkHandle);

    TLocality position;
    if (aCoordinates && aCoordinateInfo)
    {
        position = CreatePosition(*aJniEnv,
                                  aCoordinates,
                                  aCoordinateInfo);
    }

    TInt error = eventSource->ExecuteTrap(
                     SetCoordinatesL,
                     landmark,
                     aCoordinates && aCoordinateInfo ? &position : NULL);
    return error;
}

LOCAL_C void SetAddressInfoL(JNIEnv* aJniEnv, CLAPILandmark* aLandmark,
                             jobjectArray* aAddressInfo)
{
    RPointerArray<HBufC> addressInfos = CreateNativeStringArrayL(*aJniEnv,
                                        *aAddressInfo);
    CleanupResetAndDestroyPushL(addressInfos);
    TInt count = addressInfos.Count();
    if (count > 0)
    {
        // Currently, the Java side address info object is handled so that
        // if the value is completely changed, all fields will be reset
        // So if the address info is set from the Java-side, it is expected
        // that it will completely replace the existing one. Works correctly if
        // the existing address info is modified or replaced with a new one
        CLAPIAddressInfo* info = CLAPIAddressInfo::NewLC();
        // Copy the address information from the array
        for (TInt i = 0; i < count; i++)
        {
            // The ownership of the value is NOT transferred to "info"
            info->SetValueL(i + 1, addressInfos[i]);
        }
        aLandmark->SetAddressInfoL(info);
        // The landmark takes the ownership of the address information object
        CleanupStack::Pop(info);
    }
    else
    {
        // Clear the address information from the landmark
        aLandmark->SetAddressInfoL(NULL);
    }
    // The array is not needed anymore
    CleanupStack::PopAndDestroy(&addressInfos);
}

/*
 * Class:     javax_microedition_location_Landmark
 * Method:    _setAddressInfo
 * Signature: (II[Ljava/lang/String;)I
 */
JNIEXPORT jint
JNICALL Java_javax_microedition_location_Landmark__1setAddressInfo(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aEventSourceHandle,
    jint aLandmarkHandle,
    jobjectArray aAddressInfo)
{
    JELOG2(EJavaLocation);

    LocationFunctionServer* eventSource =
        reinterpret_cast< LocationFunctionServer*>(aEventSourceHandle);
    CLAPILandmark* landmark =
        reinterpret_cast< CLAPILandmark*>(aLandmarkHandle);

    // Array must be created within a leave-safe content so it must be
    // passed to the Event Server's thread. A global reference is needed
    jobjectArray arrayRef(NULL);
    if (aAddressInfo)
    {
        // Create a new global reference.
        arrayRef = static_cast< jobjectArray>(
                       aJniEnv->NewGlobalRef(aAddressInfo));
    }

    TInt error = eventSource->ExecuteTrap(
                     SetAddressInfoL,
                     eventSource->getValidJniEnv(),
                     landmark,
                     &arrayRef);

    // Remove the created global reference
    if (arrayRef)
    {
        aJniEnv->DeleteGlobalRef(arrayRef);
    }
    return error;
}

/*
 * Class:     javax_microedition_location_Landmark
 * Method:    _dispose
 * Signature: (II)V
 */
JNIEXPORT void
JNICALL Java_javax_microedition_location_Landmark__1dispose(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint /*aEventSourceHandle*/,
    jint aLandmarkHandle)
{
    JELOG2(EJavaLocation);

    CBase* object = reinterpret_cast< CBase*>(aLandmarkHandle);

    delete object;
}

// End of file
