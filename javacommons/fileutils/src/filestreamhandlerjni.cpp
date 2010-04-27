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
* Description:  JNI Layer for FileStreamHandler
 *
*/

#include <errno.h>

#include "logger.h"
#include "jniarrayutils.h"
#include "javajniutils.h"
#include "javacommonutils.h"

#include "fileextendedcommon.h"
#include "nativefileiohandler.h"
#include "com_nokia_mj_impl_fileutils_FileStreamHandler.h"

using namespace std;
using namespace java::util;
using namespace java::fileutils;
using namespace java::fileutility;

/*
 * Class:     com_nokia_mj_impl_fileutils_FileStreamHandler
 * Method:    _available
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL Java_com_nokia_mj_impl_fileutils_FileStreamHandler__1available
(JNIEnv *, jobject, jint aHandle)
{
    try
    {
        NativeFileIOHandler* handler =
            reinterpret_cast<NativeFileIOHandler*>(aHandle);
        return handler->available();
    }
    catch (...)
    {
        return 0;
    }
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FileStreamHandler
 * Method:    _skip
 * Signature: (IJ)J
 */
JNIEXPORT jlong JNICALL Java_com_nokia_mj_impl_fileutils_FileStreamHandler__1skip
(JNIEnv *, jobject, jint aHandle, jlong aOffset)
{
    try
    {
        NativeFileIOHandler* handler =
            reinterpret_cast<NativeFileIOHandler*>(aHandle);
        return handler->skip(aOffset);
    }
    catch (...)
    {
        return 0;
    }
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FileStreamHandler
 * Method:    _stopReading
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_fileutils_FileStreamHandler__1stopReading
(JNIEnv *, jobject, jint aHandle)
{
    NativeFileIOHandler* handler =
        reinterpret_cast<NativeFileIOHandler*>(aHandle);
    handler->stopReading();
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FileStreamHandler
 * Method:    _stopWriting
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_fileutils_FileStreamHandler__1stopWriting
(JNIEnv *, jobject, jint aHandle)
{
    NativeFileIOHandler* handler =
        reinterpret_cast<NativeFileIOHandler*>(aHandle);
    handler->stopWriting();
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FileStreamHandler
 * Method:    _createNativePeer
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_fileutils_FileStreamHandler__1createNativePeer
(JNIEnv *aJni, jobject, jstring aName)
{
    std::wstring name = FileUtil::jstringToWstring(aJni, aName);
    NativeFileIOHandler* handler = new NativeFileIOHandler(name);
    return reinterpret_cast<jint>(handler);
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FileStreamHandler
 * Method:    _readdata
 * Signature: ([BII)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_fileutils_FileStreamHandler__1readData
(JNIEnv *aJni, jobject, jint aHandle, jbyteArray aJavaBuffer, jint aOffset, jint aLength)
{
    char* buffer = new char[aLength + 1];
    try
    {
        NativeFileIOHandler* handler =
            reinterpret_cast<NativeFileIOHandler*>(aHandle);
        int bytesRead = handler->readBytes(buffer, aLength);

        if (bytesRead > 0)
        {
            JNIArrayUtils::CopyToJava(*aJni, buffer, bytesRead, aJavaBuffer,
                                      aOffset, bytesRead);
        }
        delete[] buffer;
        return bytesRead;
    }
    catch (int error)
    {
        WLOG1(EJavaFile, "FileStreamHandler: JNI: readData error : Error: %d",
              error);
        delete[] buffer;
        JniUtils::throwNewException(aJni, "java/io/IOException", "Read failed.");
        return 0;
    }
    catch (...)
    {
        ELOG(EJavaFile, "FileStreamHandler: JNI: readData: Unknown error.");
        delete[] buffer;
        JniUtils::throwNewException(aJni, "java/io/IOException", "Read failed.");
        return 0;
    }
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FileStreamHandler
 * Method:    _writeData
 * Signature: (I[BII)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_fileutils_FileStreamHandler__1writeData
(JNIEnv *aJni, jobject, jint aHandle, jbyteArray aJavaBuffer, jint aOffset, jint aLength)
{
    char* writeBuffer = new char[aLength + 1];
    try
    {
        NativeFileIOHandler* handler =
            reinterpret_cast<NativeFileIOHandler*>(aHandle);
        JNIArrayUtils::CopyToNative(*aJni, aJavaBuffer, aOffset, aLength,
                                    writeBuffer);
        handler->writeBytes(writeBuffer, aLength);
        delete[] writeBuffer;
    }
    catch (int error)
    {
        WLOG1(EJavaFile, "FileStreamHandler: JNI: writeData error : Error: %d",
              error);
        delete[] writeBuffer;
        JniUtils::throwNewException(aJni, "java/io/IOException",
                                    "Write failed.");
    }
    catch (...)
    {
        ELOG(EJavaFile, "FileStreamHandler: JNI: writeData: Unknown error.");
        delete[] writeBuffer;
        JniUtils::throwNewException(aJni, "java/io/IOException",
                                    "Write failed.");
    }
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FileStreamHandler
 * Method:    _closeFileStream
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_fileutils_FileStreamHandler__1closeFileStream
(JNIEnv *, jobject, jint aHandle)
{
    try
    {
        NativeFileIOHandler* handler =
            reinterpret_cast<NativeFileIOHandler*>(aHandle);
        handler->closeFileToReopen();
    }
    catch (...)
    {
        ELOG(EJavaFile, "FileUtility: JNI: closeFileStream threw exception.");
        //Nothing to do
    }
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FileStreamHandler
 * Method:    _openFileForReading
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_fileutils_FileStreamHandler__1openFileForReading
(JNIEnv *aJni, jobject, jint aHandle)
{
    try
    {
        NativeFileIOHandler* handler =
            reinterpret_cast<NativeFileIOHandler*>(aHandle);
        handler->openForReading();
    }
    catch (int error)
    {
        WLOG1(EJavaFile, "FileStreamHandler: JNI: openForReading Error: %d",
              error);
        if ((EACCES == errno) || (EPERM == errno))
        {
            JniUtils::throwNewException(aJni, "java/lang/SecurityException",
                                        "Permission denied");
            return;
        }
        else
        {
            JniUtils::throwNewException(aJni, "java/io/IOException",
                                        "Unable to open file for reading");
        }
    }
    catch (...)
    {
        ELOG(EJavaFile,
             "FileStreamHandler: JNI: openForReading: Unknown error caught");
        JniUtils::throwNewException(aJni, "java/io/IOException",
                                    "Unable to open file for reading");
    }
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FileStreamHandler
 * Method:    _openFileForWriting
 * Signature: (IJ)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_fileutils_FileStreamHandler__1openFileForWriting
(JNIEnv *aJni, jobject, jint aHandle, jlong aOffset)
{
    try
    {
        NativeFileIOHandler* handler =
            reinterpret_cast<NativeFileIOHandler*>(aHandle);
        handler->openForWriting(aOffset);
    }
    catch (int error)
    {
        WLOG1(EJavaFile, "FileStreamHandler: JNI: openForWriting Error: %d",
              error);
        if ((EACCES == errno) || (EPERM == errno))
        {
            JniUtils::throwNewException(aJni, "java/lang/SecurityException",
                                        "Permission denied");
            return;
        }
        else
        {
            JniUtils::throwNewException(aJni, "java/io/IOException",
                                        "Unable to open file for writing");
        }
    }
    catch (...)
    {
        ELOG(EJavaFile,
             "FileStreamHandler: JNI: openForWriting: Unknown error caught");
        JniUtils::throwNewException(aJni, "java/io/IOException",
                                    "Unable to open file for Writing");
    }
}

/*
 * Class:     com_nokia_mj_impl_fileutils_FileStreamHandler
 * Method:    _dispose
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_fileutils_FileStreamHandler__1dispose
(JNIEnv *, jobject, jint aHandle)
{
    try
    {
        NativeFileIOHandler* handler =
            reinterpret_cast<NativeFileIOHandler*>(aHandle);
        delete handler;
    }
    catch (...)
    {
        ELOG(EJavaFile,
             "FileUtility: JNI: dispose. Error deleting NativeFileUtils");
        //Nothing to do
    }
}
