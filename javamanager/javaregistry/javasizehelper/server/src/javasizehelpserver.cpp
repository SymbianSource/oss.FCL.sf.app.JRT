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
* Description:  Java Size Helper implementation
*
*/


#include "javasizehelpserver.h"
#include "javasizehelpcommon.h"
#include "javasizehelpsession.h"
#include "logger.h"

// ---------------------------------------------------------------------------
// CJavaSizeHelpServer::NewL
// ---------------------------------------------------------------------------
CJavaSizeHelpServer* CJavaSizeHelpServer::NewL()
{
    JELOG2(EUtils);
    CJavaSizeHelpServer* self = NewLC();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CJavaSizeHelpServer::NewLC
// ---------------------------------------------------------------------------
CJavaSizeHelpServer* CJavaSizeHelpServer::NewLC()
{
    JELOG2(EUtils);
    CJavaSizeHelpServer* self = new(ELeave) CJavaSizeHelpServer();
    CleanupStack::PushL(self);
    self->Construct();
    return self;
}

// ---------------------------------------------------------------------------
// CJavaSizeHelpServer::CJavaSizeHelpServer
// ---------------------------------------------------------------------------
CJavaSizeHelpServer::CJavaSizeHelpServer():CServer2(CActive::EPriorityStandard)
{
    JELOG2(EUtils);
}
// ---------------------------------------------------------------------------
// CJavaSizeHelpServer::Construct
// ---------------------------------------------------------------------------
void CJavaSizeHelpServer::Construct()
{
    JELOG2(EUtils);
    TInt error = Start(KJavaSizeHelpServerName);

    if (error != KErrNone)
    {
        User::Panic(KJavaSizeHelpServerName, error);
    }
}
// ---------------------------------------------------------------------------
// CJavaSizeHelpServer::~CJavaSizeHelpServer
// ---------------------------------------------------------------------------
CJavaSizeHelpServer::~CJavaSizeHelpServer()
{
    JELOG2(EUtils);
}
// ---------------------------------------------------------------------------
// CJavaSizeHelpServer::NewSessionL
// ---------------------------------------------------------------------------
CSession2* CJavaSizeHelpServer::NewSessionL(const TVersion& /*aVersion*/,
        const RMessage2& /*aMessage*/) const
{
    JELOG2(EUtils);
    return new(ELeave) CJavaSizeHelpSession;
}
// ---------------------------------------------------------------------------
// CJavaSizeHelpServer::RunError
// ---------------------------------------------------------------------------
TInt CJavaSizeHelpServer::RunError(TInt aError)
{
    JELOG2(EUtils);
    // Bad descriptor implies bad client
    if (aError == KErrBadDescriptor)
    {
        Message().Panic(KJavaSizeHelpServerName, aError);
    }
    else
    {
        Message().Complete(aError);
    }

    // Continue handling requests
    ReStart();
    return KErrNone;
}
// ---------------------------------------------------------------------------
// CJavaSizeHelpServer::AddSession
// ---------------------------------------------------------------------------
void CJavaSizeHelpServer::AddSession()
{
    ++iSessionCount;
}
// ---------------------------------------------------------------------------
// CJavaSizeHelpServer::RemoveSession
// ---------------------------------------------------------------------------
void CJavaSizeHelpServer::RemoveSession()
{
    --iSessionCount;

    if (iSessionCount == 0)
    {
        CActiveScheduler::Stop();
    }
}

LOCAL_C void ExeMainL()
{
    JELOG2(EUtils);
    // Install active scheduler
    CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
    CleanupStack::PushL(scheduler);
    CActiveScheduler::Install(scheduler);

    CJavaSizeHelpServer::NewLC();

    User::LeaveIfError(User::RenameThread(KJavaSizeHelpServerName));
    RProcess::Rendezvous(KErrNone);

    CActiveScheduler::Start();

    CleanupStack::PopAndDestroy();  // CServer
    CleanupStack::PopAndDestroy();  // CActiveScheduler
}

//GLDEF_C TInt E32Main()
int dllMain(int,char**)
{
    JELOG2(EUtils);
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();

    TRAPD(error, ExeMainL());
    if (KErrNone != error)
    {
        ELOG1(EJavaStorage, "JavaSizeHelpServer: main panics with %d", error);
        User::Panic(KJavaSizeHelpServerName, error);
    }

    delete cleanup;
    __UHEAP_MARKEND;

    return 0;
}
