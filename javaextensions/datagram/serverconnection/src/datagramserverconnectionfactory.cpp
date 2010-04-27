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
#include "datagramserverconnection.h"

#include "datagramserverconnectionfactory.h"

#if defined(__SYMBIAN32__) && defined(__WINSCW__)
#include <pls.h>
#else
static DatagramServerConnectionFactory* sDgSrvConnFac = 0;
#endif

#ifdef __SYMBIAN32__
ServerConnectionFactory& getServerConnectionFactory()
{
#else
extern "C"
ServerConnectionFactory& getServerConnectionFactory()
{
#endif

#if defined(__SYMBIAN32__) && defined(__WINSCW__)
    TUid uid = TUid::Uid(0xE0000080);
    DatagramServerConnectionFactory* dgSrvConnFac = Pls<DatagramServerConnectionFactory>(uid);
    return *dgSrvConnFac;
#else
    if (sDgSrvConnFac == 0)
    {
        sDgSrvConnFac = new DatagramServerConnectionFactory();
    }
    return *sDgSrvConnFac;
#endif
}

OS_EXPORT DatagramServerConnectionFactory& DatagramServerConnectionFactory::getFactory()
{
    JELOG2(ESOCKET);
    ServerConnectionFactory& scf = ::getServerConnectionFactory();
    return reinterpret_cast<DatagramServerConnectionFactory&>(scf);
}

/**
 *
 */
OS_EXPORT DatagramServerConnectionFactory::DatagramServerConnectionFactory()
{
    JELOG2(ESOCKET);
}

/**
 *
 */
OS_EXPORT DatagramServerConnectionFactory::~DatagramServerConnectionFactory()
{
    JELOG2(ESOCKET);
}

/**
 *
 */
OS_EXPORT ServerConnection* DatagramServerConnectionFactory::createSrvConnObj(const std::wstring& aUri,
        const std::wstring& aFilter)
{
    return new DatagramServerConnection(aUri,aFilter);
}

OS_EXPORT bool DatagramServerConnectionFactory::isMultipleSrvConnAllowed()
{
    return false;
}

/* overriding create() method, to handle special case of allowing more than 1 connection datagram://: type of url
 called from midlet */
OS_EXPORT ServerConnection* DatagramServerConnectionFactory::create(const std::wstring& aUri)
{
    JELOG2(ESOCKET);
    LOG(ESOCKET,EInfo,"DatagramServerConnectionFactory::create()");

    // ScopedLock lockObj(mMutex);
    if (aUri.length() == 12) // "datagram://:" dynamic port url first time return push object
    {
        LOG(ESOCKET,EInfo,"DatagramServerConnectionFactory::create() - dynamic url");
        SrvConnContainerIter_t iter = mServerConnections.find(aUri);
        if (iter == mServerConnections.end())
        {
            //      std::auto_ptr<ServerConnection> newSrvConn(createSrvConnObj(aUri,L""));
            //      SrvConnContainerData newDataObj(newSrvConn.release(),0,false,SrvConnContainerData::NORMAL);
            //      mServerConnections.insert(std::pair<std::wstring,SrvConnContainerData>(aUri,newDataObj));
            //      DatagramServerConnection * dsc = (DatagramServerConnection*)newDataObj.getConn();
            //      dsc->setPushEnabled();
            //      return dsc;

            DatagramServerConnection * datagramserver = new DatagramServerConnection();
            return reinterpret_cast<ServerConnection *>(datagramserver);

        }
        else
        {
            if (SrvConnContainerData::PUSH == iter->second.connType())
            {
                LOG(ESOCKET,EInfo,"DatagramServerConnectionFactory::create() - dynamic url, this url is already registered for push");
                iter->second.setConnType(SrvConnContainerData::PUSH_LISTEN_BY_MIDLET);
                return reinterpret_cast<ServerConnection *>(iter->second.getConn());
            }
            else
            {
                LOG(ESOCKET,EInfo,"DatagramServerConnectionFactory::create() - dynamic url, not registered with push db, so return normal server connection");
                DatagramServerConnection * datagramserver = new DatagramServerConnection();
                return reinterpret_cast<ServerConnection *>(datagramserver);
            }
        }
    }
    else
    {
        return ServerConnectionFactoryBase::create(aUri);
    }
}
