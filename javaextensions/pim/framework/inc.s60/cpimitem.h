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
* Description:  PIM item base implementation.
 *
*/


#ifndef CPIMITEM_H
#define CPIMITEM_H

// INTERNAL INCLUDES
#include "pimcommon.h"
#include "pimbaseitem.h"
#include "mpimitem.h"

// EXTERNAL INCLUDES
#include <e32base.h>
#include <badesca.h>

// FORWARD DECLARATIONS
class MPIMAdapterManager;
class MPIMListAdapter;
class CPIMValidator;
class CPIMItemData;

// CLASS DECLARATION

/**
 *  PIM item base implementation. The class is derivable, but not instantiable.
 *  The class acts as a container for cached PIM item data.
 */
NONSHARABLE_CLASS(CPIMItem): public CBase,
        public MPIMItem,
        public pimbaseitem
{

public: // Destructor

    /**
     * Destructor.
     */
    virtual ~CPIMItem();

public: // New abstract functions

    /**
     * Determines the list type to which the item can be associated to;
     * i.e. also the type of the item.
     */
    virtual TPIMListType ItemType() const = 0;

    /**
     * Persists the item in the native database. The modification
     * status of the item is cleared on success.
     *
     * @par Leaving:
     * @li \c KErrNotFound - The item was persisted earlier, but
     *     has probably been removed since that.
     * @li \c KErrDisconnected - The associated list is closed.
     * @li \c KErrDismounted - The item does not belong to a list.
     * @li Other - Internal error.
     */
    virtual void commit() = 0;

public: // New functions

    /**
     * Determines field data type.
     *
     * @param aField A field constant.
     *
     * @return Field data type or \ref EPIMFieldInvalid if the field is
     * not supported by the derived class.
     */
    TPIMFieldDataType fieldDataType(
        const TPIMField& aField) const;

    /**
     * Provides a complete set of valid attributes for a derived class.
     * The attribute set can be used for masking invalid attributes off
     * from a some set of attributes.
     */
    TPIMAttribute ValidAttributes() const;

    /**
     * Checks whether given boolean value is valid for given field.
     * Default implementation just consults the validator.
     *
     * @return \c EFalse, if the value is known to be invalid, \c ETrue
     *         otherwise.
     */
    virtual TBool IsValidBooleanValue(
        const TPIMField& aField,
        const TBool& aValue) const;

    /**
     * Checks whether given date value is valid for given field.
     * Default implementation just consults the validator.
     *
     * @return \c EFalse, if the value is known to be invalid, \c ETrue
     *         otherwise.
     */
    virtual TBool IsValidDateValue(
        const TPIMField& aField,
        const TPIMDate& aValue) const;

    /**
     * Checks whether given integer value is valid for given field.
     * Default implementation just consults the validator.
     *
     * @return \c EFalse, if the value is known to be invalid, \c ETrue
     *         otherwise.
     */
    virtual TBool IsValidIntegerValue(
        const TPIMField& aField,
        const TInt& aValue) const;

    /**
     * Checks whether given binary value is valid for given field.
     * Default implementation just consults the validator.
     *
     * @return \c EFalse, if the value is known to be invalid, \c ETrue
     *         otherwise.
     */
    virtual TBool IsValidBinaryValue(
        const TPIMField& aField,
        const CPIMByteArray& aValue) const;

    /**
     * Checks whether given string value is valid for given field.
     * Default implementation just consults the validator.
     *
     * @return \c EFalse, if the value is known to be invalid, \c ETrue
     *         otherwise.
     */
    virtual TBool IsValidStringValue(
        const TPIMField& aField,
        const HBufC& aValue) const;

    /**
     * Checks whether given string array value is valid for given field.
     * Default implementation just consults the validator.
     *
     * @return \c EFalse, if the value is known to be invalid, \c ETrue
     *         otherwise.
     */
    virtual TBool IsValidStringArrayValue(
        const TPIMField& aField,
        const CDesCArray& aValue) const;

    //@{
    /**
     * Sets the modification status of this item.
     * Defaults to true.
     */
    void SetModified();

    void SetModified(TBool aIsModified);
    //@}

    /**
     * Informs the item that the associated list has been closed.
     * From this point the adapter manager association is still valid
     * but the list adapter association is invalid.
     */
    virtual void ListClosed();

    /**
     * Informs the item that the name of a category has changed.
     * If the item holds the name of the category, the name must be
     * changed.
     */
    void CategoryRenamedL(
        const TDesC& aOldName,
        const TDesC& aNewName);

    /**
     * Informs the item that a category has been deleted.
     * If the item holds the name of the category, the name must be
     * deleted.
     *
     * @return \c ETrue if the item was assigned to \a aCategory and thus
     *         removed from it; \c EFalse otherwise.
     */
    TBool CategoryDeleted(const TDesC& aCategory);

    /**
     * Removes the adapter associations.
     * Performs any housekeeping related to removing the adapter
     * associations, if necessary.
     */
    virtual void RemoveAdapterAssociation();

    /**
     * Loads full item from the list adapter. This function
     * should be overwriten in the base class if it supports
     * partial data initialization
     */
    virtual void DoLoadFullItemL();

    /**
     * Loads a specific field from the list adapter. This function
     * should be overwriten in the base class if it supports
     * partial data initialization
     *
     * @param aField The field which is to be loaded from
     *        the associated list adapter
     */
    virtual void DoLoadFieldL(const TPIMField aField);

    /**
     * Updates the single value in UID string field from the Item ID.
     * This method is used by the derived classes.
     *
     * @par aUidField The UID string field applicable to the derived class.
     * @par aItemId The Item ID.
     *
     * Behavior:
     * @li If the Item ID is \ref KPIMNullItemID, the UID field is cleared.
     * @li If the Item ID is not \ref KPIMNullItemID, the UID field is
     *     set to have single value - the string representation of the
     *     Item ID.
     *
     * @par Leaving:
     * @li \c KErrCorrupt - The UID field is not in a valid state.
     * @li Any - internal error.
     */
    void UpdateUidFieldL(
        const TPIMField& aUidStringField,
        const TPIMItemID& aItemId);

    /**
     * Updates the single value in REVISION string field.
     * This method is used by the derived classes.
     *
     * @par aRevisionField The valid revision field for this class.
     * @par aLastModified The last modified date/time.
     */
    void UpdateRevisionFieldL(
        TPIMField aRevisionField,
        TPIMDate aLastModified);

public: // Functions from MPIMItem

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
    CArrayFix< TPIMField>* GetFieldsL() const;

    jintArray callGetFields(JNIEnv* aJniEnv/*,jintArray aJavaFields*/);

    /**
     * Checks whether the contents of the item have been modified since
     * it was last retrieved or committed to the native database.
     *
     * @return ETrue if the item has been modified after last retrieval or
     *         commit, EFalse otherwise.
     */
    bool isModified() const;

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
    CPIMByteArray* GetBinaryL(
        TPIMField aField,
        TInt aIndex) const;

    jbyteArray callGetBinary(
        TPIMField aField,
        int aIndex,
        JNIEnv* aJniEnv,
        jintArray aError);

    /**
     * Same as GetBinaryL, but provides the value non-Base64-encoded.
     * Ownership of the return value remains.
     */
    const CPIMByteArray& GetBinaryRawL(
        TPIMField aField,
        TInt aIndex) const;

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
    void AddBinaryL(
        TPIMField aField,
        TPIMAttribute aAttributes,
        CPIMByteArray* aValue);

    void callAddBinary(
        TPIMField aField,
        TPIMAttribute aAttributes,
        unsigned char* aValue,
        int aOffset,
        int aLength);

    /**
     * Same as AddBinaryL, but takes the value non-Base64-encoded.
     */
    void AddBinaryRawL(
        TPIMField aField,
        TPIMAttribute aAttributes,
        CPIMByteArray* aValue);

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
    void SetBinaryL(
        TPIMField aField,
        TInt aIndex,
        TPIMAttribute aAttributes,
        CPIMByteArray* aValue);

    void callSetBinary(
        TPIMField aField,
        int aIndex,
        TPIMAttribute aAttributes,
        unsigned char* aValue,
        int aOffset,
        int aLength);

    /**
     * Same as SetBinaryL, but takes the value non-Base64-encoded.
     */
    void SetBinaryRawL(
        TPIMField aField,
        TInt aIndex,
        TPIMAttribute aAttributes,
        CPIMByteArray* aValue);

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
    const TPIMDate GetDateL(
        TPIMField aField,
        TInt aIndex) const;

    jlong callGetDate(
        TPIMField aField,
        int aIndex);

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
    void AddDateL(TPIMField aField,
                  TPIMAttribute aAttributes,
                  TPIMDate aValue);

    void callAddDate(
        TPIMField aField,
        TPIMAttribute aAttributes,
        jlong aValue);

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
    void SetDateL(TPIMField aField,
                  TInt aIndex,
                  TPIMAttribute aAttributes,
                  TPIMDate aValue);

    void callSetDate(
        TPIMField aField,
        int aIndex,
        TPIMAttribute aAttributes,
        jlong aValue);

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
    int getInt(TPIMField aField, int aIndex) const;

    /**
     * Adds a int value for a int field. The value is added as the
     * last value in that field's value array.
     *
     * @param aField The field to which the value is added.
     * @param aAttributes The attributes to be set for the value.
     * @param aValue The int value to be added to the field.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aField is not valid for the implementing
     *     PIM item type.
     * @li \c KErrTooBig - \a aValue is not valid for the field.
     * @li Other - The value could not be added.
     * @li \c KErrNotSupported - The item is associated with a list and
     *     The list does not support \a aField.
     * @li \c KErrOverflow - The field already contains the maximum
     *     number of values it can hold.
     * @li \c KErrLocked - \a aField is read-only.
     */
    void addInt(TPIMField aField,
                TPIMAttribute aAttributes,
                int aValue);

    /**
     * Sets an existing int value, discarding the old value.
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
    void setInt(TPIMField aField,
                int aIndex,
                TPIMAttribute aAttributes,
                int aValue);

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
    const TDesC& GetStringL(
        TPIMField aField,
        TInt aIndex) const;

    jstring callGetString(
        TPIMField aField,
        int aIndex,
        JNIEnv* aJniEnv,
        jintArray aError);

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
    void AddStringL(TPIMField aField,
                    TPIMAttribute aAttributes,
                    HBufC* aValue);

    void callAddString(
        TPIMField aField,
        TPIMAttribute aAttributes,
        jstring aValue,
        JNIEnv* aJniEnv);

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
    void SetStringL(TPIMField aField,
                    TInt aIndex,
                    TPIMAttribute aAttributes,
                    HBufC* aValue);

    void callSetString(
        TPIMField aField,
        int aIndex,
        TPIMAttribute aAttributes,
        jstring aValue,
        JNIEnv* aJniEnv);

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
    TBool GetBooleanL(TPIMField aField, TInt aIndex) const;

    bool callGetBoolean(TPIMField aField,int aIndex);

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
    void AddBooleanL(TPIMField aField,
                     TPIMAttribute aAttributes,
                     TBool aValue);

    void callAddBoolean(
        TPIMField aField,
        TPIMAttribute aAttributes,
        jboolean aValue);

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
    void SetBooleanL(TPIMField aField,
                     TInt aIndex,
                     TPIMAttribute aAttributes,
                     TBool aValue);

    void callSetBoolean(
        TPIMField aField,
        int aIndex,
        TPIMAttribute aAttributes,
        jboolean aValue);

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
    const CDesCArray& GetStringArrayL(
        TPIMField aField,
        TInt aIndex) const;

    jobjectArray callGetStringArray(
        TPIMField aField,
        int aIndex,
        JNIEnv* aJniEnv,
        jintArray aError);

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
    void AddStringArrayL(TPIMField aField,
                         TPIMAttribute aAttributes,
                         CDesCArray* aValue);

    void callAddStringArray(
        JNIEnv* aJniEnv,
        jint aField,
        jint aAttributes,
        jobjectArray aValue);

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
     *      PIM item type or \a aValue is NULL.
     * @li \c KErrNotFound - There is not value for given \a aIndex.
     * @li \c KErrTooBig - \a aValue is not valid for the field.
     * @li \c KErrNotSupported - The item is associated with a list and
     *     The list does not support \a aField.
     * @li \c KErrLocked - \a aField is read-only.
     * @li Other - The value could not be set.
     */
    void SetStringArrayL(TPIMField aField,
                         TInt aIndex,
                         TPIMAttribute aAttributes,
                         CDesCArray* aValue);

    void callSetStringArray(JNIEnv* aJniEnv,
                            jint aField,
                            jint aIndex,
                            jint aAttributes,
                            jobjectArray aValue);

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
    TInt CountValuesL(TPIMField aField) const;

    int callCountValues(TPIMField aField) const;

    /**
     * Removes a specific value of a specific field.
     *
     * @param aField The field that has the value to be removed.
     * @param aIndex Index of the value that is to be removed.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aField is not valid for the implementing
     *     PIM item type.
     * @li \c KErrNotFound - There is not value for given \a aIndex.
     * @li \c KErrNotSupported - The item is associated with a list and
     *     The list does not support \a aField.
     * @li \c KErrLocked - \a aField is read-only.
     * @li Other - The value could not be removed cleanly. This probably
     *     means that the value array has gone corrupt.
     */
    void removeValue(TPIMField aField, int aIndex);

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
    TPIMAttribute getAttributes(TPIMField aField, TInt aIndex) const;

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
    void AddToCategoryL(const TDesC& aCategory);

    void callAddToCategory(
        jstring aCategory,
        JNIEnv* aJniEnv);

    /**
     * Removes the item from a category.
     * If the item does not belong to the category, nothing is done.
     *
     * @param aCategory. The category.
     *
     */
    void RemoveFromCategory(const TDesC& aCategory);

    void callRemoveFromCategory(jstring aCategory , JNIEnv* aJniEnv);

    /**
     * Provides the categories to which the item belongs to.
     *
     * @return Array of categories.
     *
     * @par Leaving:
     * The method leaves on error.
     */
    const CDesCArray& GetCategoriesL() const;

    jobjectArray callGetCategories(JNIEnv* aJniEnv);

    /**
     * Returns the maximum number of categories to which
     * this PIM item can be associated with
     *
     * @return The maximum number of categories
     */
    int maxCategories();

    /**
     * Sets the last modified date/time of the item.
     */
    void SetLastModifiedL(
        TPIMDate aLastModified);

    /**
     * Gets the last modified date/time of the item.
     */
    TPIMDate LastModified() const;

    /**
     * Provides the Item ID of the item.
     *
     * @return Item ID. If the Item ID is not set, \ref KPIMNullItemID is
     *         returned.
     */
    TPIMItemID GetId() const;

    /**
     * Sets the Item ID of the item.
     *
     * @param aItemID The new Item ID.
     *
     * @par Leaving:
     * @li Any - An internal error.
     */
    void SetIdL(TPIMItemID aItemID);

protected: // New functions

    /**
     * Checks whether given field is read only.
     * Default implementation just returns EFalse.
     */
    virtual TBool IsReadOnly(const TPIMField& aField);

    /**
     * Sets the associated adapter manager and list adapter.
     *
     * @param aAdapterManager Adapter manager. Must not be NULL.
     * @param aListAdapter List adapter. May be NULL.
     */
    void SetBaseAdapterAssociation(
        MPIMAdapterManager* aAdapterManager,
        MPIMListAdapter* aListAdapter);

    /**
     * Checks whether this item already belongs to a category.
     */
    TBool BelongsToCategory(const TDesC& aCategory);

    /**
     * Checks whether given field is valid and, if the item is
     * associated with a list, whether given field is supported in
     * the list. The correct field data type is also checked
     *
     * @param aField Field to be checked.
     * @param aDataType The expected data type of the field
     *
     * @par Leaving:
     * @li \c KErrArgument - \a aField is invalid for this item type.
     * @li \c KErrNotSupported - \a aField is not supported by the
     *     associated list.
     */
    void LeaveIfInvalidOrUnsupportedFieldL(
        const TPIMField aField,
        const TPIMFieldDataType aDataType) const;

    /**
     * Checks whether given field is valid and, if the item is
     * associated with a list, whether given field is supported in
     * the list.
     *
     * @param aField Field to be checked.
     * @param aDataType The expected data type of the field
     *
     * @par Leaving:
     * @li \c KErrArgument - \a aField is invalid for this item type.
     * @li \c KErrNotSupported - \a aField is not supported by the
     *     associated list.
     */
    void LeaveIfInvalidOrUnsupportedFieldL(
        const TPIMField aField) const;

    /**
     * Creates a copy of given string array, setting all unsupported
     * elements to KPIMNullArrayElement in the result array.
     */
    CDesCArray* DropUnsupportedElementsL(
        const TPIMField aStringArrayField,
        const CDesCArray& aStringArray);

    /**
     * PrepareForLoadL
     * Prepares the item for load from the native database
     * This resets the interal state of the item
     *
     * @par Leaving:
     * @li - This function leaves in OOM situation
     */
    void PrepareForLoadL();

    /**
     * Tells specialized class to load full item if it is needed
     * This function was introduced due to the lack of performance
     * when handling large amount of items
     */
    void LoadFullItemL() const;

protected: // Constructors

    /**
     * C++ default constructor.
     */
    CPIMItem(const CPIMValidator& aValidator);

    /**
     * Symbian 2nd phase constructor.
     * Populates the field maps according to the information provided by
     * validator.
     */
    void ConstructL();

private: // New functions

    /**
     * Tells specialized class to load a specific field from the native
     * database. This function was introduced due to the lack of performance
     * when handling large amount of items
     *
     * @param aField Field which is to be fetched
     */
    void LoadFieldL(const TPIMField aField) const;

    /**
     * Takes a set of attributes and provides the set of attributes with
     * invalid and unsupported bits cleared off. Unsupported bits are
     * cleared only if the item is associated with a list (adapter).
     *
     * @param aAttributes A set of attributes.
     * @param aField A field for which \a aAttributes apply to.
     *
     * @return A cleared version of \a aAttributes.
     *
     * @par Leaving.
     * @li \c KErrArgument The item is associated with a list and \a aField
     *     is not supported by the list.
     */
    TPIMAttribute ClearUnsupportedAttributesL(
        const TPIMAttribute& aAttributes,
        const TPIMField& aField);

    /**
     * Checks that a string array has at least one non-null
     * (not \ref KPIMNullArrayElement) element.
     */
    TBool NonNullElements(const CDesCArray& aStringArray) const;

protected: // Data

    /*
     * Fields, values and attributes. Owned.
     */
    CPIMItemData* iItemData;

    /**
     * Associated adapter manager.
     *
     * NULL, if not present.
     *
     * Behavioral implications:
     * @li Presence of adapter manager implies that the item is associated
     *     with a list.
     * @li Absence of adapter manager implies that the item is not
     *     associated with a list.
     */
    MPIMAdapterManager* iAdapterManager;

    /**
     * Associated list adapter.
     *
     * NULL, if not present. The list adapter may be present only if
     * the adapter manager is present.
     *
     * Behavioral implications:
     * @li Presence of list adapter implies that the list associated with
     *     the item is open.
     * @li Absence of list adapter implies that the list associated with
     *     the item is closed.
     */
    MPIMListAdapter* iListAdapter;

    /** Validator, not owned by this object. */
    const CPIMValidator& iValidator;

    /** Item ID. */
    HBufC8* iItemID;

    /** Fields that currently hold some data */
    CArrayFix< TPIMField>* iInitializedFields;

private: // Data

    /** Modification status flag. */
    TBool iIsModified;

    /** JNI native handle to this item. */
    TInt iJniNativeHandle;

    /** Last modified (committed) date/time */
    TPIMDate iLastModified;

};

#endif // CPIMITEM_H
// End of File
