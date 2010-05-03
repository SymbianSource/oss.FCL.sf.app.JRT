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
* Description:  JNI Layer for FilePlatformSpecifics
 *
*/

#include <memory>
#include <caf/content.h>

#include "javajniutils.h"
#include "jstringutils.h"
#include "logger.h"

#include "filemanager.h"
#include "fileextendedcommon.h"
#include "s60filesystemutilities.h"
#include "com_nokia_mj_impl_fileutils_FilePlatformSpecifics.h"

using namespace java::util;
using namespace java::fileutils;
using namespace java::fileutility;

/*
 * Class:     com_nokia_mj_impl_fileutils_FilePlatformSpecifics
 * Method:    _copy
 * Signature: (Ljava/lang/String;Ljava/lang/String;Z)Z
 */
JNIEXPORT jboolean JNICALL  Java_com_nokia_mj_impl_fileutils_FilePlatformSpecifics__1copy(
    JNIEnv *aJni, jclass, jstring source, jstring destination,
    jboolean overwrite)
{
    std::wstring sourcePath = FileUtil::jstringToWstring(aJni, source);
    std::wstring destPath = FileUtil::jstringToWstring(aJni, destination);

    return FileManager::copy(sourcePath, destPath, overwrite);
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FilePlatformSpecifics
 * Method:    _move
 * Signature: (Ljava/lang/String;Ljava/lang/String;Z)Z
 */
JNIEXPORT jboolean JNICALL  Java_com_nokia_mj_impl_fileutils_FilePlatformSpecifics__1move(
    JNIEnv *aJni, jclass, jstring source, jstring destination,
    jboolean overwrite)
{
    std::wstring sourcePath = FileUtil::jstringToWstring(aJni, source);
    std::wstring destPath = FileUtil::jstringToWstring(aJni, destination);

    return FileManager::move(sourcePath, destPath, overwrite);
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FilePlatformSpecifics
 * Method:    _copyAll
 * Signature: (Ljava/lang/String;Ljava/lang/String;Z)Z
 */
JNIEXPORT jboolean JNICALL  Java_com_nokia_mj_impl_fileutils_FilePlatformSpecifics__1copyAll(
    JNIEnv *aJni, jclass, jstring source, jstring destination,
    jboolean overwrite)
{
    std::wstring sourcePath = FileUtil::jstringToWstring(aJni, source);
    std::wstring destPath = FileUtil::jstringToWstring(aJni, destination);

    return FileManager::copyAll(sourcePath, destPath, overwrite);
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FilePlatformSpecifics
 * Method:    _deleteAll
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL  Java_com_nokia_mj_impl_fileutils_FilePlatformSpecifics__1deleteAll(
    JNIEnv *aJni, jclass, jstring target)
{
    std::wstring sourcePath = FileUtil::jstringToWstring(aJni, target);

    return FileManager::deleteAll(sourcePath);
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FilePlatformSpecifics
 * Method:    _setHidden
 * Signature: (Ljava/lang/String;Z)V
 */
JNIEXPORT void JNICALL  Java_com_nokia_mj_impl_fileutils_FilePlatformSpecifics__1setHidden(
    JNIEnv *aJni, jclass, jstring fileName, jboolean hidden)
{
    JStringUtils name(*aJni, fileName);
    TBool hide = hidden;

    TRAPD(err, S60FileSystemUtilities::SetHiddenL(name, hide));
    if (err != KErrNone)
    {
        ELOG1(EJavaFile,
              "Java_com_nokia_mj_impl_file_FileSystemUtils__1sethidden %d",
              err);
        JniUtils::throwNewException(aJni, "java/io/IOException", "");
        return;
    }
    return;
}


/*
 * Class:     com_nokia_mj_impl_fileutils_FilePlatformSpecifics
 * Method:    getContentId
 * Signature: (Ljava/lang/String;Z)V
 */
JNIEXPORT jstring JNICALL  Java_com_nokia_mj_impl_fileutils_FilePlatformSpecifics__1getContentId(
    JNIEnv *aJni, jclass, jstring fileName)
{
    JStringUtils path(*aJni, fileName);
    TDesC* pathStr = &path;

    std::auto_ptr<ContentAccess::CContent> content(0);

    TRAPD(err, content.reset(ContentAccess::CContent::NewL(*pathStr)));

    jstring contentId = 0;

    if (KErrNone == err)
    {
        TBool drmProtected = EFalse;
        err = content->GetAttribute(
                  ContentAccess::EIsProtected, drmProtected);

        if (KErrNone == err && drmProtected)
        {
            std::auto_ptr<HBufC>contentIdBuf(
                HBufC::New(ContentAccess::KMaxCafContentName));
            if (contentIdBuf.get())
            {
                TPtr idPtr(contentIdBuf->Des());
                content->GetStringAttribute(ContentAccess::EContentID, idPtr);
                contentId = aJni->NewString(idPtr.Ptr(), idPtr.Length());
            }
        }
    }
    return contentId;
}


