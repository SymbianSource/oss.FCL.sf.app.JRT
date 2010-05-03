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
* Description:  DriveObserverServer
*
*/

#ifndef DRIVEOBSERVERSERVER_H
#define DRIVEOBSERVERSERVER_H

#include <list>

#include "javaosheaders.h"
#include "commslistener.h"

#include "driveutilities.h"

namespace java
{
namespace comms
{
class CommsEndpoint;
}
namespace fileutils
{

class DriveChangedEventGenerator;

OS_NONSHARABLE_CLASS(DriveObserverServer) : public DriveObserverServerInterface,
        public DriveListenerInterface,
        public java::comms::CommsListener
{
public:
    DriveObserverServer();
    virtual ~DriveObserverServer();

    // DriveObserverServerInterface
    virtual void startServer(java::comms::CommsEndpoint*);
    virtual void stopServer();

    virtual void registerListener(DriveListenerInterface* aListener);
    virtual void unregisterListener(DriveListenerInterface* aListener);

    // DriveListenerInterface
    virtual void driveChanged(const int& aOperation, const driveInfo& aDriveInfo);

    // CommsListener
    virtual void processMessage(java::comms::CommsMessage& aMessage);

private:
    void registerSubscriber(java::comms::CommsMessage& aMessage);
    void unregisterSubscriber(java::comms::CommsMessage& aMessage);

    void getDrives(java::comms::CommsMessage& aMessage);

    void enableEvents();
    void disableEventsIfNotNeeded();

    typedef std::list<DriveListenerInterface*> listeners;
    typedef std::list<DriveListenerInterface*>::iterator listenersIter;
    listeners iListeners;

    OS_NONSHARABLE_STRUCT(subscriberData)
    {
        subscriberData(int aModuleId, int aSubscriberAddress)
                :iModuleId(aModuleId), iSubscriberAddress(aSubscriberAddress)
        {}

        int iModuleId;
        int iSubscriberAddress;
    };

    typedef std::list<subscriberData*> subscribers;
    typedef std::list<subscriberData*>::iterator subscribersIter;
    subscribers iSubscribers;

    java::comms::CommsEndpoint*  iComms;

    DriveChangedEventGenerator*  iEventGenerator;
};

} // end of namespace fileutils
} // end of namespace java

#endif // DRIVEOBSERVERSERVER_H
