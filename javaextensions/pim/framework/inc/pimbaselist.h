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


#ifndef PIMBASELIST_H
#define PIMBASELIST_H

#include <jni.h>
#include "pimcommon.h"

class CPIMItem;
class pimbaseitem;

class pimbaselist
{
public:

    virtual pimbaselist::~pimbaselist()
    {
    };

    virtual pimbaseitem* createItem() = 0;

    virtual void removeItem(pimbaseitem* aItem) = 0;

    virtual void addCommittedItem(pimbaseitem* aItem)= 0;

    virtual jstring getName(JNIEnv* aJniEnv)= 0;

    virtual void close() = 0;

    virtual jintArray callItemsByEnumerationType(JNIEnv* aJniEnv,
            int aEnumerationType, int aMatchingItemHandle, jstring aStringArg,
            jintArray aError) = 0;

    virtual jobjectArray getCategories(JNIEnv* aJniEnv, jintArray aError) = 0;

    virtual jboolean isCategory(jstring aCategory, JNIEnv* aJniEnv,
                                jintArray aError) = 0;

    virtual void addCategory(jstring aCategory, JNIEnv* aJniEnv,
                             jintArray aError) = 0;

    virtual jintArray deleteCategory(jstring aCategory, JNIEnv* aJniEnv,
                                     jintArray aError) = 0;

    virtual jint renameCategory(jstring aCurrentCategory,
                                jstring aNewCategory, JNIEnv* aJniEnv) = 0;

    virtual jint maxCategories() = 0;

    virtual jboolean isSupportedField(TPIMField aField) = 0;

    virtual jintArray getSupportedFields(JNIEnv* aJniEnv) = 0;

    virtual jboolean isSupportedAttribute(TPIMField aField,
                                          TPIMAttribute aAttribute) = 0;

    virtual jintArray getSupportedAttributes(TPIMField aField,
            JNIEnv* aJniEnv, jintArray aError) = 0;

    virtual jboolean isSupportedArrayElement(TPIMField aStringArrayField,
            TPIMArrayElement aArrayElement) = 0;

    virtual jintArray getSupportedArrayElements(TPIMField aStringArrayField,
            JNIEnv* aJniEnv, jintArray aError) = 0;

    virtual jint getFieldDataType(TPIMField aField, JNIEnv* aJniEnv,
                                  jintArray aError) = 0;

    virtual jint maxValues(
        TPIMField aField, JNIEnv* aJniEnv, jintArray aError) = 0;

    virtual int stringArraySize(TPIMField aStringArrayField, jintArray aError,
                                JNIEnv* aJniEnv) = 0;

    virtual jintArray updateList(
        JNIEnv* aJniEnv, pimbaseitem* aMatchingItem) = 0;

    // from cpimeventlist

    virtual jintArray itemsByDate(jint aSearchType, jlong aStartDate,
                                  jlong aEndDate, jboolean aInitialEventOnly, JNIEnv* aJniEnv,
                                  jintArray aError) = 0;

    virtual jintArray getSupportedRepeatRuleFields(jint aFrequency,
            JNIEnv* aJniEnv, jintArray aError) = 0;

    // from cpimtodolist

    virtual jintArray itemsByDate(TPIMField aField, jlong aStartDate,
                                  jlong aEndDate, JNIEnv* aJniEnv, jintArray aError) = 0;

};

#endif // PIMBASELIST_H
