/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "javaoslayer.h"
#include "com_nokia_mj_impl_utils_StartUpTrace.h"
#include "exceptionbase.h"

JNIEXPORT void JNICALL
Java_com_nokia_mj_impl_utils_StartUpTrace__1doTrace(
    JNIEnv* env, jclass, jstring header,
    jlong freeMem, jlong totalMem)
{
    try
    {
        jboolean iscopy;
        const char* utf8(env->GetStringUTFChars(header, &iscopy));
        java::util::JavaOsLayer::startUpTrace(utf8, freeMem, totalMem);
        env->ReleaseStringUTFChars(header, utf8);
    }
    catch (java::util::ExceptionBase& ex)
    {
        ELOG1(EUtils,"%s",ex.toString().c_str());
    }
    catch (...)
    {
        ELOG(EUtils,"ERROR!!! MidpRuntime.getLocale(): Unexpected exception was caught");
    }
}
