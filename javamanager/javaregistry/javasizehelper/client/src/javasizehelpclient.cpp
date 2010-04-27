/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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


#include "javasizehelpclient.h"
#include "javasizehelpcommon.h"
#include "javaprocessconstants.h"
#include "javasymbianoslayer.h"
#include "logger.h"
#include <s32mem.h>
#include <memory>

// ---------------------------------------------------------------------------
// RJavaSizeHelpClient::RJavaSizeHelpClient
// ---------------------------------------------------------------------------
EXPORT_C RJavaSizeHelpClient::RJavaSizeHelpClient()
{
    JELOG2(EUtils);
}

// ---------------------------------------------------------------------------
// RJavaSizeHelpClient::~RJavaSizeHelpClient
// ---------------------------------------------------------------------------
EXPORT_C RJavaSizeHelpClient::~RJavaSizeHelpClient()
{
    JELOG2(EUtils);
}

// ---------------------------------------------------------------------------
// RJavaSizeHelpClient::Connect
// ---------------------------------------------------------------------------
EXPORT_C TInt RJavaSizeHelpClient::Connect()
{
    JELOG2(EUtils);
    TInt retryCount(2);
    TInt error(KErrNone);
    while (retryCount)
    {
        error = CreateSession(KJavaSizeHelpServerName, TVersion(1,0,0));
        if (error != KErrNotFound && error != KErrServerTerminated)
        {
            break;
        }
        error = StartServer();
        if (error != KErrNone && error != KErrAlreadyExists)
        {
            break;
        }
        --retryCount;
    }
    return error;
}

// ---------------------------------------------------------------------------
// RJavaSizeHelpClient::StartServer
// ---------------------------------------------------------------------------
TInt RJavaSizeHelpClient::StartServer()
{
    JELOG2(EUtils);
    const TUidType serverUid = (KNullUid, KNullUid, KUidJavaSizeHelpServer);

    RProcess server;
    std::auto_ptr<HBufC> nameOfMidpProcess(stringToDes(java::runtime::JAVA_PROCESS));
    TInt error = server.Create(nameOfMidpProcess->Des(), KJavaSizeHelpServerName());
    //TInt error = server.Create( KJavaSizeHelpServerExe, KNullDesC );
    if (error != KErrNone)
    {
        return error;
    }
    // start server and wait for signal before proceeding
    TRequestStatus status;
    server.Rendezvous(status);
    if (status.Int() != KRequestPending)
    {
        server.Kill(0);
    }
    else
    {
        server.Resume();
    }

    User::WaitForRequest(status);
    error = server.ExitType() == EExitPanic ? KErrGeneral : status.Int();
    server.Close();
    return error;

//    return -1;
}

// ---------------------------------------------------------------------------
// RJavaSizeHelpClient::GetUserUsedDiskSpace
// ---------------------------------------------------------------------------
EXPORT_C TInt RJavaSizeHelpClient::GetUserUsedDiskSpace(const TDesC16& aBuffer) const
{
    JELOG2(EUtils);
    TInt usedDiskSpace(0);
    TPckg<TInt> pckgUsedDiskSpace(usedDiskSpace);
    TIpcArgs args(&aBuffer, &pckgUsedDiskSpace);
    TInt r =  SendReceive(EGetUserUsedDiskSpace, args) ;
    return (r);
}


