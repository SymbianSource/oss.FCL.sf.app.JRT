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
* Description:  Accelerometer sensor implementation
*
*/


// INTERNAL INCLUDES
#include "cacceleratorsensor.h"
#include "csensorproperties.h"
#include "sensorlistener.h"
#include "sensornativeconstants.h"
#include "logger.h"

#include <e32debug.h>

#include <sensrvtypes.h>


_LIT(KSeparator, "#");
_LIT(KDescription, "description=");
_LIT(KModel, "#model=Nokia#");
_LIT(KConnectionTypeNotSpecified, "connectiontype=0");
_LIT(KContextTypeUser, "user#");
_LIT(KDataTypeDouble, "1#");
_LIT(KValueScale, "0#");

_LIT(KMeasureRangeCount, "1#");
_LIT(KChannelTerminator, "#@ch_end#");

const TInt KChannels = 3;

const TInt KDataListening = 1;
const TInt KConditionListening = 2;
const TInt KGetDataListening = 4;
const TReal KStandardGravityValue = 9.812865328; // in m/s^2

CAcceleratorSensor::CAcceleratorSensor(const TSensrvChannelInfo& aChannelInfo) :
        iChannelInfo(aChannelInfo), iStartTime(KErrNotFound)
{
    JELOG2(ESensor);
    // No implementation needed
}

CAcceleratorSensor::~CAcceleratorSensor()
{
    JELOG2(ESensor);
    if (iChannel)
    {
        iChannel->StopDataListening();
        iChannel->CloseChannel();
    }
    delete iChannel;
}

CAcceleratorSensor* CAcceleratorSensor::NewLC(const TSensrvChannelInfo& aChannelInfo)
{
    JELOG2(ESensor);
    CAcceleratorSensor* self = new(ELeave) CAcceleratorSensor(aChannelInfo);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

CAcceleratorSensor* CAcceleratorSensor::NewL(const TSensrvChannelInfo& aChannelInfo)
{
    JELOG2(ESensor);
    CAcceleratorSensor* self = CAcceleratorSensor::NewLC(aChannelInfo);
    CleanupStack::Pop(); // self;
    return self;
}

void CAcceleratorSensor::ConstructL()
{
    JELOG2(ESensor);
    CSensorBase::ConstructL();
    iChannel = CSensrvChannel::NewL(iChannelInfo);
}

void CAcceleratorSensor::GetProperty(const RSensrvPropertyList aList,
                                     const TSensrvPropertyId aPropertyId,
                                     const TInt aItemIndex,
                                     TSensrvProperty& aProperty)
{
    JELOG2(ESensor);
    TInt propCount = aList.Count();
    for (int i = 0; i < propCount; i++)
    {
        aProperty = aList[i];
        if (aProperty.GetPropertyId() == aPropertyId &&
                aProperty.PropertyItemIndex() == aItemIndex)
        {
            return;
        }
    }
    TSensrvProperty prop;
    aProperty = prop;
}
// newcode for querying for the property using arrayindex also
void CAcceleratorSensor::GetProperty(const RSensrvPropertyList aList,
                                     const TSensrvPropertyId aPropertyId,
                                     const TInt aItemIndex,
                                     const TInt aArrayIndex,
                                     TSensrvProperty& aProperty)
{
    JELOG2(ESensor);
    TInt propCount = aList.Count();
    for (int i = 0; i < propCount; i++)
    {
        aProperty = aList[i];
        if (aProperty.GetPropertyId() == aPropertyId &&
                aProperty.PropertyItemIndex() == aItemIndex&&
                aProperty.GetArrayIndex()==aArrayIndex)

        {
            return;
        }
    }
    TSensrvProperty prop;
    aProperty = prop;
}

HBufC* CAcceleratorSensor::CreateDescriptionLC()
{
    JELOG2(ESensor);
    HBufC* description = HBufC::NewLC(KMaxSensorDescriptionLength);
    TPtr desPtr = description->Des();

    CSensrvChannel* channel = CSensrvChannel::NewL(iChannelInfo);
    CleanupStack::PushL(channel);
    channel->OpenChannelL();

    RSensrvPropertyList propList;
    CleanupClosePushL(propList);
    channel->GetAllPropertiesL(propList);
    channel->CloseChannel();

    // Quantity
    desPtr.Append(SensorProperties::GetPropertyString(
                      KQuantity, iChannelInfo.iQuantity));
    desPtr.Append(KSeparator);

    // ContextType
    desPtr.Append(KContextTypeUser);

    // Location
    TBuf<KSensrvLocationLength> location;
    location.Copy(iChannelInfo.iLocation);
    desPtr.Append(location);
    desPtr.Append(KSeparator);

    // Vendor
    TBuf<KSensrvVendorIdLength> vendor;
    vendor.Copy(iChannelInfo.iVendorId);
    desPtr.Append(vendor);
    desPtr.Append(KSeparator);

    // Description and Model
    desPtr.Append(KDescription);

    desPtr.Append(SensorProperties::GetPropertyString(KSensorDescription,
                  SensorDescriptionId()));

    Model(&desPtr);

    TSensrvProperty property;
    TInt intValue(0);

    // Connection type
    GetProperty(propList, KSensrvSensorConnectionType,    KSensrvItemIndexNone,
                property);
    if (property.PropertyType() == ESensrvIntProperty)
    {
        property.GetValue(intValue);
        desPtr.Append(SensorProperties::GetPropertyString(
                          KConnectionType, intValue));
    }
    else
    {
        desPtr.Append(KConnectionTypeNotSpecified);
    }
    desPtr.Append(KSeparator);

    // Availability
    GetProperty(propList, KSensrvPropIdAvailability, KSensrvItemIndexNone,
                property);
    if (property.PropertyType() == ESensrvIntProperty)
    {
        property.GetValue(intValue);
        desPtr.AppendNum(intValue);
    }
    else
    {
        desPtr.AppendNum(0);
    }
    desPtr.Append(KSeparator);

    //number of channels
    desPtr.AppendNum(KChannels);
    desPtr.Append(KSeparator);
    DescriptorAppendChannel(&desPtr, propList,
                            TSensrvAccelerometerAxisData::EAxisX);
    DescriptorAppendChannel(&desPtr, propList,
                            TSensrvAccelerometerAxisData::EAxisY);
    DescriptorAppendChannel(&desPtr, propList,
                            TSensrvAccelerometerAxisData::EAxisZ);

    CleanupStack::PopAndDestroy();
    CleanupStack::PopAndDestroy(channel);
    return description;
}

void CAcceleratorSensor::DescriptorAppendChannel(TPtr* aPtr,
        const RSensrvPropertyList aList,
        TInt aItemIndex)
{
    JELOG2(ESensor);
    TRealFormat realFormat;
    realFormat.iTriLen = 0;
    realFormat.iPoint = '.';
    TSensrvProperty property;
    TReal realValue(0);

    // name
    aPtr->Append(SensorProperties::GetPropertyString(
                     KAcceleratorChannelName, aItemIndex));
    aPtr->Append(KSeparator);

    // accuracy
    GetProperty(aList, KSensrvPropIdChannelAccuracy,
                KSensrvItemIndexNone, property);
    if (property.PropertyType() == ESensrvRealProperty)
    {
        property.GetValue(realValue);
        aPtr->AppendNum(realValue / 100, realFormat);
    }
    // the checking for int property was introduced for 9.2
    else if (property.PropertyType() == ESensrvIntProperty)
    {
        TInt intValue(0);
        property.GetValue(intValue);
        aPtr->AppendNum(intValue, realFormat);
    }
    else
    {
        aPtr->AppendNum(-1);
    }

    aPtr->Append(KSeparator);

    // data type
    DataType(aPtr);

    // scale
    aPtr->Append(KValueScale);

    // data unit
    DataUnit(aList, aPtr);
    aPtr->Append(KSeparator);

    // range count and range
    DataRange(aList, aPtr);
    aPtr->Append(KChannelTerminator);
}


CSensorBase* CAcceleratorSensor::DuplicateL()
{
    JELOG2(ESensor);
    return NewL(iChannelInfo);
}

TReal CAcceleratorSensor::InterpretValue(TReal aValue)
{
    JELOG2(ESensor);
    return iScaleFactor * aValue;
}

int CAcceleratorSensor::OpenChannel(SensorListener *aListener)
{
    JELOG2(ESensor);
    iSensorListener = aListener;
    TRAPD(err, OpenChannelL(););
    return err==KErrNotFound? KErrNone : err;
}

void CAcceleratorSensor::OpenChannelL()
{
    JELOG2(ESensor);
    LOG(ESensor,EInfo,"Creating Open Channel");
    iChannel->OpenChannelL();
    LOG(ESensor,EInfo,"Channel Created");
    // Check if the sensor data is scaled and set the scale factor
    iScaleFactor = 1;
    TInt format(ESensrvChannelDataFormatAbsolute);
    TSensrvProperty property;
    TRAPD(err1,iChannel->GetPropertyL(KSensrvPropIdChannelDataFormat, KSensrvItemIndexNone,
                                      property););
    LOG1(ESensor,EInfo,"CAcceleratorSensor::OpenChannelL()::GetPropertyL() One = %d",err1);
    if (property.PropertyType() == ESensrvIntProperty)
    {
        property.GetValue(format);
        LOG1(ESensor,EInfo,"CAcceleratorSensor::OpenChannelL()::GetValue()::KSensrvPropIdChannelDataFormat format = %d",format);
    }
    else
    {
        LOG(ESensor,EInfo,"CAcceleratorSensor::OpenChannelL()::KSensrvPropIdChannelDataFormat::Propety Type must be Integer");
        //KSensrvPropIdChannelDataFormat::Propety Type must be Integer
        __ASSERT_DEBUG(EFalse,User::Invariant());
    }

    if (format == ESensrvChannelDataFormatScaled)
    {
        LOG(ESensor,EInfo," format is equal to ESensrvChannelDataFormatScaled");
        // Scaled value maximum
        TSensrvProperty property_scaled;
        TInt maxScaled(0);
        TRAPD(err2,iChannel->GetPropertyL(KSensrvPropIdScaledRange, KSensrvItemIndexNone,
                                          property_scaled););
        LOG1(ESensor,EInfo,"CAcceleratorSensor::OpenChannelL()::GetPropertyL() Two = %d",err2);

        if (property_scaled.GetArrayIndex() == ESensrvArrayPropertyInfo)
        {
            LOG(ESensor,EInfo,"CAcceleratorSensor::OpenChannelL()::If");
            //for 2G Accelerometer Sensor index value is 0.
            TInt rangeIndex(0);
            TRAPD(err3,iChannel->GetPropertyL(KSensrvPropIdScaledRange, KSensrvItemIndexNone,
                                              rangeIndex, property_scaled););
            LOG1(ESensor,EInfo,"CAcceleratorSensor::OpenChannelL()::GetPropertyL() Three = %d",err3);

            property_scaled.GetMaxValue(maxScaled);

            LOG1(ESensor,EInfo,"property_scaled::maxValue = %d",maxScaled);
        }
        else
        {
            LOG(ESensor,EInfo,"CAcceleratorSensor::OpenChannelL()::else");
            property_scaled.GetMaxValue(maxScaled);
            LOG1(ESensor,EInfo,"CAcceleratorSensor::OpenChannelL()::maxScaled = %d",maxScaled);
        }
        // MeasureRange can be an array property
        TReal maxMeasure;
        TSensrvProperty property_Measured;
        TRAPD(err4,iChannel->GetPropertyL(KSensrvPropIdMeasureRange, KSensrvItemIndexNone,
                                          property_Measured););
        LOG1(ESensor,EInfo,"CAcceleratorSensor::OpenChannelL()::GetPropertyL() Four = %d",err4);

        if (err4 == KErrNone)
        {
            TInt arrayIndex = property_Measured.GetArrayIndex();
            LOG1(ESensor,EInfo,"CAcceleratorSensor::OpenChannelL()::KSensrvPropIdMeasureRange::arrayIndex = %d",arrayIndex);

            if (property_Measured.GetArrayIndex() == ESensrvArrayPropertyInfo)
            {
                LOG(ESensor,EInfo,"CAcceleratorSensor::OpenChannelL()::If");

                TInt index2g = 0;
                LOG1(ESensor,EInfo,"CAcceleratorSensor::OpenChannelL()::Getting RangeIndex = %f",index2g);
                //We need to TypeCast variable rangeIndex to TInt for
                TRAPD(err5,iChannel->GetPropertyL(KSensrvPropIdMeasureRange, KSensrvItemIndexNone,
                                                  index2g, property_Measured););
                LOG1(ESensor,EInfo,"CAcceleratorSensor::OpenChannelL()::GetPropertyL() Five = %d",err5);
                property_Measured.GetMaxValue(maxMeasure);
            }
            else
            {
                LOG(ESensor,EInfo,"CAcceleratorSensor::OpenChannelL()::else");
                property_Measured.GetMaxValue(maxMeasure);
            }

        }
        iScaleFactor = maxMeasure / maxScaled ;
        LOG1(ESensor,EInfo,"CAcceleratorSensor::OpenChannelL()::iScaleFactor = %f",iScaleFactor);
    }
    // this checking was introduced for 9.2
    else if (format == ESensrvChannelDataFormatAbsolute)
    {
        iScaleFactor = 2 * KStandardGravityValue / 127;
        // no need to calculate the scale factor
    }
}
void CAcceleratorSensor::CloseChannelL()
{
    JELOG2(ESensor);
    TInt err = iChannel->CloseChannel();
}

void CAcceleratorSensor::StartDataListeningL()
{
    JELOG2(ESensor);
    if (iStartTime == KErrNotFound)
    {
        iStartTime.UniversalTime();
    }
    iChannelXData = iData[ 0 ];
    iChannelYData = iData[ 1 ];
    iChannelZData = iData[ 2 ];
    if (!iListeningType)
    {
        iChannel->StartDataListeningL(this, iBufferSize, iBufferSize,
                                      iBufferingPeriod/1000);
    }
    if (iIsOneShot)
    {
        iListeningType = iListeningType | KGetDataListening;
    }
    else
    {
        iListeningType = iListeningType | KDataListening;
    }
}


void CAcceleratorSensor::CancelDataListeningL()
{
    JELOG2(ESensor);
    if (iListeningType & KDataListening)
    {
        iListeningType = iListeningType ^ KDataListening;
    }
    if (!iListeningType)
    {
        User::LeaveIfError(iChannel->StopDataListening());
    }
}


void CAcceleratorSensor::StopConditionListeningL()
{
    JELOG2(ESensor);
    if (iListeningType & KConditionListening)
    {
        iListeningType = iListeningType ^ KConditionListening;
        User::LeaveIfError(iChannel->StopDataListening());
        if (iListeningType)
        {
            iChannel->StartDataListeningL(this, iBufferSize, iBufferSize,
                                          iBufferingPeriod/1000);
        }
    }
}


void CAcceleratorSensor::StartConditionListeningL()
{
    JELOG2(ESensor);
    if (iListeningType & (KDataListening | KGetDataListening))
    {
        iChannel->StopDataListening();
    }
    if (!(iListeningType & KConditionListening))
    {

        // changing -1 to 0 for buffering period since -1 throws KErrArgument in 9.2
        iChannel->StartDataListeningL(this, 1, 1 /* listen every value*/, 0);
    }
    iListeningType = iListeningType | KConditionListening;
}

void CAcceleratorSensor::DataReceived(CSensrvChannel& aChannel,
                                      TInt aCount,
                                      TInt aDataLost)
{
    JELOG2(ESensor);
    TSensrvAccelerometerAxisData data;
    TPckgBuf<TSensrvAccelerometerAxisData> dataBuf;
    TBool dataFetched = EFalse;
    TTime currentTime;
    currentTime.UniversalTime();
    // no need for data preparation if only listening conditions
    if (iListeningType & (KDataListening | KGetDataListening))
    {
        // Do not go over buffer count
        if (iChannelXData->iNumOfValues + aCount > iBufferSize)
        {
            aCount = iBufferSize - iChannelXData->iNumOfValues;
        }

        // Prepare own data
        for (TInt i = iChannelXData->iNumOfValues; i < aCount + iChannelXData->iNumOfValues; i++)
        {
            aChannel.GetData(dataBuf);
            data = dataBuf();
            dataFetched = ETrue;
            CopyData(data, i);
            // add timestamps, if needed
            if (iChannelXData->iTimeStampsIncluded)
            {
                // Sensrv api returns timestams in localtime java requires UTC
                iChannelXData->iTimeStamps[ i ] = TimeStamp(currentTime);
                iChannelYData->iTimeStamps[ i ] = TimeStamp(currentTime);
                iChannelZData->iTimeStamps[ i ] = TimeStamp(currentTime);
            }
        }
        iChannelXData->iNumOfValues += aCount;
        iChannelYData->iNumOfValues += aCount;
        iChannelZData->iNumOfValues += aCount;

        if (iChannelXData->iNumOfValues == iBufferSize || iBufferingPeriod > 0
                || (iListeningType & KConditionListening
                    && currentTime.MicroSecondsFrom(iStartTime)
                    >= iBufferingPeriod))
        {
            if (iListeningType & KGetDataListening)
            {
                iListeningType = iListeningType ^ KGetDataListening;
                if (!(iListeningType & KConditionListening))
                {
                    iChannel->StopDataListening();
                }
            }
            // callback
            iSensorListener->DataReceived(iData, aDataLost > 0);
            iChannelXData->iNumOfValues = 0;
            iChannelYData->iNumOfValues = 0;
            iChannelZData->iNumOfValues = 0;
            iStartTime = KErrNotFound;
        }
    }
    // evaluate conditions if condition listening is on
    if (iListeningType & KConditionListening)
    {
        // get data if not done already by datalistening routine
        if (!dataFetched)
        {
            aChannel.GetData(dataBuf);
            data = dataBuf();
        }
        EvaluateConditions(data.iAxisX, 0);
        EvaluateConditions(data.iAxisY, 1);
        EvaluateConditions(data.iAxisZ, 2);
        if (!iJavaConditionEval && iConditions.Count() == 0)
        {
            TRAP_IGNORE(StopConditionListeningL());
        }
    }
}

void CAcceleratorSensor::DataError(CSensrvChannel& /*aChannel*/,
                                   TSensrvErrorSeverity /*aError*/)
{
    JELOG2(ESensor);
    // Currently we don't care wether error is minor or fatal
    if (!(iListeningType ^ KGetDataListening))
    {
        iChannel->StopDataListening();
    }
    TSensrvAccelerometerAxisData data;
    for (TInt i = iChannelXData->iNumOfValues; i < iBufferSize; i++)
    {
        data.iAxisX = 0;
        data.iAxisY = 0;
        data.iAxisZ = 0;
        CopyData(data, i);
        if (iValiditiesIncluded)
        {
            iChannelXData->iValidities[i] = EFalse;
            iChannelYData->iValidities[i] = EFalse;
            iChannelZData->iValidities[i] = EFalse;
        }
    }
    iSensorListener->DataReceived(iData, ETrue);
    iChannelXData->iNumOfValues = 0;
    iChannelYData->iNumOfValues = 0;
    iChannelZData->iNumOfValues = 0;
    iStartTime = KErrNotFound;
}

void CAcceleratorSensor::GetDataListenerInterfaceL(TUid /*aInterfaceUid*/,
        TAny*& aInterface)
{
    JELOG2(ESensor);
    aInterface = NULL;
    // For future extension, implement when needed
}

void CAcceleratorSensor::Model(TPtr* aPtr)
{
    JELOG2(ESensor);
    aPtr->Append(KModel);
}

void CAcceleratorSensor::DataType(TPtr* aPtr)
{
    JELOG2(ESensor);
    aPtr->Append(KDataTypeDouble);
}

void CAcceleratorSensor::DataUnit(const RSensrvPropertyList aList, TPtr* aPtr)
{
    JELOG2(ESensor);
    TSensrvProperty property;
    TInt intValue(0);
    GetProperty(aList, KSensrvPropIdChannelUnit, KSensrvItemIndexNone, property);
    if (property.PropertyType() == ESensrvIntProperty)
    {
        property.GetValue(intValue);
        aPtr->Append(SensorProperties::GetPropertyString(KChannelUnit, intValue));
    }
    else
    {
        aPtr->Append(SensorProperties::GetPropertyString(KChannelUnit,
                     ESensrvChannelUnitNotDefined));
    }
}

void CAcceleratorSensor::DataRange(const RSensrvPropertyList aList,
                                   TPtr* aPtr)
{
    JELOG2(ESensor);
    TRealFormat realFormat;
    realFormat.iTriLen = 0;
    realFormat.iPoint = '.';
    realFormat.iPlaces = 2;
    TSensrvProperty property;
    TReal realValue(0);
    GetProperty(aList, KSensrvPropIdMeasureRange,KSensrvItemIndexNone, property);

    // the extra checking for arraypropertyindex was introduced for 9.2 driver changes
    // for 5.0 and 9.2 both the platforms first and second IF blocks should be executed
    if ((property.GetArrayIndex() == ESensrvArrayPropertyInfo)&&
            (property.PropertyType() == ESensrvRealProperty))
    {
        LOG(ESensor,EInfo,"CAcceleratorSensor::DataRange()::KSensrvPropIdMeasureRange is ESensrvArrayPropertyInfo and ESensrvRealProperty::first IF");
        // Read index
        TInt index2g = 0;
        // Read actual property data from array
        GetProperty(aList, KSensrvPropIdMeasureRange,
                    KSensrvItemIndexNone, index2g, property);
        LOG1(ESensor,EInfo,"CAcceleratorSensor::OpenChannelL()::DataRange() index = %f",index);
    }

    if (property.PropertyType() == ESensrvRealProperty)
    {
        LOG(ESensor,EInfo,"CAcceleratorSensor::DataRange()::KSensrvPropIdMeasureRange ESensrvRealProperty::second IF");
        aPtr->Append(KMeasureRangeCount);
        property.GetMinValue(realValue);   // min
        LOG1(ESensor,EInfo,"CAcceleratorSensor::OpenChannelL()::DataRange() min = %f",realValue);
        aPtr->AppendNum(realValue, realFormat);
        aPtr->Append(KSeparator);
        property.GetMaxValue(realValue);   // max
        LOG1(ESensor,EInfo,"CAcceleratorSensor::OpenChannelL()::DataRange() max = %f",realValue);
        aPtr->AppendNum(realValue, realFormat);
        aPtr->Append(KSeparator);
        property.GetValue(realValue);   // resolution
        //resolution is minimum difference between the data values
        TReal resolution = realValue /127 ;
        LOG1(ESensor,EInfo,"CAcceleratorSensor::OpenChannelL()::DataRange() val = %f",resolution);
        aPtr->AppendNum(resolution, realFormat);
    }
    // the extra checking for ESensrvIntProperty was introduced for 9.2 driver changes
    else if (property.PropertyType() == ESensrvIntProperty)
    {
        //This condition is an extra check and not supposed to be entered.
        LOG(ESensor,EInfo,"CAcceleratorSensor::DataRange()::KSensrvPropIdMeasureRange ESensrvIntProperty::ELSE IF");
        TReal min,max,resolution;
        // Read TInt values
        TInt minInt,maxInt,resolutionInt;
        property.GetMinValue(minInt);
        property.GetMaxValue(maxInt);
        property.GetValue(resolutionInt);

        // Convert integers to TReal
        min=-((resolutionInt * KStandardGravityValue)/1000);
        max=((resolutionInt * KStandardGravityValue)/1000);
        // Resolution is minimum value difference which can be caught.
        // for ex. in case of raw data(-128 -to- 127) the resolution is 1
        resolution=(resolutionInt* KStandardGravityValue)/(1000 * 127);
        aPtr->Append(KMeasureRangeCount);
        aPtr->AppendNum(min, realFormat);
        aPtr->Append(KSeparator);
        aPtr->AppendNum(max, realFormat);
        aPtr->Append(KSeparator);
        aPtr->AppendNum(resolution, realFormat);

    }
    else
    {
        aPtr->AppendNum(0);
    }
}
void CAcceleratorSensor::CopyData(const TSensrvAccelerometerAxisData aData, TInt aIndex)
{
    JELOG2(ESensor);
    iChannelXData->iDoubleValues[ aIndex ] = aData.iAxisX * iScaleFactor;
    iChannelYData->iDoubleValues[ aIndex ] = aData.iAxisY * iScaleFactor;
    iChannelZData->iDoubleValues[ aIndex ] = aData.iAxisZ * iScaleFactor;
}

TInt CAcceleratorSensor::SensorDescriptionId()
{
    JELOG2(ESensor);
    return EAccelerometerSensor;
}
