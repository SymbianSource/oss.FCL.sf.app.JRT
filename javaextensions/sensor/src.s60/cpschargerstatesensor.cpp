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
* Description:  Charger state sensor implementation
*
*/

#include <hwrmpowerstatesdkpskeys.h>
#include "cpschargerstatesensor.h"
#include "csensorproperties.h"
#include "logger.h"

_LIT(KChargerStateSensorDescriptionPart1,
     "charger_state#"
     L"device#"
     L"device#"
     L"Nokia#"
     L"description=");

_LIT(KChargerStateSensorDescriptionPart2,
     "#model=Nokia#"
     L"connectiontype=1#"
     L"1#" // isAvailable
     L"1#" // channel count
     L"charger_state#" // channel name. for single channel sensors should be same as model
     L"0#" // accuracy, exact
     L"2#" // data type. here TYPE_INT
     L"0#" // scale. here zero (means that scaling is not used)
     L"boolean#" // unit. Charger is attached or not attached
     L"1#" // measurement range count
     L"0#" // smallest measurable value
     L"1#" // largest measurable value
     L"1#" // measurement resolution
     L"@ch_end#");  // channel terminator constant

const TInt KPollInterval = 100000; // 0.1 sec

CPSChargerStateSensor* CPSChargerStateSensor::NewL()
{
    JELOG2(ESensor);
    CPSChargerStateSensor* self = new(ELeave)CPSChargerStateSensor();
    CleanupStack::PushL(self);
    self->ConstructL(KPSUidHWRMPowerState, KHWRMChargingStatus);
    CleanupStack::Pop(); // self
    return self;
}

CPSChargerStateSensor::CPSChargerStateSensor()
        : CPSSensorBase(KPollInterval)
{
    JELOG2(ESensor);
}

CPSChargerStateSensor::~CPSChargerStateSensor()
{
    JELOG2(ESensor);
}

HBufC* CPSChargerStateSensor::CreateDescriptionLC()
{
    JELOG2(ESensor);
    HBufC* desc = HBufC::NewLC(KMaxSensorDescriptionLength);
    TPtr desPtr = desc->Des();
    desPtr.Append(KChargerStateSensorDescriptionPart1);
    desPtr.Append(SensorProperties::GetPropertyString(KSensorDescription,
                  EChargerStateSensor));
    desPtr.Append(KChargerStateSensorDescriptionPart2);
    return desc;
}

CSensorBase* CPSChargerStateSensor::DuplicateL()
{
    JELOG2(ESensor);
    return NewL();
}

TReal CPSChargerStateSensor::InterpretValue(TReal aValue)
{
    JELOG2(ESensor);
    // Return 1 if aValue is bigger than 0. Return 0 if 0.
    // Negative aValue is an error code and is returned as is.
    return aValue > 0 ? 1 : aValue;
}

