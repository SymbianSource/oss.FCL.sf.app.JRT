/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/
#include <jdebug.h>
#include <JniEnvWrapper.h>

#include "com_nokia_microedition_media_protocol_EMCSourceInfo.h"
#include "cmmamanager.h"

LOCAL_C void SetHeaderDataL(
    CMMAManager* aManager,
    const TUint8* aHeader,
    TInt aLength)
{
    aManager->SetSourceInfoL(aHeader, aLength);
}

/*
 * Class:     com_nokia_microedition_media_protocol_http_EMCSourceInfo
 * Method:    _writeHeaderData
 * Signature: (II[B)V
 */
JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_protocol_EMCSourceInfo__1writeHeaderData
(JNIEnv* aJni, jobject, jint aEventSourceHandle, jint aManagerHandle, jbyteArray aHeader)
{
    // Development time check.
    __ASSERT_DEBUG((aEventSourceHandle > 0) && (aManagerHandle > 0), User::Invariant());

    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);
    CMMAManager* manager = JavaUnhand< CMMAManager >(aManagerHandle);

    if (aHeader)
    {
        // Get pointer to Java header data
        jbyte* data = aJni->GetByteArrayElements(aHeader, NULL);

        // if data is null Java data could not be obtained to native and
        // KErrNoMemory is returned to Java
        if (!data)
        {
            return KErrNoMemory;
        }

        TInt headerDataLength = aJni->GetArrayLength(aHeader);

        TInt err = eventSource->ExecuteTrap(&SetHeaderDataL,
                                            manager,
                                            (const TUint8*)data,
                                            headerDataLength);

        // release bytes got with GetByteArrayElements
        aJni->ReleaseByteArrayElements(aHeader,
                                       data,
                                       0);

        return err;
    }
    else
    {
        // if aHeader is NULL, this method should not have been called
        // from java
        return KErrBadHandle;
    }
}
