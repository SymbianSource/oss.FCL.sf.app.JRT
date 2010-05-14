/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "mmapiutils.h"
#include "s60commonutils.h"
using namespace java::util;
    
enum TJavaArrayPanic
{
    EBadOffsetIntoJavaArray,
    EWritingOverEndOfJavaArray,
    EBadOffsetIntoJavaArrayForRead,
    EReadingOverEndOfJavaArray,
};


/**
 * Copies data from the native to the Java array.
 * @return The number of bytes copied.
 */
TInt MMAPIUtils::CopyToJava(JNIEnv& aJni, const TDesC8& aNativeBuffer,
                                     jbyteArray aJavaBuffer, TInt aOffset, TInt aLength)
{
    __ASSERT_DEBUG(aOffset <= aJni.GetArrayLength(aJavaBuffer),
                   User::Panic(_L("ArrayUtils"), EBadOffsetIntoJavaArray));
    __ASSERT_DEBUG(aLength <= aJni.GetArrayLength(aJavaBuffer) - aOffset,
                   User::Panic(_L("ArrayUtils"), EWritingOverEndOfJavaArray));

    TInt nativeBufferLength = aNativeBuffer.Length();
    TInt length = (nativeBufferLength < aLength) ? nativeBufferLength : aLength;
    TUint8* nativeBufferPtr = const_cast<TUint8*>(aNativeBuffer.Ptr());
    jbyte* jNativeBufferPtr = reinterpret_cast<jbyte*>(nativeBufferPtr);
    aJni.SetByteArrayRegion(aJavaBuffer, aOffset, length, jNativeBufferPtr);
    return length;
}

jobjectArray MMAPIUtils::CopyToNewJavaStringArrayL(JNIEnv& aJni, const CDesCArray& aNativeArray)
{
    jclass stringClass = aJni.FindClass("java/lang/String");
    User::LeaveIfNull(stringClass);
    //
    jobjectArray result = aJni.NewObjectArray(aNativeArray.Count(), stringClass, NULL);
    if (result != NULL)
    {
        TPtrC item;
        for (int i=0; i<aNativeArray.Count(); ++i)
        {
            item.Set(aNativeArray[i]);
            AddToJavaStringArrayL(aJni, result, i, item);
        }
    }
    return result;
}

void MMAPIUtils::AddToJavaStringArrayL(JNIEnv& aJni, jobjectArray& aContainer, TInt aPosition, const TDesC& aString)
{
    jstring javaString = S60CommonUtils::NativeToJavaString(aJni, aString);
    User::LeaveIfNull(javaString);
    //
    aJni.SetObjectArrayElement(aContainer, aPosition, javaString);
    aJni.DeleteLocalRef(javaString);
}
