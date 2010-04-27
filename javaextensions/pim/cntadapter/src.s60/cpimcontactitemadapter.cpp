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
* Description:  Handles conversion between PIM API Contact Items and Contacts
 *                Model Contact Cards
 *
*/


// CLASS HEADER
#include "cpimcontactitemadapter.h"

// INTERNAL INCLUDES
#include "cpimcontactcategorymanager.h"
#include "pimpanics.h"
#include "mpimcontactitem.h"
#include "mpimitemdata.h"
#include "cpimimagescaler.h"
#include "cpimcontactfieldinfo.h"
#include "logger.h"

// EXTERNAL INCLUDES
#include <cntitem.h> // CContactCard etc.
#include <cntfldst.h> // CContactTextField etc.
#include <tz.h>
#include <tzconverter.h>



// UNNAMED LOCAL NAMESPACE
namespace
{
// Phonebook maps Syncronisation field values with specified
// hard-coded string values. At the time implementing, the header
// was not exported so it is necessary to define own constants
// with same values
_LIT(KPIMContactClassPrivate, "private");
_LIT(KPIMContactClassPublic, "public");
_LIT(KPIMContactClassConfidential, "none");
}

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPIMContactItemAdapter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPIMContactItemAdapter* CPIMContactItemAdapter::NewL(
    CPIMContactCategoryManager& aCategoryManager)
{
    JELOG2(EPim);
    CPIMContactItemAdapter* self =
        new(ELeave) CPIMContactItemAdapter(aCategoryManager);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

// destructor
CPIMContactItemAdapter::~CPIMContactItemAdapter()
{
    JELOG2(EPim);
    delete iImageScaler;
    delete iFieldInfo;
}

// -----------------------------------------------------------------------------
// CPIMContactItemAdapter::CardL
// Converts from a PIM item data to a Contacts Model contact card.
// Returns: A contact card with the same information as given in
//          the parameter item. The ownership of the card is
//          transferred to the caller.
// -----------------------------------------------------------------------------
//
CContactCard* CPIMContactItemAdapter::CardL(const MPIMContactItem& aItem)
{
    JELOG2(EPim);
    CContactCard* card = CContactCard::NewLC();
    const MPIMItemData& item = aItem.ItemData();
    __ASSERT_DEBUG((aItem.ContactItemIdL() == 0), User::Panic(
                       KPIMPanicCategory, EPIMPanicInvalidItemID));
    CArrayFix<TPIMField>* fields = item.FieldsLC();

    TInt fieldCount = fields->Count();
    for (TInt i = 0; i < fieldCount; i++)
    {
        ConvertFieldL(aItem, *card,
                      static_cast<TPIMContactField>(fields->At(i)));
    }

    // Set default values if not present in PIM item
    if (item.CountValues(EPIMContactClass) == 0)
    {
        CContentType* type = CContentType::NewL(KUidContactFieldClass,
                                                KUidContactFieldVCardMapClass);
        CleanupStack::PushL(type);

        // Add value to the contact field
        CContactItemField* field = CContactItemField::NewLC(KStorageTypeText,
                                   *type);
        field->SetUserFlags(EContactCategoryNone);
        CContactTextField* textField = field->TextStorage();
        textField->SetTextL(KPIMContactClassPrivate());
        card->AddFieldL(*field);

        CleanupStack::Pop(field);
        CleanupStack::PopAndDestroy(type);
    }

    CleanupStack::PopAndDestroy(fields);
    CleanupStack::Pop(card);
    return card;
}

// -----------------------------------------------------------------------------
// CPIMContactItemAdapter::FillItemL
// Converts from a Contacts Model contact card to the PIM representation.
// -----------------------------------------------------------------------------
//
void CPIMContactItemAdapter::FillItemL(MPIMContactItem& aItem, // Item data container to fill with the data
                                       const CContactCard& aCard) // Contact Card to convert
{
    JELOG2(EPim);
    // convert fields
    CContactItemFieldSet& fieldSet = aCard.CardFields();
    TInt fieldCount = fieldSet.Count();
    for (TInt i = 0; i < fieldCount; i++)
    {
        ConvertFieldL(fieldSet[i], aItem);
    }
    // convert categories
    MPIMItemData& item = aItem.ItemData();
    CContactIdArray* groupArray = aCard.GroupsJoinedLC();
    TInt groupCount = groupArray->Count();
    for (TInt i = 0; i < groupCount; i++)
    {
        HBufC* label = iCategoryManager.GroupLabelL((*groupArray)[i]);
        CleanupStack::PushL(label);
        // Add item to this category if it doesn't belong to it already
        if (item.FindCategory(*label) == KErrNotFound)
        {
            item.AddNewCategoryL(*label);
        }
        CleanupStack::PopAndDestroy(label);
    }
    CleanupStack::PopAndDestroy(groupArray);
}

// -----------------------------------------------------------------------------
// CPIMContactItemAdapter::UpdateCardL
// Updates the data in the Contacts Model contact card to match
// the data in the PIM item data container.
// -----------------------------------------------------------------------------
//
void CPIMContactItemAdapter::UpdateCardL(const MPIMContactItem& aItem, // Item from which the data is read
        CContactCard& aCard) // The card which is updated
{
    JELOG2(EPim);
    CleanCard(aCard);
    const MPIMItemData& itemData = aItem.ItemData();
    // Get fields
    CArrayFix<TPIMField>* fields = itemData.FieldsLC();
    TInt fieldCount = fields->Count();
    for (TInt i = 0; i < fieldCount; i++)
    {
        ConvertFieldL(aItem, aCard, (TPIMContactField)(fields->At(i)));
    }
    CleanupStack::PopAndDestroy(); // fields
}

// -----------------------------------------------------------------------------
// CPIMContactItemAdapter::UpdateCategoriesL
// Updates the category information in the database.
// -----------------------------------------------------------------------------
//
void CPIMContactItemAdapter::UpdateCategoriesL(const MPIMContactItem& aItem, // Item containing the new categories
        const CContactCard& aCard) // Item to update
{
    JELOG2(EPim);
    const CDesCArray& pimCategories = aItem.ItemData().Categories();
    const TInt pimCategoryCount = pimCategories.Count();
    TBool* newCategoryTable = new(ELeave) TBool[pimCategoryCount];
    CleanupArrayDeletePushL(newCategoryTable);
    TInt i = 0;
    for (i = 0; i < pimCategoryCount; i++)
    {
        newCategoryTable[i] = ETrue;
    }

    CContactIdArray* cardCategories = aCard.GroupsJoinedLC();
    TContactItemId id = aItem.ContactItemIdL();

    // first remove extra groups
    const TInt cardCategoryCount = cardCategories->Count();
    for (i = 0; i < cardCategoryCount; i++)
    {
        TContactItemId groupId = (*cardCategories)[i];
        HBufC* groupLabel = iCategoryManager.GroupLabelL(groupId);
        TInt pos;
        TInt found = pimCategories.Find(*groupLabel, pos, ECmpNormal);
        delete groupLabel;
        if (found != 0)
        {
            // removed group
            iCategoryManager.RemoveFromGroupL(id, groupId);
        }
        else
        {
            // old group
            newCategoryTable[pos] = EFalse;
        }
    }
    // then add new categories
    for (i = 0; i < pimCategoryCount; i++)
    {
        if (newCategoryTable[i])
        {
            TPtrC category = pimCategories[i];
            iCategoryManager.AddToGroupL(id, category);
        }
    }
    CleanupStack::PopAndDestroy(2); // cardCategories, newCategoryTable
}

// -----------------------------------------------------------------------------
// CPIMContactItemAdapter::ContactsModelFieldType
// Other items were commented in the header
// -----------------------------------------------------------------------------
//
CArrayFix<TInt>* CPIMContactItemAdapter::ContactsModelFieldTypeL(
    TPIMContactField aContactField) const
{
    JELOG2(EPim);
    return iFieldInfo->MatchPIMFieldL(aContactField);
}

// -----------------------------------------------------------------------------
// CPIMContactItemAdapter::CPIMContactItemAdapter
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPIMContactItemAdapter::CPIMContactItemAdapter(
    CPIMContactCategoryManager& aCategoryManager) :
        iCategoryManager(aCategoryManager)
{
    JELOG2(EPim);
}

// -----------------------------------------------------------------------------
// CPIMContactItemAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
inline void CPIMContactItemAdapter::ConstructL()
{
    JELOG2(EPim);
    iFieldInfo = CPIMContactFieldInfo::NewL();
    iImageScaler = CPIMImageScaler::NewL();
}

// -----------------------------------------------------------------------------
// CPIMContactItemAdapter::CleanCardL
// Deletes all the supported fields from the contact card.
// Those fields which are not supported are left as is.
// -----------------------------------------------------------------------------
//
void CPIMContactItemAdapter::CleanCard(CContactCard& aCard)
{
    JELOG2(EPim);
    // convert fields
    CContactItemFieldSet& fieldSet = aCard.CardFields();
    TInt fieldCount = fieldSet.Count();
    for (TInt i = 0; i < fieldCount; i++)
    {
        if (IsSupportedField(fieldSet[i]))
        {
            // as the field is removed, the array shrinks
            aCard.RemoveField(i--);
            fieldCount--;
        }
    }
}
// -----------------------------------------------------------------------------
// CPIMContactItemAdapter::IsSupportedFieldL
// Checks if we support this particular contact field.
// Returns:     ETrue if the field is supported
//              EFalse otherwise
// -----------------------------------------------------------------------------
//
TBool CPIMContactItemAdapter::IsSupportedField(CContactItemField& aField)
{
    JELOG2(EPim);
    const CContentType& type = aField.ContentType();
    if (type.FieldTypeCount() == 0)
    {
        // we don't support unknown types
        return EFalse;
    }

    TFieldType fieldTypeUid = type.FieldType(0);
    TInt fieldType = fieldTypeUid.iUid;
    switch (fieldType)
    {
    case KUidContactFieldAddressValue:
    {
        // Label addresses are also stored in the database using
        // KUidContactFieldAddressValue, but we don't support them
        if (type.ContainsFieldType(KUidContactFieldVCardMapADR))
        {
            return ETrue;
        }
        else
        {
            return EFalse;
        }
    }
    case KUidContactFieldPictureValue:
    {
        // Thumbnails are supported while pictures are not
        if (type.ContainsFieldType(KUidContactFieldVCardMapPHOTO))
        {
            return ETrue;
        }
        else
        {
            return EFalse;
        }
    }
    case KUidContactFieldSIPIDValue:
    case KUidContactFieldFamilyNameValue:
    case KUidContactFieldGivenNameValue:
    case KUidContactFieldSecondNameValue:
    case KUidContactFieldFamilyNamePronunciationValue:
    case KUidContactFieldGivenNamePronunciationValue:
    case KUidContactFieldAdditionalNameValue:
    case KUidContactFieldPrefixNameValue:
    case KUidContactFieldSuffixNameValue:
    case KUidContactFieldPhoneNumberValue:
    case KUidContactFieldFaxValue:
    case KUidContactFieldEMailValue:
    case KUidContactFieldUrlValue:
    case KUidContactFieldPostOfficeValue:
    case KUidContactFieldExtendedAddressValue:
    case KUidContactFieldPostCodeValue:
    case KUidContactFieldLocalityValue:
    case KUidContactFieldRegionValue:
    case KUidContactFieldCountryValue:
    case KUidContactFieldCompanyNameValue:
    case KUidContactFieldNoteValue:
    case KUidContactFieldBirthdayValue:
    case KUidContactFieldJobTitleValue:
    case KUidContactFieldDTMFValue:
    case KUidContactFieldIMAddressValue:
    case KUidContactFieldDepartmentNameValue:
    case KUidContactFieldAssistantValue:
    case KUidContactFieldChildrenValue:
    case KUidContactFieldSpouseValue:
    case KUidContactFieldAnniversaryValue:
    case KUidContactFieldClassValue:
    {
        return ETrue;
    }
    default:
    {
        return EFalse;
    }
    }
}

// -----------------------------------------------------------------------------
// CPIMContactItemAdapter::ConvertFieldL
// Converts a field from a PIM Item to a Contact Card.
// -----------------------------------------------------------------------------
//
void CPIMContactItemAdapter::ConvertFieldL(const MPIMContactItem& aItem, // item to read the field from
        CContactCard& aCard, // card to write the field to
        TPIMContactField aField) // field to convert
{
    JELOG2(EPim);
    const MPIMItemData& itemData = aItem.ItemData();

    // These are indexes in PIM Item address values
    TInt plainAddressIndex = KErrNotFound;
    TInt homeAddressIndex = KErrNotFound;
    TInt workAddressIndex = KErrNotFound;
    if (aField == EPIMContactAddr)
    {
        AddressAttributesL(itemData, plainAddressIndex, homeAddressIndex,
                           workAddressIndex);
        ConvertAddressFieldL(itemData, aCard, aField, plainAddressIndex,
                             KPIMAttrNone);
        ConvertAddressFieldL(itemData, aCard, aField, homeAddressIndex,
                             EPIMContactAttrHome);
        ConvertAddressFieldL(itemData, aCard, aField, workAddressIndex,
                             EPIMContactAttrWork);
        return;
    }

    TInt valueCount = itemData.CountValues(aField);
    for (TInt i = 0; i < valueCount; i++)
    {
        switch (aField)
        {
        case EPIMContactUid:
        case EPIMContactRevision:
        {
            // uid and revision fields are totally ignored
            break;
        }
        case EPIMContactBirthday:
        case EPIMContactExtAnniversary:
        {
            ConvertDateFieldL(itemData, aCard, aField, i);
            break;
        }
        case EPIMContactEmail:
        case EPIMContactNickname:
        case EPIMContactNote:
        case EPIMContactOrg:
        case EPIMContactTel:
        case EPIMContactTitle:
        case EPIMContactUrl:
        case EPIMContactExtSip:
        case EPIMContactExtDtmf:
        case EPIMContactExtWvUserId:
        case EPIMContactExtDepartment:
        case EPIMContactExtAssistantName:
        case EPIMContactExtChildren:
        case EPIMContactExtSpouse:
        case EPIMContactExtVoip:
        case EPIMContactExtPTT:
        case EPIMContactExtSWIS:
        {
            ConvertStringFieldL(itemData, aCard, aField, i);
            break;
        }
        case EPIMContactName:
        {
            ConvertNameFieldL(itemData, aCard, aField, i);
            break;
        }
        case EPIMContactPhoto:
        {
            ConvertPhotoFieldL(itemData, aCard, aField, i);
            break;
        }
        case EPIMContactClass:
        {
            ConvertClassFieldL(itemData, aCard, aField, i);
            break;
        }
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
// CPIMContactItemAdapter::ConvertFieldL
// Converts a field from a Contact Card to a PIM Item.
// -----------------------------------------------------------------------------
//
void CPIMContactItemAdapter::ConvertFieldL(const CContactItemField& aField, // field to read data from
        MPIMContactItem& aItem) // field to write the data to
{
    JELOG2(EPim);
    TStorageType type = aField.StorageType();
    switch (type)
    {
    case KStorageTypeText:
    {
        ConvertTextFieldL(aField, aItem);
        break;
    }
    case KStorageTypeStore:
    {
        ConvertStoreFieldL(aField, aItem);
        break;
    }
    case KStorageTypeContactItemId:
    {
        break;
    }
    case KStorageTypeDateTime:
    {
        ConvertDateFieldL(aField, aItem);
        break;
    }
    default:
    {
        __ASSERT_DEBUG(EFalse, User::Panic(KPIMPanicCategory,
                                           EPIMPanicUnsupportedField));
    }
    }
}

// -----------------------------------------------------------------------------
// CPIMContactItemAdapter::ConvertTextFieldL
// Converts a text field from a Contact Card to a PIM Item.
// -----------------------------------------------------------------------------
//
void CPIMContactItemAdapter::ConvertTextFieldL(const CContactItemField& aField, // field to read data from
        MPIMContactItem& aItem) // field to write the data to
{
    JELOG2(EPim);
    MPIMItemData& itemData = aItem.ItemData();
    CContactTextField* textField = aField.TextStorage();
    const CContentType& type = aField.ContentType();

    // Check that can we handle this field
    if (!(textField->IsFull()) || (type.FieldTypeCount() == 0))
    {
        return;
    }

    TPtrC text = textField->Text();
    TFieldType fieldTypeUid = type.FieldType(0);
    TPIMAttribute attributes = KPIMAttrNone;

    TPIMField field = iFieldInfo->MatchContactField(fieldTypeUid.iUid);
    // Check which field we must handle
    switch (field)
    {
    case EPIMContactName:
    {
        TPIMContactNameElement
        nameElement =
            static_cast<TPIMContactNameElement>(iFieldInfo->MatchContactArrayElement(
                                                    fieldTypeUid.iUid));
        AddNameFieldToItemL(text, aItem, nameElement, aField.Label());
        break;
    }
    case EPIMContactAddr:
    {
        TPIMContactAddrElement
        addrElement =
            static_cast<TPIMContactAddrElement>(iFieldInfo->MatchContactArrayElement(
                                                    fieldTypeUid.iUid));
        attributes = iFieldInfo->FieldAttributes(aField.ContentType());

        // Label addresses are also stored in the database using
        // KUidContactFieldAddressValue, but we don't support them
        if (fieldTypeUid.iUid == KUidContactFieldAddressValue)
        {
            if (type.ContainsFieldType(KUidContactFieldVCardMapADR))
            {
                AddAddressFieldToItemL(text, aItem, EPIMContactAddrStreet,
                                       attributes, aField.Label());
            }
        }
        else
        {
            AddAddressFieldToItemL(text, aItem, addrElement, attributes,
                                   aField.Label());
        }
        break;
    }
    case EPIMContactClass:
    {
        AddClassFieldToItemL(itemData, text, aField.Label());
        break;
    }
    case EPIMContactExtSip:
    case EPIMContactExtVoip:
    case EPIMContactExtPTT:
    case EPIMContactExtSWIS:
    {
        // SIPID value can hold PoC (Push-To-Talk) Share view, SIP
        // and VOIP (Voice Over IP) fields inside it.
        // Apply internal attributes
        CArrayFix<TUid>* attrs = new(ELeave) CArrayFixFlat<TUid> (3);
        CleanupStack::PushL(attrs);
        iFieldInfo->GetFieldInternalAttributesL(type, *attrs);
        // VOIP (Internet telephone) field
        if (type.ContainsFieldType(KUidContactFieldVCardMapVOIP))
        {
            attributes = iFieldInfo->FieldAttributes(type);
            field = EPIMContactExtVoip;
        }
        else if (type.ContainsFieldType(KUidContactFieldVCardMapPOC))
        {
            field = EPIMContactExtPTT;
        }
        else if (type.ContainsFieldType(KUidContactFieldVCardMapSWIS))
        {
            field = EPIMContactExtSWIS;
        }
        else
        {
            field = EPIMContactExtSip;
        }
        // Add field, attributes, internal attributes and label to the item
        // Note that internal attributes are popped from the cleanup stack
        // inside the following function and there is no need to pop after it
        AddStringFieldWithLabelL(itemData, field, attributes, attrs, text,
                                 aField.Label());
        break;
    }
    case EPIMContactEmail:
    case EPIMContactNickname:
    case EPIMContactNote:
    case EPIMContactOrg:
    case EPIMContactTel:
    case EPIMContactTitle:
    case EPIMContactUrl:
    case EPIMContactExtDtmf:
    case EPIMContactExtWvUserId:
    case EPIMContactExtDepartment:
    case EPIMContactExtAssistantName:
    case EPIMContactExtChildren:
    case EPIMContactExtSpouse:
    {
        // Get internal attributes from the content type
        CArrayFix<TUid>* attrs = new(ELeave) CArrayFixFlat<TUid> (3);
        CleanupStack::PushL(attrs);
        iFieldInfo->GetFieldInternalAttributesL(type, *attrs);
        // Add attributes. FAX is special so it must be handled separately
        attributes = iFieldInfo->FieldAttributes(aField.ContentType());
        if (fieldTypeUid.iUid == KUidContactFieldFaxValue)
        {
            attributes |= EPIMContactAttrFax;
        }
        // Add field, attributes, internal attributes and label to the item
        // Note that internal attributes are popped from the cleanup stack
        // inside the following function and there is no need to pop after it
        AddStringFieldWithLabelL(itemData, field, attributes, attrs, text,
                                 aField.Label());
        break;
    }
    default:
    {
        // The field is not supported so we don't know what to do with it
        // and it is ignored. NOTE that contacts are opened first before
        // writing those to the contact database to ensure that unsupported
        // fields will still be included the to item and no data will be lost
        break;
    }
    }
}

// -----------------------------------------------------------------------------
// CPIMContactItemAdapter::AddStringFieldWithLabelL
// Adds a string field to the item, and optionally sets the fields label.
// -----------------------------------------------------------------------------
//
void CPIMContactItemAdapter::AddStringFieldWithLabelL(MPIMItemData& aItem,
        TPIMField aField, TPIMAttribute aAttributes,
        CArrayFix<TUid>* aInternalAttributes, const TPtrC aValue,
        const TPtrC aLabel)
{
    JELOG2(EPim);
    TInt valueCount = aItem.CountValues(aField);
    TPIMFieldData fieldData(aField, aAttributes, aValue.AllocLC());

    // Add data to the item
    aItem.AddValueL(fieldData);
    CleanupStack::Pop(); // aValue.AlloclC()
    // Apply internal attributes
    aItem.SetInternalAttributesL(aField, valueCount, aInternalAttributes);
    // aItem now owns aInternalAttributes
    CleanupStack::Pop(aInternalAttributes);

    // Set preferred index if it is set to this field value index
    // Only one value index of this field can have preferred index set
    // so we have to remove all existing preferred indexes. This has to
    // be done to SMS attribute also. There cannot be more than one
    // preferred SMS number in the S60 Phonebook
    if (aAttributes & EPIMContactAttrPreferred || aAttributes
            & EPIMContactAttrSms)
    {
        // Clear "preferred index" attribute from all other values
        // except this one (previously added one)
        for (TInt i = 0; i < valueCount; i++)
        {
            TPIMAttribute attributes = aItem.AttributesL(aField, i);
            // Clear attributes from preferred index
            if (aAttributes & EPIMContactAttrPreferred)
            {
                attributes &= ~EPIMContactAttrPreferred;
            }
            if (aAttributes & EPIMContactAttrSms)
            {
                attributes &= ~EPIMContactAttrSms;
            }
            aItem.SetAttributesL(aField, i, attributes);
        }
    }

    // Add label if necessary
    if (aLabel.Length() > 0)
    {
        aItem.SetLabelL(aField, valueCount, 0, aLabel);
    }
}

// -----------------------------------------------------------------------------
// CPIMContactItemAdapter::ConvertDateFieldL
// Converts a date field from a Contact Card to a PIM Item.
// -----------------------------------------------------------------------------
//
void CPIMContactItemAdapter::ConvertDateFieldL(const CContactItemField& aField, // field to read data from
        MPIMContactItem& aItem) // field to write the data to
{
    JELOG2(EPim);
    MPIMItemData& itemData = aItem.ItemData();
    CContactDateField* dateField = aField.DateTimeStorage();
    if (!(dateField->IsFull()))
    {
        // if date is not set, we don't read it
        return;
    }
    TTime time = dateField->Time();
    const CContentType& type = aField.ContentType();

    TPIMField field(0);
    // Check the field type
    if (type.ContainsFieldType(KUidContactFieldBirthday))
    {
        field = EPIMContactBirthday;
    }
    else if (type.ContainsFieldType(KUidContactFieldAnniversary))
    {
        field = EPIMContactExtAnniversary;
    }

    // Anniversary and Birthday are only dates so we are not interested
    // in which is the time in these fields. This is because Phonebook
    // doesn't take into account what time is used within these fields
    // Time conversion between UTC and local time is not needed here
    // because Phonebook handles only dates and PIM API times are in UTC
    // format causing this date to be set to the start of the UTC date time
    // and the date is shown correctly when midlet accesses it on the Java-side
    TDateTime temp(time.DateTime());
    temp.SetHour(0);
    temp.SetSecond(0);
    temp.SetMinute(0);
    temp.SetMicroSecond(0);

    // Count values and add new data
    TInt valueCount = itemData.CountValues(field);
    TPIMFieldData fieldData(field, KPIMAttrNone, TPIMDate(temp));
    itemData.AddValueL(fieldData);

    // Set label
    if (aField.Label().Length() > 0)
    {
        itemData.SetLabelL(field, valueCount, 0, aField.Label());
    }
}

// -----------------------------------------------------------------------------
// CPIMContactItemAdapter::ConvertStoreFieldL
// Converts a store field from a Contact Card to a PIM Item.
// -----------------------------------------------------------------------------
//
void CPIMContactItemAdapter::ConvertStoreFieldL(
    const CContactItemField& aField, // field to read data from
    MPIMContactItem& aItem) // field to write the data to
{
    JELOG2(EPim);
    CContactStoreField* storeField = aField.StoreStorage();
    if (!(storeField->IsFull()))
    {
        return;
    }
    const CContentType& type = aField.ContentType();
    if (type.ContainsFieldType(KUidContactFieldPicture))
    {
        // We don't care about other store fields
        MPIMItemData& itemData = aItem.ItemData();
        HBufC8* data = storeField->Thing();
        if (data)
        {
            TInt dataLength = data->Length();
            CPIMByteArray* byteArray = new(ELeave) CPIMByteArray(dataLength);
            CleanupStack::PushL(byteArray);
            byteArray->AppendL(data->Ptr(), dataLength);
            TPIMFieldData fieldData(EPIMContactPhoto, KPIMAttrNone, byteArray);
            itemData.AddValueL(fieldData);
            CleanupStack::Pop(); // byteArray
        }
    }
}

// -----------------------------------------------------------------------------
// CPIMContactItemAdapter::AddNameFieldToItemL
// Adds a name field to a PIM Item
// -----------------------------------------------------------------------------
//
void CPIMContactItemAdapter::AddNameFieldToItemL(TPtrC aText, // text to add
        MPIMContactItem& aItem, // item to add to
        TPIMContactNameElement aElement, // element to add
        TPtrC aLabel) // label of the field
{
    JELOG2(EPim);
    MPIMItemData& itemData = aItem.ItemData();
    // 7 for array granularity
    CDesCArrayFlat* nameArray = new(ELeave) CDesC16ArrayFlat(7);
    CleanupStack::PushL(nameArray);
    // do we already have a name field in the item
    TInt nameCount = itemData.CountValues(EPIMContactName);
    if (nameCount > 0)
    {
        const TPIMFieldData fieldData = itemData.ValueL(EPIMContactName, 0);
        const CDesCArray& oldNameArray = fieldData.StringArrayValue();
        TInt elementCount = oldNameArray.Count();

        for (TInt i = 0; i < elementCount; i++)
        {
            TPtrC16 oldName = oldNameArray[i];
            nameArray->AppendL(oldName);
        }
        // don't trust that item data has correct amount of elements
        for (TInt i = elementCount; i < EPIMContactNameNumElements; i++)
        {
            nameArray->AppendL(KPIMNullArrayElement());
        }
        // Add new element
        nameArray->Delete(aElement);
        nameArray->InsertL(aElement, aText);
        TPIMFieldData newFieldData(EPIMContactName, KPIMAttrNone, nameArray);
        itemData.SetValueL(newFieldData);
    }
    else
    {
        for (TInt i = 0; i < EPIMContactNameNumElements; i++)
        {
            nameArray->AppendL(KPIMNullArrayElement());
        }
        // Add new element
        nameArray->Delete(aElement);
        nameArray->InsertL(aElement, aText);
        TPIMFieldData newFieldData(EPIMContactName, KPIMAttrNone, nameArray);
        itemData.AddValueL(newFieldData);
    }
    // Name array is now owned by the item
    CleanupStack::Pop(nameArray);
    // Add label if it exists
    if (aLabel.Length() > 0)
    {
        itemData.SetLabelL(EPIMContactName, 0, aElement, aLabel);
    }
}

// -----------------------------------------------------------------------------
// CPIMContactItemAdapter::AddAddressFieldToItemL
// Adds an address field to a PIM Item
// -----------------------------------------------------------------------------
//
void CPIMContactItemAdapter::AddAddressFieldToItemL(TPtrC aText, // text to add
        MPIMContactItem& aItem, // item to add to
        TPIMContactAddrElement aElement, // element to add
        TPIMAttribute aAttributes, // attributes of the address
        TPtrC aLabel) // label
{
    JELOG2(EPim);
    MPIMItemData& itemData = aItem.ItemData();
    // 7 for array granularity
    CDesCArrayFlat* addrArray = new(ELeave) CDesC16ArrayFlat(7);
    CleanupStack::PushL(addrArray);
    // do we already have an address field in the item with the same attributes
    TInt addrCount = itemData.CountValues(EPIMContactAddr);
    TBool notFound = ETrue;
    TInt index = -1;
    for (TInt i = 0; (notFound) && (i < addrCount); i++)
    {
        if (itemData.AttributesL(EPIMContactAddr, i) == aAttributes)
        {
            const TPIMFieldData fieldData = itemData.ValueL(EPIMContactAddr, i);
            const CDesCArray& oldAddrArray = fieldData.StringArrayValue();
            TInt elementCount = oldAddrArray.Count();
            for (TInt j = 0; j < elementCount; j++)
            {
                TPtrC oldAddr = oldAddrArray[j];
                addrArray->AppendL(oldAddr);
            }
            for (TInt j = elementCount; j < EPIMContactAddrNumElements; j++)
            {
                addrArray->AppendL(KPIMNullArrayElement());
            }
            notFound = EFalse;
            addrArray->Delete(aElement);
            addrArray->InsertL(aElement, aText);
            TPIMFieldData newFieldData(EPIMContactAddr, aAttributes, addrArray);
            newFieldData.SetIndex(i);
            itemData.SetValueL(newFieldData);
            index = i;
        }
    }
    if (notFound)
    {
        for (TInt i = 0; i < EPIMContactAddrNumElements; i++)
        {
            addrArray->AppendL(KPIMNullArrayElement);
        }
        addrArray->Delete(aElement);
        addrArray->InsertL(aElement, aText);
        TPIMFieldData fieldData(EPIMContactAddr, aAttributes, addrArray);
        itemData.AddValueL(fieldData);
    }
    // Array is now owned by the item
    CleanupStack::Pop(addrArray);

    // Add label if it exists
    if (aLabel.Length() > 0)
    {
        // If the index is -1, it indicates that we are
        // adding new string array at the end of the others
        // so the index should be same as addrCount
        index = (index == -1) ? addrCount : index;
        itemData.SetLabelL(EPIMContactAddr, index, aElement, aLabel);
    }
}

// -----------------------------------------------------------------------------
// CPIMContactItemAdapter::ConvertAddressFieldL
// Converts an address field from a PIM Item to a Contact Card.
// -----------------------------------------------------------------------------
//
void CPIMContactItemAdapter::ConvertAddressFieldL(const MPIMItemData& aItem, // item to convert from
        CContactCard& aCard, // card to write to
        TPIMContactField aField, // field to convert
        TInt aIndex, // index to the field
        TPIMAttribute aAttributes) // attributes of the field
{
    JELOG2(EPim);
    if (aIndex == KErrNotFound)
    {
        return;
    }
    const TPIMFieldData fieldData = aItem.ValueL(aField, aIndex);
    const CDesCArray& addrArray = fieldData.StringArrayValue();
    EContactFieldCategory category = EContactCategoryNone;
    if (aAttributes == EPIMContactAttrHome)
    {
        category = EContactCategoryHome;
    }
    if (aAttributes == EPIMContactAttrWork)
    {
        category = EContactCategoryWork;
    }

    TInt addrCount = addrArray.Count();
    for (TInt i = 0; i < addrCount; i++)
    {
        if (addrArray[i].Compare(KPIMNullArrayElement()) == 0)
        {
            // we don't add null elements
            continue;
        }

        EnsureValidAddrElemLengthL(TPIMContactAddrElement(i), addrArray[i]);

        CContentType* type = iFieldInfo->AddressContentTypeL(
                                 static_cast<TPIMContactAddrElement>(i), category);
        CleanupStack::PushL(type);

        CContactItemField* field = CContactItemField::NewLC(KStorageTypeText,
                                   *type);

        // Add label to the field
        TPIMContactAddrElement element = TPIMContactAddrElement(i);
        const TPtrC label = aItem.LabelL(aField, aIndex, element);
        if (label.Compare(KPIMNullArrayElement) != 0)
        {
            field->SetLabelL(label);
        }
        field->SetUserFlags(category);
        // we don't store any label, Contacts Model should use default template
        CContactTextField* textField = field->TextStorage();
        textField->SetTextL(addrArray[i]);
        // Card takes the ownership of the field
        aCard.AddFieldL(*field);
        CleanupStack::Pop(field);
        CleanupStack::PopAndDestroy(type);
    }
}

// -----------------------------------------------------------------------------
// CPIMContactItemAdapter::ConvertNameFieldL
// Converts an address field from a PIM Item to a Contact Card.
// -----------------------------------------------------------------------------
//
void CPIMContactItemAdapter::ConvertNameFieldL(const MPIMItemData& aItem, // item to convert from
        CContactCard& aCard, // card to write to
        TPIMContactField aField, // field to convert
        TInt aIndex) // index to the field
{
    JELOG2(EPim);
    const TPIMFieldData fieldData = aItem.ValueL(aField, aIndex);
    const CDesCArray& nameArray = fieldData.StringArrayValue();
    EContactFieldCategory category = EContactCategoryHome;

    TInt nameCount = nameArray.Count();
    for (TInt i = 0; i < nameCount; i++)
    {
        if (nameArray[i].Compare(KPIMNullArrayElement()) == 0)
        {
            // we don't add null elements
            continue;
        }

        EnsureValidNameElemLengthL(TPIMContactNameElement(i), nameArray[i]);

        CContentType* type = iFieldInfo->NameContentTypeL(
                                 static_cast<TPIMContactNameElement>(i));
        CleanupStack::PushL(type);

        CContactItemField* field = CContactItemField::NewLC(KStorageTypeText,
                                   *type);

        // Add label to the field. aIndex should be 0 in this case because
        // there are only one string array in the name field
        TPIMContactNameElement element = TPIMContactNameElement(i);
        const TPtrC label = aItem.LabelL(aField, aIndex, element);
        if (label.Compare(KPIMNullArrayElement) != 0)
        {
            field->SetLabelL(label);
        }
        field->SetUserFlags(category);
        // we don't store any label, Contacts Model should use default template
        CContactTextField* textField = field->TextStorage();
        textField->SetTextL(nameArray[i]);
        // Card takes ownership of the field
        aCard.AddFieldL(*field);
        CleanupStack::Pop(field);
        CleanupStack::PopAndDestroy(type);
    }
}

// -----------------------------------------------------------------------------
// CPIMContactItemAdapter::ConvertDateFieldL
// Converts a date field from a PIM Item to a Contact Card.
// -----------------------------------------------------------------------------
//
void CPIMContactItemAdapter::ConvertDateFieldL(const MPIMItemData& aItem, // item to convert from
        CContactCard& aCard, // card to write to
        TPIMContactField aField, // field to convert
        TInt aIndex) // index to the field
{
    JELOG2(EPim);
    const TPIMFieldData fieldData = aItem.ValueL(aField, aIndex);
    const TPIMDate& date = fieldData.DateValue();
    CContentType* type = NULL;

    switch (aField)
    {
    case EPIMContactBirthday:
    {
        type = CContentType::NewL(KUidContactFieldBirthday,
                                  KUidContactFieldVCardMapBDAY);
        CleanupStack::PushL(type);
        break;
    }
    case EPIMContactExtAnniversary:
    {
        type = CContentType::NewL(KUidContactFieldAnniversary,
                                  KUidContactFieldVCardMapAnniversary);
        CleanupStack::PushL(type);
        break;
    }
    default:
    {
        User::Panic(KPIMPanicCategory, EPIMPanicUnsupportedDateField);
        break;
    }
    }
    CContactItemField* field = CContactItemField::NewLC(KStorageTypeDateTime,
                               *type);
    // set label if set in pim item
    const TPtrC label = aItem.LabelL(aField, aIndex, 0);
    if (label.Compare(KPIMNullArrayElement) != 0)
    {
        field->SetLabelL(label);
    }

    field->SetUserFlags(EContactCategoryOther);
    CContactDateField* dateField = field->DateTimeStorage();

    // PIM API stores dates as UTC time. Phonebook instead expects that dates
    // are stored as in local time format in the Contacts Model database
    RTz tzServer;
    User::LeaveIfError(tzServer.Connect());
    CleanupClosePushL(tzServer);

    // Create timezone converter
    CTzConverter* converter = CTzConverter::NewL(tzServer);
    CleanupStack::PushL(converter);

    TTime localTime(date);
    User::LeaveIfError(converter->ConvertToLocalTime(localTime));
    CleanupStack::PopAndDestroy(2); // converter, tzServer

    // Anniversary and Birthday are only dates so we are not interested
    // in which is the time in these fields. This is because Phonebook
    // doesn't take into account what time is used within these fields
    TDateTime temp(localTime.DateTime());
    temp.SetHour(0);
    temp.SetMinute(0);
    temp.SetSecond(0);
    temp.SetMicroSecond(0);

    dateField->SetTime(TTime(temp));
    // Card takes ownership of the field
    aCard.AddFieldL(*field);
    CleanupStack::Pop(field);
    CleanupStack::PopAndDestroy(type);
}

// -----------------------------------------------------------------------------
// CPIMContactItemAdapter::ConvertStringFieldL
// Converts a string field from a PIM Item to a Contact Card.
// -----------------------------------------------------------------------------
//
void CPIMContactItemAdapter::ConvertStringFieldL(const MPIMItemData& aItem, // item to convert from
        CContactCard& aCard, // card to write to
        TPIMContactField aField, // field to convert
        TInt aIndex) // index to the field
{
    JELOG2(EPim);
    const CArrayFix<TUid>& attrs = aItem.InternalAttributesL(aField, aIndex);
    const TPIMFieldData fieldData = aItem.ValueL(aField, aIndex);
    const TDesC& string = fieldData.StringValue();
    TPIMAttribute attributes = fieldData.Attributes();
    EContactFieldCategory category = EContactCategoryNone;

    EnsureValidStringValueLengthL(aField, string);

    if ((attributes & EPIMContactAttrHome) != 0)
    {
        category = EContactCategoryHome;
    }
    if ((attributes & EPIMContactAttrWork) != 0)
    {
        category = EContactCategoryWork;
    }
    CContentType* type = iFieldInfo->StringFieldContentTypeL(aField,
                         attributes, category);
    CleanupStack::PushL(type);
    // Apply internal attributes if any
    TInt interalAttrCount = attrs.Count();
    for (TInt i = 0; i < interalAttrCount; i++)
    {
        TFieldType fieldType = attrs.At(i);
        // Check to avoid duplicate field types in the content type
        // this may be possible when unrecognized fields are added to
        // internal attribute list (e.g incorrectly generated ContentType)
        if (!type->ContainsFieldType(fieldType))
        {
            type->AddFieldTypeL(fieldType);
        }
    }
    // Create new contact item field which is a text storage
    CContactItemField* field =
        CContactItemField::NewLC(KStorageTypeText, *type);
    // set label if set in pim item
    const TPtrC label = aItem.LabelL(aField, aIndex, 0);
    if (label.Compare(KPIMNullArrayElement) != 0)
    {
        field->SetLabelL(label);
    }

    field->SetUserFlags(category);
    CContactTextField* textField = field->TextStorage();
    textField->SetTextL(string);
    // Card takes ownership of the field
    aCard.AddFieldL(*field);
    CleanupStack::Pop(field);
    CleanupStack::PopAndDestroy(type);
}

// -----------------------------------------------------------------------------
// CPIMContactItemAdapter::ConvertPhotoFieldL
// Converts a photo field from a PIM Item to a Contact Card.
// -----------------------------------------------------------------------------
//
void CPIMContactItemAdapter::ConvertPhotoFieldL(const MPIMItemData& aItem, // item to convert from
        CContactCard& aCard, // card to write to
        TPIMContactField aField, // field to convert
        TInt aIndex) // index to the field
{
    JELOG2(EPim);
    const TPIMFieldData fieldData = aItem.ValueL(aField, aIndex);
    const CPIMByteArray& byteArray = fieldData.BinaryValue();
    EContactFieldCategory category = EContactCategoryNone;
    CContentType* type = CContentType::NewL(KUidContactFieldPicture,
                                            KUidContactFieldVCardMapPHOTO);
    CleanupStack::PushL(type);

    CContactItemField* field = CContactItemField::NewLC(KStorageTypeStore,
                               *type);
    field->SetUserFlags(category);

    const TUint8& byteRef = byteArray.At(0);
    const TUint8* bytePtr = &byteRef;
    TPtrC8 data(bytePtr, byteArray.Count());

    HBufC8* thumbNail = iImageScaler->ScaleL(data);
    CleanupStack::PushL(thumbNail);

    CContactStoreField* storeField = field->StoreStorage();
    storeField->SetThingL(*thumbNail);
    CleanupStack::PopAndDestroy(); // thumbNail
    // Card takes ownership of the field
    aCard.AddFieldL(*field);
    CleanupStack::Pop(field);
    CleanupStack::PopAndDestroy(type);
}

// -----------------------------------------------------------------------------
// CPIMContactItemAdapter::ConvertClassFieldL
// Converts a CLASS field from a PIM Item to a Contact Card.
// -----------------------------------------------------------------------------
//
void CPIMContactItemAdapter::ConvertClassFieldL(const MPIMItemData& aItemData,
        CContactCard& aCard, TPIMContactField aField, TInt aIndex)
{
    JELOG2(EPim);
    TPtrC classValueDes(KPIMContactClassPrivate());
    const TPIMFieldData fieldData = aItemData.ValueL(aField, aIndex);
    const TInt classValue = fieldData.IntegerValue();
    CContentType* type = CContentType::NewL(KUidContactFieldClass,
                                            KUidContactFieldVCardMapClass);
    CleanupStack::PushL(type);

    switch (classValue)
    {
    case EPIMContactClassPrivate:
    {
        classValueDes.Set(KPIMContactClassPrivate());
        break;
    }
    case EPIMContactClassPublic:
    {
        classValueDes.Set(KPIMContactClassPublic());
        break;
    }
    case EPIMContactClassConfidential:
    {
        classValueDes.Set(KPIMContactClassConfidential());
        break;
    }
    default:
    {
        __ASSERT_DEBUG(EFalse, User::Invariant());
        break; // For a happy compiler
    }
    }
    // Add value to the contact field
    CContactItemField* field =
        CContactItemField::NewLC(KStorageTypeText, *type);
    field->SetUserFlags(EContactCategoryNone);

    // Set label if any
    const TPtrC label = aItemData.LabelL(aField, aIndex, 0);
    if (label.Compare(KPIMNullArrayElement) != 0)
    {
        field->SetLabelL(label);
    }

    CContactTextField* textField = field->TextStorage();
    textField->SetTextL(classValueDes);
    // Card takes ownership of the field
    aCard.AddFieldL(*field);
    CleanupStack::Pop(field);
    CleanupStack::PopAndDestroy(type);
}

// -----------------------------------------------------------------------------
// CPIMContactItemAdapter::ConvertClassFieldL
// Converts a CLASS field from a Contact Card to PIM item
// -----------------------------------------------------------------------------
//
void CPIMContactItemAdapter::AddClassFieldToItemL(MPIMItemData& aItemData,
        TPtrC& aClassDesValue, TPtrC aLabel)
{
    JELOG2(EPim);
    // Syncronisation is private for default
    TPIMContactClassValue classValue = EPIMContactClassPrivate;
    TInt valueCount = aItemData.CountValues(EPIMContactClass);

    if (aClassDesValue.Compare(KPIMContactClassPublic()) == 0)
    {
        classValue = EPIMContactClassPublic;
    }
    else if (aClassDesValue.Compare(KPIMContactClassPrivate()) == 0)
    {
        classValue = EPIMContactClassPrivate;
    }
    else if (aClassDesValue.Compare(KPIMContactClassConfidential()) == 0)
    {
        classValue = EPIMContactClassConfidential;
    }
    TPIMFieldData fieldData(EPIMContactClass, EPIMFieldInt, KPIMAttrNone,
                            classValue);
    aItemData.AddValueL(fieldData);
    // Label if any
    if (aLabel.Length() > 0)
    {
        aItemData.SetLabelL(EPIMContactClass, valueCount, 0, aLabel);
    }
}

// -----------------------------------------------------------------------------
// CPIMContactItemAdapter::AddressAttributesL
// Checks the optimal attributes for the addresses
// -----------------------------------------------------------------------------
void CPIMContactItemAdapter::AddressAttributesL(const MPIMItemData& aItem,
        TInt& aPlainAddressIndex, TInt& aHomeAddressIndex, TInt& aWorkAddressIndex)
{
    JELOG2(EPim);
    aPlainAddressIndex = KErrNotFound;
    aHomeAddressIndex = KErrNotFound;
    aWorkAddressIndex = KErrNotFound;
    TInt addressCount = aItem.CountValues(EPIMContactAddr);

    // first pass - find zero and single attribute values if exist
    TInt i = 0;
    for (; i < addressCount; i++)
    {
        if (aItem.AttributesL(EPIMContactAddr, i) == KPIMAttrNone)
        {
            aPlainAddressIndex = i;
        }
        else if (aItem.AttributesL(EPIMContactAddr, i) == EPIMContactAttrHome)
        {
            aHomeAddressIndex = i;
        }
        else if (aItem.AttributesL(EPIMContactAddr, i) == EPIMContactAttrWork)
        {
            aWorkAddressIndex = i;
        }
    }

    // second pass - fill in the rest
    for (i = 0; i < addressCount; i++)
    {
        if ((aPlainAddressIndex != i) && (aHomeAddressIndex != i)
                && (aWorkAddressIndex != i))
        {
            // this index has not been used yet
            if (KErrNotFound == aWorkAddressIndex)
            {
                aWorkAddressIndex = i;
            }
            else if (KErrNotFound == aHomeAddressIndex)
            {
                aHomeAddressIndex = i;
            }
            else if (KErrNotFound == aPlainAddressIndex)
            {
                aPlainAddressIndex = i;
            }
            else
            {
                // all indexes are filled
                break; // out of the for loop
            }
        }
    }
}

// -----------------------------------------------------------------------------
// CPIMContactItemAdapter::EnsureValidStringValueLengthL
// -----------------------------------------------------------------------------
void CPIMContactItemAdapter::EnsureValidStringValueLengthL(
    TPIMContactField aField, const TDesC& aValue)
{
    JELOG2(EPim);
    const TInt* maxLengthArr = NULL;
    TInt offset = 0;
    TInt arrLen = 0;

    if (aField >= KContactExtStringMaxLengthOffset)
    {
        maxLengthArr = KContactExtStringMaxLengths;
        offset = KContactExtStringMaxLengthOffset;
        arrLen = KContactExtStringMaxLengthsLen;
    }
    else
    {
        maxLengthArr = KContactStringMaxLengths;
        offset = KContactStringMaxLengthsOffset;
        arrLen = KContactStringMaxLengthsLen;
    }

    TInt index = aField - offset;

    __ASSERT_ALWAYS((index >= 0), User::Invariant());
    __ASSERT_ALWAYS((index < arrLen), User::Invariant());

    TInt maxLen = maxLengthArr[index];
    if ((maxLen > 0) && (aValue.Length() > maxLen))
    {
        User::Leave(KErrTooBig);
    }
}

// -----------------------------------------------------------------------------
// CPIMContactItemAdapter::EnsureValidAddrElemLengthL
// -----------------------------------------------------------------------------
void CPIMContactItemAdapter::EnsureValidAddrElemLengthL(
    TPIMContactAddrElement aElemIndex, const TDesC& aElemValue)
{
    JELOG2(EPim);
    __ASSERT_DEBUG((aElemIndex >= 0), User::Invariant());

    __ASSERT_DEBUG((aElemIndex < EPIMContactAddrNumElements), User::Invariant());

    __ASSERT_DEBUG(
        (KContactAddrElemMaxLengths[aElemIndex] != KErrNotSupported),
        User::Invariant());

    if ((KContactAddrElemMaxLengths[aElemIndex] != KErrNotSupported)
            && (aElemValue.Length() > KContactAddrElemMaxLengths[aElemIndex]))
    {
        User::Leave(KErrTooBig);
    }
}

// -----------------------------------------------------------------------------
// CPIMContactItemAdapter::EnsureValidNameElemLengthL
// -----------------------------------------------------------------------------
void CPIMContactItemAdapter::EnsureValidNameElemLengthL(
    TPIMContactNameElement aElemIndex, const TDesC& aElemValue)
{
    JELOG2(EPim);
    __ASSERT_DEBUG((aElemIndex >= 0), User::Invariant());

    __ASSERT_DEBUG((aElemIndex < EPIMContactNameNumElements), User::Invariant());

    __ASSERT_DEBUG(
        (KContactNameElemMaxLengths[aElemIndex] != KErrNotSupported),
        User::Invariant());

    if ((KContactNameElemMaxLengths[aElemIndex] != KErrNotSupported)
            && (aElemValue.Length() > KContactNameElemMaxLengths[aElemIndex]))
    {
        User::Leave(KErrTooBig);
    }
}

// End of File
