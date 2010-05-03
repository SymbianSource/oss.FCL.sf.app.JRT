/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  JNI file for ControlContainer
*
*/


#include <jutils.h>
#include <jdebug.h>

#include "com_nokia_amms_ControlContainer.h"

#include "cammscontrolgroup.h"
#include "cammsmodule.h"

/**
 * JNI function.
 */
JNIEXPORT jstring JNICALL Java_com_nokia_amms_ControlContainer__1getControlClassName
(JNIEnv* aJniEnv,
 jclass,
 jint /*aEventSourceHandle*/,
 jint aNativeHandle)
{
    CAMMSControlGroup* controlGroup =
        JavaUnhand< CAMMSControlGroup >(aNativeHandle);

    DEBUG_STR("AMMS::ControlContainer.cpp::getControlClassName name = %S",
              controlGroup->ClassName());

    return CreateJavaString(*aJniEnv, controlGroup->ClassName());
}

/**
 * JNI function.
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_ControlContainer__1getControlsCount
(JNIEnv*,
 jclass,
 jint /*aEventSourceHandle*/,
 jint aNativeHandle)
{
    CAMMSModule* module = JavaUnhand< CAMMSModule >(aNativeHandle);

    DEBUG_INT("AMMS::ControlContainer.cpp::getControlsCount count = %d",
              module->Count());
    return module->Count();
}

/**
 * JNI function.
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_ControlContainer__1getControlHandle
(JNIEnv*,
 jclass,
 jint /*aEventSourceHandle*/,
 jint aNativeHandle,
 jint aIndex)
{
    DEBUG_INT("AMMS::ControlContainer.cpp::getControlHandle index = %d",
              aIndex);
    CAMMSModule* module = JavaUnhand< CAMMSModule >(aNativeHandle);

    // Java handles must be created from CBase derived classes.
    // Casting here is safe because all groups in the modules are derived from
    // CAMMSControlGroup.
    CAMMSControlGroup* group = static_cast< CAMMSControlGroup* >(
                                   module->At(aIndex));     // CSI: 1 Wrong index means implementation error #

    return JavaMakeHandle(group);
}

//  End of File



