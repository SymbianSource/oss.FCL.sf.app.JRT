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


#ifndef PUSHALARMHANDLER_H
#define PUSHALARMHANDLER_H

#include <map>
#include <memory>
#include "pushtimercontainer.h"
#include "pushalarmutilsinterface.h"

namespace java
{
namespace captain
{
class CoreInterface;
}
namespace util
{
class Uid;
}
namespace push
{

class PushDBHandler;
class PushAlarmHandler;

/**
 * Alarm for automatic MIDlet launch can be set by this class.
 * I.e. this class implements functionality of PushRegistry.registerAlarm() java class.
 */

class PushAlarmHandler : public PushTimerListener
{
public:

    PushAlarmHandler(java::captain::CoreInterface& aCore,
                     PushAlarmUtilsInterface& aLaunchInstance,
                     PushDBHandler& aDbHandler);

    virtual ~PushAlarmHandler();

    /**
     * Reads all alarms from the db and starts timers.
     * @throws This operation does not throw exceptions.
     */
    void readAndStartAlarms();

    /**
     * Reads and starts alarm of the application match to the Uid.
     * @param aUid UID of the application.
     * @throws This operation does not throw exceptions.
     */
    void readAndStartAlarms(const java::util::Uid& aUid);

    /**
     * Sets alarm to the application.
     * @param aUid                    UID of the application.
     * @param aAlarmTimeInMilliSecs   Time in milliseconds since January 1, 1970, 00:00:00 GMT.
     * @param aIsUidOfThisMidlet      indicates whether alarm is set to the current MIDlet.
     *                                i.e. whether alarm is set to the MIDlet where this operation
     *                                is called.
     * @return  0 is alarm was set successfully. Value of the alarm time if alarm is pending for a MIDlet.
     */
    long long setAlarm(const java::util::Uid& aUid,const long long& aAlarmTimeInMilliSecs,
                       bool aIsUidOfThisMidlet);

    /**
     * Cancels timer of the alarm.
     * @param aUid UID of the application.
     */
    void cancelAlarm(const java::util::Uid& aUid);

    /**
     * This method launches MIDlets which was not able to launch when alarm
     * expired due to missing drive.
     * This method does not launch MIDlet if expiration time is older
     * than two week.
     * @param aMediaId Media id of the added drive.
     * @throws This operation does not throw exceptions.
     */
    void handleDriveWasMissingMidlets(const unsigned int aMediaId);

    //Method of PushTimerListener interface.
    virtual void timerExpired(const java::util::Uid& aUid,const long long& aAlarmTime);
    virtual void deleteAlarm(const java::util::Uid& aUid);

private:

    //Uid is MIdlet's uid.
    typedef std::map<java::util::Uid,long long> uidAlarmTimeContainer_t;
    typedef std::map<java::util::Uid,long long>::iterator uidAlarmTimeContainerIter_t;

    //Datamembers.
    java::captain::CoreInterface& mCore;
    PushDBHandler& mPushDbHandler;
    std::auto_ptr<PushTimerContainer> mPushTimer;
    PushAlarmUtilsInterface& mUtilsInstance;
    uidAlarmTimeContainer_t mNotLaunchedExpiredAlarms;

    //Internal operations.
    void handleLessOrEqualCurrentTimeOfOtherMidlet
    (const long long& aCurTimeInMilliSecs,const long long& aAlarmTimeInMilliSecs,
     const java::util::Uid& aUid);
    void timerExpired(const java::util::Uid& aUid,const long long& aAlarmTime,
                      bool aDeleteAlarmFromDb);

    //Not implemented.
    PushAlarmHandler(const PushAlarmHandler &x);
    PushAlarmHandler &operator=(const PushAlarmHandler &x);
};

} //end namespace push
} //end namespace java

#endif // PUSHALARMHANDLER_H

