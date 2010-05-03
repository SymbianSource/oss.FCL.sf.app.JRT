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
* Description:  CLegacyServerSession
*
*/

#ifndef LEGACYSERVERSESSION_H
#define LEGACYSERVERSESSION_H

#include <e32base.h>

class CLegacyServer;

NONSHARABLE_CLASS(CLegacyServerSession): public CSession2
{
public:
    CLegacyServerSession(const CLegacyServer& aServer);
    virtual ~CLegacyServerSession();

    virtual void ServiceL(const RMessage2& aMessage);

private:
    const CLegacyServer&    mServer;
};

#endif // LEGACYSERVERSESSION_H
