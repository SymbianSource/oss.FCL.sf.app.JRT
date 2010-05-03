/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#include "javajniutils.h"
#include "bluetoothconsts.h"
#include "bluetoothplatformcontrol.h"
#include "com_nokia_mj_impl_bluetooth_BTPlatformControl.h"
#include "logger.h"
#include "serviceclasshandler.h"
#include "exceptionbase.h"

using namespace java::bluetooth;
using namespace java::util;

/*
 * Class:     com_nokia_mj_impl_bluetooth_BTPlatformControl
 * Method:    getBluetoothAddress
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_com_nokia_mj_impl_bluetooth_BTPlatformControl__1getBluetoothAddress
(JNIEnv *aJni, jclass)
{
    JELOG2(EJavaBluetooth);
    BluetoothPlatformControl* control =
        BluetoothPlatformControl::getBluetoothPlatformControl();

    if (!control)
    {
        return NULL;
    }

    std::wstring *devAddress = control->getBluetoothAddress();
    jstring result;
    try
    {
        result = java::util::JniUtils::wstringToJstring(aJni, *devAddress);
    }
    catch (ExceptionBase ex)
    {
        result = NULL;
    }


    delete devAddress;
    delete control;

    return result;
}

/*
 * Class:     com_nokia_mj_impl_bluetooth_BTPlatformControl
 * Method:    getBluetoothName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_com_nokia_mj_impl_bluetooth_BTPlatformControl__1getBluetoothName
(JNIEnv *aJni, jclass)
{
    JELOG2(EJavaBluetooth);
    BluetoothPlatformControl* control =
        BluetoothPlatformControl::getBluetoothPlatformControl();

    if (!control)
    {
        return NULL;
    }

    std::wstring *devName = control->getBluetoothName();
    jstring result;
    try
    {
        result = java::util::JniUtils::wstringToJstring(aJni, *devName);
    }
    catch (ExceptionBase ex)
    {
        result = NULL;
    }


    delete devName;
    delete control;

    return result;
}


/*
 * Class:     com_nokia_mj_impl_bluetooth_BTPlatformControl
 * Method:    getBluetoothDiscoverable
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_bluetooth_BTPlatformControl__1getBluetoothDiscoverable
(JNIEnv* , jclass)
{
    JELOG2(EJavaBluetooth);
    BluetoothPlatformControl* control =
        BluetoothPlatformControl::getBluetoothPlatformControl();

    if (!control)
    {
        ELOG(EJavaBluetooth,
             "  JNI::getBluetoothDiscoverable Error in creating BluetoothPlatformControl");
        return NOT_DISCOVERABLE;
    }

    int result = control->getVisibilityMode();

    delete control;

    return result;
}

/*
 * Class:     com_nokia_mj_impl_bluetooth_BTPlatformControl
 * Method:    setBluetoothDiscoverable
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_com_nokia_mj_impl_bluetooth_BTPlatformControl__1setBluetoothDiscoverable
(JNIEnv* , jclass, jint aMode)
{
    JELOG2(EJavaBluetooth);
    BluetoothPlatformControl* control =
        BluetoothPlatformControl::getBluetoothPlatformControl();

    if (!control)
    {
        ELOG(EJavaBluetooth,
             "  JNI::setBluetoothDiscoverable Error in creating BluetoothPlatformControl");
        return false;
    }

    int result = control->setDiscoverability(aMode);

    delete control;

    return (0==result);
}

/*
 * Class:     com_nokia_mj_impl_bluetooth_BTPlatformControl
 * Method:    isPowerOn
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_com_nokia_mj_impl_bluetooth_BTPlatformControl__1isPowerOn
(JNIEnv* , jclass)
{
    JELOG2(EJavaBluetooth);
    BluetoothPlatformControl* control =
        BluetoothPlatformControl::getBluetoothPlatformControl();

    if (!control)
    {
        ELOG(EJavaBluetooth,
             "  JNI::isPowerOn Error in creating BluetoothPlatformControl");
        return false;
    }
    int result = control->getPowerState();

    delete control;

    return (BluetoothPlatformControl::BT_YES==result);
}

/*
 * Class:     com_nokia_mj_impl_bluetooth_BTPlatformControl
 * Method:    setPowerOn
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_com_nokia_mj_impl_bluetooth_BTPlatformControl__1setPowerOn
(JNIEnv* , jclass)
{
    JELOG2(EJavaBluetooth);
    BluetoothPlatformControl* control =
        BluetoothPlatformControl::getBluetoothPlatformControl();

    if (!control)
    {
        ELOG(EJavaBluetooth,
             "  JNI::setPowerOn Error in creating BluetoothPlatformControl");
        return false;
    }

    // Returns 0 if successful.
    int result = control->setPower(true);

    delete control;

    return (0 == result);
}

/*
 * Class:     com_nokia_mj_impl_bluetooth_BTPlatformControl
 * Method:    promptPowerOn
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL
Java_com_nokia_mj_impl_bluetooth_BTPlatformControl__1promptPowerOn
(JNIEnv *aJni, jclass, jstring appName, jboolean aIsStatic)
{
    JELOG2(EJavaBluetooth);
    std::wstring name;

    try
    {
        name = java::util::JniUtils::jstringToWstring(aJni, appName);
    }
    catch (ExceptionBase ex)
    {
        name=L"Unknown Application";
    }

    BluetoothPlatformControl* control =
        BluetoothPlatformControl::getBluetoothPlatformControl();

    if (!control)
    {
        ELOG(EJavaBluetooth,
             "  JNI::promptPowerOn Error in creating BluetoothPlatformControl");
        return false;
    }

    int result;
    if (aIsStatic)
    {
        result = control->askPower(name, true);
    }
    else
    {
        result = control->askPower(name, false);
    }

    delete control;

    return (BluetoothPlatformControl::BT_YES==result);
}

/*
 * Class:     com_nokia_mj_impl_bluetooth_BTPlatformControl
 * Method:    promptVisibilityChange
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL
Java_com_nokia_mj_impl_bluetooth_BTPlatformControl__1promptVisibilityChange
(JNIEnv *aJni, jclass, jstring appName,jboolean aIsStatic)
{
    JELOG2(EJavaBluetooth);
    std::wstring name;

    try
    {
        name = java::util::JniUtils::jstringToWstring(aJni, appName);
    }
    catch (ExceptionBase ex)
    {
        name=L"Unknown Application";
    }

    BluetoothPlatformControl* control =
        BluetoothPlatformControl::getBluetoothPlatformControl();

    if (!control)
    {
        ELOG(EJavaBluetooth,
             "  JNI::promptVisibilityChange Error in creating BluetoothPlatformControl");
        return false;
    }

    int result;
    if (aIsStatic)
    {
        result = control->askDiscoverability(name, true);
    }
    else
    {
        result = control->askDiscoverability(name, false);
    }

    delete control;

    return (BluetoothPlatformControl::BT_YES==result);
}

/*
 * Class:     com_nokia_mj_impl_bluetooth_BTPlatformControl
 * Method:    getProperty
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_com_nokia_mj_impl_bluetooth_BTPlatformControl__1getProperty
(JNIEnv *aJni, jclass, jint aPropertyId)
{
    JELOG2(EJavaBluetooth);
    BluetoothPlatformControl* control =
        BluetoothPlatformControl::getBluetoothPlatformControl();

    if (!control)
    {
        ELOG(EJavaBluetooth,
             "  JNI::getProperty Error in creating BluetoothPlatformControl");
        return NULL;
    }

    jstring result;

    try
    {
        result =  java::util::JniUtils::wstringToJstring(aJni,
                  control->getSystemProperty(aPropertyId));
    }
    catch (ExceptionBase ex)
    {
        result = NULL;
    }

    delete control;
    return result;
}


/*
 * Class:     com_nokia_mj_impl_bluetooth_BTPlatformControl
 * Method:    setServiceClasses
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_bluetooth_BTPlatformControl__1setServiceClasses
(JNIEnv* /*aJni*/, jclass, jint aServiceClassBits)
{
    JELOG2(EJavaBluetooth);
    return ServiceClassHandler::setDeviceServiceClass(aServiceClassBits);
}


/*
 * Class:     com_nokia_mj_impl_bluetooth_BTPlatformControl
 * Method:    getBluetoothDeviceClass
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_bluetooth_BTPlatformControl__1getBluetoothDeviceClass
(JNIEnv* , jclass)
{
    JELOG2(EJavaBluetooth);
    BluetoothPlatformControl* control =
        BluetoothPlatformControl::getBluetoothPlatformControl();
    if (!control)
    {
        ELOG(EJavaBluetooth,
             "  JNI::getBluetoothDeviceClass Error in creating BluetoothPlatformControl");
        return 0;
    }

    int err = 0;
    int ret = control->getBluetoothDeviceClass(err);

    delete control;

    if (err)
    {
        ELOG1(EJavaBluetooth, "  JNI::getBluetoothDeviceClass Error:%d", err);
        return 0;
    }
    return ret;
}
