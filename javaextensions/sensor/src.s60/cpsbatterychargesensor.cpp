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
* Description:  Battery Level sensor implementation
*
*/

#include <hwrmpowerstatesdkpskeys.h>
#include <e32math.h>
#include "cpsbatterychargesensor.h"
#include "csensorproperties.h"
#include "logger.h"

_LIT(KBatteryLevelSensorDescriptionPart1,
     "battery_charge#"
     L"device#"
     L"device#"
     L"Nokia#"
     L"description=");
_LIT(KBatteryLevelSensorDescriptionPart2,
     "#model=Nokia#"
     L"connectiontype=1#"
     L"1#" // isAvailable
     L"1#" // channel count
     L"battery_charge#" // channel name. for single channel sensors should be same as model
     L"0.1429#" // accuracy, 7 steps (0-7)
     L"2#" // data type. here TYPE_INT
     L"0#" // scale. here zero (means that scaling is not used)
     L"%#" // unit. percent
     L"1#" // measurement range count
     L"0#" // smallest measurable value
     L"100#" // largest measurable value
     L"14.29#" // measurement resolution
     L"@ch_end#");  // channel terminator constant

const TInt KPollInterval = 100000; // 0.1 sec
const TReal KInterpretMultiplier = 14.29f;

CPSBatteryChargeSensor* CPSBatteryChargeSensor::NewL()
{
    JELOG2(ESensor);
    CPSBatteryChargeSensor* self = new(ELeave)CPSBatteryChargeSensor();
    CleanupStack::PushL(self);
    self->ConstructL(KPSUidHWRMPowerState, KHWRMBatteryLevel);
    CleanupStack::Pop(); // self
    return self;
}

CPSBatteryChargeSensor::CPSBatteryChargeSensor()
        : CPSSensorBase(KPollInterval)
{
    JELOG2(ESensor);
}

CPSBatteryChargeSensor::~CPSBatteryChargeSensor()
{
}

HBufC* CPSBatteryChargeSensor::CreateDescriptionLC()
{
    JELOG2(ESensor);
    HBufC* desc = HBufC::NewLC(KMaxSensorDescriptionLength);
    TPtr desPtr = desc->Des();
    desPtr.Append(KBatteryLevelSensorDescriptionPart1);
    desPtr.Append(SensorProperties::GetPropertyString(KSensorDescription,
                  EBatteryChargeSensor));
    desPtr.Append(KBatteryLevelSensorDescriptionPart2);
    return desc;
}

CSensorBase* CPSBatteryChargeSensor::DuplicateL()
{
    JELOG2(ESensor);
    return NewL();
}

TReal CPSBatteryChargeSensor::InterpretValue(TReal aValue)
{
    JELOG2(ESensor);
    if (aValue > 0)
    {
        TReal value = KInterpretMultiplier * aValue;
        TInt32 retVal;
        Math::Round(value, value, 0);
        Math::Int(retVal, value);
        return retVal;
    }
    else
    {
        return aValue;
    }
}

