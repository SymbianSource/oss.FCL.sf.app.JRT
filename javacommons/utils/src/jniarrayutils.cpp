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

#include "jniarrayutils.h"
#include "wchar.h"


OS_EXPORT int JNIArrayUtils::CopyToJava(JNIEnv& aJni, const char* aNativeBuffer, int aNativeBufferLength,
                                        jbyteArray aJavaBuffer, int aOffset, int aLength)
{

    int length = (aNativeBufferLength < aLength) ? aNativeBufferLength : aLength;


    char* nativeBufferPtr = const_cast<char *>(aNativeBuffer);
    jbyte* jNativeBufferPtr = reinterpret_cast<jbyte*>(nativeBufferPtr);
    aJni.SetByteArrayRegion(aJavaBuffer, aOffset, length, jNativeBufferPtr);
    return length;
}


OS_EXPORT int JNIArrayUtils::CopyToNative(JNIEnv& aJni, jbyteArray aJavaBuffer,
        int aOffset, int aLength, char* aNativeBuffer)
{
    char* nativeBufferPtr = const_cast<char*>(aNativeBuffer);
    jbyte* jNativeBufferPtr = reinterpret_cast<jbyte*>(nativeBufferPtr);
    aJni.GetByteArrayRegion(aJavaBuffer, aOffset, aLength, jNativeBufferPtr);
    return aLength;
}


OS_EXPORT jstring JNIArrayUtils::WStringToJavaString(JNIEnv& aJni, const wchar_t* aString, const int aLength)
{
    wchar_t* nativeCharArray = const_cast<wchar_t*>(aString);
    const jchar* stringPtr = reinterpret_cast<jchar*>(nativeCharArray);
    const jsize stringLength = aLength;
    jstring jniString = aJni.NewString(stringPtr, stringLength);
    return jniString;
}

