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
* Description:  CHttpsCertInfo
*
*/


#include <x500dn.h>
#include <ssl.h>
#include "gcf_constants.h"
#include "chttpscertinfo.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <x520ava.h>
#endif

_LIT(KX509certificateTypeDescriptor, "X.509");

_LIT(KMD5RSAAlgId,  "md5WithRSAEncryption");
_LIT(KMD2RSAAlgId,  "md2WithRSAEncryption");
_LIT(KSHA1RSAAlgId, "sha-1WithRSAEncryption");
_LIT(KSHA1DSAAlgId, "id-dsa-with-sha1");

_LIT(KEqualsSign,"=");
_LIT(KSeparator,";");

_LIT(KJavaX520CountryName,"C");
_LIT(KJavaX520OrganizationName,"O");
_LIT(KJavaX520OrganizationalUnitName,"OU");
_LIT(KJavaX520LocalityName,"L");
_LIT(KJavaX520CommonName,"CN");
_LIT(KJavaX520StateOrProvinceName,"ST");
_LIT(KJavaX520Surname,"SN");
_LIT(KJavaPKCS9EmailAddress, "EmailAddress");

// 5 * KEqualsSign + 4 * KSeparator + length of each KJavaX520 constants
// + max length of their content (see <x520ava.h>) + 1 padding

#define MAX_DN_LENGTH   (5 * KEqualsSign().Length() + 4 * KSeparator().Length() + KJavaX520CountryName().Length() + KX520MaxCLength + KJavaX520OrganizationName().Length() + KX520MaxOLength + KJavaX520OrganizationalUnitName().Length() + KX520MaxOULength + KJavaX520LocalityName().Length() + KX520MaxLLength + KJavaX520CommonName().Length() + KX520MaxCNLength + 1)

/**
 * KHexaTable translate a number into ascii code for corresponding hexadecimal symbol
 * {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'}
*/
const TUint16 KHexaTable[] = {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 65, 66, 67, 68, 69, 70};


const TUint16 KColonAsciiCode = 58;

EXPORT_C CHttpsCertInfo* CHttpsCertInfo::NewL(const CX509Certificate& acertInfo, HBufC* aCipherSuite)
{
    CHttpsCertInfo* me = new(ELeave) CHttpsCertInfo(acertInfo,aCipherSuite);
    CleanupStack::PushL(me);
    me->ConstructL();
    CleanupStack::Pop(me);
    return me;
}



CHttpsCertInfo::CHttpsCertInfo(const CX509Certificate& acertInfo, HBufC* aCipherSuite)
        :iCertInfo(acertInfo), iCipherSuite(*aCipherSuite)
{}

EXPORT_C CHttpsCertInfo::~CHttpsCertInfo()
{
    delete iSubject;
    delete iIssuer;
}

void CHttpsCertInfo::ConstructL()
{}


HBufC* CHttpsCertInfo::SerializeDistinguishedNameLC(const CX500DistinguishedName& aDN)
{
    HBufC* result = HBufC::NewLC(MAX_DN_LENGTH);
    TPtr   ptr = result->Des();
    TInt   count = aDN.Count();

    for (TInt index = count -1 ; index >= 0 ; --index)
    {
        const CX520AttributeTypeAndValue& element = aDN.Element(index);
        // get the element type
        const TPtrC type = element.Type();

        // map the type to a string rep
        if (type == KX520CountryName)
        {
            ptr += KJavaX520CountryName;
        }
        else if (type == KX520OrganizationName)
        {
            ptr += KJavaX520OrganizationName;
        }
        else if (type == KX520OrganizationalUnitName)
        {
            ptr += KJavaX520OrganizationalUnitName;
        }
        else if (type == KX520LocalityName)
        {
            ptr += KJavaX520LocalityName;
        }
        else if (type == KX520StateOrProvinceName)
        {
            ptr += KJavaX520StateOrProvinceName;
        }
        else if (type == KX520CommonName)
        {
            ptr += KJavaX520CommonName;
        }
        else if (type == KX520Surname)
        {
            ptr += KJavaX520Surname;
        }
        else if (type == KPKCS9EmailAddress)
        {
            ptr += KJavaPKCS9EmailAddress;
        }
        else
        {
            // unsupported type
            continue;
        }
        ptr += KEqualsSign;

        HBufC* valbuf = element.ValueL();

        ptr += *valbuf;
        delete valbuf;

        if (index > 0)
        {
            ptr += KSeparator;
        }
    }
    return result;
}


void CHttpsCertInfo::GetSecurityInfoL(CDesCArray& aStringResult, TTime& aStartTime, TTime& aEndTime)
{
    aStartTime = iCertInfo.ValidityPeriod().Start();
    aEndTime = iCertInfo.ValidityPeriod().Finish();
    aStringResult.AppendL(iCipherSuite);
    aStringResult.AppendL(KMIDSecureProtocolName());
    aStringResult.AppendL(KMIDSecureProtocolVersion());
    HBufC* buffer = SerializeDistinguishedNameLC(iCertInfo.SubjectName());
    aStringResult.AppendL(*buffer);
    CleanupStack::PopAndDestroy(buffer);
    buffer = SerializeDistinguishedNameLC(iCertInfo.IssuerName());
    aStringResult.AppendL(*buffer);
    CleanupStack::PopAndDestroy(buffer);
    aStringResult.AppendL(KX509certificateTypeDescriptor());
    TBuf<10> versionText;
    versionText.AppendNum(iCertInfo.Version());
    aStringResult.AppendL(versionText);
    switch (iCertInfo.SigningAlgorithm().DigestAlgorithm().Algorithm())
    {
    case EMD2:
        if (iCertInfo.SigningAlgorithm().AsymmetricAlgorithm().Algorithm() == ERSA)
        {
            aStringResult.AppendL(KMD2RSAAlgId());
            break;
        }
        else
        {
            User::Leave(KErrNotSupported);
        }
    case EMD5:
        if (iCertInfo.SigningAlgorithm().AsymmetricAlgorithm().Algorithm() == ERSA)
        {
            aStringResult.AppendL(KMD5RSAAlgId());
            break;
        }
        else
        {
            User::Leave(KErrNotSupported);
        }
    case ESHA1:
        if (iCertInfo.SigningAlgorithm().AsymmetricAlgorithm().Algorithm() == ERSA)
        {
            aStringResult.AppendL(KSHA1RSAAlgId());
            break;
        }
        else if (iCertInfo.SigningAlgorithm().AsymmetricAlgorithm().Algorithm() == EDSA)
        {
            aStringResult.AppendL(KSHA1DSAAlgId());
            break;
        }
        //else fall through and Leave
    default:
        User::Leave(KErrNotSupported);
    }
    TPtrC8 number = iCertInfo.SerialNumber();
    buffer = HBufC::NewLC(number.Length() * 3);

    TPtr serialPtr = buffer->Des();
    for (TInt i = 0; i < number.Length() ; ++i)
    {
        TUint8 quotient = TUint8(number[i]/16);

        serialPtr.Append(KHexaTable[quotient]);
        serialPtr.Append(KHexaTable[number[i] - quotient*16]);
        if (i != number.Length() -1)
        {
            serialPtr.Append(KColonAsciiCode);
        }
    }
    aStringResult.AppendL(*buffer);
    CleanupStack::PopAndDestroy(buffer);
}

