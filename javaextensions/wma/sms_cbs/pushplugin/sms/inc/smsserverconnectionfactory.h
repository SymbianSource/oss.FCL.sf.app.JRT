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

#ifndef SMSSERVERCONNECTIONFACTORY_H
#define SMSSERVERCONNECTIONFACTORY_H

//#include <wchar.h>
#include "javaosheaders.h"
#include "serverconnectionfactorybase.h"

using namespace java::push;
/**
 * SmsServerConnectionFactory used to create the ServerConnection.
 * It implement ServerConnectionFactoryBase.
 *
 *  @lib SmsServerConnection.dll
 *  @
 */
class SmsServerConnectionFactory: public ServerConnectionFactoryBase
{
    //Not implemented
    SmsServerConnectionFactory(const SmsServerConnectionFactory&);
    SmsServerConnectionFactory &operator=(const SmsServerConnectionFactory&);

public:
    OS_IMPORT SmsServerConnectionFactory();
    OS_IMPORT virtual ~SmsServerConnectionFactory();
    /**
     * creates a push connection for the given Connection URI
     * @param aUri -Uri for which the push connection is created
     * @param aFilter -
     * @return ServerConnection
     */
    OS_IMPORT virtual ServerConnection* createSrvConnObj(
        const std::wstring& aUri,const std::wstring& aFilter);

    /**
     * Checks for multiple server connection is allowed for the given
     * Connection URI
     *
     *
     * @returns false
     */
    OS_IMPORT virtual bool isMultipleSrvConnAllowed();
    /**
     * factory method to create the server connection factory
     * @
     *
     * @return SmsServerConnectionFactory
     */
    OS_IMPORT static SmsServerConnectionFactory& getFactory();

};

#endif // SMSSERVERCONNECTIONFACTORY_H
