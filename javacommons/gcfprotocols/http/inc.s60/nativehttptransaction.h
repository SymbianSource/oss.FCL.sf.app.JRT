/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef NATIVEHTTPTRANSACTION_H
#define NATIVEHTTPTRANSACTION_H
#include <jni.h>
#include <e32cmn.h>
#include <e32def.h>
#include <mrefhttpclientobserver.h>
#include "functionserver.h"

//#include <mevents.h> // CJavaEventSource

class CX509Certificate;
class NativeHttpSession;
class CHttpsCertInfo;
class CHttpTransactionClient;
class HttpSessionClient;
//class CJavaEventSource;

using namespace java::util;

class NativeHttpTransaction :    public MRefHttpClientObserver //, public FunctionServer
{
public:
    static TInt NewL(JNIEnv& aJni, jobject aPeer,  HttpSessionClient& aSessionClient, const TDesC* aUri,  const TDesC* aRequestMethod,FunctionServer* aFuncServer);
    /*
    * After the headers and other details have been set up,
    * call this function to make the request.
    */
    void SubmitL(JNIEnv* aJni, jobject * aPeer,const jobjectArray aHeaders, const jbyteArray aPostData, const jint aPostDataLength, int aResponseTimeout);
    /*
    * Get the response headers back from the transaction
    */
    jobjectArray GetResponseL(JNIEnv* aJni);
    /*
    * Call from Jave to attempt to read aLength of Bytes
    * Returns the total number of bytes read into the buffer,
    * or -1 if there is no more data because the end of the stream
    * has been reached.
    * Returns 0 if native buffer is empty but still more data to read;
    */
    TInt ReadBytes(TUint8* aBytes, TInt aLength);
    /*
    * Close the transaction and dispose
    */
    void Dispose();
    /*
    *   CloseTransaction
    */
    void CloseTransaction();
    /*
    * Get the secutiry information
    */
    OS_IMPORT TInt GetSecurityInfo();
    /*
    * Returns the amount of bytes avaible for read in our buffer.
    */
    TInt Available();
private: //from MRefHttpClientObserver
    void SubmitComplete(TInt aStatus);
    void DataReadyForRead(TInt aStatus);
private:
    NativeHttpTransaction(HttpSessionClient& aSession,FunctionServer* aFuncServer);
    void ConstructL(JNIEnv& aJni, jobject aPeer,/* TJavaEventServer aServer,*/ const TDesC* aUri, const TDesC* aRequestMethod);

private:
    //static void ExecuteCreateTransactionL(NativeHttpTransaction* aSelf, const TDesC* aUri, const TDesC* aRequestMethod);
    void ExecuteCreateTransactionL(int,int,int);
    void ExecuteSubmitL(int aSelfhandle , int aRawHeadershandle , int aPostBufhandle, int aResponseTimeout);
    //void ExecuteSubmitL(NativeHttpTransaction* aSelf , RPointerArray<HBufC8>* aRawHeaders, HBufC8* aPostBuf );
    //static void ExecuteGetResponseL(NativeHttpTransaction* aSelf, RPointerArray<HBufC8>* aRawHeaders);
    void ExecuteGetResponseL(int aSelf,int aRawHeaders);
    TInt ExecuteReadBytes(int aSelf, int aBytes, int aLength);
    //TInt ExecuteReadBytes(NativeHttpTransaction* aSelf, TUint8* aBytes, TInt aLength);
    //static void ExecuteCloseTransaction(NativeHttpTransaction* aSelf);
    void ExecuteCloseTransaction(int aSelfhandle);

private:
    HttpSessionClient& iSessionClient;
    CHttpTransactionClient* iTransactionClient;
    jmethodID iTransactionCallbackMethod;
    jmethodID iNotifyDataReadyForReadMethod;
    FunctionServer* iFuncServer;
    JNIEnv * iJniObject;

public:
    jobject  iJniPeer;
};

#endif // NATIVEHTTPTRANSACTION_H
