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
* Description:  Interface class to access framework item data
 *
*/


#ifndef MPIMITEMDATA_H
#define MPIMITEMDATA_H

// INTERNAL INCLUDES
#include "tpimfielddata.h"

// EXTERNAL INCLUDES
#include <e32std.h>

/**
 * This class representates one PIM item data type which
 * can be used through this interface. This interface had
 * to be represented due to the need for a raw access to
 * PIM item data. So, this interface is used by list adapters
 * to fill an item without doing extra checks for the data
 * (it must be expected that adapters now what data they are adding)
 *
 * @since S60 v3.1
 */
NONSHARABLE_CLASS(MPIMItemData)
{
public:

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
    virtual void SetValueL(const TPIMFieldData& aFieldData) = 0;

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
    virtual void AddValueL(const TPIMFieldData& aFieldData) = 0;

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
    virtual const TPIMFieldData ValueL(const TPIMField aField,
                                       const TInt aIndex) const = 0;

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
    virtual void RemoveValueL(const TPIMField aField, const TInt aIndex) = 0;

    /**
     * Counts the number of values in a specific field. The values
     * are counted as indexes within this field. If there is no requested
     * field, this function returns zero
     *
     * @param aField The field values of which are to be counted.
     * @return Number of values in field \a aField.
     *         Zero if there is no values
     */
    virtual TInt CountValues(const TPIMField aField) const = 0;

    /**
     * Returns an array of fields that currently have more than zero values
     * The array is left to the cleanup stack
     *
     * @return Array of fields. The ownership of the array is transferred
     *         to the caller
     */
    virtual CArrayFix<TPIMField>* FieldsLC() const = 0;

    /**
     * Adds new label to an existing field
     *
     * @param aField Field to which the label is added
     * @param aIndex Value index of the label
     * @param aArrayIndex Array index of the label
     * @param aLabel New label
     */
    virtual void SetLabelL(const TPIMField aField, const TInt aIndex,
                           const TInt aArrayIndex, const TDesC& aLabel) = 0;

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
    virtual const TPtrC LabelL(const TPIMField aField, const TInt aIndex,
                               const TInt aArrayIndex) const = 0;

    /**
     * Clears all fields from this item data.
     */
    virtual void Reset() = 0;

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
    virtual void SetAttributesL(const TPIMField aField, const TInt aIndex,
                                TPIMAttribute aAttributes) = 0;

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
    virtual TInt
    AttributesL(const TPIMField aField, const TInt aIndex) const = 0;

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
    virtual void SetInternalAttributesL(const TPIMField aField,
                                        const TInt aIndex, CArrayFix<TUid>* aInternalAttributes) = 0;

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
    virtual const CArrayFix<TUid>& InternalAttributesL(const TPIMField aField,
            const TInt aIndex) const = 0;

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
    virtual TInt FindCategory(const TDesC& aCategoryName) const = 0;

    /**
     * AddNewCategoryL
     * Adds new category to the category list
     *
     * @param aCategoryName Category name which is to be added
     */
    virtual void AddNewCategoryL(const TDesC& aCategoryName) = 0;

    /**
     * RemoveCategory
     * Removes a category from the category list
     * Returns ETrue if the category was found and removed
     * and EFalse if there was no such category
     *
     * @param aCategoryName Category name which is to be removed
     * @return ETrue if the category was removed, EFalse if not
     */
    virtual TBool RemoveCategory(const TDesC& aCategoryName) = 0;

    /**
     * Categories
     * Returns a reference to the categories associeated
     * with this item data
     *
     * @return A reference to the list of categories
     */
    virtual const CDesCArray& Categories() const = 0;

protected:

    // Prohibit destruction using this interface
    virtual ~MPIMItemData()
    {}
}
;

#endif // MPIMITEMDATA_H
// End of file
