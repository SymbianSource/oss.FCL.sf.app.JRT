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



#ifndef BLUETOOTHPLATFORMCONTROL_H
#define BLUETOOTHPLATFORMCONTROL_H

#include <string>
#include "javaosheaders.h"

namespace java
{
namespace bluetooth
{


/**
 * BluetoothPlatformControl offers methods that can be used to get/set power,
 * discoverability mode.
 *
 * Usage:
 * BluetoothPlatformControl* control =
 *              BluetoothPlatformControl::GetBluetoothPlatformControl();
 *
 * control-><operation>
 */
class BluetoothPlatformControl
{
public:
    static const int BT_GIAC = 1;
    static const int BT_LIAC = 2;
    static const int BT_NOTDISCOVERABLE = 3;

    static const int BT_YES = 1;
    static const int BT_NO = 0;

    virtual ~BluetoothPlatformControl() { };

    static OS_IMPORT BluetoothPlatformControl* getBluetoothPlatformControl();

    /**
     * Returns the current device class value.
     * If any err, filled in aErr.
     */
    OS_IMPORT virtual unsigned int getBluetoothDeviceClass(int &aErr) = 0;

    /**
     * Returns the state of Bluetooth power on.
     * 0 if off, 1 if on.
     */
    OS_IMPORT virtual int getPowerState() = 0;

    /**
     * Returns the Discoverability mode of the device.
     * 3 Possible values:
     *     BT_NOTDISCOVERABLE - Not Discoverable
     *     BT_GIAC - GIAC
     *     BT_LIAC - LIAC
     */
    OS_IMPORT virtual int getVisibilityMode() = 0;

    /**
     * Prompts the user to change power state. i.e. to switch on bluetooth when
     * it is off.
     * Return values:
     *     BT_YES - If user agrees to power on
     *     BT_NO  - If user denies power on
     */
    OS_IMPORT virtual int askPower(
        const std::wstring& aAppName, const bool aInstalling) = 0;

    /**
     * Prompts the user to change discoverability mode. i.e. to switch on
     * bluetooth when it is off.
     * Return values:
     *     BT_YES - If user agrees to power on
     *     BT_NO  - If user denies power on
     */
    OS_IMPORT virtual int askDiscoverability(const std::wstring& aAppName,
            const bool aInstalling) = 0;

    /**
     * Can switch on or off device bluetooth.
     * Input:
     *     true - switch on
     *     false - switch off
     * Return values:
     *     0 - If no errors
     *     - <errcode> - If something goes wrong
     */
    OS_IMPORT virtual int setPower(const bool aPowerOn) = 0;

    /**
     * Can change device discoverability mode.
     * Input:
     *     BT_NOTDISCOVERABLE - To hide the device
     *     BT_GIAC - General discoverability mode
     *     BT_LIAC - Limited discoverability
     * Return values:
     *     0 - If no errors
     *     - <errcode> - If something goes wrong
     */
    OS_IMPORT virtual int setDiscoverability(const int aValue) = 0;


    /**
     * Returns platform specific bluetooth constants required by
     * System.getProperty and LocalDevice.getProperty methods.
     * Input: One of the values in bluetoothsystemproperty.h
     * Output: Also defined in bluetoothsystemproperty.h
     *
     */
    OS_IMPORT static std::wstring getSystemProperty(const int aProperty);


    /**
     * Returns bluetooth address
     * Output: bluetooth address
     */
    OS_IMPORT virtual std::wstring *getBluetoothAddress() = 0;


    /**
     * Returns bluetooth friendly name
     * Output: bluetooth friendly name
     */
    OS_IMPORT virtual std::wstring *getBluetoothName() = 0;
};

}  //end namespace bluetooth
}  //end namespace java

#endif // BLUETOOTHPLATFORMCONTROL_H



