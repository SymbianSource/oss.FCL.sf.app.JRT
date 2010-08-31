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
* Description:  Network field intensity sensor implementation
*
*/

#include <etel3rdparty.h>
#include <e32math.h>
#include "cnetworkfieldintensitysensor.h"
#include "sensorlistener.h"
#include "csensorproperties.h"
#include "logger.h"

const TInt KNISensorChannel = 0;
const TInt KNISensorInvalidValue = 0;
const TInt KNIMaxBars = 7;

_LIT(KNetworkFieldIntensitySensorDescriptionPart1,
     "network_field_intensity#"
     L"device#"
     L"device#"
     L"Nokia#"
     L"description=");
_LIT(KNetworkFieldIntensitySensorDescriptionPart2,
     "#model=Nokia#"
     L"connectiontype=1#"
     L"1#" // isAvailable
     L"1#" // channel count
     L"network_field_intensity#" // channel name. for single channel sensors should be same as model
     L"0#" // accuracy, 100 steps (?)
     L"2#" // data type. here TYPE_INT
     L"0#" // scale. here zero (means that scaling is not used)
     L"%#" // unit. percent (?)
     L"1#" // measurement range count
     L"0#" // smallest measurable value
     L"100#" // largest measurable value
     L"1#" // measurement resolution
     L"@ch_end#");  // channel terminator constant

CNetworkFieldIntensitySensor* CNetworkFieldIntensitySensor::NewL()
{
    JELOG2(ESensor);
    CNetworkFieldIntensitySensor* self = new(ELeave)CNetworkFieldIntensitySensor();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
}

CNetworkFieldIntensitySensor::CNetworkFieldIntensitySensor()
{
    JELOG2(ESensor);
}

void CNetworkFieldIntensitySensor::ConstructL()
{
    JELOG2(ESensor);
    CSensorBase::ConstructL();
}

CNetworkFieldIntensitySensor::~CNetworkFieldIntensitySensor()
{
    JELOG2(ESensor);
    delete iNotifier;
}

int CNetworkFieldIntensitySensor::OpenChannel(SensorListener* aListener)
{
    JELOG2(ESensor);
    iSensorListener = aListener;
    TRAPD(err, iNotifier = CSignalAsyncHandler::NewL(*this));
    return err; // KErrNone if succesful
}

void CNetworkFieldIntensitySensor::StartDataListeningL()
{
    JELOG2(ESensor);
    iNotifier->Cancel();
    ResetAccumulationData();
    iNotifier->SetDataListeningMode(ETrue);
}

void CNetworkFieldIntensitySensor::CancelDataListeningL()
{
    JELOG2(ESensor);
    iNotifier->Cancel();
    iNotifier->SetDataListeningMode(EFalse);
}

void CNetworkFieldIntensitySensor::CloseChannelL()
{
    JELOG2(ESensor);
    iSensorListener = NULL;
    delete iNotifier;
    iNotifier = NULL;
}

void CNetworkFieldIntensitySensor::StartConditionListeningL()
{
    JELOG2(ESensor);
    iNotifier->Cancel();
    iNotifier->SetConditionListeningMode(ETrue);
}

void CNetworkFieldIntensitySensor::StopConditionListeningL()
{
    JELOG2(ESensor);
    iNotifier->Cancel();
    iNotifier->SetConditionListeningMode(EFalse);
}

void CNetworkFieldIntensitySensor::ResetAccumulationData()
{
    JELOG2(ESensor);
    iData[ KNISensorChannel ]->iNumOfValues = 0;
    iStartTime = KErrNotFound;
}

void CNetworkFieldIntensitySensor::SendData()
{
    JELOG2(ESensor);
    if (iIsOneShot)
    {
        iNotifier->Cancel();
        iNotifier->SetDataListeningMode(EFalse);
    }
    iSensorListener->DataReceived(iData, EFalse);
    ResetAccumulationData();
}

void CNetworkFieldIntensitySensor::SignalLevelReceivedL(TInt aLevel)
{
    JELOG2(ESensor);
    __ASSERT_DEBUG(iData != NULL, User::Invariant());
    SensorData* data = iData[ KNISensorChannel ];

    TTime currentTime;
   
        if ((data->iTimeStampsIncluded) || (iBufferingPeriod > 0))
        {
            currentTime.UniversalTime();
            if (iStartTime == KErrNotFound)
            {
                iStartTime = currentTime;
            }
        }
    
    // P&S sensors are assumed to have one channel only
    TInt interpretedValue = InterpretValue(aLevel);

    // Negative values are error codes and therefore marked
    // as invalid. Returned value for invalid values is zero.
    if (interpretedValue < 0)
    {
        data->iIntValues[ data->iNumOfValues ] = KNISensorInvalidValue;
        if (data->iValiditiesIncluded)
        {
            data->iValidities[ data->iNumOfValues ] = EFalse;
        }
    }
    else
    {
        data->iIntValues[ data->iNumOfValues ] = interpretedValue;
        if (data->iValiditiesIncluded)
        {
            data->iValidities[ data->iNumOfValues ] = ETrue;
        }
    }

    if (data->iTimeStampsIncluded)
    {
        data->iTimeStamps[ data->iNumOfValues ] = TimeStamp(currentTime);
    }
    data->iNumOfValues++;

    TInt64 intervalFromStart = -1;
    if (iBufferingPeriod > 0)
    {
        intervalFromStart = currentTime.MicroSecondsFrom(iStartTime).Int64();
    }

    if ((data->iNumOfValues == iBufferSize) ||
            (iBufferingPeriod > 0 && intervalFromStart > iBufferingPeriod))
    {
        SendData();
    }
}

void CNetworkFieldIntensitySensor::SignalLevelChangedL(TInt aLevel)
{
    JELOG2(ESensor);
    // Values lower than zero are error code and do not qualify for
    // signal change notification
    if (aLevel >= 0)
    {
        EvaluateConditions(aLevel, KNISensorChannel);
    }
}

HBufC* CNetworkFieldIntensitySensor::CreateDescriptionLC()
{
    JELOG2(ESensor);
    HBufC* desc = HBufC::NewLC(KMaxSensorDescriptionLength);
    TPtr desPtr = desc->Des();
    desPtr.Append(KNetworkFieldIntensitySensorDescriptionPart1);
    desPtr.Append(SensorProperties::GetPropertyString(KSensorDescription,
                  ENetworkFieldIntensitySensor));
    desPtr.Append(KNetworkFieldIntensitySensorDescriptionPart2);
    return desc;
}

CSensorBase* CNetworkFieldIntensitySensor::DuplicateL()
{
    JELOG2(ESensor);
    return NewL();
}

TReal CNetworkFieldIntensitySensor::InterpretValue(TReal aValue)
{
    JELOG2(ESensor);
    // Network intensity is between 0 and 100. Values below zero
    // are error codes and are returned as they are.
    return aValue > 0  ? aValue / KNIMaxBars * 100 : aValue;
}

// **************************************************************************
// CSignalAsyncHandler
// **************************************************************************

CNetworkFieldIntensitySensor::CSignalAsyncHandler*
CNetworkFieldIntensitySensor::CSignalAsyncHandler::NewL(
    CNetworkFieldIntensitySensor& aSensor)
{
    JELOG2(ESensor);
    CSignalAsyncHandler* self =
        new(ELeave)CSignalAsyncHandler(aSensor);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
}

CNetworkFieldIntensitySensor::CSignalAsyncHandler::CSignalAsyncHandler(
    CNetworkFieldIntensitySensor& aSensor)
        : CActive(EPriorityStandard), iSensor(aSensor),
        iSigStrengthV1Pckg(iSigStrengthV1), iCurrentOperation(EOperationNone),
        iPrevSignalStrength(KErrNotFound)
{
    JELOG2(ESensor);
}

CNetworkFieldIntensitySensor::CSignalAsyncHandler::~CSignalAsyncHandler()
{
    JELOG2(ESensor);
    Cancel();
    delete iTelephony;
}

void CNetworkFieldIntensitySensor::CSignalAsyncHandler::ConstructL()
{
    JELOG2(ESensor);
    iTelephony = CTelephony::NewL();
    CActiveScheduler::Add(this);
}

void CNetworkFieldIntensitySensor::CSignalAsyncHandler::RunL()
{
    JELOG2(ESensor);
#ifdef __WINS__
#pragma message("CNetworkFieldIntensitySensor: Test value used for WINS.")
    TInt signalStrength = 1;
#else
    TInt signalStrength = (iStatus == KErrNone) ?
                          iSigStrengthV1.iBar : KErrNotFound;
#endif

    if (iDataListening)
    {
        iCurrentOperation = EGetSignalIntensity;
        iTelephony->GetSignalStrength(iStatus, iSigStrengthV1Pckg);
        SetActive();
        iSensor.SignalLevelReceivedL(signalStrength);
        if (iConditionListening && (signalStrength != iPrevSignalStrength))
        {
            iSensor.SignalLevelChangedL(signalStrength);
        }
    }
    else if (iConditionListening)
    {
        iCurrentOperation = ENotifySignalIntensityChange;
        iTelephony->NotifyChange(iStatus,
                                 CTelephony::ESignalStrengthChange, iSigStrengthV1Pckg);
        SetActive();
        iSensor.SignalLevelChangedL(signalStrength);
    }
    iPrevSignalStrength = signalStrength;
}

void CNetworkFieldIntensitySensor::CSignalAsyncHandler::DoCancel()
{
    JELOG2(ESensor);
    if (iCurrentOperation == EGetSignalIntensity)
    {
        iTelephony->CancelAsync(CTelephony::EGetSignalStrengthCancel);
    }
    else if (iCurrentOperation == ENotifySignalIntensityChange)
    {
        iTelephony->CancelAsync(CTelephony::ESignalStrengthChangeCancel);
    }
    iCurrentOperation = EOperationNone;
}

void CNetworkFieldIntensitySensor::CSignalAsyncHandler::
SetDataListeningMode(TBool aIsDataListening)
{
    JELOG2(ESensor);
    __ASSERT_DEBUG(!IsActive(), User::Invariant());
    iDataListening = aIsDataListening;
    Start();
}

void CNetworkFieldIntensitySensor::CSignalAsyncHandler::
SetConditionListeningMode(TBool aIsConditionListening)
{
    JELOG2(ESensor);
    __ASSERT_DEBUG(!IsActive(), User::Invariant());
    iConditionListening = aIsConditionListening;
    Start();
}
void CNetworkFieldIntensitySensor::CSignalAsyncHandler::Start()
{
    JELOG2(ESensor);
    if (iDataListening || iConditionListening)
    {
        iPrevSignalStrength = KErrNotFound;
        iCurrentOperation = EGetSignalIntensity;
        iTelephony->GetSignalStrength(iStatus, iSigStrengthV1Pckg);
        SetActive();
    }
}
