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


#ifndef BTOBEXSERVERCONNECTIONFACTORY_H
#define BTOBEXSERVERCONNECTIONFACTORY_H

#include "javaosheaders.h"
#include "serverconnectionfactorybase.h"

namespace java
{
namespace bluetooth
{

/**
 * BTOBEXServerConnectionFactory used to create the ServerConnection.
 * It implements ServerConnectionFactoryBase.
 *
 * Internally it contains a Function Server that will be passed to every
 * ServerConnection object created.
 *
 *  @lib javabtgoepscplugin.dll
 *  @
 */
class BTOBEXServerConnectionFactory: public java::push::ServerConnectionFactoryBase
{
    //Not implemented
    BTOBEXServerConnectionFactory(const BTOBEXServerConnectionFactory&);
    BTOBEXServerConnectionFactory &operator=(
        const BTOBEXServerConnectionFactory&);

public:
    OS_IMPORT BTOBEXServerConnectionFactory();
    OS_IMPORT ~BTOBEXServerConnectionFactory();

    /**
     * This operation creates ServerConnection if it is not already created.
     * @exception throws RuntimeException if type of the existing ServerConnection
     *            object is PUSH_LISTEN_BY_MIDLET || PUSH and isMultipleSrvConnAllowed()
     *            operation returns 'false'.
     */
    OS_IMPORT virtual java::push::ServerConnection* create(
        const std::wstring& aUri);
    /**
     * create serverconnection for the given Connection URI
     * @
     *
     * @return ServerConnection
     */
    OS_IMPORT
    virtual java::push::ServerConnection* createSrvConnObj(
        const std::wstring& aUri, const std::wstring& aFilter);

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
     * @return BTOBEXServerConnectionFactory
     */
    OS_IMPORT static BTOBEXServerConnectionFactory& getFactory();

};

} // namespace java
} // namespace bluetooth


#endif // BTOBEXSERVERCONNECTIONFACTORY_H
