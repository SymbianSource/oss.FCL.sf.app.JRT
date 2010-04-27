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
* Description: CRedirectServer forwards standard outs and log messages to
*              listener
*
*/


#include "javaredirectserver.h"
#include "javaredirectsession.h"
#include "javaredirectconsts.h"

using namespace java::util;

OS_EXPORT CRedirectServer::CRedirectServer(DiagnosticListener& aListener)
        : CServer2(EPriorityStandard, ESharableSessions), mListener(aListener)
{
}

OS_EXPORT CRedirectServer::~CRedirectServer()
{
}

OS_EXPORT void CRedirectServer::StartL()
{
    StartL(KJavaRedirectServer);
}

void CRedirectServer::StartL(const TDesC& aName)
{
    CServer2::StartL(aName);
}

CSession2* CRedirectServer::NewSessionL(const TVersion& aVersion, const RMessage2& /*aMessage*/) const
{
    TVersion version(REDIRECT_VERSION_MAJOR, REDIRECT_VERSION_MINOR, REDIRECT_VERSION_BUILD);
    if (!User::QueryVersionSupported(version, aVersion))
    {
        User::Leave(KErrNotSupported);
    }

    return new(ELeave) CRedirectSession(mListener);
}
