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
* Description:  Base class for Publish & Subscripe -type Sensors
*
*/


#include "sensorlistener.h"
#include "cpssensorbase.h"
#include "logger.h"

const TInt KPSSensorChannel = 0;
const TInt KPSSensorInvalidValue = 0;

CPSSensorBase::CPSSensorBase(TTimeIntervalMicroSeconds32 aPollInterval)
        : iPollInterval(aPollInterval)
{
    JELOG2(ESensor);
}

CPSSensorBase::~CPSSensorBase()
{
    JELOG2(ESensor);
    iProperty.Close();
    if (iAsyncQuery)
    {
        iAsyncQuery->Cancel();
        delete iAsyncQuery;
    }
    if (iPollTimer)
    {
        iPollTimer->Cancel();
        delete iPollTimer;
    }
}

void CPSSensorBase::ConstructL(TUid aPropertyCategory, TInt aPropertyKey)
{
    JELOG2(ESensor);
    iPropertyCategory = aPropertyCategory;
    iPropertyKey = aPropertyKey;
    CSensorBase::ConstructL();
}

int CPSSensorBase::OpenChannel(SensorListener* aListener)
{
    JELOG2(ESensor);
    TRAPD(err, OpenChannelL(aListener));
    return err;
}

void CPSSensorBase::OpenChannelL(SensorListener* aListener)
{
    JELOG2(ESensor);
    iSensorListener = aListener;
    iPollTimer = CPSSensorDataListenTimer::NewL(*this);
    CActiveScheduler::Add(iPollTimer);

    iAsyncQuery = CPSSensorBaseAsyncQuery::NewL(*this, iPropertyCategory, iPropertyKey);
    CActiveScheduler::Add(iAsyncQuery);

    User::LeaveIfError(iProperty.Attach(iPropertyCategory, iPropertyKey));
}

void CPSSensorBase::StartDataListeningL()
{
    JELOG2(ESensor);
    if (iPollTimer->IsActive())
    {
        iPollTimer->Cancel();
    }
    ResetAccumulationData();
    AccumulateDataL();
}

void CPSSensorBase::CancelDataListeningL()
{
    JELOG2(ESensor);
    iPollTimer->Cancel();
}

void CPSSensorBase::CloseChannelL()
{
    JELOG2(ESensor);
    iSensorListener = NULL;
}

void CPSSensorBase::StartConditionListeningL()
{
    JELOG2(ESensor);
    // Evaluate current value right away to check if the current
    // value already matches
    PropertyValueChanged();

    // Request notification of property value change
    iAsyncQuery->NotifyOnChange();
}

void CPSSensorBase::StopConditionListeningL()
{
    JELOG2(ESensor);
    iAsyncQuery->Cancel();
}

TBool CPSSensorBase::EvaluateConditions(TInt aValue, TInt aChannelId)
{
    JELOG2(ESensor);
    // Values below zero are error codes; no condition listening callbacks
    // for invalid values.
    if (aValue < 0)
    {
        return EFalse;
    }
    return CSensorBase::EvaluateConditions(aValue, aChannelId);
}


void CPSSensorBase::AddConditionL(void** aHandle, int aChannelId,
                                  double aLowerLimit, double aUpperLimit, int aLowerOp,
                                  int aUpperOp)
{
    JELOG2(ESensor);
    CSensorBase::AddConditionL(aHandle, aChannelId, aLowerLimit,
                               aUpperLimit, aLowerOp, aUpperOp);

    // Check conditions right away for current value
    PropertyValueChanged();
}

void CPSSensorBase::ResetAccumulationData()
{
    JELOG2(ESensor);
    iData[ KPSSensorChannel ]->iNumOfValues = 0;
    iStartTime = KErrNotFound;
}

void CPSSensorBase::AccumulateDataL()
{
    JELOG2(ESensor);
    __ASSERT_DEBUG(iData != NULL, User::Invariant());
    SensorData* data = iData[ KPSSensorChannel ];

    // Request new data accumulation event
    iPollTimer->After(iPollInterval);

    TTime currentTime;
    if (data->iTimeStampsIncluded || (iBufferingPeriod > 0))
    {
        currentTime.UniversalTime();
        if (iStartTime == KErrNotFound)
        {
            iStartTime = currentTime;
        }
    }

    TInt currentValue = 0;
    TInt err = iProperty.Get(currentValue);
    __ASSERT_DEBUG(err == KErrNone, User::Invariant());

    // P&S sensors are assumed to have one channel only
    TInt interpretedValue = InterpretValue(currentValue);

    // Negative values are error codes and therefore marked
    // as invalid. Returned value for invalid values is zero.
    if (interpretedValue < 0)
    {
        data->iIntValues[ data->iNumOfValues ] = KPSSensorInvalidValue;
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

void CPSSensorBase::SendData()
{
    JELOG2(ESensor);
    if (iIsOneShot)
    {
        iPollTimer->Cancel();
    }
    iSensorListener->DataReceived(iData, EFalse);
    ResetAccumulationData();
}

void CPSSensorBase::PropertyValueChanged()
{
    JELOG2(ESensor);
    TInt value;
    TInt err = iProperty.Get(value);
    __ASSERT_DEBUG(err == KErrNone, User::Invariant());

    // Negative values are sensor value range
    if (err == KErrNone && value >= 0)
    {
        EvaluateConditions(value, KPSSensorChannel);
    }
}

// ***************************************************************************
// CPSSensorBaseAsyncQuery
// ***************************************************************************

CPSSensorBase::CPSSensorBaseAsyncQuery*
CPSSensorBase::CPSSensorBaseAsyncQuery::NewL(CPSSensorBase& aSensorBase,
        TUid aPropertyCategory, TInt aPropertyKey)
{
    JELOG2(ESensor);
    CPSSensorBaseAsyncQuery* self =
        new(ELeave)CPSSensorBaseAsyncQuery(aSensorBase);
    CleanupStack::PushL(self);
    self->ConstructL(aPropertyCategory, aPropertyKey);
    CleanupStack::Pop(); // self
    return self;
}

CPSSensorBase::CPSSensorBaseAsyncQuery::CPSSensorBaseAsyncQuery(CPSSensorBase& aSensorBase)
        : CActive(EPriorityStandard), iSensorBase(aSensorBase)
{
    JELOG2(ESensor);
}

void CPSSensorBase::CPSSensorBaseAsyncQuery::ConstructL(TUid aPropertyCategory, TInt aPropertyKey)
{
    JELOG2(ESensor);
    User::LeaveIfError(iProperty.Attach(aPropertyCategory, aPropertyKey));
}

CPSSensorBase::CPSSensorBaseAsyncQuery::~CPSSensorBaseAsyncQuery()
{
    JELOG2(ESensor);
    iProperty.Close();
}

void CPSSensorBase::CPSSensorBaseAsyncQuery::NotifyOnChange()
{
    JELOG2(ESensor);
    if (!IsActive())
    {
        iProperty.Subscribe(iStatus);
        SetActive();
    }
}

void CPSSensorBase::CPSSensorBaseAsyncQuery::RunL()
{
    JELOG2(ESensor);
    iProperty.Subscribe(iStatus);
    SetActive();
    iSensorBase.PropertyValueChanged();
}

void CPSSensorBase::CPSSensorBaseAsyncQuery::DoCancel()
{
    JELOG2(ESensor);
    iProperty.Cancel();
}

// ***************************************************************************
// CPSSensorDataListenTimer
// ***************************************************************************

CPSSensorBase::CPSSensorDataListenTimer*
CPSSensorBase::CPSSensorDataListenTimer::NewL(CPSSensorBase& aSensorBase)
{
    JELOG2(ESensor);
    CPSSensorDataListenTimer* self =
        new(ELeave)CPSSensorDataListenTimer(aSensorBase);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
}

CPSSensorBase::CPSSensorDataListenTimer::CPSSensorDataListenTimer(
    CPSSensorBase& aSensorBase)
        : CTimer(EPriorityNormal), iSensorBase(aSensorBase)
{
    JELOG2(ESensor);
}

CPSSensorBase::CPSSensorDataListenTimer::~CPSSensorDataListenTimer()
{
    JELOG2(ESensor);
}

void CPSSensorBase::CPSSensorDataListenTimer::RunL()
{
    JELOG2(ESensor);
    iSensorBase.AccumulateDataL();
}

void CPSSensorBase::CPSSensorDataListenTimer::DoCancel()
{
    JELOG2(ESensor);
}
