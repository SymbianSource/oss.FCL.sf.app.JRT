/*******************************************************************************
 * Copyright (c) 2005, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#include <basched.h>
#include "swtserver.h"
#include "swtclient.h"
#include "swtfunctor.h"


#define KServerVersion TVersion(1,0,0)


// ======== MEMBER FUNCTIONS ========


inline CSwtServer::CSwtSession::CSwtSession(CSwtServer& aServer)
        : iServer(aServer)
{
}

TInt CSwtServer::CSwtSession::DoServiceL(TAction aAction, const TAny* aParam)
{
    switch (aAction)
    {
    case EExecute:
        (*static_cast<const MSwtFunctor*>(aParam))(); // It doesn't look like it but this may leave
        return KErrNone;
    default:
        ASSERT(EFalse);
        return KErrNotSupported;
    }
}

void CSwtServer::CSwtSession::ServiceL(const RMessage2& aMessage)
{
    TInt result = 0;
    TRAPD(error, (result=DoServiceL(static_cast<TAction>(aMessage.Function()), aMessage.Ptr0())));
    if (error)
    {
        if (error == KLeaveExit) // This is used to exit the application, we obviously want to let it through
            User::Leave(KLeaveExit);
        aMessage.Complete(error);
    }
    else
        aMessage.Complete(result);
}

CSwtServer::CSwtServer()
        : CServer2(EPriorityUserInput, ESharableSessions)
{
}

EXPORT_C CSwtServer::~CSwtServer()
{
}

EXPORT_C CSwtServer* CSwtServer::NewL(TAny* /*aObj*/)
{
    CSwtServer* server = new(ELeave) CSwtServer;
    CleanupStack::PushL(server);
    server->StartL(KNullDesC);
    CleanupStack::Pop(server);
    return server;
}

TVersion CSwtServer::Version()
{
    return KServerVersion;
}


//
// Virtual methods from CServer2
//

TInt CSwtServer::RunError(TInt aError)
{
    if (aError == KLeaveExit) // This is used to exit the application, we obviously want to let it through
    {
        return aError;
    }
    else
    {
        return CServer2::RunError(aError);
    }
}

CSession2* CSwtServer::NewSessionL(const TVersion& aVersion, const RMessage2& /*aMessage*/) const
{
    TVersion version(KServerVersion);
    if (aVersion.iMajor!=version.iMajor || aVersion.iMinor!=version.iMinor)
        User::Leave(KErrNotSupported);

    return new(ELeave) CSwtSession(const_cast<CSwtServer&>(*this));
}
