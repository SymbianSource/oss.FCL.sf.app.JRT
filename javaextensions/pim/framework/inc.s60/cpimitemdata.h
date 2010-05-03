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
* Description:  Class for holding PIM item data
 *
*/


#ifndef CPIMITEMDATA_H
#define CPIMITEMDATA_H

// INTERNAL INCLUDES
#include "mpimitemdata.h"

// EXTERNAL INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class CPIMField;
class TPIMFieldData;

/**
 *  This class representates one PIM item data. This is the actual
 *  storage class of the data values of one PIM fields. MPIMItemData
 *  is an interface which provides access to this class
 *
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS(CPIMItemData) : public CBase,
        public MPIMItemData
{
public:

    /**
     * Two-phased constructor.
     *
     * @return New instance of this class.
     */
    static CPIMItemData* NewL();

    /**
     * Two-phased constructor.
     *
     * @return New instance of this class. The instance
     *         is pushed to the cleanup stack
     */
    static CPIMItemData* NewLC();

    /**
     * Destructor
     */
    virtual ~CPIMItemData();

public: // Methods from MPIMItemData

    /**
     * Sets PIM field data with certain index
     *
     * This function is used to set all types of PIM
     * data. The TPIMFieldData class encapsulates all
     * possible data about one field so we don't need
     * separate functions for each PIM field type
     *
     * @param aFieldData Reference to data class which
     *        all needed information about the new value
     *
     * @par Leaving
     * @li KErrNotFound - The requested field has no value
     */
    void SetValueL(const TPIMFieldData& aFieldData);

    /**
     * Adds new PIM field data to certain field
     *
     * This function is used to add all types of PIM
     * data. The TPIMFieldData class encapsulates all
     * possible data about one field so we don't need
     * separate functions for each PIM field type
     *
     * NOTE: The index value in the argument class is ignored
     *
     * @param aFieldData Reference to data class which
     *        all needed information about the new value
     *
     * @par Leaving:
     * The method leaves on error. Error codes are interpreted as follows:
     * @li \c KErrNotFound - There is not value for given \a aIndex.
     * @li Other - The value could not be removed cleanly. This probably
     *     means that the value array has gone corrupt.
     */
    void AddValueL(const TPIMFieldData& aFieldData);

    /**
     * Fetches value from specific value index. The value is
     * capsulated inside TPIMFieldData class to ease an access
     * to different types of values within the PIM item
     *
     * @param aIndex Index of the value in this specific field
     * @return A class holding requested information
     *
     * @par Leaving:
     * The method leaves on error. Error codes are interpreted as follows:
     * @li \c KErrNotFound - There is not value for given \a aIndex.
     * @li Other - The value could not be removed cleanly. This probably
     *     means that the value array has gone corrupt.
     */
    const TPIMFieldData ValueL(const TPIMField aField,
                               const TInt aIndex) const;

    /**
     * Removes a specific value of a specific field.
     *
     * @param aIndex Index of the value that is to be removed.
     *
     * @par Leaving:
     * The method leaves on error. Error codes are interpreted as follows:
     * @li \c KErrNotFound - There is not value for given \a aIndex.
     * @li Other - The value could not be removed cleanly. This probably
     *     means that the value array has gone corrupt.
     */
    void RemoveValueL(const TPIMField aField,
                      const TInt aIndex);

    /**
     * Counts the number of values in a specific field. The values
     * are counted as indexes within this field. If there is no requested
     * field, this function returns zero
     *
     * @param aField The field values of which are to be counted.
     * @return Number of values in field \a aField.
     *         Zero if there is no values
     */
    TInt CountValues(const TPIMField aField) const;

    /**
     * Returns an array of fields that currently have more than zero values
     * The array is left to the cleanup stack
     *
     * @return Array of fields. The ownership of the array is transferred
     *         to the caller
     */
    CArrayFix< TPIMField>* FieldsLC() const;

    /**
     * Adds new label to an existing field
     *
     * @param aField Field to which the label is added
     * @param aIndex Value index of the label
     * @param aArrayIndex Array index of the label
     * @param aLabel New label
     */
    void SetLabelL(
        const TPIMField aField,
        const TInt aIndex,
        const TInt aArrayIndex,
        const TDesC& aLabel);

    /**
     * Retrieves the label from the specific field with specific
     * field and array indexes.
     *
     * @param aField Field from which the label is retrieved
     * @param aIndex Index of the field's value
     * @param aArrayIndex Index of the array index if this is a string
     *        array field
     *
     * @return Label name
     */
    const TPtrC LabelL(
        const TPIMField aField,
        const TInt aIndex,
        const TInt aArrayIndex) const;

    /**
     * Clears all fields from this item data.
     */
    void Reset();

    /**
     * Sets new attributes for a specific field and value index
     *
     * @param aField Field for which the attributes are to be added
     * @param aIndex Index of the value in this specific field
     *
     * @par Leaving:
     * The method leaves on error. Error codes are interpreted as follows:
     * @li \c KErrNotFound - There is not value for given \a aIndex.
     * @li Other - The value could not be removed cleanly. This probably
     *     means that the value array has gone corrupt.
     */
    void SetAttributesL(
        const TPIMField aField,
        const TInt aIndex,
        TPIMAttribute aAttributes);

    /**
     * Retrieves attributes from an index within a field
     *
     * @param aField Field from which the attributes are to fetched
     * @param aIndex Value index of the field
     * @return Attributes of this value index
     *
     * @par Leaving:
     * The method leaves on error. Error codes are interpreted as follows:
     * @li \c KErrNotFound - There is not value for given \a aIndex.
     * @li Other - The value could not be removed cleanly. This probably
     *     means that the value array has gone corrupt.
     */
    TInt AttributesL(
        const TPIMField aField,
        const TInt aIndex) const;

    /**
     * Sets internal attributes to an index within a field.
     * Internal attributes are not accessible from the Java-side
     * so this function provides only access to set internal
     * attributes
     *
     * @param aField Field from which the attributes are to fetched
     * @param aIndex Value index of the field
     * @param aInternalAttributes New internal attributes. Old ones
     *        will be overwriten
     *
     * @par Leaving:
     * The method leaves on error. Error codes are interpreted as follows:
     * @li \c KErrNotFound - There is not value for given \a aIndex.
     * @li Other - The value could not be removed cleanly. This probably
     *     means that the value array has gone corrupt.
     */
    void SetInternalAttributesL(
        const TPIMField aField,
        const TInt aIndex,
        CArrayFix< TUid>* aInternalAttributes);

    /**
     * Retrieves internal attributes from an index within a field.
     * Internal attributes are not accessible from the Java-side
     * so this function provides only access to retrieve internal
     * attributes
     *
     * @param aField Field from which the attributes are to fetched
     * @param aIndex Value index of the field
     *
     * @return Internal attributes of this value index
     *
     * @par Leaving:
     * The method leaves on error. Error codes are interpreted as follows:
     * @li \c KErrNotFound - There is not value for given \a aIndex.
     * @li Other - The value could not be removed cleanly. This probably
     *     means that the value array has gone corrupt.
     */
    const CArrayFix< TUid>& InternalAttributesL(
        const TPIMField aField,
        const TInt aIndex) const;

    /**
     * FindCategory
     * Finds a category from the category list
     * If the requested category was not found, KErrNotFound is
     * returned
     *
     * @param aCategoryName Category name to find
     * @return If found, the index of the category and KErrNotFound
     *         if the category was not found
     */
    TInt FindCategory(const TDesC& aCategoryName) const;

    /**
     * AddNewCategoryL
     * Adds new category to the category list
     *
     * @param aCategoryName Category name which is to be added
     */
    void AddNewCategoryL(const TDesC& aCategoryName);

    /**
     * RemoveCategory
     * Removes a category from the category list
     * Returns ETrue if the category was found and removed
     * and EFalse if there was no such category
     *
     * @param aCategoryName Category name which is to be removed
     * @return ETrue if the category was removed, EFalse if not
     */
    TBool RemoveCategory(const TDesC& aCategoryName);

    /**
     * Categories
     * Returns a reference to the categories associeated
     * with this item data
     *
     * @return A reference to the list of categories
     */
    const CDesCArray& Categories() const;

public: // New functions

    /**
     * Check if this item data contains requested field
     * @param aField Field which is to be checked
     * @return ETrue if the field is present, EFalse if not
     */
    TBool HasField(const TPIMField aField) const;

private:

    /**
     * C++ default constructor
     */
    inline CPIMItemData();

    /**
     * Seconds phase constructor
     */
    inline void ConstructL();

private: // New functions

    /**
     * Finds index of the requested field if it is in the
     * field array
     *
     * @param aField requested field
     *
     * @return Index of the field. If not found, KErrNotFound
     *         is returned
     */
    TInt FieldIndex(const TPIMField aField) const;

private: // Data

    /** PIM field array */
    RPointerArray< CPIMField> iFields;

    /** Categories associated with this item data. Owned */
    CDesCArray* iCategories;

};

#endif // CPIMITEMDATA_H
// End of file
