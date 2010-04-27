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
* Description:  Common utils needed for file extended
 *
*/

#include "logger.h"
#include "fileextendedcommon.h"

using namespace std;
using namespace java::fileutility;

wstring FileUtil::jstringToWstring(JNIEnv* env, const jstring& jStr)
{
#ifdef __SYMBIAN32__
    jboolean iscopy = NULL;
    const jchar* jChr = env->GetStringChars(jStr, &iscopy);
    std::wstring wKey((wchar_t*)jChr);
    env->ReleaseStringChars(jStr, jChr);
    return wKey;
#else
    // In Linux, size of jchar is 2 and wchar_t is 4 bytes
    // need to compensate that.
    const jchar* jChr = env->GetStringChars(jStr, NULL);
    int length = env->GetStringLength(jStr);
    wchar_t *wchr = new wchar_t[length + 1];

    int i = 0;
    for (i = 0; i < length; i++)
    {
        wchr[i] = jChr[i];
    }

    wchr[i] = L'\0';
    std::wstring ret(wchr);
    env->ReleaseStringChars(jStr, jChr);
    delete[] wchr;
    return ret;
#endif
}
