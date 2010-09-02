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


//#include <jutils.h>
//#include <lcdui.h>
#include "com_nokia_microedition_media_control_VideoControl.h"
#include "mmafunctionserver.h"
#include "cmmavideocontrol.h"
//#include "CMIDToolkit.h"
#include "cmmadeleterefevent.h"
#include <logger.h>

//#include <mswtclient.h>
//#include <swtcliententry.h>
#include "s60commonutils.h"
#include "jstringutils.h"
using namespace java::util;

JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_control_VideoControl__1construct
(JNIEnv* aJni, jobject aControlObject, jint aControlHandle,
 jint aEventSourceHandle)
{
    LOG(EJavaMMAPI, EInfo, "Java_com_nokia_microedition_media_control_VideoControl__1construct + ");
    CMMAVideoControl* videoControl =
        reinterpret_cast< CMMAVideoControl* >(aControlHandle);

    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSourceHandle);


    __ASSERT_DEBUG(videoControl,
                   User::Panic(_L("videocontrol::control is null"),
                               KErrArgument));
    __ASSERT_DEBUG(eventSource,
                   User::Panic(_L("videocontrol::eventsource is null"),
                               KErrArgument));

    // reference must be done, that events can sent to java side
    jobject controlObject = aJni->NewWeakGlobalRef(aControlObject);

    // CMIDToolkit* ptoolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    //JNIENV_TO_VM(aJni);
    TInt error = eventSource->ExecuteTrap(&CMMAVideoControl::ConstructL,
                                          videoControl,
                                          controlObject,
                                          eventSource,
                                          eventSource->getValidJniEnv(), //Passes JavaVM*, not JNIEnv*.
                                          (MMMAEventPoster*)eventSource/*,
                                          ptoolkit*/);
    LOG(EJavaMMAPI, EInfo, "Java_com_nokia_microedition_media_control_VideoControl__1construct - ");
    return error;
}


JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_control_VideoControl__1setDisplayProperty
(JNIEnv*, jobject, jint aControlHandle, jint aEventSourceHandle,
 jint aPropertyA, jint aPropertyB, jint aPropertyType, jint aDisplayType)
{
    LOG(EJavaMMAPI, EInfo, "Java_com_nokia_microedition_media_control_VideoControl__1setDisplayProperty");

    CMMAVideoControl* videoControl =
        reinterpret_cast< CMMAVideoControl* >(aControlHandle);

    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer *>(aEventSourceHandle);

    __ASSERT_DEBUG(videoControl,
                   User::Panic(_L("videocontrol::control is null"),
                               KErrArgument));
    __ASSERT_DEBUG(eventSource,
                   User::Panic(_L("videocontrol::eventsource is null"),
                               KErrArgument));
    TInt error = 0;
    if (aDisplayType == CMMAVideoControl::EDISPLAY_ESWT)
    {
        TRAP(error,CMMAVideoControl::StaticSetPropertyL(videoControl, aPropertyType, aPropertyA, aPropertyB));
    }
    else
    {
        error = eventSource->ExecuteTrap(&CMMAVideoControl::StaticSetPropertyL,
                                         videoControl,
                                         aPropertyType,
                                         aPropertyA,
                                         aPropertyB);
    }
    LOG(EJavaMMAPI, EInfo, "Java_com_nokia_microedition_media_control_VideoControl__1setDisplayProperty -");
    return error;
}

JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_control_VideoControl__1getControlProperty
(JNIEnv*, jobject, jint aControlHandle, jint aEventSourceHandle,
 jint aPropertyType, jint aDisplayType)
{
    LOG(EJavaMMAPI, EInfo, "Java_com_nokia_microedition_media_control_VideoControl__1getControlProperty");

    CMMAVideoControl* videoControl =
        reinterpret_cast< CMMAVideoControl* >(aControlHandle);

    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer *>(aEventSourceHandle);

    __ASSERT_DEBUG(videoControl,
                   User::Panic(_L("videocontrol::control is null"),
                               KErrArgument));
    __ASSERT_DEBUG(eventSource,
                   User::Panic(_L("videocontrol::evetnsource is null"),
                               KErrArgument));
    TInt returnValue(KErrNone);
    TInt error = 0;

    if (aDisplayType == CMMAVideoControl::EDISPLAY_ESWT)
    {
        TRAP(error,CMMAVideoControl::StaticGetPropertyL(videoControl, aPropertyType, &returnValue));
    }
    else
    {
        error = eventSource->ExecuteTrap(&CMMAVideoControl::StaticGetPropertyL,
                                         videoControl,
                                         aPropertyType,
                                         &returnValue);
    }
    LOG(EJavaMMAPI, EInfo, "Java_com_nokia_microedition_media_control_VideoControl__1getControlProperty - ");
    if (error == KErrNone)
    {
        return returnValue;
    }
    return error;
}

JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_control_VideoControl__1getSnapshot
(JNIEnv* aJni, jobject, jint aControlHandle, jint /*aEventSourceHandle*/,
 jstring aProperties)
{
    LOG(EJavaMMAPI, EInfo, "Java_com_nokia_microedition_media_control_VideoControl__1getSnapshot + ");

    CMMAVideoControl* videoControl =
        reinterpret_cast< CMMAVideoControl* >(aControlHandle);

    //MMAFunctionServer* eventSource =
    //   reinterpret_cast< MMAFunctionServer *>(aEventSourceHandle);

    __ASSERT_DEBUG(videoControl,
                   User::Panic(_L("videocontrol::control is null"),
                               KErrArgument));
//    __ASSERT_DEBUG(eventSource,
//                   User::Panic(_L("videocontrol::evetnsource is null"),
//                               KErrArgument));

    JStringUtils properties(*aJni, aProperties);

    TInt error = 0;
    TRAP(error,CMMAVideoControl::TakeSnapShotL(videoControl, (const TDesC*)&properties));

    /*
    TInt error(eventSource->ExecuteTrap(&CMMAVideoControl::TakeSnapShotL,
                                        videoControl,
                                        (const TDesC*)&properties));
     */
    LOG(EJavaMMAPI, EInfo, "Java_com_nokia_microedition_media_control_VideoControl__1getSnapshot - ");
    return error;
}

/**
 * Initializes dynamic display mode
 * @return handle to MMMADirectContent or error code
 */
LOCAL_C jint InitDynamicMode(MMAFunctionServer* aEventSource,
                             JNIEnv* aJni,
                             CMMAVideoControl* aVideoControl,
                             jobject aGUIObject,
                             CMMAEvent* aDeleteRefEvent)
{
    TInt contentHandle = 0;
    /* TInt error = aEventSource->ExecuteTrap(
                      &CMMAVideoControl::StaticInitDynamicModeL,
                      aVideoControl,
                      &contentHandle,
                      aEventSource,
                      aGUIObject,
                      aDeleteRefEvent);
    */
    TRAPD(error,CMMAVideoControl::StaticInitDynamicModeL(aVideoControl, aJni, &contentHandle, aEventSource, aGUIObject, aDeleteRefEvent));

    // if init failed return error code
    if (error != KErrNone)
    {
        contentHandle = error;
    }
    return contentHandle;
}

JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_control_VideoControl__1initDisplayMode
(JNIEnv* aJni, jobject aJavaVideoControl, jint aControlHandle, jint aEventSourceHandle,
 jobject aJavaDisplayObj, jobject aComponent,jint aGuiType)
{
    LOG(EJavaMMAPI, EInfo, "Java_com_nokia_microedition_media_control_VideoControl__1initDisplayMode +");
    LOG1(EJavaMMAPI, EInfo, "aControlHandle = %d", aControlHandle);
    LOG1(EJavaMMAPI, EInfo, "aEventSourceHandle = %d", aEventSourceHandle);
//    LOG1( EJavaMMAPI, EInfo, "aComponentHandle = %d", aComponentHandle);

    TInt type = aGuiType;
    CMMAVideoControl* videoControl =
        reinterpret_cast< CMMAVideoControl* >(aControlHandle);

    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer *>(aEventSourceHandle);

    jobject javaVideoControlPeer = aJni->NewWeakGlobalRef(aJavaVideoControl);

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

// for javaside display object  // TODO:  delete this object after its not required in destructor of control

    // Reference need to created to the component that component won't be deleted
    // before display associated to it.
    jobject javaDisplayRef = aJni->NewGlobalRef(aJavaDisplayObj);
    if (javaDisplayRef == NULL)
    {
        return KErrNoMemory;
    }

    // other modes has component
    if (type == CMMAVideoControl::EDISPLAY_ESWT)
    {
        return InitDynamicMode(eventSource,
                               aJni,
                               videoControl,
                               javaDisplayRef,
                               deleteRefEvent);
    }

    //  MMIDComponent* component = JavaUnhand< MMIDComponent >(aComponentHandle);


    __ASSERT_DEBUG(videoControl,
                   User::Panic(_L("videocontrol::control is null"),
                               KErrArgument));
//    __ASSERT_DEBUG(component,
//                   User::Panic(_L("videocontrol::component is null"),
//                               KErrArgument));
    __ASSERT_DEBUG(eventSource,
                   User::Panic(_L("videocontrol::eventsource is null"),
                               KErrArgument));
    TInt handle(0);

    TInt error(eventSource->ExecuteTrap(&CMMAVideoControl::StaticInitL,
                                        videoControl,
                                        javaDisplayRef,
                                        eventSource,
                                        &handle,
                                        deleteRefEvent,
                                        type));
    LOG(EJavaMMAPI, EInfo, "Java_com_nokia_microedition_media_control_VideoControl__1initDisplayMode - ");
    if (error == KErrNone)
    {
        return handle;
    }
    return error;
}

JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_control_VideoControl__1setForeground
(JNIEnv* /*aJni*/, jobject, jint aControlHandle, jint aEventSourceHandle, jint aIsForeground)
{
    LOG1(EJavaMMAPI, EInfo, "_1setForeground + FOREGROUND = %d",aIsForeground);
    CMMAVideoControl* videoControl =
        reinterpret_cast< CMMAVideoControl *>(aControlHandle);

    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer *>(aEventSourceHandle);

    TInt handle(0);
    TInt error(eventSource->ExecuteTrap(&CMMAVideoControl::StaticSetForegroundL,
                                        videoControl,
                                        aIsForeground));
    if (error == KErrNone)
    {
        LOG(EJavaMMAPI, EInfo, "_1setForeground -");
        return handle;
    }

    LOG(EJavaMMAPI, EInfo, "_1setForeground -");
    return error;
}
JNIEXPORT jboolean JNICALL Java_com_nokia_microedition_media_control_VideoControl__1isESWT
(JNIEnv *, jobject)
{
    /*
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
    */
    // enable above code once you have support for eswt in 3.2.3, it is already enable for 5.0
    return false;
}
//  END OF FILE
