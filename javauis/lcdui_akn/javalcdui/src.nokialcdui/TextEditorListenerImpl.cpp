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
* Description: JNI implementation of TextEditorListenerImpl class
*
*/


// INTERNAL INCLUDES
#include "com_nokia_mid_ui_impl_TextEditorListenerImpl.h"

// EXTERNAL INCLUDES
#include <CMIDToolkit.h>
#include <CMIDTextEditorObserver.h>
#include <jdebug.h>
#include <JniEnvWrapper.h>

/**
 * Local helper function for creating the native side peer object.
 *
 * @param aNativePeerHandle On return, contains the native peer's handle
 */
LOCAL_C void CreateNativePeerL(
    JNIEnv* aJniEnv,
    CMIDToolkit* aToolkit,
    jobject aPeer,
    TInt* aNativePeerHandle)
{
    // Resolve method for callbacks.
    jclass objectClass = aJniEnv->GetObjectClass(aPeer);

    // Get method id for the notify function
    jmethodID callBack = aJniEnv->GetMethodID(
                             objectClass, "inputAction", "(I)V");

    // Perform execption check
    if (aJniEnv->ExceptionCheck())
    {
        // Previous JNI operation caused an exception. Print the stack
        // trace and clear any pending exceptions from the Java VM
        aJniEnv->ExceptionDescribe();
        aJniEnv->ExceptionClear();
        User::Leave(KErrGeneral);
    }

    CMIDTextEditorObserver* observer = CMIDTextEditorObserver::NewL(
                                           *aToolkit, *aJniEnv, aPeer, callBack);

    // Put the component to cleanup stack during the register operation.
    CleanupDisposePushL(observer);

    // Register component to the LCDUI.
    *aNativePeerHandle = aToolkit->RegisterComponentL(observer, aPeer);

    // Component can be popped from the cleanup stack.
    CleanupPopComponent(observer);
}

/*
 * Class:     com_nokia_mid_ui_impl_TextEditorListenerImpl
 * Method:    _createNativePeer
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mid_ui_impl_TextEditorListenerImpl__1createNativePeer(
    JNIEnv* aJniEnv,
    jobject aPeer,
    jint aToolkitHandle)
{
    DEBUG("TextEditorListenerImpl.cpp - createNativePeer +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    jobject peer = aJniEnv->NewWeakGlobalRef(aPeer);
    TInt handle = -1;

    TRAPD(error, CreateNativePeerL(aJniEnv, toolkit, peer, &handle));

    if (error != KErrNone)
    {
        // Global reference must be removed at this point if construction
        // failed for some reason.
        aJniEnv->DeleteWeakGlobalRef(static_cast< jweak >(peer));
    }

    DEBUG_INT("TextEditorListenerImpl.cpp - createNativePeer, error = %d",
              error);

    return (error != KErrNone ? error : handle);
}

/**
 * Local helper function for dispoisng text editor native side component.
 *
 * @param aEditor The editor to be destroyed.
 */
LOCAL_C void Dispose(
    CMIDToolkit* aToolkit,
    MMIDTextEditorObserver* aEditorObserver)
{
    aToolkit->DisposeObject(aEditorObserver);
}

/*
 * Class:     com_nokia_mid_ui_impl_TextEditorListenerImpl
 * Method:    _dispose
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_com_nokia_mid_ui_impl_TextEditorListenerImpl__1dispose(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aToolkitHandle,
    jint aNativePeerHandle)
{
    DEBUG("TextEditorListenerImpl.cpp - dispose +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditorObserver* editorObserver =
        MIDUnhandObject< MMIDTextEditorObserver >(aNativePeerHandle);

    toolkit->ExecuteV(&Dispose, toolkit, editorObserver);

    DEBUG("TextEditorListenerImpl.cpp - dispose -");
}

// End of file
