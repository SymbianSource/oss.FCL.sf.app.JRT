/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class has ManagerImpl JNI functions
*
*/

#include <jdebug.h>
#include <JniEnvWrapper.h>

#include "com_nokia_microedition_media_ManagerImpl.h"
#include "cmmamanager.h"
#include "cmmaplayer.h"

const TInt KMMADefaultArrayGranularity = 8;

JNIEXPORT void JNICALL Java_com_nokia_microedition_media_ManagerImpl__1dispose
(JNIEnv* aJni, jobject, jint aEventSourceHandle)
{
    DEBUG("MMA::ManagerImpl dispose");
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    eventSource->Dispose(*aJni);
}

LOCAL_C void ReleaseEventSource(CMMAEventSource* aEventSource)
{
    aEventSource->Release();
}

JNIEXPORT void JNICALL Java_com_nokia_microedition_media_ManagerImpl__1release
(JNIEnv*, jobject, jint aEventSourceHandle)
{
    DEBUG("MMA::ManagerImpl release");
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);
    eventSource->ExecuteV(&ReleaseEventSource,
                          eventSource);
}

/*
 * Class:     javax_microedition_media_Manager
 * Method:    _createManager
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_ManagerImpl__1createManager
(JNIEnv*, jobject, jint aEventSourceHandle, jint aMIDletSuiteID)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    CMMAManager* manager = NULL;
    TInt error = eventSource->ExecuteTrap(&CMMAManager::StaticCreateManagerL,
                                          &manager,
                                          aMIDletSuiteID);
    DEBUG_INT("MMA::ManagerImpl createManager StaticCreateManagerL %d",
              error);
    if (error != KErrNone)
    {
        return error;
    }

    TInt managerHandle(JavaMakeHandle(manager));

    error = eventSource->ExecuteTrap(CMMAEventSource::StaticAddObjectFromHandleL,
                                     eventSource,
                                     managerHandle);

    DEBUG_INT("MMA::ManagerImpl createManager StaticAddObjectFromHandleL %d",
              error);

    if (error != KErrNone)
    {
        // Adding manager to event source failed.
        delete manager;

        // Error code will be returned to java
        managerHandle = error;
    }
    DEBUG_INT("MMA::ManagerImpl createManager %d",
              managerHandle);
    return managerHandle;
}


LOCAL_C void InvokeConstructSvrL(CMMAEventSource* aEventSource)
{
    aEventSource->ConstructSvrL();
}

/*
 * Class:     javax_microedition_media_Manager
 * Method:    _getEventSource
 */
JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_ManagerImpl__1createEventSource
(JNIEnv* aJni, jobject aPeer , jint aEventServerHandle)
{
    TInt eventSourceHandle = CMMAEventSource::New(*aJni, aPeer, aEventServerHandle);
    if (eventSourceHandle > KErrNone)
    {
        CMMAEventSource* eventSource =
            JavaUnhand< CMMAEventSource >(eventSourceHandle);

        // third phase construction
        TInt error = eventSource->ExecuteTrap(&InvokeConstructSvrL,
                                              eventSource);
        if (error != KErrNone)
        {
            eventSourceHandle = error;
        }
    }
    DEBUG_INT("MMA::ManagerImpl createEventSource %d",
              eventSourceHandle);
    return eventSourceHandle;
}

/**
 * Local function which can be used to call CMMAManager class methods.
 * Type of of the function pointer must be
 * void CMMAManager::aFuncL( RPointerArray< HBufC >&, const TDesC& )
 * Java String array will be created and set to aArray variable.
 *
 * @param aManager CMMAManager instance.
 * @param aFuncL Pointer to the CMMAManager method.
 * @param aJni Used to create an array.
 * @param aArray Values got from aFuncL will be copied to this array.
 * @param aParam Parameter that will be passed to aFuncL method.
 */
LOCAL_C void GetObjectArrayL(
    CMMAManager* aManager,
    void (CMMAManager::*aFuncL)(const TDesC&,
                                CDesC16Array&),
    JNIEnv* aJni,
    jobjectArray* aArray,
    const TDesC* aParam)
{
    // Temporary descriptor array to be copied to Java array.
    CDesC16ArrayFlat* values = new(ELeave) CDesC16ArrayFlat(
        KMMADefaultArrayGranularity);
    CleanupStack::PushL(values);

    // Call void CMMAManager::aFuncL( const TDesC&, RPointerArray< HBufC >& )
    // method. Method can leave.
    // Elements of the array will be owned by this method.
    (aManager->*aFuncL)(*aParam, *values);

    // JNI interface pointer can't be passed to different thread, so
    // it is needed to get valid JNI interface pointer for Event Server thread
    aJni = JniEnvWrapper::GetValidJniRef();

    // Create new java String array and copy values from the values array
    *aArray = CopyToNewJavaStringArrayL(*aJni, *values);
    CleanupStack::PopAndDestroy(values);
}


/*
 * Class:     javax_microedition_media_Manager
 * Method:    _getSupportedContentTypes
 * Signature: (II)[[B
 */
JNIEXPORT jobjectArray JNICALL Java_com_nokia_microedition_media_ManagerImpl__1getSupportedContentTypes
(JNIEnv* aJni,
 jclass,
 jint aEventSourceHandle,
 jint aManagerHandle,
 jstring aProtocol)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);
    CMMAManager* manager = JavaUnhand< CMMAManager >(aManagerHandle);

    jobjectArray contentTypes = NULL;

    // Create descritor from Java String.
    // RJString is derived from TPtrC16 and is save to cast to TDesC
    RJString tmp(*aJni, aProtocol);
    const TDesC* protocol = &tmp;
    if (aProtocol == NULL)
    {
        protocol = &KNullDesC;
    }

    // Call manager->GetSupportedContentTypesL and create Java array.
    TInt err = eventSource->ExecuteTrap(&GetObjectArrayL,
                                        manager,
                                        &CMMAManager::GetSupportedContentTypesL,
                                        aJni,
                                        &contentTypes,
                                        protocol);

    if (err != KErrNone)
    {
        // Something failed. Returning NULL, because contentTypes array may not
        // be complete. contentTypes array is left for carbage collertor,
        // because it can't be released.
        return NULL;
    }
    return contentTypes;
}

/*
 * Class:     javax_microedition_media_Manager
 * Method:    _getSupportedProtocols
 * Signature: (II)[[B
 */
JNIEXPORT jobjectArray JNICALL Java_com_nokia_microedition_media_ManagerImpl__1getSupportedProtocols
(JNIEnv* aJni,
 jclass,
 jint aEventSourceHandle,
 jint aManagerHandle,
 jstring aContentType)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);
    CMMAManager* manager = JavaUnhand< CMMAManager >(aManagerHandle);

    jobjectArray protocols = NULL;

    // Create descritor from Java String.
    // RJString is derived from TPtrC16 and is save to cast to TDesC
    RJString tmp(*aJni, aContentType);
    const TDesC* contentType = &tmp;
    if (aContentType == NULL)
    {
        contentType = &KNullDesC;
    }

    // Call manager->GetSupportedProtocolsL and create Java array.
    TInt err = eventSource->ExecuteTrap(&GetObjectArrayL,
                                        manager,
                                        &CMMAManager::GetSupportedProtocolsL,
                                        aJni,
                                        &protocols,
                                        contentType);
    if (err != KErrNone)
    {
        // Something failed. Returning NULL, because contentTypes array may not
        // be complete. contentTypes array is left for carbage collertor,
        // because it can't be released.
        return NULL;
    }
    return protocols;
}

//  END OF FILE
