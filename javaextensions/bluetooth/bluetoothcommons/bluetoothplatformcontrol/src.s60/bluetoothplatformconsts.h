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



#ifndef BLUETOOTHPLATFORMCONSTS_H
#define BLUETOOTHPLATFORMCONSTS_H

#include <e32base.h>

namespace java
{
namespace bluetooth
{

/* ********************** Power state ********************** */

/**
 * Response indicating user wants Bluetooth to be turned on.
 */
_LIT(KAskPowerYes, "yes");

/**
 * Response indicating user does not want Bluetooth to be turned on.
 */
_LIT(KAskPowerNo, "no");

/**
 * Represents whether Bluetooth is turned on.
 * Values: ETrue or EFalse.
 */
_LIT(KPropertyPowerOn,  "Bluetooth.PowerOn");

/* ********************************************************* */


/* ******************** Discoverability ******************** */

/**
 * Response indicating user wants the device to be made discoverable.
 */
_LIT(KAskDiscoverabilityYes, "yes");

/**
 * Response indicating user does not want the device to be made discoverable.
 */
_LIT(KAskDiscoverabilityNo, "no");

/**
 * Represents whether the device is discoverable.
 * Values: one of
 *   KNotDiscoverable
 *   KGIAC (discoverable)
 *   KLIAC (limited discoverable)
 * or in the range
 *   >= DiscoveryAgent.IAC_RANGE_START && <= DiscoveryAgent.IAC_RANGE_END
 * (these values are checked at compile time in CJavaBTDiscoveryAgent.cpp
 * to be equal to the corresponding discoverability constants in
 * javax.bluetooth.DiscoveryAgent)
 */
_LIT(KPropertyDiscoverability, "Bluetooth.Discoverability");

const TInt KNotDiscoverable = 0x0;



}  // end of namespace Bluetooth
}  // end of namespace java

#endif // BLUETOOTHPLATFORMCONSTS_H
