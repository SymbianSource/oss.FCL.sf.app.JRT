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

#include "pushconncompletedtimercontainer.h"
#include "pushconstant.h"
#include "logger.h"

using namespace java::push;
using namespace java::captain;
using namespace java::comms;

/**
 *
 */
PushConnCompletedTimer::PushConnCompletedTimer(int aTimerId,const std::wstring& aUri,
        const java::comms::CommsMessage& aCommsMsg)
        : mTimerId(aTimerId),mUri(aUri),mCommsMsg(aCommsMsg)
{
}

/**
 *
 */
PushConnCompletedTimer::~PushConnCompletedTimer()
{
}

/**
 *
 */
PushConnCompletedTimer::PushConnCompletedTimer(const PushConnCompletedTimer& aData)
{
    *this = aData;
}

/**
 *
 */
PushConnCompletedTimer& PushConnCompletedTimer::operator=(const PushConnCompletedTimer& x)
{
    mTimerId = x.mTimerId;
    mUri = x.mUri;
    mCommsMsg = x.mCommsMsg;
    return *this;
}

/**
 *
 */
PushConnCompletedTimerContainer::PushConnCompletedTimerContainer(TimerServerInterface& aTimerServer,
        PushConnCompletedTimerListener& aListener)
        : mTimerServer(aTimerServer),mListener(aListener)
{
    JELOG2(EJavaPush);
}

/**
 *
 */
PushConnCompletedTimerContainer::~PushConnCompletedTimerContainer()
{
    JELOG2(EJavaPush);

    //It is enough to clear content of timer list container without cancelling pending
    //timers because this object is deleted only when JavaCaptain process is closed.
    mTimerList.clear();
}

/**
 *
 */
void PushConnCompletedTimerContainer::setTimer(const std::wstring& aUri,const CommsMessage& aMsg)
{
    JELOG2(EJavaPush);

    int timerId = mTimerServer.timerCreateSeconds(TIMEOUT_VALUE_FOR_UNREG_RESPONSE,this);
    if (0 == timerId)
    {
        ELOG(EJavaPush,"ERROR!!! PushConnCompletedTimerContainer::setTimer() creation of the timer failed");
        return;
    }
    PushConnCompletedTimer dataObj(timerId,aUri,aMsg);
    mTimerList.push_back(dataObj);
}

/**
 *
 */
void PushConnCompletedTimerContainer::cancelTimer(const std::wstring& aUri)
{
    JELOG2(EJavaPush);

    for (TimerListIter_t iter = mTimerList.begin(); iter != mTimerList.end(); ++iter)
    {
        if (iter->mUri == aUri)
        {
            mTimerServer.timerCancel(iter->mTimerId);
            mListener.connCompletedTimerExpired(iter->mCommsMsg);
            mTimerList.erase(iter);
            break;
        }
    }//end for
}

/**
 *
 */
void PushConnCompletedTimerContainer::timerTimeout(const int& aTimerId)
{
    JELOG2(EJavaPush);

    ELOG(EJavaPush,
         "ERROR!!! PushConnCompletedTimerContainer::timerTimeout Timeout occurs");

    for (TimerListIter_t iter = mTimerList.begin(); iter != mTimerList.end(); ++iter)
    {
        if (iter->mTimerId == aTimerId)
        {
            mListener.connCompletedTimerExpired(iter->mCommsMsg);
            mTimerList.erase(iter);
            break;
        }
    }//end for
}





