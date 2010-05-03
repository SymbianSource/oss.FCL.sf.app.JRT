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
* Description:
 *
*/


//  INCLUDE FILES
#include <stdlib.h>
#include <string.h>

#include "javax_microedition_securityservice_CMSMessageSignatureService.h"
#include "cstssignatureservice.h"
#include "satsajnitools.h"
#include "logger.h"
#include "javajniutils.h"
#include "jniarrayutils.h"
#include "s60commonutils.h"



JNIEXPORT jint JNICALL
Java_javax_microedition_securityservice_CMSMessageSignatureService__1createSignatureService(
    JNIEnv* /* aJniEnv */,
    jobject /* aPeer */)
{
    LOG(ESATSA, EInfo, "+ Entering JNI function __1createSignatureService");
    CSTSSignatureService* service = NULL;
    TInt serviceHandle = KErrGeneral;
    TRAPD(error, service = CSTSSignatureService::NewL());
    if (service == NULL || KErrNone != error)
    {
        return error;
    }
    serviceHandle = reinterpret_cast<TInt>(service);
    LOG(ESATSA, EInfo, "--  __1createSignatureService");
    return serviceHandle;

}

JNIEXPORT jbyteArray
JNICALL Java_javax_microedition_securityservice_CMSMessageSignatureService__1authenticate(
    JNIEnv* aJniEnv,
    jobject /* aPeer */,
    jint aServiceHandle,
    jbyteArray aByteArrayToAuthenticate, jint aOptions, jobjectArray aCaNames,
    jstring aSecurityElementPrompt, jboolean aShowData, jintArray aError)
{
    LOG(ESATSA, EInfo, "+ Entering JNI function __1authenticate");
    CSTSSignatureService* service =
        reinterpret_cast<CSTSSignatureService *>(aServiceHandle);

    HBufC8* retVal = NULL;
    int ret=0;

    TRAPD(err, ret = CSTSSignatureService::StaticAuthenticateL(aJniEnv,
                     service, aByteArrayToAuthenticate, aOptions, aCaNames,
                     aSecurityElementPrompt, aShowData, &retVal));
    LOG(ESATSA, EInfo, "__1authenticate:After the call to StaticAuthenticateL");

    STSSetJavaErrorCode(aJniEnv, aError, ret);
    if ((ret != KErrNone) || (!retVal) || (KErrNone != err))
    {
        LOG(ESATSA, EInfo, "__1authenticate:Error !! ");
        return NULL;
    }

    const TInt numElems = retVal->Length();
    jbyteArray javaValue = aJniEnv->NewByteArray(numElems);

    if (!javaValue)
    {
        LOG(ESATSA, EInfo, "__1authenticate:No Memory !! ");
        STSSetJavaErrorCode(aJniEnv, aError, KErrNoMemory);
        delete retVal;
    }

    if (numElems> 0)
    {
        LOG(ESATSA, EInfo, "__1authenticate:COpy to Java");
        aJniEnv->SetByteArrayRegion(
            javaValue, 0, numElems,
            JAVA_PTR(retVal->Ptr()));
    }

    delete retVal;
    LOG(ESATSA, EInfo, "-- __1authenticate");
    return javaValue;
}

JNIEXPORT jbyteArray
JNICALL Java_javax_microedition_securityservice_CMSMessageSignatureService__1sign(
    JNIEnv* aJniEnv,
    jobject /* aPeer */,
    jint aServiceHandle,
    jbyteArray aByteArrayToSign, jint aOptions, jobjectArray aCaNames,
    jstring aSecurityElementPrompt, jintArray aError)
{
    LOG(ESATSA, EInfo, "+ Entering JNI function __1sign");

    CSTSSignatureService* service =
        reinterpret_cast<CSTSSignatureService *>(aServiceHandle);

    HBufC8* retVal = NULL;
    int ret=0;
    TRAPD(err, ret = CSTSSignatureService::StaticSignL(aJniEnv, service, aByteArrayToSign, aOptions,
                     aCaNames, aSecurityElementPrompt, &retVal));
    LOG(ESATSA, EInfo, "__1sign:After the call to StaticSignL");

    STSSetJavaErrorCode(aJniEnv, aError, ret);
    if ((KErrNone != ret) || (!retVal) || (KErrNone != err))
    {
        return NULL;
    }
    const TInt numElems = retVal->Length();
    jbyteArray javaValue = aJniEnv->NewByteArray(numElems);

    if (!javaValue)
    {
        STSSetJavaErrorCode(aJniEnv, aError, KErrNoMemory);
        delete retVal;
    }

    if (numElems> 0)
    {
        aJniEnv->SetByteArrayRegion(
            javaValue, 0, numElems,
            JAVA_PTR(retVal->Ptr()));
    }
    delete retVal;
    LOG(ESATSA, EInfo, "--returning __1sign");
    return javaValue;
}
