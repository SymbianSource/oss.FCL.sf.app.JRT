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
* Description:  Handler File IO Operations
 *
*/

#include <errno.h>
#include <fcntl.h>
#include <locale.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>

#include "javacommonutils.h"
#include "jniarrayutils.h"
#include "logger.h"

#include "nativefileiohandler.h"

using namespace std;
using namespace java::util;
using namespace java::fileutils;

NativeFileIOHandler::NativeFileIOHandler(const std::wstring aName)
{
    // Set the current locale to suit unicode for entire process
    setlocale(LC_ALL, "");
    mFileName = aName;
    mFileDescriptor = 0;
    mClosedTemporarily = false;

    // Setting to -1 implies reading is not being done.
    // On call to openForReading, set this to 0 and increment on subsequent reads
    mReadPosition = -1;

    // Setting to -1 implies writing is not being done.
    // On call to openForWriting, set this to 0 and increment on subsequent writes
    mWritePosition = -1;
}

NativeFileIOHandler::~NativeFileIOHandler()
{
    // Set the current locale to suit unicode for entire process
    if (mFileDescriptor)
    {
        close(mFileDescriptor);
    }
}

long NativeFileIOHandler::skip(const long aOffset)
{
    JELOG2(EJavaFile);

    handleReopenCase();

    // lseek can set the offset beyond end of file.
    // but there will be no scenario where read position goes beyond end of file.

    // First set file offset to read position.
    int newOffset = lseek(mFileDescriptor, mReadPosition, SEEK_SET);

    if (newOffset < 0)
    {
        WLOG1(EJavaFile, "NativeFileIOHandler::skip: Seek failure. %d", errno);
        return 0;
    }
    else
    {
        // Now set the file offset in relation to the current read position.
        newOffset = lseek(mFileDescriptor, aOffset, SEEK_CUR);
        if (newOffset < 0)
        {
            WLOG1(EJavaFile, "NativeFileIOHandler::skip: Seek failure. %d",
                  errno);
            return 0;
        }
    }

    long retVal = newOffset - mReadPosition;
    mReadPosition = newOffset;
    return retVal;
}

int NativeFileIOHandler::readBytes(char* aBuffer, int aLength)
{
    JELOG2(EJavaFile);

    // Throws exception in case of errors.
    handleReopenCase();

    // lseek can set the offset beyond end of file.
    // but there will be no scenario where read position goes beyond end of file.
    int error = lseek(mFileDescriptor, mReadPosition, SEEK_SET);

    if (error < 0)
    {
        WLOG2(
            EJavaFile,
            "NativeFileIOHandler::readBytes:Seek failure. Read aborted:%d : Seek to: %d",
            errno, mReadPosition);
        // Errno cannot be thrown in S60. Causes random crashes.
        // So copy and throw.
        int err = errno;
        throw err;
    }

    int bytesRead = read(mFileDescriptor, aBuffer, aLength);
    if (bytesRead < 0)
    {
        LOG1(EJavaFile, EInfo,
             "NativeFileIOHandler::readBytes: Read failure: %d", errno);
        int err = errno;
        throw err;
    }

    // Adding 0 does no harm ;-)
    mReadPosition += bytesRead;
    return bytesRead;
}

int NativeFileIOHandler::writeBytes(char* aBuffer, int aLength)
{
    JELOG2(EJavaFile);

    // Throws exception in case of error
    handleReopenCase();

    // lseek can set the offset beyond end of file.
    // but there will be no scenario where read position goes beyond end of file.
    int error = lseek(mFileDescriptor, mWritePosition, SEEK_SET);
    if (error < 0)
    {
        WLOG1(
            EJavaFile,
            "NativeFileIOHandler::writeBytes: Seek failure. Write aborted: %d",
            errno);
        // Errno cannot be thrown in S60. Causes random crashes.
        // So copy and throw.
        int err = errno;
        throw err;
    }

    int bytesWritten = write(mFileDescriptor, aBuffer, aLength);
    if (bytesWritten <= 0)
    {
        LOG1(EJavaFile, EInfo,
             "NativeFileIOHandler::writeBytes: Write failure: %d", errno);
        int err = errno;
        throw err;
    }

    mWritePosition += bytesWritten;
    return bytesWritten;
}

void NativeFileIOHandler::stopReading()
{
    JELOG2(EJavaFile);
    mReadPosition = -1;
    closeStream();
}

void NativeFileIOHandler::stopWriting()
{
    JELOG2(EJavaFile);
    mWritePosition = -1;
    closeStream();
}

void NativeFileIOHandler::flush()
{
    JELOG2(EJavaFile);
    int ret = fsync(mFileDescriptor);
    if (ret < 0)
    {
        ELOG1(EJavaFile, "NativeFileIOHandler::flush() error %d", ret);
        int error = errno;
        throw error;
    }

}

void NativeFileIOHandler::closeStream()
{
    JELOG2(EJavaFile);
    if ((-1 == mReadPosition) && (-1 == mWritePosition))
    {
        close(mFileDescriptor);
        mFileDescriptor = 0;
    }
}

void NativeFileIOHandler::openForReading()
{
    JELOG2(EJavaFile);

    if (!mFileDescriptor)
    {
        // it will never come here if it is not a file.
        // so, if there is a trailing slash, remove it.
        char ch = mFileName.at(mFileName.length() - 1);
        if (ch == '\\' || ch == '/')
        {
            mFileName = mFileName.substr(0, mFileName.length() - 1);
        }

        char* utf8Name = JavaCommonUtils::wstringToUtf8(mFileName);
        if (access(utf8Name, R_OK) < 0)
        {
            // We do not have write access
            delete[] utf8Name;
            ELOG(EJavaFile,
                 "NativeFileUtils::OpenForReading: Exiting in between. no read permission");
            int error = errno;
            throw error;
        }
        mFileDescriptor = open(utf8Name, O_RDWR);

        // If open fails with Read Write mode, try read mode atleast.
        if (mFileDescriptor < 0)
        {
            mFileDescriptor = open(utf8Name, O_RDONLY);
        }

        delete[] utf8Name;
        if (mFileDescriptor < 0)
        {
            // There is some problem opening the file. Throw the cause for error.
            mFileDescriptor = 0;
            ELOG(
                EJavaFile,
                "NativeFileUtils::OpenForReading: Exiting in between unable to open in read only");
            int error = errno;
            throw error;
        }
    }

    mReadPosition = 0;
}

void NativeFileIOHandler::openForWriting(const long aOffset)
{
    JELOG2(EJavaFile);
    char* utf8Name = JavaCommonUtils::wstringToUtf8(mFileName);

    if (access(utf8Name, W_OK) < 0)
    {
        WLOG(EJavaFile,
             "NativeFileIOHandler::openForWriting: Access not allowed");
        // We do not have write access
        delete[] utf8Name;
        int error = errno;
        throw error;
    }

    if (!mFileDescriptor)
    {
        // it will never come here if it is not a file.
        // so, if there is a trailing slash, remove it.
        char ch = mFileName.at(mFileName.length() - 1);
        if (ch == '\\' || ch == '/')
        {
            mFileName = mFileName.substr(0, mFileName.length() - 1);
        }

        mFileDescriptor = open(utf8Name, O_RDWR);

        if (mFileDescriptor < 0)
        {
            delete[] utf8Name;
            mFileDescriptor = 0;
            int error = errno;
            throw error;
        }
    }

    mWritePosition = 0;
    if (aOffset > 0)
    {
        // We will need to stat the file and find out its size just to make sure
        // lseek does not set the offset to beyond end of file.
        struct stat fileStat;
        int error = lstat(utf8Name, &fileStat);

        if (error < 0)
        {
            int error = errno;
            throw error;
        }
        long fileSize = fileStat.st_size;

        if (aOffset >= fileSize)
        {
            // Move to end of file
            if (lseek(mFileDescriptor, 0, SEEK_END) < 0)
            {
                delete[] utf8Name;
                int error = errno;
                throw error;
            }
            mWritePosition = fileSize;
        }
        else
        {
            // Move to given offset
            if (lseek(mFileDescriptor, aOffset, SEEK_SET) < 0)
            {
                delete[] utf8Name;
                int error = errno;
                throw error;
            }
            mWritePosition = aOffset;
        }
    }
    delete[] utf8Name;
}

void NativeFileIOHandler::closeFileToReopen()
{
    JELOG2(EJavaFile);

    // We dont try to synchronize here. It must be done from java.
    // In case of multithreaded apps, set hidden should not be done while read
    // is in progress.
    if (mFileDescriptor)
    {
        LOG(EJavaFile, EInfo,
            "NativeFileUtils::closeFileToReopen: Closing already open Files");
        LOG1(EJavaFile, EInfo, "        Current Read Offset: %ld",
             mReadPosition);
        LOG1(EJavaFile, EInfo, "        Current Write Offset: %ld",
             mWritePosition);
        mClosedTemporarily = true;
        close(mFileDescriptor);
        mFileDescriptor = 0;
    }
}

void NativeFileIOHandler::handleReopenCase()
{
    JELOG2(EJavaFile);
    if (!mClosedTemporarily)
    {
        return;
    }

    if ((-1 == mWritePosition) && (-1 == mReadPosition))
    {
        return;
    }

    if (mWritePosition > -1)
    {
        LOG(EJavaFile, EInfo,
            "NativeFileUtils::handleReopenCase: Reopening for write");
        long curWritePos = mWritePosition;
        openForWriting(curWritePos);
    }
    else if (mReadPosition > -1)
    {
        LOG(EJavaFile, EInfo,
            "NativeFileUtils::handleReopenCase: Reopening for read");
        long curReadPos = mReadPosition;
        openForReading();
        mReadPosition = curReadPos;
    }

    LOG(EJavaFile, EInfo, "NativeFileUtils::handleReopenCase: File Open");
    LOG1(EJavaFile, EInfo, "        Setting Read Offset: %ld", mReadPosition);
    LOG1(EJavaFile, EInfo, "        Setting Write Offset: %ld", mWritePosition);

    mClosedTemporarily = false;
}

long NativeFileIOHandler::available()
{
    JELOG2(EJavaFile);
    char* utf8Name = JavaCommonUtils::wstringToUtf8(mFileName);

    struct stat fileStat;
    int error = lstat(utf8Name, &fileStat);

    delete[] utf8Name;

    if (error < 0)
    {
        int error = errno;
        throw error;
    }
    long fileSize = fileStat.st_size;

    if (mReadPosition > 0)
    {
        return (fileSize - mReadPosition);
    }
    else
    {
        return fileSize;
    }
}
