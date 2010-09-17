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


#include "discoveryagent.h"
#include "bluetoothdevicediscoverer.h"
#include "fs_methodcall.h"
#include "javajniutils.h"
#include "exceptionbase.h"

using namespace java::util;

namespace java
{
namespace bluetooth
{

DiscoveryAgent::DiscoveryAgent(BluetoothFunctionServer* aServer)
{
    JELOG2(EJavaBluetooth);
    // Initializing discovery agent data
    mFunctionServer = aServer;
    mSocketServerStatus = SOCKETSERVER_DISCONNECTED;

    // Initializing device discovery data
    mDeviceDiscoverer = NULL;
}

DiscoveryAgent::~DiscoveryAgent()
{
    JELOG2(EJavaBluetooth);
    cleanupDeviceDiscoverer();
}

/**
 * Starts the device inquiry. After starts the inquiry, call made to the java
 * indicates that inquiry started.
 *
 * @param aJni
 * @param aPeer
 * @param aAccessCode Inquiry code
 * @param aMonitor    Monitor which waits for this operation to complete
 */
int DiscoveryAgent::startDeviceDiscovery(JNIEnv* aJni, jobject& aPeer,
        int aAccessCode)
{
    JELOG2(EJavaBluetooth);

    jclass peerClass = (*aJni).GetObjectClass(aPeer);

    //Get Method ID of Device Discovered Callback
    mDeviceDiscoveredMethod = aJni->GetMethodID(peerClass,
                              "deviceDiscoveredCallBack",
                              "(Ljava/lang/String;Ljava/lang/String;I)V");

    //We call Device Inquiry Started Callback in BluetoothStackS60 itself
    mDeviceDiscoveryStartedMethod = aJni->GetMethodID(peerClass,
                                    "deviceInquiryStartedCallback", "()V");

    //Check if all the JNI inits have succeeded
    if (NULL == mDeviceDiscoveredMethod || NULL
            == mDeviceDiscoveryStartedMethod)
    {
        //Error in initialization
        ELOG(EJavaBluetooth,
             "- DiscoveryAgent::startDeviceDiscovery Error in JNIInit");
        return -1;
    }

    TRAPD(err, CallMethodL(this,
                           &java::bluetooth::DiscoveryAgent::discoverDevicesFsL, aAccessCode,
                           mFunctionServer));

    if (KErrNone != err)
    {
        cleanupDeviceDiscoverer();

        ELOG1(EJavaBluetooth,
              "- DiscoveryAgent::startDeviceDiscovery Error %d", err);
        return err;
    }

    LOG(EJavaBluetooth, EInfo,
        " DiscoveryAgent::startDeviceDiscovery Started Callback");

    startedDeviceDiscovery(aJni, aPeer);

    return err;
}

void DiscoveryAgent::discoverDevicesFsL(int aAccessCode)
{
    mDeviceDiscoverer = BluetoothDeviceDiscoverer::NewL(mFunctionServer);
    CleanupStack::PushL(mDeviceDiscoverer);
    mDeviceDiscoverer->DiscoverDevicesL(aAccessCode);
    CleanupStack::Pop(mDeviceDiscoverer);
}

/**
 * Cancels the device inquiry
 */
bool DiscoveryAgent::cancelDeviceDiscovery()
{
    JELOG2(EJavaBluetooth);

    if (mDeviceDiscoverer)
    {
        mDeviceDiscoverer->cancel();
    }
    return true;
}

/**
 * Makes callback to Java indicates that the inquiry started.
 */
void DiscoveryAgent::startedDeviceDiscovery(JNIEnv* aJni, jobject& aPeer)
{
    JELOG2(EJavaBluetooth);
    //Call method in Java here to notify that device discovery has been started
    //This too happens in Function Server Context.

    (*aJni).CallVoidMethod(aPeer, mDeviceDiscoveryStartedMethod);

}

/**
 * Searching for the next device.
 * Returns immediately, if already any device discovered;
 * otherwise waits for the new device.
 */
int DiscoveryAgent::getNextDevice(DiscoveredDevice &aDiscoveredDevice)
{
    JELOG2(EJavaBluetooth);
    int status = mDeviceDiscoverer->getNextDevice(aDiscoveredDevice);
    if (STATUS_DEVICE_FOUND != status)
    {
        cleanupDeviceDiscoverer();
    }
    return status;
}

/**
 * Makes callback to the Java only when new device found.
 * Returns the inquiry status.
 */
int DiscoveryAgent::doDeviceDiscoveryCallback(JNIEnv* aJni, jobject& aPeer,
        DiscoveredDevice &aDiscoveredDevice)
{
    JELOG2(EJavaBluetooth);

    jstring devAddr = NULL;
    jstring devName = NULL;
    int deviceClass = 0;

    if (STATUS_DEVICE_FOUND == aDiscoveredDevice.mInquiryStatus)
    {
        LOG(EJavaBluetooth, EInfo,
            "  DiscoveryAgent::doDeviceDiscoveryCallback: Device discovered callback");

        if (NULL != aDiscoveredDevice.mDeviceAddr)
        {
            try
            {
                devAddr = java::util::JniUtils::wstringToJstring(aJni,
                          *(aDiscoveredDevice.mDeviceAddr));
            }
            catch (ExceptionBase &ex)
            {
                // Nothing to handle
            }
        }
        else
        {
            ELOG(EJavaBluetooth,
                 "  DiscoveryAgent Really Serious Error: Address is NULL !!");
        }

        if (NULL != aDiscoveredDevice.mDeviceName)
        {
            try
            {
                devName = java::util::JniUtils::wstringToJstring(aJni,
                          *(aDiscoveredDevice.mDeviceName));
            }
            catch (ExceptionBase &ex)
            {
                // Nothing to handle
            }
        }
        else
        {
            ELOG(EJavaBluetooth,
                 "  DiscoveryAgent::doDeviceDiscoveryCallback: Device Name is NULL");
        }

        deviceClass = aDiscoveredDevice.mDeviceClass;

        (*aJni).CallVoidMethod(aPeer, mDeviceDiscoveredMethod, devAddr,
                               devName, deviceClass);
    }

    LOG1(EJavaBluetooth, EInfo,
         "- DiscoveryAgent::doDeviceDiscoveryCallback: %d",
         aDiscoveredDevice.mInquiryStatus);
    return aDiscoveredDevice.mInquiryStatus;
}

/**
 * Deletes the BluetoothDeviceDiscoverer
 */
void DiscoveryAgent::cleanupDeviceDiscoverer()
{
    JELOG2(EJavaBluetooth);
    delete mDeviceDiscoverer;
    mDeviceDiscoverer = NULL;
}

/**
 * Starts lookup for the friendly name of the device
 * On success, returns the friendly name of the device
 * Otherwise NULL
 *
 * Note: Must free resultant friendly name, whenever not required.
 */
std::wstring* DiscoveryAgent::lookupFriendlyNameL(long long aDevAddr)
{
    JELOG2(EJavaBluetooth);
    LOG1(EJavaBluetooth, EInfo,
         "+ DiscoveryAgent::lookupFriendlyName DeviceAddress:%x", aDevAddr);

    TInt64 devAddr = (TInt64) aDevAddr;
    std::wstring* result = NULL;

    CallMethodL(result, this, &java::bluetooth::DiscoveryAgent::nameLookupL,
                devAddr, mFunctionServer);

    return result;
}

/**
 * Creates the BluetoothNameLookup object
 */
std::wstring* DiscoveryAgent::nameLookupL(TInt64 aDevAddr)
{
    JELOG2(EJavaBluetooth);
    std::wstring* result = NULL;
    BluetoothNameLookup * nameLookup = BluetoothNameLookup::NewL();
    CleanupStack::PushL(nameLookup);
    result = nameLookup->doDeviceNameLookupL(aDevAddr);
    CleanupStack::Pop(nameLookup);
    delete nameLookup;
    return result;
}

/*
 *   ------- Service Search Methods --------
 */
int DiscoveryAgent::StartSearchServices(JNIEnv* aJni, jobject& aPeer,
                                        TInt64 aRemoteAddress, TPtrC8 aUuidsDes, TPtrC16 aAttrIdsDes,
                                        java::util::Monitor* aMonitor)
{
    JELOG2(EJavaBluetooth);

    mServiceSearchMonitor = aMonitor;
    mServiceSearchStatus = -1;

    jclass peerClass = (*aJni).GetObjectClass(aPeer);

    //Get Method ID of Service Search Started Callback
    mServiceSearchStartedMethod = aJni->GetMethodID(peerClass,
                                  "serviceSearchStartedCallback", "()V");

    //Check if all the JNI inits have succeeded
    if (NULL == mServiceSearchStartedMethod)
    {
        //Error in initialization
        ELOG(EJavaBluetooth,
             "- DiscoveryAgent::StartSearchServices: Error in JNIInit ");
        return -1;
    }

    LOG(EJavaBluetooth, EInfo,
        "  DiscoveryAgent::StartSearchServices: Starting service search ");

    TRAPD(err, CallMethodL(this,
                           &java::bluetooth::DiscoveryAgent::searchServiceFsL, aRemoteAddress,
                           aUuidsDes, aAttrIdsDes, mFunctionServer));

    if (KErrNone == err)
    {
        StartedServiceSearch(aJni, aPeer);
    }

    return err;
}

void DiscoveryAgent::searchServiceFsL(TInt64 aRemoteAddress, TPtrC8 aUuidsDes,
                                      TPtrC16 aAttrIdsDes)
{
    mServiceSearcher = BluetoothServiceSearcher::New(this, mFunctionServer);

    TRAPD(err, mServiceSearcher->SearchServicesL(aRemoteAddress, aUuidsDes,
            aAttrIdsDes));
    if (KErrNone != err)
    {
        ELOG(EJavaBluetooth,
             "  DiscoveryAgent::SearchServiceFsL: Error occured!! ");
        CleanupServiceSearcher();
        User::LeaveIfError(err);
    }
}

void DiscoveryAgent::StartedServiceSearch(JNIEnv* aJni, jobject& aPeer)
{
    JELOG2(EJavaBluetooth);
    mIsServiceSearchOn = true;

    (*aJni).CallVoidMethod(aPeer, mServiceSearchStartedMethod);

}

void DiscoveryAgent::ServiceSearchCompleted(int aStatus)
{
    JELOG2(EJavaBluetooth);
    mIsServiceSearchOn = false;
    mServiceSearchStatus = aStatus;
    mServiceSearchMonitor->notify();
    CleanupServiceSearcher();
}

bool DiscoveryAgent::CancelServiceSearch()
{
    JELOG2(EJavaBluetooth);
    if (NULL != mServiceSearcher)
    {
        CallMethod(
            this,
            &java::bluetooth::DiscoveryAgent::cancelAndCleanupServiceSearchFs,
            mFunctionServer);
    }
    CleanupAllServiceSearcherInfo();
    mIsServiceSearchOn = false;
    return true;
}

void DiscoveryAgent::cancelAndCleanupServiceSearchFs()
{
    mServiceSearcher->cancelServiceSearch();
    CleanupServiceSearcher();
}

int DiscoveryAgent::getStatusOfCompletion()
{
    JELOG2(EJavaBluetooth);
    LOG1(EJavaBluetooth, EInfo, "+ DiscoveryAgent::getStatusOfCompletion: %d",
         mServiceSearchStatus);
    return mServiceSearchStatus;
}

int DiscoveryAgent::PopulateServiceRecordAttrValue(JNIEnv* aJni,
        TInt64 aRemoteAddress, long aHandle,
        TPtrC16 aAttrIdsDes, jobject aServiceRecordImpl,
        java::util::Monitor* aMonitor)
{
    JELOG2(EJavaBluetooth);

    mServiceSearchMonitor = aMonitor;

    LOG(EJavaBluetooth, EInfo,
        "  DiscoveryAgent::PopulateServiceRecord: Starting populating service records ");
    TRAPD(err, CallMethodL(this,
                           &java::bluetooth::DiscoveryAgent::populateServiceRecordAttrValueFsL,
                           aRemoteAddress, aHandle, aAttrIdsDes, aServiceRecordImpl,
                           mFunctionServer));

    return err;
}

void DiscoveryAgent::populateServiceRecordAttrValueFsL(TInt64 aRemoteAddress,
        long aHandle, TPtrC16 aAttrIdsDes, jobject aServiceRecordImpl)
{
    mServiceSearcher = BluetoothServiceSearcher::New(this, mFunctionServer);
    mServiceSearcher->PopulateServiceRecordsL(aRemoteAddress, aHandle,
            aAttrIdsDes, aServiceRecordImpl);
}

void DiscoveryAgent::PopulateRecordCompleted(int aStatus)
{
    JELOG2(EJavaBluetooth);
    mPopulateRecordStatus = aStatus;
    mServiceSearchMonitor->notify();
    CleanupServiceSearcher();
}

int DiscoveryAgent::GetPopulateServiceRecordStatus()
{
    JELOG2(EJavaBluetooth);
    return mPopulateRecordStatus;
}

bool DiscoveryAgent::CancelPopulateServiceRecordAttrValue()
{
    JELOG2(EJavaBluetooth);
    mIsServiceSearchOn = false;
    TRAP_IGNORE(CallMethodL(mServiceSearcher,
                            &java::bluetooth::BluetoothServiceSearcher::cancelServiceSearch,
                            mFunctionServer));

    return true;
}

void DiscoveryAgent::CleanupServiceSearcher()
{
    JELOG2(EJavaBluetooth);
    //Delete all contained objects
    if (mServiceSearcher)
    {
        delete mServiceSearcher;
        mServiceSearcher = NULL;
    }
}

void DiscoveryAgent::CleanupAllServiceSearcherInfo()
{
    JELOG2(EJavaBluetooth);
    // Clear all realted data
    mServiceSearchStartedMethod = NULL;
    mIsServiceSearchOn = false;
    mPopulateRecordStatus = KErrNone;
    mServiceSearchMonitor = NULL;
}

} //end namespace bluetooth
} //end namespace java
