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
* Description:  JNI Layer corresponding to FileSystemUtils
 *
*/


#include <string>
#include <stdlib.h>

#include "javajniutils.h"
#include "javacommonutils.h"

#include "com_nokia_mj_impl_file_FileSystemUtils.h"

/*
 * Class:     com_nokia_mj_impl_file_FileSystemUtils
 * Method:    _listRoots
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_file_FileSystemUtils__1listRoots
(JNIEnv *aJni, jclass)
{
    std::wstring propVal = java::util::JavaCommonUtils::utf8ToWstring(getenv("HOME"));
    return java::util::JniUtils::wstringToJstring(aJni, propVal);
}
