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
* Description:  JNI  in UI thread context
*
*/

#include "org_eclipse_swt_widgets_ESWTProxyControl.h"
#include "logger.h"
//#include "cmmadcdisplay.h"
#include "cmmadisplay.h"

JNIEXPORT jintArray JNICALL Java_org_eclipse_swt_widgets_ESWTProxyControl__1getSourceSize
(JNIEnv *aJni, jobject, jint nativeDisplayHandle)
{
    LOG(EJavaMMAPI,EInfo,"JNI_eswtproxycontrol.cpp : getSourceSize +");
    LOG1(EJavaMMAPI,EInfo,"JNI_eswtproxycontrol.cpp : getSourceSize nativeDisplayHandle = %d", nativeDisplayHandle);
    CMMADisplay* display = reinterpret_cast<CMMADisplay*>(nativeDisplayHandle);
    TSize sourceRect = display->SourceSize();
    jint sourceWidth = sourceRect.iWidth;
    jint sourceHeight = sourceRect.iHeight;

    jintArray sourceSize = aJni->NewIntArray(2);
    aJni->SetIntArrayRegion(sourceSize,0,1,&sourceWidth);
    aJni->SetIntArrayRegion(sourceSize,1,1,&sourceHeight);
    LOG(EJavaMMAPI,EInfo,"JNI_eswtproxycontrol.cpp : getSourceSize -");
    return sourceSize;
}
