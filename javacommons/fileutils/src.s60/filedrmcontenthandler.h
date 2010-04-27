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
* Description:  Handler for DRM File IO Operations
 *
*/


#ifndef FILEDRMCONTENTHANDLER_H
#define FILEDRMCONTENTHANDLER_H

#include <caf/caf.h>
#include <caf/content.h>
#include <caf/data.h>
#include <caf/rightsmanager.h>

#include "nativefileiohandler.h"

namespace java
{
namespace util
{
class FunctionServer;
}

namespace fileutils
{

OS_NONSHARABLE_CLASS(FileDRMContentHandler): public NativeFileIOHandler
{
public:
    /**
     * Checks to see if the specified file is DRM protected or not.
     */
    static TBool isDRMProtectedFile(const TDesC& aFullName);

    /**
     * Maps Java side intent to native intent type
     */
    static ContentAccess::TIntent mapIntentToCAF(TInt aIntent);

    /**
     * Sets the DRM arguments.
     */
    void setDrmArguments(TInt aIntent, TBool aExecuteIntent);

    FileDRMContentHandler(const std::wstring aName);

    virtual ~FileDRMContentHandler();

    // Methods fron NativeFileIOHandler

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
     */
    virtual void openForReading();

    /**
     * Opens the file specified during the construction of the object for write
     * operations.
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

    /**
     * Skips aOffset number of bytes.
     * Positive value of aOffset moves the position forward and negatve backwards.
     * To be called in Function Server context.
     * Return value is the actual value that can be skipped.
     */
    long skipFs(const long aOffset);

    /**
     * Reads data from the file into aBuffer upto aLength bytes.
     * Returns the amount of data read.
     * To be called in FunctionServer Context.
     * In case of errors, exception is thrown with errno value.
     */
    int readBytesFs(char* aBuffer, int aLength);

private:
    virtual void handleReopenCase();

    void initializeAndOpenL();

    void clearContent();
private:
    ContentAccess::CContent *mContent;
    ContentAccess::CData *mData;
    ContentAccess::TIntent mDrmIntent;

    java::util::FunctionServer* mFunctionServer;
    TInt mReadPosition;
    TBool mExecuteIntent;
};

}
}

#endif // FILEDRMCONTENTHANDLER_H
