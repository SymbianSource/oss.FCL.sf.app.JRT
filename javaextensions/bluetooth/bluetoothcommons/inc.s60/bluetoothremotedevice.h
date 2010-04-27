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


#ifndef BLUETOOTHREMOTEDEVICE_H
#define BLUETOOTHREMOTEDEVICE_H

#include "bluetoothfunctionserver.h"
#include "javajniutils.h"
#include "monitor.h"

#include <btdevice.h>
#include <btmanclient.h>

namespace java
{
namespace bluetooth
{

class BluetoothDeviceDiscoverer;
class BluetoothNameLookup;
class BluetoothServiceSearcher;

#define REMOTE_AUTHENTICATED  0x01
#define REMOTE_AUTHORIZED     0x02
#define REMOTE_ENCRYPT        0x03
#define REMOTE_BANNED         0x04
#define REMOTE_TRUSTED        0x05

class BluetoothRemoteDevice
{
public:

    OS_IMPORT
    static TBool getSecurityProperty(TInt8 aSecurityId,
                                     TInt64 &aRemoteDevAddress);

private:

    static TInt getRemoteDevice(RBTRegistry& aBtRegistry,
                                TBTNamelessDevice& aDevice, TInt64 &aRemoteDevAddress);
};

} //end namespace bluetooth
} //end namespace java


#endif // BLUETOOTHREMOTEDEVICE_H
