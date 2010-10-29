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
* Description:  This class implements Comms server endpoint. Server
*                endpoint waits for incoming connections and it forwards
*
*/

#include "logger.h"

#include "commsserverendpoint.h"
#include "commsmessage.h"
#include "commscontext.h"

namespace java
{
namespace comms
{

OS_EXPORT CommsServerEndpoint::CommsServerEndpoint() : mAddress(0), mAddedToContext(false)
{
    JELOG2(EJavaComms);
    mIpc.reset(IpcConnectionFactory::createServerConnection(this));
}

OS_EXPORT CommsServerEndpoint::CommsServerEndpoint(const std::wstring& aName): mAddress(0), mAddedToContext(true)
{
    JELOG2(EJavaComms);
    mIpc.reset(IpcConnectionFactory::createServerConnection(this));
    CommsContext::getContext().add(this, aName);
}

OS_EXPORT CommsServerEndpoint::~CommsServerEndpoint()
{
    JELOG2(EJavaComms);
    stop();
    // avoid creating context when not needed
    if (mAddedToContext)
    {
        CommsContext::getContext().remove(this);
    }
}

OS_EXPORT CommsServerEndpoint* CommsServerEndpoint::find(const std::wstring& aName)
{
    JELOG2(EJavaComms);
    return CommsContext::getContext().findServer(aName);
}

OS_EXPORT int CommsServerEndpoint::start(int aAddress)
{
    JELOG2(EJavaComms);
    mAddress = aAddress;
    return mIpc->start(aAddress);
}

OS_EXPORT int CommsServerEndpoint::stop()
{
    JELOG2(EJavaComms);
    mIpc->stop();
    return 0;
}

OS_EXPORT int CommsServerEndpoint::send(CommsMessage& aMessage)
{
    JELOG2(EJavaComms);
    char* buf = aMessage.toByteArray();
    int rc =  mIpc->send(reinterpret_cast<ipcMessage_t*>(buf));
    return rc;
}

OS_EXPORT int CommsServerEndpoint::detachFromVm(JNIEnv* aEnv)
{
    JELOG2(EJavaComms);
    CommsEndpoint::detachFromVm(aEnv);

    int rc = stop();
    rc = start(mAddress);

    return rc;
}

} // namespace comms
} // namespace java


