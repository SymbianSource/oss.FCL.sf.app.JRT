/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include "javajniutils.h"
#include "com_nokia_mj_impl_security_midp_authentication_OcspChecker.h"
#include "ocspclient.h"

using namespace java::security;
using namespace std;

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_security_midp_authentication_OcspChecker__1createNativeThread
(JNIEnv *, jobject)
{
    return 0;
}

JNIEXPORT void JNICALL Java_com_nokia_mj_impl_security_midp_authentication_OcspChecker__1destroyNativeThread
(JNIEnv *, jobject, jint)
{
}

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_security_midp_authentication_OcspChecker__1createNativePeer
(JNIEnv * env, jobject, jint aNativeThreadHandle, jlong iap, jlong snap, jstring jOcspDefaultUrl)
{
    OcspClient * ocspClient = NULL;
    const char* ocspDefaultUrl = NULL;
    if (jOcspDefaultUrl != NULL)
    {
        jboolean isCopy;
        ocspDefaultUrl = env->GetStringUTFChars(jOcspDefaultUrl, &isCopy);
        ocspClient = OcspClient::createInstance(iap, ocspDefaultUrl);
    }
    else
    {
        ocspClient = OcspClient::createInstance(iap, NULL);
    }
    return reinterpret_cast<int>(ocspClient);
}

JNIEXPORT void JNICALL Java_com_nokia_mj_impl_security_midp_authentication_OcspChecker__1destroyNativePeer
(JNIEnv *, jobject, jint aNativeThreadHandle, jint aNativePeerHandle)
{
    OcspClient* ocspClient =
        reinterpret_cast< OcspClient* >(aNativePeerHandle);
    delete ocspClient;
}

JNIEXPORT void JNICALL Java_com_nokia_mj_impl_security_midp_authentication_OcspChecker__1ocspChecks
(JNIEnv * env, jobject, jint aNativeThreadHandle, jint aNativePeerHandle, jobjectArray ocspData)
{
    if (ocspData == NULL)
    {
        return;
    }
    // do the OCSP check for each of the cert chains
    OcspClient* ocspClient =
        reinterpret_cast< OcspClient* >(aNativePeerHandle);
    jint len = env->GetArrayLength(ocspData);
    jboolean isCopy;
    for (int i=0; i<len; i++)
    {
        jobject jOcspData = env->GetObjectArrayElement(ocspData, i);
        jclass ocspDataClass = env->GetObjectClass(jOcspData);
        jmethodID getCertChainMethod = env->GetMethodID(
                                           ocspDataClass,"getCertChain", "()[Ljava/lang/String;");
        jmethodID setIndividualResponsesMethod = env->GetMethodID(
                    ocspDataClass,"setIndividualResponses", "([I)V");
        jmethodID setSummaryMethod = env->GetMethodID(
                                         ocspDataClass,"setSummary", "(I)V");
        jobjectArray jCertChain = (jobjectArray)env->CallObjectMethod(
                                      jOcspData, getCertChainMethod);
        jint certChainLen = env->GetArrayLength(jCertChain);
        const char** certChain = new const char* [certChainLen];
        for (int j=0; j<certChainLen; j++)
        {
            jstring jCert  = (jstring)env->GetObjectArrayElement(jCertChain, j);
            const char *cert =  env->GetStringUTFChars(jCert, &isCopy);
            certChain[j] = cert;
        }
        // do the actual OCSP check for the current cert chain
        ocspClient->startOcspCheck(certChain, certChainLen);
        OcspResponse ocspResponse = ocspClient->getOcspCheckResponse();
        env->CallVoidMethod(jOcspData, setSummaryMethod, ocspResponse.iSummary);
        if (ocspResponse.iIndividualResponses.size() > 0)
        {
            int size = ocspResponse.iIndividualResponses.size();
            jint* tmp = new jint[size];
            for (int j=0; j<size; j++)
            {
                tmp[j] = ocspResponse.iIndividualResponses[j];
            }
            jintArray individualResponses = (jintArray)env->NewIntArray(size);
            env->SetIntArrayRegion((jintArray)individualResponses,(jsize)0,(jsize)size,tmp);
            // set the individual responses
            env->CallVoidMethod(jOcspData, setIndividualResponsesMethod, individualResponses);
            delete[] tmp;
        }
        // cleanup
        for (int j=0; j<certChainLen; j++)
        {
            jstring jCert  = (jstring)env->GetObjectArrayElement(jCertChain, j);
            env->ReleaseStringUTFChars(jCert,certChain[j]);
        }
        delete[] certChain;
    }
}

JNIEXPORT void JNICALL Java_com_nokia_mj_impl_security_midp_authentication_OcspChecker__1cancelOcspChecks
(JNIEnv *, jobject, jint aNativeThreadHandle, jint aNativePeerHandle)
{
    OcspClient* ocspClient =
        reinterpret_cast< OcspClient* >(aNativePeerHandle);
    ocspClient->cancelOcspCheck(true);
}
