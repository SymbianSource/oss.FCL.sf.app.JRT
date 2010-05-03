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

#include "logger.h"
#include "jstringutils.h"

#include "com_nokia_mj_impl_fileutils_FileDRMContentHandler.h"
#include "filedrmcontenthandler.h"
#include "fileextendedcommon.h" // To be removed once common utility is fixed

using namespace java::fileutils;

/*
 * Class:     com_nokia_mj_impl_fileutils_FileDRMContentHandler
 * Method:    _isProtectedFile
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_mj_impl_fileutils_FileDRMContentHandler__1isProtectedFile
(JNIEnv *aJni, jclass, jstring aName)
{
    JStringUtils name(*aJni, aName);
    return FileDRMContentHandler::isDRMProtectedFile(name);
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FileDRMContentHandler
 * Method:    _setDrmArguments
 * Signature: (IZI)I
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_fileutils_FileDRMContentHandler__1setDrmArguments
(JNIEnv *, jobject, jint aHandle, jboolean aExecuteIntent, jint aIntent)
{
    FileDRMContentHandler* handler =
        reinterpret_cast<FileDRMContentHandler*>(aHandle);
    return handler->setDrmArguments(aIntent, aExecuteIntent);
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FileDRMContentHandler
 * Method:    _createDrmHandler
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_fileutils_FileDRMContentHandler__1createDrmHandler
(JNIEnv *aJni, jobject, jstring aName)
{
    try
    {
        std::wstring name = java::fileutility::FileUtil::jstringToWstring(aJni, aName);
        FileDRMContentHandler* handler = new FileDRMContentHandler(name);
        return reinterpret_cast<jint>(handler);
    }
    catch (...)
    {
        ELOG(EJavaFile,
             "FileDRMContentHandler: JNI: exists caught exception. Return 0");
        return 0;
    }
}
