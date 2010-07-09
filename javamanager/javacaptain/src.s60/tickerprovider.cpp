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
* Description:  TimerServer
*
*/

#include "logger.h"

#include "tickerprovider.h"

namespace java
{
namespace captain
{

TickerProvider::TickerProvider(TickerProviderEventsInterface* aEvents)
        :CActive(EPriorityStandard), TickerProviderInterface(aEvents), mNextTickAt(0LL)
{
    JELOG2(EJavaCaptain);
    CActiveScheduler::Add(this);
    mRTimer.CreateLocal();
}

TickerProvider::~TickerProvider()
{
    JELOG2(EJavaCaptain);
    Cancel();
    mRTimer.Close();
}

void TickerProvider::nextTickAt(const long long& aJavaTime)
{
    JELOG2(EJavaCaptain);

    if (IsActive())
    {
        LOG(EJavaCaptain, EInfo, "Canceling previous RTimer");
        Cancel();
    }

    mNextTickAt = aJavaTime;

    //In this overflow situation we cannot throw an exception because midp tck expects
    //LLONG_MAX - 1 to be legal alarm value. We can set alarm to maximum value in this case.
    //2060 is maximum date year in S60 device.
    long long overFlowCheckValue = aJavaTime * 1000LL;
    if (overFlowCheckValue < aJavaTime)
    {
        TTime alarmTime(TTime(TDateTime(2060,EJanuary,0,0,0,0,0)).Int64());
        mRTimer.AtUTC(iStatus, alarmTime);
    }
    else
    {
        //TTime alarmTime(TTime(TDateTime(1970,EJanuary,0,0,0,0,0)).Int64() + (aJavaTime * 1000LL));
        TTime alarmTime(TTime(TDateTime(1970,EJanuary,0,0,0,0,0)).Int64() + (overFlowCheckValue));
        mRTimer.AtUTC(iStatus, alarmTime);
    }

    SetActive();
}

long long TickerProvider::getNextTickAt()
{
    JELOG2(EJavaCaptain);
    if (IsActive())
    {
        return mNextTickAt;
    }

    return 0LL;
}


void TickerProvider::cancel()
{
    JELOG2(EJavaCaptain);
    if (IsActive())
    {
        Cancel();
    }
    mNextTickAt = 0LL;
}

long long TickerProvider::getPlatformCurrentJavaTime()
{
    JELOG2(EJavaCaptain);
    TTime currentTime;
    currentTime.UniversalTime();

    // Current time - javaEpoc && uSeconds -> mSeconds
    return (currentTime.Int64()- TTime(TDateTime(1970,EJanuary,0,0,0,0,0)).Int64()) / 1000LL;
}

void TickerProvider::RunL()
{
    JELOG2(EJavaCaptain);

    mNextTickAt = 0LL;
    if (mEvents)
    {
        mEvents->tick();
    }
}

void TickerProvider::DoCancel()
{
    JELOG2(EJavaCaptain);
    mRTimer.Cancel();
}

} // namespace captain
} // namespace java
