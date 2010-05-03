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

#ifndef CMIDIMAGELOADER_H
#define CMIDIMAGELOADER_H

#include <mevents.h>
#include <fbs.h>
#include "CMIDToolkit.h"

//
//
//
class CAsyncDecodeEvent;
class CMIDImageLoader : public CJavaEventSource<CMIDImageLoader>, private MMIDImageObserver
{
public:
    static TInt New(JNIEnv& aJni, jobject aPeer, TJavaEventServer aServer, jint aToolkitHandle);

    CMIDImageLoader();
    void ConstructJniL(JNIEnv& aJni, jobject aPeer, TJavaEventServer aServer, CMIDToolkit*);
    void ConstructSvrL(CMIDToolkit& aToolkit);
    void FinalizeSvr();
    void FinalizeJni(JNIEnv& aJni);
    ~CMIDImageLoader();

    MMIDImageDecoder* Decoder();

public:
    /**
     * Append data to decoder buffer.
     */
    TInt AppendJni(JNIEnv& aJni, jbyteArray aByteArray, jint aByteOffset, jint aByteCount);

    /**
     * Initiate asynchronous decode.
     */
    TInt AsyncDecodeJni(JNIEnv& aJni);

private:
    static void InvokeAppendSvrL(CMIDImageLoader*);
    static void InvokeAsyncDecodeSvrL(CMIDImageLoader*);
    static void InvokeConstructSvrL(CMIDImageLoader*, CMIDToolkit*);

private:
    void AppendSvrL();
    void AsyncDecodeSvrL();
    void DecodeComplete(TInt aError);

private:
    CAsyncDecodeEvent*  iNotifyEvent;
    MMIDImageDecoder*   iDecoder;
    TUint8*             iBufferArray;
    TInt                iBufferLength;
    TInt                iBufferCount;
};

#endif // CMIDIMAGELOADER_H
