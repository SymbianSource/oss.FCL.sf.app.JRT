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
//
// ImageLoader.cpp
//
//
#include "javax_microedition_lcdui_ImageLoader.h"
#include "CMIDImageLoader.h"

//
//
//
JNIEXPORT jint JNICALL
Java_javax_microedition_lcdui_ImageLoader__1create
(
    JNIEnv* aJni,
    jobject aLoader,
    jint    aServerHandle,
    jint    aToolkitHandle
)
{
    return CMIDImageLoader::New(*aJni, aLoader, aServerHandle, aToolkitHandle);
}

//
//
//
JNIEXPORT void JNICALL
Java_javax_microedition_lcdui_ImageLoader__1destroy
(
    JNIEnv* aJni,
    jobject /*aLoader*/,
    jint    aHandle
)
{
    JavaUnhand<CMIDImageLoader>(aHandle)->Dispose(*aJni);
}

//
//
//
JNIEXPORT jint JNICALL
Java_javax_microedition_lcdui_ImageLoader__1append
(
    JNIEnv*     aJni,
    jobject     /*aLoader*/,
    jint        aHandle,
    jbyteArray  aArray,
    jint        aOffset,
    jint        aCount
)
{
    return JavaUnhand<CMIDImageLoader>(aHandle)->AppendJni(*aJni, aArray, aOffset, aCount);
}

JNIEXPORT jint JNICALL
Java_javax_microedition_lcdui_ImageLoader__1decode
(
    JNIEnv* aJni,
    jobject /*aLoader*/,
    jint    aHandle
)
{
    return JavaUnhand<CMIDImageLoader>(aHandle)->AsyncDecodeJni(*aJni);
}
