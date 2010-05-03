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


#ifndef DISCOVERYAGENT_H
#define DISCOVERYAGENT_H

#include "bluetoothfunctionserver.h"
#include "bluetoothdevicediscoverer.h"
#include "bluetoothplatformcontrol.h"
#include "bluetoothnamelookup.h"
#include "javajniutils.h"
#include "monitor.h"
#include "bluetoothstructs.h"

#include "bluetoothservicesearcher.h"

namespace java
{
namespace bluetooth
{

class BluetoothDeviceDiscoverer;
class BluetoothNameLookup;
class BluetoothServiceSearcher;

#define SOCKETSERVER_CONNECTED      0
#define SOCKETSERVER_DISCONNECTED  -1

class DiscoveryAgent
{
public:
    //
    // Methods related to Discovery Agent
    //
    DiscoveryAgent(BluetoothFunctionServer* aServer);

    ~DiscoveryAgent();

    //
    // Methods related to Device Inquiry
    //
    int startDeviceDiscovery(JNIEnv* aJni, jobject& aPeer, int aAccessCode);

    int getNextDevice(DiscoveredDevice &aDiscoveredDevice);

    int doDeviceDiscoveryCallback(JNIEnv* aJni, jobject& aPeer,
                                  DiscoveredDevice &aDiscoveredDevice);

    void deviceDiscoveryCompleted(int aStatus);

    bool cancelDeviceDiscovery();

    void deviceDiscovered(std::wstring* deviceAddr, std::wstring* deviceName,
                          int deviceClass);

    //
    // Methods related to Name Lookup
    //
    std::wstring* lookupFriendlyNameL(long long devAddr);

    // Service Search Methods
    int StartSearchServices(JNIEnv* aJni, jobject& aPeer,
                            TInt64 aRemoteAddress, TPtrC8 aUuidsDes, TPtrC16 aAttrIdsDes,
                            java::util::Monitor* aMonitor);

    void ServiceSearchCompleted(int aStatus);

    bool CancelServiceSearch();

    int getStatusOfCompletion();

    int PopulateServiceRecordAttrValue(JNIEnv* aJni, jobject& aPeer,
                                       TInt64 aRemoteAddress, long aHandle, TPtrC16 aAttrIdsDes,
                                       jobject aServiceRecordImpl, java::util::Monitor* aMonitor);

    void PopulateRecordCompleted(int aStatus);

    int GetPopulateServiceRecordStatus();

    bool CancelPopulateServiceRecordAttrValue();

private:
    // Discovery Agent Methods
    void cleanup();

    // Device discovery Methods
    void discoverDevicesFsL(int aAccessCode);

    void cleanupDeviceDiscoverer();

    void startedDeviceDiscovery(JNIEnv* aJni, jobject& aPeer);

    // Name lookup Methods
    std::wstring* nameLookupL(TInt64 aDevAddr);

    // Service Search Methods
    void searchServiceFsL(TInt64 aRemoteAddress, TPtrC8 aUuidsDes,
                          TPtrC16 aAttrIdsDes);

    void cancelAndCleanupServiceSearchFs();

    void CleanupServiceSearcher();

    void CleanupAllServiceSearcherInfo();

    void StartedServiceSearch(JNIEnv* aJni, jobject& aPeer);

    void populateServiceRecordAttrValueFs(TInt64 aRemoteAddress, long aHandle,
                                          TPtrC16 aAttrIdsDes, jobject aServiceRecordImpl);

private:
    // Service Search Data
    bool mIsServiceSearchOn;

    int mServiceSearchStatus;
    // Discovery Agent Data
    int mSocketServerStatus;
    int mPopulateRecordStatus;

    // Device Inquiry Callback JNI Method IDs
    jmethodID mDeviceDiscoveredMethod;
    jmethodID mDeviceDiscoveryStartedMethod;
    // Service Search Callback JNI Method IDs
    jmethodID mServiceSearchStartedMethod;

    BluetoothFunctionServer* mFunctionServer;
    BluetoothServiceSearcher* mServiceSearcher;
    // Device Inquiry Data
    BluetoothDeviceDiscoverer* mDeviceDiscoverer;

    java::util::Monitor* mServiceSearchMonitor;

};

} //end namespace bluetooth
} //end namespace java


#endif // DISCOVERYAGENT_H
