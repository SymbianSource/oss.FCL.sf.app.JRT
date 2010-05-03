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
 *                Model Contact Cards.
 *
*/


#ifndef CPIMCONTACTITEMADAPTER_H
#define CPIMCONTACTITEMADAPTER_H

// INTERNAL INCLUDES
#include "pimcommon.h"
#include "pimcontact.h"

// EXTERNAL INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <cntdef.h>

// CONSTANTS

/** Maximum lengths of address elements in S60 */
const TInt KContactAddrElemMaxLengths[EPIMContactAddrNumElements] =
{
    20, // P.O.Box
    50, // Extra/Extension
    50, // Street
    50, // City
    50, // State/Region
    20, // Postal code
    50 // Country
};

/** Maximum lengths of name elements in S60 */
const TInt KContactNameElemMaxLengths[EPIMContactNameNumElements] =
{
    50, // Family/surname
    50, // Given/First name
    50, // Other
    10, // Prefix
    10, // Suffix
    50, // Family name reading
    50 // Given name reading
};

/**
 * Maximum lengths of standard string field values in S60.
 * An offset to the table is found by substracting
 * KContactStringMaxLengthsOffset from a field id.
 */
const TInt KContactStringMaxLengths[] =
{
    KErrNotFound, // EPIMContactAddr
    KErrNotFound, // EPIMContactBirthday
    KErrNotFound, // EPIMContactClass
    150, // EPIMContactEmail
    KErrNotSupported, // EPIMContactFormattedAddr
    KErrNotSupported, // EPIMContactFormattedName
    KErrNotFound, // EPIMContactName
    50, // EPIMContactNickname
    1000, // EPIMContactNote
    50, // EPIMContactOrg
    KErrNotFound, // EPIMContactPhoto
    KErrNotFound, // EPIMContactPhotoUrl
    KErrNotSupported, // EPIMContactPublicKey
    KErrNotSupported, // EPIMContactPublicKeyString
    KErrNotFound, // EPIMContactRevision
    48, // EPIMContactTel
    50, // EPIMContactTitle
    KErrNotFound, // EPIMContactUid
    1000 // EPIMContactUrl
};

const TInt KContactStringMaxLengthsOffset = 100;
const TInt KContactStringMaxLengthsLen = sizeof(KContactStringMaxLengths)
        / sizeof(TInt);

/**
 * Maximum lengths of extended string field values in S60.
 */
const TInt KContactExtStringMaxLengths[] =
{
    50, // Extended WV user ID
    100, // Extended SIP
    60, // Extended DTFM
    50, // Extended Department
    50, // Extended Assistant name
    50, // Extended Children
    50, // Extended Spouse
    KErrNotFound, // Extended Anniversary
    100, // Extended VOIP
    100, // Extended PTT
    100 // Extended SWIS
};

const TInt KContactExtStringMaxLengthOffset = 0x1005001;
const TInt KContactExtStringMaxLengthsLen = sizeof(KContactExtStringMaxLengths)
        / sizeof(TInt);

// FORWARD DECLARATIONS
class CPIMContactCategoryManager;
class CContactCard;
class CViewContact;
class RContactViewSortOrder;
class MPIMContactItem;
class MPIMItemData;
class CContentType;
class CContactItemField;
class CPIMImageScaler;
class CPIMContactFieldInfo;

// CLASS DECLARATION

/**
 * Contact Item Adapter
 * Converts Contact Items from PIM representation to Contacts Model
 * representation and vice versa.
 */
NONSHARABLE_CLASS(CPIMContactItemAdapter): public CBase
{
public: // Constructor and destructor
    /**
     * Two-phased constructor.
     */
    static CPIMContactItemAdapter* NewL(
        CPIMContactCategoryManager& aCategoryManager);

    /**
     * Destructor.
     */
    ~CPIMContactItemAdapter();

public: // new methods
    /**
     * Converts from a PIM item data to a Contacts Model contact card.
     *
     * @param aItem The item to convert
     *
     * @return A contact card with the same information as given in
     *         the parameter item. The \b ownership of the card is
     *         transferred to the caller.
     *
     * @par Leaving:
     * This leaves on an error with system wide error codes.
     * @par NOTE!
     * The debug version of this method panics if the item contains
     * unsupported fields!
     */
    CContactCard* CardL(const MPIMContactItem& aItem);

    /**
     * Converts from a Contacts Model contact card to the PIM
     * representation.
     *
     * @param aItem Item data container to fill with the data
     * @param aCard Contact card to convert
     *
     * @par Leaving:
     *  - KErrNotFound: The contact belongs to a group which either does not
     *                  exist, or does not have a label. In other words, the
     *                  database has been corrupted.
     *  - This method may also leave with other system wide error codes.
     */
    void FillItemL(MPIMContactItem& aItem, const CContactCard& aCard);


    /**
     * Updates the data in the Contacts Model contact card to match
     * the data in the PIM item data container.
     *
     * @param aItem Item from which the data is read
     * @param aCard The card which is updated
     *
     * @par Leaving:
     * This leaves on an error with system wide error codes.
     * @par NOTE!
     * The debug version of this method panics if the item contains
     * unsupported fields!
     */
    void UpdateCardL(const MPIMContactItem& aItem,
                     CContactCard& aCard);

    /**
     * Updates the category information in the database.
     *
     * @param aItem Item which contains the category information which
     *              is used in the update.
     * @param aCard the card which is updated.
     *
     * @par Leaving:
     *  - KErrNotFound: There is something fishy about the groups in the
     *                  database.
     */
    void UpdateCategoriesL(const MPIMContactItem& aItem,
                           const CContactCard& aCard);

    /**
     * Returns Contacts model field which is mapped to the PIM contact
     * field. Ownership of the returned array is transferred to the caller
     *
     * @return Mapped Contacts Model fields
     */
    CArrayFix< TInt>* ContactsModelFieldTypeL(
        TPIMContactField aContactField) const;

private:
    /**
     * C++ constructor.
     */
    CPIMContactItemAdapter(CPIMContactCategoryManager& aCategoryManager);

    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL();

    /**
     * Deletes all the supported fields from the contact card.
     * Those fields which are not supported are left as is.
     *
     * @param aCard The card which is cleaned
     *
     */
    void CleanCard(CContactCard& aCard);

    /**
     * Checks if we support this particular contact field.
     *
     * @param aField The field which is checked
     *
     * @return ETrue if the field is supported, EFalse otherwise
     *
     */
    TBool IsSupportedField(CContactItemField& aField);

    /**
     * Converts a field from a PIM Item to a Contact Card.
     *
     * @param aItem item to read the field from
     * @param aCard card to write the field to
     * @param aField field to convert
     *
     * @par Leaving:
     * This method leaves on error with system wide error codes.
     * @par NOTE!
     * The debug version of this method panics if the field is unsupported!
     */
    void ConvertFieldL(const MPIMContactItem& aItem,
                       CContactCard& aCard,
                       TPIMContactField aField);

    /**
     * Converts a field from a Contact Card to a PIM Item
     *
     * @param aField field to read the data from
     * @param aItem item to write the data to
     *
     * @par Leaving:
     * This method leaves on an error with system wide error codes.
     */
    void ConvertFieldL(const CContactItemField& aField,
                       MPIMContactItem& aItem);

    /**
     * Converts a text field from a Contact Card to a PIM Item
     *
     * @param aField field to read the data from
     * @param aItem item to write the data to
     *
     * @par Leaving:
     * This method leaves on error with system wide error codes
     */
    void ConvertTextFieldL(const CContactItemField& aField,
                           MPIMContactItem& aItem);

    /**
     * Adds a string field to the item, and optionally sets the
     * fields label.
     *
     * @param aItem item to which the string field is added
     * @param aField field type
     * @param aAttributes attributes of the field to be added
     * @param aInternalAttributes Internal attributes of the field
     * @param aValue value to be added. Ownership gets transferred
     *        to the item.
     * @param aLabel label to be set. NULL if default label is to
     * be used. Ownership gets transferred.
     */
    void AddStringFieldWithLabelL(
        MPIMItemData& aItem,
        TPIMField aField,
        TPIMAttribute aAttributes,
        CArrayFix< TUid>* aInternalAttributes,
        const TPtrC aValue,
        const TPtrC aLabel);

    /**
     * Converts a date field from a Contact Card to a PIM Item
     *
     * @param aField field to read the data from
     * @param aItem item to write the data to
     *
     * @par Leaving:
     * This method leaves on error with system wide error codes
     */
    void ConvertDateFieldL(const CContactItemField& aField,
                           MPIMContactItem& aItem);

    /**
     * Converts a store field from a Contact Card to a PIM Item
     *
     * @param aField field to read the data from
     * @param aItem item to write the data to
     *
     * @par Leaving:
     * This method leaves on error with system wide error codes
     */
    void ConvertStoreFieldL(const CContactItemField& aField,
                            MPIMContactItem& aItem);

    /**
     * Adds a name field to a PIM Item
     *
     * @param aText text to add
     * @param aItem item to add the field to
     * @param aElement element which is added
     * @param aLabel lable to be set
     *
     * @par Leaving:
     * This method leaves on an error with system wide error codes.
     */
    void AddNameFieldToItemL(
        TPtrC aText,
        MPIMContactItem& aItem,
        TPIMContactNameElement aElement,
        TPtrC aLabel);

    /**
     * Adds an addres field to a PIM Item
     *
     * @param aText text to add
     * @param aItem item to add the field to
     * @param aElement element which is added
     * @param aAttributes attributes of the address field
     * @param aLabel label to be set
     *
     * @par Leaving:
     * This method leaves on an error with system wide error codes.
     */
    void AddAddressFieldToItemL(
        TPtrC aText,
        MPIMContactItem& aItem,
        TPIMContactAddrElement aElement,
        TPIMAttribute aAttributes,
        TPtrC aLabel);

    /**
     * Converts an address field from a PIM Item to a Contact Card.
     *
     * @param aItem item to read the field from
     * @param aCard card to write the field to
     * @param aField field to convert
     * @param aIndex index to the field to convert
     * @param aAttributes attributes of the field to convert
     *
     * @par Leaving:
     * This method leaves on an error with system wide error codes.
     */
    void ConvertAddressFieldL(
        const MPIMItemData& aItem,
        CContactCard& aCard,
        TPIMContactField aField,
        TInt aIndex,
        TPIMAttribute aAttributes);

    /**
     * Converts a name field from a PIM Item to a Contact Card.
     *
     * @param aItem item to read the field from
     * @param aCard card to write the field to
     * @param aField field to convert
     * @param aIndex index to the field to convert
     *
     * @par Leaving:
     * This method leaves on an error with system wide error codes.
     */
    void ConvertNameFieldL(
        const MPIMItemData& aItem,
        CContactCard& aCard,
        TPIMContactField aField,
        TInt aIndex);

    /**
     * Converts a date field from a PIM Item to a Contact Card.
     *
     * @param aItem item to read the field from
     * @param aCard card to write the field to
     * @param aField field to convert
     * @param aIndex index to the field to convert
     *
     * @par Leaving:
     * This method leaves on an error with system wide error codes.
     * @par NOTE!
     * The debug version of this method panics if the date is not a birthday!
     */
    void ConvertDateFieldL(
        const MPIMItemData& aItem,
        CContactCard& aCard,
        TPIMContactField aField,
        TInt aIndex);

    /**
     * Converts a string field from a PIM Item to a Contact Card.
     *
     * @param aItem item to read the field from
     * @param aCard card to write the field to
     * @param aField field to convert
     * @param aIndex index to the field to convert
     *
     * @par Leaving:
     * This method leaves on an error with system wide error codes.
     */
    void ConvertStringFieldL(
        const MPIMItemData& aItem,
        CContactCard& aCard,
        TPIMContactField aField,
        TInt aIndex);

    /**
     * Converts a photo field from a PIM Item to a Contact Card.
     *
     * @param aItem item to read the field from
     * @param aCard card to write the field to
     * @param aField field to convert
     * @param aIndex index to the field to convert
     *
     * @par Leaving:
     * This method may leave.
     */
    void ConvertPhotoFieldL(
        const MPIMItemData& aItem,
        CContactCard& aCard,
        TPIMContactField aField,
        TInt aIndex);

    /**
     * Converts a class field from a PIM Item to a Contact Card.
     *
     * @param aItemData item to read the field from
     * @param aCard card to write the field to
     * @param aField field to convert
     * @param aIndex index to the field to convert
     *
     * @par Leaving:
     * This method may leave.
     */
    void ConvertClassFieldL(
        const MPIMItemData& aItemData,
        CContactCard& aCard,
        TPIMContactField aField,
        TInt aIndex);

    /**
     * Converts a class field from a Contact Card to PIM item
     *
     * @param aItemData Item to add the field
     * @param aClassDesValue Pointer descriptor to the native value
     * @param aLabel Label for the field
     *
     * @par Leaving
     * This method may leve
     */
    void AddClassFieldToItemL(
        MPIMItemData& aItemData,
        TPtrC& aClassDesValue,
        TPtrC aLabel);

    /**
     * Checks the attributes in the address values in the given
     * item, and finds out optimal values for address index
     * members.
     * @param aItem item which is checked
     * @param aPlainAddressIndex the index of address without
     * attributes is placed here
     * @param aHomeAddressIndex the index of address with home
     * attribute is placed here
     * @param aWorkAddressIndex the index of address with work
     * attribute is placed here
     */
    void AddressAttributesL(const MPIMItemData& aItem,
                            TInt& aPlainAddressIndex,
                            TInt& aHomeAddressIndex,
                            TInt& aWorkAddressIndex);

    /**
     * Checks that given string value does not exceed the limit
     * specified for the given field.
     */
    void EnsureValidStringValueLengthL(
        TPIMContactField aField,
        const TDesC& aValue);

    /**
     * Checks that given address array element does not exceed
     * a predefined length limit.
     */
    void EnsureValidAddrElemLengthL(
        TPIMContactAddrElement aElemIndex,
        const TDesC& aElemValue);

    /**
     * Checks that given name array element does not exceed
     * a predefined length limit.
     */
    void EnsureValidNameElemLengthL(
        TPIMContactNameElement aElemIndex,
        const TDesC& aElemValue);

private: // members

    // Handles categories
    CPIMContactCategoryManager& iCategoryManager;

    // Scales bitmaps, owned.
    CPIMImageScaler* iImageScaler;

    // Matches Contacts Model fields to PIM contact fields
    CPIMContactFieldInfo* iFieldInfo;

};

#endif // CPIMCONTACTITEMADAPTER_H
// End of file
