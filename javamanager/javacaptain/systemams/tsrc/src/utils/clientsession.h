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
#include <e32base.h>

#ifndef CLIENTSESSION_H
#define CLIENTSESSION_H

class RClientSession: public RSessionBase
{
public:

    TInt Connect(const TDesC& aServer);

    void Count(TRequestStatus& aStatus, const TDesC& aProtectionDomain, TPckgBuf<TUint32>& aResult);
    void Info(TRequestStatus& aStatus, const TDesC& aProtectionDomain, TDes8& aResult);
    void Certificate(TRequestStatus& aStatus, TUint32 aId, TDes8& aResult);
    void Delete(TRequestStatus& aStatus, TUint32 aId);
    void Disable(TRequestStatus& aStatus, TUint32 aId);
    void Enable(TRequestStatus& aStatus, TUint32 aId);
};

#endif // CLIENTSESSION_H
