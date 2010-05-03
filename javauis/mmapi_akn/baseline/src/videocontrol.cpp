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
* Description:  This class has JNI wrappers for CMMAVideoControl
*
*/


#include <jutils.h>
#include <lcdui.h>
#include "com_nokia_microedition_media_control_VideoControl.h"
#include "cmmaeventsource.h"
#include "cmmavideocontrol.h"
#include "CMIDToolkit.h"
#include "cmmadeleterefevent.h"
#include <jdebug.h>
#include <JniEnvWrapper.h>

#include <mswtclient.h>
#include <swtcliententry.h>

JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_control_VideoControl__1construct
(JNIEnv* aJni, jobject aControlObject, jint aControlHandle,
 jint aEventSourceHandle, jint aToolkitHandle)
{
    DEBUG("Java_com_nokia_microedition_media_control_VideoControl__1construct + ");
    CMMAVideoControl* videoControl =
        JavaUnhand< CMMAVideoControl >(aControlHandle);

    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);


    __ASSERT_DEBUG(videoControl,
                   User::Panic(_L("videocontrol::control is null"),
                               KErrArgument));
    __ASSERT_DEBUG(eventSource,
                   User::Panic(_L("videocontrol::eventsource is null"),
                               KErrArgument));

    // reference must be done, that events can sent to java side
    jobject controlObject = aJni->NewWeakGlobalRef(aControlObject);

    CMIDToolkit* ptoolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    //MACRO (JniEnvWrapper.h): In case of J9, this macro gets a pointer to VM
    //and nastly casts it to JNIEnv.
    JNIENV_TO_VM(aJni);
    TInt error = eventSource->ExecuteTrap(&CMMAVideoControl::ConstructL,
                                          videoControl,
                                          controlObject,
                                          JNI_PARAM, //Passes JavaVM*, not JNIEnv*.
                                          (MMMAEventPoster*)eventSource,
                                          ptoolkit);
    DEBUG("Java_com_nokia_microedition_media_control_VideoControl__1construct - ");
    return error;
}


JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_control_VideoControl__1setDisplayProperty
(JNIEnv*, jobject, jint aControlHandle, jint aEventSourceHandle,
 jint aPropertyA, jint aPropertyB, jint aPropertyType)
{
    DEBUG("Java_com_nokia_microedition_media_control_VideoControl__1setDisplayProperty");

    CMMAVideoControl* videoControl =
        JavaUnhand< CMMAVideoControl >(aControlHandle);

    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    __ASSERT_DEBUG(videoControl,
                   User::Panic(_L("videocontrol::control is null"),
                               KErrArgument));
    __ASSERT_DEBUG(eventSource,
                   User::Panic(_L("videocontrol::eventsource is null"),
                               KErrArgument));

    TInt error(eventSource->ExecuteTrap(&CMMAVideoControl::StaticSetPropertyL,
                                        videoControl,
                                        aPropertyType,
                                        aPropertyA,
                                        aPropertyB));
    DEBUG("Java_com_nokia_microedition_media_control_VideoControl__1setDisplayProperty -");
    return error;
}

JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_control_VideoControl__1getControlProperty
(JNIEnv*, jobject, jint aControlHandle, jint aEventSourceHandle,
 jint aPropertyType)
{
    DEBUG("Java_com_nokia_microedition_media_control_VideoControl__1getControlProperty");

    CMMAVideoControl* videoControl =
        JavaUnhand< CMMAVideoControl >(aControlHandle);

    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    __ASSERT_DEBUG(videoControl,
                   User::Panic(_L("videocontrol::control is null"),
                               KErrArgument));
    __ASSERT_DEBUG(eventSource,
                   User::Panic(_L("videocontrol::evetnsource is null"),
                               KErrArgument));
    TInt returnValue(KErrNone);

    TInt error(eventSource->ExecuteTrap(&CMMAVideoControl::StaticGetPropertyL,
                                        videoControl,
                                        aPropertyType,
                                        &returnValue));
    DEBUG("Java_com_nokia_microedition_media_control_VideoControl__1getControlProperty - ");
    if (error == KErrNone)
    {
        return returnValue;
    }
    return error;
}

JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_control_VideoControl__1getSnapshot
(JNIEnv* aJni, jobject, jint aControlHandle, jint aEventSourceHandle,
 jstring aProperties)
{
    DEBUG("Java_com_nokia_microedition_media_control_VideoControl__1getSnapshot + ");

    CMMAVideoControl* videoControl =
        JavaUnhand< CMMAVideoControl >(aControlHandle);

    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    __ASSERT_DEBUG(videoControl,
                   User::Panic(_L("videocontrol::control is null"),
                               KErrArgument));
    __ASSERT_DEBUG(eventSource,
                   User::Panic(_L("videocontrol::evetnsource is null"),
                               KErrArgument));

    RJString properties(*aJni, aProperties);
    TInt error(eventSource->ExecuteTrap(&CMMAVideoControl::TakeSnapShotL,
                                        videoControl,
                                        (const TDesC*)&properties));
    DEBUG("Java_com_nokia_microedition_media_control_VideoControl__1getSnapshot - ");
    return error;
}

/**
 * Initializes dynamic display mode
 * @return handle to MMMADirectContent or error code
 */
LOCAL_C jint InitDynamicMode(CMMAEventSource* aEventSource,
                             CMMAVideoControl* aVideoControl,
                             jobject aGUIObject,
                             CMMAEvent* aDeleteRefEvent)
{
    TInt contentHandle = 0;
    TInt error = aEventSource->ExecuteTrap(
                     &CMMAVideoControl::StaticInitDynamicModeL,
                     aVideoControl,
                     &contentHandle,
                     aEventSource,
                     aGUIObject,
                     aDeleteRefEvent);

    // if init failed return error code
    if (error != KErrNone)
    {
        contentHandle = error;
    }
    return contentHandle;
}

JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_control_VideoControl__1initDisplayMode
(JNIEnv* aJni, jobject, jint aControlHandle, jint aEventSourceHandle,
 jint aComponentHandle, jobject aComponent)
{
    DEBUG("Java_com_nokia_microedition_media_control_VideoControl__1initDisplayMode +");

    DEBUG_INT("aControlHandle = %d", aControlHandle);
    DEBUG_INT("aEventSourceHandle = %d", aEventSourceHandle);
    DEBUG_INT("aComponentHandle = %d", aComponentHandle);

    CMMAVideoControl* videoControl =
        JavaUnhand< CMMAVideoControl >(aControlHandle);

    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    // Reference need to created to the component that component won't be deleted
    // before display associated to it.
    jobject componentRef = aJni->NewGlobalRef(aComponent);
    if (componentRef == NULL)
    {
        return KErrNoMemory;
    }
    // Component reference need to be deleted in controls destructor. Created here to
    // be sure that reference can be deleted. .
    CMMAEvent* deleteRefEvent = new CMMADeleteRefEvent(componentRef);
    // Cleanup stack is not needed, because static init takes the ownership and
    // cannot leave before that.
    if (!deleteRefEvent)
    {
        aJni->DeleteGlobalRef(componentRef);
        return KErrNoMemory;
    }

    // other modes has component
    if (aComponentHandle == 0)
    {
        return InitDynamicMode(eventSource,
                               videoControl,
                               componentRef,
                               deleteRefEvent);
    }

    MMIDComponent* component = JavaUnhand< MMIDComponent >(aComponentHandle);


    __ASSERT_DEBUG(videoControl,
                   User::Panic(_L("videocontrol::control is null"),
                               KErrArgument));
    __ASSERT_DEBUG(component,
                   User::Panic(_L("videocontrol::component is null"),
                               KErrArgument));
    __ASSERT_DEBUG(eventSource,
                   User::Panic(_L("videocontrol::eventsource is null"),
                               KErrArgument));
    TInt handle(0);
    TInt error(eventSource->ExecuteTrap(&CMMAVideoControl::StaticInitL,
                                        videoControl,
                                        component,
                                        &handle,
                                        deleteRefEvent));
    DEBUG("Java_com_nokia_microedition_media_control_VideoControl__1initDisplayMode - ");
    if (error == KErrNone)
    {
        return handle;
    }
    return error;
}

JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_control_VideoControl__1setForeground
(JNIEnv* /*aJni*/, jobject, jint aControlHandle, jint aEventSourceHandle, jint aIsForeground)
{
    DEBUG_INT("_1setForeground + FOREGROUND = %d",aIsForeground);
    CMMAVideoControl* videoControl =
        JavaUnhand< CMMAVideoControl >(aControlHandle);

    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    TInt handle(0);
    TInt error(eventSource->ExecuteTrap(&CMMAVideoControl::StaticSetForegroundL,
                                        videoControl,
                                        aIsForeground));
    if (error == KErrNone)
    {
        DEBUG("_1setForeground -");
        return handle;
    }

    DEBUG("_1setForeground -");
    return error;
}
JNIEXPORT jboolean JNICALL Java_com_nokia_microedition_media_control_VideoControl__1isESWT
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

    // enable above code once you have support for eswt in 3.2.3, it is already enable for 5.0

}
//  END OF FILE
