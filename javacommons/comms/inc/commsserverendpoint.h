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
* Description:  This class implements Comms server endpoint. Server
*                endpoint waits for incoming connections and it forwards
*                received messages to listeners.
*
*/

#ifndef COMMSSERVERENDPOINT_H
#define COMMSSERVERENDPOINT_H
#include <memory>

#include "transport.h"

#include "commsendpoint.h"

namespace java
{
namespace comms
{

class CommsMessage;

/**
* This class implements Comms server endpoint. Server endpoint waits for incoming
* connections and it forwards received messages to listeners.
*
* Comms endpoint provides interprocess communication functionality. Comms communication
* is point-to-point meaning that client endpoint can be connected to a single server at a time.
* Communication is message based, where users wrap their payload to CommsMessage.
* Comms endpoints support synchronous and asynchronous message sending.
*
* CommsListener provides a way to receive messages asynchronously. Listeners are registered
* to endpoint. When message belonging to listener is received, will Comms endpoint make a
* callback for listener. Application can register as many listeners as need.
*
* Application can register also a default listener, which gets called if there is no registered
* listener for received message.
*
* CommsServerEndpoint is thread safe.
*
* Usage:
* It is recomended that at least default listener is registered before CommsServerEndpoint
* is started. This ensures that no messages are lost.
* @code
* CommsServerEndpoint comms;
* comms.registerDefaultListener(listener);
* comms.start(MY_SERVER_NAME);
* ...
* comms.stop();
* @endcode
*
* @see CommsListener, CommsClientEndpoint, CommsEndpoint
*/
class CommsServerEndpoint : public CommsEndpoint
{
public:
    /**
    * A default constructor.
    */
    OS_IMPORT CommsServerEndpoint();

    /**
    * Creates a named endpoint. Reference to this endpoint can be acquired via find() method.
    * Named endpoint is useful e.g. in situations where same endpoint needs to be shared between
    * native and java sides.
    * @see find()
    */
    OS_IMPORT CommsServerEndpoint(const std::wstring& aName);

    /**
    * A destructor.
    */
    OS_IMPORT virtual ~CommsServerEndpoint();

    /**
    * Finds a named endpoint.
    * @param[in] aName Endpoint name
    * @see CommsClientEndpoint()
    * @return Endpoint in success, 0 in failure
    */
    OS_IMPORT static CommsServerEndpoint* find(const std::wstring& name);

    /**
    * Start to listen for incoming connections.
    * Once server endpoint is started, clients can connect to it using it's 'well-know' name.
    * Multiple clients can be connected to the same server. Server endpoint dispatches received
    * messages to registered listeners. It's recommended that at least one listener is
    * registered before starting endpoint.
    * @code
    * CommsServerEndpoint comms;
    * int rc = comms.start(MY_SERVER_NAME);
    * if(rc!=0)
    * {
    *     cout << "start server failed";
    * }
    * @endcode
    * @param[in] aAddress Server name
    * @see stop()
    * @see registerListener()
    * @return 0 in success, errno in failure
    */
    OS_IMPORT virtual int start(int aAddress);

    /**
    * Stops listening for incoming connecections.
    * Messages can not be sent or received after this call and clients are not able to connect
    * to server.
    * Applicaton can start to listen again using start() method.
    * Listeners remain registered after this method call so listeners do not need to re-registered.
    * themselves.
    * @code
    * CommsServerEndpoint comms;
    * comms.start(MY_SERVER_NAME);
    * comms.stop();
    * @endcode
    * @param -
    * @see start()
    * @see registerListener()
    * @return 0 in success, errno in failure
    */
    OS_IMPORT virtual int stop();

    /**
    * Sends a message.
    *
    * See description from CommsEndpoint::send().
    * @param[in] aMessage A message to be sent
    * @see sendReceive()
    * @see registerListener()
    * @return 0 in success, errno in failure
    */
    OS_IMPORT virtual int send(CommsMessage& aMessage);

    OS_IMPORT virtual int detachFromVm();
private:
    std::auto_ptr<IpcServerConnectionInterface> mIpc;
    int mAddress;
    const bool mAddedToContext;
};

} // namespace comms
} // namespace java

#endif // COMMSSERVERENDPOINT_H

