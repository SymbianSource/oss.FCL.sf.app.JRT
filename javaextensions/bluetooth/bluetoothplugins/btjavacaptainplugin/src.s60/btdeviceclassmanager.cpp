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


#include <bt_subscribe.h>
#include <btmanclient.h>
#include <e32property.h>
#include "comms.h"
#include "commsendpoint.h"
#include "commsmessage.h"
#include "logger.h"

#include "coreinterface.h"
#include "rtcinterface.h"

#include "btdeviceclassmanager.h"

#ifdef __SYMBIAN32__
java::captain::ExtensionPluginInterface* getExtensionPlugin()
{
#else
extern "C" java::captain::ExtensionPluginInterface* getExtensionPlugin()
{
#endif
    return new java::bluetooth::BtDeviceClassManager();
}

using namespace java::comms;
using namespace java::captain;

namespace java
{
namespace bluetooth
{

BtDeviceClassManager:: BtDeviceClassManager() : mCore(0)
{
    JELOG2(EJavaBluetooth);
    mInitialServiceClassBits = 0;
    LOG(EJavaBluetooth, EInfo, "Loaded BT service plugin");
}

BtDeviceClassManager::~BtDeviceClassManager()
{
    JELOG2(EJavaBluetooth);
}

void BtDeviceClassManager::startPlugin(CoreInterface* aCore)
{
    JELOG2(EJavaBluetooth);
    mCore = aCore;
    mCore->getComms()->registerListener(PLUGIN_ID_BT_DEVICE_CLASS_MANAGER_C,
                                        this);
}

void BtDeviceClassManager::stopPlugin()
{
    JELOG2(EJavaBluetooth);
    mCore->getComms()->unregisterListener(PLUGIN_ID_BT_DEVICE_CLASS_MANAGER_C,
                                          this);
    mCore = 0;
}

java::comms::CommsListener* BtDeviceClassManager::getCommsListener()
{
    JELOG2(EJavaBluetooth);
    return this;
}

// CommsListener methods
void BtDeviceClassManager::processMessage(CommsMessage& aMessage)
{
    JELOG2(EJavaBluetooth);
    switch (aMessage.getMessageId())
    {
    case JSR_82_MESSAGE_ID_RANGE_START_C:
        int isServiceBitsPersistent;
        std::wstring midletId;
        int srvClassVal;

        aMessage >> isServiceBitsPersistent;
        aMessage >> midletId;
        aMessage >> srvClassVal;

        LOG1(EJavaBluetooth, EInfo,
             "  BtDeviceClassManager::processMessage Persistent:%d",
             isServiceBitsPersistent);
        LOG1(EJavaBluetooth, EInfo,
             "  BtDeviceClassManager::processMessage midletId:%S",
             midletId.c_str());
        LOG1(EJavaBluetooth, EInfo,
             "  BtDeviceClassManager::processMessage srvClassVal:0x%X",
             srvClassVal);

        if (!srvClassVal)
        {
            LOG(EJavaBluetooth, EInfo,
                "   BtDeviceClassManager::processMessage Removing");
            mServiceListHandler.remove(midletId);
        }
        else
        {
            if (true == isServiceBitsPersistent)
            {
                srvClassVal |= mServiceListHandler.get(midletId);
            }

            LOG(EJavaBluetooth, EInfo,
                "   BtDeviceClassManager::processMessage Adding");
            mServiceListHandler.add(midletId, srvClassVal);
        }

        int retVal = setServiceClassBits(mServiceListHandler.getCollectiveData());

        CommsMessage replyMsg;
        replyMsg.replyTo(aMessage);
        replyMsg << retVal;
        mCore->getComms()->send(replyMsg);
        break;
    }
}

unsigned int BtDeviceClassManager::getServiceClassBits()
{
    JELOG2(EJavaBluetooth);
    TInt currentCODVal = 0;

    TInt error = RProperty::Get(KPropertyUidBluetoothCategory,
                                KPropertyKeyBluetoothGetDeviceClass, currentCODVal);

    if (KErrNone != error)
    {
        ELOG1(EJavaBluetooth, "- BtDeviceClassManager::getServiceClass Err:%d",
              error);
        return 0;
    }

    LOG1(EJavaBluetooth, EInfo,
         "  BtDeviceClassManager::getServiceClass COD:0x%x", currentCODVal);

    TBTDeviceClass currentCOD(currentCODVal);
    return currentCOD.MajorServiceClass();
}

int BtDeviceClassManager::setServiceClassBits(unsigned int aDeviceServiceClass)
{
    JELOG2(EJavaBluetooth);

    if (0 == mInitialServiceClassBits)
    {
        mInitialServiceClassBits = getServiceClassBits();
        LOG1(EJavaBluetooth, EInfo, "mInitialServiceClassBits : %d",
             mInitialServiceClassBits);
    }

    TInt currentCODVal = 0;

    TInt error = RProperty::Get(KPropertyUidBluetoothCategory,
                                KPropertyKeyBluetoothGetDeviceClass, currentCODVal);

    if (KErrNone != error)
    {
        ELOG1(EJavaBluetooth,
              "   BtDeviceClassManager::setServiceClass Err:%d", error);
        return error;
    }

    TBTDeviceClass currentCOD(currentCODVal);

    // Extracting the service class bits only.
    TBTDeviceClass requestedCOD(aDeviceServiceClass);

    TBTDeviceClass newCOD(mInitialServiceClassBits
                          | requestedCOD.MajorServiceClass(), currentCOD.MajorDeviceClass(),
                          currentCOD.MinorDeviceClass());

    LOG1(EJavaBluetooth, EInfo,
         "   BtDeviceClassManager::setServiceClass new service bits:0x%x",
         ((mInitialServiceClassBits | aDeviceServiceClass) << 13));

    error = RProperty::Set(KPropertyUidBluetoothCategory,
                           KPropertyKeyBluetoothSetDeviceClass, newCOD.DeviceClass());

    LOG3(
        EJavaBluetooth,
        EInfo,
        "   BtDeviceClassManager::setServiceClass 0x%X  Err:%d NewServCOD:0x%x",
        aDeviceServiceClass, error, getServiceClassBits());

    return error;
}

} // namespace bluetooth
} // namespace java

