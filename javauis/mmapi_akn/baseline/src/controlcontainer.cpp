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
* Description:  This file contains ControlContainer JNI functions
*
*/

#include <jdebug.h>
#include "com_nokia_microedition_media_ControlContainer.h"

#include "cmmaplayer.h"
#include "cmmacontrol.h"


/**
 * JNI function
 */
JNIEXPORT jstring JNICALL Java_com_nokia_microedition_media_ControlContainer__1getControlClassName
(JNIEnv* aJni, jclass,
 jint aControlHandle)
{
    CMMAControl* control =
        JavaUnhand< CMMAControl >(aControlHandle);

    const TDesC& className = control->ClassName();

    DEBUG_STR("MMA::ControlContainer.cpp::getControlClassName name = %S",
              className);

    // Create Java String from class name and return it
    return (className == KNullDesC) ? NULL : CreateJavaString(*aJni, className);
}

/**
 * JNI function
 */
JNIEXPORT jstring JNICALL Java_com_nokia_microedition_media_ControlContainer__1getPublicControlClassName
(JNIEnv* aJni, jclass,
 jint aControlHandle)
{
    CMMAControl* control =
        JavaUnhand< CMMAControl >(aControlHandle);

    const TDesC& className = control->PublicClassName();

    DEBUG_STR("MMA::ControlContainer.cpp::getPublicControlClassName name = %S",
              className);

    // Create Java String from class name and return it
    return (className == KNullDesC) ? NULL : CreateJavaString(*aJni, className);
}

/**
 * JNI function
 */
JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_ControlContainer__1getControlsCount
(JNIEnv*, jclass,
 jint aPlayerHandle)
{
    CMMAPlayer* player =
        JavaUnhand< CMMAPlayer >(aPlayerHandle);
    DEBUG_INT("MMA::ControlContainer.cpp::getControlsCount count = %d",
              player->ControlCount());
    return player->ControlCount();
}

/**
 * JNI function
 */
JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_ControlContainer__1getControlHandle
(JNIEnv*, jclass,
 jint aPlayerHandle, jint aControlIndex)
{
    CMMAPlayer* player =
        JavaUnhand< CMMAPlayer >(aPlayerHandle);

    // Get control and make Java handle
    return JavaMakeHandle(player->Control(aControlIndex));
}

//  END OF FILE
