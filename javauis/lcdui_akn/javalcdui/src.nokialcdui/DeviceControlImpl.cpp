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
* Description:  JNI class for DeviceControlImpl.java.
*
*/


#include "com_nokia_mid_ui_impl_DeviceControlImpl.h"
#include "CMIDToolkit.h"
#include "CMIDDeviceControl.h"

/**
 * Class: com_nokia_mid_ui_impl_DeviceControlImpl
 * Method: _getUserInactivityTime
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mid_ui_impl_DeviceControlImpl__1getUserInactivityTime(
    JNIEnv* /* aJniEnv */, jobject /* aObject */,
    jint aToolkitHandle, jint aDeviceControlHandle)
{
    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);
    CMIDDeviceControl* dc =
        MIDUnhand< CMIDDeviceControl >(aDeviceControlHandle);
    TBool retVal;
    toolkit->ExecuteV(&CMIDDeviceControl::StaticGetUserInactivityTimeL,
                      dc, &retVal);

    return retVal;
}



/**
 * Class: com_nokia_mid_ui_impl_DeviceControlImpl
 * Method: _resetUserInactivityTime
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mid_ui_impl_DeviceControlImpl__1resetUserInactivityTime(
    JNIEnv* /* aJniEnv */, jobject /* aObject */,
    jint aToolkitHandle, jint aDeviceControlHandle)
{
    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);
    CMIDDeviceControl* dc =
        MIDUnhand< CMIDDeviceControl >(aDeviceControlHandle);
    TInt err = toolkit->ExecuteTrap(&CMIDDeviceControl::
                                    StaticResetUserInactivityTimeL, dc);
    return err;
}


/**
 * Class: com_nokia_mid_ui_impl_DeviceControlImpl
 * Method: _setLights
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mid_ui_impl_DeviceControlImpl__1setLights(
    JNIEnv* /* aJniEnv */, jobject /* aObject */,
    jint aToolkitHandle, jint aDeviceControlHandle, jint aLevel)
{
    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);
    CMIDDeviceControl* dc =
        MIDUnhand< CMIDDeviceControl >(aDeviceControlHandle);
    TInt err = toolkit->ExecuteTrap(&CMIDDeviceControl::StaticSetLightL,
                                    dc, aLevel);
    return err;
}

/**
 * Class: com_nokia_mid_ui_impl_DeviceControlImpl
 * Method: _flashLights
 * Signature: (III)B
 */
JNIEXPORT jboolean JNICALL
Java_com_nokia_mid_ui_impl_DeviceControlImpl__1flashLights(
    JNIEnv* /* aJniEnv */, jobject /* aObject */,
    jint aToolkitHandle, jint aDeviceControlHandle, jint aDuration)
{
    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);
    CMIDDeviceControl* dc =
        MIDUnhand< CMIDDeviceControl >(aDeviceControlHandle);
    TBool retVal;
    TInt err = toolkit->ExecuteTrap(&CMIDDeviceControl::StaticFlashBacklightL,
                                    dc, aDuration, &retVal);
    if (KErrNone != err)
    {
        return false;
    }
    return retVal;
}

/**
 * Class: com_nokia_mid_ui_impl_DeviceControlImpl
 * Method: _release
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_com_nokia_mid_ui_impl_DeviceControlImpl__1release(
    JNIEnv* /* aJniEnv */, jobject /* aObject */,
    jint aToolkitHandle, jint aDeviceControlHandle)
{
    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);
    CMIDDeviceControl* dc =
        MIDUnhand< CMIDDeviceControl >(aDeviceControlHandle);
    toolkit->ExecuteV(&CMIDDeviceControl::StaticRelease, dc);
}

void CreateDeviceControlL(CMIDToolkit* aToolkit, TInt* aHandle)
{
    CMIDDeviceControl* dc = CMIDDeviceControl::NewLC(*aToolkit->Env());

    *aHandle = aToolkit->RegisterComponentL(dc, NULL);
    CleanupStack::Pop(dc);
}

/**
 * Class: com_nokia_mid_ui_impl_DeviceControlImpl
 * Method: _createDeviceControl
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mid_ui_impl_DeviceControlImpl__1createDeviceControl(
    JNIEnv* /* aJniEnv */, jobject /* aObject */,
    jint aToolkitHandle)
{
    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);
    TInt handle = 0;
    TInt err = toolkit->ExecuteTrap(&CreateDeviceControlL, toolkit,
                                    &handle);
    return err == KErrNone ? handle : err;
}



