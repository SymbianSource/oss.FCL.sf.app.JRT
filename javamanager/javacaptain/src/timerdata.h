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
* Description:  TimerData
*
*/

#ifndef TIMERDATA_H
#define TIMERDATA_H

#include "logger.h"

#include "timerserverinterface.h"

namespace java
{
namespace captain
{

class TimerData
{
public:
    TimerData(const JavaTime& aTimeout, TimerServerEventsInterface* aEvents)
            :mTimeout(aTimeout), mEvents(aEvents)
    {
        LOG3(EJavaCaptain, EInfoHeavyLoad, "TimerData(this=%#x, aTimeout=%u, aEvents=%#x)",
             this, aTimeout.getTime(), aEvents);
    }
    virtual ~TimerData()
    {
        LOG1(EJavaCaptain, EInfoHeavyLoad, "~TimerData(this=%#x)", this);
    }

    virtual const JavaTime& getTimeout() const
    {
        return mTimeout;
    }
    virtual void timeoutReached()
    {
        mEvents->timerTimeout((int)this);
    }

private:
    const JavaTime              mTimeout;
    TimerServerEventsInterface* mEvents;
};

} // namespace captain
} // namespace java

#endif // TIMERDATA_H
