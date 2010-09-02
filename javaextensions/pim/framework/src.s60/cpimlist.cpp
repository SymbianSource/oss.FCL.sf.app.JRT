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


// INCLUDE FILES
#include  "cpimlist.h"
#include  "mpimadaptermanager.h"
#include  "mpimlistadapter.h"
#include  "mpimlocalizationdata.h"
#include  "cpimvalidator.h"
#include  "cpimitem.h"
#include  "cpimitemmatcher.h"
#include  "cpimstringmatcher.h"
#include  "cleanupresetanddestroy.h"
#include  "pimexternalchanges.h"
#include  "pimpanics.h"
#include  "pimjnitools.h"
#include  "pimutils.h"
#include  "s60commonutils.h"
#include  "jstringutils.h"
#include "logger.h"

// CONSTANTS
/** Item array granularity. */
const TInt KItemArrayGranularity = 8;

CPIMList::CPIMList(const CPIMValidator& aValidator) :
        iItems(KItemArrayGranularity), iValidator(aValidator) // not owned
{
    JELOG2(EPim);
}

void CPIMList::ConstructL(MPIMLocalizationData* aLocalizationData,
                          MPIMAdapterManager* aAdapterManager, MPIMListAdapter* aListAdapter)
{
    JELOG2(EPim);
    iLocalizationData = aLocalizationData; // not owned
    iAdapterManager = aAdapterManager; // not owned
    iListAdapter = aListAdapter; // not owned
}

CPIMList::~CPIMList()
{
    JELOG2(EPim);
    iItems.Close();
}

pimbaseitem* CPIMList::createItem()
{
    JELOG2(EPim);
    TInt error = KErrNone;
    CPIMItem* item = NULL;
    TRAP(error, item = DoCreateItemL(KPIMNullItemID(), NULL));
    if (error != KErrNone)
        throw error;
    item->SetModified(ETrue);
    return item;
}

void CPIMList::removeItem(pimbaseitem* aItem)
{
    JELOG2(EPim);
    TInt error = KErrNone;
    TRAP(error,
    {
        if (!iListAdapter)
        {
            User::Leave(KErrSessionClosed);
        }
        CPIMItem* item = static_cast<CPIMItem*>(aItem);
        // Find the item in the list
        TInt itemIndex = iItems.Find(item);

        if (itemIndex == KErrNotFound)
        {
            // The item is not in the list
            User::Leave(KErrArgument);
        }

        // Remove item from native database.
        // If the item does not exist any more in the database, we will
        // not touch but let the update mechanism clean it away at
        // some other time.
        DoDeleteItemL(*item);

        // Remove item from the list
        item->RemoveAdapterAssociation();
        iItems.Remove(itemIndex);

        // The item is owned by its Java side peer, so we won't
        // delete it here.
    }
        );
    if (error != KErrNone)
        throw error;
}

void CPIMList::addCommittedItem(pimbaseitem* aItem)
{
    JELOG2(EPim);
    CPIMItem* item = static_cast<CPIMItem*>(aItem);
    TInt error = KErrNone;
    TRAP(error,
    {
        __ASSERT_DEBUG(
            iItems.Find(item)== KErrNotFound,
            User::Panic(KPIMPanicCategory, EPIMPanicCommittedExists));

        User::LeaveIfError(iItems.Append(item));
    }
        );
    if (error != KErrNone)
        throw error;
}

jstring CPIMList::getName(JNIEnv* aJniEnv)
{
    JELOG2(EPim);
    HBufC* name = NULL;
    TInt error = KErrNone;
    TRAP(error, name = iAdapterManager->ListNameL().AllocL());
    if (error != KErrNone || !name)
    {
        // If a leave occurred the name was never created
        return NULL;
    }
    jstring javaName = java::util::S60CommonUtils::NativeToJavaString(
                           *aJniEnv, *name);
    delete name;
    return javaName; // if NULL, it indicates error
}

void CPIMList::close()
{
    JELOG2(EPim);
    if (!iListAdapter)
    {
        throw KErrSessionClosed;
    }

    // Close the list adapter
    iListAdapter->Close();

    // Notify items about closing list
    const TInt n = iItems.Count();
    for (TInt i = 0; i < n; i++)
    {
        iItems[i]->ListClosed();
    }

    // Setting the list adapter NULL denotes closed list
    iListAdapter = NULL;

}

jintArray CPIMList::callItemsByEnumerationType(JNIEnv* aJniEnv,
        int aEnumerationType, int aMatchingItemHandle, jstring aStringArg,
        jintArray aError)
{
    JELOG2(EPim);
    const JStringUtils stringArg(*aJniEnv, aStringArg);
    const TDesC* nativeStringArg = (aStringArg ? &stringArg : NULL);
    TInt error = KErrNone;
    jintArray itemHandles = NULL;
    TRAP(error, itemHandles = CallItemsByEnumerationTypeL(aJniEnv,
                              aEnumerationType, aMatchingItemHandle, nativeStringArg));
    SetJavaErrorCode(aJniEnv, aError, error);
    return itemHandles;
}

jintArray CPIMList::CallItemsByEnumerationTypeL(JNIEnv* aJniEnv,
        int aEnumerationType, int aMatchingItemHandle,
        const TDesC* aStringArg)
{
    JELOG2(EPim);
    RPointerArray<CPIMItem>* items = NULL;

    if (aEnumerationType == EPIMItemAll)
    {
        items = ItemsL();
    }
    else if (aEnumerationType == EPIMItemMatchingItem)
    {
        pimbaseitem * baseMatchingItem =
            reinterpret_cast<pimbaseitem *>(aMatchingItemHandle);
        CPIMItem* matchingItem =
            static_cast<CPIMItem *>(baseMatchingItem);
        items = ItemsL(*matchingItem);
    }
    else if (aEnumerationType == EPIMItemMatchingString)
    {
        __ASSERT_DEBUG(aStringArg, User::Panic(KPIMPanicCategory,
                                               EPIMPanicNullArgument));

        items = ItemsL(*aStringArg);
    }
    else if (aEnumerationType == EPIMItemMatchingCategory)
    {
        // Category argument may be NULL, indicating "uncategorized"
        items = ItemsByCategoryL(aStringArg);
    }
    else
    {
        User::Leave(KErrCorrupt); // Error
    }

    // We now own the items array

    jintArray itemHandles = GetJavaItemHandles(*aJniEnv, *items);
    items->Close();
    delete items;
    items = NULL;

    if (!itemHandles)
    {
        User::Leave(KErrNoMemory);
    }

    return itemHandles;
}

RPointerArray<CPIMItem>* CPIMList::ItemsL()
{
    JELOG2(EPim);
    if (!iListAdapter)
    {
        User::Leave(KErrSessionClosed);
    }

    // Put all items in an array as MPIMItemData objects
    RPointerArray<MPIMItem> tempArr(KItemArrayGranularity);
    CleanupClosePushL(tempArr);

    const TInt n = iItems.Count();
    for (TInt i = 0; i < n; i++)
    {
        CPIMItem* item = iItems[i];
        User::LeaveIfError(tempArr.Append(item));
    }

    TLinearOrder<MPIMItem> order(iAdapterManager->ItemOrder());

    RPointerArray<CPIMItem>* retArr = SortAndConvertToCPIMItemsL(order,
                                      tempArr);

    CleanupStack::PopAndDestroy(); // tempArr cleanup close
    return retArr;
}

RPointerArray<CPIMItem>* CPIMList::ItemsL(const CPIMItem& aMatchingItem)
{
    JELOG2(EPim);
    if (!iListAdapter)
    {
        User::Leave(KErrSessionClosed);
    }

    // Matching items are inserted temporarily into this array for sorting
    RPointerArray<MPIMItem> tempArr(KItemArrayGranularity);
    CleanupClosePushL(tempArr);

    CPIMItemMatcher* itemMatcher = CPIMItemMatcher::NewLC(iValidator,
                                   *iAdapterManager, aMatchingItem);

    const TInt numItems = iItems.Count();
    for (TInt itemIndex = 0; itemIndex < numItems; itemIndex++)
    {
        CPIMItem* listItem = iItems[itemIndex];

        if (itemMatcher->MatchL(*listItem))
        {
            User::LeaveIfError(tempArr.Append(listItem));
        }
    }

    CleanupStack::PopAndDestroy(itemMatcher);
    TLinearOrder<MPIMItem> order(iAdapterManager->ItemOrder());

    RPointerArray<CPIMItem>* retArr = SortAndConvertToCPIMItemsL(order,
                                      tempArr);

    CleanupStack::PopAndDestroy(); // tempArr cleanup close
    return retArr;
}

RPointerArray<CPIMItem>* CPIMList::ItemsL(const TDesC& aMatchingValue)
{
    JELOG2(EPim);
    if (!iListAdapter)
    {
        User::Leave(KErrSessionClosed);
    }

    // Matching items are inserted temporarily into this array for sorting
    RPointerArray<MPIMItem> tempArr(KItemArrayGranularity);
    CleanupClosePushL(tempArr);

    CPIMStringMatcher* stringMatcher =
        new(ELeave) CPIMStringMatcher(iValidator);

    CleanupStack::PushL(stringMatcher);

    // Iterate through all items in the list
    const TInt numItems = iItems.Count();
    for (TInt itemIndex = 0; itemIndex < numItems; itemIndex++)
    {
        CPIMItem* listItem = iItems[itemIndex];

        if (stringMatcher->MatchL(aMatchingValue, *listItem))
        {
            User::LeaveIfError(tempArr.Append(listItem));
        }
    }

    CleanupStack::PopAndDestroy(stringMatcher);
    TLinearOrder<MPIMItem> order(iAdapterManager->ItemOrder());

    RPointerArray<CPIMItem>* retArr = SortAndConvertToCPIMItemsL(order,
                                      tempArr);

    CleanupStack::PopAndDestroy(); // tempArr cleanup close
    return retArr;
}

RPointerArray<CPIMItem>* CPIMList::ItemsByCategoryL(
    const TDesC* aCategory)
{
    JELOG2(EPim);
    if (!iListAdapter)
    {
        User::Leave(KErrSessionClosed);
    }

    // Matching items are inserted temporarily into this array for sorting
    RPointerArray<MPIMItem> tempArr(KItemArrayGranularity);
    CleanupClosePushL(tempArr);

    // Iterate through all items in the list
    const TInt numItems = iItems.Count();
    for (TInt itemIndex = 0; itemIndex < numItems; itemIndex++)
    {
        MPIMItem* listItem = iItems[itemIndex];

        // Iterate through the categories in the list item
        const CDesCArray& categories = listItem->GetCategoriesL();

        if (!aCategory)
        {
            // We are matching non-assigned items
            if (categories.Count() == 0)
            {
                User::LeaveIfError(tempArr.Append(listItem));
            }
        }
        else // aCategory is non-NULL
        {
            // We are matching items assigned to given category
            // Compare case-sensitive with accents etc.
            TInt dummyPosition = 0;
            if (0 == categories.Find(*aCategory, dummyPosition,
                                     ECmpNormal))
            {
                User::LeaveIfError(tempArr.Append(listItem));
            }
        }
    }

    TLinearOrder<MPIMItem> order(iAdapterManager->ItemOrder());

    RPointerArray<CPIMItem>* retArr = SortAndConvertToCPIMItemsL(order, tempArr);

    CleanupStack::PopAndDestroy(); // tempArr cleanup close
    return retArr;
}

const CDesCArray& CPIMList::GetCategoriesL()
{
    JELOG2(EPim);
    if (!iListAdapter)
    {
        User::Leave(KErrSessionClosed); // codescanner::leave
    }

    return iListAdapter->GetCategoriesL(); // codescanner::leave
}

jobjectArray CPIMList::getCategories(JNIEnv* aJniEnv, jintArray aError)
{
    JELOG2(EPim);
    const CDesCArray* categories = NULL;
    TInt error = KErrNone;
    TRAP(error, categories = &(GetCategoriesL()));

    SetJavaErrorCode(aJniEnv, aError, error);

    if (error != KErrNone)
    {
        return NULL;
    }

    jobjectArray javaCategories = CreateJavaStringArray(aJniEnv, *categories,
                                  EFalse); // (do not handle KPIMNullArrayElement elements specially)

    if (!javaCategories)
    {
        SetJavaErrorCode(aJniEnv, aError, KErrNoMemory);
    }

    return javaCategories;
}

TBool CPIMList::IsCategoryL(const TDesC& aCategory)
{
    JELOG2(EPim);
    TBool retVal = EFalse;

    if (!iListAdapter)
    {
        User::Leave(KErrSessionClosed);
    }

    const CDesCArray& categories = iListAdapter->GetCategoriesL();
    TInt dummyIndex = -1;

    if (0 == categories.Find(aCategory, dummyIndex, ECmpNormal))
    {
        retVal = ETrue;
    }

    return retVal;
}

jboolean CPIMList::isCategory(jstring aCategory, JNIEnv* aJniEnv,
                              jintArray aError)
{
    JELOG2(EPim);
    __ASSERT_DEBUG(aCategory, User::Panic(KPIMPanicCategory,
                                          EPIMPanicNullArgument));
    const JStringUtils nativeCategory(*aJniEnv, aCategory);
    TBool retVal = EFalse;
    TInt error = KErrNone;
    TRAP(error, retVal = IsCategoryL(*(static_cast<const TDesC*>(&nativeCategory))));
    SetJavaErrorCode(aJniEnv, aError, error);
    return static_cast<jboolean>(retVal);
}

void CPIMList::AddCategoryL(const TDesC& aCategory)
{
    JELOG2(EPim);
    if (!iListAdapter)
    {
        User::Leave(KErrSessionClosed);
    }

    iListAdapter->AddCategoryL(aCategory);
}

void CPIMList::addCategory(jstring aCategory, JNIEnv* aJniEnv,
                           jintArray aError)
{
    JELOG2(EPim);
    __ASSERT_DEBUG(aCategory, User::Panic(KPIMPanicCategory,
                                          EPIMPanicNullArgument));
    const JStringUtils category(*aJniEnv, aCategory);
    TInt error = KErrNone;
    TRAP(error, AddCategoryL(*(static_cast<const TDesC*>(&category))));
    SetJavaErrorCode(aJniEnv, aError, error);
}

RPointerArray<CPIMItem>* CPIMList::DeleteCategoryL(
    const TDesC& aCategory)
{
    JELOG2(EPim);
    if (!iListAdapter)
    {
        User::Leave(KErrSessionClosed);
    }

    // The adapter category deletion operation does not tell whether
    // the category did exist, so we'll put it down here.
    TBool categoryExists = EFalse;
    TInt dummyPosition = 0;
    if (0 == iListAdapter->GetCategoriesL().Find(aCategory,
            dummyPosition, ECmpNormal))
    {
        categoryExists = ETrue;
    }

    iListAdapter->DeleteCategoryL(aCategory);

    // NULL return value indicates that the category was not really
    // deleted (because it did not exist).
    RPointerArray<CPIMItem>* unassignedItems = NULL;

    if (categoryExists)
    {
        RPointerArray<CPIMItem> categoryMembers(KItemArrayGranularity);
        CleanupClosePushL(categoryMembers);

        const TInt n = iItems.Count();
        for (TInt i = 0; i < n; i++)
        {
            TBool wasMember = iItems[i]->CategoryDeleted(aCategory);
            if (wasMember)
            {
                User::LeaveIfError(categoryMembers.Append(iItems[i]));
            }
        }

        unassignedItems = ResolveUnassignedItemsL(categoryMembers);
        CleanupStack::PopAndDestroy(); // categoryMembers cleanup close
    }

    return unassignedItems;
}

jintArray CPIMList::deleteCategory(jstring aCategory, JNIEnv* aJniEnv,
                                   jintArray aError)
{
    JELOG2(EPim);
    __ASSERT_DEBUG(aCategory, User::Panic(KPIMPanicCategory,
                                          EPIMPanicNullArgument));

    RPointerArray<CPIMItem>* unassignedItems = NULL;
    const JStringUtils category(*aJniEnv, aCategory);
    TInt error = KErrNone;
    TRAP(error, unassignedItems = DeleteCategoryL(
                                      *(static_cast<const TDesC*>(&category))));
    SetJavaErrorCode(aJniEnv, aError, error);
    if (error != KErrNone || !unassignedItems)
    {
        // If the operation leaved, unassignedItems was never created.
        // If no unassignedItems are present, we're OK but no return
        // value is needed.
        return NULL;
    }

    // We now own the array of removed items

    jintArray itemHandles = GetJavaItemHandles(*aJniEnv, *unassignedItems);
    unassignedItems->Close();
    delete unassignedItems;

    if (!itemHandles)
    {
        SetJavaErrorCode(aJniEnv, aError, KErrNoMemory);
        return NULL;
    }
    return itemHandles;

}

void CPIMList::RenameCategoryL(const TDesC& aCurrentCategory,
                               const TDesC& aNewCategory)
{
    JELOG2(EPim);
    if (!iListAdapter)
    {
        User::Leave(KErrSessionClosed);
    }

    iListAdapter->RenameCategoryL(aCurrentCategory, aNewCategory);

    const TInt n = iItems.Count();
    for (TInt i = 0; i < n; i++)
    {
        iItems[i]->CategoryRenamedL(aCurrentCategory, aNewCategory);
    }
}

jint CPIMList::renameCategory(jstring aCurrentCategory,
                              jstring aNewCategory, JNIEnv* aJniEnv)
{
    JELOG2(EPim);
    __ASSERT_DEBUG(aCurrentCategory, User::Panic(KPIMPanicCategory,
                   EPIMPanicNullArgument));

    __ASSERT_DEBUG(aNewCategory, User::Panic(KPIMPanicCategory,
                   EPIMPanicNullArgument));

    const JStringUtils currentCategory(*aJniEnv, aCurrentCategory);
    const JStringUtils newCategory(*aJniEnv, aNewCategory);
    TInt error = KErrNone;

    TRAP(error, RenameCategoryL(
             *(static_cast<const TDesC*>(&currentCategory)),
             *(static_cast<const TDesC*>(&newCategory))));

    return error;
}

jint CPIMList::maxCategories()
{
    JELOG2(EPim);
    return iAdapterManager->MaxCategories();
}

jboolean CPIMList::isSupportedField(TPIMField aField)
{
    JELOG2(EPim);
    TBool isSupportedField = iAdapterManager->IsSupportedField(aField);
    return static_cast<jboolean>(isSupportedField);
}

jintArray CPIMList::getSupportedFields(JNIEnv* aJniEnv)
{
    JELOG2(EPim);
    const CArrayFix<TPIMField>* fields = NULL;
    TInt error = KErrNone;
    TRAP(error, fields = &(iAdapterManager->GetSupportedFieldsL()));
    if (error != KErrNone)
    {
        // If a leave occurred the field array was never created
        return NULL; // Error
    }
    jintArray javaFields = ConvertToJavaIntArray(*aJniEnv, *fields);
    return javaFields; // NULL indicates error
}

jboolean CPIMList::isSupportedAttribute(TPIMField aField,
                                        TPIMAttribute aAttribute)
{
    JELOG2(EPim);
    TBool isSupportedAttribute = iAdapterManager->IsSupportedAttribute(aField, aAttribute);
    return static_cast<jboolean>(isSupportedAttribute);
}

const CArrayFix<TPIMAttribute>& CPIMList::GetSupportedAttributesL(
    TPIMField aField)
{
    JELOG2(EPim);
    if (!iValidator.IsValidField(aField))
    {
        User::Leave(KErrArgument); // codescanner::leave
    }

    if (!iAdapterManager->IsSupportedField(aField))
    {
        User::Leave(KErrNotSupported); // codescanner::leave
    }

    return iAdapterManager->GetSupportedAttributesL(aField); // codescanner::leave
}

jintArray CPIMList::getSupportedAttributes(TPIMField aField,
        JNIEnv* aJniEnv, jintArray aError)
{
    JELOG2(EPim);
    const CArrayFix<TPIMAttribute>* attributes = NULL;
    TInt error = KErrNone;
    TRAP(error, attributes = &(GetSupportedAttributesL(aField)));
    SetJavaErrorCode(aJniEnv, aError, error);

    if (error != KErrNone)
    {
        return NULL;
    }

    jintArray javaAttributes = ConvertToJavaIntArray(*aJniEnv,
                               *attributes);

    if (!javaAttributes)
    {
        SetJavaErrorCode(aJniEnv, aError, KErrNoMemory);
    }
    return javaAttributes;
}

jboolean CPIMList::isSupportedArrayElement(TPIMField aStringArrayField,
        TPIMArrayElement aArrayElement)
{
    JELOG2(EPim);
    TBool retVal = iAdapterManager->IsSupportedArrayElement(
                       aStringArrayField, aArrayElement);
    return static_cast<jboolean>(retVal);
}

const CArrayFix<TPIMArrayElement>& CPIMList::GetSupportedArrayElementsL(
    TPIMField aStringArrayField)
{
    JELOG2(EPim);
    if (iValidator.FieldDataType(aStringArrayField)
            != EPIMFieldStringArray)
    {
        User::Leave(KErrArgument); // codescanner::leave
    }

    if (!iAdapterManager->IsSupportedField(aStringArrayField))
    {
        User::Leave(KErrNotSupported); // codescanner::leave
    }

    return iAdapterManager->GetSupportedArrayElementsL( // codescanner::leave
               aStringArrayField);
}

jintArray CPIMList::getSupportedArrayElements(
    TPIMField aStringArrayField, JNIEnv* aJniEnv, jintArray aError)
{
    JELOG2(EPim);
    const CArrayFix<TPIMArrayElement>* elements = NULL;
    TInt error = KErrNone;
    TRAP(error, elements = &(GetSupportedArrayElementsL(
                                 aStringArrayField)));
    SetJavaErrorCode(aJniEnv, aError, error);

    if (error != KErrNone)
    {
        return NULL;
    }

    jintArray javaElements = ConvertToJavaIntArray(*aJniEnv, *elements);

    if (!javaElements)
    {
        SetJavaErrorCode(aJniEnv, aError, KErrNoMemory);
    }
    return javaElements;
}

TPIMFieldDataType CPIMList::GetFieldDataTypeL(TPIMField aField)
{
    JELOG2(EPim);
    TPIMFieldDataType fieldDataType = iValidator.FieldDataType(aField);

    if (fieldDataType == EPIMFieldInvalid)
    {
        User::Leave(KErrArgument);
    }

    if (!iAdapterManager->IsSupportedField(aField))
    {
        User::Leave(KErrNotSupported);
    }

    return fieldDataType;
}

jint CPIMList::getFieldDataType(TPIMField aField, JNIEnv* aJniEnv,
                                jintArray aError)
{
    JELOG2(EPim);
    TPIMFieldDataType fieldDataType = EPIMFieldInvalid;
    TInt error = KErrNone;
    TRAP(error, fieldDataType = GetFieldDataTypeL(aField));
    SetJavaErrorCode(aJniEnv, aError, error);
    return fieldDataType;
}

HBufC* CPIMList::GetFieldLabelL(TPIMField aField)
{
    JELOG2(EPim);
    if (!iValidator.IsValidField(aField))
    {
        User::Leave(KErrArgument);
    }

    if (!iAdapterManager->IsSupportedField(aField))
    {
        User::Leave(KErrNotSupported);
    }

    return iLocalizationData->GetFieldLabelL(aField);
}

HBufC* CPIMList::GetAttributeLabelL(TPIMAttribute aAttribute)
{
    JELOG2(EPim);
    if (!IS_SINGLE_BIT(aAttribute) || !(iValidator.ValidAttributes()& aAttribute))
    {
        User::Leave(KErrArgument);
    }

    if (!(iAdapterManager->GetAllSupportedAttributesCombined() & aAttribute))
    {
        User::Leave(KErrNotSupported);
    }

    return iLocalizationData->GetAttributeLabelL(aAttribute);
}

HBufC* CPIMList::GetArrayElementLabelL(TPIMField aStringArrayField,
                                       TPIMArrayElement aArrayElement)
{
    JELOG2(EPim);
    if ((!iValidator.IsValidField(aStringArrayField)) || (aArrayElement < 0)
            || (aArrayElement >= iValidator.NumElementsL(aStringArrayField)))
    {
        User::Leave(KErrArgument);
    }

    if (!iAdapterManager->IsSupportedArrayElement(aStringArrayField,
            aArrayElement))
    {
        User::Leave(KErrNotSupported);
    }

    return iLocalizationData->GetArrayElementLabelL(aStringArrayField,
            aArrayElement);
}

TInt CPIMList::MaxValuesL(TPIMField aField)
{
    JELOG2(EPim);
    if (!iValidator.IsValidField(aField))
    {
        User::Leave(KErrArgument);
    }

    return iAdapterManager->MaxValues(aField);
}

jint CPIMList::maxValues(TPIMField aField, JNIEnv* aJniEnv,
                         jintArray aError)
{
    JELOG2(EPim);
    jint maxVal = 0;
    TInt error = KErrNone;
    TRAP(error, maxVal = MaxValuesL(aField));
    SetJavaErrorCode(aJniEnv, aError, error);
    return maxVal;
}

int CPIMList::StringArraySizeL(TPIMField aStringArrayField)
{
    JELOG2(EPim);
    if (iValidator.FieldDataType(aStringArrayField)
            != EPIMFieldStringArray)
    {
        User::Leave(KErrArgument);
    }

    return iValidator.NumElementsL(aStringArrayField);
}

int CPIMList::stringArraySize(TPIMField aStringArrayField,
                              jintArray aError, JNIEnv* aJniEnv)
{
    JELOG2(EPim);
    TInt error = KErrNone;
    int retVal = 0;
    TRAP(error, retVal = StringArraySizeL(aStringArrayField));

    SetJavaErrorCode(aJniEnv, aError, error);
    return retVal;
}

RPointerArray<CPIMItem>* CPIMList::UpdateListL(CPIMItem* aMatchingItem)
{
    JELOG2(EPim);
    if (!iListAdapter)
    {
        User::Leave(KErrSessionClosed);
    }

    RPointerArray<CPIMItem> tempNewItems(KItemArrayGranularity);
    CleanupClosePushL(tempNewItems);

    RPointerArray<CPIMItem> tempRemovedItems(KItemArrayGranularity);
    CleanupClosePushL(tempRemovedItems);

    MPIMListAdapter::TExternalItemChangeClass changeClass =
        iListAdapter->IsItemsExternallyModified();

    if (changeClass == MPIMListAdapter::EExternalChangesNone)
    {
        // No changes, skip.
    }
    else if (changeClass == MPIMListAdapter::EExternalChangesMinor)
    {
        HandleMinorItemChangesL(tempNewItems, tempRemovedItems,
                                aMatchingItem);
    }
    else if (changeClass == MPIMListAdapter::EExternalChangesMajor)
    {
        HandleMajorItemChangesL(tempNewItems, tempRemovedItems,
                                aMatchingItem);
    }
    else
    {
        // Should never happen
        User::Panic(KPIMPanicCategory,
                    EPIMPanicInvalidNativeChangeClass);
    }

    UpdateCategoriesL();

    // Return value
    RPointerArray<CPIMItem> * newAndRemovedItems =
        new(ELeave) RPointerArray<CPIMItem> (KItemArrayGranularity);

    // The array object needs to be pushed for deletion and closing
    // separately.
    CleanupStack::PushL(newAndRemovedItems);
    CleanupClosePushL(*newAndRemovedItems);

    // Copy the pointers to new and removed items to the result array.
    // First go the new items, then NULL, and then removed items.

    TInt i = 0; // reused iterator
    const TInt numNewItems = tempNewItems.Count();
    for (i = 0; i < numNewItems; i++)
    {
        User::LeaveIfError(newAndRemovedItems->Append(tempNewItems[i]));
    }

    // Add boundary element
    User::LeaveIfError(newAndRemovedItems->Append(NULL));

    const TInt numRemovedItems = tempRemovedItems.Count();
    for (i = 0; i < numRemovedItems; i++)
    {
        User::LeaveIfError(newAndRemovedItems->Append(
                               tempRemovedItems[i]));
    }

    CleanupStack::Pop(); // newAndRemovedItems cleanup close
    CleanupStack::Pop(newAndRemovedItems);

    CleanupStack::PopAndDestroy(); // tempRemovedItems cleanup close
    CleanupStack::PopAndDestroy(); // tempNewItems cleanup close

    return newAndRemovedItems;
}

TInt CPIMList::FindItemByItemID(const TPIMItemID& aItemId)
{
    JELOG2(EPim);
    // Any RPointerArray.Find() variant cannot be used because we're
    // matching with just an Item ID, not another item.

    const TInt n = iItems.Count();
    for (TInt i = 0; i < n; i++)
    {
        if (iItems[i]->GetId() == aItemId)
        {
            return i;
        }
    }

    return KErrNotFound;
}

RPointerArray<CPIMItem>* CPIMList::SortAndConvertToCPIMItemsL(
    TLinearOrder<MPIMItem> aOrder,
    RPointerArray<MPIMItem>& aSourceArray)
{
    JELOG2(EPim);
    aSourceArray.Sort(aOrder);

    RPointerArray<CPIMItem> * retArr = new(ELeave) RPointerArray<
    CPIMItem> (KItemArrayGranularity);

    CleanupStack::PushL(retArr);
    CleanupClosePushL(*retArr);

    const TInt n = aSourceArray.Count();
    for (TInt i = 0; i < n; i++)
    {
        User::LeaveIfError(retArr->Append(
                               static_cast<CPIMItem*>(aSourceArray[i])));
    }

    CleanupStack::Pop(); // *retArr
    CleanupStack::Pop(retArr);
    return retArr;
}

void CPIMList::HandleMinorItemChangesL(
    RPointerArray<CPIMItem>& aTempNewItems,
    RPointerArray<CPIMItem>& aTempRemovedItems, CPIMItem* aMatchingItem)
{
    JELOG2(EPim);
    RPointerArray<CPIMItemStateChange>* itemChanges =
        iListAdapter->GetExternalItemModificationsL();

    __ASSERT_DEBUG(itemChanges, User::Panic(KPIMPanicCategory,
                                            EPIMPanicUnexpectedNullExternalChangeList));

    __ASSERT_ALWAYS(itemChanges, User::Leave(KErrCorrupt));

    CleanupStack::PushL(itemChanges);
    CleanupResetAndDestroyPushL(*itemChanges);

    const TInt n = itemChanges->Count();
    for (TInt i = 0; i < n; i++)
    {
        const CPIMItemStateChange& itemChange = *(*itemChanges)[i];
        switch (itemChange.ChangeType())
        {
        case EPIMExternalChangeNew:
        {
            // Check that the item is really new and not e.g. resulting from
            // our own action.
            if (KErrNotFound == FindItemByItemID(itemChange.ItemID()))
            {
                HandleItemChangeNewL(itemChange.ItemID(),
                                     aTempNewItems, aMatchingItem);
            }
            break;
        }

        case EPIMExternalChangeModified:
        {
            // Find the index. The item should be present when this happens
            const TInt itemIndex =
                FindItemByItemID(itemChange.ItemID());

            __ASSERT_DEBUG((itemIndex != -1), User::Panic(
                               KPIMPanicCategory,
                               EPIMPanicExternalChangeUpdatingNonExistentItem));

            if (itemIndex == KErrNotFound)
            {
                User::Leave(KErrCorrupt);
            }

            CPIMItem* item = iItems[itemIndex];
            HandleItemChangeModifiedL(*item);

            break;
        }

        case EPIMExternalChangeRemoved:
        {
            // Find the item index by Item ID and remove only if it really
            // is present (and not e.g. already removed by us).
            const TInt itemIndex =
                FindItemByItemID(itemChange.ItemID());
            if (itemIndex != KErrNotFound)
            {
                HandleItemChangeRemovedL(itemIndex, aTempRemovedItems);
            }
            break;
        }

        default:
        {
            User::Panic(KPIMPanicCategory,
                        EPIMPanicInvalidNativeChangeType);
            break;
        }
        }
    }

    CleanupStack::PopAndDestroy(2, itemChanges);

    // Refresh any items that have been modified from the Java side.
    // This is done after getting the removed and modified items above
    // because we don't want to try to update removed or already
    // updated items. However, if such situations occur, we just
    // ignore them at this point and handle them during next update.
    RefreshModifiedItemsL();
}

void CPIMList::HandleMajorItemChangesL(
    RPointerArray<CPIMItem>& aTempNewItems,
    RPointerArray<CPIMItem>& aTempRemovedItems, CPIMItem* aMatchingItem)
{
    JELOG2(EPim);
    // The situation here is that the underlying list adapter
    // cannot classify the changes occurred in the native database.
    // We need to refresh the whole set of items and try to figure
    // out which entries are new and which entries have been removed.

    // At the time of writing handles to both new and removed native
    // side items are passed to the Java side in return. We want to
    // do the major update so that it does not differ in any way for
    // that mechanism. We also want to be memory-efficient rather than
    // fast, so we'll reuse and refresh the existing items that still
    // have their counterparts in the database and only create the
    // truely new items. Removed entries will, of course, be removed
    // from the framework, too.

    RPointerArray<CPIMItemStateChange>* itemChanges =
        iListAdapter->GetExternalItemModificationsL();

    __ASSERT_DEBUG(itemChanges, User::Panic(KPIMPanicCategory,
                                            EPIMPanicUnexpectedNullExternalChangeList));

    if (!itemChanges)
    {
        // No changes (although there should be some!)
        User::Leave(KErrCorrupt);
    }

    CleanupStack::PushL(itemChanges);
    CleanupResetAndDestroyPushL(*itemChanges);

    // Iterate through the item list. Some of the items will be removed
    // during the iteration, so the item count must be resolved on
    // every iteration.
    TInt itemIndex = 0;
    while (itemIndex < iItems.Count())
    {
        TPIMItemID itemId = iItems[itemIndex]->GetId();

        TBool exists = EFalse;

        // Iterate through the change list. One of the changes may be
        // removed during the iteration.
        for (TInt changeIndex = 0; changeIndex < itemChanges->Count(); changeIndex++)
        {
            CPIMItemStateChange* itemChange =
                (*itemChanges)[changeIndex];

            const TPIMItemID changedItemId = itemChange->ItemID();

            if (itemId == changedItemId)
            {
                // Exists already - update
                exists = ETrue;
                HandleItemChangeModifiedL(*(iItems[itemIndex]));

                // This change has been handled
                delete itemChange;
                itemChanges->Remove(changeIndex);

                // Next item
                itemIndex++;
                break; // out of the change loop
            }
        }

        if (!exists)
        {
            // The item didn't match a change entry - remove.
            // The item index used in the loop must not be updated, because
            // After the following operation the index will refer to the
            // next item.
            HandleItemChangeRemovedL(itemIndex, aTempRemovedItems);
        }
    }

    // Now the changes in the change list are truely new entries
    for (TInt changeIndex = 0; changeIndex < itemChanges->Count(); changeIndex++)
    {
        const CPIMItemStateChange& itemChange =
            *(*itemChanges)[changeIndex];

        const TPIMItemID newItemId = itemChange.ItemID();
        HandleItemChangeNewL(newItemId, aTempNewItems, aMatchingItem);
    }

    CleanupStack::PopAndDestroy(2, itemChanges);
}

void CPIMList::HandleItemChangeNewL(const TPIMItemID aNewItemId,
                                    RPointerArray<CPIMItem>& aTempNewItems, CPIMItem* aMatchingItem)
{
    JELOG2(EPim);

    // Create new item
    CPIMItem* newItem = NULL;
    TRAPD(errCreateItem, newItem = DoCreateItemL(aNewItemId,
                                   aMatchingItem));

    if (errCreateItem == KErrNotFound)
    {
        // The item was not in the database. The case is
        // probably that the item was created and removed
        // after last update. Skip.
        return;
    }
    else
    {
        User::LeaveIfError(errCreateItem);
    }
    newItem->SetModified(EFalse);
    CleanupStack::PushL(newItem);
    User::LeaveIfError(iItems.Append(newItem));
    CleanupStack::Pop(newItem);
    // Add to list of new items
    CleanupClosePushL(aTempNewItems);
    TInt errAddToNewItems = aTempNewItems.Append(newItem);
    if (errAddToNewItems != KErrNone)
    {
        iItems.Remove(iItems.Find(newItem));
        delete newItem;
        User::Leave(errAddToNewItems);
    }
    CleanupStack::Pop(&aTempNewItems);
}

void CPIMList::HandleItemChangeModifiedL(CPIMItem& aModifiedItem)
{
    JELOG2(EPim);
    // Any Java-originated modifications will be discarded.
    TRAPD(errUpdateItem, DoUpdateItemL(aModifiedItem));

    if (errUpdateItem == KErrNotFound)
    {
        // The item was not in the database. The case is
        // probably that the item was changed and removed
        // after last update. Let's pretend that the item
        // is up-to-date and skip it.
        aModifiedItem.SetModified(EFalse);
        return;
    }
    else
    {
        User::LeaveIfError(errUpdateItem);
    }
    aModifiedItem.SetModified(EFalse);
}

void CPIMList::HandleItemChangeRemovedL(TInt aRemovedItemIndex,
                                        RPointerArray<CPIMItem>& aTempRemovedItems)
{
    JELOG2(EPim);
    CleanupClosePushL(aTempRemovedItems);
    CPIMItem* removedItem = iItems[aRemovedItemIndex];

    // Add to list of removed items
    TInt errAddToRemovedItems = aTempRemovedItems.Append(removedItem);

    if (errAddToRemovedItems != KErrNone)
    {
        User::Leave(KErrCorrupt);
    }

    // Remove from item list and remove adapter association
    iItems.Remove(aRemovedItemIndex);
    removedItem->RemoveAdapterAssociation();
    CleanupStack::Pop(&aTempRemovedItems);
}

void CPIMList::RefreshModifiedItemsL()
{
    JELOG2(EPim);
    const TInt numItems = iItems.Count();
    for (TInt itemIndex = 0; itemIndex < numItems; itemIndex++)
    {
        CPIMItem& item = *(iItems[itemIndex]);
        if (item.isModified())
        {
            TRAPD(errUpdate, DoUpdateItemL(item));

            // If the update succeeds, we're OK. If the entry is not
            // found, we'll pretend that the entry is present until
            // next update (or failing commit). In other error,
            // leave.
            if (errUpdate == KErrNone || errUpdate == KErrNotFound)
            {
                item.SetModified(EFalse);
            }
            else
            {
                User::Leave(errUpdate);
            }
        }
    }
}

void CPIMList::UpdateCategoriesL()
{
    JELOG2(EPim);
    if (!iListAdapter)
    {
        User::Leave(KErrSessionClosed);
    }

    RPointerArray<CPIMCategoryStateChange>* categoryChanges =
        iListAdapter->GetExternalCategoryModificationsL();

    if (!categoryChanges)
    {
        // Nothing to do
        return;
    }

    CleanupStack::PushL(categoryChanges);
    CleanupResetAndDestroyPushL(*categoryChanges);

    const TInt n = categoryChanges->Count();
    for (TInt i = 0; i < n; i++)
    {
        const CPIMCategoryStateChange& change = *(*categoryChanges)[i];
        const TPIMExternalChangeType changeType = change.ChangeType();

        switch (changeType)
        {
        case EPIMExternalChangeNew:
        {
            // Nothing to do.
            break;
        }

        case EPIMExternalChangeModified:
        {
            // Update items
            const TInt numItems = iItems.Count();
            for (TInt itemIndex = 0; itemIndex < numItems; itemIndex++)
            {
                iItems[itemIndex]->CategoryRenamedL(change.Category(),
                                                    change.NewCategoryNameL());
            }
            break;
        }

        case EPIMExternalChangeRemoved:
        {
            // Update items
            const TInt numItems = iItems.Count();
            for (TInt itemIndex = 0; itemIndex < numItems; itemIndex++)
            {
                iItems[itemIndex]->CategoryDeleted(change.Category());
            }
            break;
        }

        default:
        {
            // Should never happen
            User::Panic(KPIMPanicCategory,
                        EPIMPanicInvalidNativeChangeClass);
        }
        }
    }

    CleanupStack::PopAndDestroy(); // categoryChanges cleanup close
    CleanupStack::PopAndDestroy(categoryChanges);
}

RPointerArray<CPIMItem>* CPIMList::ResolveUnassignedItemsL(
    RPointerArray<CPIMItem>& aSearchedItems)
{
    JELOG2(EPim);
    RPointerArray<CPIMItem>* unassignedItems = new(ELeave) RPointerArray<
    CPIMItem> (KItemArrayGranularity);

    CleanupStack::PushL(unassignedItems);
    CleanupClosePushL(*unassignedItems);

    // Find out the items that do not belong to any category and add
    // them to the returned array.
    const TInt n = aSearchedItems.Count();
    for (TInt i = 0; i < n; i++)
    {
        if (aSearchedItems[i]->GetCategoriesL().Count() == 0)
        {
            CPIMItem* unassignedItem = aSearchedItems[i];
            User::LeaveIfError(unassignedItems->Append(unassignedItem));
        }
    }

    CleanupStack::Pop(); // unassignedItems cleanup close
    CleanupStack::Pop(unassignedItems);
    return unassignedItems;
}

jintArray CPIMList::updateList(JNIEnv* aJniEnv,
                               pimbaseitem* aMatchingItem)
{
    JELOG2(EPim);
    TInt error = KErrNone;
    jintArray itemHandles = NULL;
    TRAP(error,
    {
        CPIMItem* matchingItem = static_cast<CPIMItem*>(aMatchingItem);
        RPointerArray< CPIMItem>* newAndRemovedItems =
            UpdateListL(matchingItem);
        // We now own the array
        CleanupStack::PushL(newAndRemovedItems);
        CleanupClosePushL(*newAndRemovedItems);
        const TInt numItems = newAndRemovedItems->Count();

        // Make a temporary integer array which we can push to cleanup stack
        TInt* tempElems = new(ELeave) TInt[ numItems ];
        CleanupArrayDeletePushL(tempElems);

        // Iterate through the item array in two parts.
        // First make and set handles to the new items
        // then get the handles of the removed items.
        // Put all handles to the result Java int array, separated by
        // a zero.
        TInt i = 0; // reused iterator


        for (i = 0; i < numItems; i++)
        {
            pimbaseitem* item = (*newAndRemovedItems)[ i ];

            if (!item)
            {
                // Reached the boundary; the next item is the first
                // removed item. Index variable is reused.
                break;
            }

            TInt newItemHandle = reinterpret_cast<int>(item);

            tempElems[ i ] = newItemHandle;
        }

        // The boundary element
        tempElems[ i++ ] = 0; // boundary mark

        // For each removed item, get its JNI handle and append it to the
        // result array.
        for (/* reuse i with its current value */; i < numItems; i++)
        {
            pimbaseitem* item = (*newAndRemovedItems)[ i ];
            tempElems[ i ] = reinterpret_cast<int>(item);
        }

        // Create the result (Java) array and copy the handles in it
        itemHandles = aJniEnv->NewIntArray(numItems);
        if (!itemHandles)
        {
            User::Leave(KErrNoMemory);   // codescanner::leave
        }

        aJniEnv->SetIntArrayRegion(itemHandles, 0, numItems, tempElems);
        CleanupStack::PopAndDestroy(tempElems);

        CleanupStack::Pop(); // newAndRemovedItems cleanup close
        CleanupStack::PopAndDestroy(newAndRemovedItems);

    }
        );
    if (error != KErrNone)
        throw error;
    return itemHandles;
}

//  End of File
