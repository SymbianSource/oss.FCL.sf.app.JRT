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
* Description:  TickerProviderInterface
*
*/

#ifndef TICKERPROVIDERINTERFACE_H
#define TICKERPROVIDERINTERFACE_H

#include <time.h>
#include <sys/time.h>

namespace java
{
namespace captain
{

class TickerProviderEventsInterface
{
public:
    virtual void tick() = 0;
};

class TickerProviderInterface
{
    TickerProviderInterface(); // Cannot be used
public:
    TickerProviderInterface(TickerProviderEventsInterface* aEvents)
            :mEvents(aEvents)
    {}
    virtual ~TickerProviderInterface()
    {}

    /**
    * Starts the ticker.
    * @param[in] aPeriod - delay in seconds
    * @return -
    */
    virtual void nextTickAt(const long long& aJavaTime) = 0;
    /**
    * Returns current tick time if set.
    * @param[in]
    * @return - 0 if not set otherwise the set time
    */
    virtual long long getNextTickAt() = 0;
    /**
    * Stops the ticker.
    * @param -
    * @return -
    */
    virtual void cancel() = 0;

    // Helpers
    /**
    * Returns milliseconds from EPOCH.
    * @param[in]  -
    * @return - milliseconds from EPOCH in success, -1 if fails
    */
    virtual long long getCurrentJavaTime()
    {
        timeval tim;
        int err = gettimeofday(&tim, NULL);
        if (-1 == err)
        {
            return -1LL;
        }

        return (tim.tv_sec * 1000LL) +
               (tim.tv_usec / 1000LL);
    }

protected:
    TickerProviderEventsInterface*  mEvents;
};

} // namespace captain
} // namespace java

#endif // TICKERPROVIDERINTERFACE_H
