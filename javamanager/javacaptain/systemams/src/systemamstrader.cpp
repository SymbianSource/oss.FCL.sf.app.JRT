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


#include "logger.h"

#include "systemamstrader.h"

_LIT(KExecutableName, "javacaptain.exe");
_LIT(KIadArg, "iad");

using namespace java::security::legacysupport;

CSystemAMSTrader::CSystemAMSTrader(TBool aIadStart) :
        mIadStart(aIadStart)
{
    JELOG2(EJavaSystemAMS);
}

CSystemAMSTrader::~CSystemAMSTrader()
{
    JELOG2(EJavaSystemAMS);
    delete mCertificatesManager;
    mCertificatesManager = NULL;
}

void CSystemAMSTrader::StartL(TBool aIadStart)
{
    JELOG2(EJavaSystemAMS);

    CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;

    CleanupStack::PushL(scheduler);
    CActiveScheduler::Install(scheduler);

    CSystemAMSTrader* trader = new(ELeave) CSystemAMSTrader(aIadStart);
    CleanupStack::PushL(trader);
    trader->ConstructL();
    CleanupStack::Pop(trader);

    CActiveScheduler::Start();
    CleanupStack::PopAndDestroy(scheduler);
}


void CSystemAMSTrader::ConstructL(void)
{
    int st;
    RProcess proc;

    if (mIadStart)
    {
        st = proc.Create(KExecutableName, KIadArg);
    }
    else
    {
        st = proc.Create(KExecutableName, KExecutableName);
    }
    if (st == KErrNone)
    {
        proc.Resume();
        proc.Close();
    }

    RProcess::Rendezvous(st);

    mPublicServer = new(ELeave) CLegacyServer(SERVER_ID_PUBLIC_C, this);
    mPublicServer->StartL();

    mSecurityAdminServer = new(ELeave) CLegacyServer(
        SERVER_ID_SECURITY_ADMIN_C, this);
    mSecurityAdminServer->StartL();

    mCertificatesManager = CertificatesManager::NewL();
}

void CSystemAMSTrader::HandleMessageL(const int serverId,
                                      const RMessage2& aMessage) const
{
    JELOG2(EJavaSystemAMS);

    switch (serverId)
    {
    case SERVER_ID_PUBLIC_C:
        handlePublicServerRequests(aMessage);
        break;
    case SERVER_ID_SECURITY_ADMIN_C:
        handleSecurityAdminRequestsL(aMessage);
        break;
    }
}

void CSystemAMSTrader::handlePublicServerRequests(const RMessage2& aMessage) const
{
    JELOG2(EJavaSystemAMS);
    // CanConnect
    aMessage.Complete(KErrNone);
}

void CSystemAMSTrader::handleSecurityAdminRequestsL(const RMessage2& aMessage) const
{
    JELOG2(EJavaSystemAMS);
    TInt ret = KErrNone;
    HBufC* name = NULL;
    TPtr namePtr(0, 0);
    TPckgBuf<TUint32> trustRootCount(0);
    switch (aMessage.Function())
    {
    case 1: // ETrustRootCount - in: protection domain name; out: count trust root info[]
        LOG(EJavaSystemAMS, EInfo, "ETrustRootCount");
        name    = HBufC::NewLC(aMessage.GetDesLengthL(0));
        namePtr.Set(name->Des());
        aMessage.ReadL(0, namePtr);
        trustRootCount = TPckgBuf<TUint32>(mCertificatesManager->TrustRootCount(namePtr));
        aMessage.WriteL(1, trustRootCount);
        CleanupStack::PopAndDestroy(name);
        break;

    case 2: // ETrustRootInfo - in: protection domain name; out:
        LOG(EJavaSystemAMS, EInfo, "ETrustRootInfo");
        name    = HBufC::NewLC(aMessage.GetDesLengthL(0));
        namePtr.Set(name->Des());
        aMessage.ReadL(0, namePtr);
        aMessage.WriteL(1, mCertificatesManager->TrustRootInfo(namePtr));
        CleanupStack::PopAndDestroy(name);
        break;

    case 3: // ETrustRootInfoCertificate - in: trust root id; out: encoded certificate
        LOG(EJavaSystemAMS, EInfo, "ETrustRootInfoCertificate");
        TRAP(ret, aMessage.WriteL(1, mCertificatesManager->TrustRootInfoCertificateL(aMessage.Int0())));
        break;

    case 4: // EDeleteTrustRoot - in: trust root id
        LOG(EJavaSystemAMS, EInfo, "EDeleteTrustRoot");
        TRAP(ret, mCertificatesManager->DeleteTrustRootL(aMessage.Int0()));
        break;

    case 5: // EDisableTrustRoot - in: trust root id
        LOG(EJavaSystemAMS, EInfo, "EDisableTrustRoot");
        TRAP(ret, mCertificatesManager->DisableTrustRootL(aMessage.Int0()));
        break;

    case 6: // EEnableTrustRoot - in: trust root id
        LOG(EJavaSystemAMS, EInfo, "EEnableTrustRoot");
        TRAP(ret, mCertificatesManager->EnableTrustRootL(aMessage.Int0()));
        break;
    }

    aMessage.Complete(ret);
}
