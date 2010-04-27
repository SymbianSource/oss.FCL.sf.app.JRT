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

#ifndef SERVERCONNECTIONFACTORYBASE_H
#define SERVERCONNECTIONFACTORYBASE_H

#include <map>
#include "serverconnectionfactory.h"
#include "serverconnection.h"
#include "scopedlocks.h"

namespace java
{
namespace push
{

class PendingConnectionListener;

/**
 * This class is internal helper class of ServerConnectionFactoryBase class.
 * This class contains data of one ServerConnection object.
 */
class SrvConnContainerData
{
public:

    enum ServerConnectionType_t
    {
        PUSH, //Push server connection which is listen by Push Controller
        NORMAL, //non-push server connection
        PUSH_LISTEN_BY_MIDLET //Push server connection listen by MIDlet.
    };

    /**
     * @param aConn This class does not take ownership of this argument.
     */
    SrvConnContainerData(ServerConnection* aConn,PendingConnectionListener* aListener,
                         bool aIsActive,ServerConnectionType_t aType)
            :mSrvConn(aConn),mPendingConnListener(aListener),mIsActive(aIsActive),mConnType(aType)
    {
    }

    virtual ~SrvConnContainerData()
    {
    }

    ServerConnection* getConn() const
    {
        return mSrvConn;
    }

    void setPendingConnListener(PendingConnectionListener* aListener)
    {
        mPendingConnListener = aListener;
    }

    PendingConnectionListener* getPendingConnListener() const
    {
        return mPendingConnListener;
    }

    void setActive(bool aIsActive)
    {
        mIsActive = aIsActive;
    }
    bool isActive() const
    {
        return mIsActive;
    }

    void setConnType(ServerConnectionType_t aConnType)
    {
        mConnType = aConnType;
    }
    ServerConnectionType_t connType() const
    {
        return mConnType;
    }

    SrvConnContainerData(const SrvConnContainerData &x)
    {
        *this = x;
    }

    SrvConnContainerData &operator=(const SrvConnContainerData &x)
    {
        mSrvConn = x.mSrvConn;
        mPendingConnListener = x.mPendingConnListener;
        mIsActive = x.mIsActive;
        mConnType = x.mConnType;
        return *this;
    }

private:

    //Datamembers.
    //This class does not take ownership of this datamember.
    ServerConnection* mSrvConn;
    PendingConnectionListener* mPendingConnListener;
    //Indicates whether incoming message has arrived to the connection.
    bool mIsActive;
    ServerConnectionType_t mConnType;
};

/**
 * This class has "default" implementation of ServerConnectionFactory interface.
 * Probably this implementation is enough for most of protocols. I.e. it is recommened
 * to use this implementation if protocol does not have special reguirements for
 * implementation of ServerConnectionFactory.
 */
class ServerConnectionFactoryBase : public ServerConnectionFactory
{

public:

    OS_IMPORT ServerConnectionFactoryBase();

    OS_IMPORT virtual ~ServerConnectionFactoryBase();

    //Methods of ServerConnectionFactory interface.
    /**
     * See comments from ServerConnectionFactory::createPushServerConn() operation.
     */
    OS_IMPORT virtual ServerConnection& createPushServerConn(const std::wstring& aUri,
            const std::wstring& aFilter,
            ConnectionListener* aListener,
            PendingConnectionListener* aPendingConnListener);

    /**
     * This operation creates ServerConnection if it is not already created.
     * @exception throws RuntimeException if type of the existing ServerConnection
     *            object is PUSH_LISTEN_BY_MIDLET || PUSH and isMultipleSrvConnAllowed()
     *            operation returns 'false'.
     */
    OS_IMPORT virtual ServerConnection* create(const std::wstring& aUri);

    /**
     * Returns all connections which connection type is PUSH_LISTEN_BY_MIDLET || PUSH.
     */
    OS_IMPORT virtual void getPushConnections(bool aAvailable,std::list<std::wstring>& aUriList);

    /**
     * See comments ServerConnectionFactory::closeAllConnections() operation.
     */
    OS_IMPORT virtual void closeAllConnections();

    /**
     * See comments ServerConnectionFactory::deletePushConnection() operation.
     */
    OS_IMPORT virtual void deletePushConnection(const std::wstring& aUri);

    /**
     * See comments ServerConnectionFactory::getPushConnection() operation.
     */
    OS_IMPORT virtual ServerConnection* getPushConnection(const std::wstring& aUri);

    /**
     * See comments ServerConnectionFactory::releaseConnection() operation.
     */
    OS_IMPORT virtual void releaseConnection(const std::wstring& aUri);

    /**
     * Implementation class of the ServerConnection interface must update status whether
     * ServerConnection object has pending message(= message arrived but not yet read).
     * This information is needed for PushRegistry.listConnections() operation. MIDlet can
     * retrieve by this operation active but not handled connections.
     * @param aUri        URI of the connection.
     * @param aPendingMsg Updating logic of pending message flag:
     *                    - Set it 'true' when message arrives to the connection.
     *                    - Set it 'false' when ServerConnection::open() operation has been called.
     */
    OS_IMPORT void setPendingMsgFlag(const std::wstring& aUri,bool aPendingMsg);

    /**
     * This operation creates protocol specific ServerConnection object. Validation of
     * arguments must be done com.nokia.mj.impl.gcf.PushValidator java framework.
     * Note: This operation does not open push connection.
     * @param aUri    ServerConnection URI.
     * @param aFilter Connection URL indicating which senders are allowed to cause
       *                the MIDlet to be launched.
       *                Value of this argument is empty string in the normal server connection case.
       *                Note: START_APP_IN_LISTEN_MODE mode is used in the creation of normal server connection.
       * @throws        PushException with following error codes:
       *                COMMON_SRV_CONN_PLUGIN_ERROR:
       *                reserved for other errors in the creation of ServerConnection object.
     */
    virtual ServerConnection* createSrvConnObj(const std::wstring& aUri,
            const std::wstring& aFilter) = 0;

    /**
     * This method indicates whether multiple Connector.open() operation call is allowed
     * for a connection. Normally this is not allowed but e.g. for some new protocol it might
     * be allowed.
     */
    virtual bool isMultipleSrvConnAllowed() = 0;

protected:

    typedef std::map<std::wstring,SrvConnContainerData> SrvConnContainer_t;
    typedef std::map<std::wstring,SrvConnContainerData>::iterator SrvConnContainerIter_t;

    //Datamembers.
    SrvConnContainer_t mServerConnections;
    java::util::ScopedMutex mMutex;

private:

    //Internal operations.
    bool deletePushConnection(SrvConnContainerIter_t& aIter);

    //Not implemented.
    ServerConnectionFactoryBase(const ServerConnectionFactoryBase &x);
    ServerConnectionFactoryBase &operator=(const ServerConnectionFactoryBase &x);

};

}//end namespace push
}//end namespace java

#endif // SERVERCONNECTIONFACTORYBASE_H
