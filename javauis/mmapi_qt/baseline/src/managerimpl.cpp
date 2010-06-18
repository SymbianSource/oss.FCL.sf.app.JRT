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

#include <logger.h>

#include <JniEnvWrapper.h>

#include "com_nokia_microedition_media_ManagerImpl.h"
#include "cmmamanager.h"
#include "cmmaplayer.h"
#include "jstringutils.h"
#include "s60commonutils.h"
using namespace java::util;

const TInt KMMADefaultArrayGranularity = 8;

JNIEXPORT void JNICALL Java_com_nokia_microedition_media_ManagerImpl__1dispose
(JNIEnv* /*aJni*/, jobject, jint aEventSourceHandle)
{
    LOG(EJavaMMAPI,EInfo,"MMA::ManagerImpl dispose");
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSourceHandle);
    delete eventSource;
}

LOCAL_C void ReleaseEventSource(MMAFunctionServer* aEventSource)
{
    aEventSource->Release();
}

JNIEXPORT void JNICALL Java_com_nokia_microedition_media_ManagerImpl__1release
(JNIEnv*, jobject, jint aEventSourceHandle)
{
    LOG(EJavaMMAPI, EInfo, "MMA::ManagerImpl release");
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSourceHandle);
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
    MMAFunctionServer* eventSource =  reinterpret_cast<MMAFunctionServer*>(aEventSourceHandle);

    CMMAManager* manager = NULL;
    TInt error = eventSource->ExecuteTrap(&CMMAManager::StaticCreateManagerL,
                                          &manager,
                                          aMIDletSuiteID);
    ELOG1(EJavaMMAPI, "MMA::ManagerImpl createManager StaticCreateManagerL %d",
          error);
    if (error != KErrNone)
    {
        return error;
    }

    TInt managerHandle(reinterpret_cast<TInt>(manager));

    error = eventSource->ExecuteTrap(MMAFunctionServer::StaticAddObjectFromHandleL,
                                     eventSource,
                                     managerHandle);

    ELOG1(EJavaMMAPI, "MMA::ManagerImpl createManager StaticAddObjectFromHandleL %d",
          error);

    if (error != KErrNone)
    {
        // Adding manager to event source failed.
        delete manager;

        // Error code will be returned to java
        managerHandle = error;
    }
    LOG1(EJavaMMAPI, EInfo, "MMA::ManagerImpl createManager %d",
         managerHandle);
    return managerHandle;
}


LOCAL_C void InvokeConstructSvrL(MMAFunctionServer* aEventSource)
{
    aEventSource->ConstructSvrL();
}

/*
 * Class:     javax_microedition_media_Manager
 * Method:    _getEventSource
 */
JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_ManagerImpl__1createEventSource
(JNIEnv* aJni, jobject aPeer)
{
    TInt eventSourceHandle = MMAFunctionServer::NewL(*aJni, aPeer);
    if (eventSourceHandle > KErrNone)
    {
        MMAFunctionServer* eventSource =
            reinterpret_cast< MMAFunctionServer* >(eventSourceHandle);

        // third phase construction
        TInt error = eventSource->ExecuteTrap(&InvokeConstructSvrL,
                                              eventSource);
        if (error != KErrNone)
        {
            eventSourceHandle = error;
        }
    }
    LOG1(EJavaMMAPI, EInfo, "MMA::ManagerImpl createEventSource %d",
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
LOCAL_C void GetObjectArrayL(MMAFunctionServer* aEventSource,
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
    aJni =aEventSource->getValidJniEnv();

    // Create new java String array and copy values from the values array
    *aArray = MMAPIUtils::CopyToNewJavaStringArrayL(*aJni, *values);
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
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer * >(aEventSourceHandle);
    CMMAManager* manager = reinterpret_cast< CMMAManager *>(aManagerHandle);

    jobjectArray contentTypes = NULL;

    // Create descritor from Java String.
    // JStringUtils is derived from TPtrC16 and is save to cast to TDesC
    JStringUtils tmp(*aJni, aProtocol);
    const TDesC* protocol = &tmp;
    if (aProtocol == NULL)
    {
        protocol = &KNullDesC;
    }

    // Call manager->GetSupportedContentTypesL and create Java array.
    TInt err = eventSource->ExecuteTrap(&GetObjectArrayL,
                                        eventSource,
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
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer *>(aEventSourceHandle);
    CMMAManager* manager = reinterpret_cast< CMMAManager *>(aManagerHandle);

    jobjectArray protocols = NULL;

    // Create descritor from Java String.
    // JStringUtils is derived from TPtrC16 and is save to cast to TDesC
    JStringUtils tmp(*aJni, aContentType);
    const TDesC* contentType = &tmp;
    if (aContentType == NULL)
    {
        contentType = &KNullDesC;
    }

    // Call manager->GetSupportedProtocolsL and create Java array.
    TInt err = eventSource->ExecuteTrap(&GetObjectArrayL,
                                        eventSource,
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
