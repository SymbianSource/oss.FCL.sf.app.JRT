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


#ifndef PUSHTIMERCONTAINER_H
#define PUSHTIMERCONTAINER_H

#include "pushdbhandler.h"
#include "timerserverinterface.h"

namespace java
{
namespace util
{
class Uid;
}

namespace push
{

/**
 * This interface provides callback operation which informs expiration of the timer
 * for automatic launch of the application.
 */
class PushTimerListener
{
public:

    virtual ~PushTimerListener() {}

    /**
     * This callback operation is called if alarm expired for application's
     * automatic launch.
     * @param aUid  UID of the application.
     * @param aAlarmTime alarm time(= epoch time) of the MIDlet in milliseconds.
     */
    virtual void timerExpired(const java::util::Uid& aUid,
                              const long long& aAlarmTime) = 0;

    /**
     * This operation deletes alarm of the application from the db.
     * This operation does not thrown any exception.
     * @param aUid  UID of the application.
     */
    virtual void deleteAlarm(const java::util::Uid& aUid) = 0;
};

class TimerData
{
public:

    TimerData(const java::util::Uid& aUid,
              const java::captain::JavaTime& aTime,
              int aTimerId);

    ~TimerData();

    TimerData(const TimerData&);

    TimerData &operator=(const TimerData&);

    java::util::Uid mUid;
    java::captain::JavaTime mTime;
    int mTimerId;
};

/**
 * This interface provides functionality for setting timer for automatic launch
 * of the application. This interface is needed because timer implementation is
 * different depending on OS system.
 */
class PushTimerContainer : public java::captain::TimerServerEventsInterface
{
public:

    PushTimerContainer(java::captain::TimerServerInterface& aTimerServer,
                       PushTimerListener& aListener);

    virtual ~PushTimerContainer();

    /**
     * @param aListener Ownership of this argument is not given to the user.
     */
    //void setListener(PushTimerListener* aListener);

    /**
     * This operation provides alarm information.
     * @param aList List of alarm times. Note: Implementation must not
     *              delete content of the list.
     */
    void setAlarmTimers(std::list<DbAlarmData>& aList);

    /**
     * Returns alarm time by UID of the application.
     * @param aUid UID of the application.
     * @return alarm Time by UID of the application. 0 is returned if alarm time
     *               by UId is not found.
     */
    long long getAlarmTime(const java::util::Uid& aUid);

    /**
     * This operation cancels pending timer of the application.
     * @param aUid UID of the application.
     * @throws This operation does not throw exception.
     */
    void cancelAlarm(const java::util::Uid& aUid);

    /**
     * @return current time in milliseconds
     *         since 00:00:00 UTC (Coordinated Universal Time), January 1 1970.
     */
    long long getCurrentTime();

    java::captain::TimerServerInterface& getTimerServerInterface() const;

    //TimerServerEventsInterface
    virtual void timerTimeout(const int& aTimerId);

private:

    //Datamembers.
    java::captain::TimerServerInterface& mTimerServer;
    PushTimerListener& mListener;
    typedef std::list<TimerData> TimerList_t;
    typedef std::list<TimerData>::iterator TimerListIter_t;
    TimerList_t mTimerList;

    //Not implemented.
    PushTimerContainer(const PushTimerContainer &x);
    PushTimerContainer &operator=(const PushTimerContainer &x);

};

} //end namespace push
} //end namespace java

#endif // PUSHTIMERCONTAINER_H
