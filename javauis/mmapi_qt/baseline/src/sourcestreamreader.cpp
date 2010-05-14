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


//#include "jutils.h"
//#include "mmapiutils.h"
#include "com_nokia_microedition_media_protocol_SourceStreamReader.h"
#include "cmmasourcestream.h"
#include "mmafunctionserver.h"

// If eventsource is already disposed, then do nothing
#define CHECK_HANDLE(x, j) { if ( !( x ) || ( ( x )->Players().Count() == 0 ) ) { return ( j ); } }
LOCAL_C void WriteDataL(TInt aSourceStream,
                        TInt aPlayer,
                        MMAFunctionServer* aEventSource,
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
    	reinterpret_cast< CMMASourceStream *>(aSourceStream);

    sourceStream->WriteL(aData, aLength, aState);
    LOG( EJavaMMAPI, EInfo, "jni SourceStreamReader.cpp: WriteDataL() -");
}

JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_protocol_SourceStreamReader__1write
(JNIEnv* aJni, jclass, jint aSourceStream, jint aEventSource,
 jbyteArray aData, jint aLength, jint aState, jint aPlayerHandle)
{
	 LOG( EJavaMMAPI, EInfo, "SourceStreamReaderjni_write");
    MMAFunctionServer* eventSource =
		reinterpret_cast< MMAFunctionServer* >(aEventSource);

    CHECK_HANDLE(eventSource, KErrNone);

    jbyte* data = aJni->GetByteArrayElements(aData, NULL);
	LOG2( EJavaMMAPI, EInfo, "SourceStreamReaderjni_write , length = %d,state = %d",(TInt)aLength,(TInt)aState);
    TInt err = eventSource->ExecuteTrap(&WriteDataL,
                                        aSourceStream,
                                        aPlayerHandle,
                                        eventSource,
                                        (const TUint8*)data,
                                        aLength,
                                        aState);
    ELOG1( EJavaMMAPI, "SourceStreamReaderjni_write , err = %d",err);
    aJni->ReleaseByteArrayElements(aData, data, JNI_ABORT);
    return err;
}

// END OF FILE
