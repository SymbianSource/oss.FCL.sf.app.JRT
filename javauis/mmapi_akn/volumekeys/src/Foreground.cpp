/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  JNI implementation for handling midlet foreground status
*
*/


// EXTERNAL INCLUDES
#include <jutils.h>
#include <jdebug.h>

#include <mswtclient.h>
#include <swtcliententry.h>

// INTERNAL INCLUDES
#include "com_nokia_microedition_volumekeys_ForegroundListener.h"
#include "cmmaforeground.h"
#include "cmmaeventsource.h"
#include "cmmaglobalvolume.h"

// for xm-radio fix
#include "CMIDToolkit.h"

/**
 * Static delegator function for initialize.
 * @param aReturnHandle Native class object.
 * @param aEventSource Event source handle.
 * @param aForegroundListener ForegroundListener java object.
 * @param aJavaMethod java method ID.
 */
LOCAL_C void CreateForegroundL(
    TInt* aReturnHandle,
    CMMAEventSource* aEventSource,
    jobject aForegroundListener,
    jmethodID aJavaMethod,
    CMIDToolkit* aToolkit) // xm-radio fix
{
    CMMAForeground* foreground =
        CMMAForeground::NewL(aEventSource,
                             aForegroundListener,
                             aJavaMethod,
                             aToolkit // xm-radio fix
                            );
    CleanupStack::PushL(foreground);

    // Create global volume which handles the volume events
    CMMAGlobalVolume* globalVolume = CMMAGlobalVolume::NewL(foreground);
    // Global volume takes the ownership of the foreground listener
    CleanupStack::Pop(foreground);
    // Set global volume to MMA event source. The ownership transfers to
    // MMA event sources as it handles adding and removing players
    aEventSource->SetGlobalVolume(globalVolume);
    *aReturnHandle = JavaMakeHandle(foreground);
}

/*
 * Class:     com_nokia_microedition_volumekeys_ForegroundListener
 * Method:    _initialize
 * Signature: (ILjava/lang/Object;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_microedition_volumekeys_ForegroundListener__1initialize
(JNIEnv* aJni,
 jobject /*aJobject*/,
 jint aEventSourceHandle,
 jobject aForegroundListener ,
 jint aToolkitHandle)   // for xm-radio fix
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    jobject fgListenerObject = NULL;
    fgListenerObject = aJni->NewWeakGlobalRef(aForegroundListener);

    jclass foregroundListenerClass = aJni->GetObjectClass(fgListenerObject);

    jmethodID javaMethod = aJni->GetMethodID(
                               foregroundListenerClass,
                               "update",
                               "()V");

    TInt foregroundHandle = NULL;

    // xm-radio fix
    CMIDToolkit* ptoolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    jint error = eventSource->ExecuteTrap(CreateForegroundL,
                                          &foregroundHandle,
                                          eventSource,
                                          fgListenerObject,
                                          javaMethod,
                                          ptoolkit);  // xm-radio fix

    if (error != KErrNone)
    {
        aJni->DeleteGlobalRef(fgListenerObject);
    }

    return (error != KErrNone) ? error : foregroundHandle;
}

/**
 * Static delegator function for setforeground
 * @param aCMMAForeground native class.
 * @param aIsForeground boolean is midlet foreground or not.

 */
LOCAL_C void SetForeground(
    CMMAForeground* aCMMAForeground,
    TBool aIsForeground)
{
    aCMMAForeground->SetForeground(aIsForeground);
}

/*
* Class:     com_nokia_microedition_volumekeys_ForegroundListener
* Method:    _setForeground
* Signature: (IZ)V
*/
JNIEXPORT void JNICALL Java_com_nokia_microedition_volumekeys_ForegroundListener__1setForeground
(JNIEnv* /*aJni*/,
 jobject /*aJobject*/,
 jint aEventSourceHandle,
 jint aForegroundHandle,
 jboolean aIsForeground)
{
    CMMAEventSource* eventSource =
        JavaUnhand<CMMAEventSource>(aEventSourceHandle);

    CMMAForeground* nativeHandle =
        JavaUnhand<CMMAForeground>(aForegroundHandle);

    eventSource->ExecuteV(SetForeground,
                          nativeHandle,
                          (TBool) aIsForeground);
}

JNIEXPORT jboolean JNICALL Java_com_nokia_microedition_volumekeys_ForegroundListener__1isESWT
(JNIEnv *, jobject)
{
    MSwtClient* client = NULL;
    TRAP_IGNORE(client = SWT::CreateClientL());

    if (!client)
    {
        return false; // LCDUI midlet running
    }
    else
    {
        delete client;
        client = NULL;
        return true;  // eSWT midlet running
    }
}


//  END OF FILE
