/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


// INCLUDES
#include <s32mem.h>
#include <appversion.h>
#include "javaregconverter.h"
#include "javaregproperty.h"
#include "logger.h"

using namespace Java::Manager::Registry;

// The size needed to write a TAppVersion in a descriptor
// 8 bit descriptor
const TInt KTAppVersionSize8 = 3 * sizeof(TInt32);
// 16 bit descriptor
const TInt KTAppVersionSize16 = 2 * KTAppVersionSize8;

// The size needed to write a TInt in a descriptor
// 8 bit descriptor
const TInt KTIntSize8 = sizeof(TInt32);
// 16 bit descriptor
const TInt KTIntSize16 = 2 * KTIntSize8;


// ============================ MEMBER FUNCTIONS ==============================

// ---------------------------------------------------------------------------
// JavaRegConverter::GetTAppVersion
// ---------------------------------------------------------------------------
//
TAppVersion JavaRegConverter::
GetTAppVersion(const TDesC& aValue, TAppVersion aDefault)
{
    TAppVersion result = aDefault;
    TRAPD(err, result = JavaRegConverter::GetTAppVersionL(aValue));
    LOG1(EJavaStorage, EInfo, "Conversion error, error code %d",  err);
    return result;
}

// ---------------------------------------------------------------------------
// JavaRegConverter::GetTAppVersionL
// ---------------------------------------------------------------------------
//
TAppVersion JavaRegConverter::GetTAppVersionL(const TDesC& aValue)
{
    TInt32 majorVersion;
    TInt32 minorVersion;
    TInt32 buildVersion;

    HBufC8* buf = HBufC8::NewLC(aValue.Length());
    TPtr8 ptr = buf->Des();
    ptr.Copy(aValue);
    RDesReadStream  stream(*buf);
    CleanupClosePushL(stream);
    stream >> majorVersion >> minorVersion >> buildVersion;
    CleanupStack::PopAndDestroy(&stream);

    CleanupStack::PopAndDestroy(buf);
    return TAppVersion(majorVersion, minorVersion, buildVersion);
}

// ---------------------------------------------------------------------------
// JavaRegConverter::StoreTAppVersionL
// ---------------------------------------------------------------------------
//
void JavaRegConverter::
StoreTAppVersionL(const TAppVersion& aValue, HBufC*& aDes)
{
    TBuf8<KTAppVersionSize8> versionBuf8;

    TInt32 majorVersion = aValue.iMajor;
    TInt32 minorVersion = aValue.iMinor;
    TInt32 buildVersion = aValue.iBuild;

    RDesWriteStream stream(versionBuf8);
    CleanupClosePushL(stream);

    stream << majorVersion << minorVersion << buildVersion;
    stream.CommitL();
    CleanupStack::PopAndDestroy(&stream);

    aDes = HBufC::NewL(KTAppVersionSize16);
    TPtr ptr = aDes->Des();
    ptr.Copy(versionBuf8);
}

// ---------------------------------------------------------------------------
// JavaRegConverter::GetTInt
// ---------------------------------------------------------------------------
//
TInt32 JavaRegConverter::GetTInt(const TDesC& aValue, TInt32 aDefault)
{
    TInt32 result = aDefault;
    TRAPD(err, result = JavaRegConverter::GetTIntL(aValue));
    LOG1(EJavaStorage, EInfo, "Conversion error, error code %d",  err);
    return result;
}

// ---------------------------------------------------------------------------
// JavaRegConverter::GetTIntL
// ---------------------------------------------------------------------------
//
TInt32 JavaRegConverter::GetTIntL(const TDesC& aValue)
{
    TInt32 intValue;

    HBufC8* buf = HBufC8::NewLC(aValue.Length());
    TPtr8 ptr = buf->Des();
    ptr.Copy(aValue);
    RDesReadStream  stream(*buf);
    CleanupClosePushL(stream);
    stream >> intValue;
    CleanupStack::PopAndDestroy(&stream);

    CleanupStack::PopAndDestroy(buf);
    return intValue;
}

// ---------------------------------------------------------------------------
// JavaRegConverter::StoreTIntL
// ---------------------------------------------------------------------------
//
void JavaRegConverter::StoreTIntL(const TInt32 aValue, HBufC*& aDes)
{
    TBuf8<KTIntSize8> intBuf8;

    TInt32 intValue = aValue;
    RDesWriteStream stream(intBuf8);
    CleanupClosePushL(stream);

    stream << intValue;
    stream.CommitL();
    CleanupStack::PopAndDestroy(&stream);

    aDes = HBufC::NewL(KTIntSize16);
    TPtr ptr = aDes->Des();
    ptr.Copy(intBuf8);
}

// ---------------------------------------------------------------------------
// JavaRegConverter::GetTBool
// ---------------------------------------------------------------------------
//
TBool JavaRegConverter::GetTBool(const TDesC& aValue, TBool aDefault)
{
    return JavaRegConverter::GetTInt(aValue, aDefault);
}

// ---------------------------------------------------------------------------
// JavaRegConverter::GetTBoolL
// ---------------------------------------------------------------------------
//
TBool JavaRegConverter::GetTBoolL(const TDesC& aValue)
{
    return JavaRegConverter::GetTIntL(aValue);
}

// ---------------------------------------------------------------------------
// JavaRegConverter::StoreTBoolL
// ---------------------------------------------------------------------------
//
void JavaRegConverter::StoreTBoolL(const TBool aValue, HBufC*& aDes)
{
    JavaRegConverter::StoreTIntL(aValue, aDes);
}

// ---------------------------------------------------------------------------
// JavaRegConverter::GetCertificateChainsL
// ---------------------------------------------------------------------------
//
void JavaRegConverter::GetCertificateChainsL
(const TDesC& aValue, RPointerArray<HBufC8>& aCertChains)
{
    TInt32 count;

    HBufC8* buf = HBufC8::NewLC(aValue.Length());
    TPtr8 ptr = buf->Des();
    ptr.Copy(aValue);

    RDesReadStream  stream(*buf);
    CleanupClosePushL(stream);

    stream >> count;
    for (TInt i=0; i < count; i++)
    {
        TInt32 size;
        stream >> size;
        HBufC8* cert = HBufC8::NewLC(size);
        TPtr8 certptr = cert->Des();
        stream.ReadL(certptr, size);
        aCertChains.AppendL(cert);
        CleanupStack::Pop(cert);
    }

    CleanupStack::PopAndDestroy(&stream);
    CleanupStack::PopAndDestroy(buf);
}

// ---------------------------------------------------------------------------
// JavaRegConverter::StoreCertificateChainsL
// ---------------------------------------------------------------------------
//
void JavaRegConverter::StoreCertificateChainsL(
    const RPointerArray<HBufC8>& aValue,
    HBufC*& aDes)
{
    TInt32 count = aValue.Count();

    TInt32 fullSize = sizeof(TInt32);   // sizeof( count )

    for (TInt i=0; i < count; i++)
    {
        // sizeof( descriptorsize ) + descriptorsize
        fullSize += sizeof(TInt32) + aValue[i]->Size();
    }

    HBufC8* certBuf8 = HBufC8::NewLC(fullSize);
    TPtr8 ptr8 = certBuf8->Des();
    RDesWriteStream stream(ptr8);
    CleanupClosePushL(stream);

    stream << count;

    for (TInt i=0; i < count; i++)
    {
        TInt32 size = aValue[i]->Size();
        stream <<  size;
        stream.WriteL(*(aValue[i]));
    }
    stream.CommitL();
    CleanupStack::PopAndDestroy(&stream);

    aDes = HBufC::NewL(2 * fullSize);
    TPtr ptr = aDes->Des();
    ptr.Copy(ptr8);

    CleanupStack::PopAndDestroy(certBuf8);
}


// ---------------------------------------------------------------------------
// JavaRegConverter::GetUnicodeDescriptorsL
// ---------------------------------------------------------------------------
//
void JavaRegConverter::GetUnicodeDescriptorsL(
    const TDesC& aValue,
    RPointerArray<HBufC>& aDescriptors)
{
    TInt32 count;

    HBufC8* buf = HBufC8::NewLC(aValue.Length());
    TPtr8 ptr = buf->Des();
    ptr.Copy(aValue);

    RDesReadStream  stream(*buf);
    CleanupClosePushL(stream);

    stream >> count;
    for (TInt i=0; i < count; i++)
    {
        TInt32 size;
        stream >> size;
        HBufC* desc = HBufC::NewLC(stream, size);
        aDescriptors.AppendL(desc);
        CleanupStack::Pop(desc);
    }

    CleanupStack::PopAndDestroy(&stream);
    CleanupStack::PopAndDestroy(buf);
}

// ---------------------------------------------------------------------------
// JavaRegConverter::StoreUnicodeDescriptorsL
// ---------------------------------------------------------------------------
//
void JavaRegConverter::StoreUnicodeDescriptorsL(
    const RPointerArray<HBufC>& aValue,
    HBufC*& aDes)
{
    TInt32 count = aValue.Count();

    TInt32 fullSize = sizeof(TInt32);   // sizeof( count )

    for (TInt i=0; i < count; i++)
    {
        // sizeof( descriptorsize ) + descriptorsize
        fullSize += sizeof(TInt32) + aValue[i]->Size();
    }

    HBufC8* certBuf8 = HBufC8::NewLC(fullSize);
    TPtr8 ptr8 = certBuf8->Des();
    RDesWriteStream stream(ptr8);
    CleanupClosePushL(stream);

    stream << count;

    for (TInt i=0; i < count; i++)
    {
        TInt32 size = aValue[i]->Size();
        stream <<  size << *(aValue[i]);
    }
    stream.CommitL();
    CleanupStack::PopAndDestroy(&stream);

    aDes = HBufC::NewL(2 * fullSize);
    TPtr ptr = aDes->Des();
    ptr.Copy(ptr8);

    CleanupStack::PopAndDestroy(certBuf8);
}
