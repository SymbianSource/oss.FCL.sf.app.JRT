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
* Description:  This class has JNI wrappers for CMMASourceStream
*
*/


#include "jutils.h"
#include "com_nokia_microedition_media_protocol_SourceStreamReader.h"
#include "cmmasourcestream.h"
#include "cmmaeventsource.h"

// If eventsource is already disposed, then do nothing
#define CHECK_HANDLE(x, j) { if ( !( x ) || ( ( x )->Players().Count() == 0 ) ) { return ( j ); } }
LOCAL_C void WriteDataL(TInt aSourceStream,
                        TInt aPlayer,
                        CMMAEventSource* aEventSource,
                        const TUint8* aData,
                        TInt aLength,
                        TInt aState)
{
    if (!aEventSource->FindPlayer(aPlayer))
    {
        // Native object was already deleted
        User::Leave(KErrBadHandle);
    }
    CMMASourceStream* sourceStream =
        JavaUnhand< CMMASourceStream >(aSourceStream);

    sourceStream->WriteL(aData, aLength, aState);
}

JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_protocol_SourceStreamReader__1write
(JNIEnv* aJni, jclass, jint aSourceStream, jint aEventSource,
 jbyteArray aData, jint aLength, jint aState, jint aPlayerHandle)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSource);

    CHECK_HANDLE(eventSource, KErrNone);

    jbyte* data = aJni->GetByteArrayElements(aData, NULL);

    TInt err = eventSource->ExecuteTrap(&WriteDataL,
                                        aSourceStream,
                                        aPlayerHandle,
                                        eventSource,
                                        (const TUint8*)data,
                                        aLength,
                                        aState);

    aJni->ReleaseByteArrayElements(aData, data, JNI_ABORT);
    return err;
}

// END OF FILE
