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
* Description:  JNI implementation of LandmarkStore class
 *
*/


// INTERNAL INCLUDES
#include "javax_microedition_location_LandmarkStore.h"
#include "logger.h"
#include "jstringutils.h"
#include "lapijnicommon.h"
#include "clapilandmark.h"
#include "clapilandmarkstore.h"
#include "mlapicategorymanager.h"
#include "tlapisearchcriteria.h"
#include "lapipanics.h"
#include "locationfunctionserver.h"

using namespace java::location;

LOCAL_C void AddLandmarkL(CLAPILandmarkStore* aLandmarkStore,
                          CLAPILandmark* aLandmark, const TDesC* aCategory)
{
    JELOG2(EJavaLocation);
    MLAPICategoryManager* categoryManager = aLandmarkStore->CategoryManagerL();
    // Category argument can be null
    if (aCategory && categoryManager)
    {
        TRAPD(err, categoryManager->AddLandmarkToCategoryL(*aLandmark,
                *aCategory));
        // The landmark was not found from the store. Add new
        if (err == KErrNotFound)
        {
            aLandmarkStore->AddLandmarkL(*aLandmark);
            categoryManager->AddLandmarkToCategoryL(*aLandmark, *aCategory);
            err = KErrNone;
        }
        // Leave if some other error occured
        User::LeaveIfError(err);
    }
    else
    {
        // No category information. Add a new landmark
        aLandmarkStore->AddLandmarkL(*aLandmark);
    }

}

/*
 * Class:     javax_microedition_location_LandmarkStore
 * Method:    _addLandmark
 * Signature: (III)I
 */
JNIEXPORT jint
JNICALL Java_javax_microedition_location_LandmarkStore__1addLandmark(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aEventSourceHandle,
    jint aStoreHandle,
    jint aLandmarkHandle,
    jstring aCategory)
{
    JELOG2(EJavaLocation);

    LocationFunctionServer* eventSource =
        reinterpret_cast< LocationFunctionServer*>(aEventSourceHandle);
    CLAPILandmarkStore* landmarkStore =
        reinterpret_cast< CLAPILandmarkStore*>(aStoreHandle);
    CLAPILandmark* landmark =
        reinterpret_cast< CLAPILandmark*>(aLandmarkHandle);

    const JStringUtils category(*aJniEnv, aCategory);
    const TDesC* categoryArg = (aCategory ? &category : NULL);

    TInt error = eventSource->ExecuteTrap(
                     AddLandmarkL,
                     landmarkStore,
                     landmark,
                     categoryArg);

    LOG1(EJavaLocation,EInfo, "Java_javax_microedition_location_LandmarkStore__1addLandmark - error %d", error);

    return error;
}

LOCAL_C void LandmarksL(JNIEnv* aJniEnv, CLAPILandmarkStore* aLandmarkStore,
                        TUint* aAttributes, TLAPISearchCriteria* aSearchCriteria,
                        jintArray* aLandmarkHandles)
{
    JELOG2(EJavaLocation);

    // Get items using the specified search criteria
    CArrayPtr<CLAPILandmark>* landmarks = aLandmarkStore->LandmarksL(
                                              *aAttributes, aSearchCriteria);
    // No need to push the array to the cleanup stack since it is
    // used from non-leaving context
    *aLandmarkHandles = CreateHandleArray(*aJniEnv, *landmarks);
    // Reset the items since the ownership is transferred to Java side
    landmarks->Reset();
    // Delete the empty array
    delete landmarks;
    // Leave if the result array is null
    __ASSERT_ALWAYS(aLandmarkHandles, User::Leave(KErrNoMemory));
}

/*
 * Class:     javax_microedition_location_LandmarkStore
 * Method:    _getLandmarks
 * Signature: (II[I)[I
 */
JNIEXPORT jintArray
JNICALL Java_javax_microedition_location_LandmarkStore__1getLandmarks(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aEventSourceHandle,
    jint aStoreHandle,
    jintArray aError)
{
    JELOG2(EJavaLocation);

    LocationFunctionServer* eventSource =
        reinterpret_cast< LocationFunctionServer*>(aEventSourceHandle);
    CLAPILandmarkStore* landmarkStore =
        reinterpret_cast< CLAPILandmarkStore*>(aStoreHandle);

    // VM will own the result array
    jintArray handles(NULL);
    // Criteria is null so all landmark should be returned
    TLAPISearchCriteria* criteria(NULL);
    // Initialize only the name from each landmark
    TUint initializedAttributes = ELAPILmAttrName;

    TInt error = eventSource->ExecuteTrap(
                     LandmarksL,
                     eventSource->getValidJniEnv(),
                     landmarkStore,
                     &initializedAttributes,
                     criteria,
                     &handles);

    LOG1(EJavaLocation,EInfo, "Java_javax_microedition_location_LandmarkStore__1getLandmarks - error %d", error);

    // Set error code if LandmarksL leaved. KErrNone if successful
    SetJavaErrorCode(*aJniEnv, aError, error);
    return handles; // NULL indicates an error
}

/*
 * Class:     javax_microedition_location_LandmarkStore
 * Method:    _getLandmarksFromCategory
 * Signature: (IILjava/lang/String;Ljava/lang/String;[I)[I
 */
JNIEXPORT jintArray
JNICALL Java_javax_microedition_location_LandmarkStore__1getLandmarksFromCategory(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aEventSourceHandle,
    jint aStoreHandle,
    jstring aCategory,
    jstring aName,
    jintArray aError)
{
    JELOG2(EJavaLocation);

    LocationFunctionServer* eventSource =
        reinterpret_cast< LocationFunctionServer*>(aEventSourceHandle);
    CLAPILandmarkStore* landmarkStore =
        reinterpret_cast< CLAPILandmarkStore*>(aStoreHandle);

    // VM will own the result array
    jintArray handles(NULL);
    // Resolve name argument. NULL indicates a wild card
    const JStringUtils name(*aJniEnv, aName);
    const TDesC* nameArg = (aName ? &name : NULL);
    // Resolve category argument. NULL indicates a wild card
    const JStringUtils category(*aJniEnv, aCategory);
    const TDesC* categoryArg = (aCategory ? &category : NULL);

    TLAPISearchCriteria criteria;
    criteria.SetCategoryName(categoryArg);
    criteria.SetText(nameArg);
    criteria.SetTextAttributes(ELAPILmAttrName);

    // Initialize name information to each item. Category information
    // cannot be currently obtained using the Java landmark so there
    // is no need to initialize it
    TUint initializedAttributes = ELAPILmAttrName;

    TInt error = eventSource->ExecuteTrap(
                     LandmarksL,
                     eventSource->getValidJniEnv(),
                     landmarkStore,
                     &initializedAttributes,
                     &criteria,
                     &handles);

    LOG1(EJavaLocation,EInfo, "Java_javax_microedition_location_LandmarkStore__1getLandmarksFromCategory - error %d", error);

    // Set error code if LandmarksL leaved. KErrNone if successful. Invalid search
    // arguments will return null result with no errors
    error = error == KErrArgument ? KErrNone : error;
    SetJavaErrorCode(*aJniEnv, aError, error);
    return handles; // NULL indicates an error
}

/*
 * Class:     javax_microedition_location_LandmarkStore
 * Method:    _getLandmarksFromArea
 * Signature: (IILjava/lang/String;DDDD[I)[I
 */
JNIEXPORT jintArray
JNICALL Java_javax_microedition_location_LandmarkStore__1getLandmarksFromArea(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aEventSourceHandle,
    jint aStoreHandle,
    jstring aCategory,
    jdouble aMinLatitude,
    jdouble aMaxLatitude,
    jdouble aMinLongitude,
    jdouble aMaxLongitude,
    jintArray aError)
{
    JELOG2(EJavaLocation);

    LocationFunctionServer* eventSource =
        reinterpret_cast< LocationFunctionServer*>(aEventSourceHandle);
    CLAPILandmarkStore* landmarkStore =
        reinterpret_cast< CLAPILandmarkStore*>(aStoreHandle);

    // VM will own the result array
    jintArray handles(NULL);
    const JStringUtils category(*aJniEnv, aCategory);
    const TDesC* categoryArg = (aCategory ? &category : NULL);

    TLAPISearchCriteria criteria;
    criteria.SetCategoryName(categoryArg);
    criteria.SetArea(aMinLatitude, aMaxLatitude, aMinLongitude, aMaxLongitude);

    // Initialize name and position to each item. Category information
    // cannot be currently obtained using the Java landmark so there
    // is no need to initialize it
    TUint initializedAttributes = ELAPILmAttrName | ELAPILmAttrPosition;

    TInt error = eventSource->ExecuteTrap(
                     LandmarksL,
                     eventSource->getValidJniEnv(),
                     landmarkStore,
                     &initializedAttributes,
                     &criteria,
                     &handles);

    LOG1(EJavaLocation,EInfo, "Java_javax_microedition_location_LandmarkStore__1getLandmarksFromArea - error %d", error);

    // Set error code if LandmarksL leaved. KErrNone if successful. Invalid search
    // arguments will return null result with no errors
    error = error == KErrArgument ? KErrNone : error;
    SetJavaErrorCode(*aJniEnv, aError, error);
    return handles; // NULL indicates an error
}

LOCAL_C void RemoveLandmarkFromCategoryL(CLAPILandmarkStore* aLandmarkStore,
        CLAPILandmark* aLandmark, const TDesC* aCategoryName)
{
    JELOG2(EJavaLocation);
    __ASSERT_DEBUG(aCategoryName, LAPIError::Panic(ELAPIPanicNullArgument));
    MLAPICategoryManager* categoryManager = aLandmarkStore->CategoryManagerL();
    if (categoryManager)
    {
        // Remove the landmark from the category if it is possible
        categoryManager->RemoveLandmarkFromCategoryL(*aLandmark, *aCategoryName);
    }
}

/*
 * Class:     javax_microedition_location_LandmarkStore
 * Method:    _removeLandmarkFromCategory
 * Signature: (IIILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_javax_microedition_location_LandmarkStore__1removeLandmarkFromCategory(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aEventSourceHandle,
    jint aStoreHandle,
    jint aLandmarkHandle,
    jstring aCategory)
{
    JELOG2(EJavaLocation);

    LocationFunctionServer* eventSource =
        reinterpret_cast< LocationFunctionServer*>(aEventSourceHandle);
    CLAPILandmarkStore* landmarkStore =
        reinterpret_cast< CLAPILandmarkStore*>(aStoreHandle);
    CLAPILandmark* landmark =
        reinterpret_cast< CLAPILandmark*>(aLandmarkHandle);

    const JStringUtils category(*aJniEnv, aCategory);

    TInt error = eventSource->ExecuteTrap(
                     RemoveLandmarkFromCategoryL,
                     landmarkStore,
                     landmark,
                     static_cast< const TDesC*>(&category));

    return error;
}

LOCAL_C void UpdateLandmarkL(CLAPILandmarkStore* aLandmarkStore,
                             CLAPILandmark* aLandmark)
{
    aLandmarkStore->UpdateLandmarkL(*aLandmark);
}

/*
 * Class:     javax_microedition_location_LandmarkStore
 * Method:    _updateLandmark
 * Signature: (III)I
 */
JNIEXPORT jint
JNICALL Java_javax_microedition_location_LandmarkStore__1updateLandmark(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aEventSourceHandle,
    jint aStoreHandle,
    jint aLandmarkHandle)
{
    JELOG2(EJavaLocation);

    LocationFunctionServer* eventSource =
        reinterpret_cast< LocationFunctionServer*>(aEventSourceHandle);
    CLAPILandmarkStore* landmarkStore =
        reinterpret_cast< CLAPILandmarkStore*>(aStoreHandle);
    CLAPILandmark* landmark =
        reinterpret_cast< CLAPILandmark*>(aLandmarkHandle);

    TInt error = eventSource->ExecuteTrap(
                     UpdateLandmarkL,
                     landmarkStore,
                     landmark);

    LOG1(EJavaLocation,EInfo, "Java_javax_microedition_location_LandmarkStore__1updateLandmark - error %d", error);

    return error;
}

LOCAL_C void DeleteLandmarkL(CLAPILandmarkStore* aLandmarkStore,
                             CLAPILandmark* aLandmark)
{
    aLandmarkStore->RemoveLandmarkL(*aLandmark);
}

/*
 * Class:     javax_microedition_location_LandmarkStore
 * Method:    _deleteLandmark
 * Signature: (III)I
 */
JNIEXPORT jint
JNICALL Java_javax_microedition_location_LandmarkStore__1deleteLandmark(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aEventSourceHandle,
    jint aStoreHandle,
    jint aLandmarkHandle)
{
    JELOG2(EJavaLocation);

    LocationFunctionServer* eventSource =
        reinterpret_cast< LocationFunctionServer*>(aEventSourceHandle);
    CLAPILandmarkStore* landmarkStore =
        reinterpret_cast< CLAPILandmarkStore*>(aStoreHandle);
    CLAPILandmark* landmark =
        reinterpret_cast< CLAPILandmark*>(aLandmarkHandle);

    TInt error = eventSource->ExecuteTrap(
                     DeleteLandmarkL,
                     landmarkStore,
                     landmark);

    LOG1(EJavaLocation,EInfo, "Java_javax_microedition_location_LandmarkStore__1deleteLandmark - error %d", error);

    // Ignore errors indicating that the landmark has already been ignored
    return error == KErrNotFound ? KErrNone : error;
}

LOCAL_C void ListCategoriesL(JNIEnv* aJniEnv,
                             CLAPILandmarkStore* aLandmarkStore, jobjectArray* aCategories)
{
    JELOG2(EJavaLocation);

    // Get an array of landmark store uris from the manager
    MLAPICategoryManager* categoryManager = aLandmarkStore->CategoryManagerL();
    // Supports category handling
    if (categoryManager)
    {
        const CDesCArray& categories = categoryManager->ListCategoriesL();
        // Create a java object array from the category names. Java-side takes
        // the ownership of the new object array
        *aCategories = CopyToNewJavaStringArrayL(*aJniEnv, categories);
    }
}

/*
 * Class:     javax_microedition_location_LandmarkStore
 * Method:    _listCategories
 * Signature: (II[I)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray
JNICALL Java_javax_microedition_location_LandmarkStore__1listCategories(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aEventSourceHandle,
    jint aStoreHandle,
    jintArray aError)
{
    JELOG2(EJavaLocation);

    LocationFunctionServer* eventSource =
        reinterpret_cast< LocationFunctionServer*>(aEventSourceHandle);
    CLAPILandmarkStore* landmarkStore =
        reinterpret_cast< CLAPILandmarkStore*>(aStoreHandle);

    // Ownership of the name array is transferred to Java side
    jobjectArray javaStringArray = NULL;

    TInt error = eventSource->ExecuteTrap(
                     ListCategoriesL,
                     eventSource->getValidJniEnv(),
                     landmarkStore,
                     &javaStringArray);

    SetJavaErrorCode(*aJniEnv, aError, error);
    return javaStringArray;
}

LOCAL_C void AddCategoryL(CLAPILandmarkStore* aLandmarkStore,
                          const TDesC* aCategoryName)
{
    __ASSERT_DEBUG(aCategoryName, LAPIError::Panic(ELAPIPanicNullArgument));
    // Get the category manager and add new category if the store supports it
    MLAPICategoryManager* categoryManager = aLandmarkStore->CategoryManagerL();
    // Supports category handling
    if (categoryManager)
    {
        categoryManager->AddCategoryL(*aCategoryName);
        // Compact the store if needed
        aLandmarkStore->CompactIfNeededL();
    }
}

/*
 * Class:     javax_microedition_location_LandmarkStore
 * Method:    _addCategory
 * Signature: (IILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_javax_microedition_location_LandmarkStore__1addCategory(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aEventSourceHandle,
    jint aStoreHandle,
    jstring aCategory)
{
    JELOG2(EJavaLocation);

    LocationFunctionServer* eventSource =
        reinterpret_cast< LocationFunctionServer*>(aEventSourceHandle);
    CLAPILandmarkStore* landmarkStore =
        reinterpret_cast< CLAPILandmarkStore*>(aStoreHandle);

    // Store name must not be null
    const JStringUtils categoryName(*aJniEnv, aCategory);

    TInt error = eventSource->ExecuteTrap(
                     AddCategoryL,
                     landmarkStore,
                     static_cast< const TDesC*>(&categoryName));

    LOG1(EJavaLocation,EInfo, "Java_javax_microedition_location_LandmarkStore__1addCategory error %d", error);

    return error;
}

LOCAL_C void DeleteCategoryL(CLAPILandmarkStore* aLandmarkStore,
                             const TDesC* aCategoryName)
{
    __ASSERT_DEBUG(aCategoryName, LAPIError::Panic(ELAPIPanicNullArgument));
    // Get the category manager and add new category if the store supports it
    MLAPICategoryManager* categoryManager = aLandmarkStore->CategoryManagerL();
    // Supports category handling
    if (categoryManager)
    {
        categoryManager->RemoveCategoryL(*aCategoryName);

    }
}

/*
 * Class:     javax_microedition_location_LandmarkStore
 * Method:    _deleteCategory
 * Signature: (IILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_javax_microedition_location_LandmarkStore__1deleteCategory(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aEventSourceHandle,
    jint aStoreHandle,
    jstring aCategory)
{
    JELOG2(EJavaLocation);

    LocationFunctionServer* eventSource =
        reinterpret_cast< LocationFunctionServer*>(aEventSourceHandle);
    CLAPILandmarkStore* landmarkStore =
        reinterpret_cast< CLAPILandmarkStore*>(aStoreHandle);

    // Store name must not be null
    const JStringUtils categoryName(*aJniEnv, aCategory);

    TInt error = eventSource->ExecuteTrap(
                     DeleteCategoryL,
                     landmarkStore,
                     static_cast< const TDesC*>(&categoryName));

    LOG1(EJavaLocation,EInfo,
         "Java_javax_microedition_location_LandmarkStore__1deleteCategory error %d", error);

    return error;
}

LOCAL_C void DisposeObject(CLAPILandmarkStore* aLandmarkStore)
{
    delete aLandmarkStore;
}
/*
 * Class:     javax_microedition_location_LandmarkStore
 * Method:    _dispose
 * Signature: (II)V
 */
JNIEXPORT void
JNICALL Java_javax_microedition_location_LandmarkStore__1dispose(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aEventSourceHandle,
    jint aManagerHandle)
{
    JELOG2(EJavaLocation);

    LocationFunctionServer* eventSource =
        reinterpret_cast< LocationFunctionServer*>(aEventSourceHandle);
    CLAPILandmarkStore* object =
        reinterpret_cast< CLAPILandmarkStore*>(aManagerHandle);

    eventSource->ExecuteV(
        DisposeObject,
        object);
}

// End of file
