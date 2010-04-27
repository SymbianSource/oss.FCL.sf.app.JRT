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
* Description:  ?Description
*
*/


#include <e32def.h>
#include <e32cmn.h>
#include <e32std.h>

#include "jutils.h"
#include "jni.h"




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
OS_EXPORT  TInt ArrayUtils::CopyToNative(JNIEnv& aJni, jbyteArray aJavaBuffer,
        TInt aOffset, TInt aLength, TDes8& aNativeBuffer)
{

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
OS_EXPORT  TInt ArrayUtils::CopyToJava(JNIEnv& aJni, const TDesC8& aNativeBuffer,
                                       jbyteArray aJavaBuffer, TInt aOffset, TInt aLength)
{

    TInt nativeBufferLength = aNativeBuffer.Length();
    TInt length = (nativeBufferLength < aLength) ? nativeBufferLength : aLength;
    TUint8* nativeBufferPtr = const_cast<TUint8*>(aNativeBuffer.Ptr());
    jbyte* jNativeBufferPtr = reinterpret_cast<jbyte*>(nativeBufferPtr);
    aJni.SetByteArrayRegion(aJavaBuffer, aOffset, length, jNativeBufferPtr);
    return length;
}







