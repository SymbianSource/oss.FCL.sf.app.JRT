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
* Description:  Provides static information about contact lists and creates
 *                contact list adapters.
 *
*/


// CLASS HEADER
#include "cpimcmadaptermanager.h"

// INTERNAL INCLUDES
#include "cpimcontactlistadapter.h"
#include "pimlocalizationids.h"
#include "mpimitem.h"
#include "logger.h"

// EXTERNAL INCLUDES
#include <badesca.h>

//LOCAL FUNCTION PROTOTYPES
HBufC* CreateFullNameLC(const CDesCArray& aNames);

//============================== LOCAL FUNCTIONS ===============================

//------------------------------------------------------------------------------
//CreateFullNameLC
//Creates a buffer to contain both family name and given name (if present)
//Returns: pointer to the buffer
//------------------------------------------------------------------------------
//
HBufC* CreateFullNameLC(const CDesCArray& aNames)
{
    JELOG2(EPim);
    HBufC* name = HBufC::NewLC(aNames[EPIMContactNameGiven].Length()
                               + aNames[EPIMContactNameFamily].Length());

    TPtr namePtr(name->Des());
    if (aNames[EPIMContactNameFamily] != KPIMNullArrayElement)
    {
        namePtr.Append(aNames[EPIMContactNameFamily]);
    }
    if (aNames[EPIMContactNameGiven] != KPIMNullArrayElement)
    {
        namePtr.Append(aNames[EPIMContactNameGiven]);
    }

    return name;
}

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPIMCMAdapterManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPIMCMAdapterManager* CPIMCMAdapterManager::NewL(const TDesC& aListName)
{
    JELOG2(EPim);
    CPIMCMAdapterManager* self = new(ELeave) CPIMCMAdapterManager;

    CleanupStack::PushL(self);
    self->ConstructL(aListName);
    CleanupStack::Pop(self);

    return self;
}

CPIMCMAdapterManager::~CPIMCMAdapterManager()
{
    JELOG2(EPim);
    delete iListName;
    delete iSupportedFields;
    delete iNoAttributes;
    delete iAddrAttributes;
    delete iEmailAttributes;
    delete iTelAttributes;
    delete iNameElements;
    delete iAddrElements;
    delete iVoipAttributes;
}

const TDesC& CPIMCMAdapterManager::ListNameL()
{
    JELOG2(EPim);
    return *iListName;
}

// -----------------------------------------------------------------------------
// CPIMCMAdapterManager::MaxCategories
// Returns:     Number of maximum supported categories
// -----------------------------------------------------------------------------
//

TInt CPIMCMAdapterManager::MaxCategories()
{
    JELOG2(EPim);
    return -1;
}

// -----------------------------------------------------------------------------
// CPIMCMAdapterManager::IsSupportedField
// Returns:     ETrue: field is supported
//              EFalse: field is not supported
// -----------------------------------------------------------------------------
//

TBool CPIMCMAdapterManager::IsSupportedField(TPIMField aField) // The field to be checked
{
    JELOG2(EPim);
    for (TInt i = 0; i < KPIMSupportedFieldsCount; i++)
    {
        if (KPIMSupportedFields[i] == aField)
        {
            return ETrue;
        }
    }
    return EFalse;
}

// -----------------------------------------------------------------------------
// CPIMCMAdapterManager::GetSupportedFieldsL
// Returns: An array containing all supported attributes
// -----------------------------------------------------------------------------
//

const CArrayFix<TPIMField>& CPIMCMAdapterManager::GetSupportedFieldsL()
{
    JELOG2(EPim);
    if (NULL == iSupportedFields)
    {
        iSupportedFields
        = new(ELeave) CArrayFixFlat<TPIMField> (KPIMSupportedFieldsCount);  // codescanner::leave
        iSupportedFields->AppendL(KPIMSupportedFields, KPIMSupportedFieldsCount); // codescanner::leave
    }
    return *iSupportedFields;
}

// -----------------------------------------------------------------------------
// CPIMCMAdapterManager::IsSupportedAttribute
// Returns:     ETrue: attribute is supported
//              EFalse: attribute is not supported
// -----------------------------------------------------------------------------
//

TBool CPIMCMAdapterManager::IsSupportedAttribute(TPIMField aField, // the field to be checked
        TPIMAttribute aAttribute) // The attribute to be checked
{
    JELOG2(EPim);
    TBool retval = EFalse;
    TRAPD(err, retval = IsSupportedAttributeL(aField, aAttribute));
    if (err != KErrNone)
    {
        return EFalse;
    }
    return retval;
}

// -----------------------------------------------------------------------------
// CPIMCMAdapterManager::GetSupportedAttributesL
// Returns: An array containing all supported attributes
// -----------------------------------------------------------------------------
//

const CArrayFix<TPIMAttribute>&
CPIMCMAdapterManager::GetSupportedAttributesL(TPIMField aField)
{
    JELOG2(EPim);
    CArrayFix<TPIMAttribute>* retVal = NULL;
    switch (aField)
    {
    case EPIMContactName:
    case EPIMContactFormattedName:
    case EPIMContactOrg:
    case EPIMContactTitle:
    case EPIMContactNickname:
    case EPIMContactBirthday:
    case EPIMContactNote:
    case EPIMContactPhoto:
    case EPIMContactUid:
    case EPIMContactClass:
    case EPIMContactExtWvUserId:
    case EPIMContactExtSip:
    case EPIMContactExtDtmf:
    case EPIMContactRevision:
    case EPIMContactExtDepartment:
    case EPIMContactExtAssistantName:
    case EPIMContactExtSpouse:
    case EPIMContactExtChildren:
    case EPIMContactExtAnniversary:
    case EPIMContactExtPTT:
    case EPIMContactExtSWIS:
    {
        if (NULL == iNoAttributes)
        {
            iNoAttributes = new(ELeave) CArrayFixFlat<TPIMAttribute> (1);
        }
        retVal = iNoAttributes;
        break;
    }
    case EPIMContactAddr:
    case EPIMContactUrl:
    {
        if (NULL == iAddrAttributes)
        {
            iAddrAttributes
            = new(ELeave) CArrayFixFlat<TPIMAttribute> (KPIMAddrAttributeCount);
            iAddrAttributes->AppendL(KPIMAddrAttributes, KPIMAddrAttributeCount);
        }
        retVal = iAddrAttributes;
        break;
    }
    case EPIMContactEmail:
    {
        if (NULL == iEmailAttributes)
        {
            iEmailAttributes
            = new(ELeave) CArrayFixFlat<TPIMAttribute> (KPIMEmailAttributeCount);
            iEmailAttributes->AppendL(KPIMEmailAttributes,
                                      KPIMEmailAttributeCount);
        }
        retVal = iEmailAttributes;
        break;
    }
    case EPIMContactTel:
    {
        if (NULL == iTelAttributes)
        {
            iTelAttributes
            = new(ELeave) CArrayFixFlat<TPIMAttribute> (KPIMTelAttributeCount);
            iTelAttributes->AppendL(KPIMTelAttributes, KPIMTelAttributeCount);
        }
        retVal = iTelAttributes;
        break;
    }
    case EPIMContactExtVoip:
    {
        if (NULL == iVoipAttributes)
        {
            iVoipAttributes
            = new(ELeave) CArrayFixFlat<TPIMAttribute> (KPIMVoipAttributeCount);
            iVoipAttributes->AppendL(KPIMVoipAttributes, KPIMVoipAttributeCount);
        }
        retVal = iVoipAttributes;
        break;
    }
    default:
    {
        User::Leave(KErrArgument);
    }
    }
    return *retVal;
}

// -----------------------------------------------------------------------------
// CPIMCMAdapterManager::GetSupportedAttributesCombinedL
// Returns: An integer holding the combination of all attributes
//          supported for aField.
// -----------------------------------------------------------------------------
//
TPIMAttribute CPIMCMAdapterManager::GetSupportedAttributesCombinedL(
    TPIMField aField)
{
    JELOG2(EPim);
    const CArrayFix<TPIMAttribute>& attributeArray = GetSupportedAttributesL(
                aField);
    TPIMAttribute retVal = KPIMAttrNone;
    TInt attributeCount = attributeArray.Count();
    for (TInt i = 0; i < attributeCount; i++)
    {
        retVal |= attributeArray[i];
    }
    return retVal;
}

// -----------------------------------------------------------------------------
// CPIMCMAdapterManager::GetAllSupportedAttributesCombinedL
// Returns: An integer holding the combination of all attributes
//          supported.
// -----------------------------------------------------------------------------
//
TPIMAttribute CPIMCMAdapterManager::GetAllSupportedAttributesCombined()
{
    JELOG2(EPim);
    return KPIMAddrAttributesCombined | KPIMEmailAttributesCombined
           | KPIMTelAttributesCombined;
}

// -----------------------------------------------------------------------------
// CPIMCMAdapterManager::IsSupportedArrayElement
// Returns:     ETrue: element is supported
//              EFalse: element is not supported
// -----------------------------------------------------------------------------
//

TBool CPIMCMAdapterManager::IsSupportedArrayElement(
    TPIMField aStringArrayField, // the field to be checked
    TPIMArrayElement aArrayElement) // The array element to be checked
{
    JELOG2(EPim);
    TBool retval = EFalse;
    TRAPD(err, retval = IsSupportedArrayElementL(aStringArrayField,
                        aArrayElement));
    if (err != KErrNone)
    {
        return EFalse;
    }
    return retval;
}

// -----------------------------------------------------------------------------
// CPIMCMAdapterManager::GetSupportedArrayElementsL
// Returns: An array containing all supported array elements
// -----------------------------------------------------------------------------
//

const CArrayFix<TPIMArrayElement>&
CPIMCMAdapterManager::GetSupportedArrayElementsL(TPIMField aStringArrayField)
{
    JELOG2(EPim);
    CArrayFix<TPIMArrayElement>* retVal = NULL;
    switch (aStringArrayField)
    {
    case EPIMContactName:
    {
        retVal = iNameElements;
        break;
    }
    case EPIMContactAddr:
    {
        retVal = iAddrElements;
        break;
    }
    default:
    {
        User::Leave(KErrArgument);
    }
    }
    return *retVal;
}

// -----------------------------------------------------------------------------
// CPIMCMAdapterManager::MaxValues
// Returns: Number of values supported for the field
// -----------------------------------------------------------------------------
//

TInt CPIMCMAdapterManager::MaxValues(TPIMField aField)
{
    JELOG2(EPim);
    TInt retVal = 0;

    if (aField >= KPIMContactSmallestField)
    {
        if (aField <= KPIMContactLargestField)
        {
            retVal = KPIMMaxValues[aField - KPIMContactMaxValueMappingOffset];
        }
        else if (aField >= KPIMContactSmallestExtField)
        {
            if (aField <= KPIMContactLargestExtField)
            {
                retVal = KPIMMaxExtValues[aField
                                          - KPIMContactMaxExtValueMappingOffset];
            }
            // else over extended fields
        }
        // else under extended fields
    }
    // else under standard fields

    return retVal;
}

// -----------------------------------------------------------------------------
// CPIMCMAdapterManager::StringArraySizeL
// Returns: Number of elements in a single string array value
// -----------------------------------------------------------------------------
//

TInt CPIMCMAdapterManager::StringArraySizeL(TPIMField aStringArrayField)
{
    JELOG2(EPim);
    TInt retVal = 0;
    switch (aStringArrayField)
    {
    case EPIMContactName:
    {
        retVal = KPIMNameArraySize;
        break;
    }
    case EPIMContactAddr:
    {
        retVal = KPIMAddrElementCount;
        break;
    }
    default:
    {
        User::Leave(KErrArgument);
    }
    }
    return retVal;
}

// -----------------------------------------------------------------------------
// CPIMCMAdapterManager::ItemOrder
// Provides pointer to a function which implements an algorithm that
// determines the order of two items.
// -----------------------------------------------------------------------------
//
TPIMItemComparisonFunc CPIMCMAdapterManager::ItemOrder()
{
    JELOG2(EPim);
    return ItemComparisonFunc;
}

// -----------------------------------------------------------------------------
// CPIMCMAdapterManager::GetFieldDataType
// Returns: Field type
// -----------------------------------------------------------------------------
//

TPIMFieldDataType CPIMCMAdapterManager::GetFieldDataTypeL(TPIMField aField)
{
    JELOG2(EPim);
    TPIMFieldDataType retVal = EPIMFieldInvalid;
    switch (aField)
    {
    case EPIMContactName:
    case EPIMContactAddr:
    {
        retVal = EPIMFieldStringArray;
        break;
    }
    case EPIMContactEmail:
    case EPIMContactFormattedName:
    case EPIMContactNickname:
    case EPIMContactNote:
    case EPIMContactOrg:
    case EPIMContactTel:
    case EPIMContactTitle:
    case EPIMContactUrl:
    case EPIMContactUid:
    case EPIMContactExtWvUserId:
    case EPIMContactExtSip:
    case EPIMContactExtDtmf:
    case EPIMContactExtDepartment:
    case EPIMContactExtAssistantName:
    case EPIMContactExtSpouse:
    case EPIMContactExtChildren:
    case EPIMContactExtVoip:
    case EPIMContactExtPTT:
    case EPIMContactExtSWIS:
    {
        retVal = EPIMFieldString;
        break;
    }
    case EPIMContactBirthday:
    case EPIMContactRevision:
    case EPIMContactExtAnniversary:
    {
        retVal = EPIMFieldDate;
        break;
    }
    case EPIMContactPhoto:
    {
        retVal = EPIMFieldBinary;
        break;
    }
    case EPIMContactClass:
    {
        retVal = EPIMFieldInt;
        break;
    }
    default:
    {
        User::Leave(KErrArgument);
    }
    }
    return retVal;
}

// -----------------------------------------------------------------------------
// CPIMCMAdapterManager::GetAdapterManager
// Provides access to the MPIMAdapterManager representation of this object
// Returns: MPIMAdapterManager representation of this object
// -----------------------------------------------------------------------------
//
MPIMAdapterManager* CPIMCMAdapterManager::GetAdapterManager()
{
    JELOG2(EPim);
    return this;
}

// -----------------------------------------------------------------------------
// CPIMCMAdapterManager::ItemComparisonFunc
// Compares two MPIMItemData objects.
// Returns: < 0: aFirst < aSecond
//          0: aFirst == aSecond
//          > 0: aFirst > aSecond
// -----------------------------------------------------------------------------
//
TInt CPIMCMAdapterManager::ItemComparisonFunc(const MPIMItem& aFirst,
        const MPIMItem& aSecond)
{
    JELOG2(EPim);
    TInt retVal = 0;
    TRAPD(err, retVal = ItemComparisonFuncL(aFirst, aSecond));
    if (err != KErrNone)
    {
        // the comparison failed, but there is no way to tell about it
        return 0;
    }
    return retVal;
}

// -----------------------------------------------------------------------------
// CPIMCMAdapterManager::ItemComparisonFuncL
// Compares two MPIMItemData objects.
// Returns: < 0: aFirst < aSecond
//          0: aFirst == aSecond
//          > 0: aFirst > aSecond
// -----------------------------------------------------------------------------
//

TInt CPIMCMAdapterManager::ItemComparisonFuncL(const MPIMItem& aFirst,
        const MPIMItem& aSecond)
{
    JELOG2(EPim);
    TInt result = 0; // Items are considered as equal by default
    TInt countFirst = aFirst.CountValuesL(EPIMContactName);
    TInt countSecond = aSecond.CountValuesL(EPIMContactName);

    if (countFirst != 0 && countSecond != 0)
    {
        // Both have same values in the name array so compare family and given
        // name based on that is there a value in the element or not. Family name
        // first. If not available, use given name. Note that if family names
        // are equal, the given name must be used
        const CDesCArray& fNames = aFirst.GetStringArrayL(EPIMContactName, 0);
        const CDesCArray& sNames = aSecond.GetStringArrayL(EPIMContactName, 0);

        HBufC* firstName = CreateFullNameLC(fNames);
        HBufC* secondName = CreateFullNameLC(sNames);

        result = firstName->CompareC(*secondName);
        CleanupStack::PopAndDestroy(2, firstName);
    }
    // The first has a name but the second does not. The second
    // is considered as lower priority in the sort order
    else if (countFirst != 0 && countSecond == 0)
    {
        result = -1;
    }
    // The second has a name but the first does not. The first
    // is considered as lower priority in the sort order
    else if (countFirst == 0 && countSecond != 0)
    {
        result = 1;
    }

    return result;
}
// -----------------------------------------------------------------------------
// CPIMCMAdapterManager::CPIMCMAdapterManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPIMCMAdapterManager::CPIMCMAdapterManager()
{
    JELOG2(EPim);
}

// -----------------------------------------------------------------------------
// CPIMCMAdapterManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPIMCMAdapterManager::ConstructL(const TDesC& aListName)
{
    JELOG2(EPim);
    iListName = aListName.AllocL();

    iNameElements
    = new(ELeave) CArrayFixFlat<TPIMArrayElement> (KPIMNameElementCount);
    iNameElements->AppendL(KPIMNameElements, KPIMNameElementCount);

    iAddrElements
    = new(ELeave) CArrayFixFlat<TPIMArrayElement> (KPIMAddrElementCount);
    iAddrElements->AppendL(KPIMAddrElements, KPIMAddrElementCount);
}

// -----------------------------------------------------------------------------
// CPIMCMAdapterManager::IsSupportedAttribute
// Returns:     ETrue: attribute is supported
//              EFalse: attribute is not supported
// -----------------------------------------------------------------------------
//

TBool CPIMCMAdapterManager::IsSupportedAttributeL(TPIMField aField, // the field to be checked
        TPIMAttribute aAttribute) // The attribute to be checked
{
    JELOG2(EPim);
    if (!IsSupportedField(aField))
    {
        return EFalse;
    }
    const CArrayFix<TPIMAttribute>& attributes =
        GetSupportedAttributesL(aField);
    TInt attributeCount = attributes.Count();
    for (TInt i = 0; i < attributeCount; i++)
    {
        if (attributes[i] == aAttribute)
        {
            return ETrue;
        }
    }
    return EFalse;
}

// -----------------------------------------------------------------------------
// CPIMCMAdapterManager::IsSupportedArrayElementL
// Returns:     ETrue: element is supported
//              EFalse: element is not supported
// -----------------------------------------------------------------------------
//

TBool CPIMCMAdapterManager::IsSupportedArrayElementL(
    TPIMField aStringArrayField, // the field to be checked
    TPIMArrayElement aArrayElement) // The array element to be checked
{
    JELOG2(EPim);
    if (!IsSupportedField(aStringArrayField))
    {
        return EFalse;
    }
    if (GetFieldDataTypeL(aStringArrayField) != EPIMFieldStringArray)
    {
        return EFalse;
    }
    const CArrayFix<TPIMArrayElement>& elements = GetSupportedArrayElementsL(
                aStringArrayField);
    TInt elementCount = elements.Count();
    for (TInt i = 0; i < elementCount; i++)
    {
        if (elements[i] == aArrayElement)
        {
            return ETrue;
        }
    }
    return EFalse;
}
