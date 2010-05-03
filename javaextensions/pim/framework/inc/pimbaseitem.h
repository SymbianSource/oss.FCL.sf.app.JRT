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
* Description:  PIM item base implementation.*
*/


#ifndef PIMBASEITEM_H
#define PIMBASEITEM_H

#include <jni.h>
#include "pimcommon.h"

class pimbasemanager;

class pimbaseitem
{
public:

    virtual pimbaseitem::~pimbaseitem()
    {
    };

    virtual TPIMFieldDataType fieldDataType(const TPIMField& aField) const = 0;

    virtual void commit() = 0;

    virtual bool isModified() const = 0;

    virtual jbyteArray callGetBinary(TPIMField aField, int aIndex,
                                     JNIEnv* aJniEnv, jintArray aError) = 0;

    virtual void callAddBinary(TPIMField aField, TPIMAttribute aAttributes,
                               unsigned char* aValue, int aOffset, int aLength) = 0;

    virtual jintArray callGetFields(JNIEnv* aJniEnv) = 0;

    virtual void callSetBinary(TPIMField aField, int aIndex,
                               TPIMAttribute aAttributes, unsigned char* aValue, int aOffset,
                               int aLength) = 0;

    virtual jlong callGetDate(TPIMField aField, int aIndex) = 0;

    virtual void callAddDate(TPIMField aField, TPIMAttribute aAttributes,
                             jlong aValue) = 0;

    virtual void callSetDate(TPIMField aField, int aIndex,
                             TPIMAttribute aAttributes, jlong aValue) = 0;

    virtual int getInt(TPIMField aField, int aIndex) const = 0;

    virtual void addInt(TPIMField aField, TPIMAttribute aAttributes,
                        int aValue) = 0;

    virtual void setInt(TPIMField aField, int aIndex,
                        TPIMAttribute aAttributes, int aValue) = 0;

    virtual jstring callGetString(TPIMField aField, int aIndex,
                                  JNIEnv* aJniEnv, jintArray aError) = 0;

    virtual void callAddString(TPIMField aField, TPIMAttribute aAttributes,
                               jstring aValue, JNIEnv* aJniEnv) = 0;

    virtual void callSetString(TPIMField aField, int aIndex,
                               TPIMAttribute aAttributes, jstring aValue, JNIEnv* aJniEnv) = 0;

    virtual bool callGetBoolean(TPIMField aField, int aIndex) = 0;

    virtual void callAddBoolean(TPIMField aField, TPIMAttribute aAttributes,
                                jboolean aValue) = 0;

    virtual void callSetBoolean(TPIMField aField, int aIndex,
                                TPIMAttribute aAttributes, jboolean aValue) = 0;

    virtual jobjectArray callGetStringArray(TPIMField aField, int aIndex,
                                            JNIEnv* aJniEnv, jintArray aError) = 0;

    virtual void callAddStringArray(JNIEnv* aJniEnv, jint aField,
                                    jint aAttributes, jobjectArray aValue) = 0;

    virtual void callSetStringArray(JNIEnv* aJniEnv, jint aField, jint aIndex,
                                    jint aAttributes, jobjectArray aValue) = 0;

    virtual int callCountValues(TPIMField aField) const = 0;

    virtual void removeValue(TPIMField aField, int aIndex) = 0;

    virtual TPIMAttribute getAttributes(TPIMField aField, int aIndex) const = 0;

    virtual void callAddToCategory(jstring aCategory, JNIEnv* aJniEnv) = 0;

    virtual void callRemoveFromCategory(jstring aCategory, JNIEnv* aJniEnv) = 0;

    virtual jobjectArray callGetCategories(JNIEnv* aJniEnv) = 0;

    virtual int maxCategories() = 0;

    // CPIMContactItem functions

    static pimbaseitem* getContactItemInstance(pimbasemanager* aPimManager);

    virtual int getPreferredIndex(TPIMField aField) const = 0;

    // CPIMEventItem functions

    static pimbaseitem* getEventItemInstance(pimbasemanager* aPimManager);

    virtual int getRepeatHandle() = 0;

    virtual bool isItemRepeating() const = 0;

    virtual void setItemRepeating(jboolean aSetRepeating) = 0;

    // CPIMEventItem functions

    static pimbaseitem* getToDoItemInstance(pimbasemanager* aPimManager);

};

#endif // PIMBASEITEM_H
