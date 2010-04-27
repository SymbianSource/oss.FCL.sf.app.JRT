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
* Description: Factory methods for creating client and server connections
*
*/

#include "logger.h"

#include "transport.h"
#include "ipcclient.h"
#include "ipcserver.h"

namespace java
{
namespace comms
{

OS_EXPORT IpcConnectionInterface* IpcConnectionFactory::createConnection(IpcListener* aListener)
{
    IpcConnectionInterface* con = new IpcClient(aListener);
    return con;
}


OS_EXPORT IpcServerConnectionInterface* IpcConnectionFactory::createServerConnection(IpcListener* aListener)
{
    IpcServerConnectionInterface* con = new IpcServer(aListener);
    return con;
}

} // namespace comms
} // namespace java

