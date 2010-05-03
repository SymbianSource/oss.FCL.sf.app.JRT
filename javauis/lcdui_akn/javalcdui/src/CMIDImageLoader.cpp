/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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

#include "CMIDImageLoader.h"
#include <jni.h>

const TInt KInitialBufferSize = 1024*32;    // 32kb

//
//
//
class CAsyncDecodeEvent : public CJavaEvent<CMIDImageLoader>
{
public:
    CAsyncDecodeEvent(jmethodID aMethod);

    void SetResult(jint aResult);
    void Dispatch(JNIEnv& aJni);

private:
    jmethodID   iMethod;
    jint        iResult;
};

CAsyncDecodeEvent::CAsyncDecodeEvent(jmethodID aMethod)
        : iMethod(aMethod)
        , iResult(KErrGeneral)
{
}

void CAsyncDecodeEvent::SetResult(jint aResult)
{
    iResult=aResult;
}

void CAsyncDecodeEvent::Dispatch(JNIEnv& aJni)
{
    jobject peerObject = aJni.NewLocalRef(Object().Peer());
    if (peerObject)
    {
        aJni.CallVoidMethod(peerObject, iMethod, iResult);
        aJni.DeleteLocalRef(peerObject);
    }
}

TInt CMIDImageLoader::New(JNIEnv& aJni, jobject aPeer, TJavaEventServer aServer, jint aToolkitHandle)
{
    TInt handle;
    TRAP(handle,
         TConstructor self(aJni);
         self->ConstructJniL(aJni,aPeer,aServer, JavaUnhand<CMIDToolkit>(aToolkitHandle));
         handle = self.GetHandle();
        );
    return handle;
}

CMIDImageLoader::CMIDImageLoader()
{
}

CMIDImageLoader::~CMIDImageLoader()
{
    ASSERT(NULL == iNotifyEvent);
    ASSERT(NULL == iDecoder);
    ASSERT(NULL == iBufferArray);
}

void CMIDImageLoader::ConstructJniL(JNIEnv& aJni, jobject aPeer, TJavaEventServer aServer, CMIDToolkit* aToolkit)
{
    CJavaEventSourceBase::ConstructL(aJni, aPeer, aServer);
    iBufferArray  = (TUint8*)User::AllocL(KInitialBufferSize);
    iBufferLength = KInitialBufferSize;
    iBufferCount  = 0;
    User::LeaveIfError(ExecuteTrap(InvokeConstructSvrL, this, aToolkit));
}

void CMIDImageLoader::InvokeConstructSvrL(CMIDImageLoader* aLoader, CMIDToolkit* aToolkit)
{
    aLoader->ConstructSvrL(*aToolkit);
}

void CMIDImageLoader::ConstructSvrL(CMIDToolkit& aToolkit)
{
    iDecoder = aToolkit.GraphicsFactory().NewImageDecoderL();
}

void CMIDImageLoader::FinalizeSvr()
{
    if (iDecoder)
    {
        iDecoder->Dispose();
        iDecoder = NULL;
    }
}

void CMIDImageLoader::FinalizeJni(JNIEnv& /*aJni*/)
{
    delete iNotifyEvent;
    iNotifyEvent = NULL;
    User::Free(iBufferArray);
    iBufferArray = NULL;
}

/**
 * Append data to decoder buffer. Currently we just extract the bytes from the java array
 * and do an ipc here for each block. We could choose buffer up client side until the buffer
 * is full and then flush. This would require keeping track of the byte count.
 */
TInt CMIDImageLoader::AppendJni(JNIEnv& aJni, jbyteArray aByteArray, jint aByteOffset, jint aByteCount)
{
    if (iBufferLength < aByteCount)
    {
        TUint8* array  = (TUint8*)User::ReAlloc(iBufferArray, aByteCount);
        if (array)
        {
            iBufferArray  = array;
            iBufferLength = aByteCount;
        }
        else
        {
            return KErrNoMemory;
        }
    }

    ASSERT(iBufferLength >= aByteCount);

    aJni.GetByteArrayRegion(aByteArray, aByteOffset, aByteCount, (jbyte*)(iBufferArray));

    //
    // No deferred flush yet.
    //
    ASSERT(iBufferCount == 0);
    iBufferCount = aByteCount;
    TInt err = ExecuteTrap(InvokeAppendSvrL, this);
    iBufferCount = 0;

    return err;
}

void CMIDImageLoader::AppendSvrL()
{
    TPtrC8 bufferPtr(iBufferArray, iBufferCount);
    iDecoder->AppendL(bufferPtr);
}

void CMIDImageLoader::InvokeAppendSvrL(CMIDImageLoader* aLoader)
{
    aLoader->AppendSvrL();
}

void CMIDImageLoader::InvokeAsyncDecodeSvrL(CMIDImageLoader* aLoader)
{
    aLoader->AsyncDecodeSvrL();
}

/**
 * Initiate asynchronous decode.
 */
TInt CMIDImageLoader::AsyncDecodeJni(JNIEnv& aJni)
{
    jobject peerObj = aJni.NewLocalRef(Peer());

    if (peerObj)
    {
        jclass    clazz  = aJni.GetObjectClass(peerObj);
        jmethodID method = aJni.GetMethodID(clazz,"signal","(I)V");
        aJni.DeleteLocalRef(peerObj);
        if (!iNotifyEvent)
        {
            iNotifyEvent = new CAsyncDecodeEvent(method);
        }
        if (!iNotifyEvent)
        {
            return KErrNoMemory;
        }
        return ExecuteTrap(InvokeAsyncDecodeSvrL, this);
    }
    return -1;
}

void CMIDImageLoader::AsyncDecodeSvrL()
{
    iDecoder->DecodeL(this);
}

/**
 *
 */
MMIDImageDecoder* CMIDImageLoader::Decoder()
{
    return iDecoder;
}

void CMIDImageLoader::DecodeComplete(TInt aError)
{
    CAsyncDecodeEvent* event = iNotifyEvent;
    iNotifyEvent = NULL;
    ASSERT(event);
    event->SetResult(aError);
    PostEvent(event,CJavaEventBase::ENotifyPriority);
}
