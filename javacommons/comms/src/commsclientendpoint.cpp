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
* Description:  This class implements Comms client endpoint. Client
*                endpoint can be connected to server endpoint and it forwards
*
*/

#include <time.h>
#include <errno.h>
#include <string>

#include "logger.h"

#include "commsclientendpoint.h"
#include "comms.h"
#include "commsmessage.h"
#include "commscontext.h"

namespace java
{
namespace comms
{


OS_EXPORT CommsClientEndpoint::CommsClientEndpoint(): mAddress(0), mAddedToContext(false)
{
    JELOG2(EJavaComms);
    mIpc.reset(IpcConnectionFactory::createConnection(this));
}

OS_EXPORT CommsClientEndpoint::CommsClientEndpoint(const std::wstring& aName): mAddress(0), mAddedToContext(true)
{
    JELOG2(EJavaComms);
    mIpc.reset(IpcConnectionFactory::createConnection(this));
    CommsContext::getContext().add(this, aName);
}

OS_EXPORT CommsClientEndpoint::~CommsClientEndpoint()
{
    JELOG2(EJavaComms);
    disconnect();
    // avoid creating context when not needed
    if (mAddedToContext)
    {
        CommsContext::getContext().remove(this);
    }
}

OS_EXPORT CommsClientEndpoint* CommsClientEndpoint::find(const std::wstring& aName)
{
    JELOG2(EJavaComms);
    return CommsContext::getContext().find(aName);
}

OS_EXPORT int CommsClientEndpoint::connect(int aAddress)
{
    JELOG2(EJavaComms);
    mAddress = aAddress;
    return mIpc->connect(aAddress);
}

OS_EXPORT int CommsClientEndpoint::disconnect()
{
    JELOG2(EJavaComms);
    mIpc->disconnect();
    return 0;
}


OS_EXPORT int CommsClientEndpoint::send(CommsMessage& aMessage)
{
    JELOG2(EJavaComms);
    char* buf = aMessage.toByteArray();
    int rc =  mIpc->send(reinterpret_cast<ipcMessage_t*>(buf));
    return rc;
}


OS_EXPORT int CommsClientEndpoint::detachFromVm(JNIEnv* aEnv)
{
    JELOG2(EJavaComms);
    CommsEndpoint::detachFromVm(aEnv);

    int rc = disconnect();
    rc = connect(mAddress);

    return rc;
}

} // namespace comms
} // namespace java

