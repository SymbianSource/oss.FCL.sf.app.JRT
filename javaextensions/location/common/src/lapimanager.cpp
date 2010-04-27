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
* Description:  JNI implementation of LAPIManager class
 *
*/


// INTERNAL INCLUDES
#include "com_nokia_mj_impl_location_LAPIManager.h"

// EXTERNAL INCLUDES
#include "logger.h"
#include "locationfunctionserver.h"

/*
 * Class:     com_nokia_mj_impl_location_LAPIManager
 * Method:    _createFunctionSource
 * Signature: (I)I
 */
JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_location_LAPIManager__1createFunctionSource(
    JNIEnv* aJniEnv,
    jobject aPeer)
{
    JELOG2(EJavaLocation);
    java::location::LocationFunctionServer* mFunctionServer;
    mFunctionServer = new java::location::LocationFunctionServer(*aJniEnv,
            aPeer);
    return reinterpret_cast<jint>(mFunctionServer);
}

/*
 * Class:     com_nokia_mj_impl_location_LAPIManager
 * Method:    _dispose
 * Signature: (I)V
 */
JNIEXPORT void
JNICALL Java_com_nokia_mj_impl_location_LAPIManager__1dispose(
    JNIEnv* ,
    jobject /*aPeer*/,
    jint aEventSourceHandle)
{
    JELOG2(EJavaLocation);
    LOG(EJavaLocation, EInfo,
        "Java_com_nokia_mj_impl_location_LAPIManager__1dispose() called.");

    java::location::LocationFunctionServer* mFunctionServer =
        reinterpret_cast< java::location::LocationFunctionServer*>(
            aEventSourceHandle);

    delete mFunctionServer;
}

// End of file
