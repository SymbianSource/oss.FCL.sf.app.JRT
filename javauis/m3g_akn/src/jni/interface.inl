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
#include "javax_microedition_m3g_Interface.h"

TInt initM3GEventSourceL(JNIEnv& aJni,jobject aPeer,TJavaEventServer aServer);

/*!
 * \brief Error handler for the Java interface
 *
 * Converts M3G errors to exceptions and throws them automatically.
 */
static void errorHandler(M3Genum errorCode, M3GInterface /*m3g*/)
{
    CSynchronization::InstanceL()->SetErrorCode(errorCode);
}


JNIEXPORT jint JNICALL Java_javax_microedition_m3g_Interface__1initEventSource
(JNIEnv *aEnv, jclass aPeer, /*jint aContext,*/ jint aServer)
{
    TInt ret(0);
    TRAPD(err, ret = initM3GEventSourceL(*aEnv, aPeer, aServer));
    if (err == KErrNone)
    {
        return ret;
    }
    return err;
}


JNIEXPORT void JNICALL Java_javax_microedition_m3g_Interface__1finalizeEventSource(JNIEnv* aEnv, jclass, jint aEventSourceHandle)
{
    CJavaM3GEventSource* eventSource = JavaUnhand<CJavaM3GEventSource>(aEventSourceHandle);
    eventSource->Dispose(*aEnv);
    eventSource = NULL;
}

static int createInterface(M3Gparams* aCs)
{
    return ((unsigned) m3gCreateInterface(aCs));
}

JNIEXPORT jint JNICALL Java_javax_microedition_m3g_Interface__1ctor(JNIEnv* aEnv, jclass, jint aEventSourceHandle)
{
    M3Gparams cs;
    memset(&cs, 0, sizeof(cs));
    cs.mallocFunc = malloc;
    cs.freeFunc   = free;
    cs.errorFunc = errorHandler;

    M3G_DO_LOCK
    /* Call to the Eventserver side */
    CJavaM3GEventSource* eventSource = JavaUnhand<CJavaM3GEventSource>(aEventSourceHandle);
    jint handle = eventSource->Execute(&createInterface, &cs);
    M3G_DO_UNLOCK(aEnv);
    return handle;
}

JNIEXPORT jint JNICALL Java_javax_microedition_m3g_Interface__1getClassID
(JNIEnv* aEnv, jclass, jint aHObject)
{
    M3G_DO_LOCK
    jint handle = m3gGetClass((M3GObject)aHObject);
    M3G_DO_UNLOCK(aEnv);
    return handle;
}
