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
* Description: Wrapper for low level Symbian IPC methods
*
*/


#include "logger.h"

#include "rcomms.h"
#include "common.h"

namespace java
{
namespace comms
{
const TInt MAX_MESSAGE_SLOTS = 20;

TInt RComms::Connect(const TDesC& aName)
{
    TInt rc = CreateSession(aName, TVersion(1,1,1), MAX_MESSAGE_SLOTS, EIpcSession_Sharable);

    if (rc)
    {
        ELOG2(EJavaComms, "%s failed, err = %d", __PRETTY_FUNCTION__, rc);
    }
    return rc;
}

void RComms::Disconnect()
{
    Close();
}

TInt RComms::Send(const TDesC8& aData)
{
    return SendReceive(ESend, TIpcArgs(&aData));
}

void RComms::Receive(RBuf8& aBuffer, TPckgBuf<TInt>& aRequiredSize, TRequestStatus& aStatus)
{
    TIpcArgs args;
    args.Set(0, &aBuffer);
    args.Set(1, &aRequiredSize);

    SendReceive(EReceive, args, aStatus);
}

void RComms::CancelReceive()
{
    SendReceive(ECancelReceive, TIpcArgs());
}

} // namespace comms
} // namespace java
