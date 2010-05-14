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
#include "logger.h"
#include "com_nokia_microedition_media_control_BaseDisplay.h"
#include "cmmadisplay.h"

/*
 * Class:     com_nokia_microedition_media_control_BaseDisplay
 * Method:    _setVisible
 * Signature: (IIZ)I
 */
JNIEXPORT void JNICALL Java_com_nokia_microedition_media_control_BaseDisplay__1nativeMethodCallInUiThread
  (JNIEnv *, jobject, jint nativeDisplayHandle, jint nativeFunctionID)
  {
   LOG(EJavaMMAPI,EInfo,"JNI_Display.cpp : nativeMethodCallInUiThread +");
	 CMMADisplay* display = reinterpret_cast<CMMADisplay*>(nativeDisplayHandle);
   display ->CalledBackInUiThread(nativeFunctionID);
   LOG(EJavaMMAPI,EInfo,"JNI_Display.cpp : nativeMethodCallInUiThread -");
  }

