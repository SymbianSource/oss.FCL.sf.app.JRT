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


#include "bluetoothnamelookup.h"
#include "fs_methodcall.h"
#include "javajniutils.h"
#include "logger.h"

namespace java
{
namespace bluetooth
{

_LIT(KBTLinkManagerTxt, "BTLinkManager");

OS_EXPORT BluetoothNameLookup* BluetoothNameLookup::NewL()
{
    JELOG2(EJavaBluetooth);
    BluetoothNameLookup* btNameLookup = new BluetoothNameLookup();
    CleanupStack::PushL(btNameLookup);
    btNameLookup->constructL();
    CleanupStack::Pop(btNameLookup);
    return btNameLookup;
}

void BluetoothNameLookup::constructL()
{
    JELOG2(EJavaBluetooth);
    User::LeaveIfError(mSocketServ.Connect());
}

OS_EXPORT BluetoothNameLookup::~BluetoothNameLookup()
{
    JELOG2(EJavaBluetooth);
    mHostResolver.Close();
    mSocketServ.Close();
    delete mLookupMonitor;
}

OS_EXPORT std::wstring * BluetoothNameLookup::doDeviceNameLookupL(
    TInt64 aDevAddr)
{
    JELOG2(EJavaBluetooth);
    mLookupName = NULL;

    if (0 == mLookupMonitor)
    {
        mLookupMonitor = new CActiveSchedulerWait();
    }
    LOG1(EJavaBluetooth, EInfo,
         "+ BluetoothNameLookup::doDeviceNameLookupL DeviceAddress:%llx",
         aDevAddr);
    TProtocolDesc pdesc;
    User::LeaveIfError(mSocketServ.FindProtocol(KBTLinkManagerTxt(), pdesc));

    // Initialize host resolver
    mHostResolver.Close();
    User::LeaveIfError(mHostResolver.Open(mSocketServ, pdesc.iAddrFamily,
                                          pdesc.iProtocol));

    TBTDevAddr deviceAddr(aDevAddr);

    mSockAddr.SetBTAddr(deviceAddr);
    mSockAddr.SetAction(KHostResName | KHostResIgnoreCache);
    mHostResolver.GetByAddress(mSockAddr, mNameEntry, iStatus);

    SetActive();
    mLookupMonitor->Start();
    if (KErrNone != mLookupStatus && KErrCancel != mLookupStatus
            && KErrCompletion != mLookupStatus)
    {
        //if there is any error, then we send null. need to throw IOException
        //at the java side if null is returned
        ELOG1(EJavaBluetooth,
              "- BluetoothNameLookup::doDeviceNameLookupL Error %d",
              mLookupStatus);
        User::Leave(mLookupStatus);
    }

    return mLookupName;
}

void BluetoothNameLookup::RunL()
{
    JELOG2(EJavaBluetooth);
    std::wstring *deviceName = NULL;
    if (KErrNone == iStatus.Int() && NULL != mNameEntry().iName.Ptr())
    {
        deviceName = new std::wstring((wchar_t*) mNameEntry().iName.Ptr());
        deviceName->resize(mNameEntry().iName.Length());
    }

    nameLookupCompleted(iStatus.Int(), deviceName);
    //We do not set this to active again as we just need to lookup once
}

/**
 * Notify the monitor, which waits for the name lookup to complete
 */
void BluetoothNameLookup::nameLookupCompleted(int aStatus,
        std::wstring* deviceName)
{
    JELOG2(EJavaBluetooth);
    mLookupStatus = aStatus;
    mLookupName = deviceName;
    mLookupMonitor->AsyncStop();
}

void BluetoothNameLookup::DoCancel()
{
}

} //end namespace java
} //end namespace bluetooth
