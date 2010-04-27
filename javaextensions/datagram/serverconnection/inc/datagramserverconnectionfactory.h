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
* Description:  ?Description
 *
*/

#ifndef DATAGRAMSERVERCONNECTIONFACTORY_H
#define DATAGRAMSERVERCONNECTIONFACTORY_H

#include "serverconnectionfactorybase.h"
#include "javaosheaders.h"

using namespace java::push;

/**
 * This class implements the virtual method of ServerConnectionFactory
 * and uses the default implementation of ServerConnectionFactory interface.
 * for rest of the methods
 */

class DatagramServerConnectionFactory : public ServerConnectionFactoryBase
{
    //Not implemented
    DatagramServerConnectionFactory(const DatagramServerConnectionFactory&);
    DatagramServerConnectionFactory &operator=(const DatagramServerConnectionFactory&);

public:
    OS_IMPORT DatagramServerConnectionFactory();
    OS_IMPORT virtual ~DatagramServerConnectionFactory();

    // ServerConnectionFactoryBase methods
    OS_IMPORT virtual ServerConnection* createSrvConnObj(const std::wstring& aUri,
            const std::wstring& aFilter);
    OS_IMPORT virtual bool isMultipleSrvConnAllowed();

    OS_IMPORT static DatagramServerConnectionFactory& getFactory();
    OS_IMPORT virtual ServerConnection* create(const std::wstring& aUri);

};

#endif // DATAGRAMSERVERCONNECTIONFACTORY_H
