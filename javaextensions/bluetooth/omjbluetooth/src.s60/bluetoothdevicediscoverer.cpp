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


#include <string>
#include <e32base.h>

#include "logger.h"
#include "fs_methodcall.h"
#include "bluetoothconsts.h"
#include "bluetoothdevicediscoverer.h"

// Constants used to create device class
#define SERVICE_CLASS_POS    13
#define MAJOR_CLASS_POS       8
#define MINOR_CLASS_POS       2

#define SOCKETSERVER_CONNECTED 1

namespace java
{
namespace bluetooth
{

_LIT(KBTLinkManagerTxt,"BTLinkManager");

BluetoothDeviceDiscoverer* BluetoothDeviceDiscoverer::NewL(
    BluetoothFunctionServer *aFunctionServer)
{
    JELOG2(EJavaBluetooth);
    BluetoothDeviceDiscoverer *self = new BluetoothDeviceDiscoverer(
        aFunctionServer);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

void BluetoothDeviceDiscoverer::ConstructL()
{
    int result = -1;
    User::LeaveIfError(mSocketServerStatus = mSocketServer.Connect());
    mSocketServer.ShareAuto();
    mSocketServerStatus = SOCKETSERVER_CONNECTED;

    result = pthread_mutex_init(&mMutex, 0);
    if (result == 0)
    {
        result = pthread_cond_init(&mCondVar, 0);
    }
    if (0 != result)
    {
        User::Leave(result);
    }
}

BluetoothDeviceDiscoverer::BluetoothDeviceDiscoverer(
    BluetoothFunctionServer *aFunctionServer) :
        CActive(EPriorityStandard)
{
    CActiveScheduler::Add(this);
    mFunctionServer = aFunctionServer;
}

BluetoothDeviceDiscoverer::~BluetoothDeviceDiscoverer()
{
    JELOG2(EJavaBluetooth);

    mHostResolver.Close();

    if (SOCKETSERVER_CONNECTED == mSocketServerStatus)
        mSocketServer.Close();

    clearQueue();
    pthread_mutex_destroy(&mMutex);
    pthread_cond_destroy(&mCondVar);


}

/**
 * Searches for the next device.
 * If found, then returns the discovered device information
 * Otherwise, in case of error or completion, returns INQUIRY_COMAPLETED
 * and in case of cancele, returnsn INQUIRY_TERMINATED
 */
int BluetoothDeviceDiscoverer::getNextDevice(
    DiscoveredDevice &aDiscoveredDevice)
{
    JELOG2(EJavaBluetooth);

    dequeue(aDiscoveredDevice);
    return aDiscoveredDevice.mInquiryStatus;
}

/**
 * Starts the searching for the next device
 */
void BluetoothDeviceDiscoverer::setNext()
{
    JELOG2(EJavaBluetooth);

    if (!IsActive())
    {
        // Get next discovered device
        mHostResolver.Next(mNameEntry, iStatus);
        SetActive();
    }
}

/**
 * Initializes the device inquiry
 */
void BluetoothDeviceDiscoverer::DiscoverDevicesL(int aAccessCode)
{
    JELOG2(EJavaBluetooth);

    TInquirySockAddr inquirySockAddr;

    if (!IsActive())
    {
        LOG(EJavaBluetooth, EInfo,
            "+ BluetoothDeviceDiscoverer::DiscoverDevicesL Finding protocol");
        // Load protocol for discovery
        TProtocolDesc pdesc;
        User::LeaveIfError(mSocketServer.FindProtocol(KBTLinkManagerTxt(),
                           pdesc));

        // Initialize host resolver
        mHostResolver.Close();

        User::LeaveIfError(mHostResolver.Open(mSocketServer, pdesc.iAddrFamily,
                                              pdesc.iProtocol));

        // Start device discovery by invoking remote address lookup
        if (KGIAC == aAccessCode)
            LOG(EJavaBluetooth, EInfo,
                "  BluetoothDeviceDiscoverer::DiscoverDevicesL GIAC");
        else if (KLIAC == aAccessCode)
            LOG(EJavaBluetooth, EInfo,
                "  BluetoothDeviceDiscoverer::DiscoverDevicesL LIAC");
        else
            LOG1(EJavaBluetooth, EInfo,
                 "  BluetoothDeviceDiscoverer::DiscoverDevicesL Val: %X",
                 aAccessCode);

        inquirySockAddr.SetIAC(aAccessCode);
        inquirySockAddr.SetAction(KHostResInquiry | KHostResName
                                  | KHostResIgnoreCache);

        mHostResolver.GetByAddress(inquirySockAddr, mNameEntry, iStatus);

        SetActive();
    }
    else
    {
        ELOG(EJavaBluetooth,
             "- BluetoothDeviceDiscoverer::DiscoverDevicesL Leaving. Not ready!");
        User::Leave(KErrNotReady);
    }
}

void BluetoothDeviceDiscoverer::RunL()
{
    JELOG2(EJavaBluetooth);
    bool continueFlag = false;

    DiscoveredDevice *discoveredDevice = new DiscoveredDevice();

    if (iStatus.Int() == KErrCancel)
    {
        RThread thread;
        int error = thread.Open(mThreadName, EOwnerProcess);
        if (error == KErrNone)
        {
            thread.RequestSignal();
        }

        clearQueue();
        discoveredDevice->mInquiryStatus = INQUIRY_TERMINATED;
    }
    else if (KErrNone == iStatus.Int())
    {
        fillDeviceInfo(*discoveredDevice);
        continueFlag = true;
    }
    else
    {
        discoveredDevice->mInquiryStatus = INQUIRY_COMPLETED;
    }

    discoveredDevice->mErrVal = iStatus.Int();

    enqueue(discoveredDevice);

    if (continueFlag)
    {
        setNext();
    }

}

void BluetoothDeviceDiscoverer::DoCancel()
{
    JELOG2(EJavaBluetooth);

    RThread thread;
    thread.FullName(mThreadName);
    mHostResolver.Cancel();
}

/**
 * Fills the discovered device information into DiscoveredDevice object
 */
void BluetoothDeviceDiscoverer::fillDeviceInfo(
    DiscoveredDevice &aDiscoveredDevice)
{
    JELOG2(EJavaBluetooth);

    // Filling inquiry status
    aDiscoveredDevice.mInquiryStatus = STATUS_DEVICE_FOUND;

    // Filling device name
    std::wstring *deviceName = new std::wstring(
        (wchar_t*) mNameEntry().iName.Ptr());
    deviceName->resize(mNameEntry().iName.Length());

    aDiscoveredDevice.mDeviceName = deviceName;

    LOG1(EJavaBluetooth, EInfo,
         "  BluetoothDeviceDiscoverer::fillDeviceInfo Device Name %S",
         deviceName->c_str());

    // Filling device address
    TBTDevAddr add = static_cast<TBTSockAddr>(mNameEntry().iAddr).BTAddr();
    TBuf<20> buff;

    add.GetReadable(buff);
    std::wstring *deviceAddr = new std::wstring((wchar_t*) buff.Ptr());
    deviceAddr->resize(buff.Length());

    aDiscoveredDevice.mDeviceAddr = deviceAddr;

    LOG1(EJavaBluetooth, EInfo,
         "  BluetoothDeviceDiscoverer::fillDeviceInfo Device Address %S",
         deviceAddr->c_str());

    // Filling device class
    TInquirySockAddr& dev = TInquirySockAddr::Cast(mNameEntry().iAddr);

    int deviceClass = (dev.MajorServiceClass() << SERVICE_CLASS_POS)
                      | (dev.MajorClassOfDevice() << MAJOR_CLASS_POS)
                      | (dev.MajorClassOfDevice() << MINOR_CLASS_POS);

    aDiscoveredDevice.mDeviceClass = deviceClass;

    LOG1(EJavaBluetooth, EInfo,
         "  BluetoothDeviceDiscoverer::fillDeviceInfo DeviceClass %d",
         deviceClass);
}

void BluetoothDeviceDiscoverer::enqueue(DiscoveredDevice *aDiscoveredDevice)
{
    JELOG2(EJavaBluetooth);

    pthread_mutex_lock(&mMutex);

    mDiscoveredDevicesQueue.push(aDiscoveredDevice);
    if (mWaiting)
    {
        LOG(EJavaBluetooth, EInfo,
            "- BluetoothDeviceDiscoverer::enqueue NOTIFYING");
        pthread_cond_signal(&mCondVar);
    }
    mWaiting = false;
    pthread_mutex_unlock(&mMutex);
}

void BluetoothDeviceDiscoverer::dequeue(DiscoveredDevice& aDiscoveredDevice)
{
    JELOG2(EJavaBluetooth);
    LOG1(EJavaBluetooth, EInfo, "+ BluetoothDeviceDiscoverer::dequeue :%d",
         mDiscoveredDevicesQueue.empty());
    DiscoveredDevice ret;

    pthread_mutex_lock(&mMutex);
    if (mDiscoveredDevicesQueue.empty())
    {
        LOG(EJavaBluetooth, EInfo,
            "- BluetoothDeviceDiscoverer::dequeue WAITING");
        mWaiting = true;

        while (mWaiting)
        {
            pthread_cond_wait(&mCondVar, &mMutex);
        }
    }

    DiscoveredDevice *ptr = mDiscoveredDevicesQueue.front();
    aDiscoveredDevice = *ptr;
    delete ptr;   // pop doesn't invoke the destructor
    mDiscoveredDevicesQueue.pop();
    pthread_mutex_unlock(&mMutex);
}

void BluetoothDeviceDiscoverer::clearQueue()
{
    JELOG2(EJavaBluetooth);
    pthread_mutex_lock(&mMutex);

    while (!mDiscoveredDevicesQueue.empty())
    {
        mDiscoveredDevicesQueue.pop();
    }
    pthread_mutex_unlock(&mMutex);
}

} //end namespace java
} //end namespace bluetooth
