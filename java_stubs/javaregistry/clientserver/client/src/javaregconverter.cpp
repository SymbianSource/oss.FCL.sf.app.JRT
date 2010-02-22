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
* Description:  javaregconverter implementation
*              : (__JAVA_EXCLUDED VERSION)
*
*/


// INCLUDES
#include "javaregconverter.h"
#include <appversion.h>

using namespace Java::Manager::Registry;

// ============================ MEMBER FUNCTIONS ==============================

// ---------------------------------------------------------------------------
// JavaRegConverter::GetTAppVersion
// ---------------------------------------------------------------------------
//
EXPORT_C TAppVersion JavaRegConverter::
GetTAppVersion(const TDesC& /* aValue */,
               TAppVersion /* aDefault */)
{
    return TAppVersion();
}

// ---------------------------------------------------------------------------
// JavaRegConverter::GetTAppVersionL
// ---------------------------------------------------------------------------
//
EXPORT_C TAppVersion JavaRegConverter::
GetTAppVersionL(const TDesC& /* aValue */)
{
    User::Leave(KErrNotSupported);
    return TAppVersion();
}

// ---------------------------------------------------------------------------
// JavaRegConverter::StoreTAppVersionL
// ---------------------------------------------------------------------------
//
EXPORT_C void JavaRegConverter::
StoreTAppVersionL(const TAppVersion& /* aValue */,
                  HBufC*& /* aDes */)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// JavaRegConverter::GetTInt
// ---------------------------------------------------------------------------
//
EXPORT_C TInt32 JavaRegConverter::
GetTInt(const TDesC& /* aValue */, TInt32 /* aDefault */)
{
    return 0;
}

// ---------------------------------------------------------------------------
// JavaRegConverter::GetTIntL
// ---------------------------------------------------------------------------
//
EXPORT_C TInt32 JavaRegConverter::GetTIntL(const TDesC& /* aValue */)
{
    User::Leave(KErrNotSupported);
    return 0;
}

// ---------------------------------------------------------------------------
// JavaRegConverter::StoreTIntL
// ---------------------------------------------------------------------------
//
EXPORT_C void JavaRegConverter::
StoreTIntL(const TInt32 /* aValue */, HBufC*& /* aDes */)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// JavaRegConverter::GetTBool
// ---------------------------------------------------------------------------
//
EXPORT_C TBool JavaRegConverter::
GetTBool(const TDesC& /* aValue */, TBool /* aDefault */)
{
    return EFalse;
}

// ---------------------------------------------------------------------------
// JavaRegConverter::GetTBoolL
// ---------------------------------------------------------------------------
//
EXPORT_C TBool JavaRegConverter::
GetTBoolL(const TDesC& /* aValue */)
{
    User::Leave(KErrNotSupported);
    return EFalse;
}

// ---------------------------------------------------------------------------
// JavaRegConverter::StoreTBoolL
// ---------------------------------------------------------------------------
//
EXPORT_C void JavaRegConverter::
StoreTBoolL(const TBool /* aValue */, HBufC*& /* aDes */)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// JavaRegConverter::GetCertificateChainsL
// ---------------------------------------------------------------------------
//
EXPORT_C void JavaRegConverter::
GetCertificateChainsL(const TDesC& /* aValue */,
                      RPointerArray<HBufC8>& /* aCertChains */)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// JavaRegConverter::StoreCertificateChainsL
// ---------------------------------------------------------------------------
//
EXPORT_C void JavaRegConverter::StoreCertificateChainsL(
    const RPointerArray<HBufC8>& /* aValue */,
    HBufC*& /* aDes */)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// JavaRegConverter::GetUnicodeDescriptorsL
// ---------------------------------------------------------------------------
//
EXPORT_C void JavaRegConverter::GetUnicodeDescriptorsL(
    const TDesC& /* aValue */,
    RPointerArray<HBufC>& /* aDescriptors */)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// JavaRegConverter::StoreUnicodeDescriptorsL
// ---------------------------------------------------------------------------
//
EXPORT_C void JavaRegConverter::StoreUnicodeDescriptorsL(
    const RPointerArray<HBufC>& /* aValue */,
    HBufC*& /* aDes */)
{
    User::Leave(KErrNotSupported);
}
