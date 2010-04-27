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

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <securitydefs.h>
#else
#include <securitydefs.h>
#include <securitydefsconst.h>
#endif

#include "javacertstoretoken.h"
#include "javacertstoretokentype.h"
#include "logger.h"
#include "javacommonutils.h"

using namespace java::security;

//_LIT(KName, "MIDP2 Trust Roots");
_LIT(KName, "Java Trust Roots");

/**
 *
 */
CJavaCertStoreTokenType::CJavaCertStoreTokenType(TBool aIsInstantiatedByECom)
        : mIsInstantiatedByECom(aIsInstantiatedByECom)
{
}

/**
 *
 */
CCTTokenType* CJavaCertStoreTokenType::NewL(void)
{
    return new(ELeave) CJavaCertStoreTokenType(ETrue);
}

/**
 *
 */
CCTTokenType* CJavaCertStoreTokenType::New2L(void)
{
    CJavaCertStoreTokenType* impl = new(ELeave) CJavaCertStoreTokenType(EFalse);
    CleanupStack::PushL(impl);
    impl->ConstructL();
    CleanupStack::Pop(impl);
    return impl;
}

/**
 *
 */
void CJavaCertStoreTokenType::ConstructL()
{
    TInt err = mFs.Connect();
    if (0 != err)
    {
        std::string errTxt("Connection to RFs failed: ");
        errTxt.append(java::util::JavaCommonUtils::intToString(err));
        ELOG1(EJavaSecurity, "ERROR!!! %s",errTxt.c_str());
        User::Leave(err);
    }
    CCTTokenType::ConstructL(KMidletInstallApplicabilityUid,KName(),mFs);
}

/**
 *
 */
CJavaCertStoreTokenType::~CJavaCertStoreTokenType()
{
    if (!mIsInstantiatedByECom)
    {
        mFs.Close();
    }
}

/**
 *
 */
void CJavaCertStoreTokenType::List(RCPointerArray<HBufC>& aTokens,
                                   TRequestStatus& aStatus)
{
    aStatus = KRequestPending;

    HBufC* name = KName().Alloc();
    TInt status = KErrNone;
    if (name==NULL)
    {
        status = KErrNoMemory;
    }
    else
    {
        status = aTokens.Append(name);
        if (status != KErrNone)
        {
            delete name;
            name = NULL;
        }
    }
    TRequestStatus* localStatus = &aStatus;
    User::RequestComplete(localStatus, status);
}

void CJavaCertStoreTokenType::CancelList()
{
}

void CJavaCertStoreTokenType::OpenToken(const TDesC& aTokenInfo,
                                        MCTToken*& aToken, TRequestStatus& aStatus)
{
    TInt status = KErrNotSupported;

    if (aTokenInfo == KName)
    {
        aToken = CJavaCertStoreToken::New(0x1234, *this);
        if (aToken==NULL)
        {
            status = KErrNoMemory;
        }
        else
        {
            status = KErrNone;
            IncReferenceCount();
        }
    }
    TRequestStatus* localStatus = &aStatus;
    User::RequestComplete(localStatus, status);
}

void CJavaCertStoreTokenType::OpenToken(
    TCTTokenHandle  /* aHandle */,
    MCTToken*&      /* aToken  */,
    TRequestStatus& aRequestStatus)
{
    aRequestStatus = KRequestPending;
    TRequestStatus* pRequestStatus = &aRequestStatus;
    User::RequestComplete(pRequestStatus, KErrNotSupported);
}

void CJavaCertStoreTokenType::CancelOpenToken()
{
}

TUid CJavaCertStoreTokenType::Type() const
{
    //Uid of the old unicertstoreplugin must be provided because CertManUI
    //has special functionality for java unicertstore plugin. CertManUI detects
    //java unicertstore plugin by uid.
    return TUid::Uid(0x101F5B71);
}

