/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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


#include "jutils.h"



enum TJavaArrayPanic
{
    EBadOffsetIntoJavaArray,
    EWritingOverEndOfJavaArray,
    EBadOffsetIntoJavaArrayForRead,
    EReadingOverEndOfJavaArray,
};



/**
 * Accesses the Java array and copies its data into the native descriptor.
 * @param aJni The JNI environment.
 * @param aJavaBuffer The Java buffer to copy data from.
 * @param aOffset Start of data in Java buffer to copy. This is assumed to be valid.
 * @param aLength Amount of data to copy. This is assumed to be valid.
 * @param aNativeBuffer Target for data. This is assumed to be long enough.
 * @returns An error code.
 */
TInt ArrayUtils::CopyToNative(JNIEnv& aJni, jbyteArray aJavaBuffer,
                              TInt aOffset, TInt aLength, TDes8& aNativeBuffer)
{
    __ASSERT_DEBUG(aOffset <= aJni.GetArrayLength(aJavaBuffer),
                   User::Panic(_L("ArrayUtils"), EBadOffsetIntoJavaArrayForRead));
    __ASSERT_DEBUG(aLength <= aJni.GetArrayLength(aJavaBuffer) - aOffset,
                   User::Panic(_L("ArrayUtils"), EReadingOverEndOfJavaArray));

    aNativeBuffer.SetLength(aLength);
    TUint8* nativeBufferPtr = const_cast<TUint8*>(aNativeBuffer.Ptr());
    jbyte* jNativeBufferPtr = reinterpret_cast<jbyte*>(nativeBufferPtr);
    aJni.GetByteArrayRegion(aJavaBuffer, aOffset, aLength, jNativeBufferPtr);
    return KErrNone;
}



/**
 * Copies data from the native to the Java array.
 * @return The number of bytes copied.
 */
EXPORT_C TInt ArrayUtils::CopyToJava(JNIEnv& aJni, const TDesC8& aNativeBuffer,
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



/**
 * Creates a Java array of strings from a native array of descriptors allocated
 * on the heap.
 * @param aJni The JNI environment.
 * @param aNativeArray The array of descriptors.
 * @return The newly created Java array of String objects, or NULL on error.
 */
jobjectArray ArrayUtils::CopyToNewJavaStringArray(JNIEnv& aJni,
        const RPointerArray<HBufC>& aNativeArray)
{
    jclass stringClass = aJni.FindClass("java/lang/String");
    if (stringClass == NULL)
    {
        return NULL;
    }

    TInt count = aNativeArray.Count();
    jobjectArray result = aJni.NewObjectArray(count, stringClass, NULL);
    if (result == NULL)
    {
        return NULL;
    }

    for (int ii = 0; ii< count; ii++)
    {
        jstring javaString = CreateJavaString(aJni, *aNativeArray[ii]);
        if (javaString == NULL)
        {
            aJni.DeleteLocalRef(result);
            return NULL;
        }

        aJni.SetObjectArrayElement(result, ii, javaString);
        aJni.DeleteLocalRef(javaString);
    }

    return result;
}
