/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  javaregproperty implementation
*              : (__JAVA_EXCLUDED VERSION)
*
*/


#include "javaregproperty.h"

using namespace Java::Manager::Registry;

// ============================ MEMBER FUNCTIONS ==============================

// ---------------------------------------------------------------------------
// CJavaProperty::NewL
// ---------------------------------------------------------------------------

EXPORT_C CJavaProperty* CJavaProperty::NewL(TInt32 /* aId */,
        const TDesC&  /* aValue */)
{
    User::Leave(KErrNotSupported);
    return NULL;
}

// ---------------------------------------------------------------------------
// CJavaProperty::NewLC
// ---------------------------------------------------------------------------
EXPORT_C CJavaProperty* CJavaProperty::NewLC(TInt32 /* aId */,
        const TDesC& /* aValue */)
{
    User::Leave(KErrNotSupported);
    return NULL;
}

// ---------------------------------------------------------------------------
// CJavaProperty::~CJavaProperty
// ---------------------------------------------------------------------------
EXPORT_C CJavaProperty::~CJavaProperty()
{
}

// ---------------------------------------------------------------------------
// CJavaProperty::Id
// ---------------------------------------------------------------------------
EXPORT_C TInt32 CJavaProperty::Id() const
{
    return 0;
}

// ---------------------------------------------------------------------------
// CJavaProperty::Value
// ---------------------------------------------------------------------------
EXPORT_C const TDesC& CJavaProperty::Value() const
{
    return KNullDesC;
}

// ---------------------------------------------------------------------------
// operator==
// ---------------------------------------------------------------------------
EXPORT_C TBool CJavaProperty::operator==(const CJavaProperty& /* aProp */)
{
    return EFalse;
}
