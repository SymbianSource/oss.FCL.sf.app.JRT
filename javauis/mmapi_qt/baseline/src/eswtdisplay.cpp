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
* Description:
*
*/
#include "com_nokia_microedition_media_control_MMAeSWTDisplay.h"
#include "logger.h"
#include "qwidget.h"
#include "cmmadcdisplay.h"

JNIEXPORT void JNICALL Java_com_nokia_microedition_media_control_MMAeSWTDisplay__1setVisible
(JNIEnv *, jobject, jint nativeDisplayHandle, jboolean isVisible)
{
    LOG(EJavaMMAPI,EInfo,"JNI_eswtdisplay.cpp : setVisible +");
    CMMADisplay* display = reinterpret_cast<CMMADisplay*>(nativeDisplayHandle);
    TBool visible = (TBool)isVisible;
    display ->SetVisible(visible);
    LOG(EJavaMMAPI,EInfo,"JNI_eswtdisplay.cpp : setVisible -");
}


JNIEXPORT void JNICALL Java_com_nokia_microedition_media_control_MMAeSWTDisplay__1setContainerVisible
(JNIEnv *, jobject, jint /*nativeDisplayHandle*/, jboolean /*isActive*/)
{
    LOG(EJavaMMAPI,EInfo,"JNI_eswtdisplay.cpp : setContainerVisible +");
    /*        CMMADisplay* display = reinterpret_cast<CMMADisplay*>(nativeDisplayHandle);
            TBool visible = (TBool)isActive;
            display ->SetContainerVisibility(visible);*/
    LOG(EJavaMMAPI,EInfo,"JNI_eswtdisplay.cpp : setContainerVisible -");
}

JNIEXPORT void JNICALL Java_com_nokia_microedition_media_control_MMAeSWTDisplay__1setFullScreenMode
(JNIEnv *, jobject, jint nativeDisplayHandle, jboolean isFullScreen)
{
    LOG(EJavaMMAPI,EInfo,"JNI_eswtdisplay.cpp : setFullScreenMode  +");
    CMMADisplay* display = reinterpret_cast<CMMADisplay*>(nativeDisplayHandle);
    TBool mode = (TBool)isFullScreen;
    TRAP_IGNORE(display ->SetFullScreenL(mode));
    LOG(EJavaMMAPI,EInfo,"JNI_eswtdisplay.cpp : setFullScreenMode  -");
}

JNIEXPORT void JNICALL Java_com_nokia_microedition_media_control_MMAeSWTDisplay__1setWindowToNative
(JNIEnv *, jobject,jint nativeDisplayHandle, jint qtWidgetHandle)
{
    LOG(EJavaMMAPI,EInfo,"JNI_eswtdisplay.cpp : setWindowToNative + ");
    QWidget *widget = reinterpret_cast<QWidget*>(qtWidgetHandle);
    CMMADisplay* display = reinterpret_cast<CMMADisplay*>(nativeDisplayHandle);
    display->SetWindowResources(widget);
    LOG(EJavaMMAPI,EInfo,"JNI_eswtdisplay.cpp : setWindowToNative - ");
}

LOCAL_C void LocalSourceSizeChanged(CMMADisplay* display,TInt width,TInt height)
{
    TRect temp(0,0,0,0);
    LOG2(EJavaMMAPI,EInfo,"JNI_eswtdisplay.cpp : setWindowToNative + %d %d",width,height);
    display->SourceSizeChanged(width,height,0,0,temp);
}

JNIEXPORT void JNICALL Java_com_nokia_microedition_media_control_MMAeSWTDisplay__1setNativeWindowSize
(JNIEnv *, jobject,jint nativeDisplayHandle, jint /*aEventSourceHandle*/, jint eswtControlwidth,jint eswtControlheight)
{
    LOG(EJavaMMAPI,EInfo,"JNI_eswtdisplay.cpp : setNativeWindowSize + ");
    CMMADisplay* display = reinterpret_cast<CMMADisplay*>(nativeDisplayHandle);
    TInt width = eswtControlwidth;
    TInt height = eswtControlheight;
    // TSize srcsize = TSize(360,487);
    // TO-DO remove JavaUnhand in final code
    //MMAFunctionServer* eventSource =  reinterpret_cast<MMAFunctionServer *>(aEventSourceHandle);
    // need to call in Function server thread context because CMMADisplay's JNI pointer
    // is valid in that thread context only.
    /*TInt error = eventSource->ExecuteTrap(&LocalSourceSizeChanged,
                                                          display,
                                                          width,
                                                          height);*/
    LocalSourceSizeChanged(display, width, height);

    LOG(EJavaMMAPI,EInfo,"JNI_eswtdisplay.cpp : setNativeWindowSize - ");
}

/*
LOCAL_C void LocalSetDisplayPosition(CMMADisplay* display,TInt uiX,TInt uiY,TInt vcX,TInt vcY)
{
    display->SetDisplayPosition(uiX,uiY,vcX,vcY);
}
*/
JNIEXPORT void JNICALL Java_com_nokia_microedition_media_control_MMAeSWTDisplay__1setPosition
(JNIEnv *, jobject,jint /*nativeDisplayHandle*/,jint /*aEventSourceHandle*/,jint /*uiX*/,jint /*uiY*/,jint /*vcX*/,jint /*vcY*/)
{
    /*  LOG(EJavaMMAPI,EInfo,"JNI_canvasdisplay.cpp : _setPosition + ");
      CMMADisplay* display = reinterpret_cast<CMMADisplay*>(nativeDisplayHandle);
      TInt uiControlLocationX = uiX;
      TInt uiControlLocationY = uiY;
      TInt videoControlLocationX = vcX;
      TInt videoControlLocationY = vcY;
      MMAFunctionServer* eventSource =  reinterpret_cast<MMAFunctionServer *>(aEventSourceHandle);
      // need to call in Function server thread context because CMMADisplay's JNI pointer
      // is valid in that thread context only.
      TInt error = eventSource->ExecuteTrap(&LocalSetDisplayPosition,
                                                               display,
                                                                     uiControlLocationX,
                                                                             uiControlLocationY,
                                                                                     videoControlLocationX,
                                                                                             videoControlLocationY);

      LOG(EJavaMMAPI,EInfo,"JNI_canvasdisplay.cpp : _setPosition - ");*/
}




LOCAL_C void LocalSetDisplaySize(CMMADisplay* display,TInt vcX,TInt vcY)
{
    LOG2(EJavaMMAPI,EInfo,"JNI_canvasdisplay.cpp : LocalSetDisplaySize vcX = %d,vcY=%d ",vcX,vcY);
    TSize size(vcX,vcY);
    TRAP_IGNORE(display->SetDisplaySizeL(size));
}

JNIEXPORT void JNICALL Java_com_nokia_microedition_media_control_MMAeSWTDisplay__1setDisplaySize
(JNIEnv *, jobject,jint nativeDisplayHandle,jint /*aEventSourceHandle*/,jint width,jint height)
{
    LOG2(EJavaMMAPI,EInfo,"JNI_eswtDisplay.cpp : _setDisplaySize +  width = %d,height = %d",width,height);
    CMMADisplay* display = reinterpret_cast<CMMADisplay*>(nativeDisplayHandle);
    TInt vc_width = width;
    TInt vc_height = height;
    //MMAFunctionServer* eventSource =  reinterpret_cast<MMAFunctionServer *>(aEventSourceHandle);
    // need to call in Function server thread context because CMMADisplay's JNI pointer
    // is valid in that thread context only.
    LOG2(EJavaMMAPI,EInfo,"JNI_eswtdisplay.cpp : _setDisplaySize +  vc_width = %d,vc_height = %d",vc_width,vc_height);
    /*TInt error = eventSource->ExecuteTrap(&LocalSetDisplaySize,
                                                             display,
                                                                   vc_width,
                                                                           vc_height);
    */
    LocalSetDisplaySize(display, vc_width, vc_height);
    LOG(EJavaMMAPI,EInfo,"JNI_ESWTDisplay.cpp : _setDisplaySize - ");
}
