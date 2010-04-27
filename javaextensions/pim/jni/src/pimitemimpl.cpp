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
* Description:  PIMItemImpl JNI wrapper.
 *
*/


//  INCLUDE FILES

#include "com_nokia_mj_impl_pim_PIMItemImpl.h"
#include "pimcommon.h"
#include "pimbaseitem.h"
#include "pimutils.h"
#include "pimpanics.h"
#include "logger.h"


JNIEXPORT void
JNICALL Java_com_nokia_mj_impl_pim_PIMItemImpl__1dispose(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aItemHandle)
{
    JELOG2(EPim);
    pimbaseitem* item = reinterpret_cast< pimbaseitem *>(aItemHandle);
    delete item;
}

JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_PIMItemImpl__1fieldDataType(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aItemHandle,
    jint aField)
{
    JELOG2(EPim);
    pimbaseitem* item = reinterpret_cast< pimbaseitem *>(aItemHandle);
    TPIMFieldDataType fieldDataType = item->fieldDataType(aField);
    return fieldDataType;
}

JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_PIMItemImpl__1commit(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aItemHandle)
{
    JELOG2(EPim);
    pimbaseitem* item = reinterpret_cast< pimbaseitem *>(aItemHandle);
    int error = 0;
    try
    {
        item->commit();
    }
    catch (int aError)
    {
        error = aError;
    }
    return error;
}

JNIEXPORT jboolean
JNICALL Java_com_nokia_mj_impl_pim_PIMItemImpl__1isModified(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aItemHandle)
{
    JELOG2(EPim);
    const pimbaseitem* item = reinterpret_cast< pimbaseitem *>(aItemHandle);
    jboolean isModified = item->isModified();
    return isModified;
}

JNIEXPORT jintArray
JNICALL Java_com_nokia_mj_impl_pim_PIMItemImpl__1getFields(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aItemHandle)
{
    JELOG2(EPim);
    pimbaseitem* item = reinterpret_cast< pimbaseitem *>(aItemHandle);
    jintArray javaFields = item->callGetFields(aJniEnv);
    return javaFields; // NULL indicates error
}

JNIEXPORT jbyteArray
JNICALL Java_com_nokia_mj_impl_pim_PIMItemImpl__1getBinary(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aItemHandle,
    jint aField,
    jint aIndex,
    jintArray aError)
{
    JELOG2(EPim);
    pimbaseitem* item = reinterpret_cast< pimbaseitem *>(aItemHandle);
    jbyteArray javaValue = item->callGetBinary(aField,aIndex,aJniEnv,aError);
    return javaValue;
}

JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_PIMItemImpl__1addBinary(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aItemHandle,
    jint aField,
    jint aAttributes,
    jbyteArray aValue,
    jint aOffset,
    jint aLength)
{
    JELOG2(EPim);
    const int valueLength = aJniEnv->GetArrayLength(aValue);
    if (valueLength < (aOffset + aLength))
    {
        aLength = valueLength - aOffset;
    }

    pimbaseitem* item = reinterpret_cast< pimbaseitem *>(aItemHandle);
    jbyte* elems = aJniEnv->GetByteArrayElements(aValue, NULL);
    int error = 0;
    try
    {
        item->callAddBinary(aField,
                            aAttributes,
                            reinterpret_cast< unsigned char*>(elems),
                            aOffset,
                            aLength);
    }
    catch (int aError)
    {
        error = aError;
    }
    aJniEnv->ReleaseByteArrayElements(aValue, elems, 0);
    return error;
}

JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_PIMItemImpl__1setBinary(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aItemHandle,
    jint aField,
    jint aIndex,
    jint aAttributes,
    jbyteArray aValue,
    jint aOffset,
    jint aLength)
{
    JELOG2(EPim);
    // If length from offset goes beyond the array end, the length
    // argument must (as specified in PIM API) be interpreted as
    // "to the end of the array".
    const int valueLength = aJniEnv->GetArrayLength(aValue);
    if (valueLength < (aOffset + aLength))
    {
        aLength = valueLength - aOffset;
    }

    pimbaseitem* item = reinterpret_cast< pimbaseitem *>(aItemHandle);
    jbyte* elems = aJniEnv->GetByteArrayElements(aValue, NULL);
    int error = 0;
    try
    {
        item->callSetBinary(aField,
                            aIndex,
                            aAttributes,
                            reinterpret_cast< unsigned char*>(elems),
                            aOffset,
                            aLength);
    }
    catch (int aError)
    {
        error = aError;
    }
    aJniEnv->ReleaseByteArrayElements(aValue, elems, 0);
    return error;
}

JNIEXPORT jlong
JNICALL Java_com_nokia_mj_impl_pim_PIMItemImpl__1getDate(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aItemHandle,
    jint aField,
    jint aIndex,
    jintArray aError)
{
    JELOG2(EPim);
    pimbaseitem* item = reinterpret_cast< pimbaseitem *>(aItemHandle);
    int error = 0;
    jlong retVal = 0;
    try
    {
        retVal = item->callGetDate(aField,aIndex);
    }
    catch (int aError)
    {
        error = aError;
    }
    SetJavaErrorCode(aJniEnv, aError, error);
    return retVal;
}

JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_PIMItemImpl__1addDate(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aItemHandle,
    jint aField,
    jint aAttributes,
    jlong aValue)
{
    JELOG2(EPim);
    pimbaseitem* item = reinterpret_cast< pimbaseitem *>(aItemHandle);
    int error = 0;
    try
    {
        item->callAddDate(aField, aAttributes, aValue);
    }
    catch (int aError)
    {
        error = aError;
    }
    return error;
}

JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_PIMItemImpl__1setDate(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aItemHandle,
    jint aField,
    jint aIndex,
    jint aAttributes,
    jlong aValue)
{
    JELOG2(EPim);
    pimbaseitem* item = reinterpret_cast< pimbaseitem *>(aItemHandle);
    int error = 0;
    try
    {
        item->callSetDate(aField, aIndex, aAttributes, aValue);
    }
    catch (int aError)
    {
        error = aError;
    }
    return error;
}

JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_PIMItemImpl__1getInt(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aItemHandle,
    jint aField,
    jint aIndex,
    jintArray aError)
{

    JELOG2(EPim);
    pimbaseitem* item = reinterpret_cast< pimbaseitem *>(aItemHandle);
    jint value = 0;
    jint error = 0;
    try
    {
        value = item->getInt(aField,aIndex);
    }
    catch (int aError)
    {
        error = aError;
    }

    SetJavaErrorCode(aJniEnv, aError, error);
    return value;
}

JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_PIMItemImpl__1addInt(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aItemHandle,
    jint aField,
    jint aAttributes,
    jint aValue)
{
    JELOG2(EPim);
    pimbaseitem* item = reinterpret_cast< pimbaseitem *>(aItemHandle);
    int error = 0;
    try
    {
        item->addInt(aField, aAttributes, aValue);
    }
    catch (int aError)
    {
        error = aError;
    }
    return error;
}

JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_PIMItemImpl__1setInt(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aItemHandle,
    jint aField,
    jint aIndex,
    jint aAttributes,
    jint aValue)
{
    JELOG2(EPim);
    pimbaseitem* item = reinterpret_cast< pimbaseitem *>(aItemHandle);
    int error = 0;
    try
    {
        item->setInt(aField, aIndex, aAttributes, aValue);
    }
    catch (int aError)
    {
        error = aError;
    }
    return error;
}

JNIEXPORT jstring
JNICALL Java_com_nokia_mj_impl_pim_PIMItemImpl__1getString(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aItemHandle,
    jint aField,
    jint aIndex,
    jintArray aError)
{
    JELOG2(EPim);
    pimbaseitem* item = reinterpret_cast< pimbaseitem *>(aItemHandle);

    jstring javaValue = item->callGetString(
                            aField,
                            aIndex,
                            aJniEnv,
                            aError);

    // Ownership of the string is not transferred.
    return javaValue;
}

JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_PIMItemImpl__1addString(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aItemHandle,
    jint aField,
    jint aAttributes,
    jstring aValue)
{
    JELOG2(EPim);
    pimbaseitem* item = reinterpret_cast< pimbaseitem *>(aItemHandle);
    int error = 0;
    try
    {
        item->callAddString(aField,aAttributes,aValue,aJniEnv);
    }
    catch (int aError)
    {
        error = aError;
    }
    return error;
}

JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_PIMItemImpl__1setString(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aItemHandle,
    jint aField,
    jint aIndex,
    jint aAttributes,
    jstring aValue)
{
    JELOG2(EPim);
    pimbaseitem* item = reinterpret_cast< pimbaseitem *>(aItemHandle);
    int error = 0;
    try
    {
        item->callSetString(aField, aIndex, aAttributes, aValue, aJniEnv);
    }
    catch (int aError)
    {
        error = aError;
    }
    return error;
}

JNIEXPORT jboolean
JNICALL Java_com_nokia_mj_impl_pim_PIMItemImpl__1getBoolean(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aItemHandle,
    jint aField,
    jint aIndex,
    jintArray aError)
{
    JELOG2(EPim);
    pimbaseitem* item = reinterpret_cast< pimbaseitem *>(aItemHandle);
    int error = 0;
    jboolean value = false;
    try
    {
        value = item->callGetBoolean(aField, aIndex);
    }
    catch (int aError)
    {
        error = aError;
    }
    SetJavaErrorCode(aJniEnv, aError, error);
    return value;
}

JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_PIMItemImpl__1addBoolean(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aItemHandle,
    jint aField,
    jint aAttributes,
    jboolean aValue)
{
    JELOG2(EPim);
    pimbaseitem* item = reinterpret_cast< pimbaseitem *>(aItemHandle);

    int error = 0;
    try
    {
        item->callAddBoolean(aField, aAttributes, aValue);
    }
    catch (int aError)
    {
        error = aError;
    }
    return error;
}

JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_PIMItemImpl__1setBoolean(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aItemHandle,
    jint aField,
    jint aIndex,
    jint aAttributes,
    jboolean aValue)
{
    JELOG2(EPim);
    pimbaseitem* item = reinterpret_cast< pimbaseitem *>(aItemHandle);

    int error = 0;
    try
    {
        item->callSetBoolean(aField, aIndex, aAttributes, aValue);
    }
    catch (int aError)
    {
        error = aError;
    }
    return error;
}

JNIEXPORT jobjectArray
JNICALL Java_com_nokia_mj_impl_pim_PIMItemImpl__1getStringArray(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aItemHandle,
    jint aField,
    jint aIndex,
    jintArray aError)
{
    JELOG2(EPim);
    pimbaseitem* item = reinterpret_cast< pimbaseitem *>(aItemHandle);

    jobjectArray javaStringArray = item->callGetStringArray(
                                       aField,
                                       aIndex,
                                       aJniEnv,
                                       aError);
    // Ownership of the string array is not transferred.
    return javaStringArray;
}

JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_PIMItemImpl__1addStringArray(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aItemHandle,
    jint aField,
    jint aAttributes,
    jobjectArray aValue)
{
    JELOG2(EPim);
    pimbaseitem* item = reinterpret_cast< pimbaseitem *>(aItemHandle);
    int error = 0;
    try
    {
        item->callAddStringArray(
            aJniEnv,
            aField,
            aAttributes,
            aValue);
    }
    catch (int aError)
    {
        error = aError;
    }
    return error;
}

JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_PIMItemImpl__1setStringArray(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aItemHandle,
    jint aField,
    jint aIndex,
    jint aAttributes,
    jobjectArray aValue)
{
    JELOG2(EPim);
    pimbaseitem* item = reinterpret_cast< pimbaseitem *>(aItemHandle);

    int error = 0;
    try
    {
        item->callSetStringArray(
            aJniEnv,
            aField,
            aIndex,
            aAttributes,
            aValue);
    }
    catch (int aError)
    {
        error = aError;
    }

    return error;
}

JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_PIMItemImpl__1countValues(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aItemHandle,
    jint aField)
{
    JELOG2(EPim);
    pimbaseitem* item = reinterpret_cast< pimbaseitem *>(aItemHandle);
    int retValue = 0;
    try
    {
        retValue = item->callCountValues(aField);
    }
    catch (int aError)
    {
        retValue = aError;
    }
    return retValue;
}

JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_PIMItemImpl__1removeValue(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aItemHandle,
    jint aField,
    jint aIndex)
{
    JELOG2(EPim);
    pimbaseitem* item = reinterpret_cast< pimbaseitem *>(aItemHandle);
    int error = 0;
    try
    {
        item->removeValue(aField,aIndex);
    }
    catch (int aError)
    {
        error = aError;
    }
    return error;
}

JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_PIMItemImpl__1getAttributes(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aItemHandle,
    jint aField,
    jint aIndex,
    jintArray aError)
{
    JELOG2(EPim);
    pimbaseitem* item = reinterpret_cast< pimbaseitem *>(aItemHandle);
    TPIMAttribute attributes = KPIMAttrNone;

    int error = 0;
    try
    {
        attributes = item->getAttributes(aField,aIndex);
    }
    catch (int aError)
    {
        error = aError;
    }
    SetJavaErrorCode(aJniEnv, aError, error);
    return attributes;
}

JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_PIMItemImpl__1addToCategory(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aItemHandle,
    jstring aCategory)
{
    JELOG2(EPim);
    pimbaseitem* item = reinterpret_cast< pimbaseitem *>(aItemHandle);
    int error = 0;
    try
    {
        item->callAddToCategory(aCategory,aJniEnv);
    }
    catch (int aError)
    {
        error = aError;
    }
    return error;
}

JNIEXPORT void
JNICALL Java_com_nokia_mj_impl_pim_PIMItemImpl__1removeFromCategory(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aItemHandle,
    jstring aCategory)
{
    JELOG2(EPim);
    pimbaseitem* item = reinterpret_cast< pimbaseitem *>(aItemHandle);
    item->callRemoveFromCategory(aCategory,aJniEnv);
}

JNIEXPORT jobjectArray
JNICALL Java_com_nokia_mj_impl_pim_PIMItemImpl__1getCategories(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aItemHandle)
{
    JELOG2(EPim);
    pimbaseitem* item = reinterpret_cast< pimbaseitem *>(aItemHandle);
    jobjectArray javaCategories =item->callGetCategories(aJniEnv);
    return javaCategories;
}

JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_PIMItemImpl__1maxCategories(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aItemHandle)
{
    JELOG2(EPim);
    pimbaseitem* item = reinterpret_cast< pimbaseitem *>(aItemHandle);
    int maxCategories = item->maxCategories();
    return maxCategories;
}

// End of File
