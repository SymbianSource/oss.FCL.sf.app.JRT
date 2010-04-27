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


#include <basched.h>
#include "fs_functor.h"
#include "os_functionserver.h"
#include "javasymbianoslayer.h"
#include "logger.h"
#include "exceptionbase.h"

using namespace java::util;


// ======== MEMBER FUNCTIONS ========


inline OsFunctionServer::CFunctionSession::CFunctionSession()
{
    JELOG2(EUtils);
}

TInt OsFunctionServer::CFunctionSession::doServiceL(TAction action, const TAny* param)
{
    JELOG2(EUtils);
    switch (action)
    {
    case EExecute:
        (*static_cast<const Functor*>(param))(); // It doesn't look like it but this may leave
        return KErrNone;
    case EStop:
        ((OsFunctionServer*)Server())->startShutDown();
        return KErrNone;
    default:
        return KErrNotSupported;
    }
}

void OsFunctionServer::CFunctionSession::ServiceL(const RMessage2& message)
{
    JELOG2(EUtils);
    TRAPD(error, (doServiceL(static_cast<TAction>(message.Function()), message.Ptr0())));
    if (error)
    {
        if (error == KLeaveExit) // This is used to exit the application, we obviously want to let it through
        {
            User::Leave(KLeaveExit);
        }
    }
    message.Complete(error);
}

OsFunctionServer::OsFunctionServer()
        : CServer2(EPriorityStandard, ESharableSessions)
{
    JELOG2(EUtils);
}

OsFunctionServer::~OsFunctionServer()
{
    JELOG2(EUtils);
}
void OsFunctionServer::startShutDown()
{
    JELOG2(EUtils);
    mShutdown.startShutDown();
}

void OsFunctionServer::startOsServer()
{
    JELOG2(EUtils);
    TRAPD(err, StartL(KNullDesC));
    if (err != 0)
    {
        std::string errorStr("FServer creation failed: ");
        errorStr.append(JavaCommonUtils::intToString(err));
        throw java::util::ExceptionBase(errorStr, __FILE__, __FUNCTION__, __LINE__);
    }
}


//
// Virtual methods from CServer2
//

OS_EXPORT TInt OsFunctionServer::RunError(TInt error)
{
    JELOG2(EUtils);
    if (error == KLeaveExit) // This is used to exit the application, we obviously want to let it through
    {
        return error;
    }
    else
    {
        return CServer2::RunError(error);
    }
}

OS_EXPORT CSession2* OsFunctionServer::NewSessionL(const TVersion& /*version*/, const RMessage2& /*message*/) const
{
    JELOG2(EUtils);
    return new(ELeave) CFunctionSession();
}


void OsFunctionServer::stopServerInsideServerThread()
{
    JELOG2(EUtils);
    CActiveScheduler::Stop();
}

ServerShutdown::ServerShutdown()
        : CActive(CActive::EPriorityStandard)
{
}

void ServerShutdown::startShutDown()
{
    CActiveScheduler::Add(this);
    iStatus=KRequestPending;
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
}


void ServerShutdown::RunL()
{
    JELOG2(EUtils);
    CActiveScheduler::Stop();
}

TInt ServerShutdown::RunError(TInt error)
{
    JELOG2(EUtils);
    if (error == KLeaveExit)
    {
        return error;
    }
    return KErrNone;
}

void ServerShutdown::DoCancel()
{
    JELOG2(EUtils);
    // nop
}

