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
* Description:  NativeHttpSession
*
*/


#include "nativehttpsession.h"
#include "nativehttptransaction.h"
#include <http/thttpfilteriterator.h>
#include "httpsessionclient.h"
#include <e32svr.h>
#include "logger.h"
#include <flogger.h>

#include "javajniutils.h"

using namespace java::util;

#include "s60commonutils.h"
#include "fs_methodcall.h"


NativeHttpSession::NativeHttpSession(): java::util::FunctionServer("Myhttp1Server")
{
    JELOG2(ESOCKET);
    createServerToNewThread();
    iMonitor = java::util::Monitor::createMonitor();
}


NativeHttpSession::~NativeHttpSession()
{
    JELOG2(ESOCKET);

    //delete iHttpSessionClient;
    TInt handle = reinterpret_cast<TInt>(this);

    CallMethod(this, &NativeHttpSession::ExecuteCloseHttpSessionL,handle, this);

    if (iMonitor != NULL)
        iMonitor->wait();

    iHttpSessionClient = NULL;
    stopServer();

    delete iMonitor;
    iMonitor = NULL;

}

void NativeHttpSession::ExecuteCloseHttpSessionL(int /*aSelfhandle*/)
{
    if (iHttpSessionClient !=NULL)
    {
        delete iHttpSessionClient;
        iHttpSessionClient = NULL;
    }

    iMonitor->notify();

}
/**
 * This uses the TConstructor class to ensure safe construction of the CJavaEventSource
 * subclass, and then returns the Java handle to the object directly, hence the
 * somewhat different NewL method.
 */

TInt NativeHttpSession::NewL(JNIEnv& aJni, jobject aPeer,/* TJavaEventServer aServer,*/ TInt aType, TInt aAPNId, TInt * aErr, TInt * apnerr)
{
    NativeHttpSession* self =   new(ELeave) NativeHttpSession();
    // Cleanup the event source object if a leave occurs during construction:
    //TConstructor selfCleanup(self, aJni);
//  FeatureManager::InitializeLibL();
    // TBool isSupported = false; //FeatureManager::FeatureSupported( KFeatureIdJavaApiEnhancementsIap );//KFeatureIdJavaAPIEnhancementsIAP
//   FeatureManager::UnInitializeLib();
    //  if( isSupported )
    //      {
    *aErr = self->ConstructL(aJni, aPeer  /* aServer*/, aType, aAPNId, apnerr);
    LOG1(ESOCKET,EInfo,"NativeHttpSession::NewL - aErr : %d",*aErr);
    //    }
    //else
    //    {
    //  self->ConstructL(aJni, aPeer, /*aServer,*/ aType, -1 );
    // }
    self->attachToVm(aJni,aPeer);
    // Pop the cleanup of the object and create a handle:
    return reinterpret_cast<TInt>(self); //selfCleanup.GetHandle();
}

void NativeHttpSession::vmAttached()
{
    LOG(ESOCKET,EInfo,"+vmAttached");
    jclass httpNativeClass = NULL;
    httpNativeClass = mJniEnv->FindClass("com/nokia/mj/impl/http/HttpConnectionNative");

    iSubmitCallbackMethodID = NULL;
    iSubmitCallbackMethodID = mJniEnv->GetMethodID(httpNativeClass, "transactionSubmitCallback", "(I)V");

    iReadCallbackMethodID = NULL;
    iReadCallbackMethodID = mJniEnv->GetMethodID(httpNativeClass, "dataReadyForReadCallBack", "(I)V");

    // mJniEnv->CallVoidMethod(mJavaPeerObject,iSubmitCallbackMethodID,1);

    /*  httpNativeClass = mJniEnv->FindClass("com/nokia/mj/impl/gcf/protocol/http/Protocol");
       if ( httpNativeClass == NULL )
        {
           LOG(ESOCKET,EInfo,"+httpNativeClass is null");
        }
    iSubmitCallbackMethodID = NULL;
      iSubmitCallbackMethodID = mJniEnv->GetMethodID( httpNativeClass, "temp", "(I)V" );
      if ( iSubmitCallbackMethodID == NULL )
        {
           LOG(ESOCKET,EInfo,"+iMethodID is NULL");
        }
         mJniEnv->CallVoidMethod(mJavaPeerObject,iSubmitCallbackMethodID,1);*/
    LOG(ESOCKET,EInfo,"-vmAttached");
}

TInt NativeHttpSession::ConstructL(JNIEnv& /*aJni*/, jobject /*aPeer*/,/* TJavaEventServer aServer, */ TInt aType, TInt aAPNId, TInt * apnerr)
{
//  CJavaEventSourceBase::ConstructL(aJni, aPeer, aServer);
    //iSuiteId = aSuiteId;

    TInt err;
    TInt handle = reinterpret_cast<TInt>(this);
    CallMethodL(err, this, &NativeHttpSession::ExecuteCreateHttpSessionL,handle, aType, aAPNId,apnerr, this);

    return err;
}

TInt NativeHttpSession::CreateTransactionL(JNIEnv* aJni, jobject aPeer, jstring aUri,  jstring aRequestMethod)
{
    std::wstring uri2 = JniUtils::jstringToWstring(aJni,aUri);
    std::wstring  req2 = JniUtils::jstringToWstring(aJni,aRequestMethod);

    HBufC* t1 = S60CommonUtils::wstringToDes(uri2.c_str());
    HBufC* t2 = S60CommonUtils::wstringToDes(req2.c_str());

    //  RJString uri( *aJni, aUri );
    // RJString reqMethod( *aJni, aRequestMethod );
    //TInt handle= NativeHttpTransaction::NewL(*aJni,aPeer, Server(), *iHttpSessionClient, (const TDesC*)&uri, (const TDesC*)&reqMethod);
    TInt handle= NativeHttpTransaction::NewL(*aJni,aPeer,/* NULL, */*iHttpSessionClient, (const TDesC*)t1, (const TDesC*)t2,this);
    return handle;
}

TInt NativeHttpSession::ExecuteCreateHttpSessionL(int aSelfhandle, TInt aType, TInt aAPNId, TInt*  apnerr)
{
    LOG(ESOCKET,EInfo,"+ExecuteCreateHttpSessionL");
    TInt err = 0;
    NativeHttpSession *aSelf = reinterpret_cast<NativeHttpSession*>(aSelfhandle);

    LOG1(ESOCKET,EInfo,"NativeHttpSession uses APN: %d", aAPNId);
    if (aAPNId != -1)
    {
        LOG1(ESOCKET,EInfo,"NativeHttpSession creates client with APN: %d", aAPNId);
        TRAP(err,aSelf->iHttpSessionClient  = HttpSessionClient::NewL(aType, aAPNId, apnerr));
    }
    else
    {
        LOG(ESOCKET,EInfo,"NativeHttpSession creates client no APN");
        TRAP(err,aSelf->iHttpSessionClient  = HttpSessionClient::NewL(-1,-1, apnerr));
    }

    LOG1(ESOCKET,EInfo,"ExecuteCreateHttpSessionL : err :  %d",err);
    LOG(ESOCKET,EInfo,"-ExecuteCreateHttpSessionL ");
    return err;
}

//RHTTPSession&  NativeHttpSession::Session()
//  {
//  return iHttpSessionClient->Session();
//  }




void NativeHttpSession::doSubmitCallback(TInt aStatus,jobject &aPeer)
{
    LOG(ESOCKET,EInfo,"+doSubmitCallback1");
    //mJniEnv->CallVoidMethod(mJavaPeerObject,iSubmitCallbackMethodID,aStatus);
    mJniEnv->CallVoidMethod(aPeer,iSubmitCallbackMethodID,aStatus);
    LOG(ESOCKET,EInfo,"-doSubmitCallback1");
}

void NativeHttpSession::doReadCallback(TInt aStatus,jobject &aPeer)
{
    LOG(ESOCKET,EInfo,"+doReadCallback1");
//  mJniEnv->CallVoidMethod(mJavaPeerObject,iReadCallbackMethodID,aStatus);
    mJniEnv->CallVoidMethod(aPeer,iReadCallbackMethodID,aStatus);
    LOG(ESOCKET,EInfo,"-doReadCallback1");
}

