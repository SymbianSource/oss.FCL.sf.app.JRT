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
* Description:  CSystemAMSTrader
*
*/

#ifndef SYSTEMAMSTRADER_H
#define SYSTEMAMSTRADER_H

#include <e32base.h>

#include "legacyserver.h"
#include "certificatesmanager.h"

NONSHARABLE_CLASS(CSystemAMSTrader): public CBase,
        public MLegacyServerHandler
{
public:
    CSystemAMSTrader(TBool aIadStart);
    virtual ~CSystemAMSTrader();

    static void StartL(TBool aIadStart);
    void ConstructL(void);

    void HandleMessageL(const int serverId, const RMessage2& aMessage) const;

private:
    void handlePublicServerRequests(const RMessage2& aMessage) const;
    void handleSecurityAdminRequestsL(const RMessage2& aMessage) const;

    CLegacyServer*  mPublicServer;
    CLegacyServer*  mSecurityAdminServer;
    java::security::legacysupport::CertificatesManager* mCertificatesManager;
    TBool mIadStart;
};

#endif // SYSTEMAMSTRADER_H
