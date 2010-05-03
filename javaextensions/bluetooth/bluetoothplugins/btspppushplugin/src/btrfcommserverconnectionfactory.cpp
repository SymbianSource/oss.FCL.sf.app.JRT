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
#include "btrfcommserverconnectionfactory.h"

using namespace java::util;
using namespace java::push;
using namespace java::bluetooth;

#if defined(__SYMBIAN32__) && defined(__WINSCW__)
#include <pls.h>
#else
static BTRFCOMMServerConnectionFactory* sRfcommSrvConnFac = 0;
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
    BTRFCOMMServerConnectionFactory* retObj =
        Pls<BTRFCOMMServerConnectionFactory>(uid);
    return *retObj;
#else
    if (sRfcommSrvConnFac == 0)
    {
        sRfcommSrvConnFac = new BTRFCOMMServerConnectionFactory();
    }
    return *sRfcommSrvConnFac;
#endif
}


OS_EXPORT
BTRFCOMMServerConnectionFactory& BTRFCOMMServerConnectionFactory::getFactory()
{
    JELOG2(EJavaBluetooth);
    ServerConnectionFactory& scf = ::getServerConnectionFactory();
    return reinterpret_cast<BTRFCOMMServerConnectionFactory&>(scf);
}

/**
 *
 */
OS_EXPORT BTRFCOMMServerConnectionFactory::BTRFCOMMServerConnectionFactory()
{
    JELOG2(EJavaBluetooth);
}

/**
 *
 */
OS_EXPORT BTRFCOMMServerConnectionFactory::~BTRFCOMMServerConnectionFactory()
{
    JELOG2(EJavaBluetooth);
}

/**
 *
 */
OS_EXPORT ServerConnection* BTRFCOMMServerConnectionFactory::createSrvConnObj(
    const std::wstring& aUri, const std::wstring& aFilter)
{
    JELOG2(EJavaBluetooth);

    // If launched by push, URI and Filter are already valid. No need to check.
    // Function Server also needs to be passed during construction.
    return new RFCOMMPushServerConnection(aUri, aFilter,this);
}

/**
 * The create method is overrided to support bluetooth specific behaviour
 * of create .
 */
OS_EXPORT ServerConnection* BTRFCOMMServerConnectionFactory::create(
    const std::wstring& aUri)
{
    JELOG2(EJavaBluetooth);
    RFCOMMPushServerConnection * server = NULL;
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
        server = (RFCOMMPushServerConnection*)newDataObj.getConn();
        server->setCreatedByPush();
    }
    else if (SrvConnContainerData::PUSH_LISTEN_BY_MIDLET == iter->second.connType()
             || SrvConnContainerData::NORMAL == iter->second.connType())
    {
        // If a server connection is already created for the given uri
        // and it is handled by application then create a new instance
        // of server connection
        server = new RFCOMMPushServerConnection(aUri, L"",this);
    }
    else
    {
        // If a server connection is already created for the given uri
        // and it is handled by push framework then return the same
        // instance of server connection to the application
        iter->second.setConnType(SrvConnContainerData::PUSH_LISTEN_BY_MIDLET);
        server = (RFCOMMPushServerConnection*)(*iter).second.getConn();
        server->setCreatedByPush();
    }
    return server;
}

OS_EXPORT bool BTRFCOMMServerConnectionFactory::isMultipleSrvConnAllowed()
{
    return false;
}

