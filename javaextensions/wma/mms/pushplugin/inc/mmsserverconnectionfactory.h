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

#ifndef MMSSERVERCONNECTIONFACTORY_H
#define MMSSERVERCONNECTIONFACTORY_H

#include "serverconnectionfactorybase.h"

using namespace java::push;
/**
 * MmsServerConnectionFactory used to create the ServerConnection.
 * It implement ServerConnectionFactoryBase.
 *
 *  @lib MmsServerConnection.dll
 *  @
 */
class MmsServerConnectionFactory: public ServerConnectionFactoryBase
{
    //Not implemented
    MmsServerConnectionFactory(const MmsServerConnectionFactory&);
    MmsServerConnectionFactory &operator=(const MmsServerConnectionFactory&);

public:
    OS_IMPORT MmsServerConnectionFactory();
    OS_IMPORT virtual ~MmsServerConnectionFactory();
    /**
     * create serverconnection for the given Connection URI
     * @
     *
     * @return ServerConnection
     */
    OS_IMPORT virtual ServerConnection* createSrvConnObj(
        const std::wstring& aUri,
        const std::wstring& aFilter);
    /**
     * Checks for multiple server connection is allowed for the given
     * Connection URI
     *
     *
     * @return true if allowed otherwise false.
     */
    OS_IMPORT virtual bool isMultipleSrvConnAllowed();
    /**
     * factory method to create the server connection factory
     * @
     *
     * @return MmsServerConnectionFactory
     */
    OS_IMPORT static MmsServerConnectionFactory& getFactory();

};

#endif // MMSSERVERCONNECTIONFACTORY_H
