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
* Description:  Abstract PIMList base class.
 *
*/


#ifndef CPIMLIST_H
#define CPIMLIST_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>
#include "pimcommon.h"
#include "pimexternalchanges.h"
#include "pimbaselist.h"

// FORWARD DECLARATIONS
class CPIMItem;
class pimbaseitem;
class MPIMListAdapter;
class MPIMAdapterManager;
class MPIMLocalizationData;
class CPIMValidator;
class pimbaseitem;

// CLASS DECLARATION
/**
 *  PIM list implementation base. The class is derivable but not instantiable.
 *  The class implements the common functionalities of PIM lists.
 */
NONSHARABLE_CLASS(CPIMList): public CBase, public pimbaselist
{
public: // Constructors and destructor

    /**
     * Destructor.
     */
    virtual ~CPIMList();

public: // New functions

    /**
     * Creates a new specialized item and associates it with the list but
     * does not add it to the list.
     *
     * @return New item of specialized type. Ownership is transferred
     *         to caller.
     */
    pimbaseitem* createItem();

    /**
     * Removes given specialized item from native database and native
     * side list.
     */

    void removeItem(pimbaseitem* aItem);

    /**
     * Adds a new, first time committed item to the list.
     *
     * @par Leaving:
     * @li Any - internal error.
     */

    void addCommittedItem(
        pimbaseitem* aItem);

    /**
     * Provides list name.
     *
     * @return List name.
     *         \b Ownership of the returned value is transferred to caller.
     */
    jstring getName(JNIEnv* aJniEnv);

    /**
     * Closes list.
     *
     * @par Leaving:
     * @li \c KErrSessionClosed - list is already closed.
     */
    virtual void close();

    jintArray callItemsByEnumerationType(
        JNIEnv* aJniEnv,
        int aEnumerationType,
        int aMatchingItemHandle,
        jstring aStringArg,
        jintArray aError);

    jintArray CallItemsByEnumerationTypeL(
        JNIEnv* aJniEnv,
        int aEnumerationType,
        int aMatchingItemHandle,
        const TDesC* aStringArg);

    /**
     * @return List of items.
     *         \b Ownership of the returned value is transferred to caller.
     *
     * @par Leaving:
     * @li \c KErrSessionClosed - list is closed.
     * @li Other - internal error.
     */
    virtual RPointerArray< CPIMItem>* ItemsL();

    /**
     * @return List of items.
     *         \b Ownership of the returned value is transferred to caller.
     *
     * @par Leaving:
     * @li \c KErrSessionClosed - list is closed.
     * @li Other - internal error.
     */
    virtual RPointerArray< CPIMItem>* ItemsL(
        const CPIMItem& aMatchingItem);

    /**
     * @return List of items.
     *         \b Ownership of the returned value is transferred to caller.
     *
     * @par Leaving:
     * @li \c KErrSessionClosed - list is closed.
     * @li Other - internal error.
     */
    virtual RPointerArray< CPIMItem>* ItemsL(
        const TDesC& aMatchingValue);

    /**
     * @return List of items.
     *         \b Ownership of the returned value is transferred to caller.
     *
     * @param aCategory Matching category. If NULL, items that do not
     *        belong to any category are matched.
     *
     * @par Leaving:
     * @li \c KErrSessionClosed - list is already closed.
     */
    virtual RPointerArray< CPIMItem>* ItemsByCategoryL(
        const TDesC* aCategory);

    /**
     * @par Leaving:
     * @li \c KErrSessionClosed - list is already closed.
     * @li Other - internal error.
     */
    virtual const CDesCArray& GetCategoriesL();

    jobjectArray getCategories(
        JNIEnv* aJniEnv,
        jintArray aError);

    /**
     * @par Leaving:
     * @li \c KErrSessionClosed - list is already closed.
     * @li Other - internal error.
     */
    virtual TBool IsCategoryL(
        const TDesC& aCategory);

    jboolean isCategory(
        jstring aCategory,
        JNIEnv* aJniEnv,
        jintArray aError);

    /**
     * @par Leaving:
     * @li \c KErrSessionClosed - list is already closed.
     * @li \c KErrArgument - \a aNewCategory is invalid.
     * @li \c KErrNotSupported - The operation is not supported.
     * @li Other - Internal error.
     */
    virtual void AddCategoryL(
        const TDesC& aCategory);

    void addCategory(
        jstring aCategory,
        JNIEnv* aJniEnv,
        jintArray aError);
    /**
     * Deletes given category. If the category does not exist, nothing
     * is done.
     *
     * @return Array of items that belonged to \a aCategory but do
     *         not belong to any category after deletion of
     *         \a aCategory; NULL if \a aCategory was not deleted
     *         (i.e. did not exist).
     *         Ownership of the return value is transferred to caller.
     *
     * @par Leaving:
     * @li \c KErrSessionClosed - list is already closed.
     * @li \c KErrArgument - \a aCategory is invalid.
     * @li \c KErrNotSupported - The operation is not supported.
     * @li Other - Internal error.
     */
    virtual RPointerArray< CPIMItem>* DeleteCategoryL(
        const TDesC& aCategory);

    jintArray deleteCategory(
        jstring aCategory,
        JNIEnv* aJniEnv,
        jintArray aError);

    /**
     * @par Leaving:
     * @li \c KErrSessionClosed - list is already closed.
     * @li \c KErrArgument - \a aNewCategory is invalid.
     * @li \c KErrNotFound - \a aOldCategory does not exist.
     * @li \c KErrNotSupported - The operation is not supported.
     */
    virtual void RenameCategoryL(
        const TDesC& aCurrentCategory,
        const TDesC& aNewCategory);

    jint renameCategory(
        jstring aCurrentCategory,
        jstring aNewCategory,
        JNIEnv* aJniEnv);

    /**
     * @return Maximum number of categories in this list. 0 if unsupported,
     *         KPIMUnlimitedCategories if unlimited.
     */
    virtual jint maxCategories();

    /**
     * Determines whether given field is supported in the list.
     */
    virtual jboolean isSupportedField(
        TPIMField aField);


    virtual jintArray getSupportedFields(
        JNIEnv* aJniEnv);

    /**
     * Determines wheter given attribute is supported for given field.
     * @return ETrue if \a aAttribute and \a aField are valid and
     *         \a aAttribute is supported for \a aField; EFalse otherwise.
     */
    virtual jboolean isSupportedAttribute(
        TPIMField aField,
        TPIMAttribute aAttribute);

    /**
     * Provides all supported attributes.
     *
     * @par Leaving:
     * @li \c KErrArgument - \a aField is invalid for this list type.
     * @li \c KErrNotSupported - \a aField is valid but not supported for
     *     this list.
     * @li Other - internal error.
     */
    virtual const CArrayFix< TPIMAttribute>& GetSupportedAttributesL(
        TPIMField aField);

    jintArray getSupportedAttributes(
        TPIMField aField,
        JNIEnv* aJniEnv,
        jintArray aError);

    /**
     * Determines whether given array element is supported for given
     * string array field.
     *
     * @return ETrue if \a aStringArrayField and \a aArrayElement are
     *         valid and \a aArrayElement is supported for
     *         \a aStringArrayField.
     */
    virtual jboolean isSupportedArrayElement(
        TPIMField aStringArrayField,
        TPIMArrayElement aArrayElement);

    /**
     * Provides all supported array elements for given field.
     *
     * @par Leaving:
     * @li \c KErrArgument - \a aField is invalid for this list type or
     *     is not a string array field.
     * @li \c KErrNotSupported - \a aField is valid but not supported for
     *     this list.
     * @li Other - internal error.
     */
    virtual const CArrayFix< TPIMArrayElement>&
    GetSupportedArrayElementsL(
        TPIMField aStringArrayField);

    jintArray getSupportedArrayElements(
        TPIMField aStringArrayField,
        JNIEnv* aJniEnv,
        jintArray aError);

    /**
     * Determines the data type of given field.
     *
     * @par Leaving:
     * @li \c KErrArgument - \a aField is invalid for this list type.
     * @li \c KErrNotSupported - \a aField is valid but not supported for
     *     this list.
     */
    virtual TPIMFieldDataType GetFieldDataTypeL(
        TPIMField aField);

    jint getFieldDataType(
        TPIMField aField,
        JNIEnv* aJniEnv,
        jintArray aError);

    /**
     * @return Field label.
     *         \b Ownership of the returned objec is transferred to caller.
     *
     * @par Leaving:
     * @li \c KErrArgument - \a aField is invalid for this list type.
     * @li \c KErrNotSupported - \a aField is not supported for this list.
     * @li Other - internal error.
     */
    virtual HBufC* GetFieldLabelL(
        TPIMField aField);

    /**
     * @return Attribute label or NULL if not supported.
     *         \b Ownership of the returned objec is transferred to caller.
     *
     * @par Leaving:
     * @li \c KErrArgument - \a aField is invalid for this list type.
     * @li \c KErrNotSupported - \a aField is not supported for this list.
     * @li Other - internal error.
     */
    virtual HBufC* GetAttributeLabelL(
        TPIMAttribute aAttribute);

    /**
     * @return Array element label or NULL if not supported.
     *         \b Ownership of the returned objec is transferred to caller.
     *
     * @par Leaving:
     * @li \c KErrArgument - \a aField is invalid for this list type.
     * @li \c KErrNotSupported - \a aField is not supported for this list.
     * @li Other - internal error.
     */
    virtual HBufC* GetArrayElementLabelL(
        TPIMField aStringArrayField,
        TPIMArrayElement aArrayElement);

    /**
     * @return Maximum number of values for given field. 0, if \a aField
     *         is not supported, KPIMUnlimitedValues if unlimited.
     *
     * @par Leaving:
     * @li \c KErrArgument - \a aField is invalid for this list type.
     */
    virtual TInt MaxValuesL(
        TPIMField aField);

    jint maxValues(
        TPIMField aField,
        JNIEnv* aJniEnv,
        jintArray aError);

    /**
     * @return Number of elements for given string array field.
     *
     * @par Leaving:
     * @li \c KErrArgument - \a aField is invalid for this list type or not
     *     a string array field.
     */
    virtual TInt StringArraySizeL(
        TPIMField aStringArrayField);

    int stringArraySize(
        TPIMField aStringArrayField,
        jintArray aError,
        JNIEnv* aJniEnv);

    /**
     * Fetches modifications from native database and updates the
     * list of new items and the list of removed items.
     * @li New items are created in the native side and list
     *     associations are removed from removed items.
     * @li For new and updated items the modification status is
     *     cleared.
     * @li Any items modified from the Java side are refreshed and
     *     their modification statuses are cleared.
     * @li Any changes in categories are reflected in existing
     *     items.
     *
     * @return An array, consisting of new items, NULL and removed
     *         items, contiguously, in that order.
     *         \b Ownership of the return value is transferred to caller.
     *
     * @par Leaving:
     * @li \c KErrSessionClosed - The list is closed.
     * @li \c KErrCorrupt - Updating failed and the list is in a corrupt
     *     state.
     */
    RPointerArray< CPIMItem>* UpdateListL(
        CPIMItem* aMatchingItem);

    jintArray updateList(
        JNIEnv* aJniEnv,
        pimbaseitem* aMatchingItem);

protected: // New functions

    /**
     * Creates new item of specialized type and associates it with the
     * list. The modification status of the item is cleared.
     *
     * @param aItemId Either a valid Item ID or KPIMNullItemID.
     *        If \a aItemId is KPIMNullItemID, a blank, associated item
     *        is to be created; otherwise the item data must be updated
     *        from native database.
     * @param aMatchingItem Matching item which provides information
     *        about loaded fields. NOTE: This may be NULL!
     *
     * @li \c KErrSessionClosed - The list is closed.
     * @li \c KErrNotFound - \a aItemId is valid but the entry is not
     *     found from the native database (probably removed).
     * @li Other - internal error.
     */
    virtual CPIMItem* DoCreateItemL(const TPIMItemID& aItemId,
                                    CPIMItem* aMatchingItem) = 0;

    /**
     * Updates an item of specialized type from the specialized list's
     * database.
     *
     * @param aItem A specialized item, needs to be casted to the list's
     *        specialized item type.
     *
     * @par Leaving:
     * @li \c KErrSessionClosed - The list is closed.
     * @li \c KErrNotFound - \a aItemId is valid but the entry is not
     *     found from the native database (probably removed).
     * @li Other - internal error.
     */
    virtual void DoUpdateItemL(CPIMItem& aItem) = 0;

    virtual void DoDeleteItemL(CPIMItem& aItem) = 0;

    /**
     * Finds an item in the item list by Item ID.
     *
     * @param aItemId The Item ID to match.
     *
     * @return Index of the item with \a aItemId or KErrNotFound
     *         if not found.
     */
    TInt FindItemByItemID(const TPIMItemID& aItemId);

    /**
     * Sorts an array of MPIMItemData objects to given order
     * and casts the objects to CPIMItem objects.
     * The method is used in ItemsL() methods and is necessary
     * because an adapter can sort only MPIMItemData objects but
     * in JNI context the objects are needed as CPIMItem objects.
     *
     * @param aSourceArray Array of MPIMItemData objects to be
     *        sorted and converted to CPIMItem objects.
     *
     * @return The items in \a aSourceArray, sorted and cast to
     *         CPIMItem objects.
     */
    RPointerArray< CPIMItem>* SortAndConvertToCPIMItemsL(
        TLinearOrder< MPIMItem> aOrder,
        RPointerArray< MPIMItem>& aSourceArray);

private: // New functions

    /**
     * Handles the minor item changes in UpdateItemListL().
     */
    void HandleMinorItemChangesL(
        RPointerArray< CPIMItem>& aTempNewItems,
        RPointerArray< CPIMItem>& aTempRemovedItems,
        CPIMItem* aMatchingItem);

    /**
     * Handles the marjor item changes in UpdateItemListL().
     */
    void HandleMajorItemChangesL(
        RPointerArray< CPIMItem>& aTempNewItems,
        RPointerArray< CPIMItem>& aTempRemovedItems,
        CPIMItem* aMatchingItem);

    /**
     * Handles item change - new item.
     */
    void HandleItemChangeNewL(
        const TPIMItemID aModifiedItemId,
        RPointerArray< CPIMItem>& aTempNewItems,
        CPIMItem* aMatchingItem);

    /**
     * Handles item change - modified item.
     */
    void HandleItemChangeModifiedL(
        CPIMItem& aModifiedItem);

    /**
     * Handles item change - removed item.
     */
    void HandleItemChangeRemovedL(
        TInt aRemovedItemIndex,
        RPointerArray< CPIMItem>& aTempRemovedItems);

    /**
     * Updates all items that have modification flag set.
     * Items that do not exist in the database any more are
     * ignored.
     *
     * @par Leaving:
     * @li Any - internal error.
     */
    void RefreshModifiedItemsL();

    /**
     * Updates native-originating changes in categories.
     * Changes are reflected in the existing items.
     * Note that the categories are not cached in the framework
     * lists.
     *
     * @par Leaving:
     * @li \c KErrSessionClosed - The list is closed.
     * @li Other - internal error.
     */
    void UpdateCategoriesL();

    /**
     * Resolve those items in given set that are not assigned to
     * any category.
     *
     * @param aItems A set of items from which the unassigned
     *        items are searched for. This should be a subset of
     *        all items.
     *
     * @return Array of items in \a aSearchedItems that do not
     *         belong to any category. The array may be empty.
     */
    RPointerArray< CPIMItem>* ResolveUnassignedItemsL(
        RPointerArray< CPIMItem>& aSearchedItems);

protected: // Constructors

    /**
     * Constructor.
     */
    CPIMList(const CPIMValidator& aValidator);

    /**
     * 2nd phase constructor.
     * \b Must be called from derived classes during their instantiation.
     */
    void ConstructL(
        MPIMLocalizationData* aLocalizationData,
        MPIMAdapterManager* aAdapterManager,
        MPIMListAdapter* aListAdapter);

protected: // Data

    /** Items, \b owned by the object. */
    RPointerArray< CPIMItem> iItems;

    /** Validator, not owned by the object. */
    const CPIMValidator& iValidator;

    /**
     * Associated adapter manager, not owned by the object.
     * Adapter manager must always be present.
     */
    MPIMAdapterManager* iAdapterManager;

    /**
     * Associated list adapter, not owned by the object.
     * May be NULL, which indicates that the list is closed.
     */
    MPIMListAdapter* iListAdapter;

private: // Data

    /** Associated localization data, not owned by the object. */
    MPIMLocalizationData* iLocalizationData;

};

#endif // CPIMLIST_H
// End of File
