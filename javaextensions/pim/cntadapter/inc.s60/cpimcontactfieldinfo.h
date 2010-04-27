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


#ifndef CPIMCONTACTFIELDINFO_H
#define CPIMCONTACTFIELDINFO_H

// INTERNAL INCLUDES
#include "pimcommon.h"
#include "pimcontact.h"

// EXTERNAL INCLUDES
#include <e32std.h>
#include <cntdef.hrh>

// FORWARD DECLARATIONS
class CContentType;

/**
 * Helper clas to map fields between PIM API and Contacts
 * Model fields.
 */
NONSHARABLE_CLASS(CPIMContactFieldInfo) : public CBase
{
public: // Type definitions

    /**
     * Struct for mapping PIM field to a
     * Contacts Model field
     */
    struct TPIMFieldInfo
    {
public:
        // Constructor
        TPIMFieldInfo(TInt aContactsModelField,
        TPIMContactField aPIMField) :
                iPIMField(aPIMField),
                iSubElement(EPIMFieldInvalid),
                iContactsModelField(aContactsModelField)
        {}

        // Constructor with subelement
        TPIMFieldInfo(TInt aContactsModelField,
        TPIMContactField aPIMField,
        TInt aSubElement) :
                iPIMField(aPIMField),
                iSubElement(aSubElement),
                iContactsModelField(aContactsModelField)
        {}

        // PIM field id
        TPIMContactField iPIMField;
        // Sub field. e.g array element
        TInt iSubElement;
        // Contacts Model field id
        TInt iContactsModelField;
    };

public:

    /**
     * Two phase constructor
     * Returns an instance of this class
     */
    static CPIMContactFieldInfo* NewL();

    /**
     * Two phase constructor
     * Returns an instance of this class. The instance
     * is left to the cleanup stack
     */
    static CPIMContactFieldInfo* NewLC();

    /**
     * Destructor
     */
    ~CPIMContactFieldInfo();

public: // New functions

    /**
     * MatchContactField
     * Matches a Contacts Model field type to
     * PIM item contact field type
     *
     * @param aContactsModelFieldType Contacts Model field type
     * @return Matched PIM contact field
     */
    TPIMContactField MatchContactField(
        TInt aContactsModelFieldType) const;

    /**
     * MatchPIMField
     * Matches a PIM field to a Contacts Model field. Ownership of
     * the returned array is transferred to the caller
     *
     * @param aContactField PIM field
     * @return All matched fields with this PIM contact field.
     *         In addition, one PIM field may have array elements
     *         as sub elements
     */
    CArrayFix< TInt>* MatchPIMFieldL(TPIMContactField
                                     aContactField) const;

    /**
     * MatchContactNameElement
     * Matches a Contacts Model field type to PIM contact
     * name/address element. This function should be use when
     * the actual field type is known
     *
     * @param aContactModelFieldType Contacts Model field type
     * @return Matched PIM constact name/address field array element
     */
    TInt MatchContactArrayElement(TInt aContactsModelFieldType) const;

    /**
     * FieldAttributes
     * Returns the attributes of the specific Contacts Model
     * field.
     *
     * @param aContentType Content type from which to get attributes
     * @return Attributes of the passed content type
     */
    TPIMAttribute FieldAttributes(const CContentType& aContentType) const;

    /**
     * GetFieldInternalAttributesL
     * Returns the internal attributes of the specific Contacts Model
     * field.
     *
     * @param aContentType Content type from which to get attributes
     * @param aArrayOfAttributes Attributes array which fields will be added
     * @return Attributes of the passed content type
     */
    void GetFieldInternalAttributesL(
        const CContentType& aContentType,
        CArrayFix< TUid>& aArrayOfAttributes) const;

    /**
     * Creates a content type for a specific address element.
     *
     * @param aElement PIM Address field element
     * @param aCategory     Phonebook category for the field.
     *                      Values can be EContactCategoryNone,
     *                      EContactCategoryHome,
     *                      EContactCategoryWork or
     *                      EContactCategoryOther
     *
     * @return The requested Content Type. The ownership of the object
     *      is transferred to the caller. The object is left in the
     *      cleanup stack.
     *
     * @par Leaving:
     *      This leaves if theres an error with a system wide error code
     */
    CContentType* AddressContentTypeL(
        TPIMContactAddrElement aElement,
        EContactFieldCategory aCategory) const;

    /**
     * Creates a content type for a specific name element.
     *
     * @param aElement PIM Address field element
     *
     * @return The requested Content Type. The ownership of the object
     *      is transferred to the caller.
     *
     * @par Leaving:
     *      This leaves if theres an error with a system wide error code
     */
    CContentType* NameContentTypeL(TPIMContactNameElement aElement) const;

    /**
     * Creates a content type for a specific string field
     *
     * @param aContactField Contact field which Content Type
     *        is to be created
     * @param aAttributes Attributes of this field
     * @param aCaterory Category of this field. In return, contains
     *        new category if modification is needed
     * @return The requested Content Type. The ownership is transferred
     *         to the caller
     */
    CContentType* StringFieldContentTypeL(
        const TPIMContactField aContactField,
        const TPIMAttribute aAttributes,
        EContactFieldCategory& aCategory) const;

private:

    /**
     * Default C++ constructor
     */
    inline CPIMContactFieldInfo();

    /**
     * Second phase constructor
     */
    void ConstructL();

private: // Data

    // Array holding fields info
    CArrayFix< TPIMFieldInfo>* iFieldsInfo;

};

#endif // CPIMCONTACTFIELDINFO_H
// End of file
