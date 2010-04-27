/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "pushtimercontainer.h"
//#include "timerserverinterface.h"
#include "pushtimerutils.h"

using namespace java::push;
using namespace java::captain;
using namespace java::util;

/**
 *
 */
PushTimerContainer::PushTimerContainer(TimerServerInterface& aTimerServer,
                                       PushTimerListener& aListener)
        : mTimerServer(aTimerServer),mListener(aListener)
{
    JELOG2(EJavaPush);
}

/**
 *
 */
PushTimerContainer::~PushTimerContainer()
{
    JELOG2(EJavaPush);
    mTimerList.clear();
}

/**
 *
 */
void PushTimerContainer::setAlarmTimers(std::list<DbAlarmData>& aList)
{
    JELOG2(EJavaPush);

    std::list<DbAlarmData>::iterator iter;
    for (iter = aList.begin(); iter != aList.end(); ++iter)
    {
        try
        {
            JavaTime timeObj(iter->getTime());
            if (iter->validateAlarmTime())
            {
                bool timerExpired = PushTimerUtils::isTimerExpired
                                    (mListener,iter->getUid(),timeObj,mTimerServer);
                if (true == timerExpired)
                    continue;
            }
            int timerId = mTimerServer.timerCreateJavaTime(timeObj,this);
            if (0 == timerId)
            {
                ELOG(EJavaPush,"ERROR!!! Setting a timer failed");
                continue;
            }
            TimerData dataObj(iter->getUid(),timeObj,timerId);
            mTimerList.push_back(dataObj);
        }
        catch (ExceptionBase& ex)
        {
            ELOG1(EJavaPush,
                  "ERROR!!! Exception in PushTimerContainer::setAlarmTimers() operation: %s",
                  ex.toString().c_str());
        }
    }//end for
}

/**
 *
 */
long long PushTimerContainer::getAlarmTime(const java::util::Uid& aUid)
{
    JELOG2(EJavaPush);

    for (TimerListIter_t iter = mTimerList.begin(); iter != mTimerList.end(); ++iter)
    {
        if (iter->mUid == aUid)
            return iter->mTime.getTime();
    }
    return 0LL;
}

/**
 *
 */
void PushTimerContainer::cancelAlarm(const java::util::Uid& aUid)
{
    JELOG2(EJavaPush);

    for (TimerListIter_t iter = mTimerList.begin(); iter != mTimerList.end(); ++iter)
    {
        if (iter->mUid == aUid)
        {
            mTimerServer.timerCancel(iter->mTimerId);
            mTimerList.erase(iter);
            break;
        }
    }//end for
}

/**
 *
 */
void PushTimerContainer::timerTimeout(const int& aTimerId)
{
    JELOG2(EJavaPush);

    for (TimerListIter_t iter = mTimerList.begin(); iter != mTimerList.end(); ++iter)
    {
        if (iter->mTimerId == aTimerId)
        {
            //We are not here interesting about return value of isTimerExpired() operation.
            PushTimerUtils::isTimerExpired(mListener,iter->mUid,iter->mTime,mTimerServer);
            mTimerList.erase(iter);
            break;
        }
    }//end for
}

/**
 *
 */
long long PushTimerContainer::getCurrentTime()
{
    JavaTime currentTime;
    mTimerServer.getCurrentJavaTime(currentTime);
    return currentTime.getTime();
}

/**
 *
 */
TimerServerInterface& PushTimerContainer::getTimerServerInterface() const
{
    return mTimerServer;
}

/**
 * IMPLEMENTATION OF TimerData CLASS.
 */

TimerData::TimerData(const java::util::Uid& aUid,
                     const JavaTime& aTime,
                     int aTimerId)
        :mUid(aUid),mTime(aTime),mTimerId(aTimerId)
{
}

TimerData::~TimerData()
{
}

TimerData::TimerData(const TimerData& aData)
{
    *this = aData;
}

TimerData& TimerData::operator=(const TimerData& aData)
{
    mUid = aData.mUid;
    mTime = aData.mTime;
    mTimerId = aData.mTimerId;
    return *this;
}

