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
* Description:
*
*/


#ifndef PUSHTIMERUTILS_H
#define PUSHTIMERUTILS_H

#include "timerserverinterface.h"

namespace java
{
namespace util
{
class Uid;
}

namespace push
{

class PushTimerListener;

class PushTimerUtils
{
public:
    virtual ~PushTimerUtils() {}

    /**
     * Checks whether timer has expired. This operation calls PushTimerListener::deleteAlarm()
     * or PushTimerListener::timerExpired() callback operation depending expiration time.
     * @param aListener PushTimerListener object.
     * @param aUid              UID of the application.
     * @param aAlarmTime Alram time in milliseconds since since 00:00:00 UTC
     *                   (Coordinated Universal Time), January 1 1970.
     * @param aTimerInterface instance of TimerServerInterface
     * @return 'true' if expiration occurred.
     *         'false' if expiration not occurred.
     * @throws PushException with following error code:
     *         INIT_OF_ALARM_TIMER_FAILED:
     *         - Getting current time failed.
     */
    static bool isTimerExpired(PushTimerListener& aListener,const java::util::Uid& aUid,
                               const java::captain::JavaTime& aAlarmTime,
                               java::captain::TimerServerInterface& aTimerInterface);

    static bool isTimeOlderThanTwoWeeks(java::captain::TimerServerInterface& aTimerInterface,
                                        const java::captain::JavaTime& aCurrentTime,
                                        const java::captain::JavaTime& aAlarmTime);

};

} //end namespace push
} //end namespace java

#endif // PUSHTIMERUTILS_H
