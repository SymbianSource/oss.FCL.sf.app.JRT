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

#include "s60commonutils.h"
#include "javajniutils.h"
#include "com_nokia_mj_impl_satsa_APDUConnectionImpl.h"
#include "cstsconnection.h"
#include "cstsresphandler.h"
#include "cstsmidletinfo.h"
#include "javaapduconnection.h"
#include "logger.h"
#include "javacommonutils.h"

// CONSTANTS

const TInt KReturnValueArrayLength = 2;
const TInt KHandleIndex = 0;
const TInt KConnTypeIndex = 1;

using namespace java::satsa;

/**
 * JNI function.
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_satsa_APDUConnectionImpl__1doPinMethod(
    JNIEnv* aJni, jclass aPeer,jint aHandle,
    jint aPinMethodType, jint aPinID)
{
    LOG(ESATSA, EInfo, "+ Entering doPinMethod JNI function");
    JavaApduConnection* apduConn = reinterpret_cast<JavaApduConnection *>(aHandle);
    TInt err = apduConn->DoPinMethod(*aJni, aPeer, aHandle, aPinMethodType, aPinID);
    LOG(ESATSA, EInfo, "Returning from doPinMethod JNI function");
    //return error code to caller
    return err;
}

/**
 * JNI function.
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_satsa_APDUConnectionImpl__1exchangeAPDU(
    JNIEnv *aJni, jclass aPeer,jint aHandle,
    jbyteArray aCommandAPDU)
{
    LOG(ESATSA, EInfo, "+ Entering exchangeAPDU JNI function");

    JavaApduConnection* apduConn = reinterpret_cast<JavaApduConnection *>(aHandle);
    TInt err = 0;
    err = apduConn->ExchangeAPDU(*aJni, aPeer, aHandle, aCommandAPDU);

    LOG(ESATSA, EInfo, "-- Returning from exchnageAPDU JNI function");
    //return error code to caller
    return err;
}

/**
 * JNI function.
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_satsa_APDUConnectionImpl__1unblockPin(
    JNIEnv* aJni, jclass aPeer,jint aHandle,
    jint aBlockedPinID, jint aUnblockingPinID)
{
    LOG(ESATSA, EInfo, "+ Entering UnblockPin JNI function");

    JavaApduConnection* apduConn = reinterpret_cast<JavaApduConnection *>(aHandle);
    TInt err = apduConn->UnblockPin(*aJni, aPeer, aHandle, aBlockedPinID, aUnblockingPinID);
    LOG(ESATSA, EInfo, "-- Returning from unblockPin JNI function");
    //return error code to caller
    return err;
}

/**
 * JNI function.
 */
JNIEXPORT jbyteArray JNICALL Java_com_nokia_mj_impl_satsa_APDUConnectionImpl__1getATR(
    JNIEnv *aJni, jclass /*aPeer*/, jint aHandle,
    jintArray aErrArray)
{
    LOG(ESATSA,  EInfo, "GetATR JNI function");

    JavaApduConnection* apduConn = reinterpret_cast<JavaApduConnection *>(aHandle);
    TInt err = 0;
    //err = apduConn->GetATR(*aJni, aPeer, aHandle, aCommandAPDU);
    std::wstring atrData;
    TRAP(err,
         atrData = apduConn->GetATR());

    // return null if resp is empty (card has been removed or the connection
    // has been closed.)

    jbyteArray responseAPDU = NULL;

    if (atrData.length()> 0)
    {
        const char* utf8 = java::util::JavaCommonUtils::wstringToUtf8(atrData);
        int len = strlen(utf8);
        responseAPDU = aJni->NewByteArray(len);
        if (responseAPDU != NULL)
        {
            aJni->SetByteArrayRegion(responseAPDU, 0, len,
                                     (jbyte *)utf8);
        }
        else
        {
            ELOG(ESATSA,"__1getATR: responseAPDU is NULL");
            err = KErrNoMemory;
        }
        delete[] utf8;
    }

    //error array contains only one error value
    jint javaError[ 1 ] =
        {   err};
    aJni->SetIntArrayRegion(aErrArray, 0, 1, javaError);

    ILOG1(ESATSA, "APDUConnectionImpl::XXX__1getATR::GetATR returned: %d", err);
    return responseAPDU;

}

/**
 * JNI function.
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_satsa_APDUConnectionImpl__1create
(JNIEnv* aJni, jobject aPeer,
 jstring aURI, jstring aName, jstring aVendor,
 jstring aVersion,jstring aDomain,jstring aRootCertHash,
 jstring aSigningCertHash,
 jintArray aTypeArray)
{
    LOG(ESATSA, EInfo, "+ Entering create JNI function");

    //creates array for return values, because ExecuteTrap will accept only
    //nine parameters at maximum. This array will consist of handle and
    //connectionType
    TInt returnValueArray[ KReturnValueArrayLength ] =
        {   0};

    //get handle to java operationComplete java method
    jmethodID handleEventMethod =
        aJni->GetMethodID(aJni->GetObjectClass(aPeer),
                          "operationComplete",
                          "(II[B)V");

    jobject connectionObject = aJni->NewWeakGlobalRef(aPeer);

    JavaApduConnection *apduConn = NULL;
    try
    {
        apduConn = JavaApduConnection::getInstance();
    }
    catch (int err)
    {
        ELOG1(ESATSA,"Caught an Exception --Value is:%d",err);
        delete apduConn;
        apduConn=NULL;
    }

    TInt connectionHandle = reinterpret_cast<TInt>(apduConn);
    returnValueArray[ KHandleIndex ] = connectionHandle;

    TInt err = apduConn->Create(*aJni, aURI, aName, aVendor,
                                aVersion, aDomain, aRootCertHash,
                                aSigningCertHash, returnValueArray,
                                connectionObject,handleEventMethod);
    if (err != KErrNone)
    {
        // CreateConnectionL leaved.
        // Set error code to handle which is returned to Java
        returnValueArray[ KHandleIndex ] = err;
    }

    //set type of connection to the "returned" type array
    jint javaConnType[ 1 ] =
        {   returnValueArray[ KConnTypeIndex ]};
    aJni->SetIntArrayRegion(aTypeArray, 0, 1, javaConnType);

    ILOG1(ESATSA, "STS::APDUConnectionImpl.cpp::create return value %d", err);
    // return handle to new connection object or error code
    return returnValueArray[ KHandleIndex ];

}

/**
 * JNI function.
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_satsa_APDUConnectionImpl__1open(
    JNIEnv* aJni, jclass aPeer, jint aHandle)
{
    LOG(ESATSA, EInfo, "+ Entering Open JNI function");
    JavaApduConnection* apduConn = reinterpret_cast<JavaApduConnection *>(aHandle);
    TInt err = apduConn->Open(*aJni, aPeer, aHandle);
    LOG(ESATSA, EInfo, "-- Returning from Open JNI function");
    return err;

}

/**
 * JNI function.
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_satsa_APDUConnectionImpl__1close(
    JNIEnv* aJni, jclass aPeer, jint aHandle)
{
    LOG(ESATSA, EInfo, "+ Entering close JNI function");
    JavaApduConnection* apduConn = reinterpret_cast<JavaApduConnection *>(aHandle);
    TInt err = apduConn->Close(*aJni, aPeer, aHandle);
    LOG(ESATSA, EInfo, "-- Returning from close JNI function");
    return err;

}
