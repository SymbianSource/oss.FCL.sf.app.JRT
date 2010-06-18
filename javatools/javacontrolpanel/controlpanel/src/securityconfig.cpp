/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <string>
#include "com_nokia_mj_impl_javacontrolpanel_SecurityConfig.h"
#include "javajniutils.h"

#ifdef __SYMBIAN32__
#include "security.h"
#endif

/*
 * Class:     com_nokia_mj_impl_javacontrolpanel_SecurityConfig
 * Method:    _getSecurityWarningsMode
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_javacontrolpanel_SecurityConfig__1getSecurityWarningsMode
  (JNIEnv* aEnv, jobject)
{
#ifdef __SYMBIAN32__
    Security security;
    std::wstring mode = security.getSecurityWarningsMode();
    return java::util::JniUtils::wstringToJstring(aEnv, mode);
#else
    return java::util::JniUtils::wstringToJstring(aEnv, L"2");;
#endif
}

/*
 * Class:     com_nokia_mj_impl_javacontrolpanel_SecurityConfig
 * Method:    _setSecurityWarningsMode
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_javacontrolpanel_SecurityConfig__1setSecurityWarningsMode
  (JNIEnv* aEnv, jobject, jstring aMode)
{
#ifdef __SYMBIAN32__
    Security security;
    std::wstring mode = java::util::JniUtils::jstringToWstring(aEnv, aMode);
    security.setSecurityWarningsMode(mode);
#endif
}

/*
 * Class:     com_nokia_mj_impl_javacontrolpanel_SecurityConfig
 * Method:    _getSecurityPolicy
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_javacontrolpanel_SecurityConfig__1getSecurityPolicy
  (JNIEnv *, jobject)
{
#ifdef __SYMBIAN32__
    Security security;
    return security.getSecurityPolicy();
#else
    return 1;
#endif
}

/*
 * Class:     com_nokia_mj_impl_javacontrolpanel_SecurityConfig
 * Method:    _setSecurityPolicy
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_javacontrolpanel_SecurityConfig__1setSecurityPolicy
  (JNIEnv *, jobject, jint aPolicyId)
{
#ifdef __SYMBIAN32__
    Security security;
    security.setSecurityPolicy(aPolicyId);
#endif
}

