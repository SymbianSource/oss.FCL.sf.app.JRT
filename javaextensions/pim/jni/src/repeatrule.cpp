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
* Description:  RepeatRule JNI wrapper.
 *
*/


//  INCLUDE FILES

#include "javax_microedition_pim_RepeatRule.h"
#include "pimbaserepeatrule.h"
#include "pimcommon.h"
#include "pimutils.h"
#include "logger.h"

JNIEXPORT jint
JNICALL Java_javax_microedition_pim_RepeatRule__1createRepeatRule(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jintArray aError)
{
    JELOG2(EPim);
    pimbaserepeatrule* repeatRule = NULL;
    int error = 0;
    try
    {
        repeatRule = pimbaserepeatrule::getInstance();
    }
    catch (int aError)
    {
        error = aError;
    }
    SetJavaErrorCode(aJniEnv, aError, error);

    return reinterpret_cast<int>(repeatRule);
}

JNIEXPORT void
JNICALL Java_javax_microedition_pim_RepeatRule__1dispose(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aRepeatRuleHandle)
{
    JELOG2(EPim);
    pimbaserepeatrule* repeatRule =
        reinterpret_cast< pimbaserepeatrule*>(aRepeatRuleHandle);
    delete repeatRule;
}

JNIEXPORT jintArray
JNICALL Java_javax_microedition_pim_RepeatRule__1dates(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aRepeatRuleHandle,
    jlong aStartDate,
    jlong aSubsetBeginning,
    jlong aSubsetEnding)
{
    JELOG2(EPim);
    pimbaserepeatrule* repeatRule =
        reinterpret_cast< pimbaserepeatrule*>(aRepeatRuleHandle);
    jintArray repeatDatesIntArray = repeatRule->dates(
                                        aStartDate,
                                        aSubsetBeginning,
                                        aSubsetEnding,
                                        aJniEnv);

    return repeatDatesIntArray;
}

JNIEXPORT void
JNICALL Java_javax_microedition_pim_RepeatRule__1addExceptDate(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aRepeatRuleHandle,
    jlong aValue)
{
    JELOG2(EPim);
    pimbaserepeatrule* repeatRule =
        reinterpret_cast< pimbaserepeatrule*>(aRepeatRuleHandle);

    repeatRule->addExceptDate(aValue);
}

JNIEXPORT void
JNICALL Java_javax_microedition_pim_RepeatRule__1removeExceptDate(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aRepeatRuleHandle,
    jlong aValue)
{
    JELOG2(EPim);
    pimbaserepeatrule* repeatRule =
        reinterpret_cast< pimbaserepeatrule*>(aRepeatRuleHandle);

    repeatRule->removeExceptDate(aValue);
}

JNIEXPORT jintArray
JNICALL Java_javax_microedition_pim_RepeatRule__1getExceptDates(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aRepeatRuleHandle)
{
    JELOG2(EPim);
    pimbaserepeatrule* repeatRule =
        reinterpret_cast< pimbaserepeatrule*>(aRepeatRuleHandle);

    jintArray exceptDatesIntArray = repeatRule->getExceptDates(aJniEnv);

    return exceptDatesIntArray;
}

JNIEXPORT jint
JNICALL Java_javax_microedition_pim_RepeatRule__1getInt(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aRepeatRuleHandle,
    jint aField,
    jintArray aError)
{
    JELOG2(EPim);
    pimbaserepeatrule* repeatRule =
        reinterpret_cast< pimbaserepeatrule*>(aRepeatRuleHandle);

    int retVal = 0;
    int error = 0;
    try
    {
        retVal = repeatRule->getRepeatInt(aField);
    }
    catch (int aError)
    {
        error = aError;
    }
    SetJavaErrorCode(aJniEnv, aError, error);
    return retVal;
}

JNIEXPORT void
JNICALL Java_javax_microedition_pim_RepeatRule__1setInt(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aRepeatRuleHandle,
    jint aField,
    jint aValue,
    jintArray aError)
{
    JELOG2(EPim);
    pimbaserepeatrule* repeatRule =
        reinterpret_cast< pimbaserepeatrule*>(aRepeatRuleHandle);
    int error = 0;
    try
    {
        repeatRule->setRepeatInt(aField, aValue);
    }
    catch (int aError)
    {
        error = aError;
    }

    SetJavaErrorCode(aJniEnv, aError, error);
}

JNIEXPORT jlong
JNICALL Java_javax_microedition_pim_RepeatRule__1getDate(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aRepeatRuleHandle,
    jint aField,
    jintArray aError)
{
    JELOG2(EPim);
    pimbaserepeatrule* repeatRule =
        reinterpret_cast< pimbaserepeatrule*>(aRepeatRuleHandle);
    jlong retVal = 0;
    int error = 0;
    try
    {
        retVal = repeatRule->getRepeatDate(aField);
    }
    catch (int aError)
    {
        error = aError;
    }
    SetJavaErrorCode(aJniEnv, aError, error);
    return retVal;
}

JNIEXPORT void
JNICALL Java_javax_microedition_pim_RepeatRule__1setDate(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aRepeatRuleHandle,
    jint aField,
    jlong aValue,
    jintArray aError)
{
    JELOG2(EPim);
    pimbaserepeatrule* repeatRule =
        reinterpret_cast< pimbaserepeatrule*>(aRepeatRuleHandle);
    int error = 0;
    try
    {
        repeatRule->setDate(aField, aValue);
    }
    catch (int aError)
    {
        error = aError;
    }
    SetJavaErrorCode(aJniEnv, aError, error);
}

JNIEXPORT jintArray
JNICALL Java_javax_microedition_pim_RepeatRule__1getFields(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aRepeatRuleHandle)
{
    JELOG2(EPim);
    pimbaserepeatrule* repeatRule =
        reinterpret_cast< pimbaserepeatrule*>(aRepeatRuleHandle);

    jintArray javaFields = repeatRule->getFields(aJniEnv);

    return javaFields;
}

// End of File
