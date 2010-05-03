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


#ifndef CPIMCMADAPTERMANAGER_H
#define CPIMCMADAPTERMANAGER_H

//  INCLUDES
#include "mpimcontactadaptermanager.h"
#include "mpimadaptermanager.h"
#include "pimcontact.h" // field enums
#include "pimcommon.h" // basic types
#include <e32base.h>

// CONSTANTS


const TPIMField KPIMSupportedFields[] =
{
    EPIMContactPhoto,
    EPIMContactName,
    EPIMContactNickname,
    EPIMContactTel,
    EPIMContactEmail,
    EPIMContactExtDtmf,
    EPIMContactExtVoip,
    EPIMContactExtPTT,
    EPIMContactExtSWIS,
    EPIMContactExtSip,
    EPIMContactOrg,
    EPIMContactExtDepartment,
    EPIMContactTitle,
    EPIMContactExtAssistantName,
    EPIMContactExtSpouse,
    EPIMContactExtChildren,
    EPIMContactUrl,
    EPIMContactAddr,
    EPIMContactBirthday,
    EPIMContactExtAnniversary,
    EPIMContactNote,
    EPIMContactClass,
    EPIMContactExtWvUserId,
    EPIMContactUid,
    EPIMContactRevision
};

/**
 * Number of supported Contact fields by S60 platform
 */
const TInt KPIMSupportedFieldsCount = sizeof(KPIMSupportedFields)
                                      / sizeof(TPIMField);

/**
 * Maximum number of values for all standard PIM API Contact fields.
 *
 * The field constants are numbered from 100 to 118; thus they can be
 * mapped to a table starting from 0 by reducing 100 from the field
 * constant values.
 *
 * See @ref KPIMContactMaxValueMappingOffset.
 */
const TInt KPIMMaxValues[] =
{
    3, // address
    1, // birthday
    1, // class
    KPIMUnlimitedValues, // email
    0, // formatted address
    0, // formatted name
    1, // name
    1, // nickname
    KPIMUnlimitedValues, // note
    1, // org
    1, // photo
    0, // photo url
    0, // public key
    0, // public key string
    1, // revision
    KPIMUnlimitedValues, // tel
    KPIMUnlimitedValues, // title
    1, // uid
    KPIMUnlimitedValues // url
};

/**
 * Maximum number of values for all extended PIM API Contact fields.
 *
 * The field constants are numbered from 0x1005001 to 0x1005003;
 * thus they can be mapped to a table starting from 0 by reducing 0x1005001
 * from the field constant values.
 *
 * See @ref KPIMContactMaxExtValueMappingOffset.
 */
const TInt KPIMMaxExtValues[] =
{
    1, // Extended WV User ID
    KPIMUnlimitedValues, // Extended SIP
    KPIMUnlimitedValues, // Extended DTMF
    1, // Extended Department
    1, // Extended Assistant name
    1, // Extended Children
    1, // Extended Spouse
    1, // Extended Anniversary
    KPIMUnlimitedValues, // Extended VOIP
    KPIMUnlimitedValues, // Extended PTT
    KPIMUnlimitedValues // Extended SWIS
};

/**
 * Smallest Contact standard field constant.
 */
const TInt KPIMContactSmallestField = 100;

/**
 * Largest Contact standard field constant.
 */
const TInt KPIMContactLargestField = 118;

/**
 * The offset for mapping Contact standard field constant values to
 * elements in @ref KPIMMaxValues table.
 */
const TInt KPIMContactMaxValueMappingOffset = 100;

/**
 * Smallest Contact extended field constant.
 */
const TInt KPIMContactSmallestExtField = 0x1005001;

/**
 * Largest Contact extended field constant.
 */
const TInt KPIMContactLargestExtField = 0x100500B;
/**
 * The offset for mapping Contact extended field constant values to
 * elements in @ref KPIMMaxExtValues table.
 */
const TInt KPIMContactMaxExtValueMappingOffset = 0x1005001;

/**
 * Number of Contact fields in PIM API.
 */
const TInt KPIMFieldCount = sizeof(KPIMMaxValues) / sizeof(TInt)
                            + sizeof(KPIMMaxExtValues) / sizeof(TInt);

// Supported ADDR attributes
const TPIMAttribute KPIMAddrAttributes[] =
{
    EPIMContactAttrHome,
    EPIMContactAttrWork
};

const TPIMAttribute KPIMAddrAttributesCombined = (EPIMContactAttrWork
        | EPIMContactAttrHome);

const TInt KPIMAddrAttributeCount = sizeof(KPIMAddrAttributes)
                                    / sizeof(TPIMAttribute);

// Supported EMAIL attributes
const TPIMAttribute KPIMEmailAttributes[] =
{
    EPIMContactAttrHome,
    EPIMContactAttrWork,
    EPIMContactAttrPreferred
};

const TPIMAttribute KPIMEmailAttributesCombined = (EPIMContactAttrHome
        | EPIMContactAttrWork | EPIMContactAttrPreferred);

const TInt KPIMEmailAttributeCount = sizeof(KPIMEmailAttributes)
                                     / sizeof(TPIMAttribute);

// Supported TEL attributes
const TPIMAttribute KPIMTelAttributes[] =
{
    EPIMContactAttrHome,
    EPIMContactAttrWork,
    EPIMContactAttrMobile,
    EPIMContactAttrAuto,
    EPIMContactAttrExtVideoCall,
    EPIMContactAttrFax,
    EPIMContactAttrPager,
    EPIMContactAttrPreferred,
    EPIMContactAttrSms,
    EPIMContactAttrAsst,
    EPIMContactAttrOther // We "support" this but it is silently discarded
};

const TPIMAttribute KPIMTelAttributesCombined = (EPIMContactAttrHome
        | EPIMContactAttrWork | EPIMContactAttrMobile | EPIMContactAttrAuto
        | EPIMContactAttrExtVideoCall | EPIMContactAttrFax
        | EPIMContactAttrPager | EPIMContactAttrPreferred | EPIMContactAttrSms
        | EPIMContactAttrAsst | EPIMContactAttrOther);

const TInt KPIMTelAttributeCount = sizeof(KPIMTelAttributes)
                                   / sizeof(TPIMAttribute);

// Note: These are only the supported elements
const TPIMArrayElement KPIMNameElements[] =
{
    EPIMContactNamePrefix,
    EPIMContactNameGiven,
    EPIMContactExtGivenNameReading,
    EPIMContactNameOther,
    EPIMContactNameFamily,
    EPIMContactExtFamilyNameReading,
    EPIMContactNameSuffix
};

const TInt KPIMNameElementCount = sizeof(KPIMNameElements)
                                  / sizeof(TPIMArrayElement);

const TInt KPIMNameArraySize = EPIMContactNameNumElements;

// Note: All elements are currently supported
const TInt KPIMAddrElementCount = EPIMContactAddrNumElements;

const TPIMArrayElement KPIMAddrElements[KPIMAddrElementCount] =
{
    EPIMContactAddrPoBox,
    EPIMContactAddrExtra,
    EPIMContactAddrStreet,
    EPIMContactAddrPostalCode,
    EPIMContactAddrLocality,
    EPIMContactAddrRegion,
    EPIMContactAddrCountry
};

// Supported VOIP attributes
const TPIMAttribute KPIMVoipAttributes[] =
{
    EPIMContactAttrHome,
    EPIMContactAttrWork,
    EPIMContactAttrPreferred
};

const TInt KPIMVoipAttributeCount = sizeof(KPIMVoipAttributes)
                                    / sizeof(TPIMAttribute);

// FORWARD DECLARATIONS
class MPIMItem;

// CLASS DECLARATION

/**
 * This class provides access to the PIM Contacts Model list
 */

NONSHARABLE_CLASS(CPIMCMAdapterManager): public CBase,
        public MPIMContactAdapterManager,
        public MPIMAdapterManager
{
public: // Constructors and a destructor

    /**
     * Two-phased constructor.
     *
     * @param aListName List name.
     */
    static CPIMCMAdapterManager* NewL(const TDesC& aListName);

    /**
     * destructor
     */
    ~CPIMCMAdapterManager();

public: // MPIMAdapterManager

    const TDesC& ListNameL();

    /**
     * Provides number of maximum categories supported by the list adapter.
     * @return Number of maximum supported categories .
     */
    TInt MaxCategories();

    /**
     * Checks whether a specific field is supported by the list adapter.
     * @param aField The field to be checked.
     * @return ETrue if the given field is supported, EFalse otherwise.
     */
    TBool IsSupportedField(TPIMField aField);

    /**
     * Provides all fields supported by the list adapter.
     *
     * @return An array containing all supported fields.
     *
     * @par Leaving:
     * The method leaves on error. Such error always means that the adapter
     * is non-functional.
     */
    const CArrayFix<TPIMField>& GetSupportedFieldsL();

    /**
     * Checks whether a specific attribute is supported by the list adapter.
     * @param aAttribute The attribute to be checked.
     * @return ETrue if the given attribute is supported, EFalse otherwise.
     */
    TBool IsSupportedAttribute(TPIMField aField,
                               TPIMAttribute aAttribute);

    /**
     * Provides all attributes supported by the list adapter.
     *
     * @return An array containing all supported attributes.
     *
     * @par Leaving:
     * The method leaves on error. Such error always means that the adapter
     * is non-functional.
     */
    const CArrayFix<TPIMAttribute>& GetSupportedAttributesL(
        TPIMField aField);

    /**
     * Provides attributes supported for a field, combined into single
     * integer value.
     *
     * @param aField The field to which the attributes are associated with.
     *
     * @return An integer holding the combination of all attributes
     *         supported for \a aField.
     *
     * @par Leaving.
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aField is not valid.
     */
    TPIMAttribute GetSupportedAttributesCombinedL(
        TPIMField aField);

    /**
     * Provides all attributes supported for all fields, combined into
     * single integer value.
     *
     * @return An integer holding the combination of all attributes
     *         supported for all fields.
     */
    TPIMAttribute GetAllSupportedAttributesCombined();

    /**
     * Checks whether a specific array element is supported by the list adapter.
     * @param aArrayElement The array element to be checked.
     * @return ETrue if the given array element is supported, EFalse otherwise.
     */
    TBool IsSupportedArrayElement(TPIMField aStringArrayField,
                                  TPIMArrayElement aArrayElement);

    /**
     * Provides all array elements supported by the list adapter.
     *
     * @return An array containing all supported array elements.
     *
     * @par Leaving:
     * The method leaves on error. Such error always means that the adapter
     * is non-functional.
     */
    const CArrayFix<TPIMArrayElement>& GetSupportedArrayElementsL(
        TPIMField aStringArrayField);

    /**
     * Provides the number of maximum values supported for a specific field.
     * @param aField The field to be evaluated.
     * @return Number of values supported for the field.
     */
    TInt MaxValues(TPIMField aField);

    /**
     * Provides the array size of a specific string array field.
     * The values in a string array field are arrays themselves.
     * StringArraySize method returns the number of elements in every value
     * of the field.
     *
     * @param aStringArrayField The string array field to be evaluated.
     * @return Number of elements in a single string array value.
     *
     * @par Leaving:
     * The method leaves with \c KErrArgument if \a aStringArrayField
     * is not a string array field.
     */
    TInt StringArraySizeL(TPIMField aStringArrayField);

    /**
     * Provides pointer to a function which implements an algorithm that
     * determines the order of two items.
     *
     * @return Item comparison function.
     */
    TPIMItemComparisonFunc ItemOrder();

public: // MPIMContactAdapterManager

    /**
     * Provides access to the \ref MPIMAdapterManager representation of
     * this object.
     *
     * @return The \ref MPIMAdapterManager representation of this object.
     */
    MPIMAdapterManager* GetAdapterManager();

public: // new methods

    /**
     * Compares two MPIMItemData objects.
     * Used for determining
     * their ordering in an item enumeration.
     *
     * @param aFirst first object to compare
     * @param aSecond second object to compare
     * @return
     * @li < 0, if aFirst becomes \b before aSecond
     * @li 0, if the objects are equal in the ordering
     * @li > 0, if aFirst becomes \b after aSecond
     */
    static TInt ItemComparisonFunc(const MPIMItem& aFirst,
                                   const MPIMItem& aSecond);

    /**
     * Compares two MPIMItemData objects.
     * Used for determining
     * their ordering in an item enumeration.
     *
     * @param aFirst first object to compare
     * @param aSecond second object to compare
     * @return
     * @li < 0, if aFirst becomes \b before aSecond
     * @li 0, if the objects are equal in the ordering
     * @li > 0, if aFirst becomes \b after aSecond
     */
    static TInt ItemComparisonFuncL(const MPIMItem& aFirst,
                                    const MPIMItem& aSecond);

protected: // New methods

    /**
     * Provides data type of a specific field.
     * @param aField The field to be evaluated.
     * @return Field type.
     */
    TPIMFieldDataType GetFieldDataTypeL(TPIMField aField);

protected:
    /**
     * C++ default constructor.
     */
    CPIMCMAdapterManager();

    /**
     * Symbian 2nd phase constructor
     */
    void ConstructL(const TDesC& aListName);

private: // private methods
    TBool IsSupportedAttributeL(TPIMField aField,
                                TPIMAttribute aAttribute);
    TBool IsSupportedArrayElementL(TPIMField aArrayField,
                                   TPIMArrayElement aElement);

private: // members

    /** List name, owned. The name does not change after initialization. */
    TDesC* iListName;

    // List of supported fields by this adapter, owned.
    CArrayFixFlat<TPIMField>* iSupportedFields;

    // List of attributes for those fields which support no attributes,
    // owned.
    CArrayFixFlat<TPIMAttribute>* iNoAttributes;

    // List of attributes for address and url fields, owned.
    CArrayFixFlat<TPIMAttribute>* iAddrAttributes;

    // List of attributes for the email field, owned.
    CArrayFixFlat<TPIMAttribute>* iEmailAttributes;

    // List of attributes for the tel field, owned.
    CArrayFixFlat<TPIMAttribute>* iTelAttributes;

    // List of supported name array elements, owned.
    CArrayFixFlat<TPIMArrayElement>* iNameElements;

    // List of supported address array elements, owned.
    CArrayFixFlat<TPIMArrayElement>* iAddrElements;

    // List of attributes for the voip field, woned.
    CArrayFixFlat<TPIMAttribute>* iVoipAttributes;

};

#endif // CPIMCMADAPTERMANAGER_H
// End of File
