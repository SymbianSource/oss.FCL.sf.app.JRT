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
* Description:  DriveObserverServer - platform independent
*
*/



#include "logger.h"
#include "comms.h"
#include "commsmessage.h"
#include "commsendpoint.h"

#include "driveobserverserver.h"
#include "driveobservermessages.h"
#include "drivechangedeventgenerator.h"

using java::comms::CommsEndpoint;
using java::comms::CommsMessage;

namespace java
{
namespace fileutils
{

DriveObserverServer::DriveObserverServer()
        :iComms(0), iEventGenerator(0)
{
    JELOG2(EJavaFile);
}

DriveObserverServer::~DriveObserverServer()
{
    JELOG2(EJavaFile);

    // delete event generator
    if (iEventGenerator)
    {
        iEventGenerator->stopEvents();
        delete iEventGenerator;
    }

    // Clear listeners and subscribers collections
    iListeners.clear();
    iSubscribers.clear();
}

void DriveObserverServer::startServer(CommsEndpoint* aComms)
{
    JELOG2(EJavaFile);

    iComms = aComms;

    // Register moduleId
    if (iComms)
    {
        iComms->registerListener(java::comms::PLUGIN_ID_DRIVE_OBSERVER_NATIVE_C, this);
    }
}

void DriveObserverServer::stopServer()
{
    JELOG2(EJavaFile);

    if (iEventGenerator)
    {
        iEventGenerator->stopEvents();
    }

    if (iComms)
    {
        // Unregister moduleId
        iComms->unregisterListener(java::comms::PLUGIN_ID_DRIVE_OBSERVER_NATIVE_C, this);
    }
}


void DriveObserverServer::registerListener(DriveListenerInterface* aListener)
{
    JELOG2(EJavaFile);

    iListeners.push_back(aListener);

    enableEvents();
}

void DriveObserverServer::unregisterListener(DriveListenerInterface* aListener)
{
    JELOG2(EJavaFile);

    for (listenersIter iter = iListeners.begin();
            iter != iListeners.end() ; /* Empty*/)
    {
        if (*iter == aListener)
        {
            iter = iListeners.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
    disableEventsIfNotNeeded();
}

void DriveObserverServer::driveChanged(const int& aOperation, const driveInfo& aDriveInfo)
{
    JELOG2(EJavaFile);

    for (listenersIter iter = iListeners.begin() ;
            iter != iListeners.end() ; ++iter)
    {
        (*iter)->driveChanged(aOperation, aDriveInfo);
    }

    if (iSubscribers.size() > 0)
    {
        CommsMessage msg;

        for (subscribersIter iter = iSubscribers.begin() ;
                iter != iSubscribers.end() ; ++iter)
        {
            setDriveChangedParams(msg, (*iter)->iSubscriberAddress,
                                  (*iter)->iModuleId, aOperation, aDriveInfo);
            int rc = 0;
            LOG2(EJavaFile, EInfo, "Sending driveChanged message to %d/%d",
                 (*iter)->iSubscriberAddress, (*iter)->iModuleId);
            if (0 != (rc = iComms->send(msg)))
            {
                ELOG1(EJavaFile, "Event message sent failed %d", rc);
            }
            msg.reset();
        }
    }
}

void DriveObserverServer::processMessage(CommsMessage& aMessage)
{
    JELOG2(EJavaFile);

    switch (aMessage.getMessageId())
    {
    case DRIVEOBSERVER_MSG_SUBSCRIBE_EVENTS:
        registerSubscriber(aMessage);
        break;

    case DRIVEOBSERVER_MSG_UNSUBSCRIBE_EVENTS:
        unregisterSubscriber(aMessage);
        break;

    case DRIVEOBSERVER_MSG_GETDRIVES_REQUEST:
        getDrives(aMessage);
        break;

    default:
        ELOG1(EJavaFile, "Unknown message received %d", aMessage.getMessageId());
    }
}

void DriveObserverServer::registerSubscriber(CommsMessage& aMessage)
{
    JELOG2(EJavaFile);

    int subsModuleId = 0;
    getSubscribeParams(aMessage, subsModuleId);
    iSubscribers.push_back(new subscriberData(subsModuleId,
                           aMessage.getSender()));
    enableEvents();
}

void DriveObserverServer::unregisterSubscriber(CommsMessage& aMessage)
{
    JELOG2(EJavaFile);

    for (subscribersIter iter = iSubscribers.begin() ;
            iter != iSubscribers.end() ; /* empty*/)
    {
        if ((*iter)->iSubscriberAddress == aMessage.getSender() &&
                (*iter)->iModuleId          == aMessage.getModuleId())
        {
            iter = iSubscribers.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
    disableEventsIfNotNeeded();
}

void DriveObserverServer::getDrives(CommsMessage& aMessage)
{
    JELOG2(EJavaFile);

    int driveTypes = 0;
    getGetDrivesParams(aMessage, driveTypes);

    driveInfos drives;
    CommsMessage replyMsg;
    replyMsg.replyTo(aMessage);

    switch (driveTypes)
    {
    case DRIVEOBSERER_GET_DRIVES_ALL:
        DriveUtilities::getAllDrives(drives);
        break;
    case DRIVEOBSERER_GET_DRIVES_ACCESIBLE:
        DriveUtilities::getAccesibleDrives(drives);
        break;
    }

    setGetDrivesResultParams(replyMsg, drives);
    iComms->send(replyMsg);
}

void DriveObserverServer::enableEvents()
{
    JELOG2(EJavaFile);

    if (!iEventGenerator)
    {
        // Create platform specific event generator
        iEventGenerator = new DriveChangedEventGenerator(this);
        iEventGenerator->startEvents();
    }
}

void DriveObserverServer::disableEventsIfNotNeeded()
{
    JELOG2(EJavaFile);

    if (0 == iSubscribers.size() &&
            0 == iListeners.size() &&
            iEventGenerator)
    {
        iEventGenerator->stopEvents();
        delete iEventGenerator;
        iEventGenerator = 0;
    }
}

} // end of namespace fileutils
} // end of namespace java
