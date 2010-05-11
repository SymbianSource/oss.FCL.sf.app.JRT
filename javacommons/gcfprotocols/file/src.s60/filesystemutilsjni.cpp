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
* Description:  JNI Layer corresponding to FileSystemUtils.java
 *
*/


#include <e32cmn.h>
#include <f32file.h>

#include "javajniutils.h"
#include "javasymbianoslayer.h"
#include "s60commonutils.h"
#include "logger.h"
#include "javaoslayer.h"

#include "systempropertyprovider.h"
#include "com_nokia_mj_impl_file_FileSystemUtils.h"

using namespace std;
using namespace java::file;
using namespace java::util;

/*
 * Class:     com_nokia_mj_impl_file_FileSystemUtils
 * Method:    _getMemoryCardDrivePath
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_file_FileSystemUtils__1getMemoryCardDrivePath
(JNIEnv *aJni, jclass)
{
    JELOG2(EJavaFile);
    TFileName fileName;
    SystemPropertyProvider::GetMemoryCardPath(fileName);
    wstring name((wchar_t*)fileName.PtrZ());
    return JniUtils::wstringToJstring(aJni, name);
}

/*
 * Class:     com_nokia_mj_impl_file_FileSystemUtils
 * Method:    _getTemporaryDrivePath
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_file_FileSystemUtils__1getTemporaryDrivePath
(JNIEnv *aJni, jclass)
{
    JELOG2(EJavaFile);
    TFileName fileName;
    SystemPropertyProvider::GetTemporaryDrive(fileName);
    wstring name((wchar_t*)fileName.PtrZ());
    return JniUtils::wstringToJstring(aJni, name);
}

/*
 * Class:     com_nokia_mj_impl_file_FileSystemUtils
 * Method:    _getRomDrivePath
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_file_FileSystemUtils__1getRomDrivePath
(JNIEnv *aJni, jclass)
{
    JELOG2(EJavaFile);
    TFileName fileName;
    SystemPropertyProvider::GetRomDrive(fileName);
    wstring name((wchar_t*)fileName.PtrZ());
    return JniUtils::wstringToJstring(aJni, name);
}

/*
 * Class:     com_nokia_mj_impl_file_FileSystemUtils
 * Method:    _getDefaultRootPath
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_file_FileSystemUtils__1getDefaultRootPath
(JNIEnv *aJni, jclass)
{
    JELOG2(EJavaFile);
    TFileName fileName;
    SystemPropertyProvider::GetDefaultRoot(fileName);
    wstring name((wchar_t*)fileName.PtrZ());
    return JniUtils::wstringToJstring(aJni, name);
}

/*
 * Class:     com_nokia_mj_impl_file_FileSystemUtils
 * Method:    _getPathOfProperty
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_file_FileSystemUtils__1getPathOfProperty
(JNIEnv *aJni, jclass, jint aKey, jboolean addRoot)
{
    JELOG2(EJavaFile);
    TFileName fileName;
    SystemPropertyProvider::GetPathOfProperty(aKey, fileName, addRoot);
    wstring name((wchar_t*)fileName.PtrZ());
    return JniUtils::wstringToJstring(aJni, name);
}

/*
 * Class:     com_nokia_mj_impl_file_FileSystemUtils
 * Method:    _getMidpRoot
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_file_FileSystemUtils__1getMidpRoot
(JNIEnv *aEnv, jclass)
{
    std::string path;
    java::util::JavaOsLayer::getMidpRoot(path);

    return aEnv->NewStringUTF((const char*)(path.c_str()));
}
