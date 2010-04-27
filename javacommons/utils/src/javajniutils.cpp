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
* Description:
*
*/


#include "logger.h"
#include "javacommonutils.h"
#include "javajniutils.h"

using namespace java::util;


OS_EXPORT
std::wstring JniUtils::jstringToWstring(JNIEnv* env, const jstring& jStr)
{
    jboolean iscopy;
#ifdef __SYMBIAN32__
    // In Symbian the size of jchar == the size of wchar_t
    const jchar* jChr = env->GetStringChars(jStr, &iscopy);
    std::wstring wStr((wchar_t *)jChr, env->GetStringLength(jStr));
    env->ReleaseStringChars(jStr, jChr);
#else
    // The size of jchar != the size of wchar_t
    const char* utf8(env->GetStringUTFChars(jStr, &iscopy));
    std::wstring wStr = JavaCommonUtils::utf8ToWstring(utf8);
    env->ReleaseStringUTFChars(jStr, utf8);
#endif

    return wStr;
}

OS_EXPORT
jstring JniUtils::wstringToJstring(JNIEnv* env, const std::wstring& wStr)
{
#ifdef __SYMBIAN32__
    // In Symbian the size of jchar == the size of wchar_t
    jstring jStr = env->NewString(
                       reinterpret_cast<const jchar*>(wStr.c_str()), wStr.length());
#else
    // The size of jchar != the size of wchar_t
    const char* utf8 = JavaCommonUtils::wstringToUtf8(wStr);
    jstring jStr = env->NewStringUTF(utf8);
    delete[] utf8;
#endif

    return jStr;
}

OS_EXPORT
void JniUtils::throwNewException(JNIEnv* aEnv,const std::string& aClassNameOfException,
                                 const std::string& aExceptionText)
{
    jclass cls = aEnv->FindClass(aClassNameOfException.c_str());
    // if cls is NULL, an exception has already been thrown
    if (cls != 0)
    {
        aEnv->ThrowNew(cls, aExceptionText.c_str());
    }
    // free the local ref
    aEnv->DeleteLocalRef(cls);
}

