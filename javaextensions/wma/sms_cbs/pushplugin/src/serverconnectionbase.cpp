/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
 *
*/


#include "logger.h"
#include "fileutilities.h"
#include "exceptionbase.h"
#include "javacommonutils.h"
#include "serverconnectionbase.h"

using namespace java::util;
using namespace java::fileutils;

namespace java
{
namespace wma
{

const TInt KFirstMessage = 10000001;

ServerConnectionBase::ServerConnectionBase(const std::wstring& aUri,
        const std::wstring& aFilter)
        :mIsAppLaunched(false), mIsListening(false),mPort(0), mUri(aUri),
        mFilter(aFilter), mMessagesOnStore(0), mNextMessageInStore(KFirstMessage),
        mFirstMessageInStore(-1)
{
    JELOG2(EWMA);
}

void ServerConnectionBase::deleteMessage()
{
    JELOG2(EWMA);
    std::wstring path(mMessageStoreDirName);
    path += JavaCommonUtils::intToWstring(mFirstMessageInStore);
    char* messagePath = JavaCommonUtils::wstringToUtf8(path);
    //Delete message from the store
    if ((remove(messagePath)) != 0)
    {
        // If deleting of message fails , increment the first message
        // in store so next read retrieves the next message.
        ELOG1(EWMA, "WMA : Removing File  Failed : %s", messagePath);
    }
    mFirstMessageInStore++;
    delete[] messagePath;
}

OS_EXPORT std::wstring ServerConnectionBase::getUri() const
{
    JELOG2(EWMA);
    return mUri;
}

OS_EXPORT void ServerConnectionBase::setFilter(const std::wstring& aFilter)
{
    JELOG2(EWMA);
    mFilter = aFilter;
}

OS_EXPORT std::wstring ServerConnectionBase::getFilter() const
{
    JELOG2(EWMA);
    return mFilter;
}

OS_EXPORT int ServerConnectionBase::getMessagesOnStore() const
{
    JELOG2(EWMA);
    return mMessagesOnStore;
}

int ServerConnectionBase::createMessageStore(std::wstring aDirectoryPath, bool aDeleteStore)
{
    JELOG2(EWMA);
    aDirectoryPath += JavaCommonUtils::intToWstring(mPort);
    // If not push connection delete the message store.
    if (aDeleteStore)
       removeDir(aDirectoryPath);
    mMessageStoreDirName = aDirectoryPath + L"\\";
    LOG1(EWMA, EInfo , " Creating message store in path %S",
         mMessageStoreDirName.c_str());
    // See if directory is present
    if (FileUtilities::isDirectory(aDirectoryPath))
    {
        std::list<std::wstring> dirList = FileUtilities::getDirContentsList
                                          (aDirectoryPath);
        mMessagesOnStore = dirList.size();
        if (mMessagesOnStore> 0)
        {
            // Get the id of the first message in the store
            dirList.sort();
            mFirstMessageInStore = JavaCommonUtils::wstringToInt(
                                       dirList.front());
            // Get the id of the next message to be added to the store,
            // which will be one more than the value of the last
            // message in the store
            mNextMessageInStore = JavaCommonUtils::wstringToInt(dirList.back());
            mNextMessageInStore++;
        }
        return 0;
    }
    else
    {
        // Otherwise create a new directory
        return makeDirAll(mMessageStoreDirName);
    }
}

int ServerConnectionBase::makeDirAll(const std::wstring aDirPath)
{
    JELOG2(EWMA);
    bool ableToOpen = true;
    std::wstring path;
    wchar_t *last, *tok, *delim =L"\\" ;
    wchar_t *stringToTokenize = new wchar_t[aDirPath.length()+ 1];
    wcscpy(stringToTokenize, aDirPath.c_str());

    struct stat temp;

    for (tok = wcstok(stringToTokenize,delim,&last); tok!=NULL;
            tok = wcstok(NULL, delim, &last))
    {
        path += std::wstring(tok);
        path += L"\\";
        char *dirName =0;
        try 
        {
           dirName = JavaCommonUtils::wstringToUtf8(path);
        }
        catch (ExceptionBase &ex)
        {
           delete[] stringToTokenize;
           return -1;
        }
        if (ableToOpen)
        {
            if (0 != lstat(dirName, &temp))
            {
                ableToOpen = false;
                if (mkdir(dirName,0666) < 0)
                {
                    ELOG1(EWMA,"WMA : Directory Creation Failed : %s",dirName);
                    delete[] dirName;
                    delete[] stringToTokenize;
                    return -1;
                }
            }
        }
        else
        {
            if (mkdir(dirName,0666) < 0)
            {
                ELOG1(EWMA,"WMA : Directory Creation Failed : %s",dirName);
                delete[] dirName;
                delete[] stringToTokenize;
                return -1;
            }
        }
        delete[] dirName;
    }
    delete[] stringToTokenize;
    return 0;
}

int ServerConnectionBase::removeDir(const std::wstring aDirPath)
{
    JELOG2(EWMA);
    std::wstring path1;
    path1 +=  aDirPath;
    char* path =0;
    int error = 0;
    try
    {
        path = JavaCommonUtils::wstringToUtf8(path1);
        LOG1(EWMA, EInfo, "WMA : Removing Message Store %s",path);
        struct stat temp;
        if (0 != lstat(path, &temp))
        {
            ELOG1(EWMA, "WMA : Directory doesn't Exists : %s", path);
            delete[] path;
            return -1;
        }
        std::list<std::wstring> dirList =
            FileUtilities::getDirContentsList(aDirPath);
        std::list<std::wstring>::iterator it;
        if (dirList.size())
        {
            for (it = dirList.begin(); it != dirList.end(); it++)
            {
                std::wstring element(aDirPath + L"\\");
                element += *it;
                char *fileName = JavaCommonUtils::wstringToUtf8(element);
                if ((error = remove(fileName)) != 0)
                {
                    ELOG1(EWMA, "WMA : Removing File  Failed : %s", fileName);
                    delete[] fileName;
                    delete[] path;
                    return error;
                }
                delete[] fileName;
            }
        }
        if ((error = remove(path)) != 0)
        {
            ELOG1(EWMA, "WMA : Removing Directory  Failed : %s", path);
            delete[] path;
            return error;
        }
        delete[] path;
    }
    catch (ExceptionBase &ex)
    {
        delete[] path;
        ELOG(EWMA,"WMA : Cought an exception while removing Dir");
        return -1;
    }
    return error;
}

}// end of namespace wma
}// end of namespace java
