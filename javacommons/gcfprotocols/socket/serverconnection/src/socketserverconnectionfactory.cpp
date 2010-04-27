/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include "logger.h"
#include "socketserverconnection.h"
#include "socketserverconnectionfactory.h"
#include "serverconnectionfactorybase.h"
#include "socketserverconnection.h"


#if defined(__SYMBIAN32__) && defined(__WINSCW__)
#include <pls.h>
#else
static SocketServerConnectionFactory* sSocSrvConnFac = 0;
#endif


/*
* returns a pointer of SocketServerConnectionFactory to the push controller
* so that it can invoke this class when a incoming push event arrives
* for this connection
*/

#ifdef __SYMBIAN32__
ServerConnectionFactory& getServerConnectionFactory()
{
#else
extern "C" ServerConnectionFactory& getServerConnectionFactory()
{
#endif
    //CONTEXT_REMOVAL
#if defined(__SYMBIAN32__) && defined(__WINSCW__)
    TUid uid = TUid::Uid(0xE0000079);
    SocketServerConnectionFactory* socSrvConnFac = Pls<SocketServerConnectionFactory>(uid);
    return *socSrvConnFac;
#else
    if (sSocSrvConnFac == 0)
    {
        sSocSrvConnFac = new SocketServerConnectionFactory();
    }
    return *sSocSrvConnFac;
#endif
}

OS_EXPORT SocketServerConnectionFactory& SocketServerConnectionFactory::getFactory()
{
    JELOG2(ESOCKET);
    LOG(ESOCKET,EInfo,"+SocketServerConnectionFactory::getFactory() ");
    ServerConnectionFactory& scf = ::getServerConnectionFactory();
    return reinterpret_cast<SocketServerConnectionFactory&>(scf);
}

/**
 * Construcot
 */
OS_EXPORT SocketServerConnectionFactory::SocketServerConnectionFactory()
{
    JELOG2(ESOCKET);
}

/**
 * Destrucor
 */
OS_EXPORT SocketServerConnectionFactory::~SocketServerConnectionFactory()
{
    JELOG2(ESOCKET);
}

/**
 * This operation creates protocol specific ServerConnection object
 * @param aUri    ServerConnection URI.
 * @param aFilter Connection URL indicating which senders are allowed to cause
 * the MIDlet to be launched.
 * Value of this argument is empty string in the normal server connection case.
 */
OS_EXPORT ServerConnection* SocketServerConnectionFactory::createSrvConnObj(const std::wstring& aUri, const std::wstring& aFilter)
{
    return new SocketServerConnection(aUri,aFilter);
}

/*
* This method indicates whether multiple Connector.open() operation call is allowed
* for a connection.
*/
OS_EXPORT bool SocketServerConnectionFactory::isMultipleSrvConnAllowed()
{
    return false;
}

/* overriding create() method, to handle special case of allowing more than 1 connection socket://: type of url
 called from midlet */

OS_EXPORT ServerConnection* SocketServerConnectionFactory::create(const std::wstring& aUri)
{
    JELOG2(ESOCKET);
    LOG(ESOCKET,EInfo,"SocketServerConnectionFactory::create()");

    // ScopedLock lockObj(mMutex);
    if (aUri.length() == 10) // "socket://:" dynamic port url first time return push object
    {
        LOG(ESOCKET,EInfo,"SocketServerConnectionFactory::create() - dynamic url");
        SrvConnContainerIter_t iter = mServerConnections.find(aUri);
        if (iter == mServerConnections.end())
        {
            //      std::auto_ptr<ServerConnection> newSrvConn(createSrvConnObj(aUri,L""));
            //      SrvConnContainerData newDataObj(newSrvConn.release(),0,false,SrvConnContainerData::NORMAL);
            //      mServerConnections.insert(std::pair<std::wstring,SrvConnContainerData>(aUri,newDataObj));
            //      SocketServerConnection * dsc = (SocketServerConnection*)newDataObj.getConn();
            //      dsc->setPushEnabled();
            //      return dsc;
            LOG(ESOCKET,EInfo,"SocketServerConnectionFactory::create() - dynamic url, normal server connection");
            SocketServerConnection * socketserver = new SocketServerConnection();
            return reinterpret_cast<ServerConnection *>(socketserver);

        }
        else
        {
            if (SrvConnContainerData::PUSH == iter->second.connType())
            {
                LOG(ESOCKET,EInfo,"SocketServerConnectionFactory::create() - dynamic url, this url is already registered for push");
                iter->second.setConnType(SrvConnContainerData::PUSH_LISTEN_BY_MIDLET);
                return reinterpret_cast<ServerConnection *>(iter->second.getConn());
            }
            else
            {
                LOG(ESOCKET,EInfo,"SocketServerConnectionFactory::create() - dynamic url, not registered with push db, so return normal server connection");
                SocketServerConnection * socketserver = new SocketServerConnection();
                return reinterpret_cast<ServerConnection *>(socketserver);
            }
        }
    }
    else
    {
        return ServerConnectionFactoryBase::create(aUri);
    }
}
