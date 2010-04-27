/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  formatternative
*
*/


#include <time.h>

#include "com_nokia_mj_impl_utils_Formatter.h"
#include "com_nokia_mj_impl_utils_ResourceLoader.h"
#include "javacommonutils.h"
#include "javajniutils.h"
#include "logger.h"

JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_utils_Formatter__1formatInteger
(JNIEnv *aJni, jobject, jint aNumber)
{
    // Stub implementation
    std::string intStr = java::util::JavaCommonUtils::intToString(aNumber);
    return aJni->NewStringUTF(intStr.c_str());
}

JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_utils_Formatter__1formatDate
(JNIEnv * aJni, jobject, jlong timeInMillis)
{
    // Stub implementation. Missing localisation.
    time_t javaTime = timeInMillis/1000;
    char buffer[80];
    struct tm* timeInfo;
    timeInfo = localtime(&javaTime);

    // European format as default.
    strftime(buffer, 80, "%d/%m/%Y", timeInfo);

    std::string dateStr(buffer);

    return aJni->NewStringUTF(dateStr.c_str());
}

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_utils_ResourceLoader__1getLocaleId
(JNIEnv *, jobject)

{
    // Stub implementation. This triggers using default "sc" engineering
    // english at Java side.
    return (jint) -1;
}

