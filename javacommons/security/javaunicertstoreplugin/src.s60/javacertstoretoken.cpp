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

#include <mctcertstore.h>
#include <mctwritablecertstore.h>

#include "javacertstoreimpl.h"
#include "javacertstoretokentype.h"
#include "javacertstoretoken.h"
#include "logger.h"

using namespace java::security;

CJavaCertStoreToken* CJavaCertStoreToken::New(TInt aId, CJavaCertStoreTokenType& aType)
{
    return new CJavaCertStoreToken(aId,aType);
}

CJavaCertStoreToken::CJavaCertStoreToken(TInt aId,CJavaCertStoreTokenType& aType)
        : iType(aType),iId(aId),iRefCount(0)
{
}

CJavaCertStoreToken::~CJavaCertStoreToken()
{
}

MCTTokenType& CJavaCertStoreToken::TokenType()
{
    return iType;
}

_LIT(KDummy1, "++Dummy1++");

const TDesC& CJavaCertStoreToken::Label()
{
    return KDummy1();
}

TCTTokenHandle CJavaCertStoreToken::Handle()
{
    return TCTTokenHandle(iType.Type(),iId);
}

TInt& CJavaCertStoreToken::ReferenceCount()
{
    return iRefCount;
}

void CJavaCertStoreToken::DoGetInterface(TUid aRequiredInterface,
        MCTTokenInterface*& aReturnedInterface,
        TRequestStatus& aStatus)
{

    aStatus = KRequestPending;

    TInt status = KErrNotSupported;
    aReturnedInterface = NULL;
    MCTTokenInterface* tmpObj = NULL;
    if (KInterfaceCertStore == aRequiredInterface.iUid)
    {
        status = KErrNone;
        TRAP(status, tmpObj = CJavaCertStoreImpl::NewL(*this));
    }
    else if (KInterfaceWritableCertStore == aRequiredInterface.iUid)
    {
        status = KErrNone;
        TRAP(status, tmpObj = CJavaCertStoreImpl::NewL(*this));
    }
    else
    {
        Release();
    }
    if (KErrNone == status)
    {
        aReturnedInterface = tmpObj;
    }
    //aStatus = KRequestPending;
    TRequestStatus* pStatus = &aStatus;
    User::RequestComplete(pStatus,status);
}

TBool CJavaCertStoreToken::DoCancelGetInterface()
{
    return EFalse;
}

_LIT(KDummy2, "++Dummy2++");

const TDesC& CJavaCertStoreToken::Information(TTokenInformation)
{
    return KDummy2();
}



