/*
* Copyright (c) 2002-2003 Nokia Corporation and/or its subsidiary(-ies).
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

#include <badesca.h>

#include "mnativesecureconnectioninformation.h"
#include "com_nokia_mj_impl_https_SecurityInfoImpl.h"
#include "s60commonutils.h"
#include "logger.h"


using namespace java::util;

// Constants that define the date/time '00:00, 1 Jan 1970' when used to create a TTime object
const TUint JavaUpperTimeFor1970 = 14474675;
const TUint JavaLowerTimeFor1970 = 254771200;

/* Takes a jlong which is the number of milliseconds from the Java epoch time
of 00:00 1st Jan 1970 and converts it into a EPOC TTime object */
TTime CreateEpocTTime(jlong aJavaTime)
{
    // Convert jlong to a TInt64
    TInt64 timeNum = *reinterpret_cast<TInt64*>(&aJavaTime);
    // Create a TTime object that represents the Java Date 'epoch' time of 00:00, 1 Jan 1970
    TInt64 javaEpocTimeNum = MAKE_TINT64(JavaUpperTimeFor1970, JavaLowerTimeFor1970);
    TTime time(javaEpocTimeNum);
    TTimeIntervalMicroSeconds delta(timeNum * 1000);
    return time + delta;
}

/* Takes a TTime and returns the number of milliseconds since the Java epoch time
of 00:00 1st Jan 1970 */
jlong CreateJavaTime(TTime aEpocTime)
{
    // Create a TTime object that represents the Java Date 'epoch' time of 00:00, 1 Jan 1970
    TInt64 javaEpocTimeNum = MAKE_TINT64(JavaUpperTimeFor1970, JavaLowerTimeFor1970);
    TTime javaEpochTime(javaEpocTimeNum);
    // Find difference in microseconds between 'epoch' and EPOC date and adjust to milliseconds
    TTimeIntervalMicroSeconds microInterval = aEpocTime.MicroSecondsFrom(javaEpochTime);
    TInt64 intervalNum = microInterval.Int64();
    intervalNum /= 1000;
    jlong jInterval = *reinterpret_cast<jlong*>(&intervalNum);
    return jInterval;
}

void AddToJavaStringArrayL(JNIEnv& aJni, jobjectArray& aContainer, TInt aPosition, const TDesC& aString)
{
    jstring javaString = S60CommonUtils::NativeToJavaString(aJni, aString);
    if (javaString == NULL)
        return;

    aJni.SetObjectArrayElement(aContainer, aPosition, javaString);
    aJni.DeleteLocalRef(javaString);
}

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_https_SecurityInfoImpl__1createSecurityInfo(
    JNIEnv* aJni,
    jobject,
    jint aSecureConnectionHandle,
    jobjectArray aStringResultArray,
    jlongArray aLongResultArray)
{
    //Java unhand uses a reinterpret_cast which messes up the vtables
    //as we are casting to a mixin.
    LOG(ESOCKET,EInfo,"++ jni _createSecurityInfo");
    MNativeSecureConnectionInformation* endPoint = reinterpret_cast<MNativeSecureConnectionInformation*>(aSecureConnectionHandle);
    TTime start = TTime();
    TTime end = TTime();
    CDesCArraySeg* stringResult = NULL;


    stringResult = new CDesCArraySeg(com_nokia_mj_impl_https_SecurityInfoImpl_DATA_STRING_NUMBER);

    if (!stringResult)
        return KErrNoMemory;

    TInt error = KErrNone;

    if (endPoint)
    {
        TRAP(error, endPoint->GetSecurityInfoL(*stringResult, start, end););
    }

    if (error == KErrNone)
    {
        jlong longResult = CreateJavaTime(start);
        aJni->SetLongArrayRegion(aLongResultArray,
                                 com_nokia_mj_impl_https_SecurityInfoImpl_CERT_NOT_BEFORE,
                                 1, &longResult);
        longResult = CreateJavaTime(end);
        aJni->SetLongArrayRegion(aLongResultArray,
                                 com_nokia_mj_impl_https_SecurityInfoImpl_CERT_NOT_AFTER,
                                 1, &longResult);
        for (TInt i = 0; i < stringResult->Count() && error == KErrNone; ++i)
        {
            TRAP(error, AddToJavaStringArrayL(*aJni, aStringResultArray, i, (*stringResult)[i]));
        }
        delete stringResult;
    }
    //Ignore the error if there is no cert, Java will throw a Certificate Exception
    if (error==KErrNotFound)
        error=KErrNone;
    LOG(ESOCKET,EInfo,"-- jni _createSecurityInfo");
    return error;
}




