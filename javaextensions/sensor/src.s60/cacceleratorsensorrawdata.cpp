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
* Description:  Accelerometer sensor raw data implementation
*
*/


// INTERNAL INCLUDES
#include "cacceleratorsensorrawdata.h"
#include "csensorproperties.h"
#include "logger.h"

#include <sensrvgeneralproperties.h>
#include <sensrvtypes.h>

_LIT(KSeparator, "#");
_LIT(KModel, "#model=Nokia2#");
_LIT(KDataTypeInt, "2#");
_LIT(KDataUnitRaw, "raw");
_LIT(KMeasureRangeCount, "1#");

CAcceleratorSensorRawData* CAcceleratorSensorRawData::NewLC(const TSensrvChannelInfo& aChannelInfo)
{
    JELOG2(ESensor);
    CAcceleratorSensorRawData* self = new(ELeave) CAcceleratorSensorRawData(aChannelInfo);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

CAcceleratorSensorRawData* CAcceleratorSensorRawData::NewL(const TSensrvChannelInfo& aChannelInfo)
{
    JELOG2(ESensor);
    CAcceleratorSensorRawData* self = CAcceleratorSensorRawData::NewLC(aChannelInfo);
    CleanupStack::Pop(); // self;
    return self;
}

void CAcceleratorSensorRawData::ConstructL()
{
    JELOG2(ESensor);
    CAcceleratorSensor::ConstructL();
}

CSensorBase* CAcceleratorSensorRawData::DuplicateL()
{
    JELOG2(ESensor);
    return NewL(iChannelInfo);
}

CAcceleratorSensorRawData::CAcceleratorSensorRawData(const TSensrvChannelInfo& aChannelInfo):
        CAcceleratorSensor(aChannelInfo)
{
    JELOG2(ESensor);
    // No implementation needed
}

CAcceleratorSensorRawData::~CAcceleratorSensorRawData()
{
    JELOG2(ESensor);
    // No implementation needed
}

void CAcceleratorSensorRawData::Model(TPtr* aPtr)
{
    JELOG2(ESensor);
    aPtr->Append(KModel);
}

void CAcceleratorSensorRawData::DataType(TPtr* aPtr)
{
    JELOG2(ESensor);
    aPtr->Append(KDataTypeInt);
}

void CAcceleratorSensorRawData::DataUnit(const RSensrvPropertyList /*aList*/, TPtr* aPtr)
{
    JELOG2(ESensor);
    aPtr->Append(KDataUnitRaw);
}

void CAcceleratorSensorRawData::DataRange(const RSensrvPropertyList aList,
        TPtr* aPtr)
{
    JELOG2(ESensor);
    TSensrvProperty property;
    TInt intValue(0);
    GetProperty(aList, KSensrvPropIdScaledRange,KSensrvItemIndexNone, property);

    if ((property.GetArrayIndex() == ESensrvArrayPropertyInfo)&&
            (property.PropertyType() == ESensrvIntProperty))
    {
        // Read index
        TInt rangeIndex = 0;

        // Read actual property data from array
        GetProperty(aList, KSensrvPropIdScaledRange,
                    KSensrvItemIndexNone, rangeIndex, property);
    }

    if (property.PropertyType() == ESensrvIntProperty)
    {
        aPtr->Append(KMeasureRangeCount);
        property.GetMinValue(intValue);   // min
        aPtr->AppendNum(intValue);
        aPtr->Append(KSeparator);
        property.GetMaxValue(intValue);   // max
        aPtr->AppendNum(intValue);
        aPtr->Append(KSeparator);
        aPtr->AppendNum(1);
    }
    else
    {
        aPtr->AppendNum(0);
    }
}
void CAcceleratorSensorRawData::CopyData(const TSensrvAccelerometerAxisData aData, TInt aIndex)
{
    JELOG2(ESensor);
    iChannelXData->iIntValues[ aIndex ] = aData.iAxisX;
    iChannelYData->iIntValues[ aIndex ] = aData.iAxisY;
    iChannelZData->iIntValues[ aIndex ] = aData.iAxisZ;
}

TInt CAcceleratorSensorRawData::SensorDescriptionId()
{
    JELOG2(ESensor);
    return EAccelerometerSensorRawData;
}

TReal CAcceleratorSensorRawData::InterpretValue(TReal aValue)
{
    JELOG2(ESensor);
    return aValue;
}

void CAcceleratorSensorRawData::DataReceived(CSensrvChannel& aChannel, TInt aCount, TInt aDataLost)
{
    JELOG2(ESensor);
    CAcceleratorSensor::DataReceived(aChannel, aCount, aDataLost);
}
