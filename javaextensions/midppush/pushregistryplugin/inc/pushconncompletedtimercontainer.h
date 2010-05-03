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


#ifndef PUSHCONNCOMPLETEDTIMERCONTAINER_H
#define PUSHCONNCOMPLETEDTIMERCONTAINER_H

#include <list>
#include <string>
#include "commsmessage.h"
#include "timerserverinterface.h"

namespace java
{
namespace push
{

class PushConnCompletedTimerListener
{
public:

    virtual ~PushConnCompletedTimerListener() {}

    virtual void connCompletedTimerExpired(const java::comms::CommsMessage& aMsg) = 0;

};

class PushConnCompletedTimer
{
public:

    PushConnCompletedTimer(int aTimerId,const std::wstring& aUri,
                           const java::comms::CommsMessage& aCommsMsg);

    virtual ~PushConnCompletedTimer();

    PushConnCompletedTimer(const PushConnCompletedTimer&);

    PushConnCompletedTimer& operator=(const PushConnCompletedTimer&);

    //Datamembers.
    int mTimerId;
    std::wstring mUri;
    java::comms::CommsMessage mCommsMsg;

};

class PushConnCompletedTimerContainer : public java::captain::TimerServerEventsInterface
{
public:

    PushConnCompletedTimerContainer(java::captain::TimerServerInterface& aTimerServer,
                                    PushConnCompletedTimerListener& aListener);

    virtual ~PushConnCompletedTimerContainer();

    /**
     * Sets timeout timer for IPC_PR_UNREG_COMPLETED comms message.
     * @param aUri URI of the unregistered push connection.
     * @param aMsg Comms msg of the IPC_PR_UNREGISTER_DYNAMIC_PUSH_CONN.
     * @throws This operation does not throw exceptions.
     */
    void setTimer(const std::wstring& aUri,const java::comms::CommsMessage& aMsg);

    /**
     * Cancels timer. Agter cancellation of the timer this operation
     * calls unregTimerExpired() operation.
     * @param aUri URI of the unregistered push connection.
     * @throws This operation does not throw exceptions.
     */
    void cancelTimer(const std::wstring& aUri);

    //TimerServerEventsInterface
    virtual void timerTimeout(const int& aTimerId);

private:

    //Datamembers.
    java::captain::TimerServerInterface& mTimerServer;
    PushConnCompletedTimerListener& mListener;
    typedef std::list<PushConnCompletedTimer> TimerList_t;
    typedef std::list<PushConnCompletedTimer>::iterator TimerListIter_t;
    TimerList_t mTimerList;

    //Not implemented.
    PushConnCompletedTimerContainer(const PushConnCompletedTimerContainer&);
    PushConnCompletedTimerContainer& operator=(const PushConnCompletedTimerContainer&);

};

}//end namespace push
}//end namespace java

#endif // PUSHCONNCOMPLETEDTIMERCONTAINER_H

