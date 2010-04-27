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
* Description:  ContactImpl JNI wrapper.
 *
*/


//  INCLUDE FILES

#include "com_nokia_mj_impl_pim_ContactImpl.h"
#include "pimbasemanager.h"
#include "pimbaseitem.h"
#include "pimcommon.h"
#include "pimutils.h"
#include "logger.h"


JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_ContactImpl__1createNativePeer(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aManagerHandle,
    jintArray aError)
{
    JELOG2(EPim);
    pimbasemanager* pimManager = reinterpret_cast< pimbasemanager*>(aManagerHandle);
    pimbaseitem* item = NULL;
    int error = 0;
    try
    {
        item = pimbaseitem::getContactItemInstance(pimManager);
    }
    catch (int aError)
    {
        error = aError;
    }
    SetJavaErrorCode(aJniEnv, aError, error);

    return reinterpret_cast<int>(item);

}

JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_ContactImpl__1getPreferredIndex(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aContactItemHandle,
    jint aField,
    jintArray aError)
{
    JELOG2(EPim);
    pimbaseitem* item = reinterpret_cast< pimbaseitem *>(aContactItemHandle);
    int error = 0;
    int preferredIndex = 0;
    try
    {
        preferredIndex = item->getPreferredIndex(aField);
    }
    catch (int aError)
    {
        error = aError;
    }
    SetJavaErrorCode(aJniEnv, aError, error);

    return preferredIndex;
}

// End of File
