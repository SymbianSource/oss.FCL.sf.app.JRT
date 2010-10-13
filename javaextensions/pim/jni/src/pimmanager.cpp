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
* Description:  PIMManager JNI wrapper.
 *
*/


//  INCLUDE FILES
#include <jni.h>
#include "com_nokia_mj_impl_pim_PIMManager.h"
#include <badesca.h>
#include "pimcommon.h"
#include "pimbasemanager.h"
#include "pimbaseitem.h"
#include "mpimlocalizationmanager.h"
#include "pimpanics.h"
#include "logger.h"
#include "pimutils.h"

JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_PIMManager__1createPIMManager(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jintArray aError)
{
    JELOG2(EPim);
    int error = 0;
    pimbasemanager* manager = NULL;
    try
    {
        manager = pimbasemanager::getInstance();
    }
    catch (int aError)
    {
        error = aError;
    }
    SetJavaErrorCode(aJniEnv, aError, error);
    return reinterpret_cast<int>(manager);

}

JNIEXPORT void
JNICALL Java_com_nokia_mj_impl_pim_PIMManager__1dispose(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aManagerHandle)
{
    JELOG2(EPim);
    pimbasemanager* manager = reinterpret_cast< pimbasemanager *>(aManagerHandle);
    manager->dispose();

}

JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_PIMManager__1openPIMList(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aManagerHandle,
    jint aPimListType,
    jstring aPimListName,
    jintArray aError)
{
    JELOG2(EPim);
    pimbasemanager* manager = reinterpret_cast< pimbasemanager *>(aManagerHandle);
    pimbaselist* list = NULL;
    int error = 0;
    try
    {
        list = manager->openPimList(
                   static_cast< TPIMListType>(aPimListType),aPimListName, aJniEnv);
    }
    catch (int aError)
    {
        error = aError;
    }
    SetJavaErrorCode(aJniEnv, aError, error);

    // We now own the list (through the handle). The ownership of
    // the list is transferred to the Java side peer list object.
    return reinterpret_cast<int>(list);

}

JNIEXPORT jobjectArray
JNICALL Java_com_nokia_mj_impl_pim_PIMManager__1listPIMLists(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aManagerHandle,
    jint aPimListType,
    jintArray aError)
{
    JELOG2(EPim);
    pimbasemanager* manager =
        reinterpret_cast< pimbasemanager *>(aManagerHandle);

    jobjectArray javaStringArray = NULL;

    javaStringArray = manager->listPimLists(
                          static_cast< TPIMListType>(aPimListType),
                          aError,
                          aJniEnv);

    return javaStringArray;
}



// End of File
