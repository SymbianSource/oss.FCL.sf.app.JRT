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

#ifndef LEGACYSERVER_H
#define LEGACYSERVER_H

#include <e32base.h>

#include "legacyserverhandler.h"

NONSHARABLE_CLASS(CLegacyServer): public CServer2
{
public:
    CLegacyServer(const int serverId,
    MLegacyServerHandler* aHandler);
    virtual ~CLegacyServer();

    void StartL();

    void HandleMessageL(const RMessage2& aMessage) const;

private:
    virtual CSession2* NewSessionL(const TVersion&, const RMessage2& aMessage) const;

    const int       mServerId;
    MLegacyServerHandler*   mHandler;
};

#endif // LEGACYSERVER_H
