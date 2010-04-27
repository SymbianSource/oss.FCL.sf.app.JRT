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
* Description:  EventImpl JNI wrapper.
 *
*/


//  INCLUDE FILES

#include "com_nokia_mj_impl_pim_EventImpl.h"
#include "pimbasemanager.h"
#include "pimbaseitem.h"
#include "pimbaserepeatrule.h"
#include "pimcommon.h"
#include "pimutils.h"
#include "pimpanics.h"
#include "logger.h"

JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_EventImpl__1createNativePeer(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aManagerHandle,
    jintArray aRepeatRuleHandle,
    jintArray aError)
{
    JELOG2(EPim);
    pimbasemanager* pimManager = reinterpret_cast< pimbasemanager*>(aManagerHandle);
    pimbaseitem* eventItem;
    int error = 0;
    try
    {
        eventItem = pimbaseitem::getEventItemInstance(pimManager);
    }
    catch (int aErr)
    {
        error = aErr;
        SetJavaErrorCode(aJniEnv, aError, error);
        return 0;
    }
    SetJavaErrorCode(aJniEnv, aError, error);
    int eventItemHandle = reinterpret_cast<int>(eventItem);

    int repeatRuleHandle = eventItem->getRepeatHandle();
    aJniEnv->SetIntArrayRegion(aRepeatRuleHandle, 0, 1, &repeatRuleHandle);

    // Ownership of the item is transferred to the Java side peer object
    return eventItemHandle;
}

JNIEXPORT jboolean
JNICALL Java_com_nokia_mj_impl_pim_EventImpl__1isRepeating(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aEventHandle)
{
    JELOG2(EPim);
    pimbaseitem* eventItem = reinterpret_cast< pimbaseitem*>(aEventHandle);
    bool isRepeating = eventItem->isItemRepeating();
    return isRepeating;
}

JNIEXPORT void
JNICALL Java_com_nokia_mj_impl_pim_EventImpl__1setRepeating(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aEventHandle,
    jboolean aSetRepeating)
{
    JELOG2(EPim);
    pimbaseitem* eventItem = reinterpret_cast< pimbaseitem *>(aEventHandle);
    eventItem->setItemRepeating(aSetRepeating);
}

JNIEXPORT void
JNICALL Java_com_nokia_mj_impl_pim_EventImpl__1clearRepeatRule(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aRepeatRuleHandle)
{
    JELOG2(EPim);
    pimbaserepeatrule* repeatRule = reinterpret_cast< pimbaserepeatrule*>(aRepeatRuleHandle);
    repeatRule->clear();
}

// End of File
