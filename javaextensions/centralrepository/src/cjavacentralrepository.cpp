/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <centralrepository.h>  // CRepository

#include "cjavacentralrepository.h" // CJavaCentralRepository
#include "logger.h"
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CJavaCentralRepository::NewL
// ---------------------------------------------------------------------------
CJavaCentralRepository* CJavaCentralRepository::NewL(TUid aRepositoryUid)
{
    CJavaCentralRepository* self = CJavaCentralRepository::NewLC(aRepositoryUid);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CJavaCentralRepository::NewLC
// ---------------------------------------------------------------------------
CJavaCentralRepository* CJavaCentralRepository::NewLC(TUid aRepositoryUid)
{
    CJavaCentralRepository* self =
        new(ELeave) CJavaCentralRepository(aRepositoryUid);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// ---------------------------------------------------------------------------
// CJavaCentralRepository::~CJavaCentralRepository
// ---------------------------------------------------------------------------
CJavaCentralRepository::~CJavaCentralRepository()
{
    Close();
}

// ---------------------------------------------------------------------------
// CJavaCentralRepository::Close
// ---------------------------------------------------------------------------
void CJavaCentralRepository::Close()
{
    iRepositoryUid = TUid::Null();
    delete iRepository;
    iRepository = NULL;
}

// ---------------------------------------------------------------------------
// CJavaCentralRepository::CJavaCentralRepository
// ---------------------------------------------------------------------------
CJavaCentralRepository::CJavaCentralRepository(TUid aRepositoryUid)
{
    iRepositoryUid = aRepositoryUid;
}

// ---------------------------------------------------------------------------
// CJavaCentralRepository::ConstructL
// ---------------------------------------------------------------------------
void CJavaCentralRepository::ConstructL()
{
    iRepository = CRepository::NewL(iRepositoryUid);
}

/**
 * GetStringL
 */
HBufC* CJavaCentralRepository::GetStringL(TUint32 aKey)
{
    HBufC* value = HBufC::NewLC(NCentralRepositoryConstants::KMaxUnicodeStringLength);
    TPtr ptr(value->Des());
    GetL(aKey, ptr);

    //aValue = value;
    CleanupStack::Pop(value);
    return value;
}

// ---------------------------------------------------------------------------
// CJavaCentralRepository::Get
// ---------------------------------------------------------------------------
void CJavaCentralRepository::GetL(TUint32 aKey, TDes& aValue)
{
    User::LeaveIfError(iRepository->Get(aKey, aValue));
}

/**
 * GetIntL
 */
TInt CJavaCentralRepository::GetIntL(TUint32 aKey/*, TInt& aValue*/)
{
    TInt value(0);
    GetL(aKey, value);
    return value;
}

// ---------------------------------------------------------------------------
// CJavaCentralRepository::Get
// ---------------------------------------------------------------------------
void CJavaCentralRepository::GetL(TUint32 aKey, TInt& aValue)
{
    User::LeaveIfError(iRepository->Get(aKey, aValue));
}

// ---------------------------------------------------------------------------
// CJavaCentralRepository::Set
// ---------------------------------------------------------------------------
void CJavaCentralRepository::SetL(TUint32 aKey, TDesC& aValue)
{
    User::LeaveIfError(iRepository->Set(aKey, aValue));
}

// ---------------------------------------------------------------------------
// CJavaCentralRepository::Set
// ---------------------------------------------------------------------------
void CJavaCentralRepository::SetL(TUint32 aKey, TInt aValue)
{
    User::LeaveIfError(iRepository->Set(aKey, aValue));
}
