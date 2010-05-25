/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class has JNI wrappers for CMMAOutputStream
*
*/


#include "jutils.h"
#include "com_nokia_microedition_media_protocol_OutputStreamWriter.h"
#include "cmmaoutputstream.h"
#include "cmmaeventsource.h"

// If eventsource is already disposed, then do nothing
#define CHECK_HANDLE(x, j) { if ( !( x ) || ( ( x )->Players().Count() == 0 ) ) { return ( j ); } }

LOCAL_C void ReadDataL(CMMAEventSource* aEventSource,
                       TInt aPlayer,
                       CMMAOutputStream* aOutputStream,
                       TUint8* aOutputBuffer,
                       TInt* aOutputBufferSize,
                       TInt* aReadStatus)
{
    if (!aEventSource->FindPlayer(aPlayer))
    {
        // Native object was already deleted
        User::Leave(KErrBadHandle);
    }

    aOutputStream->ReadDataL(aOutputBuffer, aOutputBufferSize, aReadStatus);
}

JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_protocol_OutputStreamWriter__1readData
(JNIEnv* aJni, jclass, jint aOutputStream, jint aEventSource,
 jbyteArray aOutputBuffer, jintArray aOutputBufferSize, jint aLength, jint aPlayerHandle)
{
    CMMAEventSource* eventSource = JavaUnhand< CMMAEventSource >(aEventSource);

    CHECK_HANDLE(eventSource, KErrNone);

    CMMAOutputStream* outputStream = JavaUnhand< CMMAOutputStream >(aOutputStream);

    TInt readStatus;
    jbyte* outputBuffer = aJni->GetByteArrayElements(aOutputBuffer, NULL);

    // NULL is returned if there is not enough memory
    if (!outputBuffer)
    {
        return KErrNoMemory;
    }

    jint* outputBufferSize = aJni->GetIntArrayElements(aOutputBufferSize, NULL);
    if (!outputBufferSize)
    {
        // outputBuffer was already allocated
        aJni->ReleaseByteArrayElements(aOutputBuffer, outputBuffer, JNI_ABORT);
        return KErrNoMemory;
    }

    *outputBufferSize = aLength; // size of the buffer

    TInt err = eventSource->ExecuteTrap(ReadDataL,
                                        eventSource,
                                        aPlayerHandle,
                                        outputStream,
                                        (TUint8*) outputBuffer,
                                        outputBufferSize, // returns readed size
                                        &readStatus);

    aJni->ReleaseByteArrayElements(aOutputBuffer, outputBuffer, 0);
    aJni->ReleaseIntArrayElements(aOutputBufferSize, outputBufferSize, 0);

    if (err != KErrNone)
    {
        return err;
    }

    // Tells if there's more data available or not
    return readStatus;
}

//  END OF FILE
