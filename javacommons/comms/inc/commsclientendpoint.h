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
* Description:  This class implements Comms client endpoint. Client
*                endpoint can be connected to server endpoint and it forwards
*                received messages to listeners.
*
*/

#ifndef COMMSCLIENTENDPOINT_H
#define COMMSCLIENTENDPOINT_H
#include <memory>

#include "transport.h"

#include "commsendpoint.h"

namespace java
{
namespace comms
{
class CommsMessage;

/**
* This class implements Comms client endpoint. Client endpoint can be connected to server
* endpoint and it forwards received messages to listeners.
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
* CommsClientEndpoint is thread safe.
*
* Usage:
* It is recomended that at least default listener is registered before CommsClientEndpoint
* is connected. This ensures that no messages are lost.
* @code
* CommsClientEndpoint comms;
* comms.registerDefaultListener(listener);
* comms.connect(MY_SERVER_NAME);
* ...
* comms.disconnect();
* @endcode
*
* @see CommsListener, CommsServerEndpoint, CommsEndpoint
*/
class CommsClientEndpoint : public CommsEndpoint
{
public:
    /**
    * A default constructor.
    */
    OS_IMPORT CommsClientEndpoint();

    /**
    * Creates a named endpoint. Reference to this endpoint can be acquired via find() method.
    * Named endpoint is useful e.g. in situations where same endpoint needs to be shared between
    * native and java sides.
    * @see find()
    */
    OS_IMPORT CommsClientEndpoint(const std::wstring& aName);

    /**
    * A destructor.
    */
    OS_IMPORT virtual ~CommsClientEndpoint();

    /**
    * Finds a named endpoint.
    * @param[in] aName Endpoint name
    * @see CommsClientEndpoint()
    * @return Endpoint in success, 0 in failure
    */
    OS_IMPORT static CommsClientEndpoint* find(const std::wstring& name);

    /**
    * Connects to a named endpoint.
    * Endpoint needs to be connected before messages can be sent or received.
    * It's recommended that at least one listener is registered before connecting to server.
    * @code
    * CommsClientEndpoint comms;
    * int rc = comms.connect(MY_SERVER_NAME);
    * if(rc!=0)
    * {
    *     cout << "connect failed";
    * }
    * @endcode
    * @param[in] aAddress Server name
    * @see disconnect()
    * @see registerListener()
    * @return 0 in success, errno in failure
    */
    OS_IMPORT virtual int connect(int aAddress);

    /**
    * Disconnects from from server endpoint. Messages can not be sent or received after this call.
    * Applicaton can reconnect again using connect() method.
    * Listeners remain registered after this method call so listeners do not need to re-registered.
    * @code
    * CommsClientEndpoint comms;
    * comms.connect(MY_SERVER_NAME);
    * comms.disconnect();
    * @endcode
    * @param -
    * @see connect()
    * @see registerListener()
    * @return 0 in success, errno in failure
    */
    OS_IMPORT virtual int disconnect();

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
    std::auto_ptr<IpcConnectionInterface> mIpc;
    int mAddress;
    const bool mAddedToContext;
};

} // namespace comms
} // namespace java

#endif // COMMSCLIENTENDPOINT_H
