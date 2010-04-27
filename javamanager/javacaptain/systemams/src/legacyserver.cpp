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
* Description:  CLegacyServer
*
*/


#include "logger.h"

#include "legacyserversession.h"
#include "legacyserver.h"

CLegacyServer::CLegacyServer(const int aServerId,
                             MLegacyServerHandler* aHandler)
        :CServer2(EPriorityNormal),
        mServerId(aServerId),
        mHandler(aHandler)
{
    JELOG2(EJavaSystemAMS);
}

CLegacyServer::~CLegacyServer()
{
    JELOG2(EJavaSystemAMS);
}

CSession2* CLegacyServer::NewSessionL(const TVersion&, const RMessage2& /*aMessage*/) const
{
    return new(ELeave) CLegacyServerSession(*this);
}

_LIT(KPublicServerName,     "!SystemAMSTrader.Public");
_LIT(KAdminServerName,  "!SystemAMS.SecurityAdmin");

void CLegacyServer::StartL()
{

    switch (mServerId)
    {
    case SERVER_ID_PUBLIC_C:
        CServer2::StartL(KPublicServerName);
        break;

    case SERVER_ID_SECURITY_ADMIN_C:
        CServer2::StartL(KAdminServerName);
        break;
    }
}

void CLegacyServer::HandleMessageL(const RMessage2& aMessage) const
{
    JELOG2(EJavaSystemAMS);
    mHandler->HandleMessageL(mServerId, aMessage);
}

