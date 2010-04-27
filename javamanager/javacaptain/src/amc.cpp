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
* Description:  Amc
*
*/


#include <set>
#include <algorithm>

#include "logger.h"
#include "javacommonutils.h"

#include "commsendpoint.h"
#include "comms.h"
#include "commsmessage.h"

#include "amc.h"

#include "coreinterface.h"
#include "applicationmanagementeventsinterface.h"

namespace java
{
namespace captain
{

Amc::Amc():mCore(0), mAmEventsDispatcher(0), mStopRequest(0)
{
    JELOG2(EJavaCaptain);
}

Amc::~Amc()
{
    JELOG2(EJavaCaptain);
    mCore = 0;
    mAmEventsDispatcher = 0;
    mStopRequest = 0;
}

bool Amc::start(CoreInterface* aCore,
                ApplicationManagementEventsInterface* aAmEventsDispatcher)
{
    JELOG2(EJavaCaptain);
    mCore = aCore;
    mAmEventsDispatcher = aAmEventsDispatcher;
    mCore->getComms()->registerListener(PLUGIN_ID_AMC_C, this);
    return true;
}

bool Amc::stop()
{
    JELOG2(EJavaCaptain);
    if (mCore)
    {
        mCore->getComms()->unregisterListener(PLUGIN_ID_AMC_C, this);
    }
    return true;
}

void Amc::arLaunched(const Uid&, const int&)
{   // Ignored
    JELOG2(EJavaCaptain);
}

void Amc::arTerminated(const Uid& aUID, const int& /*aExitCode*/)
{
    JELOG2(EJavaCaptain);
    // status is hard coded to be ok (=0) on purpose because
    // javainstaller only cares that application has exited
    if (mStopRequest && mStopRequest->updateStatus(aUID, 0))
    {
        delete mStopRequest;
        mStopRequest = 0;
    }
}

// CommsListener methods
void Amc::processMessage(CommsMessage& aMessage)
{
    JELOG2(EJavaCaptain);

    switch (aMessage.getMessageId())
    {
    case AMC_MSG_ID_REQUEST:
    {
        int operation = 0;
        int options = 0;
        getAmcRequestParams(aMessage, operation, options);
        LOG2(EJavaCaptain, EInfo, "Amc::Request %s, %s",
             amcRequestOperation2String(operation),
             amcRequestOptions2String(options));
        switch (operation)
        {
        case AMC_REQUEST_OPERATION_START:
            handleStart(aMessage);
            break;

        case AMC_REQUEST_OPERATION_STOP:
            handleStop(aMessage);
            break;

        case AMC_REQUEST_OPERATION_UPDATE:
            handleUpdate(aMessage);
            break;

        default:
            ELOG1(EJavaCaptain, "Unknown AMC operation %d", operation);
            break;
        }
    }
    break;

    default:
        ELOG1(EJavaCaptain, "Unknown message sent to AMC %d", aMessage.getMessageId());
        break;
    }
}

void Amc::handleStart(CommsMessage& aMessage)
{
    JELOG2(EJavaCaptain);

    int numOfUids = 0;
    aMessage >> numOfUids;

    if (numOfUids > 0)
    {
        Uid uid;
        for (int i = 0 ; i < numOfUids ; i++)
        {
            aMessage >> uid;
            LOG1WSTR(EJavaCaptain, EInfo, "handleStart() uid = %s", uid.toString());
            mCore->getRtc()->launch(uid);
        }
    }
}

void Amc::handleStop(CommsMessage& aMessage)
{
    JELOG2(EJavaCaptain);

    int numOfUids = 0;
    aMessage >> numOfUids;

    if (numOfUids == 0)
    {
        CommsMessage message;
        message.setReceiver(aMessage.getSender());
        message.setModuleId(aMessage.getModuleId());
        message.setMessageRef(aMessage.getMessageRef());
        setAmcResponseParamaters(message, 0);
        mCore->getComms()->send(message);
    }
    else if (numOfUids > 0)
    {
        Uid* uids = new Uid[numOfUids];
        for (int i = 0 ; i < numOfUids ; i++)
        {
            aMessage >> uids[i];
            LOG1WSTR(EJavaCaptain, EInfo, "handleStop uid[] = %s", uids[i].toString());
        }

        StopRequest* request = new StopRequest(mCore,
                                               aMessage.getSender(),
                                               aMessage.getModuleId(),
                                               aMessage.getMessageRef(),
                                               numOfUids, &uids[0]);

        // Permission check should be done for STOP_APPLICATION permission.
        // More strict check is used until javainstaller gets required
        // permission in place
        bool allowed = aMessage.hasPermission(LAUNCH_APPLICATION);
        if (allowed)
        {
            if (request->stopUids())
            {
                delete request;
            }
            else
            {
                mStopRequest = request;
            }
        }
        else
        {
            WLOG(EJavaCaptain, "AMC STOP not allowed");
            request->sendStopNotAllowed(numOfUids, &uids[0]);
            delete request;
        }
        delete [] uids;
    }
}

void Amc::handleUpdate(CommsMessage& aMessage)
{
    JELOG2(EJavaCaptain);

    std::wstring uid;

    uids_t beforeUids;
    uids_t afterUids;

    int numOfUids = 0;
    int i = 0;
    aMessage >> numOfUids;
    for (i = 0 ; i < numOfUids ; i++)
    {
        aMessage >> uid;
        beforeUids.insert(Uid(uid));
        LOG2(EJavaCaptain, EInfo, "before uids[%d] = %s", i, std::string(uid.begin(), uid.end()).c_str());
    }
    aMessage >> numOfUids;
    for (i = 0 ; i < numOfUids ; i++)
    {
        aMessage >> uid;
        afterUids.insert(Uid(uid));
        LOG2(EJavaCaptain, EInfo, "after uids[%d] = %s", i, std::string(uid.begin(), uid.end()).c_str());
    }

    LOG1(EJavaCaptain, EInfo, "beforeUids.size() = %d", beforeUids.size());
    LOG1(EJavaCaptain, EInfo, "afterUids.size() = %d", afterUids.size());

    uids_t resultUids;

    // Enable all uids which were disabled at the beginning of the installation
    std::set_union(afterUids.begin(), afterUids.end(),
                   beforeUids.begin(), beforeUids.end(),
                   std::inserter(resultUids, resultUids.end()));
    if (resultUids.size() > 0)
    {
        for (uids_t::const_iterator iter = resultUids.begin();
                iter != resultUids.end() ; ++iter)
        {
            mCore->getRtc()->enable(*iter);
        }
        resultUids.clear();
    }

    // Added -- exists only in afterUids collection
    std::set_difference(afterUids.begin(), afterUids.end(),
                        beforeUids.begin(), beforeUids.end(),
                        std::inserter(resultUids, resultUids.end()));
    if (resultUids.size() > 0)
    {
        mAmEventsDispatcher->amAdded(resultUids);
        resultUids.clear();
    }

    // Updated -- exists in both collections
    std::set_intersection(beforeUids.begin(), beforeUids.end(),
                          afterUids.begin(), afterUids.end(),
                          std::inserter(resultUids, resultUids.end()));
    if (resultUids.size() > 0)
    {
        mAmEventsDispatcher->amUpdated(resultUids);
        resultUids.clear();
    }

    // Deleted -- exists only in beforeUids collection
    std::set_difference(beforeUids.begin(), beforeUids.end(),
                        afterUids.begin(), afterUids.end(),
                        std::inserter(resultUids, resultUids.end()));
    if (resultUids.size() > 0)
    {
        mAmEventsDispatcher->amDeleted(resultUids);
        resultUids.clear();
    }

}



} // namespace captain
} // namespace java

