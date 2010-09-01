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
* Description:  Base class for sensors
*
*/


#include "csensorbase.h"
#include "csensorlimitcondition.h"
#include "csensorrangecondition.h"
#include "sensorlistener.h"
#include "logger.h"
#include <bautils.h>
#include <barsc.h>

const TUint JavaUpperTimeFor1970 = 14474675;
const TUint JavaLowerTimeFor1970 = 254771200;

CSensorBase::~CSensorBase()
{
    JELOG2(ESensor);
    iConditions.ResetAndDestroy();
    iConditions.Close();
    Cancel();
    delete iActiveScheduler;
    iRunnerThread.Close();
    iMethodArray.Close();
}
CSensorBase::CSensorBase() : CActive(EPriorityHigh),
        iListeningType(0)
{
    JELOG2(ESensor);
}

void CSensorBase::ConstructL()
{
    JELOG2(ESensor);
    User::LeaveIfError(iRunnerThread.Open(RThread().Id()));
    if (!CActiveScheduler::Current())
    {
        iActiveScheduler = new(ELeave) CActiveScheduler();
        CActiveScheduler::Install(iActiveScheduler);
    }
    CActiveScheduler::Add(this);
}

TInt64 CSensorBase::TimeStamp(const TTime& aTime)
{
    JELOG2(ESensor);
    // Create a TTime object that represents the Java Date
    // 'epoch' time of 00:00, 1 Jan 1970
    TInt64 javaEpocTimeNum =
        MAKE_TINT64(JavaUpperTimeFor1970, JavaLowerTimeFor1970);
    TTime javaEpochTime(javaEpocTimeNum);
    // Find difference in microseconds between 'epoch' and EPOC
    // date and adjust to milliseconds
    TTimeIntervalMicroSeconds microInterval =
        aTime.MicroSecondsFrom(javaEpochTime);
    TInt64 intervalNum = microInterval.Int64();
    intervalNum /= 1000;
    return intervalNum;
}

void CSensorBase::StartServer()
{
    JELOG2(ESensor);
    CActiveScheduler::Start();
}

void CSensorBase::StopServer()
{
    JELOG2(ESensor);
    TMethod method(TMethod::EStopServer);
    AsyncCallback(method);
}

int CSensorBase::StartDataListening(SensorData** aData,
                                    int aBufferSize,
                                    long aBufferingPeriod,
                                    bool aTimestampsIncluded,
                                    bool aValiditiesIncluded,
                                    bool aIsOneShot)
{
    JELOG2(ESensor);
    TMethod method(TMethod::EStartDataListening);
    method.SetSensorData(aData);
    method.SetBufferSize(aBufferSize);
    method.SetBufferingPeriod(aBufferingPeriod);
    method.SetTimestampsIncluded(aTimestampsIncluded);
    method.SetValiditiesIncluded(aValiditiesIncluded);
    method.SetIsOneShot(aIsOneShot);
    AsyncCallback(method);
    return KErrNone;
}

int CSensorBase::StartConditionListening(int aJavaConditionEval)
{
    JELOG2(ESensor);
    TMethod method(TMethod::EStartConditionListening);
    method.SetJavaConditionEval(aJavaConditionEval);
    AsyncCallback(method);
    return KErrNone;
}

int CSensorBase::CancelDataListening()
{
    JELOG2(ESensor);
    TMethod method(TMethod::ECancelDataListening);
    AsyncCallback(method);
    return KErrNone;
}

int CSensorBase::StopConditionListening()
{
    JELOG2(ESensor);
    TMethod method(TMethod::EStopConditionListening);
    AsyncCallback(method);
    return KErrNone;
}

void CSensorBase::CloseChannel()
{
    JELOG2(ESensor);
    TMethod method(TMethod::ECloseChannel);
    AsyncCallback(method);
}

void CSensorBase::AsyncCallback(TMethod aMethod)
{
    JELOG2(ESensor);
    TInt err = iMethodArray.Append(aMethod);
    // if there was error when adding the method to array then just return
    if ( err  )
    {
        ELOG2(ESensor, "CSensorBase::AsyncCallback - Error (code = %d) when "
              "adding method (type = %d) to method buffer.", err, aMethod.iMethodType);
        return;
    }

    // If there is old request ongoing, wait it for completion
    if (iMethodArray.Count() > 1)
    {
        return;
    }
    iStatus = KRequestPending;
    TRequestStatus* status = &iStatus;
    SetActive();
    iRunnerThread.RequestComplete(status, KErrNone);
}

void CSensorBase::RunL()
{
    JELOG2(ESensor);
    // RunL should never be running if there is no method in array
    __ASSERT_DEBUG(iMethodArray.Count() > 0, User::Invariant());
    TMethod method = iMethodArray[0];

    switch (method.GetMethodType())
    {
    case TMethod::EStartDataListening:
    {
        iData = method.GetSensorData();
        iBufferSize = method.GetBufferSize();
        iBufferingPeriod = 1000 * method.GetBufferingPeriod();
        iIsOneShot = method.GetIsOneShot();
        iTimestampsIncluded = method.GetTimestampsIncluded();
        iValiditiesIncluded = method.GetValiditiesIncluded();

        StartDataListeningL();
        break;
    }
    case TMethod::EStartConditionListening:
    {
        iJavaConditionEval = method.GetJavaConditionEval();
        StartConditionListeningL();
        break;
    }
    case TMethod::EStopConditionListening:
    {
        StopConditionListeningL();
        break;
    }
    case TMethod::ECancelDataListening:
    {
        CancelDataListeningL();
        break;
    }
    case TMethod::ECloseChannel:
    {
        CloseChannelL();
        break;
    }
    case TMethod::EStopServer:
    {
        CActiveScheduler::Stop();
        break;
    }
    case TMethod::ERemoveCondition:
    {
        RemoveConditionL(method.GetConditionToRemove());
        break;
    }
    default:
    {
        break;
    }
    }
    // If there is still methods left in array, then request new run
    if (iMethodArray.Count() > 1)
    {
        iStatus = KRequestPending;
        TRequestStatus* status = &iStatus;
        SetActive();
        iRunnerThread.RequestComplete(status, KErrNone);
    }
    iMethodArray.Remove(0);
}

void CSensorBase::DoCancel()
{
    JELOG2(ESensor);
}

TBool CSensorBase::EvaluateConditions(TReal aValue, TInt aChannelId)
{
    JELOG2(ESensor);
    TReal currentValue = InterpretValue(aValue);
    TBool matched = EFalse;
    TTime time;
    time.UniversalTime();
    TInt64 javaTime = TimeStamp(time);
    // Condition array is iterated backwards so we can
    // remove matched elements at the same time
    for (TInt i = iConditions.Count() - 1; i >= 0; i--)
    {
        if (iConditions[ i ]->Evaluate(currentValue, aChannelId))
        {
            matched = ETrue;
            CSensorConditionBase *condition = iConditions[i];
            iConditions.Remove(i);
            if (iSensorListener)
            {
                iSensorListener->ConditionMet(
                    condition,
                    condition->GetChannelId(),
                    currentValue,
                    javaTime);
            }
            delete condition;
        }
    }

    // Also send all values separately if we have java side custom conditions
    if (iJavaConditionEval)
    {
        iSensorListener->ConditionMet(0, aChannelId, currentValue, javaTime);
    }
    return matched;
}

int CSensorBase::AddCondition(
    void** aHandle,
    int aChannelId,
    double aLowerLimit,
    double aUpperLimit,
    int aLowerOp,
    int aUpperOp)
{
    JELOG2(ESensor);
    TRAPD(err, AddConditionL(aHandle,    aChannelId,
                             aLowerLimit, aUpperLimit, aLowerOp, aUpperOp));
    return err;
}

void CSensorBase::AddConditionL(void** aHandle, int aChannelId,
                                double aLowerLimit, double aUpperLimit, int aLowerOp,
                                int aUpperOp)
{
    JELOG2(ESensor);
    CSensorConditionBase* condition;
    if (aUpperOp < 0)
    {
        condition = new(ELeave)CSensorLimitCondition(aChannelId,
                aLowerLimit, aLowerOp);
    }
    else
    {
        condition = new(ELeave)CSensorRangeCondition(aChannelId,
                aLowerLimit, aUpperLimit, aLowerOp, aUpperOp);
    }
    CleanupStack::PushL(condition);
    iConditions.AppendL(condition);
    CleanupStack::Pop();
    *aHandle = condition;
}

int CSensorBase::RemoveCondition(void* aHandle)
{
    JELOG2(ESensor);
    TMethod method(TMethod::ERemoveCondition);
    method.SetConditionToRemove(aHandle);
    AsyncCallback(method);
    return KErrNone;
}

void CSensorBase::RemoveConditionL(
    void* aHandle)
{
    JELOG2(ESensor);
    CSensorConditionBase* condition = (CSensorConditionBase*) aHandle;
    TInt index = iConditions.Find(condition);
    if (index < KErrNone)
    {
        return;
    }
    iConditions.Remove(index);
    delete condition;
}


