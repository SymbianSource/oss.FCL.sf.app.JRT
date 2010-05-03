/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


#include "logger.h"
#include "javacommonutils.h"
#include "com_nokia_mj_impl_properties_common_Locale.h"
#include "com_nokia_mj_impl_properties_common_Platform.h"
#include "com_nokia_mj_impl_properties_common_FreeMemory.h"
#include "commonproperties.h"
#include "exceptionbase.h"

using namespace java::util;

JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_properties_common_Locale__1getLocale
(JNIEnv* env, jobject /*peer*/)
{
    JELOG2(EUtils);
    jstring result = 0;
    try
    {
        result = getLocaleImpl(env);
    }
    catch (ExceptionBase& ex)
    {
        ELOG1(EUtils,"%s",ex.toString().c_str());
    }
    catch (...)
    {
        ELOG(EUtils,"ERROR!!! MidpRuntime.getLocale(): Unexpected exception was caught");
    }
    return result;
}

JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_properties_common_Platform__1getPlatform
(JNIEnv* env, jobject /*peer*/)
{
    JELOG2(EUtils);
    jstring result = 0;
    try
    {
        result = getPlatformImpl(env);
    }
    catch (ExceptionBase& ex)
    {
        ELOG1(EUtils,"%s",ex.toString().c_str());
    }
    catch (...)
    {
        ELOG(EUtils,"ERROR!!! MidpRuntime.getPlatform(): Unexpected exception was caught");
    }
    return result;
}


JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_properties_common_FreeMemory__1getFreeMemory
(JNIEnv* env, jobject /*peer*/)
{
    JELOG2(EUtils);
    jint freeMemory = -1;
    try
    {
        freeMemory = getFreeMemoryImpl(env);
    }
    catch (ExceptionBase& ex)
    {
        ELOG1(EUtils,"%s",ex.toString().c_str());
    }
    catch (...)
    {
        ELOG(EUtils,"ERROR!!! getFreeMemoryImpl(): Unexpected exception was caught");
    }
    return freeMemory;
}
