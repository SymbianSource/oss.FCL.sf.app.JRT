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
* Description:  JNI implementation of LandmarkStoreManager class
 *
*/


// INTERNAL INCLUDES
#include    "com_nokia_mj_impl_location_LandmarkStoreManager.h"
#include    "jstringutils.h"
#include    "clapilandmarkstoremanager.h"
#include    "lapijnicommon.h"
#include    "lapipanics.h"
#include    "locationfunctionserver.h"

using namespace java::location;

LOCAL_C void CreateManagerL(TInt* aManagerHandle)
{
    JELOG2(EJavaLocation);
    CLAPILandmarkStoreManager* manager = CLAPILandmarkStoreManager::NewL();
    CleanupStack::PushL(manager);
    // Add object to event source
    TInt managerHandle = reinterpret_cast<TInt>(manager);
    CleanupStack::Pop(manager);
    // Return the handle to the JNI context
    *aManagerHandle = managerHandle;
}

/*
 * Class:     com_nokia_microedition_location_LandmarkStoreManager
 * Method:    _createNativePeer
 * Signature: (I)I
 */
JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_location_LandmarkStoreManager__1createNativePeer(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aEventSourceHandle)
{
    JELOG2(EJavaLocation);

    LocationFunctionServer* eventSource =
        reinterpret_cast< LocationFunctionServer*>(aEventSourceHandle);

    TInt managerHandle = KErrGeneral;

    TInt error = eventSource->ExecuteTrap(
                     CreateManagerL,
                     &managerHandle);

    // Return the handle if it was successfully obtained
    return error == KErrNone ? managerHandle : error;
}

LOCAL_C void OpenStoreL(CLAPILandmarkStoreManager* aManager,
                        const TDesC* aStoreName, TInt* aStoreHandle)
{
    __ASSERT_DEBUG(aStoreName, LAPIError::Panic(ELAPIPanicNullArgument));

    // KNulDesC indicates that the default store should be opened
    CLAPILandmarkStore* store = aManager->OpenStoreL(*aStoreName);

    // Add opened store to the event source
    TInt storeHandle = reinterpret_cast<TInt>(store);

    *aStoreHandle = storeHandle;
}

/*
 * Class:     com_nokia_microedition_location_LandmarkStoreManager
 * Method:    _openStore
 * Signature: (IILjava/lang/String;)I
 */
JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_location_LandmarkStoreManager__1openStore(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aEventSourceHandle,
    jint aManagerHandle,
    jstring aStoreName)
{
    JELOG2(EJavaLocation);
    LocationFunctionServer* eventSource =
        reinterpret_cast< LocationFunctionServer*>(aEventSourceHandle);
    CLAPILandmarkStoreManager* manager =
        reinterpret_cast< CLAPILandmarkStoreManager*>(aManagerHandle);

    const JStringUtils storeName(*aJniEnv, aStoreName);
    const TDesC* storeNameArg = (aStoreName ? &storeName : &KNullDesC());

    TInt storeHandle = KErrGeneral;

    TInt error = eventSource->ExecuteTrap(
                     OpenStoreL,
                     manager,
                     storeNameArg,
                     &storeHandle);

    // Return the handle if it was successfully obtained
    return error == KErrNone ? storeHandle : error;
}

LOCAL_C void CreateStoreL(CLAPILandmarkStoreManager* aManager,
                          const TDesC* aStoreName)
{
    aManager->CreateStoreL(*aStoreName);
}

/*
 * Class:     com_nokia_microedition_location_LandmarkStoreManager
 * Method:    _createStore
 * Signature: (IILjava/lang/String;)I
 */
JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_location_LandmarkStoreManager__1createStore(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aEventSourceHandle,
    jint aManagerHandle,
    jstring aStoreName)
{
    JELOG2(EJavaLocation);

    LocationFunctionServer* eventSource =
        reinterpret_cast< LocationFunctionServer*>(aEventSourceHandle);
    CLAPILandmarkStoreManager* manager =
        reinterpret_cast< CLAPILandmarkStoreManager*>(aManagerHandle);

    // Store name must not be null
    const JStringUtils storeName(*aJniEnv, aStoreName);

    TInt error = eventSource->ExecuteTrap(
                     CreateStoreL,
                     manager,
                     static_cast< const TDesC*>(&storeName));

    LOG1(EJavaLocation,EInfo, "Java_com_nokia_microedition_location_LandmarkStoreManager__1createStore - error %d", error);
    return error;
}

LOCAL_C void DeleteStoreL(CLAPILandmarkStoreManager* aManager,
                          const TDesC* aStoreName)
{
    aManager->DeleteStoreL(*aStoreName);
}

/*
 * Class:     com_nokia_microedition_location_LandmarkStoreManager
 * Method:    _deleteStore
 * Signature: (IILjava/lang/String;)I
 */
JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_location_LandmarkStoreManager__1deleteStore(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aEventSourceHandle,
    jint aManagerHandle,
    jstring aStoreName)
{
    JELOG2(EJavaLocation);
    LocationFunctionServer* eventSource =
        reinterpret_cast< LocationFunctionServer*>(aEventSourceHandle);
    CLAPILandmarkStoreManager* manager =
        reinterpret_cast< CLAPILandmarkStoreManager*>(aManagerHandle);

    // Store name must not be null
    const JStringUtils storeName(*aJniEnv, aStoreName);

    TInt error = eventSource->ExecuteTrap(
                     DeleteStoreL,
                     manager,
                     static_cast< const TDesC*>(&storeName));

    LOG1(EJavaLocation,EInfo, "Java_com_nokia_microedition_location_LandmarkStoreManager__1deleteStore - error %d", error);
    return error;
}

LOCAL_C void RemoveLandmarkStore(CLAPILandmarkStoreManager* aManager,
                                 CLAPILandmarkStore* aLandmarkStore)
{
    if (aLandmarkStore)
    {
        aManager->RemoveStore(*aLandmarkStore);
    }
}

/*
 * Class:     com_nokia_microedition_location_LandmarkStoreManager
 * Method:    _removeStore
 * Signature: (III)V
 */
JNIEXPORT void
JNICALL Java_com_nokia_mj_impl_location_LandmarkStoreManager__1removeStore(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aEventSourceHandle,
    jint aManagerHandle,
    jint aStoreHandle)
{
    JELOG2(EJavaLocation);

    LocationFunctionServer* eventSource =
        reinterpret_cast< LocationFunctionServer*>(aEventSourceHandle);
    CLAPILandmarkStoreManager* manager =
        reinterpret_cast< CLAPILandmarkStoreManager*>(aManagerHandle);
    CLAPILandmarkStore* landmarkStore =
        reinterpret_cast< CLAPILandmarkStore*>(aStoreHandle);

    eventSource->ExecuteV(
        RemoveLandmarkStore,
        manager,
        landmarkStore);
}

LOCAL_C void ListStoresL(JNIEnv *aJniEnv, CLAPILandmarkStoreManager* aManager,
                         jobjectArray* aStoreUris)
{
    JELOG2(EJavaLocation);

    // Get an array of landmark store uris from the manager
    CDesCArray* stores = aManager->LandmarkStoresL();
    CleanupStack::PushL(stores);

    // Create a java object array from the store uris: Java-side takes
    // the ownership of the new object array
    *aStoreUris = CopyToNewJavaStringArrayL(*aJniEnv, *stores);
    CleanupStack::PopAndDestroy(stores);
}

/*
 * Class:     com_nokia_microedition_location_LandmarkStoreManager
 * Method:    _listStores
 * Signature: (II)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray
JNICALL Java_com_nokia_mj_impl_location_LandmarkStoreManager__1listStores(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aEventSourceHandle,
    jint aManagerHandle,
    jintArray aErrors)
{
    JELOG2(EJavaLocation);

    LocationFunctionServer* eventSource =
        reinterpret_cast< LocationFunctionServer*>(aEventSourceHandle);
    CLAPILandmarkStoreManager* manager =
        reinterpret_cast< CLAPILandmarkStoreManager*>(aManagerHandle);

    // Ownership of the name array is transferred to Java side
    jobjectArray javaStringArray = NULL;

    TInt error = eventSource->ExecuteTrap(
                     ListStoresL,
                     eventSource->getValidJniEnv(),
                     manager,
                     &javaStringArray);

    LOG1(EJavaLocation,EInfo, "Java_com_nokia_microedition_location_LandmarkStoreManager__1listStores - error %d", error);

    SetJavaErrorCode(*aJniEnv, aErrors, error);
    return javaStringArray;
}

/*
 * Class:     com_nokia_microedition_location_LandmarkStoreManager
 * Method:    _dispose
 * Signature: (II)V
 */
JNIEXPORT void
JNICALL Java_com_nokia_mj_impl_location_LandmarkStoreManager__1dispose(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint /*aFunctionServerHandle*/,
    jint aManagerHandle)
{
    JELOG2(EJavaLocation);

    CBase* object = reinterpret_cast< CBase*>(aManagerHandle);
    delete object;
}

// End of file
