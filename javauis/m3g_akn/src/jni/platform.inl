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
#include "javax_microedition_m3g_Platform.h"

static void nativeFinalize(M3GObject hObj)
{
    m3gDeleteObject(hObj);
}

static void deleteInterface(M3GInterface aM3g)
{
    m3gDeleteInterface(aM3g);
}

JNIEXPORT void JNICALL Java_javax_microedition_m3g_Platform_finalizeInterface
(JNIEnv* aEnv, jclass, jint aHObj, jint aEventSourceHandle)
{
    M3G_DO_LOCK
    // Call to the other thread in case OpenGL objects need to be
    // deleted
    CJavaM3GEventSource* eventSource = JavaUnhand<CJavaM3GEventSource>(aEventSourceHandle);
    eventSource->ExecuteV(&deleteInterface, (M3GInterface)aHObj);
    M3G_DO_UNLOCK(aEnv)

}

JNIEXPORT void JNICALL Java_javax_microedition_m3g_Platform__1finalizeObject
(JNIEnv* aEnv, jclass, jint aEventSourceHandle, jint aHObj)
{
    M3G_DO_LOCK
    // Call to the other thread in case OpenGL objects need to be
    // deleted
    CJavaM3GEventSource* eventSource = JavaUnhand<CJavaM3GEventSource>(aEventSourceHandle);
    eventSource->ExecuteV(&nativeFinalize, (M3GObject)aHObj);
    M3G_DO_UNLOCK(aEnv)
}
