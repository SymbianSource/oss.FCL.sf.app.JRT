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
* Description:  JNI Layer for File DRM Handler
 *
*/


#include "com_nokia_mj_impl_fileutils_FileDRMContentHandler.h"

/*
 * Class:     com_nokia_mj_impl_fileutils_FileDRMContentHandler
 * Method:    _isProtectedFile
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_mj_impl_fileutils_FileDRMContentHandler__1isProtectedFile
(JNIEnv *aJni, jclass, jstring aName)
{
    return false;
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FileDRMContentHandler
 * Method:    _setDrmArguments
 * Signature: (IZI)I
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_fileutils_FileDRMContentHandler__1setDrmArguments
(JNIEnv *, jobject, jint, jboolean, jint)
{
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FileDRMContentHandler
 * Method:    _createDrmHandler
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_fileutils_FileDRMContentHandler__1createDrmHandler
(JNIEnv *aJni, jobject, jstring aName)
{
    return 0;
}
