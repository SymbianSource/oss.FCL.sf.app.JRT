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
* Description: CCommsServer implements the Symbian IPC server
*
*/


#include <e32std.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <e32def_private.h>
#endif

#include "logger.h"

#include "commssession.h"
#include "commsserver.h"
#include "commspermissions.h"
#include "common.h"

namespace java
{
namespace comms
{

const TInt KRangeCnt = 1;
const TInt Ranges[KRangeCnt] =
{
    0
};

const TUint8 PolicyInds[] =
{
    CPolicyServer::EAlwaysPass
};

const CPolicyServer::TPolicy Policy =
{
    CPolicyServer::EAlwaysPass,    // on connect
    KRangeCnt,
    Ranges,
    PolicyInds,
    0 // Mandatory spare
};

CCommsServer::CCommsServer(IpcListener& aListener)
        : CPolicyServer(EPriorityStandard, Policy, ESharableSessions), mListener(aListener)
{
}

CCommsServer::~CCommsServer()
{
}


CSession2* CCommsServer::NewSessionL(const TVersion& /* aVersion */, const RMessage2& aMessage) const
{
    int permissions = getPermissions(aMessage);
    return new(ELeave) CCommsSession(mListener, permissions);
}

void CCommsServer::StartL(const TDesC& aName)
{
    CPolicyServer::StartL(aName);
}

int CCommsServer::getPermissions(const RMessage2& aMessage) const
{
    int permissions = 0;

    if (aMessage.HasCapability(ECapabilityWriteDeviceData,
                               __PLATSEC_DIAGNOSTIC_STRING("javacomms: certificate management not allowed")))
    {
        permissions |= MANAGE_CERTIFICATES;
    }

    if (aMessage.HasCapability(ECapabilityTrustedUI,
                               __PLATSEC_DIAGNOSTIC_STRING("javacomms: install application not allowed")))
    {
        permissions |= INSTALL_APPLICATION;
    }

    if (aMessage.HasCapability(ECapabilityNetworkControl,
                               __PLATSEC_DIAGNOSTIC_STRING("javacomms: launch application not allowed")))
    {
        permissions |= LAUNCH_APPLICATION;
    }

    if (aMessage.HasCapability(ECapabilityPowerMgmt,
                               __PLATSEC_DIAGNOSTIC_STRING("javacomms: stop application not allowed")))
    {
        permissions |= STOP_APPLICATION;
    }

    return permissions;
}

} // namespace comms
} // namespace java
