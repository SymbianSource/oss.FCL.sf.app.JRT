/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ItemDisplay JNI
*
*/

#include "logger.h"
#include "com_nokia_microedition_media_control_ItemDisplay.h"
#include "cmmaitemdisplay.h"
#include "qwidget.h"
#include "cmmasurfacewindow.h"


/*
 * Class:     com_nokia_microedition_media_control_itemdisplay
 * Method:    _setVisible
 * Signature: (IIZ)I
 */
JNIEXPORT void JNICALL Java_com_nokia_microedition_media_control_ItemDisplay__1setVisible
(JNIEnv *, jobject, jint nativeDisplayHandle, jboolean isVisible)
{
    LOG(EJavaMMAPI,EInfo,"ItemDisplayJni : setVisible +");
    CMMADisplay* display = reinterpret_cast<CMMADisplay*>(nativeDisplayHandle);
    TBool visible = (TBool)isVisible;
    display ->SetVisible(visible);
    LOG(EJavaMMAPI,EInfo,"ItemDisplayJni : setVisible -");
}

LOCAL_C void LocalSetContainerVisible(CMMADisplay* display,TBool visible)
{
    display ->SetContainerVisibility(visible);
}

JNIEXPORT void JNICALL Java_com_nokia_microedition_media_control_ItemDisplay__1setContainerVisible
(JNIEnv *, jobject, jint aEventSourceHandle,jint nativeDisplayHandle, jboolean aIsActive)
{
    LOG(EJavaMMAPI,EInfo,"ItemDisplayJni : setContainerVisible +");
    MMAFunctionServer* eventSource =  reinterpret_cast<MMAFunctionServer *>(aEventSourceHandle);
    CMMADisplay* display = reinterpret_cast<CMMADisplay*>(nativeDisplayHandle);
    TBool visible = (TBool)aIsActive;
    eventSource->ExecuteTrap(&LocalSetContainerVisible,display,visible);
    LOG(EJavaMMAPI,EInfo,"itemdisplayjni.cpp : setContainerVisible -");
}

JNIEXPORT void JNICALL Java_com_nokia_microedition_media_control_ItemDisplay__1setFullScreenMode
(JNIEnv *, jobject, jint nativeDisplayHandle, jboolean isFullScreen)
{
    LOG(EJavaMMAPI,EInfo,"ItemDisplayJni : setFullScreenMode  +");
    CMMADisplay* display = reinterpret_cast<CMMADisplay*>(nativeDisplayHandle);
    TBool mode = (TBool)isFullScreen;
    TRAP_IGNORE(display ->SetFullScreenL(mode));
    LOG(EJavaMMAPI,EInfo,"ItemDisplayJni : setFullScreenMode  -");
}

JNIEXPORT void JNICALL Java_com_nokia_microedition_media_control_ItemDisplay__1setWindowToNative
(JNIEnv *, jobject,jint nativeDisplayHandle, jint qtWidgetHandle)
{
    LOG(EJavaMMAPI,EInfo,"ItemDisplayJni : setWindowToNative + ");
    QWidget *widget = reinterpret_cast<QWidget*>(qtWidgetHandle);
    CMMADisplay* display = reinterpret_cast<CMMADisplay*>(nativeDisplayHandle);
    display->SetWindowResources(widget);
    LOG(EJavaMMAPI,EInfo,"ItemDisplayJni : setWindowToNative - ");
}


/*
JNIEXPORT void JNICALL Java_com_nokia_microedition_media_control_ItemDisplay__1nativeDummy
(JNIEnv *, jobject,)
{
*/

LOCAL_C void LocalSourceSizeChanged(CMMADisplay* display,TInt width,TInt height,TInt x,TInt y,TRect aFormRect)
{
    display->SourceSizeChanged(width,height,x,y,aFormRect);
}

JNIEXPORT void JNICALL Java_com_nokia_microedition_media_control_ItemDisplay__1setNativeWindowSize
(JNIEnv *aJni, jobject,jint nativeDisplayHandle, jint aEventSourceHandle, jint eswtControlwidth,jint eswtControlheight,jint aItemXCoordinate,jint aItemYCoordinate,
 jintArray aFormArray)
{
    LOG(EJavaMMAPI,EInfo,"ItemDisplayJni : setNativeWindowSize + ");
    CMMADisplay* display = reinterpret_cast<CMMADisplay*>(nativeDisplayHandle);
    TInt width = eswtControlwidth;
    TInt height = eswtControlheight;
    TInt x = aItemXCoordinate;
    TInt y = aItemYCoordinate;

    jint * formbounds = aJni->GetIntArrayElements(aFormArray,NULL);
    TInt a = formbounds[0];
    TInt b = formbounds[1];
    TInt w = formbounds[2];
    TInt h = formbounds[3];
    aJni->ReleaseIntArrayElements(aFormArray,formbounds,0);
    TRect formRect(TPoint(a,b),TPoint(w,h));
    MMAFunctionServer* eventSource =  reinterpret_cast<MMAFunctionServer *>(aEventSourceHandle);
    // need to call in Function server thread context because CMMADisplay's JNI pointer
    // is valid in that thread context only.
    TInt error = eventSource->ExecuteTrap(&LocalSourceSizeChanged,
                                          display,
                                          width,
                                          height,x,y,formRect);

    LOG(EJavaMMAPI,EInfo,"ItemDisplayJni : setNativeWindowSize - ");
}
