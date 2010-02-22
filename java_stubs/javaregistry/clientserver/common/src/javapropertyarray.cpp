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
* Description:  javapropertyarray implementation
*              : (__JAVA_EXCLUDED VERSION)
*
*/


#include "javapropertyarray.h"

using namespace Java::Manager::Registry;

// ---------------------------------------------------------------------------
// CJavaPropertyArray::~CJavaPropertyArray
// ---------------------------------------------------------------------------
CJavaPropertyArray::~CJavaPropertyArray()
{
}

// ---------------------------------------------------------------------------
// CJavaPropertyArray::InternalizeL
// ---------------------------------------------------------------------------
void CJavaPropertyArray::InternalizeL(RReadStream&  /* aStream */)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CJavaPropertyArray::ExternalizeL
// ---------------------------------------------------------------------------
void CJavaPropertyArray::ExternalizeL(RWriteStream&  /* aStream */) const
{
    User::Leave(KErrNotSupported);
}
