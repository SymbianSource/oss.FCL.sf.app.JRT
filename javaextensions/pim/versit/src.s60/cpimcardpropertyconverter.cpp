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
* Description:  Converts properties PIM <-> vCard
 *
*/


// CLASS HEADER
#include "cpimcardpropertyconverter.h"

// INTERNAL INCLUDES
#include "pimcommon.h"
#include "pimpanics.h"
#include "pimversit.h"
#include "cpimitem.h"
#include "cpimvcardparser.h"
#include "cpimcontactitem.h"
#include "cpimparserproperty.h"
#include "cpimcontactvalidator.h"
#include "cpimvcardparserparamarray.h"
#include "logger.h"

// EXTERNAL INCLUDES
#include <vcard.h>
#include <cntdef.h> // parameter literals
#include <tz.h>
#include <tzconverter.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPIMCardPropertyConverter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPIMCardPropertyConverter* CPIMCardPropertyConverter::NewL(
    const CPIMContactValidator& aValidator)
{
    JELOG2(EPim);
    CPIMCardPropertyConverter* self =
        new(ELeave) CPIMCardPropertyConverter(aValidator);

    return self;
}

// Destructor
CPIMCardPropertyConverter::~CPIMCardPropertyConverter()
{
    JELOG2(EPim);
}

// -----------------------------------------------------------------------------
// CPIMCardPropertyConverter::ConvertFieldL
// Inserts a Field from a PIM Contact Item to CParserVCard as a property.
// -----------------------------------------------------------------------------
//
void CPIMCardPropertyConverter::ConvertFieldL(const CPIMItem& aItem, // item to read the field from
        CParserVCard& aParser, // parser to insert the property to.
        TPIMContactField aField) // field to convert
{
    JELOG2(EPim);
    TInt valueCount = aItem.CountValuesL(aField);
    for (TInt i = 0; i < valueCount; i++)
    {
        switch (aField)
        {
        case EPIMContactAddr:
        {
            ConvertAddressFieldL(aItem, aParser, aField, i);
            break;
        }
        case EPIMContactBirthday:
        case EPIMContactRevision:
        case EPIMContactExtAnniversary:
        {
            ConvertDateFieldL(aItem, aParser, aField, i);
            break;
        }
        case EPIMContactEmail:
        case EPIMContactNote:
        case EPIMContactTel:
        case EPIMContactTitle:
        case EPIMContactUrl:
        case EPIMContactNickname:
        case EPIMContactExtSip:
        case EPIMContactExtDtmf:
        case EPIMContactExtWvUserId:
        case EPIMContactExtSpouse:
        case EPIMContactExtChildren:
        case EPIMContactExtAssistantName:
        case EPIMContactExtVoip:
        case EPIMContactExtPTT:
        case EPIMContactExtSWIS:
            // UID is required by VFX. Do not fix even though this is wrong
        case EPIMContactUid:
        {
            ConvertStringFieldL(aItem, aParser, aField, i);
            break;
        }
        case EPIMContactName:
        {
            ConvertNameFieldL(aItem, aParser, aField, i);
            break;
        }
        case EPIMContactOrg:
        case EPIMContactExtDepartment:
        {
            ConvertOrganisationalFieldsL(aItem, aParser, aField, i);
            break;
        }
        case EPIMContactPhoto:
        {
            ConvertPhotoFieldL(aItem, aParser, aField, i);
            break;
        }
        case EPIMContactClass:
        {
            ConvertClassFieldL(aItem, aParser, aField, i);
            break;
        }
        // PIM API vCard parser does not support converting formatted names
        // since the conversion is not unambiguous and an optional feature
        // for vCard parsers based on IMC vCard specification. Also, PIM
        // API internal specification version 4.3 does not allow
        // Contact.FORMATTED_NAME to be a supported fields in S60 implementations
        case EPIMContactFormattedName:
        case EPIMContactFormattedAddr:
        case EPIMContactPhotoUrl:
        case EPIMContactPublicKey:
        case EPIMContactPublicKeyString:
        default:
        {
            __ASSERT_DEBUG(EFalse, User::Panic(KPIMPanicCategory,
                                               EPIMPanicUnsupportedField));
            return;
        }
        }
    }
}

// -----------------------------------------------------------------------------
// CPIMCardPropertyConverter::ConvertPropertyL
// Inserts a proprety from a vCard to a PIM Contact Item as a field
// -----------------------------------------------------------------------------
//
CParserVCard* CPIMCardPropertyConverter::ConvertPropertyL(
    const CParserProperty& aProperty, // property to convert
    CPIMContactItem& aItem) // item to insert the field to
{
    JELOG2(EPim);
    TUid valueTypeUid = aProperty.Uid();
    TInt valueType = valueTypeUid.iUid;
    CParserVCard* retVal = NULL;

    // The following, rather ugly, cast makes it possible for us to access
    // the protected iArrayOfParams field.
    const CPIMParserProperty& property =
        static_cast<const CPIMParserProperty&>(aProperty);

    // CLASS field is converted before others
    if (aProperty.Name().CompareF(KVersitTokenClass) == 0)
    {
        ConvertClassPropertyL(property, aItem);
    }
    // If SOUND field holds parameter X-IRMC-N, it countains name reading fields
    else if ((aProperty.Name().CompareF(KVersitTokenSOUND) == 0)
             && (aProperty.Param(KVersitParam8NamePrn)))
    {
        ConvertNameReadingFieldL(property, aItem);
    }
    else
    {
        switch (valueType)
        {
        case KVersitPropertyBinaryUid:
        {
            ConvertPhotoPropertyL(property, aItem);
            break;
        }
        case KVersitPropertyCDesCArrayUid:
        {
            // name and address arrays
            ConvertCDesCArrayPropertyL(property, aItem);
            break;
        }
        case KVersitPropertyDateUid:
        case KVersitPropertyDateTimeUid:
        {
            // REV, BDAY, X-ANNIVERSARY
            ConvertDatePropertyL(property, aItem);
            break;
        }
        case KVersitPropertyHBufCUid:
        {
            ConvertStringPropertyL(property, aItem);
            break;
        }
        case KVCardPropertyAgentUid:
        {
            retVal = ConvertAgentPropertyL(property);
        }
        default:
        {
            // don't support, don't care
        }
        }
    }
    return retVal;
}

// -----------------------------------------------------------------------------
// CPIMCardPropertyConverter::CPIMCardPropertyConverter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPIMCardPropertyConverter::CPIMCardPropertyConverter(
    const CPIMContactValidator& aValidator) :
        iContactValidator(aValidator)
{
    JELOG2(EPim);
}

// -----------------------------------------------------------------------------
// CPIMCardPropertyConverter::ConvertAgentPropertyL
// Converts an agent property.
// -----------------------------------------------------------------------------
//
inline CParserVCard* CPIMCardPropertyConverter::ConvertAgentPropertyL(
    const CPIMParserProperty& aProperty) // property to convert
{
    JELOG2(EPim);
    CParserPropertyValueAgent* propertyValue =
        static_cast<CParserPropertyValueAgent*>(aProperty.Value());
    CParserVCard* value = propertyValue->Value();
    return value;
}

// -----------------------------------------------------------------------------
// CPIMCardPropertyConverter::AddParamHomeAndWorkL
// Adds parameters "HOME" and "WORK" if necessary to given property parameter
// array.
// -----------------------------------------------------------------------------
//
void CPIMCardPropertyConverter::AddParamHomeAndWorkL(TPIMAttribute aAttributes, // checked if it contains AttrHome or AttrWork
        CPIMVCardParserParamArray& aParamArray) // array to add the parameters to
{
    JELOG2(EPim);
    if ((aAttributes & EPIMContactAttrHome) != 0)
    {
        AddParserParameterL(aParamArray, KVersitParam8Home());
    }
    if ((aAttributes & EPIMContactAttrWork) != 0)
    {
        AddParserParameterL(aParamArray, KVersitParam8Work());
    }
}

// -----------------------------------------------------------------------------
// CPIMCardPropertyConverter::ConvertAddressFieldL
// Converts an address field from a PIM Contact Item to CParserVCard.
// The field is stored in the parser as a property.
// The address values are stored in CParserPropertyValue
// CParserProperty is created, given the value.
// Parameters are added, and finally the property is added to parser.
// -----------------------------------------------------------------------------
//
void CPIMCardPropertyConverter::ConvertAddressFieldL(const CPIMItem& aItem, // item to convert from
        CParserVCard& aParser, // parsers to write to
        TPIMContactField aField, // field to convert
        TInt aIndex) // index to the field
{
    JELOG2(EPim);
    // Add attributes if there are any
    TPIMAttribute attributes = aItem.getAttributes(aField, aIndex);
    CPIMVCardParserParamArray* paramArray =
        new(ELeave) CPIMVCardParserParamArray(3);
    CleanupStack::PushL(paramArray);
    AddParamHomeAndWorkL(attributes, *paramArray);

    const CDesCArray& itemAddrArray = aItem.GetStringArrayL(aField, aIndex);
    // Copy the array
    TInt elementCount = itemAddrArray.Count();
    CDesCArrayFlat* addrArray = new(ELeave) CDesCArrayFlat(elementCount);
    CleanupStack::PushL(addrArray);
    for (TInt i = 0; i < elementCount; i++)
    {
        if (itemAddrArray[i].Compare(KPIMNullArrayElement) == 0)
        {
            addrArray->AppendL(KNullDesC);
        }
        else
        {
            addrArray->AppendL(itemAddrArray[i]);
        }
    }

    CParserPropertyValue* propertyValue =
        new(ELeave) CParserPropertyValueCDesCArray(addrArray);
    // addrArray is now owned by propertyValue
    CleanupStack::Pop(addrArray);
    AddPropertyToParserL(propertyValue, KVersitTokenADR(), paramArray, aParser);
    // The paramArray and propertyValue are popped by the previous function call
}

// -----------------------------------------------------------------------------
// CPIMCardPropertyConverter::ConvertNameFieldL
// Converts a Name field from a PIM Contact Item to CParserVCard.
// The field is stored in the parser as a property.
// The name values are stored in CParserPropertyValue
// CParserProperty is created, given the value.
// Parameters are added, and finally the property is added to parser.
// -----------------------------------------------------------------------------
//
void CPIMCardPropertyConverter::ConvertNameFieldL(const CPIMItem& aItem, // item to convert from
        CParserVCard& aParser, // parsers to write to
        TPIMContactField aField, // field to convert
        TInt aIndex) // index to the field
{
    JELOG2(EPim);
    TBool readingFieldsPresent = EFalse;
    const CDesCArray& itemNameArray = aItem.GetStringArrayL(aField, aIndex);
    TInt elementCount = itemNameArray.Count();
    CDesCArray* readingArray = new(ELeave) CDesCArrayFlat(2);
    CleanupStack::PushL(readingArray);
    CDesCArray* nameArray = new(ELeave) CDesCArrayFlat(elementCount);
    CleanupStack::PushL(nameArray);

    // Names should be ordered as follows:
    // Family name, Given name, Middle name, Prefix, Suffix
    // so check TPIMContactNameElement that it is in the right order
    for (TInt i = 0; i < elementCount; i++)
    {
        // Skip extended name reading fields on here
        if (i == EPIMContactExtFamilyNameReading || i
                == EPIMContactExtGivenNameReading)
        {
            if (itemNameArray[i].Compare(KPIMNullArrayElement) != 0)
            {
                readingFieldsPresent = ETrue;
                readingArray->AppendL(itemNameArray[TPIMContactNameElement(i)]);
            }
            else
            {
                readingArray->AppendL(KNullDesC);
            }
        }
        // Replace null array elements with KNullDesC
        else if (itemNameArray[i].Compare(KPIMNullArrayElement) == 0)
        {
            nameArray->AppendL(KNullDesC);
        }
        else
        {
            nameArray->AppendL(itemNameArray[TPIMContactNameElement(i)]);
        }
    }
    CParserPropertyValue* propertyValue =
        new(ELeave) CParserPropertyValueCDesCArray(nameArray);
    // nameArray is now owned by propertyValue
    CleanupStack::Pop(nameArray);
    AddPropertyToParserL(propertyValue, KVersitTokenN(), NULL, aParser);
    // The paramArray and propertyValue are popped by the previous function call

    // Add name reading fields if they are present
    if (readingFieldsPresent)
    {
        // Create parameter array
        CPIMVCardParserParamArray* paramArray =
            new(ELeave) CPIMVCardParserParamArray(1);
        CleanupStack::PushL(paramArray);
        // Create pronunciation parameter and add it to the parameter array
        AddParserParameterL(*paramArray, KVersitParam8NamePrn());
        // Create a new proprety value from the reading names array
        CParserPropertyValue* readingPropValue =
            new(ELeave) CParserPropertyValueCDesCArray(readingArray);
        // nameArray is now owned by propertyValue
        CleanupStack::Pop(paramArray);
        CleanupStack::Pop(readingArray);
        CleanupStack::PushL(paramArray);
        AddPropertyToParserL(readingPropValue, KVersitTokenSOUND(), paramArray,
                             aParser);
        // The paramArray and propertyValue are popped
        // by the previous function call
    }
    else
    {
        // Clean the reading array
        CleanupStack::PopAndDestroy(readingArray);
    }
}

// -----------------------------------------------------------------------------
// CPIMCardPropertyConverter::ConvertCDesCArrayPropertyL
// Converts name and address arrays from a vCard to a PIM Contact Item
// -----------------------------------------------------------------------------
//
void CPIMCardPropertyConverter::ConvertCDesCArrayPropertyL(
    const CPIMParserProperty& aProperty, // property to convert
    CPIMContactItem& aItem) // item to insert the field to
{
    JELOG2(EPim);
    TPIMField field = aProperty.MatchContactField();

    if ((field != EPIMContactAddr) && (field != EPIMContactName) && (field
            != EPIMContactOrg) && (field != EPIMContactExtChildren))
    {
        // unsupported properties are silently discarded
        return;
    }

    CParserPropertyValueCDesCArray* propertyValue =
        static_cast<CParserPropertyValueCDesCArray*>(aProperty.Value());
    const CDesCArray* value = propertyValue->Value();

    // In ORG field, the Organizational name is the first item and the second
    // item is extended department name
    if (field == EPIMContactOrg)
    {
        TInt valueCount = value->Count();
        if (valueCount >= 1)
        {
            HBufC* orgValue = value->MdcaPoint(0).AllocLC();
            aItem.AddStringL(field, KPIMAttrNone, orgValue);
            CleanupStack::Pop(orgValue);
            // Get department from the array
            if (valueCount > 1)
            {
                HBufC* department = value->MdcaPoint(1).AllocLC();
                aItem.AddStringL(EPIMContactExtDepartment, KPIMAttrNone,
                                 department);
                CleanupStack::Pop(department);
            }
        }
        return;
    }
    // Children is imported as a CDesCArray so we have to convert it here
    if (field == EPIMContactExtChildren && value->Count() >= 1)
    {
        // We are only interested about the first value
        HBufC* children = value->MdcaPoint(0).AllocLC();
        aItem.AddStringL(field, KPIMAttrNone, children);
        CleanupStack::Pop(children);
        return;
    }

    // Copy the array
    TInt elementCount = value->Count();
    CDesCArrayFlat* itemArray = new(ELeave) CDesCArrayFlat(elementCount);
    CleanupStack::PushL(itemArray);
    TInt i;
    for (i = 0; i < elementCount; i++)
    {
        itemArray->AppendL((*value)[i]);
    }

    // If we need more elements, add nulls
    TInt itemElementCount = iContactValidator.NumElementsL(field);
    TPtrC nullArrayElement(KPIMNullArrayElement);
    for (; i < itemElementCount; i++)
    {
        itemArray->AppendL(nullArrayElement);
    }

    TPIMAttribute attributes = KPIMAttrNone;
    // Usually people only have one name. However, both PIM api and
    // vCards allow two names: Formatted name is not supported anymore since
    // converting it is not unambiguous and it must not be supported by the
    // PIM API internal specification version 4.3. IMC's vCard specification
    // states that Formatted name is an optional field for vCard parsers.
    if (field == EPIMContactName && aItem.CountValuesL(EPIMContactName) > 0)
    {
        // we already have a name, so we just change the current one
        // NOTE: There can be name reading fields before the name array
        // elements are added so we have take care of it
        const CDesCArray& nameArray = aItem.GetStringArrayL(EPIMContactName, 0);
        TPtrC name(nameArray.MdcaPoint(EPIMContactExtFamilyNameReading));
        if (name != KPIMNullArrayElement)
        {
            // Delete empty item and add existing item
            itemArray->Delete(EPIMContactExtFamilyNameReading);
            itemArray->InsertL(EPIMContactExtFamilyNameReading, name);
        }
        name.Set(nameArray.MdcaPoint(EPIMContactExtGivenNameReading));
        if (name != KPIMNullArrayElement)
        {
            // Delete empty item and add existing item
            itemArray->Delete(EPIMContactExtGivenNameReading);
            itemArray->InsertL(EPIMContactExtGivenNameReading, name);
        }

        aItem.SetStringArrayL(EPIMContactName, 0, attributes, itemArray);
        CleanupStack::Pop(); // itemArray now owned by aItem
        return;
    }
    else
    {
        // address can have attributes
        attributes = aProperty.MatchHomeAndWorkAttributes();
    }
    aItem.AddStringArrayL(field, attributes, itemArray);
    CleanupStack::Pop(); // itemArray now owned by aItem
}

// -----------------------------------------------------------------------------
// CPIMCardPropertyConverter::ConvertDateFieldL
// Converts a date field from a PIM Contact Item to a CParserVCard.
// -----------------------------------------------------------------------------
//
void CPIMCardPropertyConverter::ConvertDateFieldL(const CPIMItem& aItem, // item to convert from
        CParserVCard& aParser, // parser to insert the property to.
        TPIMContactField aField, // field to convert
        TInt aIndex) // index to the field
{
    JELOG2(EPim);
    TVersitDateTime* versitDateTime = NULL;
    CParserPropertyValue* propertyValue = NULL;
    TPtrC8 propertyName(KNullDesC8);

    // Handle fields separately
    switch (aField)
    {
    case EPIMContactExtAnniversary:
    case EPIMContactBirthday:
    {
        const TPIMDate date = aItem.GetDateL(aField, aIndex);
        // Convert times to local time since UTC time designator is not
        // supported in anniversary and birthday fields
        RTz tzServer;
        User::LeaveIfError(tzServer.Connect());
        CleanupClosePushL(tzServer);

        // Create timezone converter
        CTzConverter* converter = CTzConverter::NewL(tzServer);
        CleanupStack::PushL(converter);

        TTime localTime(date);
        User::LeaveIfError(converter->ConvertToLocalTime(localTime));
        CleanupStack::PopAndDestroy(2); // converter, tzServer

        versitDateTime
        = new(ELeave) TVersitDateTime(date.DateTime(), TVersitDateTime::EIsMachineLocal);

        // Set specific property name
        if (aField == EPIMContactBirthday)
        {
            propertyName.Set(KVersitTokenBDAY);
        }
        else if (aField == EPIMContactExtAnniversary)
        {
            propertyName.Set(KVersitTokenAnniversary);
        }
        CleanupDeletePushL(versitDateTime);
        // Create new date property value
        propertyValue = new(ELeave) CParserPropertyValueDate(versitDateTime);
        break;
    }
    case EPIMContactRevision:
    {
        TPIMDate date = aItem.LastModified();
        versitDateTime
        = new(ELeave) TVersitDateTime(date.DateTime(), TVersitDateTime::EIsUTC);
        propertyName.Set(KVersitTokenREV);
        CleanupDeletePushL(versitDateTime);
        // Create new date and time property value
        propertyValue
        = new(ELeave) CParserPropertyValueDateTime(versitDateTime);
        break;
    }
    default:
    {
        User::Panic(KPIMPanicCategory, EPIMPanicUnsupportedDateField);
        break;
    }
    }
    // versitDateTime is now owned by propertyValue
    CleanupStack::Pop(versitDateTime);
    AddPropertyToParserL(propertyValue, propertyName, NULL, aParser);
    // The paramArray and propertyValue are popped by the previous function call
}

// -----------------------------------------------------------------------------
// CPIMCardPropertyConverter::ConvertDatePropertyL
// Converts a birthday from a vCard to a PIM Contact Item
// -----------------------------------------------------------------------------
//
void CPIMCardPropertyConverter::ConvertDatePropertyL(
    const CPIMParserProperty& aProperty, // property to convert
    CPIMContactItem& aItem) // item to insert the field to
{
    JELOG2(EPim);
    TPIMField field = aProperty.MatchContactField();
    switch (field)
    {
    case EPIMContactExtAnniversary:
    case EPIMContactBirthday:
    case EPIMContactRevision:
    {
        // All date property values are equal (in some way)
        CParserPropertyValueDateTime* value =
            static_cast<CParserPropertyValueDateTime*>(aProperty.Value());
        // Get versit date time object. Both date time properties
        // return TVersitDateTime so we don't care about the actual object
        const TVersitDateTime* vDateTime = value->Value();
        // Add new date to PIM item
        TPIMDate date(vDateTime->iDateTime);
        aItem.AddDateL(field, KPIMAttrNone, date);
        break;
    }
    default:
    {
        // Discard other fields
    }
    }
}

// -----------------------------------------------------------------------------
// CPIMCardPropertyConverter::ConvertStringFieldL
// Converts a string field from a PIM Item to vCard
// -----------------------------------------------------------------------------
//
void CPIMCardPropertyConverter::ConvertStringFieldL(const CPIMItem& aItem, // item to convert from
        CParserVCard& aParser, // parser to write to
        TPIMContactField aField, // field to convert
        TInt aIndex) // index to the value in the field
{
    JELOG2(EPim);
    // We'll gather all property parameters (attributes) to an array
    // which we give to the property in construction. It seems to be
    // the only way to give a property multiple parameters of same type.
    TPIMAttribute attributes = aItem.getAttributes(aField, aIndex);
    // Assistant tel field is mapped for its own field in s60, so
    // it must not be associated with the standard TEL field in vCards
    if (aField == EPIMContactTel && (attributes & EPIMContactAttrAsst) != 0)
    {
        const TDesC& asstTel = aItem.GetStringL(aField, aIndex);
        CParserPropertyValue* propertyValue = CParserPropertyValueHBufC::NewL(
                                                  asstTel);
        AddPropertyToParserL(propertyValue, KPIMVersitTokenASSTTEL(), NULL,
                             aParser);
        // Needed cleanup stack cleanup is done within the function
        return;
    }
    CPIMVCardParserParamArray* paramArray =
        new(ELeave) CPIMVCardParserParamArray(3);
    CleanupStack::PushL(paramArray);

    TPtrC8 propertyName; // mapped from field identifier

    switch (aField)
    {
    case EPIMContactEmail:
    {
        propertyName.Set(KVersitTokenEMAIL);
        AddParamHomeAndWorkL(attributes, *paramArray);
        // Contacts Model and PIM support PREF attribute, how ever
        // vCard specification does not. Thus we don't add it.
        break;
    }
    case EPIMContactNote:
    {
        propertyName.Set(KVersitTokenNOTE);
        break;
    }
    case EPIMContactTel:
    {
        propertyName.Set(KVersitTokenTEL);
        if ((attributes & EPIMContactAttrFax) != 0)
        {
            AddParserParameterL(*paramArray, KVersitParam8Fax());
        }
        if ((attributes & EPIMContactAttrMobile) != 0)
        {
            AddParserParameterL(*paramArray, KVersitParam8Cell());
        }
        if ((attributes & EPIMContactAttrPager) != 0)
        {
            AddParserParameterL(*paramArray, KVersitParam8Pager());
        }
        if ((attributes & (EPIMContactAttrPager | EPIMContactAttrFax
                           | EPIMContactAttrSms | EPIMContactAttrAuto
                           | EPIMContactAttrExtVideoCall)) == 0)
        {
            AddParserParameterL(*paramArray, KVersitParam8Voice());
        }
        if ((attributes & (EPIMContactAttrPreferred)) != 0)
        {
            AddParserParameterL(*paramArray, KVersitParam8Pref());
        }
        if ((attributes & EPIMContactAttrAuto) != 0)
        {
            AddParserParameterL(*paramArray, KVersitParam8Car());
        }
        if ((attributes & EPIMContactAttrExtVideoCall) != 0)
        {
            AddParserParameterL(*paramArray, KVersitParam8Video());
        }
        // Contacts Model and PIM support SMS attribute, how ever
        // vCard specification does not. Thus we ignore it.
        // ATTR_OTHER is also ingored because Contacts Model doesn't
        // provide anything where we could map ATTR_OTHER

        // Add HOME or WORK after all other attributes
        AddParamHomeAndWorkL(attributes, *paramArray);
        break;
    }
    case EPIMContactTitle:
    {
        propertyName.Set(KVersitTokenTITLE);
        break;
    }
    case EPIMContactUrl:
    {
        propertyName.Set(KVersitTokenURL);
        AddParamHomeAndWorkL(attributes, *paramArray);
        break;
    }
    case EPIMContactNickname:
    {
        propertyName.Set(KPIMVersitTokenNICKNAME);
        break;
    }
    case EPIMContactExtDtmf:
    {
        propertyName.Set(KPIMVersitTokenDTMF);
        break;
    }
    case EPIMContactExtWvUserId:
    {
        propertyName.Set(KPIMVersitTokenWVID);
        break;
    }
    case EPIMContactExtSpouse:
    {
        propertyName.Set(KVersitTokenSpouse);
        break;
    }
    case EPIMContactExtChildren:
    {
        propertyName.Set(KVersitTokenChildren);
        break;
    }
    case EPIMContactExtAssistantName:
    {
        propertyName.Set(KVersitTokenAssistant);
        break;
    }
    case EPIMContactExtVoip:
    case EPIMContactExtSip:
    case EPIMContactExtPTT:
    case EPIMContactExtSWIS:
    {
        TPtrC8 paramName;
        propertyName.Set(KPIMVersitTokenSIP);
        // VOIP is mapped to a subfield of X-SIP
        if (aField == EPIMContactExtVoip)
        {
            paramName.Set(KPIMVersitTokenVoip);
        }
        // PTT is mapped to a subfield of X-SIP
        else if (aField == EPIMContactExtPTT)
        {
            paramName.Set(KPIMVersitTokenPoc);
        }
        // SWIS is mapped to a subfield of X-SIP
        else if (aField == EPIMContactExtSWIS)
        {
            paramName.Set(KPIMVersitTokenSwis);
        }
        if (aField != EPIMContactExtSip)
        {
            AddParserParameterL(*paramArray, paramName);
        }

        AddParamHomeAndWorkL(attributes, *paramArray);
        // Contacts Model and PIM support PREF attribute, how ever
        // vCard specification does not. Thus we don't add it.
        break;
    }
    case EPIMContactUid:
    {
        propertyName.Set(KVersitTokenUID);
        break;
    }
    default:
    {
        User::Panic(KPIMPanicCategory, EPIMPanicUnsupportedStringField);
    }
    }

    const TDesC& pimStringValue = aItem.GetStringL(aField, aIndex);

    CParserPropertyValue* propertyValue = CParserPropertyValueHBufC::NewL(
                                              pimStringValue);
    AddPropertyToParserL(propertyValue, propertyName, paramArray, aParser);
    // The paramArray and propertyValue are popped by the previous function call
}

// -----------------------------------------------------------------------------
// CPIMCardPropertyConverter::AddParserParameterL
// Adds new parser parameter to the parameter array
// -----------------------------------------------------------------------------
//
void CPIMCardPropertyConverter::AddParserParameterL(
    CPIMVCardParserParamArray& aArrayOfParams, const TPtrC8 aParamName)
{
    JELOG2(EPim);
    // Create a new parameter from the name
    CParserParam* newParam = CParserParam::NewL(aParamName, KNullDesC8);
    CleanupStack::PushL(newParam);
    // Add parameter to the param array
    aArrayOfParams.AppendL(newParam);
    // newParam is now owned by the parameter array
    CleanupStack::Pop(newParam);
}

// -----------------------------------------------------------------------------
// CPIMCardPropertyConverter::ConvertStringPropertyL
// Converts a string property from a vCard to a PIM Contact Item
// -----------------------------------------------------------------------------
//
void CPIMCardPropertyConverter::ConvertStringPropertyL(
    const CPIMParserProperty& aProperty, // property to convert
    CPIMContactItem& aItem) // item to insert the field to
{
    JELOG2(EPim);
    TPIMField field = aProperty.MatchContactField();
    TPIMAttribute attributes = KPIMAttrNone;

    if (field == KErrNotFound)
    {
        return;
    }

    CParserPropertyValueHBufC* propertyValue =
        static_cast<CParserPropertyValueHBufC*>(aProperty.Value());
    HBufC* value = propertyValue->TakeValueOwnership();
    CleanupStack::PushL(value);

    switch (field)
    {
    case EPIMContactUrl:
    case EPIMContactEmail:
        // vCard EMAIL does not support PREF
    {
        attributes = aProperty.MatchHomeAndWorkAttributes();
        break;
    }
    case EPIMContactTel:
    {
        // Assistant phone is not an attribute in TEL field
        // it must be handled separately
        if (aProperty.Name().CompareF(KPIMVersitTokenASSTTEL) == 0)
        {
            attributes = EPIMContactAttrAsst;
        }
        else
        {
            attributes = aProperty.MatchAllAttributes();
        }
        break;
    }
    case EPIMContactExtVoip:
    {
        // VOIP field can have parameters HOME or WORK
        attributes = aProperty.MatchHomeAndWorkAttributes();
        break;
    }
    default:
    {
        // no attributes
    }
    }
    aItem.AddStringL(field, attributes, value);
    CleanupStack::Pop(value);
}

// -----------------------------------------------------------------------------
// CPIMCardPropertyConverter::ConvertPhotoFieldL
// Converts a photo field from a PIM Contact Item to a vCard
// -----------------------------------------------------------------------------
//
void CPIMCardPropertyConverter::ConvertPhotoFieldL(const CPIMItem& aItem, // item to convert from
        CParserVCard& aParser, // parser to write to
        TPIMContactField aField, // field to convert
        TInt aIndex) // index to the field
{
    JELOG2(EPim);
    const CPIMByteArray& byteArray = aItem.GetBinaryRawL(aField, aIndex);

    const TUint8& byteRef = byteArray.At(0);
    const TUint8* bytePtr = &byteRef;
    TPtrC8 data(bytePtr, byteArray.Count());

    CParserPropertyValue* propertyValue =
        CParserPropertyValueBinary::NewL(data);
    AddPropertyToParserL(propertyValue, KVersitTokenPHOTO(), NULL, aParser);
    // The paramArray and propertyValue are popped by the previous function call
}

// -----------------------------------------------------------------------------
// CPIMCardPropertyConverter::ConvertPhotoPropertyL
// Converts a photo field from a vCard to a PIM Contact Item
// -----------------------------------------------------------------------------
//
void CPIMCardPropertyConverter::ConvertPhotoPropertyL(
    const CPIMParserProperty& aProperty, // property to convert
    CPIMContactItem& aItem) // item to insert the field to
{
    JELOG2(EPim);
    if (aProperty.Name().CompareF(KVersitTokenPHOTO) != 0)
    {
        // Unsupported binary property, silently discarded
        return;
    }

    CParserPropertyValueBinary* propertyValue =
        static_cast<CParserPropertyValueBinary*>(aProperty.Value());
    const CBufSeg* value = propertyValue->Value();

    TInt binarySize = value->Size();
    CPIMByteArray* byteArray = new(ELeave) CPIMByteArray(binarySize);
    CleanupStack::PushL(byteArray);
    byteArray->ResizeL(binarySize);
    TUint8& byteRef = byteArray->At(0);
    TUint8* bytePtr = &byteRef;

    value->Read(0, static_cast<TAny*>(bytePtr), binarySize);

    aItem.AddBinaryRawL(EPIMContactPhoto, KPIMAttrNone, byteArray);
    CleanupStack::Pop(byteArray); // byteArray now owned by aItem
}

// -----------------------------------------------------------------------------
// CPIMCardPropertyConverter::ConvertClassFieldL
// Converts a CLASS field from a PIM Contact Item to a vCard
// -----------------------------------------------------------------------------
//
void CPIMCardPropertyConverter::ConvertClassFieldL(const CPIMItem& aItem,
        CParserVCard& aParser, TPIMContactField aField, TInt aIndex)
{
    JELOG2(EPim);
    TInt classIntValue = aItem.getInt(aField, aIndex);
    TPtrC classStringValue(KPIMClassStringConfidential());
    switch (classIntValue)
    {
    case EPIMContactClassConfidential:
    {
        classStringValue.Set(KPIMClassStringConfidential());
        break;
    }
    case EPIMContactClassPrivate:
    {
        classStringValue.Set(KPIMClassStringPrivate());
        break;
    }
    case EPIMContactClassPublic:
    {
        classStringValue.Set(KPIMClassStringPublic());
        break;
    }
    default:
    {
        __ASSERT_DEBUG(EFalse, User::Invariant());
        break;
    }
    }
    CParserPropertyValue* propertyValue = CParserPropertyValueHBufC::NewL(
                                              classStringValue);
    AddPropertyToParserL(propertyValue, KVersitTokenClass(), NULL, aParser);
    // The paramArray and propertyValue are popped by the previous function call
}

// -----------------------------------------------------------------------------
// CPIMCardPropertyConverter::ConvertClassPropertyL
// Converts CLASS property to a PIM Event item.
// -----------------------------------------------------------------------------
//
void CPIMCardPropertyConverter::ConvertClassPropertyL(
    const CPIMParserProperty& aProperty, CPIMContactItem& aItem)
{
    JELOG2(EPim);
    CParserPropertyValueHBufC* propertyValue =
        static_cast<CParserPropertyValueHBufC*>(aProperty.Value());

    const TPtrC classStringPtr = propertyValue->Value();
    TInt classInt = KErrCorrupt;

    if (classStringPtr.CompareF(KPIMClassStringPublic) == 0)
    {
        classInt = EPIMContactClassPublic;
    }
    else if (classStringPtr.CompareF(KPIMClassStringPrivate) == 0)
    {
        classInt = EPIMContactClassPrivate;
    }
    else if (classStringPtr.CompareF(KPIMClassStringConfidential) == 0)
    {
        classInt = EPIMContactClassConfidential;
    }
    // else the class value in the originating vCalendar is flawed - ignore

    if (classInt != KErrCorrupt)
    {
        aItem.addInt(EPIMContactClass, KPIMAttrNone, classInt);
    }
}

// -----------------------------------------------------------------------------
// CPIMCardPropertyConverter::ConvertOrganisationalFieldsL
// Converts organisational fields from PIM item to CParserVCard
// -----------------------------------------------------------------------------
//
void CPIMCardPropertyConverter::ConvertOrganisationalFieldsL(
    const CPIMItem& aItem, CParserVCard& aParser, TPIMContactField /*aField*/,
    TInt /*aIndex*/)
{
    JELOG2(EPim);
    // Check that the field is not converted twice (e.g one of the
    // organisational fields have been already converted and caused
    // other field to be converted also
    CArrayPtr<CParserProperty>* props = aParser.ArrayOfProperties(EFalse);
    if (props)
    {
        TInt propCount = props->Count();
        for (TInt i = 0; i < propCount; i++)
        {
            const CParserProperty* property = props->At(i);
            if (property->Name().CompareF(KVersitTokenORG) == 0)
            {
                return;
            }
        }
    }

    // Create array from organisation and department fields
    CDesCArray* orgArray = new(ELeave) CDesCArrayFlat(2);
    CleanupStack::PushL(orgArray);
    if (aItem.CountValuesL(EPIMContactOrg) > 0)
    {
        const TDesC& org = aItem.GetStringL(EPIMContactOrg, 0);
        orgArray->AppendL(org);
    }
    else
    {
        orgArray->AppendL(KNullDesC);
    }
    // Department field is after organisational unit
    if (aItem.CountValuesL(EPIMContactExtDepartment) > 0)
    {
        const TDesC& dep = aItem.GetStringL(EPIMContactExtDepartment, 0);
        orgArray->AppendL(dep);
    }
    else
    {
        orgArray->AppendL(KNullDesC);
    }

    CParserPropertyValue* propertyValue =
        new(ELeave) CParserPropertyValueCDesCArray(orgArray);
    CleanupStack::Pop(orgArray); // orgArray now owned by propertyValue
    AddPropertyToParserL(propertyValue, KVersitTokenORG(), NULL, aParser);
    // The paramArray and propertyValue are popped by the previous function call
}

// -----------------------------------------------------------------------------
// CPIMCardPropertyConverter::ConvertNameReadingFieldL
// Converts name reading fields from vCard to PIM item
// -----------------------------------------------------------------------------
//
void CPIMCardPropertyConverter::ConvertNameReadingFieldL(
    const CPIMParserProperty& aProperty, CPIMContactItem& aItem)
{
    JELOG2(EPim);
    TInt nameCount = iContactValidator.NumElementsL(EPIMContactName);

    // Name reading fields come in a HBufC property value although they
    // are in an array property value and SHOULD come in as an array.
    // So, we have to generate an array property from the HBufC proprety
    CParserPropertyValueHBufC* propertyValue =
        static_cast<CParserPropertyValueHBufC*>(aProperty.Value());
    HBufC* value = propertyValue->TakeValueOwnership();
    CleanupStack::PushL(value);
    // PIM parser to access protected methods
    CPIMVCardParser* parser = CPIMVCardParser::NewLC();
    CDesCArray* readingValues = parser->MakePropertyValueCDesCArrayL(
                                    value->Des());
    CleanupStack::PopAndDestroy(2, value); // parser
    CleanupStack::PushL(readingValues);

    // Array for the new name elements
    CDesCArray* newNameArray = new(ELeave) CDesCArrayFlat(nameCount);
    CleanupStack::PushL(newNameArray);
    // Old elements exist
    if (aItem.CountValuesL(EPIMContactName) > 0)
    {
        const CDesCArray& nameArray = aItem.GetStringArrayL(EPIMContactName, 0);
        for (TInt i = 0; i < nameCount; i++)
        {
            TPtrC element(nameArray.MdcaPoint(i));
            newNameArray->AppendL(element);
        }
        // Add array elements. If the array count is larger than zero
        // it indicates that at least the Last name reading field is present
        if (readingValues->Count() > 0)
        {
            // Remove old value and add new
            newNameArray->Delete(EPIMContactExtFamilyNameReading);
            newNameArray->InsertL(EPIMContactExtFamilyNameReading,
                                  readingValues->MdcaPoint(0));
        }
        // If the array count is larger than one it indicates that
        // the First name reading is also present
        if (readingValues->Count() > 1)
        {
            newNameArray->Delete(EPIMContactExtGivenNameReading);
            newNameArray->InsertL(EPIMContactExtGivenNameReading,
                                  readingValues->MdcaPoint(1));
        }

        // Set new string array for the contact name field
        aItem.SetStringArrayL(EPIMContactName, 0, KPIMAttrNone, newNameArray);
    }
    // No old name array
    else
    {
        // Create a new array and append name reading values to it
        for (TInt i = 0; i < nameCount; i++)
        {
            newNameArray->AppendL(KPIMNullArrayElement);
        }
        // Add array elements. If the array count is larger than zero
        // it indicates that at least the Last name reading field is present
        if (readingValues->Count() > 0)
        {
            // Remove old value and add new
            newNameArray->Delete(EPIMContactExtFamilyNameReading);
            newNameArray->InsertL(EPIMContactExtFamilyNameReading,
                                  readingValues->MdcaPoint(0));
        }
        // If the array count is larger than one it indicates that
        // the First name reading is also present
        if (readingValues->Count() > 1)
        {
            newNameArray->Delete(EPIMContactExtGivenNameReading);
            newNameArray->InsertL(EPIMContactExtGivenNameReading,
                                  readingValues->MdcaPoint(1));
        }
        // Add new string array for the cotnact name field
        aItem.AddStringArrayL(EPIMContactName, KPIMAttrNone, newNameArray);
    }

    CleanupStack::Pop(newNameArray);
    CleanupStack::PopAndDestroy(readingValues);
}

// -----------------------------------------------------------------------------
// CPIMCardPropertyConverter::AddPropertyToParserL
// adds a new property value to the parser. NOTE that the property value
// -----------------------------------------------------------------------------
//
void CPIMCardPropertyConverter::AddPropertyToParserL(
    CParserPropertyValue* aPropertyValue, const TDesC8& aPropertyName,
    CPIMVCardParserParamArray* aArrayOfParams, CParserVCard& aParser)
{
    JELOG2(EPim);
    // Add property value to the cleanup stack. It should not be added
    // before this call. This function is an exception and reduces much code
    CleanupStack::PushL(aPropertyValue);
    // Create a new parser property from the property value, its name and
    // array of parameters. If there are no parameters, the aArrayOfParams
    // should be NULL and the parser property will be generated without params
    CParserProperty* property = CParserProperty::NewL(*aPropertyValue,
                                aPropertyName, aArrayOfParams);
    // property takes ownership of the property value
    CleanupStack::Pop(aPropertyValue);
    // Pop aArrayOfParams if it is not NULL
    if (aArrayOfParams)
    {
        CleanupStack::Pop(aArrayOfParams);
    }
    // NOTE: property MUST not be pushed to the cleanup stack since the
    // AddPropertyL pushes it right away to the cleanup stack. So, we must not
    // push it here to avoid duplicate stack pointers
    aParser.AddPropertyL(property);
    // Property is now owned by the parser but we do not have to pop it
}

// End of file
