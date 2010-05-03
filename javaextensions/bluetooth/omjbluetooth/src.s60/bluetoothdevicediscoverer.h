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


#ifndef BLUETOOTHDEVICEDISCOVERER_H
#define BLUETOOTHDEVICEDISCOVERER_H

#include <bt_sock.h>
#include <e32base.h>
#include <queue>

#include "logger.h"
#include "monitor.h"
#include "bluetoothstructs.h"
#include "bluetoothfunctionserver.h"
#include "scopedlocks.h"

namespace java
{
namespace bluetooth
{

class BluetoothDeviceDiscoverer: public CActive
{
public:

    /**
     * @param aFunctionServer Pointer to BluetoothFunctionServer
     */
    static BluetoothDeviceDiscoverer* NewL(
        BluetoothFunctionServer *aFunctionServer);

    ~BluetoothDeviceDiscoverer();

    /**
     * Starts device inquiry
     */
    void DiscoverDevicesL(int aAccessCode);

    /**
     * Searches for the next device
     */
    int getNextDevice(DiscoveredDevice &aDiscoveredDevice);

    /**
     *
     */
    void RunL();

    /**
     * Cancels device inquiry
     */
    void DoCancel();

    inline void cancel()
    {
        Cancel();
    }

private:

    BluetoothDeviceDiscoverer(BluetoothFunctionServer *aFunctionServer);

    BluetoothDeviceDiscoverer() : CActive(EPriorityStandard)
    {
    };

    void ConstructL();

    void setNext();

    void fillDeviceInfo(DiscoveredDevice &aDiscoveredDevice);

    void enqueue(DiscoveredDevice *aDiscoveredDevice);

    void dequeue(DiscoveredDevice& aDiscoveredDevice);

    void clearQueue();

private:

    bool mWaiting;
    int mStatus;
    int mSocketServerStatus;

    pthread_mutex_t  mMutex;
    pthread_cond_t   mCondVar;
    TNameEntry mNameEntry;
    RSocketServ mSocketServer;
    RHostResolver mHostResolver;
    TFullName mThreadName;

    std::queue<DiscoveredDevice*> mDiscoveredDevicesQueue;

    java::util::Monitor* mDeviceDiscoveryMonitor;
    BluetoothFunctionServer* mFunctionServer;
};

} //end namespace bluetooth
} //end namespace java

#endif // BLUETOOTHDEVICEDISCOVERER_H
