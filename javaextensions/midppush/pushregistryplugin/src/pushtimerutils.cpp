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


#include "pushtimerutils.h"
#include "logger.h"
#include "pushconstant.h"
#include "pushtimercontainer.h"
#include "pusherrorcodes.h"
#include "pushexception.h"
#include "javauid.h"

using namespace java::push;
using namespace java::util;
using namespace java::captain;

/**
 *
 */
bool PushTimerUtils::isTimerExpired(PushTimerListener& aListener,const Uid& aUid,
                                    const JavaTime& aAlarmTime,
                                    TimerServerInterface& aTimerInterface)
{
    JELOG2(EJavaPush);
    JavaTime currentTime;
    aTimerInterface.getCurrentJavaTime(currentTime);

    if (true == isTimeOlderThanTwoWeeks(aTimerInterface,currentTime,aAlarmTime))
    {
        aListener.deleteAlarm(aUid);
        return true;
    }
    else if ((true == aTimerInterface.isLess(aAlarmTime,currentTime))
             || (true == aTimerInterface.isEqual(aAlarmTime,currentTime)))
    {
        aListener.timerExpired(aUid,aAlarmTime.getTime());
        return true;
    }
    return false;
}

/**
 *
 */
bool PushTimerUtils::isTimeOlderThanTwoWeeks(TimerServerInterface& aTimerInterface,
        const JavaTime& aCurrentTime,
        const JavaTime& aAlarmTime)
{
    JELOG2(EJavaPush);

    long long tmpTime = aCurrentTime.getTime() - TWO_WEEKS_IN_MILLI_SECS;
    JavaTime twoWeeksAgoTime(tmpTime);
    if (true == aTimerInterface.isLess(aAlarmTime,twoWeeksAgoTime))
        return true;
    return false;
}

