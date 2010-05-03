/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/
#include "clientsession.h"

NONSHARABLE_CLASS(RSystemAMSTraderPublicClient): public RSessionBase
{
public:

    TBool CanConnect(void);
};

_LIT(KSystemAMSTraderPublicServerName, "!SystemAMSTrader.Public");

TBool RSystemAMSTraderPublicClient::CanConnect(void)
{
    TVersion version;

    if (CreateSession(KSystemAMSTraderPublicServerName, version) != KErrNone)
    {
        return EFalse;
    }

    TInt status = SendReceive(1, TIpcArgs());

    Close();
    return status == KErrNone;
}

TInt RClientSession::Connect(const TDesC& aServerName)
{
    RSystemAMSTraderPublicClient client;

    return client.CanConnect() ? CreateSession(aServerName, TVersion()) : KErrNotFound;
}

void RClientSession::Count(TRequestStatus& aStatus, const TDesC& aProtectionDomain, TPckgBuf<TUint32>& aResult)
{
    SendReceive(1, TIpcArgs(&aProtectionDomain, &aResult), aStatus); //ETrustRootCount
}

void RClientSession::Info(TRequestStatus& aStatus, const TDesC& aProtectionDomain, TDes8& aResult)
{
    SendReceive(2, TIpcArgs(&aProtectionDomain, &aResult), aStatus); // ETrustRootInfo
}

void RClientSession::Certificate(TRequestStatus& aStatus, TUint32 aId, TDes8& aResult)
{
    SendReceive(3, TIpcArgs(aId, &aResult), aStatus); // ETrustRootInfoCertificate
}

void RClientSession::Delete(TRequestStatus& aStatus, TUint32 aId)
{
    SendReceive(4, TIpcArgs(aId), aStatus); // EDeleteTrustRoot
}

void RClientSession::Disable(TRequestStatus& aStatus, TUint32 aId)
{
    SendReceive(5, TIpcArgs(aId), aStatus); // EDisableTrustRoot
}

void RClientSession::Enable(TRequestStatus& aStatus, TUint32 aId)
{
    SendReceive(6, TIpcArgs(aId), aStatus); // EEnableTrustRoot
}
