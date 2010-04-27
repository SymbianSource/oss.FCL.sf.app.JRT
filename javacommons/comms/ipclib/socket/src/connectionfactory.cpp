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

#include "logger.h"

#include "transport.h"
#include "serversocketconnection.h"
#include "socketconnection.h"

namespace java
{
namespace comms
{

OS_EXPORT IpcConnectionInterface* IpcConnectionFactory::createConnection(IpcListener* aListener)
{
    IpcConnectionInterface* con = new SocketConnection(aListener);
    return con;
}


OS_EXPORT IpcServerConnectionInterface* IpcConnectionFactory::createServerConnection(IpcListener* aListener)
{
    IpcServerConnectionInterface* con = new ServerSocketConnection(aListener);
    return con;
}

} // namespace comms
} // namespace java

