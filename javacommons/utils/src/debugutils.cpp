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
* Description:  ?Description
*
*/


#include "logger.h"
#include "javacommonutils.h"
#include "threaddumper.h"


#include "com_nokia_mj_impl_utils_DebugUtils.h"

using namespace java::util;


JNIEXPORT void JNICALL Java_com_nokia_mj_impl_utils_DebugUtils__1doThreadDump
(JNIEnv* /*aEnv*/, jclass)
{
    ThreadDump::doDump();
}


JNIEXPORT void JNICALL Java_com_nokia_mj_impl_utils_DebugUtils__1closeThreadDumper
(JNIEnv* /*aEnv*/, jclass)
{
    ThreadDump::closeDumper();
}

/**
 * A utility for getting the stack trace for further processing.
 * @param th The throwable from where the stack trace should be get.
 * @param printStream Where to print the stack trace.
 */
JNIEXPORT void JNICALL
Java_com_nokia_mj_impl_utils_DebugUtils__1getStackTrace
(JNIEnv* jniEnv, jclass, jthrowable th, jobject printStream)
{
    /*
     * call Throwable.printStackTrace(java.io.PrintStream)
     * this method is not part of CLDC spec, but it's supported by VM vendor.
     */
    jclass classThrowable = jniEnv->GetObjectClass(th);
    if (classThrowable)
    {
        jmethodID methodId = jniEnv->GetMethodID(classThrowable,
                             "printStackTrace",
                             "(Ljava/io/PrintStream;)V");
        if (methodId)
        {
            jniEnv->CallVoidMethod(th, methodId, printStream);
        }
    }
}
