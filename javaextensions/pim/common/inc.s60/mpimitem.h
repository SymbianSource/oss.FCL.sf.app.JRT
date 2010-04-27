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
* Description:  Access interface to PIM item common data representation.
 *
*/


#ifndef MPIMITEM_H
#define MPIMITEM_H

//  INCLUDES
#include "pimcommon.h"
#include "bamdesca.h" // For MDesCArray
#include "badesca.h" // For CDesCArray
/**
 *  Interface for manipulating item data in the Framework.
 *  The interface does
 *  as few checks as possible to provide "raw" access to the data in a
 *  list-independent manner.
 *
 *  Only CBase-derived classes may implement this interface. Deriving other
 *  M-classes from this class is \b prohibited. Deriving specialized M-classes
 *  from this class might result in a "diamond-shaped" inheritance tree,
 *  because it is intended that the common parts of the item implementation
 *  implement this interface. Specialized implementations will be derived
 *  from that common implementation and those must implement the specialized
 *  interfaces (M-classes), too. Diamond-shaped inheritance is highly
 *  discouraged in Symbian OS. The case is resolved so that the M-classes
 *  that would be derived from this class provide a \e getter method, say
 *  \c "GetItemData()", that returns a pointer to this interface. The two
 *  interfaces access the same object, thus providing an "inheritance-like"
 *  accessibility to that object.
 *
 *  When accessing data through this interface, the following things are
 *  checked:
 *  @li Field validity in accordance to the PIM type (Contact, Event, ToDo).
 *      No fields belonging to different PIM type are allowed to be manipulated
 *      through this interface.
 *  @li Field value validity in accordance to the field value type (binary,
 *      boolean, integer, ...). Value types do not mix.
 *  @li Existence of a requested value for given field and index in the value
 *      array when getting a value.
 *  @li Is a field supported by a specific list (adapter) implementation, if
 *      the item belongs to a list.
 *  @li Is an attribute valid for a specific field.
 *  @li Is an attribute supported by a specific list (adapter) implementation,
 *      if the item belongs to a list.
 *  @li Value array maximum sizes, if the item belongs to a list.
 *
 * NOTE: If you use PIM item via this interface, be sure that when modifiying
 *       non-initialized fields with huge amount if item may cause significant
 *       overhead if the data is not available.
 */
NONSHARABLE_CLASS(MPIMItem)
{
public: // Destructor

    /**
     * Destructor.
     */
    virtual ~MPIMItem()
    {}

public: // New functions

    /**
     * Provides all fields that have values in the item.
     *
     * @return An array of fields. The \b ownership of the array is
     *         transferred to the caller. The order of fields is
     *         undefined.
     *
     * @par Leaving:
     * The method leaves on error.
     */
    virtual CArrayFix< TPIMField>* GetFieldsL() const = 0;

    /* --- Binary --- */

    /**
     * Gets specific binary value.
     * The value is Base64-encoded.
     *
     * @param aField Field constant.
     * @param aIndex Value index.
     * @return Byte array value.
     *         \b Ownership of the return value is transferred to caller.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aField is not valid for the implementing
     *     PIM item type.
     * @li \c KErrNotFound - There is no value for given \a aIndex.
     * @li \c KErrNotSupported - The item is associated with a list and
     *     The list does not support \a aField.
     */
    virtual CPIMByteArray* GetBinaryL(TPIMField aField, TInt aIndex) const = 0;

    /**
     * Same as GetBinaryL, but provides the value non-Base64-encoded.
     * Ownership of the return value remains.
     */
    virtual const CPIMByteArray
    & GetBinaryRawL(TPIMField aField, TInt aIndex) const = 0;

    /**
     * Adds a binary value for a binary field. The value is added as the
     * last value in that field's value array.
     * The value must be Base64-encoded.
     *
     * @param aField The field to which the value is added.
     * @param aAttributes The attributes to be set for the value.
     * @param aValue The binary value to be added to the field.
     *        \b Ownership of the value is transferred to the called
     *        object.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aField is not valid for the implementing
     *     PIM item type or \a aValue is NULL.
     * @li \c KErrTooBig - \a aValue is not valid for the field.
     * @li \c KErrNotSupported - The item is associated with a list and
     *     The list does not support \a aField.
     * @li \c KErrOverflow - The field already contains the maximum
     *     number of values it can hold.
     * @li \c KErrLocked - \a aField is read-only.
     * @li Other - The value could not be added.
     */
    virtual void AddBinaryL(TPIMField aField, TPIMAttribute aAttributes,
                            CPIMByteArray* aValue) = 0;

    /**
     * Same as AddBinaryL, but takes the value non-Base64-encoded.
     */
    virtual void AddBinaryRawL(TPIMField aField, TPIMAttribute aAttributes,
                               CPIMByteArray* aValue) = 0;

    /**
     * Sets an existing binary value, discarding the old value.
     * The value must be Base64-encoded.
     *
     * @param aField The field for which the value is set.
     * @param aIndex The index of the value in the value array.
     * @param aAttributes The attributes to be set for the value.
     * @param aValue The value to be set in the place of the old value.
     *        \b Ownership of the new value is transferred to the called
     *        object. The old value is deleted.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aField is not valid for the implementing
     *     PIM item type or \a aValue is NULL.
     * @li \c KErrNotFound - There is not value for given \a aIndex.
     * @li \c KErrTooBig - \a aValue is not valid for the field.
     * @li \c KErrNotSupported - The item is associated with a list and
     *     The list does not support \a aField.
     * @li \c KErrLocked - \a aField is read-only.
     * @li Other - The value could not be set.
     */
    virtual void SetBinaryL(TPIMField aField, TInt aIndex,
                            TPIMAttribute aAttributes, CPIMByteArray* aValue) = 0;

    /**
     * Same as SetBinaryL, but takes the value non-Base64-encoded.
     */
    virtual void SetBinaryRawL(TPIMField aField, TInt aIndex,
                               TPIMAttribute aAttributes, CPIMByteArray* aValue) = 0;

    /* --- Date --- */

    /**
     * Gets specific date value.
     *
     * @param aField Field constant.
     * @param aIndex Value index.
     * @return Date value.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aField is not valid for the implementing
     *     PIM item type.
     * @li \c KErrNotFound - There is no value for given \a aIndex.
     * @li \c KErrNotSupported - The item is associated with a list and
     *     The list does not support \a aField.
     */
    virtual const TPIMDate GetDateL(TPIMField aField, TInt aIndex) const = 0;

    /**
     * Adds a date value for a date field. The value is added as the
     * last value in that field's value array.
     *
     * @param aField The field to which the value is added.
     * @param aAttributes The attributes to be set for the value.
     * @param aValue The date value to be added to the field.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aField is not valid for the implementing
     *     PIM item type.
     * @li \c KErrTooBig - \a aValue is not valid for the field.
     * @li \c KErrNotSupported - The item is associated with a list and
     *     The list does not support \a aField.
     * @li \c KErrOverflow - The field already contains the maximum
     *     number of values it can hold.
     * @li \c KErrLocked - \a aField is read-only.
     * @li Other - The value could not be added.
     */
    virtual void AddDateL(TPIMField aField, TPIMAttribute aAttributes,
                          TPIMDate aValue) = 0;

    /**
     * Sets an existing date value, discarding the old value.
     *
     * @param aField The field for which the value is set.
     * @param aIndex The index of the value in the value array.
     * @param aAttributes The attributes to be set for the value.
     * @param aValue The value to be set in the place of the old value.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aField is not valid for the implementing
     *     PIM item type.
     * @li \c KErrNotFound - There is not value for given \a aIndex.
     * @li \c KErrTooBig - \a aValue is not valid for the field.
     * @li \c KErrNotSupported - The item is associated with a list and
     *     The list does not support \a aField.
     * @li \c KErrLocked - \a aField is read-only.
     * @li Other - The value could not be set.
     */
    virtual void SetDateL(TPIMField aField, TInt aIndex,
                          TPIMAttribute aAttributes, TPIMDate aValue) = 0;

    /* --- Int --- */

    /**
     * Gets specific int value.
     *
     * @param aField Field constant.
     * @param aIndex Value index.
     * @return Integer value.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aField is not valid for the implementing
     *     PIM item type.
     * @li \c KErrNotFound - There is no value for given \a aIndex.
     * @li \c KErrNotSupported - The item is associated with a list and
     *     The list does not support \a aField.
     */
    virtual int getInt(TPIMField aField, int aIndex) const = 0;


    /* --- String --- */

    /**
     * Gets specific string value.
     *
     * @param aField Field constant.
     * @param aIndex Value index.
     * @return String value.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aField is not valid for the implementing
     *     PIM item type.
     * @li \c KErrNotFound - There is no value for given \a aIndex.
     * @li \c KErrNotSupported - The item is associated with a list and
     *     The list does not support \a aField.
     */
    virtual const TDesC& GetStringL(TPIMField aField, TInt aIndex) const = 0;

    /**
     * Adds a string value for a string field. The value is added as the
     * last value in that field's value array.
     *
     * @param aField The field to which the value is added.
     * @param aAttributes The attributes to be set for the value.
     * @param aValue The string value to be added to the field.
     *        \b Ownership of the value is transferred to the called
     *        object.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aField is not valid for the implementing
     *     PIM item type or \a aValue is NULL.
     * @li \c KErrTooBig - \a aValue is not valid for the field.
     * @li \c KErrNotSupported - The item is associated with a list and
     *     The list does not support \a aField.
     * @li \c KErrOverflow - The field already contains the maximum
     *     number of values it can hold.
     * @li \c KErrLocked - \a aField is read-only.
     * @li Other - The value could not be added.
     */
    virtual void AddStringL(TPIMField aField, TPIMAttribute aAttributes,
                            HBufC* aValue) = 0;

    /**
     * Sets an existing string value, discarding the old value.
     *
     * @param aField The field for which the value is set.
     * @param aIndex The index of the value in the value array.
     * @param aAttributes The attributes to be set for the value.
     * @param aValue The value to be set in the place of the old value.
     *        \b Ownership of the value is transferred to the called
     *        object. The old value is deleted.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aField is not valid for the implementing
     *     PIM item type or \a aValue is NULL.
     * @li \c KErrNotFound - There is not value for given \a aIndex.
     * @li \c KErrTooBig - \a aValue is not valid for the field.
     * @li \c KErrNotSupported - The item is associated with a list and
     *     The list does not support \a aField.
     * @li \c KErrLocked - \a aField is read-only.
     * @li Other - The value could not be set.
     */
    virtual void SetStringL(TPIMField aField, TInt aIndex,
                            TPIMAttribute aAttributes, HBufC* aValue) = 0;

    /* --- Boolean --- */

    /**
     * Gets specific boolean value.
     *
     * @param aField Field constant.
     * @param aIndex Value index.
     * @return Boolean value.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aField is not valid for the implementing
     *     PIM item type.
     * @li \c KErrNotFound - There is no value for given \a aIndex.
     * @li \c KErrNotSupported - The item is associated with a list and
     *     The list does not support \a aField.
     */
    virtual TBool GetBooleanL(TPIMField aField, TInt aIndex) const = 0;

    /**
     * Adds a boolean value for a boolean field. The value is added as the
     * last value in that field's value array.
     *
     * @param aField The field to which the value is added.
     * @param aAttributes The attributes to be set for the value.
     * @param aValue The boolean value to be added to the field.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aField is not valid for the implementing
     *     PIM item type.
     * @li \c KErrTooBig - \a aValue is not valid for the field.
     * @li \c KErrNotSupported - The item is associated with a list and
     *     The list does not support \a aField.
     * @li \c KErrOverflow - The field already contains the maximum
     *     number of values it can hold.
     * @li \c KErrLocked - \a aField is read-only.
     * @li Other - The value could not be added.
     */
    virtual void AddBooleanL(TPIMField aField, TPIMAttribute aAttributes,
                             TBool aValue) = 0;

    /**
     * Sets an existing boolean value, discarding the old value.
     *
     * @param aField The field for which the value is set.
     * @param aIndex The index of the value in the value array.
     * @param aAttributes The attributes to be set for the value.
     * @param aValue The value to be set in the place of the old value.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aField is not valid for the implementing
     *     PIM item type.
     * @li \c KErrNotFound - There is not value for given \a aIndex.
     * @li \c KErrTooBig - \a aValue is not valid for the field.
     * @li \c KErrNotSupported - The item is associated with a list and
     *     The list does not support \a aField.
     * @li \c KErrLocked - \a aField is read-only.
     * @li Other - The value could not be set.
     */
    virtual void SetBooleanL(TPIMField aField, TInt aIndex,
                             TPIMAttribute aAttributes, TBool aValue) = 0;

    /* --- StringArray --- */

    /**
     * Gets specific string array value.
     * Empty string array elements are denoted with an empty string.
     *
     * @param aField Field constant.
     * @param aIndex Value index.
     * @return StringArray value.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aField is not valid for the implementing
     *     PIM item type.
     * @li \c KErrNotFound - There is no value for given \a aIndex.
     * @li \c KErrNotSupported - The item is associated with a list and
     *     The list does not support \a aField.
     */
    virtual const CDesCArray
    & GetStringArrayL(TPIMField aField, TInt aIndex) const = 0;

    /**
     * Adds a string array value for a string array field.
     * The value is added as the last value in that field's value array.
     * All elements of the string array must be present in their specified
     * indexes. Empty elements are denoted with an empty string.
     *
     * @param aField The field to which the value is added.
     * @param aAttributes The attributes to be set for the value.
     * @param aValue The string array value to be added to the field.
     *        \b Ownership of the value is transferred to the called
     *        object.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aField is not valid for the implementing
     *     PIM item type or \a aValue is NULL.
     * @li \c KErrTooBig - \a aValue is not valid for the field.
     * @li \c KErrNotSupported - The item is associated with a list and
     *     The list does not support \a aField.
     * @li \c KErrOverflow - The field already contains the maximum
     *     number of values it can hold.
     * @li \c KErrLocked - \a aField is read-only.
     * @li Other - The value could not be added.
     */
    virtual void AddStringArrayL(TPIMField aField, TPIMAttribute aAttributes,
                                 CDesCArray* aValue) = 0;

    /**
     * Sets an existing string array value, discarding the old value.
     * All elements of the string array must be present in their specified
     * indexes. Empty elements are denoted with an empty string.
     *
     * @param aField The field for which the value is set.
     * @param aIndex The index of the value in the value array.
     * @param aAttributes The attributes to be set for the value.
     * @param aValue The value to be set in the place of the old value.
     *        \b Ownership of the value is transferred to the called
     *        object. The old value is deleted.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aField is not valid for the implementing
     *     PIM item type or \a aValue is NULL.
     * @li \c KErrNotFound - There is not value for given \a aIndex.
     * @li \c KErrTooBig - \a aValue is not valid for the field.
     * @li \c KErrNotSupported - The item is associated with a list and
     *     The list does not support \a aField.
     * @li \c KErrLocked - \a aField is read-only.
     * @li Other - The value could not be set.
     */
    virtual void SetStringArrayL(TPIMField aField, TInt aIndex,
                                 TPIMAttribute aAttributes, CDesCArray* aValue) = 0;

    /**
     * Counts the number of values in a specific field.
     *
     * @param aField The field values of which are to be counted.
     *
     * @return Number of values in field \a aField.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aField is not valid for the implementing
     *     PIM item type.
     * @li \c KErrNotSupported - The item is associated with a list and
     *     The list does not support \a aField.
     */
    virtual TInt CountValuesL(TPIMField aField) const = 0;


    /**
     * Provides attributes of a specific value of a specific field.
     *
     * @param aField The field that has the value.
     * @param aIndex The index of the value in the value array.
     *
     * @return The attributes set for the value, orred bitwise.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aField is not valid for the implementing
     *     PIM item type.
     * @li \c KErrNotFound - There is not value for given \a aIndex.
     * @li \c KErrNotSupported - The item is associated with a list and
     *     The list does not support \a aField.
     */
    virtual TPIMAttribute
    getAttributes(TPIMField aField, TInt aIndex) const = 0;

    /**
     * Adds the item to a category.
     * If the item already belongs to the category, nothing is done.
     *
     * @param aCategory The category. Category name is copied if necessary.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aCategory is not valid for the item.
     * @li Other - The item could not be added to the category for some
     *     internal reason.
     */
    virtual void AddToCategoryL(const TDesC& aCategory) = 0;

    /**
     * Removes the item from a category.
     * If the item does not belong to the category, nothing is done.
     *
     * @param aCategory. The category.
     *
     */
    virtual void RemoveFromCategory(const TDesC& aCategory) = 0;

    /**
     * Provides the categories to which the item belongs to.
     *
     * @return Array of categories.
     *
     * @par Leaving:
     * The method leaves on error.
     */
    virtual const CDesCArray& GetCategoriesL() const = 0;

    /**
     * Provides the Item ID of the item.
     *
     * @return Item ID. If the Item ID is not set, \ref KPIMNullItemID is
     *         returned.
     */
    virtual TPIMItemID GetId() const = 0;

    /**
     * Sets the Item ID of the item.
     *
     * @param aItemID The new Item ID.
     *
     * @par Leaving:
     * @li Any - An internal error.
     */
    virtual void SetIdL(TPIMItemID aItemID) = 0;

    /**
     * Sets the last modified date/time of the item.
     */
    virtual void SetLastModifiedL(TPIMDate aLastModified) = 0;

    /**
     * Gets the last modified date/time of the item.
     */
    virtual TPIMDate LastModified() const = 0;

protected: // Non-public operations

    // Allow derivation with protected default constructor.
    MPIMItem()
    {}

}
;

#endif // MPIMITEM_H
// End of File
