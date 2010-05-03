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
#include "btrfcommpushserverconnection.h"
#include "btobexserverconnectionfactory.h"

using namespace java::util;
using namespace java::push;
using namespace java::bluetooth;

#if defined(__SYMBIAN32__) && defined(__WINSCW__)
#include <pls.h>
#else
static BTOBEXServerConnectionFactory* sObexSrvConnFac = 0;
#endif

#ifdef __SYMBIAN32__
ServerConnectionFactory& getServerConnectionFactory()
{
#else
extern "C" ServerConnectionFactory& getServerConnectionFactory()
{
#endif
    JELOG2(EJavaBluetooth);

    //CONTEXT_REMOVAL
#if defined(__SYMBIAN32__) && defined(__WINSCW__)
    TUid uid = TUid::Uid(0xE0000055);
    BTOBEXServerConnectionFactory* retObj =
        Pls<BTOBEXServerConnectionFactory>(uid);
    return *retObj;
#else
    if (sObexSrvConnFac == 0)
    {
        sObexSrvConnFac = new BTOBEXServerConnectionFactory();
    }
    return *sObexSrvConnFac;
#endif
}


OS_EXPORT
BTOBEXServerConnectionFactory& BTOBEXServerConnectionFactory::getFactory()
{
    JELOG2(EJavaBluetooth);
    ServerConnectionFactory& scf = ::getServerConnectionFactory();
    return reinterpret_cast<BTOBEXServerConnectionFactory&>(scf);
}

/**
 *
 */
OS_EXPORT BTOBEXServerConnectionFactory::BTOBEXServerConnectionFactory()
{
    JELOG2(EJavaBluetooth);
}

/**
 *
 */
OS_EXPORT BTOBEXServerConnectionFactory::~BTOBEXServerConnectionFactory()
{
    JELOG2(EJavaBluetooth);
}

/**
 *
 */
OS_EXPORT ServerConnection* BTOBEXServerConnectionFactory::createSrvConnObj(
    const std::wstring& aUri, const std::wstring& aFilter)
{
    JELOG2(EJavaBluetooth);

    // If launched by push, URI and Filter are already valid.
    return new RFCOMMPushServerConnection(aUri, aFilter,this);
}

OS_EXPORT ServerConnection* BTOBEXServerConnectionFactory::create(
    const std::wstring& aUri)
{
    JELOG2(EJavaBluetooth);
    RFCOMMPushServerConnection * server = NULL;
    SrvConnContainerIter_t iter = mServerConnections.find(aUri);
    if (iter == mServerConnections.end())
    {
        std::auto_ptr<ServerConnection> newSrvConn(createSrvConnObj(aUri, L""));
        SrvConnContainerData newDataObj(newSrvConn.release(),0,false,SrvConnContainerData::NORMAL);
        mServerConnections.insert(std::pair<std::wstring,SrvConnContainerData>(aUri,newDataObj));
        server = (RFCOMMPushServerConnection*)newDataObj.getConn();
        server->setCreatedByPush();
    }
    else if (SrvConnContainerData::PUSH_LISTEN_BY_MIDLET == iter->second.connType()
             || SrvConnContainerData::NORMAL == iter->second.connType())
    {
        server = new RFCOMMPushServerConnection(aUri, L"",this);
    }
    else
    {
        iter->second.setConnType(SrvConnContainerData::PUSH_LISTEN_BY_MIDLET);
        server = (RFCOMMPushServerConnection*)(*iter).second.getConn();
        server->setCreatedByPush();
    }
    return server;
}

OS_EXPORT bool BTOBEXServerConnectionFactory::isMultipleSrvConnAllowed()
{
    return false;
}

