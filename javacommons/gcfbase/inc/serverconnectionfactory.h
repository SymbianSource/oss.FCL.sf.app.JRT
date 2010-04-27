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


#ifndef SERVERCONNECTIONFACTORY_H
#define SERVERCONNECTIONFACTORY_H

#include <memory>
#include <string>
#include <list>
#include "dynamiclibloader.h"

namespace java
{
namespace push
{

class ServerConnection;
class ConnectionListener;
class ServerConnectionFactory;
class PendingConnectionListener;

typedef ServerConnectionFactory& (*GetServerConnectionFactory)();

/**
 * This interface provides functionality to create/get and delete Server Connection.
 * Each Server Connection Plugin must implement this interface.
 * Note: Server Connection Plugin can use default implementation of this interface
 *       if it does not need any special implementation for functionality of this interface.
 *       ServerConnectionFactoryBase class provides default implementation for this interface.
 */

class ServerConnectionFactory
{
public:

    virtual ~ServerConnectionFactory() {}

    /**
     * User should not used this operation. This operation has been reserved for internal
     * usage of push framework.
     * This method creates ServerConnection for push registration. Type of connection is changed to PUSH
     * if normal server connection already exists with same URI.
     * Note: This method does not validate arguments. Validation must be done using PushValidator
     *       java framework before calling this operation.
     * @param aUri    Push connection URI.
     * @param aFilter Connection URI which is used to check whether senders are allowed to cause
     *                the MIDlet to be launched.
     * @param aListener Listener object. Ownership of the listener is not given to the implementation
     *                  of the ServerConnectionFactory interface.
     * @param aPendingConnListener Listener object. Ownership of the listener is not given to the
     *                             implementation of the ServerConnectionFactory interface.
     * @throws  PushException with following error codes:
     *          COMMON_SRV_CONN_PLUGIN_ERROR:
     *          Reserved for other errors in the creation of ServerConnection object.
     *          PUSH_CONNECTION_ALREADY_EXISTS:
     *          Other push connection with same URI already exists.
     *          Note: this does not concern normal server connection.
     */
    virtual ServerConnection& createPushServerConn(const std::wstring& aUri,
            const std::wstring& aFilter,
            ConnectionListener* aListener,
            PendingConnectionListener* aPendingConnListener) = 0;

    /**
     * User must use this operation to get ServerConnection object.
     * This operation creates an normal ServerConnection object or returns push ServerConnection
     * object. Note: ServerConnection object can be retrieved only once by this operation if
     * protocol does not support multiple instances for the same connection.
     * Existing object is returned otherwise.
     * @param aUri    Push connection URI.
     * @throws PushException with following error codes:
     *         PUSH_CONNECTION_ALREADY_EXISTS:
     *         If normal ServerConnection already exists or if ServerConnection object is retrieved
     *         more than once from this operation.
     * @return Ownership of the return value is not given to the user.
     */
    virtual ServerConnection* create(const std::wstring& aUri) = 0;

    /**
     * This operation populates aUriList output argument with active or all push
     * connection URIs of the MIDlet depending value of the aAvailable argument.
     * @param aAvailable 'true' means that only active push connections are returned.
     *                   Active push connection means connection where msg has been arrived.
     *                   'false' means that all push connections of the MIDlet is returned.
     * @param aUriList Output argument. Push connection URIs are populated into this list.
     * @throws This operation does not throw exceptions.
     */
    virtual void getPushConnections(bool aAvailable,std::list<std::wstring>& aUriList) = 0;

    /**
     * This operation calls ServerConnection::close() operation for each connection object.
     * This operation does not throw exceptions.
     */
    virtual void closeAllConnections() = 0;

    /**
     * NOTE: Usage of this operation is reserved only for the push framework.
     * This operation calls close() operation for ServerConnection object, removes
     * object from the container and delete it. This operation does not do anything
     * if ServerConnection object is not found.
     */
    virtual void deletePushConnection(const std::wstring& aUri) = 0;

    /**
     * This operation return push ServerConnection object.
     * @param aUri Push connection URI.
     * @throws This operation does not throw exceptions.
     * @return Push ServerConnection object. Ownership of the returned object is not given to the user.
     *         Returns NULL if ServerConnection is not found by URI or if ServerConnection
     *         is not push connection.
     */
    virtual ServerConnection* getPushConnection(const std::wstring& aUri) = 0;

    /**
     * Releases a server connection depending state of the connection.
     * Nothing is done if ServerConnection object by URI is not found.
     * Push capable protocol MUST use this operation to release ServerConnection object.
     * Logic of this operation:
     * [Server connection is non-push connection]
     *    First this operation calls close() to the ServerConnection object and after that
     *    object is removed from the container and deleted.
     * [Server connection is push connection listened by MIDlet]
     *    First this operation calls close() to the ServerConnection object and after that
     *    it changed type of the ServerConnection to 'push'.
     * @throws This operation does not throw exceptions.
     */
    virtual void releaseConnection(const std::wstring& aUri) = 0;

    /**
     * This operation loads Server Connection plugin and returns ServerConnectionFactory
     * object. Note: ServerConnectionFactory must be singleton object in the process.
     * @param aDllName name of the dll.
     * @param aLoader Output parameter. This object manages loading Server Connection Plugin
     *                dll and creation of ServerConnectionFactory object. Note: Deleting this object
     *                unloads Server Connection Plugin dll (if this dll has been loaded only once).
     *                I.e. this object have to be alive until this plugin is not used.
     * @throws        LibraryLoaderException with following error codes:
     *                OPENING_LIBRARY_FAILED:
     *                opening a Server Connection Plugin failed.
     *                E.g. in the situation where certain protocol is not supported.
     *                CANNOT_FIND_FUNCTION_FAILURE:
     *                Function by "getServerConnectionFactory"
     *                name is not found from ServerConnection Plugin.
     */
    static ServerConnectionFactory&
    getServerConnectionFactory(const std::string& aDllName,
                               std::auto_ptr<java::util::DynamicLibLoader>& aLoader);
};

inline ServerConnectionFactory&
ServerConnectionFactory::getServerConnectionFactory(const std::string& aDllName,
        std::auto_ptr<java::util::DynamicLibLoader>& loader)
{
    //A function pointer to the factory method.
    if (0 == loader.get())
    {
        loader.reset(new java::util::DynamicLibLoader(aDllName.c_str(),false));
    }

    //Cast the function ptr
    GetServerConnectionFactory getServerConnFactory =
        reinterpret_cast<GetServerConnectionFactory>(loader->getFunction("getServerConnectionFactory"));

    return getServerConnFactory();
}

}//end namespace push
}//end namespace java

#endif // SERVERCONNECTIONFACTORY_H
