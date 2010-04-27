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


#include "bluetoothremotedevice.h"
#include "fs_methodcall.h"
#include "javajniutils.h"

namespace java
{
namespace bluetooth
{

/**
 * Returns ETrue if SecurityId matches. Otherwise returns EFalse
 */
OS_EXPORT TBool BluetoothRemoteDevice::getSecurityProperty(TInt8 aSecurityId,
        TInt64 &aRemoteDevAddress)
{
    JELOG2(EJavaBluetooth);
    RBTRegServ bTRegistryServ;
    RBTRegistry btRegistry;
    TBool retVal = EFalse;

    TBTNamelessDevice device;
    TInt err = KErrNone;

    err = bTRegistryServ.Connect();
    if (KErrNone != err)
        return EFalse;

    err = btRegistry.Open(bTRegistryServ);
    if (KErrNone != err)
        return EFalse;

    err = getRemoteDevice(btRegistry, device, aRemoteDevAddress);

    if (KErrNone == err)
    {
        if (REMOTE_AUTHENTICATED == aSecurityId)
        {
            retVal = device.IsPaired();
        }
        else if (device.IsValidGlobalSecurity())
        {
            TBTDeviceSecurity globalSecurity(device.GlobalSecurity());

            switch (aSecurityId)
            {
            case REMOTE_AUTHORIZED:
                retVal = !globalSecurity.NoAuthorise()
                         && !globalSecurity.Banned();
                break;
            case REMOTE_ENCRYPT:
                retVal = globalSecurity.Encrypt();
                break;
            case REMOTE_BANNED:
                retVal = globalSecurity.Banned();
                break;
            case REMOTE_TRUSTED:
                retVal = !globalSecurity.Banned()
                         && !globalSecurity.NoAuthenticate()
                         && globalSecurity.NoAuthorise();
                break;
            }
        }
    }

    btRegistry.Close();
    bTRegistryServ.Close();

    LOG2(EJavaBluetooth, EInfo,
         "- BluetoothRemoteDevice::getSecurityProperty %d:%d", aSecurityId,
         retVal);

    return retVal;
}

TInt BluetoothRemoteDevice::getRemoteDevice(RBTRegistry& aBtRegistry,
        TBTNamelessDevice& aDevice, TInt64 &aRemoteDevAddress)
{
    JELOG2(EJavaBluetooth);
    TRequestStatus status;
    const TBTDevAddr deviceAddress(aRemoteDevAddress);

    aDevice.SetAddress(deviceAddress);

    aBtRegistry.GetDevice(aDevice, status);
    User::WaitForRequest(status);

    TInt result = status.Int();

    LOG1(EJavaBluetooth, EInfo, "- BluetoothRemoteDevice::getRemoteDevice %d",
         result);
    return result;
}

} //end namespace bluetooth
} //end namespace java
