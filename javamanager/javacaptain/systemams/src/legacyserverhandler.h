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
* Description:  MLegacyServerHandler
*
*/

#ifndef LEGACYSERVERHANDLER_H
#define LEGACYSERVERHANDLER_H

const int SERVER_ID_PUBLIC_C = 1;
const int SERVER_ID_SECURITY_ADMIN_C = 2;

class MLegacyServerHandler
{
public:
    virtual void HandleMessageL(const int serverId,
                                const RMessage2& aMessage) const = 0;
};

#endif // LEGACYSERVERHANDLER_H
