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


#ifndef S60BLUETOOTHPLATFORMCONTROL_H
#define S60BLUETOOTHPLATFORMCONTROL_H

#include "bluetoothplatformcontrol.h"
#include "bluetoothplatformplugin.h"

namespace java
{
namespace bluetooth
{

class S60BluetoothPlatformControl : public BluetoothPlatformControl
{
public:

    ~S60BluetoothPlatformControl();


    //Methods from BluetoothPlatformPlugin

    /**
     * Returns the state of Bluetooth power on.
     * 0 if off, 1 if on.
     */
    int getPowerState();


    /**
     * Returns the current device class value.
     * If any err, filled in aErr.
     */
    unsigned int getBluetoothDeviceClass(int &aErr);

    /**
     * Returns the Discoverability mode of the device.
     * 3 Possible values:
     *     BT_NOTDISCOVERABLE - Not Discoverable
     *     BT_GIAC - GIAC
     *     BT_LIAC - LIAC
     */
    int getVisibilityMode();

    /**
     * Prompts the user to change power state. i.e. to switch on bluetooth when
     * it is off.
     * Return values:
     *     BT_YES - If user agrees to power on
     *     BT_NO  - If user denies power on
     */
    int askPower(const std::wstring& aAppName, const bool aInstalling);

    /**
     * Prompts the user to change discoverability mode. i.e. to switch on
     * bluetooth when it is off.
     * Return values:
     *     BT_YES - If user agrees to power on
     *     BT_NO  - If user denies power on
     */
    int askDiscoverability(
        const std::wstring& aAppName, const bool aInstalling);

    /**
     * Can switch on or off device bluetooth.
     * Input:
     *     true - switch on
     *     false - switch off
     * Return values:
     *     0 - If no errors
     *     - <errcode> - If something goes wrong
     */
    int setPower(const bool aPowerOn);

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
    int setDiscoverability(const int aValue);

    /**
     * Returns platform specific bluetooth constants required by
     * System.getProperty and LocalDevice.getProperty methods.
     * Input: One of the values in bluetoothsystemproperty.h
     * Output: Also defined in bluetoothsystemproperty.h
     *
     */
    std::wstring getSystemProperty(const int aProperty);

    /**
     * Returns bluetooth address
     * Output: bluetooth address
     */
    std::wstring *getBluetoothAddress();

    /**
     * Returns bluetooth friendly name
     * Output: bluetooth friendly name
     */
    std::wstring *getBluetoothName();

    static S60BluetoothPlatformControl* NewL();

private:
    S60BluetoothPlatformControl();
    void ConstructL();

private:
    CBluetoothPlatformPlugin* mPlatformPlugin;
};

}  //end namespace bluetooth
}  //end namespace java

#endif // S60BLUETOOTHPLATFORMCONTROL_H
