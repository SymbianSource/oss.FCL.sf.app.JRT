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
* Description:  JNI Layer for FileUtility
 *
*/


#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/stat.h>

#include "logger.h"
#include "javajniutils.h"
#include "javacommonutils.h"

#include "fileextendedcommon.h" // To be removed once common utility is fixed
#include "fileutilities.h"
#include "com_nokia_mj_impl_fileutils_FileUtility.h"

using namespace std;
using namespace java::util;
using namespace java::fileutils;
using namespace java::fileutility;

/*
 * Class:     com_nokia_mj_impl_fileutils_FileUtility
 * Method:    _exists
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_mj_impl_fileutils_FileUtility__1exists(
    JNIEnv *aJni, jclass, jstring fileName)
{
    try
    {
        std::wstring name = FileUtil::jstringToWstring(aJni, fileName);
        return FileUtilities::exists(name);
    }
    catch (...)
    {
        ELOG(EJavaFile,
             "FileUtility: JNI: exists caught exception. Return false");
        return false;
    }
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FileUtility
 * Method:    _getRealPath
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_fileutils_FileUtility__1getRealPath(
    JNIEnv *aJni, jclass, jstring aPath)
{
    std::wstring wName = L"";
    try
    {
        std::wstring path = FileUtil::jstringToWstring(aJni, aPath);
        char* utf8Name = JavaCommonUtils::wstringToUtf8(path);
        char resolvepath[PATH_MAX];
        char *rpath = NULL;

        rpath = realpath(utf8Name, resolvepath);
        delete[] utf8Name;
        utf8Name = NULL;

        if (rpath == NULL)
        {
            if ((errno!=ENOENT) && (errno != EACCES))
            {
                ELOG1(EJavaFile,"FileUtility: JNI: getRealPath: Unable to resolve. Throw IOException %d", errno);
                JniUtils::throwNewException(aJni, "java/io/IOException", "");
                return NULL;
            }
            else
            {
                return aPath;
            }
        }
        else
        {
            wName = JavaCommonUtils::utf8ToWstring(rpath);
        }
    }
    catch (...)
    {
        ELOG(EJavaFile, "FileUtility: JNI: getRealPath: Exception occurred. Returning empty");
    }

    jstring retValue = JniUtils::wstringToJstring(aJni, wName);
    return retValue;
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FileUtility
 * Method:    _availableSize
 * Signature: (Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_nokia_mj_impl_fileutils_FileUtility__1availableSize(
    JNIEnv *aJni, jobject, jstring fileName)
{
    try
    {
        std::wstring name = FileUtil::jstringToWstring(aJni, fileName);
        return FileUtilities::availableSize(name);
    }
    catch (...)
    {
        ELOG(EJavaFile,
             "FileUtility: JNI: availableSize caught exception. Return -1");
        return -1;
    }
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FileUtility
 * Method:    _canRead
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_mj_impl_fileutils_FileUtility__1canRead(
    JNIEnv *aJni, jobject, jstring fileName)
{
    try
    {
        std::wstring name = FileUtil::jstringToWstring(aJni, fileName);
        return FileUtilities::canRead(name);
    }
    catch (...)
    {
        ELOG(EJavaFile,
             "FileUtility: JNI: canRead caught exception. Return false");
        return false;
    }
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FileUtility
 * Method:    _canWrite
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_mj_impl_fileutils_FileUtility__1canWrite(
    JNIEnv *aJni, jobject, jstring fileName)
{
    try
    {
        std::wstring name = FileUtil::jstringToWstring(aJni, fileName);
        return FileUtilities::canWrite(name);
    }
    catch (...)
    {
        ELOG(EJavaFile,
             "FileUtility: JNI: canWrite caught exception. Return false");
        return false;
    }
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FileUtility
 * Method:    _createNewDirectory
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_mj_impl_fileutils_FileUtility__1createNewDirectory(
    JNIEnv *aJni, jobject, jstring fileName)
{
    JELOG2(EJavaFile);
    try
    {
        std::wstring name = FileUtil::jstringToWstring(aJni, fileName);
        char* utf8Name = JavaCommonUtils::wstringToUtf8(name);

        int error = mkdir(utf8Name, 0777);
        delete[] utf8Name;

        if (error < 0)
        {
            ELOG2(
                EJavaFile,
                "FileUtility: JNI: createNewDirectory: Error: %d: Name: %S",
                errno, name.c_str());
            if ((EEXIST == errno))
            {
                // It already exists
                return false;
            }

            if ((EACCES == errno) || (EPERM == errno))
            {
                ELOG(EJavaFile,
                     "JNI: CreateNewDirectory: throwing sec exception");
                JniUtils::throwNewException(aJni,
                                            "java/lang/SecurityException", "Permission denied");
            }
            else
            {
                ELOG(EJavaFile, "JNI: CreateNewDirectory: ioexception");
                JniUtils::throwNewException(aJni, "java/io/IOException", "Unable to create directory");
            }
        }
        return true;
    }
    catch (...)
    {
        ELOG(EJavaFile,
             "JNI: CreateNewDirectory: Exception occured returning false");
        return false;
    }
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FileUtility
 * Method:    _createNewFile
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_mj_impl_fileutils_FileUtility__1createNewFile(
    JNIEnv *aJni, jobject, jstring fileName)
{
    try
    {
        JELOG2(EJavaFile);
        std::wstring name = FileUtil::jstringToWstring(aJni, fileName);
        char* utf8Name = JavaCommonUtils::wstringToUtf8(name);

        //jboolean iscopy;
        //const char* utf8Name = (aJni->GetStringUTFChars(fileName, &iscopy));

        int fileDes = open(utf8Name, O_CREAT | O_EXCL, 0666);
        delete[] utf8Name;

        if (fileDes < 0)
        {
            WLOG1(EJavaFile, "FileUtility: JNI: createNewFile: Error: %d", errno);
            if ((EEXIST == errno))
            {
                // It already exists
                return false;
            }

            if ((EACCES == errno) || (EPERM == errno))
            {
                JniUtils::throwNewException(aJni,
                                            "java/lang/SecurityException", "Permission denied");
            }
            else
            {
                // Add also string from the posix utility that gives cause as an error string.
                JniUtils::throwNewException(aJni, "java/io/IOException", "Unable to create file");
            }
        }
        else
        {
            close(fileDes);
        }

        return true;
    }
    catch (...)
    {
        ELOG(EJavaFile,
             "JNI: createNewFile: unknown exception. returning false");
        return false;
    }
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FileUtility
 * Method:    _delete
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_mj_impl_fileutils_FileUtility__1delete(
    JNIEnv *aJni, jobject, jstring fileName)
{
    JELOG2(EJavaFile);
    try
    {
        const char* tempName = aJni->GetStringUTFChars(fileName, 0);
        string name(tempName);
        aJni->ReleaseStringUTFChars(fileName, tempName);

        int error = remove(name.c_str());

        if (error < 0)
        {
            WLOG2(EJavaFile, "FileUtility: JNI: delete: Error: %d: Name:%S",
                  errno, name.c_str());

            if ((EACCES == errno) || (EPERM == errno))
            {
                JniUtils::throwNewException(aJni,
                                            "java/lang/SecurityException", "Permission denied");
            }
            else
            {
                return false;
            }
        }
        return true;
    }
    catch (...)
    {
        ELOG(EJavaFile,
             "FileUtility: JNI: delete: unknown exception occured. Returning false");
        return false;
    }
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FileUtility
 * Method:    _dirSize
 * Signature: (Ljava/lang/String;Z)J
 */
JNIEXPORT jlong JNICALL Java_com_nokia_mj_impl_fileutils_FileUtility__1dirSize(
    JNIEnv *aJni, jobject, jstring fileName, jboolean subDirs)
{
    try
    {
        std::wstring name = FileUtil::jstringToWstring(aJni, fileName);
        int retVal = FileUtilities::getDirSize(name, subDirs);
        return (retVal < 0) ? 0 : retVal;
    }
    catch (...)
    {
        ELOG(EJavaFile, "FileUtility: JNI: dirSize threw exception. Return 0");
        return 0;
    }
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FileUtility
 * Method:    _fileSize
 * Signature: (Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_nokia_mj_impl_fileutils_FileUtility__1fileSize(
    JNIEnv *aJni, jobject, jstring fileName)
{
    std::wstring name = FileUtil::jstringToWstring(aJni, fileName);
    int ret = 0;
    try
    {
        ret = FileUtilities::fileSize(name);
    }
    catch (int err)
    {
        ELOG2(EJavaFile, "FileUtility: JNI: fileSize: Error: %d: Name: %S",
              err, name.c_str());
        if ((EACCES == err) || (EPERM == err))
        {
            JniUtils::throwNewException(aJni, "java/lang/SecurityException",
                                        "Permission denied");
        }
        else
        {
            JniUtils::throwNewException(aJni, "java/io/IOException", "Unable to determine file size");
        }

    }
    catch (...)
    {
        ELOG1(EJavaFile,
              "FileUtility: JNI: fileSize: Unknown error caught: Name: %S",
              name.c_str());
        JniUtils::throwNewException(aJni, "java/io/IOException", "");
    }
    return ret;
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FileUtility
 * Method:    _isDirectory
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_mj_impl_fileutils_FileUtility__1isDirectory(
    JNIEnv *aJni, jobject, jstring fileName)
{
    try
    {
        std::wstring name = FileUtil::jstringToWstring(aJni, fileName);
        return FileUtilities::isDirectory(name);
    }
    catch (...)
    {
        ELOG(EJavaFile,
             "FileUtility: JNI: isDirectory: Unknown error caught: return false");
        // Unknown error
        return false;
    }
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FileUtility
 * Method:    _isFile
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_mj_impl_fileutils_FileUtility__1isFile(
    JNIEnv *aJni, jobject, jstring fileName)
{
    try
    {
        std::wstring name = FileUtil::jstringToWstring(aJni, fileName);
        return FileUtilities::isFile(name);
    }
    catch (...)
    {
        ELOG(EJavaFile,
             "FileUtility: JNI: isFile: Unknown error caught: return false");
        // Unknown error
        return false;
    }
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FileUtility
 * Method:    _isHidden
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_mj_impl_fileutils_FileUtility__1isHidden(
    JNIEnv *aJni, jobject, jstring fileName)
{
    try
    {
        std::wstring name = FileUtil::jstringToWstring(aJni, fileName);
        return FileUtilities::isHidden(name);
    }
    catch (...)
    {
        ELOG(EJavaFile,
             "FileUtility: JNI: isHidden: Unknown error caught: return false");
        // Unknown error
        return false;
    }
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FileUtility
 * Method:    _lastModified
 * Signature: (Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_nokia_mj_impl_fileutils_FileUtility__1lastModified(
    JNIEnv *aJni, jobject, jstring fileName)
{
    try
    {
        std::wstring name = FileUtil::jstringToWstring(aJni, fileName);
        return FileUtilities::lastModified(name);
    }
    catch (...)
    {
        ELOG(EJavaFile,
             "FileUtility: JNI: lastModified: Unknown error caught: return 0");
        // Unknown error
        return 0;
    }
}


/*
 * Class:     com_nokia_mj_impl_fileutils_FileUtility
 * Method:    _listDirContents
 * Signature: (Ljava/lang/String;Z)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_fileutils_FileUtility__1listDirContents
(JNIEnv *aJni, jobject, jstring fileName, jboolean includeHidden)
{
    try
    {
        std::wstring name = FileUtil::jstringToWstring(aJni, fileName);
        std::wstring result = FileUtilities::getDirContents(name, includeHidden);
        jstring retValue = JniUtils::wstringToJstring(aJni, result);
        return retValue;
    }
    catch (...)
    {
        ELOG(EJavaFile,
             "FileUtility: JNI: listDirContents: Unknown error caught: return null");
        // Unknown error
        return NULL;
    }
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FileUtility
 * Method:    _renameFile
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_mj_impl_fileutils_FileUtility__1renameFile(
    JNIEnv *aJni, jobject, jstring aOldName, jstring aNewName)
{
    try
    {
        const char* oldName = aJni->GetStringUTFChars(aOldName, 0);
        const char* newName = aJni->GetStringUTFChars(aNewName, 0);

        int error = rename(oldName, newName);

        aJni->ReleaseStringUTFChars(aOldName, oldName);
        aJni->ReleaseStringUTFChars(aNewName, newName);

        if (error < 0)
        {
            WLOG1(EJavaFile, "FileUtility: JNI: renameFile Error: %d", error);
            if ((EACCES == errno) || (EPERM == errno))
            {
                JniUtils::throwNewException(aJni,
                                            "java/lang/SecurityException", "Permission denied");
            }
            else
            {
                JniUtils::throwNewException(aJni, "java/io/IOException", "Unable to rename file or directory");
            }
        }
        return true;
    }
    catch (...)
    {
        ELOG(EJavaFile,
             "FileUtility: JNI: renameFile: UNknown exception occured. Returning false ");
        return false;
    }
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FileUtility
 * Method:    _setReadable
 * Signature: (Ljava/lang/String;Z)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_fileutils_FileUtility__1setReadable(
    JNIEnv *aJni, jobject, jstring fileName, jboolean readable)
{
    try
    {
        std::wstring name = FileUtil::jstringToWstring(aJni, fileName);
        FileUtilities::setReadable(name, readable);
    }
    catch (...)
    {
        JniUtils::throwNewException(aJni, "java/io/IOException", "");
        ELOG(EJavaFile, "FileUtility: JNI: setReadable: Unknown error caught");
        //nothing to do
    }
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FileUtility
 * Method:    _setWritable
 * Signature: (Ljava/lang/String;Z)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_fileutils_FileUtility__1setWritable(
    JNIEnv *aJni, jobject, jstring fileName, jboolean writable)
{
    try
    {
        std::wstring name = FileUtil::jstringToWstring(aJni, fileName);
        FileUtilities::setWritable(name, writable);
    }
    catch (...)
    {
        JniUtils::throwNewException(aJni, "java/io/IOException", "");
        ELOG(EJavaFile, "FileUtility: JNI: setWritable: Unknown error caught");
        //nothing to do
    }
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FileUtility
 * Method:    _totalSize
 * Signature: (Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_nokia_mj_impl_fileutils_FileUtility__1totalSize(
    JNIEnv *aJni, jobject, jstring fileName)
{
    try
    {
        std::wstring name = FileUtil::jstringToWstring(aJni, fileName);
        return FileUtilities::totalSize(name);
    }
    catch (...)
    {
        ELOG(EJavaFile,
             "FileUtility: JNI: totalSize: Unknown error caught: Return 0");
        return 0;
    }
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FileUtility
 * Method:    _truncate
 * Signature: (Ljava/lang/String;J)J
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_fileutils_FileUtility__1truncate(
    JNIEnv *aJni, jobject, jstring file, jlong offset)
{
    try
    {
        jboolean iscopy;
        const char* utf8Name = (aJni->GetStringUTFChars(file, &iscopy));
        int error = truncate(utf8Name, offset);

        if (error < 0)
        {
            WLOG2(EJavaFile, "FileUtility: JNI: truncate: error: %d Name: %s",
                  errno, utf8Name);
            JniUtils::throwNewException(aJni, "java/io/IOException",
                                        "");
        }

        aJni->ReleaseStringUTFChars(file, utf8Name);
    }
    catch (...)
    {
        ELOG(EJavaFile, "FileUtility: JNI: truncate: Unknown error caught");
        // Nothing to do
    }
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FileUtility
 * Method:    _usedSize
 * Signature: (Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_nokia_mj_impl_fileutils_FileUtility__1usedSize(
    JNIEnv *aJni, jobject, jstring fileName)
{
    try
    {
        std::wstring name = FileUtil::jstringToWstring(aJni, fileName);
        return FileUtilities::usedSize(name);
    }
    catch (...)
    {
        ELOG(EJavaFile,
             "FileUtility: JNI: usedSize: Unknown error caught: return 0");
        return 0;
    }
}
