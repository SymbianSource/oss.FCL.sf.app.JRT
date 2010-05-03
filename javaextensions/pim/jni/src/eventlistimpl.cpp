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
* Description:  EventListImpl JNI wrapper.
 *
*/


//  INCLUDE FILES

#include "com_nokia_mj_impl_pim_EventListImpl.h"
#include "pimcommon.h"
#include "pimbaselist.h"
#include "pimbaseitem.h"
#include "pimbaserepeatrule.h"
#include "pimutils.h"
#include "logger.h"

JNIEXPORT jintArray
JNICALL Java_com_nokia_mj_impl_pim_EventListImpl__1itemsByDate(
    JNIEnv* aJniEnv, jobject /*aPeer*/, jint aEventListHandle,
    jint aSearchType, jlong aStartDate, jlong aEndDate,
    jboolean aInitialEventOnly, jintArray aError)
{

    JELOG2(EPim);
    pimbaselist* eventList =
        reinterpret_cast< pimbaselist*>(aEventListHandle);
    jintArray itemHandles = eventList->itemsByDate(
                                aSearchType, aStartDate, aEndDate, aInitialEventOnly,
                                aJniEnv, aError);

    return itemHandles;
}

JNIEXPORT jintArray
JNICALL
Java_com_nokia_mj_impl_pim_EventListImpl__1getSupportedRepeatRuleFields(
    JNIEnv* aJniEnv, jobject /*aPeer*/, jint aEventListHandle,
    jint aFrequency, jintArray aError)
{
    JELOG2(EPim);
    pimbaselist* eventList = reinterpret_cast< pimbaselist *>(aEventListHandle);
    jintArray javaFields = eventList->getSupportedRepeatRuleFields(
                               aFrequency, aJniEnv, aError);
    return javaFields;
}

JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_EventListImpl__1createRepeatRuleHandle(
    JNIEnv* aJniEnv, jobject /*aPeer*/, jint aEventItemHandle,
    jintArray aError)
{
    JELOG2(EPim);
    pimbaseitem* eventItem = reinterpret_cast< pimbaseitem *>(aEventItemHandle);
    int repeatRuleHandle = 0;
    int error = 0;
    try
    {
        repeatRuleHandle = eventItem->getRepeatHandle();
    }
    catch (int aErr)
    {
        error = aErr;
    }
    SetJavaErrorCode(aJniEnv, aError, error);
    return repeatRuleHandle;
}
