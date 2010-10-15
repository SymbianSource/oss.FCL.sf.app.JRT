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

#ifndef NATIVEHTTPSESSION_H
#define NATIVEHTTPSESSION_H

#include <e32def.h>
#include <jni.h>
#include "http/rhttpsession.h"
#include "functionserver.h"
#include "monitor.h"
using namespace java::util;

class CJavaValidationFilter;
class MMidHttpProxyPlugin;
class HttpSessionClient;

class NativeHttpSession  : public FunctionServer //CJavaEventSource<NativeHttpSession>
{
public:

    static TInt NewL(JNIEnv& aJni, jobject aPeer,/* TJavaEventServer aServer,*/ TInt aSuiteId, TInt aAPNId, TInt *err, TInt * apnerr);
    TInt CreateTransactionL(JNIEnv* aJni, jobject aPeer, jstring aUri, jstring aRequestMethod);
    //RHTTPSession& Session();
    //void doSubmitCallback(TInt aStatus);
    void doSubmitCallback(TInt aStatus,jobject &aPeer);
    //void doReadCallback(TInt aStatus);
    void doReadCallback(TInt aStatus,jobject &aPeer);
    void doDataConsumedCallback(jobject &aPeer);
    void vmAttached();

private:
    NativeHttpSession();


    TInt ConstructL(JNIEnv& aJni, jobject aPeer, /*TJavaEventServer aServer,*/TInt aType, TInt aAPNId,  TInt * apnerr);
private:

    TInt ExecuteCreateHttpSessionL(int aSelf, TInt aType,  TInt aAPNId, TInt* apnerr);

private:
    HttpSessionClient* iHttpSessionClient;
    TInt                iSuiteId;
    jmethodID   iSubmitCallbackMethodID;
    jmethodID   iReadCallbackMethodID;
    jmethodID   iPostCallbackMethodID;
public:
    java::util::Monitor *iMonitor;
};

#endif // NATIVEHTTPSESSION_H
