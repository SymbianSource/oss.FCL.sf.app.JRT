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
* Description:  Handles DRM File IO Operations
 *
*/

#include <errno.h>
#include <unistd.h>
#include <string>

#include "s60commonutils.h"
#include "javacommonutils.h"
#include "jniarrayutils.h"
#include "logger.h"
#include "functionserver.h"
#include "fs_methodcall.h"

#include "filedrmcontenthandler.h"
#include "s60filesystemutilities.h"
#include "com_nokia_mj_impl_fileutils_FileDRMContentHandler.h"

using namespace std;
using namespace java::util;
using namespace java::fileutils;

TBool FileDRMContentHandler::isDRMProtectedFile(const TDesC& aFullName)
{
    JELOG2(EJavaFile);
    LOG(EJavaFile, EInfo, "+FileDRMContentHandler::isDRMProtectedFile{}");
    TInt value = 0;
    HBufC* fileName = 0;

    TRAPD(err, fileName = HBufC::NewL(aFullName.Length()));
    if (KErrNone == err)
    {
        TPtr fileNamePtr(fileName->Des());
        fileNamePtr = aFullName;
        S60FileSystemUtilities::ReplaceCharacters(fileNamePtr, '/', '\\');

        ContentAccess::CContent* content = 0;
        // Use CAF to check whether the content is protected
        TRAP(err, content = ContentAccess::CContent::NewL(fileNamePtr));

        if (KErrNone == err)
        {
            LOG(EJavaFile, EInfo,
                "    FileDRMContentHandler::isDRMProtectedFile: " \
                "Getting attribute.");
            err = content->GetAttribute(ContentAccess::EIsProtected, value);
            if (err != KErrNone)
            {
                return EFalse;
            }
            delete content; //Cleanup after NewL
        }
    }
    LOG1(EJavaFile, EInfo,
         "-FileDRMContentHandler::isDRMProtectedFile: returns %d",
         value);
    delete fileName;
    return value ? ETrue : EFalse;
}

ContentAccess::TIntent FileDRMContentHandler::mapIntentToCAF(TInt aIntent)
{
    JELOG2(EJavaFile);
    ContentAccess::TIntent intent;
    switch (aIntent)
    {
    case com_nokia_mj_impl_fileutils_FileDRMContentHandler_Peek:
        intent = ContentAccess::EPeek;
        break;
    case com_nokia_mj_impl_fileutils_FileDRMContentHandler_Play:
        intent = ContentAccess::EPlay;
        break;
    case com_nokia_mj_impl_fileutils_FileDRMContentHandler_View:
        intent = ContentAccess::EView;
        break;
    case com_nokia_mj_impl_fileutils_FileDRMContentHandler_Execute:
        intent = ContentAccess::EExecute;
        break;
    case com_nokia_mj_impl_fileutils_FileDRMContentHandler_Print:
        intent = ContentAccess::EPrint;
        break;
    default:
        intent = ContentAccess::EUnknown;
    }
    return intent;

}

void FileDRMContentHandler::setDrmArguments(TInt aIntent, TBool aExecuteIntent)
{
    LOG2(EJavaFile, EInfo, "FileDRMContentHandler::setDrmArguments(): "
         " Intent to %d and Set Intent?", aIntent, aExecuteIntent);

    mExecuteIntent = aExecuteIntent;
    mDrmIntent = mapIntentToCAF(aIntent);
}

FileDRMContentHandler::FileDRMContentHandler(const std::wstring aName) :
        NativeFileIOHandler(aName), mContent(0), mData(0), mReadPosition(0)
{
    // CContent and CData both use RFile internally.
    // Calling in different thread context will result in a panic.
    // Hence we need to use Function Server to maintain a single thread context.
    LOG(EJavaFile, EInfo, "FileDRMContentHandler: Creating new FunctionServer");
    mFunctionServer = new FunctionServer("FileDRMContentHandler");
    mFunctionServer->createServerToNewThread();
}

FileDRMContentHandler::~FileDRMContentHandler()
{
    JELOG2(EJavaFile);
    CallMethod(this, &FileDRMContentHandler::clearContent, mFunctionServer);
    delete mFunctionServer;
}

void FileDRMContentHandler::clearContent()
{
    if (mContent)
    {
        delete mContent;
        mContent = 0;
    }

    if (mData)
    {
        delete mData;
        mData = 0;
    }
}

long FileDRMContentHandler::skip(const long aOffset)
{
    long result = 0;
    CallMethod(result, this, &FileDRMContentHandler::skipFs, aOffset,
               mFunctionServer);
    return result;
}

long FileDRMContentHandler::skipFs(const long aOffset)
{
    JELOG2(EJavaFile);
    handleReopenCase();

    TInt pos = aOffset;
    TInt currentOffset = mReadPosition;

    LOG1(EJavaFile, EInfo, "FileDRMContentHandler::skip: trying to skip: %d",
         aOffset);

    TInt error = mData->Seek(ESeekCurrent, pos);
    TInt skipped = 0;

    if (KErrNone == error)
    {
        mReadPosition = pos;
        LOG1(EJavaFile, EInfo,
             "FileDRMContentHandler::skip: setting read pos to: %d",
             mReadPosition);
        skipped = (mReadPosition - currentOffset);
    }

    LOG1(EJavaFile, EInfo, "FileDRMContentHandler::skip: skipped: %d", skipped);
    return skipped;
}

int FileDRMContentHandler::readBytes(char* aBuffer, int aLength)
{
    int result = 0;
    CallMethod(result, this, &FileDRMContentHandler::readBytesFs, aBuffer,
               aLength, mFunctionServer);
    return result;
}

int FileDRMContentHandler::readBytesFs(char* aBuffer, int aLength)
{
    JELOG2(EJavaFile);
    handleReopenCase();

    HBufC8* data = HBufC8::New(aLength + 1);
    TPtr8 buffer(data->Des());
    LOG(EJavaFile, EInfo, "FileDRMContentHandler::readBytes() Reading data...");

    TInt error = mData->Read(buffer, aLength);
    TInt bytesRead = buffer.Size();

    memcpy(aBuffer, buffer.Ptr(), bytesRead);
    aBuffer[bytesRead] = 0;

    mReadPosition += bytesRead;
    delete data;

    LOG1(EJavaFile, EInfo, "FileDRMContentHandler::readBytes: returning: %d",
         bytesRead);
    return bytesRead;
}

int FileDRMContentHandler::writeBytes(char* /*aBuffer*/, int /*aLength*/)
{
    JELOG2(EJavaFile);
    // There is no write for DRM protected content. We should not be here.
    return 0;
}

void FileDRMContentHandler::stopReading()
{
    JELOG2(EJavaFile);
    mReadPosition = 0;
}

void FileDRMContentHandler::stopWriting()
{
    JELOG2(EJavaFile);
    ELOG(EJavaFile, "FileDRMContentHandler::stopWriting() "
         " WE SHOULD NEVER BE HERE. SOMETHING WRONG.");
    // NOTE: There is no write for DRM protected content. We should not be here.
}

void FileDRMContentHandler::initializeAndOpenL()
{
    JELOG2(EJavaFile);

    LOG(EJavaFile, EInfo, "+FileDRMContentHandler::initializeAndOpenL() ");

    // Check and initialize DRM content here.
    HBufC* fileName = S60CommonUtils::wstringToDes(mFileName.c_str());
    TPtr fileNameDes(fileName->Des());
    S60FileSystemUtilities::ReplaceCharacters(fileNameDes, '/', '\\');

    CleanupStack::PushL(fileName);

    mContent = ContentAccess::CContent::NewL(fileNameDes);
    mData = mContent->OpenContentL(mDrmIntent);
    LOG(EJavaFile, EInfo,
        "    FileDRMContentHandler::initializeAndOpenL(): Content opened.");

    int error = mData->EvaluateIntent(mDrmIntent);
    User::LeaveIfError(error);

    LOG(EJavaFile, EInfo,
        "    FileDRMContentHandler::initializeAndOpenL(): Intent evaluated");

    if (mExecuteIntent)
    {
        LOG(EJavaFile, EInfo,
            "    FileDRMContentHandler::initializeAndOpenL(): Executing intent");

        // Initialize and open should be called only once until the file is not closed.
        error = mData->ExecuteIntent(mDrmIntent);
        User::LeaveIfError(error);
    }

    CleanupStack::PopAndDestroy(fileName);
    LOG(EJavaFile, EInfo, "-FileDRMContentHandler::initializeAndOpenL() ");
}

void FileDRMContentHandler::openForReading()
{
    JELOG2(EJavaFile);
    LOG1(EJavaFile, EInfo, "FileDRMContentHandler::openForReading(): "
         " Opening: %S", mFileName.c_str());

    TRAPD(error, CallMethodL(this,
                             &java::fileutils::FileDRMContentHandler::initializeAndOpenL,
                             mFunctionServer));

    if ((KErrCANoPermission == error) || (KErrCANoRights == error)
            || (KErrCAPendingRights == error))
    {
        ELOG1(EJavaFile, "FileDRMContentHandler::openForReading() Open failed."
              " Error: %d", error);
        throw EPERM;
    }

    if (KErrNone != error)
    {
        ELOG1(EJavaFile, "FileDRMContentHandler::openForReading() Open failed."
              " Error: %d", error);
        throw EIO;
    }

    mReadPosition = 0;
}

void FileDRMContentHandler::openForWriting(const long /*aOffset*/)
{
    JELOG2(EJavaFile);
    // NOTE: There is no write for DRM protected content. We should not be here.
    ELOG(EJavaFile, "FileDRMContentHandler::openForWriting(): "
         " Trying to write into DRM file. Throwing exception EPERM.");
    throw EPERM;
}

void FileDRMContentHandler::closeFileToReopen()
{
    JELOG2(EJavaFile);
    // We never close file to re-open again.
}

void FileDRMContentHandler::handleReopenCase()
{
    JELOG2(EJavaFile);
    // File is never closed temporarily in case of DRM files.
    // Kept for future use.
}

long FileDRMContentHandler::available()
{
    JELOG2(EJavaFile);
    TInt fileLength = 0;
    TRAPD(error, CallMethodL(mData, &ContentAccess::CData::DataSizeL,
                             fileLength, mFunctionServer));

    TInt total = 0;
    if (KErrNone == error)
    {
        total = fileLength - mReadPosition;
    }

    LOG1(EJavaFile, EInfo, "FileDRMContentHandler::available: Returning: %d",
         total);
    return total;
}

