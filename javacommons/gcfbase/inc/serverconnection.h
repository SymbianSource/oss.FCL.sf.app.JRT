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

#ifndef SERVERCONNECTION_H
#define SERVERCONNECTION_H

#include <string>

namespace java
{
namespace push
{

class ConnectionListener;

/**
 * This interface provides functionality open and get/set information related to
 * protocol connection. Each push capable protocol must implement this interface.
 */

class ServerConnection
{
public:

    virtual ~ServerConnection() {}

    /**
     * This operation set listener and opens physical protocol connection. Code execution must not
     * return from this operation before connection is physically opened. I.e. this method opens
     * connection synchronously.
     * Implementation of this operation must prepare situation wherein this operation is called
     * more than once after connection is already established. In this situation implementation of
     * this operation must only update a listener. This happens e.g. when push framework
     * takes listening responsible of the connection from the MIDlet(MIDlet has called
     * Connection.close() for a server connection).
     * @param aListener This operation does not take ownership of this pointer.
     * @throws PushException with COMMON_SRV_CONN_PLUGIN_ERROR error code if
     *         creation of the connection fails.
     */
    virtual void open(ConnectionListener* aListener) = 0;

    /**
     * This operation closes physical protocol connection. Closing a connection must be done
     * synchronously. I.e. code execution must not return from this operation before connection
     * is physically opened.
     * Note: ServerConnection must be closed by ServerConnectionFactory::releaseConnection() operation.
     * This operation does not throw an exception.
     */
    virtual void close() = 0;

    /**
     * @return connection URI.
     */
    virtual std::wstring getUri() const = 0;

    /**
     * @return filter URI of the push connection. Empty string is returned in the normal
     *         ServerConnection case.
     */
    virtual std::wstring getFilter() const = 0;

    /**
     * Sets 'filter' argument for the connection. This method is needed in the situation where
     * 'normal' ServerConnection has been created before dynamic push registration.
     */
    virtual void setFilter(const std::wstring& aFilter) = 0;

};

}//end namespace push
}//end namespace java

#endif // SERVERCONNECTION_H
