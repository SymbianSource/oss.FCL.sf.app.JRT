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

#include <memory>

#include "functionserver.h"
#include "os_functionclient.h"
#include "logger.h"
#include "javasymbianoslayer.h"

using namespace java::util;

/**
 *
 */

OsFunctionClient::OsFunctionClient()
{
    JELOG2(EUtils);
    mLock.CreateLocal();
}

OsFunctionClient::~OsFunctionClient()
{
    JELOG2(EUtils);
    mLock.Close();
}

TInt OsFunctionClient::connect(const OsFunctionServer& server)
{
    JELOG2(EUtils);

    int error = CreateSession(server.Server(), TVersion());
    if (error != KErrNone)
    {
        return error;
    }
    return ShareAuto();
}

/**
 *
 */
TInt OsFunctionClient::execute(const Functor& functor) const
{
    JELOG2(EUtils);
//    mLock.Wait();
    int res = SendReceive(OsFunctionServer::EExecute, TIpcArgs(&functor));
//    mLock.Signal();

    return res;
}

/**
 *
 */
TInt OsFunctionClient::sendCloseMessage(FunctionServer* server) const
{
    JELOG2(EUtils);
    int res = SendReceive(OsFunctionServer::EStop, TIpcArgs(server));

    return res;
}
