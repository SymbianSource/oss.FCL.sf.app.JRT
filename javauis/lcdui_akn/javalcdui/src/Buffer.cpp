/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32def.h> // FIX FOR MAKE_TINT64 warning
#include <e32std.h>
#include "javax_microedition_lcdui_Buffer.h"
#include "CMIDBuffer.h"
#include "CMIDToolkit.h"

JNIEXPORT jint JNICALL
Java_javax_microedition_lcdui_Buffer__1open(JNIEnv* aJni, jobject aBuffer, jint aEventServerHandle)
{
    return CMIDBuffer::New(*aJni, aBuffer, aEventServerHandle);
}

JNIEXPORT void JNICALL
Java_javax_microedition_lcdui_Buffer__1close
(
    JNIEnv* aJni,
    jobject /*aBuffer*/,
    jint    aHandle
)
{
    JavaUnhand<CMIDBuffer>(aHandle)->Dispose(*aJni);
}

JNIEXPORT void JNICALL
Java_javax_microedition_lcdui_Buffer__1flush
(
    JNIEnv*     aJni,
    jobject     /* aObj */,
    jint        aBuffer,
    jintArray   aArray,
    jint        aCount
)
{
    ASSERT(aBuffer);
    ASSERT(aArray);
    ASSERT(aCount > 0);

    //
    // Flusher condition variable ensures that we cannot get here
    // whilst the server is processing the buffer. Hence it is safe to
    // access the buffer directly. If we wished to support a cicular
    // buffer, this could be done with a lightweight mutex.
    //
    CMIDBuffer& buffer = * JavaUnhand<CMIDBuffer>(aBuffer);

    buffer.RequestProcess(*aJni, aArray, aCount);
}

#ifdef GRAPHICS_BUFFER_DRAWPIXELS
JNIEXPORT jint JNICALL
Java_javax_microedition_lcdui_Buffer__1alloc
(
    JNIEnv*     aJni,
    jobject     /* aObj */,
    jintArray   aArray,
    jint        aOffset,
    jint        aLength
)
{
    jint* address = (jint*)User::Alloc(aLength*sizeof(jint));
    if (!address)
    {
        return KErrNoMemory;
    }
    aJni->GetIntArrayRegion(aArray, aOffset, aLength, address);
    return JavaMakeHandle(address);
}

JNIEXPORT void JNICALL
Java_javax_microedition_lcdui_Buffer__1free
(
    JNIEnv*     /*aJni*/,
    jobject     /* aObj */,
    jint        aAddress
)
{
    User::Free(JavaUnhand<jint*>(aAddress));
}
#endif
