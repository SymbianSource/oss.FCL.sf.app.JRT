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


#include "com_nokia_mj_impl_properties_satsa_SmartCardSlots.h"

#include "javaapduconnection.h"
#include "cstsconnection.h"

#include "cstsresphandler.h"
#include "fs_methodcall.h"
#include "logger.h"
#include "javajniutils.h"
#include "s60commonutils.h"

using namespace java::satsa;

/**
 * JNI function.
 */
JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_properties_satsa_SmartCardSlots__1getSlots(
    JNIEnv* aJni, jclass /*aPeer*/, jint aHandle,
    jintArray aErrArray)
{
    LOG(ESATSA, EInfo,"+ entering the __1getSlots JNI");

    TPtr respPtr(NULL, 0);
    jstring retSlots = NULL;

    JavaApduConnection* apduConn = reinterpret_cast<JavaApduConnection *>(aHandle);

    std::wstring slotString(L"");
    TRAPD(err, slotString = apduConn->GetSlot());
    if (KErrNone != err)
    {
        return NULL;
    }

    if (slotString.length()> 0)
    {
        retSlots =
            java::util::JniUtils::wstringToJstring(aJni, slotString);
    }
    else
    {
        std::wstring str(L"");
        return java::util::JniUtils::wstringToJstring(aJni, str);
    }

    jint javaError[ 1 ] =
        {   err};
    aJni->SetIntArrayRegion(aErrArray, 0, 1, javaError);
    LOG(ESATSA, EInfo, "-- returning from __1getSlots JNI");
    return retSlots;

}

/**
 * JNI function.
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_properties_satsa_SmartCardSlots__1create
(JNIEnv* aJni, jobject aPeer)
{
    LOG(ESATSA, EInfo, "+ entering the __1create slots JNI");

    jobject connectionObject = aJni->NewWeakGlobalRef(aPeer);

    TInt handle = KErrNoMemory;
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
    handle = connectionHandle;

    TInt err = apduConn->CreateSlot(*aJni, connectionObject);
    if (err != KErrNone)
    {
        // Set error code to handle which is returned to Java
        ELOG(ESATSA,"__1create slots JNI:");
        return err;
    }
    LOG1(ESATSA, EInfo, "APDUConnectionImpl.cpp::create return value %d", err);
    LOG(ESATSA, EInfo, "-- returning from the __1create slots JNI");
    // return handle to new connection object or error code
    return handle;
}

