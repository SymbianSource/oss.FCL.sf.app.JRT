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


#ifndef JAVAJNIUTILS_H
#define JAVAJNIUTILS_H

#include <string>
#include <jni.h>
#include "javaosheaders.h"

namespace java
{
namespace util
{

/**
 * This class contains jni utility operations.
 */
class JniUtils
{
public:
    OS_IMPORT static std::wstring jstringToWstring(JNIEnv* env, const jstring& str);
    OS_IMPORT static jstring wstringToJstring(JNIEnv* env, const std::wstring& str);

    /**
     * Throws exception from native side to java side.
     * @param aEnv JNIEnv object.
     * @param aClassNameOfException class name and name of the exception.
     * @param aExceptionText exception text added to java exception.
     * Example:
     * throwNewException(aEnv,"java/lang/NullPointerException","Creation of native handler failed");
     */
    OS_IMPORT static void throwNewException(JNIEnv* aEnv,const std::string& aClassNameOfException,
                                            const std::string& aExceptionText);
};

} //end namespace util
} //end namespace java

#endif // JAVAJNIUTILS_H

