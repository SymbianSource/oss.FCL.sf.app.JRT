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


#include <limits.h>

#include "pushalarmhandler.h"
#include "pushdbhandler.h"
#include "logger.h"
#include "pushexception.h"
#include "pusherrorcodes.h"
#include "coreinterface.h"
#include "rtcinterface.h"
#include "pushtimerutils.h"
#include "pushconstant.h"
#include "javauid.h"
#include "javacommonutils.h"

using namespace java::push;
using namespace java::captain;
using namespace java::comms;
using namespace java::util;
using namespace std;

/**
 *
 */
PushAlarmHandler::PushAlarmHandler(java::captain::CoreInterface& aCore,
                                   PushAlarmUtilsInterface& aLaunchInstance,
                                   PushDBHandler& aDbHandler)
        : mCore(aCore),mPushDbHandler(aDbHandler),mPushTimer(NULL),mUtilsInstance(aLaunchInstance)
{
    JELOG2(EJavaPush);
    mPushTimer.reset(new PushTimerContainer(*aCore.getTimerServer(),*this));
}

/**
 *
 */
PushAlarmHandler::~PushAlarmHandler()
{
    JELOG2(EJavaPush);
    mNotLaunchedExpiredAlarms.clear();
}

/**
 *
 */
void PushAlarmHandler::readAndStartAlarms()
{
    JELOG2(EJavaPush);

    Uid emptyUid;
    readAndStartAlarms(emptyUid);
}

/**
 *
 */
void PushAlarmHandler::readAndStartAlarms(const Uid& aUid)
{
    JELOG2(EJavaPush);
    LOG1WSTR(EJavaPush,EInfo,"readAndStartAlarms(): Uid: %s",aUid.toString());

    try
    {
        std::list<DbAlarmData> alarms;
        mPushDbHandler.getAlarms(aUid,alarms);
        if (0 == alarms.size())
            return;

        mPushTimer->setAlarmTimers(alarms);
        alarms.clear();
    }
    catch (...)
    {
        ELOG(EJavaPush,"ERROR!!! Exception caught in the PushAlarmHandler::readAndStartAlarms()");
    }
}

/**
 *
 */
long long PushAlarmHandler::setAlarm(const Uid& aUid,const long long& aAlarmTimeInMilliSecs,
                                     bool aIsUidOfThisMidlet)
{
    JELOG2(EJavaPush);

    LOG1WSTR(EJavaPush,EInfo, "setAlarm() aUid: %s",aUid.toString());
    long long curTimeInMilliSecs = mPushTimer->getCurrentTime();
    long long curTimeInMilliSecsPlusSomeExtra =
        curTimeInMilliSecs + NO_NEED_TO_INITIALZE_TIMER_TIME_IN_MILLI_SECS;

    long long currentWakeUpTime = mPushTimer->getAlarmTime(aUid);
    //Alarm can be first deleted from the Storage and cancelled.
    if (0LL != currentWakeUpTime)
    {
        cancelAlarm(aUid);
        deleteAlarm(aUid);
    }

    if (aAlarmTimeInMilliSecs < curTimeInMilliSecsPlusSomeExtra)
    {
        LOG1(EJavaPush,EInfo,"aAlarmTimeInMilliSecs %lld",aAlarmTimeInMilliSecs);
        LOG1(EJavaPush,EInfo,"curTimeInMilliSecsPlusSomeExtra %lld",curTimeInMilliSecsPlusSomeExtra);
        //We can return 0 in the case wherein MIDlet registers alarm to itself and alarm is max.
        //'current time + 1'.
        if (false == aIsUidOfThisMidlet)
            handleLessOrEqualCurrentTimeOfOtherMidlet(curTimeInMilliSecs,aAlarmTimeInMilliSecs,aUid);

        //Zero is returned if alarm time has passed.
        if (curTimeInMilliSecs < aAlarmTimeInMilliSecs)
        {
            LOG(EJavaPush,EInfo,"Returns 0");
            return 0LL;
        }

        LOG(EJavaPush,EInfo,"Returns currentWakeUpTime");
        return currentWakeUpTime;
    }

    LOG(EJavaPush,EInfo,"Setting alarm");
    std::list<DbAlarmData> validAlarms;
    DbAlarmData newAlaramDataObj(aUid,aAlarmTimeInMilliSecs,false);
    validAlarms.push_back(newAlaramDataObj);
    mPushTimer->setAlarmTimers(validAlarms);
    validAlarms.clear();

    try
    {
        mPushDbHandler.storeAlarm(aUid,aAlarmTimeInMilliSecs);
    }
    catch (...)
    {
        cancelAlarm(aUid);
        throw;
    }
    return currentWakeUpTime;
}

/**
 *
 */
void PushAlarmHandler::handleLessOrEqualCurrentTimeOfOtherMidlet
(const long long& aCurTimeInMilliSecs,const long long& aAlarmTimeInMilliSecs,
 const Uid& aUid)
{
    JELOG2(EJavaPush);

    JavaTime currentTimeObj(aCurTimeInMilliSecs);
    JavaTime alarmTimeObj(aAlarmTimeInMilliSecs);
    //In this case alarm time is older than two weeks so MIDlet is not launched.
    bool flag = PushTimerUtils::isTimeOlderThanTwoWeeks(mPushTimer->getTimerServerInterface(),
                currentTimeObj,alarmTimeObj);
    if (true == flag)
        return;

    //In this case alarm time is less than 'currentTime + NO_NEED_TO_INITIALZE_TIMER_TIME_IN_MILLI_SECS'
    //or less than two weeks old so MIDlet is launched.
    timerExpired(aUid,aAlarmTimeInMilliSecs,false);
}

/**
 *
 */
void PushAlarmHandler::cancelAlarm(const java::util::Uid& aUid)
{
    JELOG2(EJavaPush);

    mPushTimer->cancelAlarm(aUid);
}

/**
 *
 */
void PushAlarmHandler::handleDriveWasMissingMidlets(const unsigned int aMediaId)
{
    JELOG2(EJavaPush);

    list<Uid> handledUidsList;
    uidAlarmTimeContainerIter_t iter = mNotLaunchedExpiredAlarms.begin();
    for (; iter != mNotLaunchedExpiredAlarms.end(); ++iter)
    {
        int mediaId = mUtilsInstance.getMediaIdByMidletUid(iter->first);
        if (mediaId == aMediaId)
        {
            handledUidsList.push_back(iter->first);
            TimerServerInterface* timerInterface = mCore.getTimerServer();
            JavaTime currentTime;
            timerInterface->getCurrentJavaTime(currentTime);
            JavaTime alarmTime(iter->second);
            bool flag = PushTimerUtils::isTimeOlderThanTwoWeeks(*timerInterface,
                        currentTime,alarmTime);
            if (true == flag)
            {
                deleteAlarm(iter->first);
            }
            else
            {
                //Now we know that alarm is less than current time but not
                //older than two weeks so MIDlet can be launched.
                timerExpired(iter->first,iter->second);
            }
        }//end if(mediaId == aMediaId)
    }//end for

    //Removing handled Uids from mNotLaunchedExpiredAlarms container.
    list<Uid>::iterator iter2 = handledUidsList.begin();
    for (; iter2 != handledUidsList.end(); ++iter2)
    {
        uidAlarmTimeContainerIter_t iter = mNotLaunchedExpiredAlarms.find((*iter2));
        if (iter != mNotLaunchedExpiredAlarms.end())
            mNotLaunchedExpiredAlarms.erase(iter);
    }
    handledUidsList.clear();
}

/**
 *
 */
void PushAlarmHandler::timerExpired(const Uid& aUid,const long long& aAlarmTime)
{
    timerExpired(aUid,aAlarmTime,true);
}

/**
 *
 */
void PushAlarmHandler::timerExpired(const Uid& aUid,const long long& aAlarmTime,
                                    bool aDeleteAlarmFromDb)
{
    JELOG2(EJavaPush);

    LOG1WSTR(EJavaPush,EInfo,"timerExpired(), Uid: %s",aUid.toString());
    bool driveAvailable =
        mUtilsInstance.launchMidlet(aUid,java::captain::RTC_LAUNCH_TYPE_AUTO_INVOCATION_C);
    if ((true == driveAvailable) && (true == aDeleteAlarmFromDb))
    {
        deleteAlarm(aUid);
    }
    else
    {
        mNotLaunchedExpiredAlarms.insert(std::pair<Uid,long long>(aUid,aAlarmTime));
    }
}

/**
 *
 */
void PushAlarmHandler::deleteAlarm(const java::util::Uid& aUid)
{
    JELOG2(EJavaPush);

    //Removing alarm from the db and internal "drive was not available when alarm expired"
    //list if uid exists in this list.
    try
    {
        mPushDbHandler.deleteAlarm(aUid);
    }
    catch (...)
    {
        //Exceptions are ignored in this case.
        ELOG(EJavaPush, "ERROR!!! Unexpected error occurred in the deletion of the alarm from the db");
    }
}


