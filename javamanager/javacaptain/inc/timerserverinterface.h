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
* Description:  TimerServerInterface
*
*/

#ifndef TIMERSERVERINTERFACE_H
#define TIMERSERVERINTERFACE_H

namespace java
{
namespace captain
{

class TimerServerEventsInterface
{
public:
    virtual ~TimerServerEventsInterface() {}

    virtual void timerTimeout(const int& aTimerId) = 0;
};

class JavaTime
{
public:
    JavaTime():mMillisFromEpoch(0LL) {}
    JavaTime(const long long& aTime):mMillisFromEpoch(aTime) {}
    JavaTime(const JavaTime& aJt):mMillisFromEpoch(aJt.getTime()) {}
    JavaTime& operator= (const JavaTime& aJt)
    {
        mMillisFromEpoch = aJt.getTime();
        return *this;
    }
    const long long& operator+ (const long long& a)
    {
        return (this->mMillisFromEpoch += a);
    }
    virtual ~JavaTime() {}

    virtual long long getTime() const
    {
        return mMillisFromEpoch;
    }
    virtual void setTime(const long long& aNewTime)
    {
        mMillisFromEpoch = aNewTime;
    }

private:
    long long mMillisFromEpoch;
};

class TimerServerInterface
{
public:
    virtual ~TimerServerInterface() {}

    /**
    * Creates a new timer.
    * @param[in] aTimeoutInSeconds
    * @param[in] aTimerEvents - Callback interface when timer expires
    * @return >0 TimerId in success, 0 in failure
    */
    virtual int timerCreateSeconds(const unsigned int& aTimeoutInSeconds,
                                   TimerServerEventsInterface* aTimerEvents) = 0;

    /**
    * Creates a new timer.
    * @param[in] aJavaTime - time when timer should expire in java format
    * @param[in] aTimerEvents - Callback interface when timer expires
    * @return >0 TimerId in success, 0 in failure
    */
    virtual int timerCreateJavaTime(const JavaTime& aJavaTime,
                                    TimerServerEventsInterface* aTimerEvents) = 0;

    /**
    * Cancels a timer.
    * @param[in] aTimerId
    * @return -
    */
    virtual void timerCancel(const int& aTimerId) = 0;

    virtual JavaTime& getCurrentJavaTime(JavaTime&) const = 0;

    virtual bool hasExpired(const JavaTime&, const long long& aAccuracyInMs = 100) const = 0;

    virtual bool isLess(const JavaTime& a, const JavaTime& b) const = 0;
    virtual bool isEqual(const JavaTime& a, const JavaTime& b) const = 0;
    virtual bool isMore(const JavaTime& a, const JavaTime& b) const = 0;
};

} // namespace captain
} // namespace java

#endif // TIMERSERVERINTERFACE_H
