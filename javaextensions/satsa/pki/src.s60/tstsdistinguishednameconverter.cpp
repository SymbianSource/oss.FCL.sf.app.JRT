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
* Description:
*
*/



// INCLUDE FILES
#include "tstsdistinguishednameconverter.h"
#include "tstscharactersetconverter.h"
#include "stscommon.h" // for CleanupResetAndDestroy
#include "stspkiconstants.h" // for error constants
#include <x500dn.h>
#include <charconv.h>




// CONSTANTS
const TInt KSTSGeneratedNameLength = 23;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TSTSDistinguishedNameConverter::TSTSDistinguishedNameConverter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TSTSDistinguishedNameConverter::TSTSDistinguishedNameConverter()
{
}

// -----------------------------------------------------------------------------
// TSTSDistinguishedNameConverter::CreateDNL
// Create CX500DistinguishedName
// -----------------------------------------------------------------------------
CX500DistinguishedName* TSTSDistinguishedNameConverter::CreateDNL(
    const TDesC& aNameInfo)
{

    CArrayPtrFlat< CX520AttributeTypeAndValue>* elements =
        new(ELeave) CArrayPtrFlat<CX520AttributeTypeAndValue> (1);
    CleanupStack::PushL(elements);
    CleanupResetAndDestroyPushL(*elements);
    TInt pos(0);
    TInt nameLength(aNameInfo.Length());
    TInt elementStart(0);

    while (pos < nameLength)
    {
        if (aNameInfo[ pos ] == '\\')
        {
            // next character is escaped, so we jump over it
            pos++;
        }
        else if ((aNameInfo[ pos ] == ',') || (pos == (nameLength - 1)))
        {
            // found the end of single element, parse it
            TInt elementLength = pos - elementStart;
            if (pos == (nameLength - 1))
            {
                elementLength++;
            }
            TPtrC elementDes(aNameInfo.Mid(elementStart, elementLength));
            CX520AttributeTypeAndValue* element =
                ParseDNElementL(elementDes);
            if (element)
            {
                CleanupStack::PushL(element);
                elements->AppendL(element);
                CleanupStack::Pop();  // element
            }
            elementStart = pos + 1;
        }
        pos++;
    }

    TInt elementCount = elements->Count();

    if (elementCount == 0)
    {
        CX520AttributeTypeAndValue* element = GenerateDNElementLC();
        elements->AppendL(element);
        CleanupStack::Pop();  // element
        elementCount++;
    }

    // In certificates the element order is reversed
    CArrayPtrFlat< CX520AttributeTypeAndValue>* reversedElements =
        new(ELeave) CArrayPtrFlat<CX520AttributeTypeAndValue> (elementCount);
    CleanupStack::PushL(reversedElements);

    for (TInt i = elementCount - 1; i >= 0; i--)
    {
        reversedElements->AppendL(elements->At(i));
    }

    CX500DistinguishedName* dn =
        CX500DistinguishedName::NewL(*reversedElements);

    CleanupStack::PopAndDestroy(3);   // elements, reversedElements


    return dn;
}

// -----------------------------------------------------------------------------
// TSTSDistinguishedNameConverter::GenerateDNElementL
// Generates a pseudo random distinguished name element
// -----------------------------------------------------------------------------
CX520AttributeTypeAndValue*
TSTSDistinguishedNameConverter::GenerateDNElementLC()
{
    TTime currentTime;
    currentTime.UniversalTime();
    TBuf< KSTSGeneratedNameLength > value;
    currentTime.FormatL(value, KSTSDistinguishedNameFormat());

    TSTSCharacterSetConverter csConverter;
    HBufC8* byteValue = csConverter.EscapedUnicodeToPrintableLC(value);

    CX520AttributeTypeAndValue* retVal =
        CX520AttributeTypeAndValue::NewL(ECommonName, *byteValue);

    CleanupStack::PopAndDestroy(byteValue);
    CleanupStack::PushL(retVal);

    return retVal;
}

// -----------------------------------------------------------------------------
// TSTSDistinguishedNameConverter::ParseDNElementLC
// Parse single element from distinguished name
// -----------------------------------------------------------------------------
CX520AttributeTypeAndValue* TSTSDistinguishedNameConverter::ParseDNElementL(
    TDesC& aSingleElement)
{

    TInt elementLength = aSingleElement.Length();
    TInt pos(0);

    CX520AttributeTypeAndValue* retVal = NULL;

    while (pos < elementLength)
    {
        if (aSingleElement[ pos ] == '=')
        {
            TPtrC type = aSingleElement.Left(pos);
            TPtrC value = aSingleElement.Mid(pos + 1);
            retVal = CreateDNElementL(type, value);
            // break in order to get quickly out of the while loop
            break;
        }
        pos++;
    }
    if (pos == elementLength)
    {
        User::Leave(KSTSErrInvalidCAName);
    }

    return retVal;
}


// -----------------------------------------------------------------------------
// TSTSDistinguishedNameConverter::CreateDNElementLC
// Create a single DN element from given parameters
// -----------------------------------------------------------------------------
CX520AttributeTypeAndValue* TSTSDistinguishedNameConverter::CreateDNElementL(
    TDesC& aType, TDesC& aValue)
{

    TLex typeLexer(aType);
    typeLexer.SkipSpace();
    TPtrC16 lexedType = typeLexer.Remainder();

    TSTSCharacterSetConverter csConverter;

    HBufC8* value = csConverter.EscapedUnicodeToPrintableLC(aValue);

    CX520AttributeTypeAndValue* retVal = NULL;
    if (lexedType.Compare(KSTSX500AttributeCN()) == 0)
    {
        retVal = CX520AttributeTypeAndValue::NewL(ECommonName, *value);
    }
    else if (lexedType.Compare(KSTSX500AttributeL()) == 0)
    {
        retVal = CX520AttributeTypeAndValue::NewL(ELocalityName, *value);
    }
    else if (lexedType.Compare(KSTSX500AttributeST()) == 0)
    {
        retVal = CX520AttributeTypeAndValue::NewL(EStateOrProvinceName,
                 *value);
    }
    else if (lexedType.Compare(KSTSX500AttributeO()) == 0)
    {
        retVal = CX520AttributeTypeAndValue::NewL(EOrganizationName, *value);
    }
    else if (lexedType.Compare(KSTSX500AttributeOU()) == 0)
    {
        retVal = CX520AttributeTypeAndValue::NewL(EOrganizationalUnitName,
                 *value);
    }
    else if (lexedType.Compare(KSTSX500AttributeC()) == 0)
    {
        retVal = CX520AttributeTypeAndValue::NewL(ECountryName, *value);
    }
    else if (lexedType.Compare(KSTSX500AttributeSTREET()) == 0)
    {
        retVal = CX520AttributeTypeAndValue::NewL(ERFC2256Street, *value);
    }
    else if (lexedType.Compare(KSTSX500AttributeDC()) == 0)
    {
        retVal = CX520AttributeTypeAndValue::NewL(ERFC2247DomainComponent,
                 *value);
    }
    // UID is mentioned in RFC2253, but its not supported by native
    // implementation
    else if (lexedType.Compare(KSTSX500AttributeUID()) != 0)
    {
        User::Leave(KErrArgument);
    }
    CleanupStack::PopAndDestroy(value);


    return retVal;
}


//  End of File
