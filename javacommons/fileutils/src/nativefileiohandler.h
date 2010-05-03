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
* Description:  Handler for File IO Operations
 *
*/


#ifndef NATIVEFILEIOHANDLER_H
#define NATIVEFILEIOHANDLER_H

#include <string>

namespace java
{
namespace fileutils
{

/**
 * Implements methods to read/write data from the native file system.
 */
OS_NONSHARABLE_CLASS(NativeFileIOHandler)
{
public:
    NativeFileIOHandler(const std::wstring aName);
    virtual ~NativeFileIOHandler();

    /**
     * Reads data from the file into aBuffer upto aLength bytes.
     * Returns the amount of data read.
     * In case of errors, exception is thrown with errno value.
     */
    virtual int readBytes(char* aBuffer, int aLength);

    /**
     * Writes data from aBuffer into the file upto aLength bytes.
     * Returns the amount of data written.
     * In case of errors, exception is thrown with errno value.
     */
    virtual int writeBytes(char* aBuffer, int aLength);

    /**
     * Stops read operation. In case the file is opened in read-write mode and
     * if the write operations are in progress, the file is kept open to allow
     * for write operations. Else the file is closed.
     */
    virtual void stopReading();

    /**
     * Stops write operation. In case the file is opened in read-write mode and
     * if the read operations are in progress, the file is kept open to allow
     * for read operations. Else the file is closed.
     */
    virtual void stopWriting();

    /**
     * Opens the file specified during the construction of the object for read
     * operations.
     * @throws errno value in case open failed.
     */
    virtual void openForReading();

    /**
     * Opens the file specified during the construction of the object for write
     * operations.
     * @throws errno value in case open failed.
     */
    virtual void openForWriting(const long aOffset);

    /**
     * Skips aOffset number of bytes.
     * Positive value of aOffset moves the position forward and negatve backwards.
     * Return value is the actual value that can be skipped.
     */
    virtual long skip(const long aOffset);

    /**
     * Closes the file descriptors but will not delete all data.
     * On next call to read or write, we continue from previous offset.
     */
    virtual void closeFileToReopen();

    /**
     * Returns the amount of data available in the file from current read offset.
     */
    virtual long available();

protected:
    virtual void handleReopenCase();

private:
    void closeStream();

    NativeFileIOHandler();

protected:
    // Name of the file to which the stream is opened
    std::wstring mFileName;

    // Will be set in case the descriptors were closed temporarily. On next call
    // to read or write, we re-open the file and continue with previous offset.
    bool mClosedTemporarily;
private:
    // Stream to do read and write
    int mFileDescriptor;

    // Holds the mode in which the file has to be opened
    int mFileAccessMode;

    // Holds the current read position
    long mReadPosition;

    // Holds the current write position
    long mWritePosition;

};

} // end namespace fileutils
} // end namespace java

#endif // NATIVEFILEIOHANDLER_H
