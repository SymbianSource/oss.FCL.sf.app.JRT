/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  StorageServer
*
*/

#ifndef STORAGESERVER_H
#define STORAGESERVER_H

#include "commslistener.h"
#include "commsserverendpoint.h"

using java::comms::CommsListener;
using java::comms::CommsMessage;
using java::comms::CommsServerEndpoint;

namespace java
{
namespace util
{
class Monitor;
}

namespace storage
{

class StorageDBHandler;

/**
 * StorageServer serves storage clients.
 * StorageServer servers incoming Comms messages and responds to them. They
 * are handled synchronously. Incoming message is identified and then processed
 * accordingly. Usually call is redirected to underlying database and response
 * contains database response.
 */
class StorageServer : CommsListener
{
public:
    /**
     * Constructor.
     *
     * @param aMonitor for life cycle managenent.
     */
    StorageServer(java::util::Monitor* aMonitor = NULL);
    virtual ~StorageServer();

    /**
     * Start storage server. Comms server is started to allow message
     * receiving.
     */
    void start();

    /**
     * Stop storage server.
     */
    void stop();

    /**
     * Process incoming message and responds to it.
     * Method of CommsListener.
     *
     * @param aMessage incoming message.
     */
    virtual void processMessage(CommsMessage& aMessage);

private:

    /**
     * Send response message.
     *
     * @param aReceivedMessage received message to extract sender information.
     * @param aResponseMessage response message. Receiver information is added.
     */
    void respond(const CommsMessage& aReceivedMessage,
                 CommsMessage& aResponseMessage);

private:
    CommsServerEndpoint     iComms;
    StorageDBHandler*       iDbHandler;
    java::util::Monitor*    iMonitor;
};

}    // java
}    // storage

#endif // STORAGESERVER_H

