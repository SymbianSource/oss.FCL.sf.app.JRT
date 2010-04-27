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
* Description:  JNI utils
*
*/


#ifndef SENSORJNIUTILS_H
#define SENSORJNIUTILS_H
#include <jni.h>
#include "sensornativeconstants.h"

/**
 * Handles JNI exception which may have occured during the previous JNI
 * invocation or Java function call.
 *
 * Prints the pending exception and a backtrace of the stack to the
 * system error-reporting channel System.out.err.This function has the
 * side effect of clearing the pending exception.
 *
 * @param aJniEnv Reference to valid JNI environment
 * @return KErrNone if no exceptions are pending. Other system-wide error
 *         code in exception situations
 */
inline int HandleException(JNIEnv& aJniEnv)
{
    int err = ERROR_NONE;
    if (aJniEnv.ExceptionCheck())
    {
        // Previous JNI operation caused an exception. Print the stack
        // trace and clear any pending exceptions from the Java VM
        aJniEnv.ExceptionDescribe();
        aJniEnv.ExceptionClear();
        err = ERROR_GENERAL;
    }
    return err;
}

#endif // SENSORJNIUTILS_H

