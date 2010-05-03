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
#include "btl2cappushserverconnection.h"
#include "btl2capserverconnectionfactory.h"

using namespace java::util;
using namespace java::push;
using namespace std;
using namespace java::bluetooth;

#if defined(__SYMBIAN32__) && defined(__WINSCW__)
#include <pls.h>
#else
static BtL2CapServerConnectionFactory* sL2CapSrvConnFac = 0;
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
    TUid uid = TUid::Uid(0xE0000053);
    BtL2CapServerConnectionFactory* retObj =
        Pls<BtL2CapServerConnectionFactory>(uid);
    return *retObj;
#else
    if (sL2CapSrvConnFac == 0)
    {
        sL2CapSrvConnFac = new BtL2CapServerConnectionFactory();
    }
    return *sL2CapSrvConnFac;
#endif
}


OS_EXPORT
BtL2CapServerConnectionFactory& BtL2CapServerConnectionFactory::getFactory()
{
    JELOG2(EJavaBluetooth);
    ServerConnectionFactory& scf = ::getServerConnectionFactory();
    return reinterpret_cast<BtL2CapServerConnectionFactory&>(scf);
}

/**
 *
 */
OS_EXPORT BtL2CapServerConnectionFactory::BtL2CapServerConnectionFactory()
{
    JELOG2(EJavaBluetooth);
}

/**
 *
 */
OS_EXPORT BtL2CapServerConnectionFactory::~BtL2CapServerConnectionFactory()
{
    JELOG2(EJavaBluetooth);

}

/**
 *
 */
OS_EXPORT ServerConnection* BtL2CapServerConnectionFactory::createSrvConnObj(
    const std::wstring& aUri, const std::wstring& aFilter)
{
    JELOG2(EJavaBluetooth);

    // If launched by push, URI and Filter are already valid. No need to check.
    // Function Server also needs to be passed during construction.
    return new L2CapPushServerConnection(aUri, aFilter);
}

/**
 * The create method is overrided to support bluetooth specific behaviour
 * of create .
 */
OS_EXPORT ServerConnection* BtL2CapServerConnectionFactory::create(
    const std::wstring& aUri)
{
    JELOG2(EJavaBluetooth);
    L2CapPushServerConnection * server = NULL;
    SrvConnContainerIter_t iter = mServerConnections.find(aUri);
    if (iter == mServerConnections.end())
    {
        // If no server connection object is created before for the given uri
        // then create and return the server connection
        std::auto_ptr<ServerConnection> newSrvConn(createSrvConnObj(aUri, L""));
        SrvConnContainerData newDataObj(newSrvConn.release(), 0, false,
                                        SrvConnContainerData::NORMAL);
        mServerConnections.insert(std::pair<std::wstring,SrvConnContainerData>
                                  (aUri,newDataObj));
        server = (L2CapPushServerConnection*)newDataObj.getConn();
        server->setCreatedByPush();
    }
    else if (SrvConnContainerData::PUSH_LISTEN_BY_MIDLET == iter->second.connType()
             || SrvConnContainerData::NORMAL == iter->second.connType())
    {
        // If a server connection is already created for the given uri
        // and it is handled by application then create a new instance
        // of server connection
        server = new L2CapPushServerConnection(aUri, L"");
    }
    else
    {
        // If a server connection is already created for the given uri
        // and it is handled by push framework then return the same
        // instance of server connection to the application
        iter->second.setConnType(SrvConnContainerData::PUSH_LISTEN_BY_MIDLET);
        server = (L2CapPushServerConnection*)(*iter).second.getConn();
        server->setCreatedByPush();
    }
    return server;
}

OS_EXPORT bool BtL2CapServerConnectionFactory::isMultipleSrvConnAllowed()
{
    return false;
}



