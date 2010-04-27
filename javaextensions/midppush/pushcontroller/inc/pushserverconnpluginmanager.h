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


#ifndef PUSHSERVERCONNPLUGINMANAGER_H
#define PUSHSERVERCONNPLUGINMANAGER_H

#include <memory>
#include <list>
#include <map>
#include <string>
#include <exception>

#include "dynamiclibloader.h"
#include "scopedlocks.h"
#include "serverconnectionfactory.h"
#include "serverconnection.h"
#include "connectionlistener.h"
#include "pushregdata.h"


namespace java
{
namespace push
{

class PushControllerErrorHandlerInterface
{
public:

    virtual ~PushControllerErrorHandlerInterface() {}

    /**
     * This operation sends IPC_PR_ALL_CONNS_FAILED msg
     * to PushRegistryPlugin.
     * @throws This operation does not throw exceptions.
     */
    virtual void sendAllConnsFailedMsg() = 0;

    /**
     * This operation informs Runtime to close itself.
     * @throws This operation does not throw exceptions.
     */
    virtual void closeRuntime() = 0;

};

class ConnManagedInterface
{
public:

    virtual ~ConnManagedInterface() {}

    /**
     * This method sends IPC_PR_CONN_COMPLETED comms message to PushRegistryPlugin
     * when push connection, registered/unregistered by other MIDlet, has been completed.
     * @param aUri URI of the managed connection.
     */
    virtual void connCompleted(const std::wstring& aUri) = 0;

};

/**
 * This class contains Server Connection Plugin data. Reserved only for internal usage.
 */
class ServerConnectionPluginData
{
public:

    /**
     * @param aSrvConnLibAccess This class does not take ownership of this object.
     */
    ServerConnectionPluginData(java::util::DynamicLibLoader* aSrvConnLibAccess,
                               java::push::ServerConnectionFactory& aServerConnFactory)
            : mSrvConnLibAccess(aSrvConnLibAccess),mServerConnFactory(aServerConnFactory)
    {
    }

    virtual ~ServerConnectionPluginData()
    {
    }

    ServerConnectionPluginData(const ServerConnectionPluginData& x)
            : mSrvConnLibAccess(x.mSrvConnLibAccess),mServerConnFactory(x.mServerConnFactory)
    {
    }

    java::util::DynamicLibLoader* mSrvConnLibAccess;
    java::push::ServerConnectionFactory& mServerConnFactory;

private:
    //Not implemented.
    ServerConnectionPluginData &operator=(const ServerConnectionPluginData&);
};

/**
 * This class caches Server Connection Plugins. This class provides functionality to start
 * listening push connections, register and unregister new dynamic push registrations and
 * getting information from Server Connections.
 */
class PushServerConnPluginManager
{
public:

    typedef std::list<PushRegData> UriList_t;
    typedef std::list<PushRegData>::iterator UriListIter_t;

    /**
     * @param aListener This class does not take ownership into this argument.
     * @param aPendingConnListener This class does not take ownership into this argument.
     */
    PushServerConnPluginManager(ConnectionListener* aListener,
                                PendingConnectionListener* aPendingConnListener,
                                ConnManagedInterface* aConnManagedInterface);

    virtual ~PushServerConnPluginManager();

    /**
     * This operation is used to open push connections to listen arriving messages.
     * This operation loads Server Connection Plugins, creates ServerConnection objects and
     * calls ServerConnection.open() operation for each URI.
     * @param aUriList Push connection URIs.
     * @param aThrowException indicates whether this operation throws an exception
     *                        if creation/opening a connection fails.
     * @throws  PushException with following error codes:
     *          INVALID_URI:
     *          Uri passed as argument is not valid.
     *          COMMON_SRV_CONN_PLUGIN_ERROR:
     *          creation of ServerConnection object failed.
     *          INIT_OF_ALL_PUSH_CONNS_FAILED:
                initialization of all operations fails
     *          LibraryLoaderException with following error codes:
     *          OPENING_LIBRARY_FAILED:
     *          if opening a Server Connection Plugin failed.
     *          E.g. in the situation where certain protocol is not supported.
     *          CANNOT_FIND_FUNCTION_FAILURE:
     *          if function function by "getServerConnectionFactory" name is not found
     *          from ServerConnection Plugin.
     */
    void startListen(UriList_t& aUriList,bool aThrowException,bool aCallConnectionHandled);

    /**
     * This operation is used to update push connnections. This operation creates and
     * opens new push connections or closes and deletes/sets to "non-push" existing push connections.
     * @param aUriList Push connection URIs.
     * @param aErrorHandler error handler object.
     * @throws If error occurs in the update this operation send same exceptions as
     *         startListen(UriList_t& aUriList) operation.
     *         In the unregistering case this operation does not throw exceptions.
     */
    void updatePushRegs(UriList_t& aUriList,PushControllerErrorHandlerInterface& aErrorHandler);

    /**
     * This operation populates aUriList output argument with active or all push
     * connection URIs of the MIDlet depending value of the aAvailable argument.
     * @param aAvailable 'true' means that only active push connections are returned.
     *                   Active push connection means connection where msg has been arrived.
     *                   'false' means that all push connections of the MIDlet is returned.
     * @param aUriList Output argument. Push connection URIs are populated into this list.
     * @throws This operation does not throw exceptions.
     */
    void listConnections(bool aAvailable,std::list<std::wstring>& aUriList);

    /**
     * This operation returns push ServerConnection object.
     * @throws PushException with following error codes:
     *         SRV_CONN_NOT_FOUND:
     *         ServerConnectionFactory object by URI is not found or if ServerConnection
     *         by URI is not found.
     *         INVALID_URI:
     *         Scheme cannot be resolved from the URI.
     */
    ServerConnection& getPushConnection(const std::wstring& aUri);

    /**
     * Changed dynamic push registration to normal ServerConnection in the Server Connection Plugin.
     * This operation does not do anything if ServerConnection by URI is not found.
     * @throws  PushException with following error codes:
     *          INVALID_URI:
     *          Scheme cannot be resolved from the URI.
     */
    void unregisterDynamicPushConn(const std::wstring& aUri);

    /**
     * Push ServerConnection can be set to failed by this operation in the case where
     * ServerConnection calls error() operation.
     * @param aUri   Push connection URI.
     * @return int  Returns number of successful push ServerConnections.
     */
    int setConnFailed(const std::wstring& aUri);

    /**
     * Deletes ServerConnection from the ServerConnectionFactory.
     * This can be used e.g. in the error situation to remove already created
     * ServerConnection objects.
     * @param aUri   Push connection URI.
     * @throws This operation does not thrown exception.
     */
    void deletePushServerConn(const std::wstring& aUri);

    /**
     * This operation checks that connection is not accepted by an another
     * application. This operation must be called when push connection is registered
     * dynamically to other MIDlet in the suite.
     * @param aUri   Push connection URI.
     * @return true if URI is valid and push connection can be registered.
     * @throws This operation does not throw exceptions.
     */
    bool isValidPushUri(const std::wstring& aUri);

private:

    //Datamembers.
    typedef std::map<std::wstring,ServerConnectionPluginData> SrvConnContainer_t;
    typedef std::map<std::wstring,ServerConnectionPluginData>::iterator SrvConnContainerIter_t;
    SrvConnContainer_t mSrvConnContainer;

    typedef std::map<std::wstring,bool> SuccessPushConnList_t;
    typedef std::map<std::wstring,bool>::iterator SuccessPushConnListIter_t;
    SuccessPushConnList_t mSuccessPushConnList;

    //This class does not take ownershhip of the object.
    ConnectionListener* mConnListener;
    java::util::ScopedMutex mMutex;
    //This class does not take ownershhip of the object.
    PendingConnectionListener* mPendingConnListener;
    //This class does not take ownershhip of the object.
    ConnManagedInterface* mConnManagedInterface;

    //Internal operations.
    ServerConnectionFactory* getDllFromContainer(const std::wstring& aScheme);
    ServerConnectionFactory& loadAndStoreDll(const std::wstring& aUri,
            std::wstring& aOutScheme);
    void createAndOpenConn(const PushRegData& aData,const std::wstring& aScheme);
    std::wstring resolveScheme(const std::wstring& aUri) const;
    std::string resolveDllName(const std::wstring& aScheme) const;
    int getNumOfSuccessfulPushConns();
    void findAndDeleteUnregPushConns(std::list<std::wstring>& aExistingUriList,
                                     UriList_t& aNewUriListFromStorage,
                                     PushControllerErrorHandlerInterface& aErrorHandler);
    void findAndAddNewPushConns(std::list<std::wstring>& aExistingUriList,
                                UriList_t& aNewUriListFromStorage);

    //Not implemented.
    PushServerConnPluginManager(const PushServerConnPluginManager &x);
    PushServerConnPluginManager &operator=(const PushServerConnPluginManager &x);

};

}//end namespace push
}//end namespace java

#endif // PUSHSERVERCONNPLUGINMANAGER_H
