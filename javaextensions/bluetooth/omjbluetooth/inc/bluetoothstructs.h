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


#ifndef BLUETOOTHSTRUCTS_H
#define BLUETOOTHSTRUCTS_H

namespace java
{
namespace bluetooth
{

#define STATUS_DEVICE_FOUND       1

class DiscoveredDevice
{
public:
    DiscoveredDevice()
    {
        mInquiryStatus = 0;
        mDeviceClass = 0;
        mErrVal = 0;
        mDeviceAddr = NULL;
        mDeviceName = NULL;
    }

    ~DiscoveredDevice()
    {
        delete mDeviceAddr;
        delete mDeviceName;
    }

    DiscoveredDevice(const DiscoveredDevice &aDiscoveredDevice)
    {
        mInquiryStatus = aDiscoveredDevice.mInquiryStatus;
        mDeviceClass = aDiscoveredDevice.mDeviceClass;
        mErrVal = aDiscoveredDevice.mErrVal;
        if (aDiscoveredDevice.mDeviceAddr)
            mDeviceAddr = new std::wstring(*(aDiscoveredDevice.mDeviceAddr));
        if (aDiscoveredDevice.mDeviceName)
            mDeviceName = new std::wstring(*(aDiscoveredDevice.mDeviceName));
    }

    DiscoveredDevice& DiscoveredDevice::operator=(
        const DiscoveredDevice &aDiscoveredDevice)
    {
        mInquiryStatus = aDiscoveredDevice.mInquiryStatus;
        mDeviceClass = aDiscoveredDevice.mDeviceClass;
        mErrVal = aDiscoveredDevice.mErrVal;
        if (aDiscoveredDevice.mDeviceAddr)
            mDeviceAddr = new std::wstring(*(aDiscoveredDevice.mDeviceAddr));
        if (aDiscoveredDevice.mDeviceName)
            mDeviceName = new std::wstring(*(aDiscoveredDevice.mDeviceName));

        return *this;
    }

    void clean()
    {
        mInquiryStatus = 0;
        mDeviceClass = 0;
        mErrVal = 0;
        delete mDeviceAddr;
        mDeviceAddr = NULL;
        delete mDeviceName;
        mDeviceName = NULL;
    }

    int mInquiryStatus; //set to 0 usually. In case of inquiryCompleted callback
    //will be set to either Inquiry_Completed or Inquiry error or Inquiry terminated
    int mErrVal; // In case of any err, this contains the error value.
    int mDeviceClass;
    std::wstring* mDeviceAddr;
    std::wstring* mDeviceName;
};

} //end namespace bluetooth
} //end namespace java

#endif // BLUETOOTHSTRUCTS_H
