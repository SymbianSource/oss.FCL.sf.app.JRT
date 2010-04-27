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
* Description:  Helper class to map Contacts Model fields
 *
*/


// CLASS HEADER
#include "cpimcontactfieldinfo.h"

// INTERNAL INCLUDES
#include "pimpanics.h"
#include "logger.h"

// EXTERNAL INCLUDES
#include <cntdef.hrh>
#include <cntitem.h>

// UNNAMED LOCAL NAMESPACE
namespace
{
// Voip field Phonebook default identifier
const TInt KPIMIntPbkDefaultFieldVoip = 0x101F8863;
// SMS field Phonebook default ideintifier
const TInt KPIMIntPbkDefaultFieldSms = 0x101f4cf1;

// PIM attribute vs Contacts Model field vCard map element size
const TInt KPIMFieldAttributeMapElementSize = 2;
// PIM attribute vs Contacts Model field vCard map map. This array
// maps PIM attributes to Contacts Model field vCard maps and vice versa
const TInt KPIMFieldAttributeMap[][KPIMFieldAttributeMapElementSize] =
{
    { EPIMContactAttrHome, KIntContactFieldVCardMapHOME },
    { EPIMContactAttrWork, KIntContactFieldVCardMapWORK },
    { EPIMContactAttrPreferred, KIntContactFieldVCardMapPREF },
    { EPIMContactAttrMobile, KIntContactFieldVCardMapCELL },
    { EPIMContactAttrPager, KIntContactFieldVCardMapPAGER },
    { EPIMContactAttrSms, KPIMIntPbkDefaultFieldSms },
    { EPIMContactAttrAsst, KIntContactFieldVCardMapAssistantTel },
    { EPIMContactAttrAuto, KIntContactFieldVCardMapCAR },
    { EPIMContactAttrExtVideoCall, KIntContactFieldVCardMapVIDEO },
    { EPIMContactAttrPreferred, KPIMIntPbkDefaultFieldVoip }
};

// PIM attribute vs. Contacts Model field vCard map map size
const TInt KPIMFieldAttributeMapSize = sizeof(KPIMFieldAttributeMap)
                                       / (sizeof(TInt) * KPIMFieldAttributeMapElementSize);
}

// ---------------------------------------------------------------------------
// CPIMContactFieldInfo::NewL
// Two-phase constructor
// ---------------------------------------------------------------------------
//
CPIMContactFieldInfo* CPIMContactFieldInfo::NewL()
{
    JELOG2(EPim);
    CPIMContactFieldInfo* self = CPIMContactFieldInfo::NewLC();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CPIMContactFieldInfo::NewLC
// Two-phase constructor
// ---------------------------------------------------------------------------
//
CPIMContactFieldInfo* CPIMContactFieldInfo::NewLC()
{
    JELOG2(EPim);
    CPIMContactFieldInfo* self = new(ELeave) CPIMContactFieldInfo;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// ---------------------------------------------------------------------------
// CPIMContactFieldInfo::~CPIMContactFieldInfo
// Destructor
// ---------------------------------------------------------------------------
//
CPIMContactFieldInfo::~CPIMContactFieldInfo()
{
    JELOG2(EPim);
    delete iFieldsInfo;
}

// ---------------------------------------------------------------------------
// CPIMContactFieldInfo::MatchContactField
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TPIMContactField CPIMContactFieldInfo::MatchContactField(
    TInt aContactsModelFieldType) const
{
    JELOG2(EPim);
    TInt count = iFieldsInfo->Count();
    for (TInt i = 0; i < count; i++)
    {
        const TPIMFieldInfo& fieldInfo = (*iFieldsInfo)[i];
        if (fieldInfo.iContactsModelField == aContactsModelFieldType)
        {
            return fieldInfo.iPIMField;
        }
    }
    return EPIMContactInvalidField; // Invalid field
}

// ---------------------------------------------------------------------------
// CPIMContactFieldInfo::MatchPIMField
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
CArrayFix<TInt>* CPIMContactFieldInfo::MatchPIMFieldL(
    TPIMContactField aContactField) const
{
    JELOG2(EPim);
    CArrayFix<TInt>* fieldArray = new(ELeave) CArrayFixFlat<TInt> (7);
    CleanupStack::PushL(fieldArray);
    TInt count = iFieldsInfo->Count();
    for (TInt i = 0; i < count; i++)
    {
        const TPIMFieldInfo& fieldInfo = (*iFieldsInfo)[i];
        if (fieldInfo.iPIMField == aContactField)
        {
            fieldArray->AppendL(fieldInfo.iContactsModelField);
        }
    }
    CleanupStack::Pop(fieldArray);
    return fieldArray;
}

// ---------------------------------------------------------------------------
// CPIMContactFieldInfo::MatchContactNameElement
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TInt CPIMContactFieldInfo::MatchContactArrayElement(
    TInt aContactsModelFieldType) const
{
    JELOG2(EPim);
    TInt count = iFieldsInfo->Count();
    for (TInt i = 0; i < count; i++)
    {
        const TPIMFieldInfo& fieldInfo = (*iFieldsInfo)[i];
        if (fieldInfo.iContactsModelField == aContactsModelFieldType)
        {
            return fieldInfo.iSubElement;
        }
    }
    return -1; // Invalid field
}

// ---------------------------------------------------------------------------
// CPIMContactFieldInfo::FieldAttributes
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TPIMAttribute CPIMContactFieldInfo::FieldAttributes(
    const CContentType& aContentType) const
{
    JELOG2(EPim);
    TPIMAttribute retVal = KPIMAttrNone;
    for (TInt i = 0; i < KPIMFieldAttributeMapSize; i++)
    {
        // Generate uid from the specified Contacts Model field id
        TUid uid =
            { KPIMFieldAttributeMap[i][1] };
        // Match if this content type contains certain PIM attribute
        if (aContentType.ContainsFieldType(uid))
        {
            retVal |= KPIMFieldAttributeMap[i][0];
        }
    }
    return retVal;
}

// ---------------------------------------------------------------------------
// CPIMContactFieldInfo::FieldInternalAttributes
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMContactFieldInfo::GetFieldInternalAttributesL(
    const CContentType& aContentType, CArrayFix<TUid>& aArrayOfAttributes) const
{
    JELOG2(EPim);
    TInt fieldCount = aContentType.FieldTypeCount();
    for (TInt i = 0; i < fieldCount; i++)
    {
        TFieldType fieldType = aContentType.FieldType(i);
        TBool isSupported = EFalse;
        for (TInt j = 0; j < KPIMFieldAttributeMapSize && !isSupported; j++)
        {
            // Generate uid from the specified Contacts Model field id
            TUid uid =
                { KPIMFieldAttributeMap[j][1] };

            // Add this attribute to internal attributes array if
            // it is not supported. This covers all unknow attributes
            // that those won't be lost when a database item is modified
            if (fieldType == uid)
            {
                isSupported = ETrue;
            }
        }
        // Check that if field info array knows this Uid
        TInt fieldInfoCount = iFieldsInfo->Count();
        for (TInt j = 0; j < fieldInfoCount && !isSupported; j++)
        {
            const TPIMFieldInfo& fieldInfo = (*iFieldsInfo)[j];
            // Contacts Model field type values are defined as field types
            // So the following mappings is equal to check with the field type
            TUid uid =
                { fieldInfo.iContactsModelField };
            if (fieldType == uid)
            {
                isSupported = ETrue;
            }
        }
        // Not supported, add to internal attributes
        if (!isSupported)
        {
            aArrayOfAttributes.AppendL(fieldType);
        }
    }
}

// ---------------------------------------------------------------------------
// CPIMContactFieldInfo::AddressContentTypeL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
CContentType* CPIMContactFieldInfo::AddressContentTypeL(
    TPIMContactAddrElement aElement, EContactFieldCategory aCategory) const
{
    JELOG2(EPim);
    CContentType* type = NULL;
    switch (aElement)
    {
    case EPIMContactAddrPoBox:
    {
        type = CContentType::NewL(KUidContactFieldPostOffice,
                                  KUidContactFieldVCardMapPOSTOFFICE);
        break;
    }
    case EPIMContactAddrExtra:
    {
        type = CContentType::NewL(KUidContactFieldExtendedAddress,
                                  KUidContactFieldVCardMapEXTENDEDADR);
        break;
    }
    case EPIMContactAddrStreet:
    {
        type = CContentType::NewL(KUidContactFieldAddress,
                                  KUidContactFieldVCardMapADR);
        break;
    }
    case EPIMContactAddrLocality:
    {
        type = CContentType::NewL(KUidContactFieldLocality,
                                  KUidContactFieldVCardMapLOCALITY);
        break;
    }
    case EPIMContactAddrRegion:
    {
        type = CContentType::NewL(KUidContactFieldRegion,
                                  KUidContactFieldVCardMapREGION);
        break;
    }
    case EPIMContactAddrPostalCode:
    {
        type = CContentType::NewL(KUidContactFieldPostcode,
                                  KUidContactFieldVCardMapPOSTCODE);
        break;
    }
    case EPIMContactAddrCountry:
    {
        type = CContentType::NewL(KUidContactFieldCountry,
                                  KUidContactFieldVCardMapCOUNTRY);
        break;
    }
    default:
    {
        User::Leave(KErrArgument);
    }
    }
    CleanupStack::PushL(type);

    // Add category
    if (EContactCategoryHome == aCategory)
    {
        type->AddFieldTypeL(KUidContactFieldVCardMapHOME);
    }
    else if (EContactCategoryWork == aCategory)
    {
        type->AddFieldTypeL(KUidContactFieldVCardMapWORK);
    }

    CleanupStack::Pop(type);
    return type;
}

// ---------------------------------------------------------------------------
// CPIMContactFieldInfo::NameContentTypeL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
CContentType* CPIMContactFieldInfo::NameContentTypeL(
    TPIMContactNameElement aElement) const
{
    JELOG2(EPim);
    CContentType* type = NULL;
    switch (aElement)
    {
    case EPIMContactNameFamily:
    {
        type = CContentType::NewL(KUidContactFieldFamilyName,
                                  KUidContactFieldVCardMapUnusedN);
        break;
    }
    case EPIMContactNameGiven:
    {
        type = CContentType::NewL(KUidContactFieldGivenName,
                                  KUidContactFieldVCardMapUnusedN);
        break;
    }
    case EPIMContactNameOther:
    {
        type = CContentType::NewL(KUidContactFieldAdditionalName,
                                  KUidContactFieldVCardMapUnusedN);
        break;
    }
    case EPIMContactNameSuffix:
    {
        type = CContentType::NewL(KUidContactFieldSuffixName,
                                  KUidContactFieldVCardMapUnusedN);
        break;
    }
    case EPIMContactNamePrefix:
    {
        type = CContentType::NewL(KUidContactFieldPrefixName,
                                  KUidContactFieldVCardMapUnusedN);
        break;
    }
    case EPIMContactExtFamilyNameReading:
    {
        type = CContentType::NewL(KUidContactFieldFamilyNamePronunciation,
                                  KUidContactFieldVCardMapUnusedN);
        break;
    }
    case EPIMContactExtGivenNameReading:
    {
        type = CContentType::NewL(KUidContactFieldGivenNamePronunciation,
                                  KUidContactFieldVCardMapUnusedN);
        break;
    }
    default:
    {
        User::Leave(KErrArgument);
    }
    }
    return type;
}

// ---------------------------------------------------------------------------
// CPIMContactFieldInfo::StringFieldContentTypeL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
CContentType* CPIMContactFieldInfo::StringFieldContentTypeL(
    const TPIMContactField aContactField, const TPIMAttribute aAttributes,
    EContactFieldCategory& aCategory) const
{
    JELOG2(EPim);
    CContentType* type = NULL;

    switch (aContactField)
    {
    case EPIMContactEmail:
    {
        type = CContentType::NewL(KUidContactFieldEMail,
                                  KUidContactFieldVCardMapEMAILINTERNET);
        CleanupStack::PushL(type);

        if (EContactCategoryHome == aCategory)
        {
            type->AddFieldTypeL(KUidContactFieldVCardMapHOME);
        }
        if (EContactCategoryWork == aCategory)
        {
            type->AddFieldTypeL(KUidContactFieldVCardMapWORK);
        }
        if ((aAttributes & EPIMContactAttrPreferred) != 0)
        {
            type->AddFieldTypeL(KUidContactFieldVCardMapPREF);
        }
        CleanupStack::Pop(type);
        break;
    }
    case EPIMContactNote:
    {
        type = CContentType::NewL(KUidContactFieldNote,
                                  KUidContactFieldVCardMapNOTE);
        aCategory = EContactCategoryOther;
        break;
    }
    case EPIMContactOrg:
    {
        type = CContentType::NewL(KUidContactFieldCompanyName,
                                  KUidContactFieldVCardMapORG);
        aCategory = EContactCategoryWork;
        break;
    }
    case EPIMContactTel:
    {
        // PIM API Contact.TEL field is mapped to either a
        // telephone or a fax field in Contacts Model according to
        // PIM API attributes
        if ((aAttributes & EPIMContactAttrFax) != 0)
        {
            type = CContentType::NewL(KUidContactFieldFax,
                                      KUidContactFieldVCardMapTEL);
        }
        else
        {
            type = CContentType::NewL(KUidContactFieldPhoneNumber,
                                      KUidContactFieldVCardMapTEL);
        }
        CleanupStack::PushL(type);

        // PIM API attributes are further mapped to details in
        // Contacts Model fields.
        // Fax, pager and mobile numbers are preferred in this order
        // (rarest first).
        if ((aAttributes & EPIMContactAttrFax) != 0)
        {
            type->AddFieldTypeL(KUidContactFieldVCardMapFAX);
        }
        else if ((aAttributes & EPIMContactAttrPager) != 0)
        {
            type->AddFieldTypeL(KUidContactFieldVCardMapPAGER);
            aCategory = EContactCategoryNone;
        }
        else if ((aAttributes & EPIMContactAttrMobile) != 0)
        {
            type->AddFieldTypeL(KUidContactFieldVCardMapCELL);
        }
        else if ((aAttributes & EPIMContactAttrAuto) != 0)
        {
            type->AddFieldTypeL(KUidContactFieldVCardMapCAR);
            aCategory = EContactCategoryNone;
        }
        else if ((aAttributes & EPIMContactAttrAsst) != 0)
        {
            type->AddFieldTypeL(KUidContactFieldVCardMapAssistantTel);
            // Assistant Phone is not mapped as a Telephone field in Pbk
            // so we have to set the mapping to match it
            type->SetMapping(KUidContactFieldVCardMapAssistantTel);
        }
        else if ((aAttributes & EPIMContactAttrExtVideoCall) != 0)
        {
            type ->AddFieldTypeL(KUidContactFieldVCardMapVIDEO);
        }
        else if ((aAttributes & EPIMContactAttrOther) != 0)
        {
            // Discard ATTR_OTHER because contacts model doesn't
            // provide anything where we could map this attribute
        }
        else
        {
            type->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
        }

        if (EContactCategoryHome == aCategory)
        {
            type->AddFieldTypeL(KUidContactFieldVCardMapHOME);
        }
        if (EContactCategoryWork == aCategory)
        {
            type->AddFieldTypeL(KUidContactFieldVCardMapWORK);
        }
        if ((aAttributes & EPIMContactAttrPreferred) != 0)
        {
            type->AddFieldTypeL(KUidContactFieldVCardMapPREF);
        }
        if ((aAttributes & EPIMContactAttrSms) != 0)
        {
            TUid uid =
                { KPIMIntPbkDefaultFieldSms };
            type->AddFieldTypeL(uid);
        }
        CleanupStack::Pop(type);
        break;
    }
    case EPIMContactTitle:
    {
        type = CContentType::NewL(KUidContactFieldJobTitle,
                                  KUidContactFieldVCardMapTITLE);
        aCategory = EContactCategoryWork;
        break;
    }
    case EPIMContactNickname:
    {
        type = CContentType::NewL(KUidContactFieldSecondName,
                                  KUidContactFieldVCardMapSECONDNAME);
        break;
    }
    case EPIMContactUrl:
    {
        type = CContentType::NewL(KUidContactFieldUrl,
                                  KUidContactFieldVCardMapURL);
        CleanupStack::PushL(type);
        if (EContactCategoryHome == aCategory)
        {
            type->AddFieldTypeL(KUidContactFieldVCardMapHOME);
        }
        if (EContactCategoryWork == aCategory)
        {
            type->AddFieldTypeL(KUidContactFieldVCardMapWORK);
        }
        CleanupStack::Pop(type);
        break;
    }
    case EPIMContactExtSip:
    {
        type = CContentType::NewL(KUidContactFieldSIPID,
                                  KUidContactFieldVCardMapSIPID);
        CleanupStack::PushL(type);
        // Phonebook needs this field to separate SIP field from VOIP field
        type->AddFieldTypeL(KUidContactFieldVCardMapSIPID);
        CleanupStack::Pop(type);
        break;
    }
    case EPIMContactExtDtmf:
    {
        // Note: DTMF is not imported or exported in vCards
        type = CContentType::NewL(KUidContactFieldDTMF,
                                  KUidContactFieldVCardMapUnknown);
        break;
    }
    case EPIMContactExtWvUserId:
    {
        type = CContentType::NewL(KUidContactFieldIMAddress,
                                  KUidContactFieldVCardMapWV);
        break;
    }
    case EPIMContactExtDepartment:
    {
        type = CContentType::NewL(KUidContactFieldDepartmentName,
                                  KUidContactFieldVCardMapDepartment);
        break;
    }
    case EPIMContactExtAssistantName:
    {
        type = CContentType::NewL(KUidContactFieldAssistant,
                                  KUidContactFieldVCardMapAssistant);
        break;
    }
    case EPIMContactExtChildren:
    {
        type = CContentType::NewL(KUidContactFieldChildren,
                                  KUidContactFieldVCardMapChildren);
        break;
    }
    case EPIMContactExtSpouse:
    {
        type = CContentType::NewL(KUidContactFieldSpouse,
                                  KUidContactFieldVCardMapSpouse);
        break;
    }
    case EPIMContactExtVoip:
    {
        type = CContentType::NewL(KUidContactFieldSIPID,
                                  KUidContactFieldVCardMapVOIP);
        CleanupStack::PushL(type);
        if (EContactCategoryHome == aCategory)
        {
            type->AddFieldTypeL(KUidContactFieldVCardMapHOME);
        }
        if (EContactCategoryWork == aCategory)
        {
            type->AddFieldTypeL(KUidContactFieldVCardMapWORK);
        }
        if ((aAttributes & EPIMContactAttrPreferred) != 0)
        {
            TUid uid =
                { KPIMIntPbkDefaultFieldVoip };
            type->AddFieldTypeL(uid);
        }
        CleanupStack::Pop(type);
        break;
    }
    case EPIMContactExtPTT:
    {
        type = CContentType::NewL(KUidContactFieldSIPID,
                                  KUidContactFieldVCardMapPOC);
        CleanupStack::PushL(type);
        // Phonebook needs this field to separate SIP field from PTT field
        type->AddFieldTypeL(KUidContactFieldVCardMapPOC);
        CleanupStack::Pop(type);
        break;
    }
    case EPIMContactExtSWIS:
    {
        type = CContentType::NewL(KUidContactFieldSIPID,
                                  KUidContactFieldVCardMapSWIS);
        CleanupStack::PushL(type);
        // Phonebook needs this field to separate SIP field from SWIS field
        type->AddFieldTypeL(KUidContactFieldVCardMapSWIS);
        CleanupStack::Pop(type);
        break;
    }
    default:
    {
        __ASSERT_DEBUG(EFalse, User::Panic(KPIMPanicCategory,
                                           EPIMPanicUnsupportedField));
    }
    }

    return type;
}

// ---------------------------------------------------------------------------
// CPIMContactFieldInfo::CPIMContactFieldInfo
// Default C++ constructor
// ---------------------------------------------------------------------------
//
inline CPIMContactFieldInfo::CPIMContactFieldInfo()
{
    JELOG2(EPim);
}

void CPIMContactFieldInfo::ConstructL()
{
    JELOG2(EPim);
    // 5 for array granularity
    iFieldsInfo = new(ELeave) CArrayFixFlat<TPIMFieldInfo> (5);

    // Add fields to the array
    // Name fields and sub-elements
    iFieldsInfo->AppendL(TPIMFieldInfo(KUidContactFieldFamilyNameValue,
                                       EPIMContactName, EPIMContactNameFamily));
    iFieldsInfo->AppendL(TPIMFieldInfo(KUidContactFieldGivenNameValue,
                                       EPIMContactName, EPIMContactNameGiven));
    iFieldsInfo->AppendL(TPIMFieldInfo(KUidContactFieldAdditionalNameValue,
                                       EPIMContactName, EPIMContactNameOther));
    iFieldsInfo->AppendL(TPIMFieldInfo(KUidContactFieldSuffixNameValue,
                                       EPIMContactName, EPIMContactNameSuffix));
    iFieldsInfo->AppendL(TPIMFieldInfo(KUidContactFieldPrefixNameValue,
                                       EPIMContactName, EPIMContactNamePrefix));
    iFieldsInfo->AppendL(TPIMFieldInfo(
                             KUidContactFieldFamilyNamePronunciationValue, EPIMContactName,
                             EPIMContactExtFamilyNameReading));
    iFieldsInfo->AppendL(TPIMFieldInfo(
                             KUidContactFieldGivenNamePronunciationValue, EPIMContactName,
                             EPIMContactExtGivenNameReading));

    iFieldsInfo->AppendL(TPIMFieldInfo(KUidContactFieldSecondNameValue,
                                       EPIMContactNickname));

    iFieldsInfo->AppendL(TPIMFieldInfo(KUidContactFieldPhoneNumberValue,
                                       EPIMContactTel));

    iFieldsInfo->AppendL(
        TPIMFieldInfo(KUidContactFieldFaxValue, EPIMContactTel));

    iFieldsInfo->AppendL(TPIMFieldInfo(KUidContactFieldEMailValue,
                                       EPIMContactEmail));

    iFieldsInfo->AppendL(
        TPIMFieldInfo(KUidContactFieldUrlValue, EPIMContactUrl));

    // Address fields and sub-elements
    iFieldsInfo->AppendL(TPIMFieldInfo(KUidContactFieldAddressValue,
                                       EPIMContactAddr, EPIMContactAddrStreet));
    iFieldsInfo->AppendL(TPIMFieldInfo(KUidContactFieldPostOfficeValue,
                                       EPIMContactAddr, EPIMContactAddrPoBox));
    iFieldsInfo->AppendL(TPIMFieldInfo(KUidContactFieldExtendedAddressValue,
                                       EPIMContactAddr, EPIMContactAddrExtra));
    iFieldsInfo->AppendL(TPIMFieldInfo(KUidContactFieldPostCodeValue,
                                       EPIMContactAddr, EPIMContactAddrPostalCode));
    iFieldsInfo->AppendL(TPIMFieldInfo(KUidContactFieldLocalityValue,
                                       EPIMContactAddr, EPIMContactAddrLocality));
    iFieldsInfo->AppendL(TPIMFieldInfo(KUidContactFieldRegionValue,
                                       EPIMContactAddr, EPIMContactAddrRegion));
    iFieldsInfo->AppendL(TPIMFieldInfo(KUidContactFieldCountryValue,
                                       EPIMContactAddr, EPIMContactAddrCountry));

    iFieldsInfo->AppendL(TPIMFieldInfo(KUidContactFieldCompanyNameValue,
                                       EPIMContactOrg));

    iFieldsInfo->AppendL(TPIMFieldInfo(KUidContactFieldNoteValue,
                                       EPIMContactNote));

    iFieldsInfo->AppendL(TPIMFieldInfo(KUidContactFieldJobTitleValue,
                                       EPIMContactTitle));
    // VOIP, SIP and PoC are mapped under the same Contacts Model field
    iFieldsInfo->AppendL(TPIMFieldInfo(KUidContactFieldSIPIDValue,
                                       EPIMContactExtSip));

    // PTT is recognized with KIntContactFieldVCardMapPOC and SIPID
    iFieldsInfo->AppendL(TPIMFieldInfo(KIntContactFieldVCardMapPOC,
                                       EPIMContactExtPTT));
    iFieldsInfo->AppendL(TPIMFieldInfo(KUidContactFieldSIPIDValue,
                                       EPIMContactExtPTT));

    // SWIS is recognized with KIntContactFieldVCardMapSWIS and SIPID
    iFieldsInfo->AppendL(TPIMFieldInfo(KIntContactFieldVCardMapSWIS,
                                       EPIMContactExtSWIS));
    iFieldsInfo->AppendL(TPIMFieldInfo(KUidContactFieldSIPIDValue,
                                       EPIMContactExtSWIS));

    // VOIP is recognized with KIntContactFieldVCardMapVOIP and SIPID
    iFieldsInfo->AppendL(TPIMFieldInfo(KIntContactFieldVCardMapVOIP,
                                       EPIMContactExtVoip));
    iFieldsInfo->AppendL(TPIMFieldInfo(KUidContactFieldSIPIDValue,
                                       EPIMContactExtVoip));

    iFieldsInfo->AppendL(TPIMFieldInfo(KUidContactFieldDTMFValue,
                                       EPIMContactExtDtmf));

    iFieldsInfo->AppendL(TPIMFieldInfo(KUidContactFieldIMAddressValue,
                                       EPIMContactExtWvUserId));

    iFieldsInfo->AppendL(TPIMFieldInfo(KUidContactFieldAssistantValue,
                                       EPIMContactExtAssistantName));

    iFieldsInfo->AppendL(TPIMFieldInfo(KUidContactFieldSpouseValue,
                                       EPIMContactExtSpouse));

    iFieldsInfo->AppendL(TPIMFieldInfo(KUidContactFieldChildrenValue,
                                       EPIMContactExtChildren));

    iFieldsInfo->AppendL(TPIMFieldInfo(KUidContactFieldDepartmentNameValue,
                                       EPIMContactExtDepartment));

    iFieldsInfo->AppendL(TPIMFieldInfo(KUidContactFieldAnniversaryValue,
                                       EPIMContactExtAnniversary));

    iFieldsInfo->AppendL(TPIMFieldInfo(KUidContactFieldBirthdayValue,
                                       EPIMContactBirthday));

    iFieldsInfo->AppendL(TPIMFieldInfo(KUidContactFieldClassValue,
                                       EPIMContactClass));

    iFieldsInfo->AppendL(TPIMFieldInfo(KUidContactFieldPictureValue,
                                       EPIMContactPhoto));
}

// End of file
