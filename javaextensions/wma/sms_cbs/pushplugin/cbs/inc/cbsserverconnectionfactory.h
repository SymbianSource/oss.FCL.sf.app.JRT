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

#ifndef CBSSERVERCONNECTIONFACTORY_H
#define CBSSERVERCONNECTIONFACTORY_H

//#include <wchar.h>
#include "javaosheaders.h"
#include "serverconnectionfactorybase.h"

using namespace java::push;
using namespace java::wma;
/**
 * CbsServerConnectionFactory used to create the ServerConnection.
 * It implement ServerConnectionFactoryBase.
 *
 *  @lib CbsServerConnection.dll
 *  @
 */
class CbsServerConnectionFactory: public ServerConnectionFactoryBase
{
    //Not implemented
    CbsServerConnectionFactory(const CbsServerConnectionFactory&);
    CbsServerConnectionFactory &operator=(const CbsServerConnectionFactory&);

public:
    OS_IMPORT CbsServerConnectionFactory();
    OS_IMPORT virtual ~CbsServerConnectionFactory();
    /**
     * create connection for the given Connection URI
     * @
     *
     * @return ServerConnection
     */
    OS_IMPORT virtual ServerConnection* createSrvConnObj(
        const std::wstring& aUri,const std::wstring& aFilter);

    /**
     * Checks for multiple server connection is allowed for the given
     * Connection URI
     *
     *
     * @return
     */
    OS_IMPORT virtual bool isMultipleSrvConnAllowed();
    /**
     * factory method to create the server connection factory
     * @
     *
     * @return CbsServerConnectionFactory
     */
    OS_IMPORT static CbsServerConnectionFactory& getFactory();

};

#endif // CBSSERVERCONNECTIONFACTORY_H
