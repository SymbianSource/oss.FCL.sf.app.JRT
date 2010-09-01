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
* Description:  This class has NativePlayerFactory JNI functions
*
*/

#include <jdebug.h>

#include "com_nokia_microedition_media_NativePlayerFactory.h"
#include "cmmamanager.h"
#include "cmmaplayer.h"

/**
 * Adds player to event source. If player could not be created
 * handle KErrNone indicates in Java side that there wasn't suitable factory.
 */
LOCAL_C void HandleCreatePlayerL(CMMAPlayer* aPlayer,
                                 CMMAEventSource* aEventSource,
                                 TInt* aHandle)
{
    if (aPlayer)
    {
        // Player was created, add it to event source
        CleanupStack::PushL(aPlayer);
        aEventSource->AddPlayerL(aPlayer);
        *aHandle = JavaMakeHandle(aPlayer);
        CleanupStack::Pop(aPlayer);
    }
    else
    {
        // Data was not supported and there is no error.
        // Return KErrNone to java side
        *aHandle = KErrNone;
    }
}

/**
 * Local function that calls CMMAManager's CreatePlayerL method.
 */
LOCAL_C void CreatePlayerHeaderDataL(CMMAManager* aManager,
                                     CMMAEventSource* aEventSource,
                                     const TDesC8* aHeaderData,
                                     TInt* aHandle)
{
    HandleCreatePlayerL(aManager->CreatePlayerL(*aHeaderData),
                        aEventSource,
                        aHandle);
}

/**
 * JNI function from NativePlayerFactory class.
 */
JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_NativePlayerFactory__1createPlayerHeaderData
(JNIEnv* aJni, jclass,
 jint aEventSourceHandle, jint aManagerHandle,
 jbyteArray aHeaderData)
{
    // Development time check.
    __ASSERT_DEBUG((aEventSourceHandle > 0) && (aManagerHandle > 0), User::Invariant());

    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);
    CMMAManager* manager = JavaUnhand< CMMAManager >(aManagerHandle);

    // Returning just KErrNone if there is no header data
    TInt playerHandle = KErrNone;
    if (aHeaderData)
    {
        // Get pointer to Java header data
        jbyte* data = aJni->GetByteArrayElements(aHeaderData, NULL);
        // if data is null Java data could not be obtained to native and
        // KErrNoMemory is returned to Java
        if (!data)
        {
            return KErrNoMemory;
        }

        TInt headerDataLength = aJni->GetArrayLength(aHeaderData);
        TPtrC8 headerData((TUint8*)data, headerDataLength);
        TInt err = eventSource->ExecuteTrap(&CreatePlayerHeaderDataL,
                                            manager,
                                            eventSource,
                                            (const TDesC8*)&headerData,
                                            &playerHandle);

        // release bytes got with GetByteArrayElements
        aJni->ReleaseByteArrayElements(aHeaderData,
                                       data,
                                       0);
        if (err != KErrNone)
        {
            // Leave occured return error code to Java
            playerHandle = err;
            DEBUG_INT("MMA::NativePlayerFactory createPlayerHeaderData err %d",
                      playerHandle);
        }
    }
    return playerHandle;
}

/**
 * Local function that calls CMMAManager's CreatePlayerL method.
 */
LOCAL_C void CreatePlayerLocatorL(CMMAManager* aManager,
                                  CMMAEventSource* aEventSource,
                                  const TDesC* aProtocol,
                                  const TDesC* aMiddlePart,
                                  const TDesC* aParameters,
                                  TInt* aHandle)
{
    HandleCreatePlayerL(aManager->CreatePlayerL(*aProtocol,
                        *aMiddlePart,
                        *aParameters),
                        aEventSource,
                        aHandle);
}

/**
 * JNI function from NativePlayerFactory class.
 */
JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_NativePlayerFactory__1createPlayerLocator
(JNIEnv* aJni, jclass,
 jint aEventSourceHandle, jint aManagerHandle,
 jstring aProtocol, jstring aMiddlePart, jstring aParameters)
{
    // Development time check.
    __ASSERT_DEBUG((aEventSourceHandle > 0) && (aManagerHandle > 0), User::Invariant());

    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);
    CMMAManager* manager = JavaUnhand< CMMAManager >(aManagerHandle);

    // Get Java strings to native
    RJString protocol(*aJni, aProtocol);
    RJString middlePart(*aJni, aMiddlePart);

    // If Java parameters is null, empty descriptor will be given to
    // CreatePlayerL method.
    TPtrC parameters(KNullDesC);
    RJString* tmp = NULL;
    if (aParameters != NULL)
    {
        tmp = new RJString(*aJni, aParameters);
        if (tmp)
        {
            parameters.Set(*tmp);
        }
        else
        {
            // failed to create string
            return KErrNoMemory;
        }
    }

    TInt playerHandle = KErrNoMemory;
    TInt err = eventSource->ExecuteTrap(&CreatePlayerLocatorL,
                                        manager,
                                        eventSource,
                                        (const TDesC*)&protocol,
                                        (const TDesC*)&middlePart,
                                        (const TDesC*)&parameters,
                                        &playerHandle);
    delete tmp;
    if (err != KErrNone)
    {
        // Leave occured return error code to Java
        playerHandle = err;
        DEBUG_INT("MMA::NativePlayerFactory createPlayerLocator err %d",
                  playerHandle);
    }
    return playerHandle;
}

/**
 * Local function that calls CMMAManager's CreatePlayerL method.
 */
LOCAL_C void CreatePlayerContentTypeL(CMMAManager* aManager,
                                      CMMAEventSource* aEventSource,
                                      const TDesC* aContentType,
                                      TInt* aHandle)
{
    HandleCreatePlayerL(aManager->CreatePlayerL(*aContentType),
                        aEventSource,
                        aHandle);
}

/**
 * JNI function from NativePlayerFactory class.
 */
JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_NativePlayerFactory__1createPlayerContentType
(JNIEnv* aJni, jclass,
 jint aEventSourceHandle, jint aManagerHandle,
 jstring aContentType)
{
    // Development time check.
    __ASSERT_DEBUG((aEventSourceHandle > 0) && (aManagerHandle > 0), User::Invariant());

    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);
    CMMAManager* manager = JavaUnhand< CMMAManager >(aManagerHandle);

    RJString contentType(*aJni, aContentType);
    TInt playerHandle = KErrNoMemory;
    TInt err = eventSource->ExecuteTrap(&CreatePlayerContentTypeL,
                                        manager,
                                        eventSource,
                                        (const TDesC*)&contentType,
                                        &playerHandle);
    if (err != KErrNone)
    {
        // Leave occured return error code to Java
        playerHandle = err;
        DEBUG_INT("MMA::NativePlayerFactory createPlayerContentType err %d",
                  playerHandle);
    }
    return playerHandle;
}

//  END OF FILE
