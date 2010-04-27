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
* Description:  Socket Server Connection factory class for push feature
*
*/

#ifndef SOCKETSERVERCONNECTIONFACTORY_H
#define SOCKETSERVERCONNECTIONFACTORY_H

#include "serverconnectionfactorybase.h"
#include "javaosheaders.h"
#include <logger.h>

using namespace java::push;


/**
 * This class implements the virtual method of ServerConnectionFactory
 * and uses the default implementation of ServerConnectionFactory interface.
 * for rest of the methods
 */
class SocketServerConnectionFactory : public ServerConnectionFactoryBase
{
    //Not implemented
    SocketServerConnectionFactory(const SocketServerConnectionFactory&);
    SocketServerConnectionFactory &operator=(const SocketServerConnectionFactory&);

public:
    OS_IMPORT SocketServerConnectionFactory();
    OS_IMPORT virtual ~SocketServerConnectionFactory();

    // ServerConnectionFactoryBase methods
    OS_IMPORT virtual ServerConnection* createSrvConnObj(const std::wstring& aUri,const std::wstring& aFilter);
    OS_IMPORT virtual bool isMultipleSrvConnAllowed();

    OS_IMPORT static SocketServerConnectionFactory& getFactory();
    OS_IMPORT virtual ServerConnection* create(const std::wstring& aUri);
    bool isPushEnabled();


};

#endif // SOCKETSERVERCONNECTIONFACTORY_H
