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


#include "logger.h"
#include "mmsserverconnection.h"
#include "mmsserverconnectionfactory.h"

using namespace java::util;
using namespace java::wma;

#if defined(__SYMBIAN32__) && defined(__WINSCW__)
#include <pls.h>
#else
static MmsServerConnectionFactory* sMmsSrvConnFac = 0;
#endif

#ifdef __SYMBIAN32__
ServerConnectionFactory& getServerConnectionFactory()
{
#else
extern "C"
ServerConnectionFactory& getServerConnectionFactory()
{
#endif
    JELOG2(EWMA);

    //CONTEXT_REMOVAL
#if defined(__SYMBIAN32__) && defined(__WINSCW__)
    TUid uid = TUid::Uid(0xE0000139);
    MmsServerConnectionFactory* retObj = Pls<MmsServerConnectionFactory>(uid);
    return *retObj;
#else
    if (sMmsSrvConnFac == 0)
    {
        sMmsSrvConnFac = new MmsServerConnectionFactory();
    }
    return *sMmsSrvConnFac;
#endif

    /*
     Context& context = Context::getContext();
     void* ptr = context.getService("mmspushplugin::ServerConnectionFactory");
     if(NULL == ptr)
     {
     MmsServerConnectionFactory* newObj = new MmsServerConnectionFactory();
     context.addService("mmspushplugin::ServerConnectionFactory",newObj);
     return *newObj;
     }
     ServerConnectionFactory* retObj = reinterpret_cast
     <ServerConnectionFactory*>(ptr);
     return *retObj;
     */
}

OS_EXPORT MmsServerConnectionFactory& MmsServerConnectionFactory::getFactory()
{
    JELOG2(EWMA);
    ServerConnectionFactory& scf = ::getServerConnectionFactory();
    return reinterpret_cast<MmsServerConnectionFactory&>(scf);
}

/**
 *
 */
OS_EXPORT MmsServerConnectionFactory::MmsServerConnectionFactory()
{
    JELOG2(EWMA);
}

/**
 *
 */
OS_EXPORT MmsServerConnectionFactory::~MmsServerConnectionFactory()
{
    JELOG2(EWMA);
}

ServerConnection&
MmsServerConnectionFactory::createPushServerConn(const std::wstring& aUri,
        const std::wstring& aFilter,
        ConnectionListener* aListener,
        PendingConnectionListener* aPendingConnListener)
{
    JELOG2(EWMA);
    ScopedLock lockObj(mMutex);

    SrvConnContainerIter_t iter = mServerConnections.find(aUri);
    if (iter != mServerConnections.end())
    {
        if ((SrvConnContainerData::NORMAL == iter->second.connType()))
        {
            LOG(EWMA,EInfo,"Clearing Unregister flag");
            MmsServerConnection* serverConn = 0;
            serverConn = reinterpret_cast<MmsServerConnection*>(iter->second.getConn());
            serverConn->clearUnregister();
        }
    }
    return ServerConnectionFactoryBase::createPushServerConn(aUri,aFilter,
                                           aListener,aPendingConnListener);
}

void MmsServerConnectionFactory::deletePushConnection(const std::wstring& aUri)
{
    JELOG2(EWMA);
    LOG1(EWMA,EInfo,"MmsServerConnection on ID %S", aUri.c_str());
    MmsServerConnection* serverConn = 0;
    serverConn = reinterpret_cast<MmsServerConnection*>(getPushConnection(aUri));
    if (0 != serverConn)
       serverConn->setUnregister();
    ServerConnectionFactoryBase::deletePushConnection(aUri);
}
/**
 *
 */
OS_EXPORT ServerConnection* MmsServerConnectionFactory::createSrvConnObj(
    const std::wstring& aUri,
    const std::wstring& aFilter)
{
    return MmsServerConnection::getServerConnection(aUri,aFilter);
}

OS_EXPORT bool MmsServerConnectionFactory::isMultipleSrvConnAllowed()
{
    return false;
}

