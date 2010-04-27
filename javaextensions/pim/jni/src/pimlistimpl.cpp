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
* Description:  PIMListImpl JNI wrapper.
 *
*/


//  INCLUDE FILES

#include "com_nokia_mj_impl_pim_PIMListImpl.h"
#include "pimbaselist.h"
#include "pimbaseitem.h"
#include "pimcommon.h"
#include "pimutils.h"
#include "pimpanics.h"
#include "cpimlist.h"
#include "logger.h"
#include "s60commonutils.h" // need to be removed once localisation is finalised

//  CONSTANTS

/**
 * Represents the type of label fetched from the framework. Used in
 * the common implementation of all three label getting operations.
 */
enum TPIMLabelType
{
    EPIMLabelTypeField = com_nokia_mj_impl_pim_PIMListImpl_LABEL_TYPE_FIELD,

    EPIMLabelTypeAttribute = com_nokia_mj_impl_pim_PIMListImpl_LABEL_TYPE_ATTRIBUTE,

    EPIMLabelTypeArrayElement = com_nokia_mj_impl_pim_PIMListImpl_LABEL_TYPE_ARRAY_ELEMENT
};

/**
 * Represents enumeration (item listing) type in different enumeration
 * operations.
 */
enum TPIMEnumerationType
{
    EPIMEnumerationTypeAll = com_nokia_mj_impl_pim_PIMListImpl_ENUMERATION_ITEMS_ALL,

    EPIMEnumerationTypeMatchingItem = com_nokia_mj_impl_pim_PIMListImpl_ENUMERATION_ITEMS_MATCHING_ITEM,

    EPIMEnumerationTypeMatchingString = com_nokia_mj_impl_pim_PIMListImpl_ENUMERATION_ITEMS_MATCHING_STRING,

    EPIMEnumerationTypeMatchingCategory = com_nokia_mj_impl_pim_PIMListImpl_ENUMERATION_ITEMS_MATCHING_CATEGORY
};

JNIEXPORT void
JNICALL Java_com_nokia_mj_impl_pim_PIMListImpl__1dispose(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aListHandle)
{
    JELOG2(EPim);
    pimbaselist* list = reinterpret_cast< pimbaselist*>(aListHandle);
    delete list;
}

JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_PIMListImpl__1createItem(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aListHandle,
    jintArray aError)
{
    JELOG2(EPim);
    pimbaselist* list = reinterpret_cast< pimbaselist*>(aListHandle);
    pimbaseitem* item = NULL;
    int error = 0;
    try
    {
        item = list->createItem();
    }
    catch (int aErr)
    {
        error = aErr;
    }
    SetJavaErrorCode(aJniEnv, aError, error);
    // We now own the item (through the handle)
    return reinterpret_cast<int>(item);
}

JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_PIMListImpl__1removeItem(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aListHandle,
    jint aItemHandle)
{
    JELOG2(EPim);
    pimbaselist* list = reinterpret_cast< pimbaselist*>(aListHandle);
    pimbaseitem* item = reinterpret_cast< pimbaseitem *>(aItemHandle);
    int error = 0;
    try
    {
        list->removeItem(item);
    }
    catch (int err)
    {
        error = err;
    }
    return error;
}

JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_PIMListImpl__1addCommittedItem(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aListHandle,
    jint aItemHandle)
{
    JELOG2(EPim);
    pimbaselist* list = reinterpret_cast< pimbaselist*>(aListHandle);
    pimbaseitem* item = reinterpret_cast< pimbaseitem *>(aItemHandle);
    int error = 0;
    // Ownership of the item retains with the Java side peer item object.
    try
    {
        list->addCommittedItem(item);
    }
    catch (int err)
    {
        error = err;
    }
    return error;
}

JNIEXPORT jstring
JNICALL Java_com_nokia_mj_impl_pim_PIMListImpl__1getName(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aListHandle)
{
    JELOG2(EPim);
    pimbaselist* list = reinterpret_cast< pimbaselist*>(aListHandle);
    jstring javaName = javaName =list->getName(aJniEnv);
    // We now own the name
    return javaName; // if NULL, it indicates error
}

JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_PIMListImpl__1close(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aListHandle)
{
    JELOG2(EPim);
    pimbaselist* list = reinterpret_cast< pimbaselist*>(aListHandle);
    int error = 0;
    try
    {
        list->close();
    }
    catch (int aError)
    {
        error = aError;
    }
    return error;
}

JNIEXPORT jintArray
JNICALL Java_com_nokia_mj_impl_pim_PIMListImpl__1items(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aListHandle,
    jint aEnumerationType,
    jint aMatchingItemHandle,
    jstring aStringArg,
    jintArray aError)
{
    JELOG2(EPim);
    pimbaselist* list = reinterpret_cast< pimbaselist*>(aListHandle);
    int enumerationType;
    switch (aEnumerationType)
    {
    case EPIMEnumerationTypeAll:
        enumerationType = EPIMItemAll;
        break;
    case EPIMEnumerationTypeMatchingItem:
        enumerationType = EPIMItemMatchingItem;
        break;
    case EPIMEnumerationTypeMatchingString:
        enumerationType = EPIMItemMatchingString;
        break;
    case EPIMEnumerationTypeMatchingCategory:
        enumerationType = EPIMItemMatchingCategory;
        break;
    default:
        enumerationType = EPIMItemCorrupt;
    }

    jintArray itemHandles =list->callItemsByEnumerationType(
                               aJniEnv,
                               enumerationType,
                               aMatchingItemHandle,
                               aStringArg,
                               aError);

    return itemHandles;
}

JNIEXPORT jobjectArray
JNICALL Java_com_nokia_mj_impl_pim_PIMListImpl__1getCategories(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aListHandle,
    jintArray aError)
{
    JELOG2(EPim);
    pimbaselist* list = reinterpret_cast< pimbaselist*>(aListHandle);
    jobjectArray javaCategories = list->getCategories(aJniEnv, aError);
    // Ownership of the categories is not transferred.
    return javaCategories;
}

JNIEXPORT jboolean
JNICALL Java_com_nokia_mj_impl_pim_PIMListImpl__1isCategory(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aListHandle,
    jstring aCategory,
    jintArray aError)
{
    JELOG2(EPim);
    pimbaselist* list = reinterpret_cast< pimbaselist*>(aListHandle);

    jboolean isCategory = list->isCategory(
                              aCategory, aJniEnv, aError);
    return isCategory;
}

JNIEXPORT void
JNICALL Java_com_nokia_mj_impl_pim_PIMListImpl__1addCategory(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aListHandle,
    jstring aCategory,
    jintArray aError)
{
    JELOG2(EPim);
    pimbaselist* list = reinterpret_cast< pimbaselist*>(aListHandle);

    list->addCategory(aCategory,aJniEnv,aError);
}

JNIEXPORT jintArray
JNICALL Java_com_nokia_mj_impl_pim_PIMListImpl__1deleteCategory(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aListHandle,
    jstring aCategory,
    jintArray aError)
{
    JELOG2(EPim);
    pimbaselist* list = reinterpret_cast< pimbaselist*>(aListHandle);

    jintArray itemHandles = list->deleteCategory(aCategory,aJniEnv, aError);

    return itemHandles;
}

JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_PIMListImpl__1renameCategory(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aListHandle,
    jstring aCurrentCategory,
    jstring aNewCategory)
{
    JELOG2(EPim);
    pimbaselist* list = reinterpret_cast< pimbaselist*>(aListHandle);

    jint error = list->renameCategory(
                     aCurrentCategory, aNewCategory, aJniEnv);

    return error;
}

JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_PIMListImpl__1maxCategories(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aListHandle)
{
    JELOG2(EPim);
    pimbaselist* list = reinterpret_cast< pimbaselist*>(aListHandle);
    jint maxCategories = list->maxCategories();
    return maxCategories;
}

JNIEXPORT jboolean
JNICALL Java_com_nokia_mj_impl_pim_PIMListImpl__1isSupportedField(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aListHandle,
    jint aField)
{
    JELOG2(EPim);
    pimbaselist* list = reinterpret_cast< pimbaselist*>(aListHandle);
    jboolean isSupportedField = isSupportedField = list->isSupportedField(aField);
    return isSupportedField;
}

JNIEXPORT jintArray
JNICALL Java_com_nokia_mj_impl_pim_PIMListImpl__1getSupportedFields(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aListHandle)
{
    JELOG2(EPim);
    pimbaselist* list = reinterpret_cast< pimbaselist*>(aListHandle);
    jintArray javaFields = list->getSupportedFields(aJniEnv);
    return javaFields; // NULL indicates error
}

JNIEXPORT jboolean
JNICALL Java_com_nokia_mj_impl_pim_PIMListImpl__1isSupportedAttribute(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aListHandle,
    jint aField,
    jint aAttribute)
{
    JELOG2(EPim);
    pimbaselist* list = reinterpret_cast< pimbaselist*>(aListHandle);
    jboolean isSupportedAttribute = list->isSupportedAttribute(
                                        aField,
                                        aAttribute);
    return isSupportedAttribute;
}

JNIEXPORT jintArray
JNICALL Java_com_nokia_mj_impl_pim_PIMListImpl__1getSupportedAttributes(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aListHandle,
    jint aField,
    jintArray aError)
{
    JELOG2(EPim);
    pimbaselist* list = reinterpret_cast< pimbaselist*>(aListHandle);

    jintArray javaAttributes = list->getSupportedAttributes(
                                   aField,
                                   aJniEnv,
                                   aError);

    return javaAttributes;
}

JNIEXPORT jboolean
JNICALL Java_com_nokia_mj_impl_pim_PIMListImpl__1isSupportedArrayElement(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aListHandle,
    jint aStringArrayField,
    jint aStringArrayElement)
{
    JELOG2(EPim);
    pimbaselist* list = reinterpret_cast< pimbaselist*>(aListHandle);
    jboolean isSupportedArrayElement = list->isSupportedArrayElement(
                                           aStringArrayField,
                                           aStringArrayElement);

    return isSupportedArrayElement;
}

JNIEXPORT jintArray
JNICALL Java_com_nokia_mj_impl_pim_PIMListImpl__1getSupportedArrayElements(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aListHandle,
    jint aStringArrayField,
    jintArray aError)
{
    JELOG2(EPim);
    pimbaselist* list = reinterpret_cast< pimbaselist*>(aListHandle);

    jintArray javaElements = list->getSupportedArrayElements(
                                 aStringArrayField,
                                 aJniEnv,
                                 aError);

    return javaElements;
}

JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_PIMListImpl__1getFieldDataType(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aListHandle,
    jint aField,
    jintArray aError)
{
    JELOG2(EPim);
    pimbaselist* list = reinterpret_cast< pimbaselist*>(aListHandle);
    jint fieldDataType = list->getFieldDataType(
                             aField,
                             aJniEnv,
                             aError);
    return fieldDataType;
}

/**
 * Calls GetFieldLabelL, GetAttributeLabelL or GetArrayElementLabelL
 * as indicated by the aLabelType argument.
 *
 * @param aList The list to which the call is delegated.
 * @param aLabelType Label type (field, attribute, array element).
 * @param aLabelSpecElems Label specification elements which specify
 *        the requested label.
 * @param aLabelSpecLen Length of the label specification.
 * @param aRetVal The label. Ownership of the label is transferred to
 *        caller.
 */
LOCAL_C void CallGetLabelL(CPIMList* aList, jint aLabelType,
                           jint* aLabelSpecElems, jint /*aLabelSpecLen*/, HBufC** aRetVal)
{
    if (aLabelType == EPIMLabelTypeField)
    {
        *aRetVal = aList->GetFieldLabelL(aLabelSpecElems[0]); // field constant
    }
    else if (aLabelType == EPIMLabelTypeAttribute)
    {
        *aRetVal = aList->GetAttributeLabelL(aLabelSpecElems[0]); // attribute constant
    }
    else if (aLabelType == EPIMLabelTypeArrayElement)
    {
        *aRetVal = aList->GetArrayElementLabelL(aLabelSpecElems[0], // string array field constant
                                                aLabelSpecElems[1]); // string array element constant
    }
    else
    {
        // We should never end up here
        User::Panic(KPIMPanicCategory, EPIMPanicInvalidState);
    }
}

JNIEXPORT jstring
JNICALL Java_com_nokia_mj_impl_pim_PIMListImpl__1getLabel(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aListHandle,
    jint aLabelType,
    jintArray aLabelSpec,
    jintArray aError)
{
    JELOG2(EPim);
    pimbaselist* baseList = reinterpret_cast< pimbaselist*>(aListHandle);
    CPIMList* list = static_cast< CPIMList*>(baseList);

    TInt labelSpecLen = aJniEnv->GetArrayLength(aLabelSpec);
    jint* labelSpecElems = aJniEnv->GetIntArrayElements(aLabelSpec, NULL);

    if (!labelSpecElems)
    {
        SetJavaErrorCode(aJniEnv, aError, KErrNoMemory);
        return NULL;
    }

    HBufC* label = NULL;

    TInt error = 0;
    TRAP(error, CallGetLabelL(
             list,
             aLabelType,
             labelSpecElems,
             labelSpecLen,
             &label));

    SetJavaErrorCode(aJniEnv, aError, error);
    aJniEnv->ReleaseIntArrayElements(aLabelSpec, labelSpecElems, 0);

    // The label might be NULL on error
    if (error != KErrNone || !label)
    {
        return NULL;
    }

    // We now own the label

    jstring javaLabel = java::util::S60CommonUtils::NativeToJavaString(*aJniEnv, *label);
    delete label;

    if (!javaLabel)
    {
        SetJavaErrorCode(aJniEnv, aError, KErrNoMemory);
    }

    return javaLabel;
}

JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_PIMListImpl__1maxValues(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aListHandle,
    jint aField,
    jintArray aError)
{
    JELOG2(EPim);
    pimbaselist* list = reinterpret_cast< pimbaselist*>(aListHandle);
    jint maxValues = list->maxValues(
                         aField,
                         aJniEnv,
                         aError);

    return maxValues;
}

JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_PIMListImpl__1stringArraySize(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aListHandle,
    jint aStringArrayField,
    jintArray aError)
{
    JELOG2(EPim);
    pimbaselist* list = reinterpret_cast< pimbaselist*>(aListHandle);

    int stringArraySize = list->stringArraySize(
                              aStringArrayField,
                              aError,
                              aJniEnv);

    return stringArraySize;
}

/**
 * Calls CPIMList::UpdateListL() for given list and converts the
 * result into set of JNI handles.
 *
 * New handles are made and set for new items and the items are added
 * to the Event Source.
 *
 * @param aJniEnv JNIEnv for jintArray manipulation.
 * @param aList The list for which the update is called and the list
 *        of new and removed items is got.
 * @param aMatchingItem An item from which the initialized fields
 *        can be fetched and loaded from the database to all items
 * @param aNewAndRemovedItemHandles Pointer to a jintArray (pointer
 *        to a pointer) through which the resulting array of new
 *        and removed item handles are returned.
 *
 * @par Leaving:
 * @li Any - internal error.
 */

JNIEXPORT jintArray
JNICALL Java_com_nokia_mj_impl_pim_PIMListImpl__1updateList(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aListHandle,
    jint aMatchingItemHandle,
    jintArray aError)
{
    JELOG2(EPim);
    pimbaseitem* matchingItem = NULL;
    if (aMatchingItemHandle)
    {
        matchingItem = reinterpret_cast< pimbaseitem *>(aMatchingItemHandle);
    }

    pimbaselist* list = reinterpret_cast< pimbaselist*>(aListHandle);
    jintArray newAndRemovedItemHandles = NULL;
    try
    {
        newAndRemovedItemHandles = list->updateList(aJniEnv,
                                   matchingItem);
        SetJavaErrorCode(aJniEnv, aError, 0);
    }
    catch (int error)
    {
        SetJavaErrorCode(aJniEnv, aError, error);
    }

    return newAndRemovedItemHandles;
}

// End of File
