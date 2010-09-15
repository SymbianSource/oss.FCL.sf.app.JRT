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



#include <bttypes.h>
#include <bt_sock.h>
#include <bt_subscribe.h>
#include <btmanclient.h>

#include "logger.h"
#include "javaosheaders.h"
#include "bluetoothplatformplugin.h"
#include "bluetoothplatformconsts.h"
#include "bluetoothsystemproperty.h"
#include "bluetoothplatformcontrol.h"
#include "s60bluetoothplatformcontrol.h"
#include "javasymbianoslayer.h"

using namespace java::bluetooth;


_LIT(KAskYes, "yes");

OS_EXPORT BluetoothPlatformControl*
BluetoothPlatformControl::getBluetoothPlatformControl()
{
    JELOG2(EJavaBluetooth);
    S60BluetoothPlatformControl* control = NULL;

    TRAPD(err, control = S60BluetoothPlatformControl::NewL());
    if (KErrNone != err)
        return NULL;

    return control;
}

S60BluetoothPlatformControl::S60BluetoothPlatformControl()
{
    JELOG2(EJavaBluetooth);
    mPlatformPlugin = NULL;
}

S60BluetoothPlatformControl* S60BluetoothPlatformControl::NewL()
{
    S60BluetoothPlatformControl *self = new(ELeave) S60BluetoothPlatformControl();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

void S60BluetoothPlatformControl::ConstructL()
{
    JELOG2(EJavaBluetooth);
    mPlatformPlugin = CBluetoothPlatformPlugin::NewL();
}

S60BluetoothPlatformControl::~S60BluetoothPlatformControl()
{
    JELOG2(EJavaBluetooth);
    delete mPlatformPlugin;
}

OS_EXPORT std::wstring BluetoothPlatformControl::getSystemProperty(const int aProperty)
{
    JELOG2(EJavaBluetooth);

    std::wstring propertyValue;
    switch (aProperty)
    {
        // The maximum ReceiveMTU size in bytes supported in L2CAP.
    case BT_L2CAP_RECEIVEMTU_MAX:
        propertyValue = BT_L2CAP_RECEIVEMTU_MAX_V;
        break;

        // The maximum number of connected devices supported.
    case BT_CONNECTED_DEVICES_MAX:
        propertyValue = BT_CONNECTED_DEVICES_MAX_V;
        break;

        // Is inquiry allowed during a connection?
    case BT_CONNECTED_INQUIRY:
        propertyValue = BT_CONNECTED_INQUIRY_V;
        break;

        // Is paging allowed during a connection?
    case BT_CONNECTED_PAGE:
        propertyValue = BT_CONNECTED_PAGE_V;
        break;

        // Is inquiry scanning allowed during connection?
    case BT_CONNECTED_INQUIRY_SCAN:
        propertyValue = BT_CONNECTED_INQUIRY_SCAN_V;
        break;

        // Is page scanning allowed during connection?
    case BT_CONNECTED_PAGE_SCAN:
        propertyValue = BT_CONNECTED_PAGE_SCAN_V;
        break;

        // Is master/slave switch allowed?
    case BT_MASTER_SWITCH:
        propertyValue = BT_MASTER_SWITCH_V;
        break;

        // Maximum number of concurrent service discovery transactions
    case BT_SD_TRANS_MAX:
        propertyValue = BT_SD_TRANS_MAX_V;
        break;

        // Maximum no. of service attributes to be retrieved per service record.
    case BT_SD_ATTR_RETRIEVE_MAX:
        propertyValue = BT_SD_ATTR_RETRIEVE_MAX_V;
        break;

    default:
        propertyValue =L"" ;
    }
    return propertyValue;
}


OS_EXPORT unsigned int S60BluetoothPlatformControl::getBluetoothDeviceClass(int &aErr)
{
    JELOG2(EJavaBluetooth);

    TInt currentCODVal = 0;

    aErr = 0;

    TInt error = RProperty::Get(KPropertyUidBluetoothCategory,
                                KPropertyKeyBluetoothGetDeviceClass,
                                currentCODVal);

    if (KErrNone != error)
    {
        aErr = error;
        return 0;
    }

    LOG1(EJavaBluetooth, EInfo,
         "- S60BluetoothPlatformControl::getCurrentCOD 0x%X",
         currentCODVal);

    return (unsigned int)currentCODVal;
}


/**
 * Returns the state of Bluetooth power on.
 * 0 if off, 1 if on.
 */
int S60BluetoothPlatformControl::getPowerState()
{
    JELOG2(EJavaBluetooth);
    TInt value;

    mPlatformPlugin->GetProperty(KPropertyPowerOn(), value);
    return value ? BluetoothPlatformControl::BT_YES
           : BluetoothPlatformControl::BT_NO;
}

/**
 * Returns the Discoverability mode of the device.
 * 3 Possible values:
 *     BT_NOTDISCOVERABLE - Not Discoverable
 *     BT_GIAC - GIAC
 *     BT_LIAC - LIAC
 */
int S60BluetoothPlatformControl::getVisibilityMode()
{
    JELOG2(EJavaBluetooth);
    TInt value;

    mPlatformPlugin->GetProperty(KPropertyDiscoverability(), value);

    LOG1(EJavaBluetooth, EInfo,
         "S60BluetoothPlatformControl::getVisibilityMode %x", value);

    return value;
}

/**
 * Prompts the user to change power state. i.e. to switch on bluetooth when
 * it is off.
 * Return values:
 *     BT_YES - If user agrees to power on
 *     BT_NO  - If user denies power on
 */
int S60BluetoothPlatformControl::askPower(const std::wstring& aAppName,
        const bool aInstalling)
{
    JELOG2(EJavaBluetooth);
    LOG(EJavaBluetooth, EInfo, "+ S60BluetoothPlatformControl::AskPower()");

    TInt maxResponseLength = 5;
    HBufC* response = HBufC::New(maxResponseLength);
    TPtrC resPtr(response->Des());

    HBufC* app = wstringToBuf(aAppName);
    TPtrC appname(app->Des());

    TRAPD(err, mPlatformPlugin->AskPowerL(appname, aInstalling, resPtr));
    if (KErrNone != err)
    {
        ELOG1(EJavaBluetooth,
              "# S60BluetoothPlatformControl::AskPower() Query Failed %d", err);
        //Not getting result from user is bad. We must not switch on BT.
        return BluetoothPlatformControl::BT_NO;
    }

    int retVal = 0;
    if (resPtr == KAskYes())
    {
        retVal = BluetoothPlatformControl::BT_YES;
    }
    else
    {
        retVal = BluetoothPlatformControl::BT_NO;
    }

    LOG1(EJavaBluetooth, EInfo,
         "- S60BluetoothPlatformControl::AskPower(): %d", retVal);
    return retVal;
}

/**
 * Prompts the user to change discoverability mode. i.e. to switch on
 * bluetooth when it is off.
 * Return values:
 *     BT_YES - If user agrees to power on
 *     BT_NO  - If user denies power on
 */
int S60BluetoothPlatformControl::askDiscoverability(
    const std::wstring& aAppName, const bool aInstalling)
{
    JELOG2(EJavaBluetooth);
    LOG(EJavaBluetooth, EInfo,
        "+ S60BluetoothPlatformControl::AskDiscoverability()");

    TInt maxResponseLength = 5;
    HBufC* response = HBufC::New(maxResponseLength);
    TPtrC resPtr(response->Des());

    HBufC* app = wstringToBuf(aAppName);
    TPtrC appname(app->Des());

    TRAPD(err, mPlatformPlugin->AskDiscoverabilityL(appname, aInstalling,
            resPtr));
    if (KErrNone != err)
    {
        ELOG1(EJavaBluetooth,
              "# S60BluetoothPlatformControl::AskDiscoverability() Query Failed %d", err);
        //Not getting result from user is bad. We must not switch on BT.
        return BluetoothPlatformControl::BT_NO;
    }

    int retVal = 0;
    if (resPtr == KAskYes())
    {
        retVal = BluetoothPlatformControl::BT_YES;
    }
    else
    {
        retVal = BluetoothPlatformControl::BT_NO;
    }

    LOG1(EJavaBluetooth, EInfo,
         "- S60BluetoothPlatformControl::AskDiscoverability(): %d", retVal);
    return retVal;
}

/**
 * Can switch on or off device bluetooth.
 * Input:
 *     true - switch on
 *     false - switch off
 * Return values:
 *     0 - If no errors
 *     - <errcode> - If something goes wrong
 */
int S60BluetoothPlatformControl::setPower(const bool aPowerOn)
{
    JELOG2(EJavaBluetooth);
    LOG(EJavaBluetooth, EInfo, "+ S60BluetoothPlatformControl::SetPower()");

    int status = mPlatformPlugin->SetPower(aPowerOn);
    LOG(EJavaBluetooth, EInfo, "- S60BluetoothPlatformControl::SetPower()");
    return status;
}

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
int S60BluetoothPlatformControl::setDiscoverability(const int aValue)
{
    JELOG2(EJavaBluetooth);
    LOG1(EJavaBluetooth, EInfo,
         "+ S60BluetoothPlatformControl::SetDiscoverability() aValue=%x", aValue);

    TInt status = mPlatformPlugin->SetDiscoverability(aValue);
    LOG(EJavaBluetooth, EInfo,
        "- S60BluetoothPlatformControl::SetDiscoverability()");
    return status;
}

/**
 * Returns bluetooth address
 * Output: bluetooth address
 */
std::wstring *S60BluetoothPlatformControl::getBluetoothAddress()
{
    TPckgBuf<TBTDevAddr> localDeviceBluetoothAddress;
    TBuf16<20> bluetoothAddress;

    JELOG2(EJavaBluetooth);
    TInt error = RProperty::Get(KPropertyUidBluetoothCategory,
                                KPropertyKeyBluetoothGetLocalDeviceAddress,
                                localDeviceBluetoothAddress);

    if (error != KErrNone)
    {
        return NULL;
    }

    localDeviceBluetoothAddress().GetReadable(bluetoothAddress);
    std::wstring *deviceAddress =
        new std::wstring((wchar_t*) bluetoothAddress.Ptr());
    deviceAddress->resize(bluetoothAddress.Length());
    return deviceAddress;
}

/**
 * Returns bluetooth friendly name
 * Output: bluetooth friendly name
 */
std::wstring *S60BluetoothPlatformControl::getBluetoothName()
{
    TBuf16<KMaxBluetoothNameLen> bluetoothName;

    JELOG2(EJavaBluetooth);
    TInt error = RProperty::Get(KPropertyUidBluetoothCategory,
                                KPropertyKeyBluetoothGetDeviceName, bluetoothName);

    if (error != KErrNone)
    {
        return NULL;
    }

    std::wstring *deviceName =
        new std::wstring((wchar_t*) bluetoothName.Ptr());

    deviceName->resize(bluetoothName.Length());
    return deviceName;
}
