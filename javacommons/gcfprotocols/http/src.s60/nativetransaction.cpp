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
* Description:  NativeTransaction - all jni calls made through here
*
*/


#include <e32def.h>
#include <centralrepository.h>

#ifdef RD_JAVA_S60_RELEASE_10_1_ONWARDS
#include <CUserAgent.h>
#else
#include <cuseragent.h>
#endif

#include "com_nokia_mj_impl_http_HttpConnectionNative.h"
#include "nativehttptransaction.h"
#include "nativehttpsession.h"
#include "monitor.h"
#include "logger.h"
#include "s60commonutils.h"

using namespace java::util;

// Constants for default User-Agent Header
const TUid KCRUidJ2MEConfiguration = {0x102823D3};
const TUint KHTTPUserAgentHeader = 2;
const TUint KHTTPUserAgentHeaderValue = 3;
const TInt KHTTPUserAgentS60Header = 0;
const TInt KHTTPUserAgentBrowserHeader = 1;
const TInt KHTTPUserAgentDefinedHeader = 2;
const TInt KMaxLength = 1024;

// Forward declaration
jstring GetUserAgentL(JNIEnv *aJni, jboolean aMidpRuntime);

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_http_HttpConnectionNative__1createHttpSession(JNIEnv* aJni, jobject aPeer, jint /*aCommsServerHandle*/, jint aType, jint aAPNId, jintArray  aReturnValue)
{
    TInt err = -1, apnerr = 0;

    TRAPD(handle,   handle = NativeHttpSession::NewL(*aJni, aPeer,  aType, aAPNId, &err, &apnerr));
    ILOG1(ESOCKET, "createsession returned %d ",err);
    ILOG1(ESOCKET, "apn setting returned %d ",apnerr);
    aJni->SetIntArrayRegion(aReturnValue,0,1,&err);
    aJni->SetIntArrayRegion(aReturnValue,1,1,&apnerr);

    //DEBUGCOMP_INT( KLogFileHttp ,"Java_com_symbian_midp_io_protocol_http_Protocol__1CreateHttpSession: handle = %d", handle );
    return handle;
}

JNIEXPORT void JNICALL Java_com_nokia_mj_impl_http_HttpConnectionNative__1closeTransaction(
    JNIEnv* aJni,
    jobject aPeer,
    jint aNativeHttpTransaction)
{

    NativeHttpTransaction* tran = reinterpret_cast<NativeHttpTransaction*>(aNativeHttpTransaction);
    tran->iJniPeer = aJni->NewWeakGlobalRef(aPeer);
    try
    {
        tran->Dispose();
    }
    catch (...)
    {
        // function server usage may throw an exception.
        // ignore, called when transcation is closed
        ELOG(ESOCKET,"Http JNI Error, exception caught!: _closeTransaction");
    }
    delete tran;
}

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_http_HttpConnectionNative__1createNativeTransaction(
    JNIEnv* aJni,
    jobject aPeer,
    jint aNativeHttpSession,
    jstring aUri,
    jstring aRequestMethod)
{
    LOG(ESOCKET,EInfo,"http jni _createNativeTransaction()");
    NativeHttpSession* session = reinterpret_cast<NativeHttpSession*>(aNativeHttpSession);
    //tran->iJniPeer = aJni->NewGlobalRef(aPeer);
    try
    {
        TRAPD(handle,   handle = session->CreateTransactionL(aJni, aPeer , aUri, aRequestMethod););
        return handle;
    }
    catch (...)
    {
        // function server usage may throw an exception.
        ELOG(ESOCKET,"Http JNI Error, exception caught!: _createTransaction");
        return -1;
    }

}

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_http_HttpConnectionNative__1submitTransaction(
    JNIEnv* aJni,
    jobject aPeer,
    jint aNativeHttpTransaction,
    jobjectArray aHeaders,
    jbyteArray aPostData,
    jint aPostDataLength,
    jint aResponseTimeout)
{
    LOG(ESOCKET,EInfo,"http jni _submitTransaction");
    NativeHttpTransaction* tran = reinterpret_cast<NativeHttpTransaction*>(aNativeHttpTransaction);
    int respTimeOut = aResponseTimeout;
    tran->iJniPeer = aJni->NewWeakGlobalRef(aPeer);
    try
    {
        TRAPD(err,tran->SubmitL(aJni, &aPeer,aHeaders, aPostData, aPostDataLength, respTimeOut));
        return err;
    }
    catch (...)
    {
        ELOG(ESOCKET,"Http JNI Error, exception caught!: _submitTransaction");
        return -1;
    }
}

JNIEXPORT jobjectArray JNICALL Java_com_nokia_mj_impl_http_HttpConnectionNative__1getResponse(
    JNIEnv* aJni,
    jobject aPeer,
    jint aNativeHttpTransaction)
{
    LOG(ESOCKET,EInfo,"http jni _getResponse");
    jobjectArray rawHeaders=NULL;
    NativeHttpTransaction* tran = reinterpret_cast<NativeHttpTransaction*>(aNativeHttpTransaction);
    tran->iJniPeer = aJni->NewWeakGlobalRef(aPeer);
    try
    {
        TRAPD(err, rawHeaders =  tran->GetResponseL(aJni));
        if (err!=KErrNone)
        {
            rawHeaders=NULL;
        }
    }
    catch (...)
    {
        rawHeaders=NULL;
        ELOG(ESOCKET,"Http JNI Error, exception caught!: _getResponse");

    }
    return rawHeaders;
}


JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_http_HttpConnectionNative__1getBytes(
    JNIEnv*     aEnv,
    jobject aPeer,
    jint        aNativeHttpTransaction,
    jbyteArray  aBytes,
    jint        aLength)
{
    LOG(ESOCKET,EInfo,"http jni _getBytes");
    jbyte* bytes = aEnv->GetByteArrayElements(aBytes, NULL);

    if (bytes == NULL)
    {
        return -1;
    }
    try
    {
        NativeHttpTransaction* tran = reinterpret_cast<NativeHttpTransaction*>(aNativeHttpTransaction);
        tran->iJniPeer = aEnv->NewWeakGlobalRef(aPeer);
        TInt length = tran->ReadBytes(reinterpret_cast<TUint8*>(bytes), aLength);

        aEnv->ReleaseByteArrayElements(aBytes, bytes, NULL);
        return length;
    }
    catch (...)
    {
        ELOG(ESOCKET,"Http JNI Error, exception caught!: _getBytes");
        return -1;
    }
}

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_http_HttpConnectionNative__1available(
    JNIEnv* aJni,
    jobject aPeer,
    jint aNativeHttpTransaction)
{
    NativeHttpTransaction* tran = reinterpret_cast<NativeHttpTransaction*>(aNativeHttpTransaction);
    tran->iJniPeer = aJni->NewWeakGlobalRef(aPeer);
    try
    {
        return tran->Available();
    }
    catch (...)
    {
        ELOG(ESOCKET,"Http JNI Error, exception caught!: _available");
        return -1;
    }
}

JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_http_HttpConnectionNative__1getUserAgentHeaderValue(
    JNIEnv *aJni,
    jobject, jboolean aMidpRuntime)
{
    jstring header = NULL;
    TRAPD(err, header = GetUserAgentL(aJni, aMidpRuntime));
    if (err != KErrNone)
    {
        header = NULL;
    }
    return header;
}

jstring GetUserAgentL(JNIEnv *aJni, jboolean aMidpRuntime)
{

    jstring header = NULL;

    if (aMidpRuntime == false)
    {
        CUserAgent* userAgent = CUserAgent::NewL();
        CleanupStack::PushL(userAgent);

        HBufC8* agent8 = userAgent->UserAgentL();
        CleanupStack::PushL(agent8);
        HBufC* agent = HBufC::NewMaxLC(agent8->Length());
        agent->Des().Copy(*agent8);
        header = S60CommonUtils::NativeToJavaString(*aJni, agent->Des());

        CleanupStack::PopAndDestroy(agent);
        CleanupStack::PopAndDestroy(agent8);
        CleanupStack::PopAndDestroy(userAgent);

        return header;
    }

    CRepository* repository = NULL;

    repository = CRepository::NewL(KCRUidJ2MEConfiguration);

    if (repository)
    {
        CleanupStack::PushL(repository);

        TInt headerType;
        TInt ret = repository->Get(KHTTPUserAgentHeader, headerType);
        if (ret == KErrNone)
        {
            switch (headerType)
            {
            case KHTTPUserAgentBrowserHeader:
            {
                CUserAgent* userAgent = CUserAgent::NewL();
                CleanupStack::PushL(userAgent);

                HBufC8* agent8 = userAgent->UserAgentL();
                CleanupStack::PushL(agent8);
                HBufC* agent = HBufC::NewMaxLC(agent8->Length());
                agent->Des().Copy(*agent8);
                header = S60CommonUtils::NativeToJavaString(*aJni, agent->Des());

                CleanupStack::PopAndDestroy(agent);
                CleanupStack::PopAndDestroy(agent8);
                CleanupStack::PopAndDestroy(userAgent);
            }
            break;

            case KHTTPUserAgentDefinedHeader:
            {
                HBufC* headerValue = HBufC::NewLC(KMaxLength);
                TPtr ptr(headerValue->Des());
                ret = repository->Get(KHTTPUserAgentHeaderValue, ptr);
                header = S60CommonUtils::NativeToJavaString(*aJni, headerValue->Des());
                CleanupStack::PopAndDestroy(headerValue);
            }
            break;

            case KHTTPUserAgentS60Header:
            default:
                header = NULL;
                break;
            }
        }

        CleanupStack::PopAndDestroy(repository);
    }

    return header;
}
