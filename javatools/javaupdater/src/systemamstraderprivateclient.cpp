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
* Description:  systemamstraderprivateclient.cpp
*               Part of OMJ S60 environment updater process.
*               Used to close the old S60 java environment in a controlled way.
*
*/


#include "systemamstraderprivateclient.h"


// constants
_LIT(KSystemAMSTraderPrivateServerName, "!SystemAMSTrader.Private");
const TInt KShutDownJavaEnv = 3;


//Definition
TBool RSystemAMSTraderPrivateClient::ShutdownJavaEnv(TRequestStatus& aStatus)
{
    TVersion version;
    TInt err;
    err = CreateSession(KSystemAMSTraderPrivateServerName, version);
    if (err == KErrNone)
    {
        SendReceive(KShutDownJavaEnv, TIpcArgs(), aStatus);
    }
    return err == KErrNone;
}

