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


#ifndef BLUETOOTHNAMELOOKUP_H
#define BLUETOOTHNAMELOOKUP_H

#include <bt_sock.h>
#include "logger.h"
#include "bluetoothfunctionserver.h"
#include "monitor.h"

namespace java
{
namespace bluetooth
{

class BluetoothNameLookup: public CActive
{
public:

    OS_IMPORT static BluetoothNameLookup* NewL();

    OS_IMPORT ~BluetoothNameLookup();

    void RunL();

    void DoCancel();

    OS_IMPORT std::wstring * doDeviceNameLookupL(TInt64 devAddr);

private:
    //Create Lookup object and add to active scheduler
    BluetoothNameLookup():CActive(EPriorityStandard)
    {
        CActiveScheduler::Add(this);
    }


    void constructL();

    void nameLookupCompleted(int aStatus, std::wstring* deviceName);
private:

    int mLookupStatus;
    std::wstring* mLookupName;

    RSocketServ mSocketServ;
    RHostResolver mHostResolver;
    TInquirySockAddr mSockAddr;
    TNameEntry mNameEntry;
    // Name Lookup Data
    CActiveSchedulerWait* mLookupMonitor;
};

} //end namespace bluetooth
} //end namespace java

#endif // BLUETOOTHNAMELOOKUP_H
