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
* Description:  NativeHttpTransaction
*
*/


#include <e32def.h>
#include <http/rhttpresponse.h>
#include <http/rhttpheaders.h>
#include <httpstringconstants.h>
#include <http/thttphdrval.h>
#include <stringpool.h>
#include <e32svr.h>
#include <x509cert.h>
//#include <jdebug.h>

#include <jni.h>

#include "chttpscertinfo.h"
#include "nativehttptransaction.h"
#include "nativehttpsession.h"
#include "chttptransactionclient.h"

#include "javajniutils.h"
#include "s60commonutils.h"
#include "jstringutils.h"

#include "fs_methodcall.h"

using namespace java::util;

const TInt KResponseGranularity=10;

void CleanUpResetAndDestroy(TAny *aArray)
{
    if (aArray)
    {
        RPointerArray<HBufC>* array=(RPointerArray<HBufC>*)aArray;
        array->ResetAndDestroy();
        array->Close();
    }
}


NativeHttpTransaction::NativeHttpTransaction(HttpSessionClient& aSession, FunctionServer* aFuncServer)
        :iSessionClient(aSession) //, java::util::FunctionServer("MyhttpServer2")
{
  
    iFuncServer = aFuncServer;
}


/**
 * This uses the TConstructor class to ensure safe construction of the CJavaEventSource
 * subclass, and then returns the Java handle to the object directly, hence the
 * somewhat different NewL method.
 */
TInt NativeHttpTransaction::NewL(JNIEnv &aJni, jobject aPeer, HttpSessionClient& aSession,
                                 const TDesC* aUri, const TDesC* aRequestMethod,java::util::FunctionServer* aFuncServer)
{
    NativeHttpTransaction* self =   new(ELeave) NativeHttpTransaction(aSession,aFuncServer);

    
    self->ConstructL(aJni, aPeer, /*aServer, */ aUri, aRequestMethod);


    // Pop the cleanup of the object and create a handle:
    return reinterpret_cast<int>(self); //selfCleanup.GetHandle();
}

void NativeHttpTransaction::ConstructL(JNIEnv& aJni, jobject aPeer,/* TJavaEventServer aServer,*/
                                       const TDesC* aUri, const TDesC* aRequestMethod)
{


    iJniPeer = aJni.NewWeakGlobalRef(aPeer);


    // iFuncServer->attachToVm(aJni, aPeer);
    int handle = reinterpret_cast<int>(this);
    int urihandle = reinterpret_cast<int>(aUri);
    int methodhandle = reinterpret_cast<int>(aRequestMethod);
    //open the transaction
    
    CallMethodL(this, &NativeHttpTransaction::ExecuteCreateTransactionL,handle,urihandle , methodhandle, iFuncServer);
    
}

void NativeHttpTransaction::ExecuteCreateTransactionL(int aSelfhandle, int aUrihandle, int aMethodhandle)
{
    NativeHttpTransaction* aSelf = reinterpret_cast<NativeHttpTransaction*>(aSelfhandle);
    HBufC* aUri = reinterpret_cast<HBufC*>(aUrihandle);
    HBufC* aRequestMethod = reinterpret_cast<HBufC*>(aMethodhandle);

    aSelf->iTransactionClient = CHttpTransactionClient::NewL(aSelf->iSessionClient, aSelf, aUri, aRequestMethod);
}

void NativeHttpTransaction::SubmitL(JNIEnv* aJni, jobject* /*aPeer*/,const jobjectArray aHeaders, const jbyteArray aPostData, const jint aPostDataLength,
                                    int aResponseTimeout)
{

    //retrieve the headers
    RPointerArray<HBufC8> rawHeaderArray;
    CleanupStack::PushL(TCleanupItem(CleanUpResetAndDestroy,&rawHeaderArray));
    iJniObject = aJni;
    

    if (aHeaders!=NULL)
    {
        TInt count = aJni->GetArrayLength(aHeaders);
        if (count>KErrNone)
        {
            jstring jniString = NULL;
            HBufC8* narrowBuffer=NULL;
            TInt length=0;
            for (TInt ii=0; ii<count; ++ii)
            {
                jniString = (jstring)aJni->GetObjectArrayElement(aHeaders, ii);
                {
                    JStringUtils rawHeader(*aJni,jniString);
                    length=rawHeader.Length();

                    narrowBuffer = HBufC8::NewLC(length);
                    TPtr8 narrowPtr = narrowBuffer->Des();
                    narrowPtr.Copy(rawHeader);
                    rawHeaderArray.Append(narrowBuffer);
                    CleanupStack::Pop(narrowBuffer);
                }
                aJni->DeleteLocalRef(jniString);
            }
        }
    }
    //Convert the post data to native
    TInt offSet=0;
    HBufC8* postBuf=NULL;
    if (aPostDataLength>KErrNone)
    {
        postBuf=HBufC8::NewLC(aPostDataLength);
        TPtr8 bufPtr = postBuf->Des();
        User::LeaveIfError(S60CommonUtils::CopyToNative(*aJni , aPostData , offSet , aPostDataLength , bufPtr));
        CleanupStack::Pop(postBuf);
    }
    //Submit the transaction

    int handle = reinterpret_cast<int>(this);

    int arrayhandle = reinterpret_cast<int>(&rawHeaderArray);

    int hbufhandle = reinterpret_cast<int>(postBuf);

    CallMethodL(this, &NativeHttpTransaction::ExecuteSubmitL,handle,arrayhandle , hbufhandle, aResponseTimeout, iFuncServer);
    if (postBuf!=NULL)
    {
        delete postBuf;
        postBuf = NULL;
    }


    CleanupStack::PopAndDestroy();//rawHeaderArray;
}

void NativeHttpTransaction::ExecuteSubmitL(int aSelfhandle , int aRawHeadershandle , int aPostBufhandle, int aResponseTimeout)
{
    
    NativeHttpTransaction *aSelf = reinterpret_cast<NativeHttpTransaction*>(aSelfhandle);
    RPointerArray<HBufC8>* aRawHeaders = reinterpret_cast<RPointerArray<HBufC8>*>(aRawHeadershandle);
    HBufC8* aPostBuf = reinterpret_cast<HBufC8*>(aPostBufhandle);

    aSelf->iTransactionClient->SubmitL(aRawHeaders, aPostBuf, aResponseTimeout);
}

jobjectArray NativeHttpTransaction::GetResponseL(JNIEnv* aJni)
{
    jobjectArray objArray=NULL;
    RPointerArray<HBufC8> rawHeaders(KResponseGranularity);
    CleanupStack::PushL(TCleanupItem(CleanUpResetAndDestroy,&rawHeaders));
    
    int handle = reinterpret_cast<int>(this);

    int arrayhandle = reinterpret_cast<int>(&rawHeaders);
    CallMethodL(this, &NativeHttpTransaction::ExecuteGetResponseL,handle,arrayhandle , iFuncServer);
    
    const TInt headerCount = rawHeaders.Count();
    if (headerCount>KErrNone)
    {
        jclass stringClass = aJni->FindClass("java/lang/String");
        User::LeaveIfNull(stringClass);
        objArray = aJni->NewObjectArray(headerCount, stringClass, NULL);
        User::LeaveIfNull(objArray);

        HBufC* stringBuf = NULL;
        HBufC8* rawHeader = NULL;
        for (TInt i=0; i<headerCount; i++)
        {
            rawHeader= (rawHeaders)[i];
            stringBuf = HBufC::NewLC(rawHeader->Length());
            TPtr stringBufPtr = stringBuf->Des();
            stringBufPtr.Copy(*rawHeader);
            jstring string = S60CommonUtils::NativeToJavaString(*aJni, *stringBuf);
            aJni->SetObjectArrayElement(objArray, i, string);
            aJni->DeleteLocalRef(string);
            CleanupStack::PopAndDestroy(stringBuf);
        }
    }
    CleanupStack::PopAndDestroy();//rawHeaders
    return  objArray;
}


void NativeHttpTransaction::ExecuteGetResponseL(int aSelfhandle, int aRawHeadershandle)
{
    NativeHttpTransaction *aSelf = reinterpret_cast<NativeHttpTransaction*>(aSelfhandle);
    RPointerArray<HBufC8>* aRawHeaders = reinterpret_cast<RPointerArray<HBufC8>*>(aRawHeadershandle);
    aSelf->iTransactionClient->GetResponseL(aRawHeaders);
}
/*
* Returns the total number of bytes read into the buffer,
* or -1 if there is no more data because the end of the stream
* has been reached.
*/
TInt NativeHttpTransaction::ReadBytes(TUint8* aBytes, TInt aLength)
{
    
    int handle = reinterpret_cast<int>(this);
    int uinthandle = reinterpret_cast<int>(aBytes);

    int ret = 0;
    CallMethod(ret,this, &NativeHttpTransaction::ExecuteReadBytes,handle,uinthandle,aLength,iFuncServer);

    return ret;
    
}

TInt NativeHttpTransaction::ExecuteReadBytes(int aSelfhandle, int aByteshandle, TInt aLength)
{
    NativeHttpTransaction *aSelf = reinterpret_cast<NativeHttpTransaction*>(aSelfhandle);
    TUint8* aBytes = reinterpret_cast<TUint8*>(aByteshandle);

    TPtr8 bufferPtr(aBytes, aLength);
    return  aSelf->iTransactionClient->ReadBytes(bufferPtr);
}

void NativeHttpTransaction::SubmitComplete(TInt aStatus)
{
    LOG(ESOCKET,EInfo,"+NativeHttpTransaction::SubmitComplete");


    NativeHttpSession* session = reinterpret_cast<NativeHttpSession*>(iFuncServer);
    session->doSubmitCallback(aStatus,iJniPeer);

    LOG(ESOCKET,EInfo,"-NativeHttpTransaction::SubmitComplete");
}

void NativeHttpTransaction::DataReadyForRead(TInt aStatus)
{
    LOG(ESOCKET,EInfo,"+NativeHttpTransaction::DataReadyForRead");

    NativeHttpSession* session = reinterpret_cast<NativeHttpSession*>(iFuncServer);
    session->doReadCallback(aStatus,iJniPeer);

    LOG(ESOCKET,EInfo,"-NativeHttpTransaction::DataReadyForRead");
}

void NativeHttpTransaction::ExecuteCloseTransaction(int aSelfhandle)
{
    NativeHttpTransaction *aSelf = reinterpret_cast<NativeHttpTransaction*>(aSelfhandle);
    aSelf->iTransactionClient->CloseTransaction();
}

void NativeHttpTransaction::Dispose()
{
    LOG(ESOCKET,EInfo,"+NativeHttpTransaction::Dispose()");
    CloseTransaction();
    delete iTransactionClient;
    iTransactionClient=NULL;

    LOG(ESOCKET,EInfo,"-NativeHttpTransaction::Dispose()");
}


void NativeHttpTransaction::CloseTransaction()
{
    int handle = reinterpret_cast<int>(this);
    CallMethod(this, &NativeHttpTransaction::ExecuteCloseTransaction,handle,iFuncServer);
}



OS_EXPORT TInt NativeHttpTransaction::GetSecurityInfo()
{
    TInt handle=0;
    MNativeSecureConnectionInformation* security = iTransactionClient->GetSecurityInfo();
    if (security)
    {
        handle = reinterpret_cast<int>(security);
    }
    return handle;
}

/*
* Returns the amount of bytes avaible for read in our buffer.
*/
TInt NativeHttpTransaction::Available()
{
    return iTransactionClient->Available();
}



