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
* Description:  Contains implementation of FileUtilities
 *
*/


#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stack>

#include "logger.h"
#include "javaoslayer.h"
#include "javacommonutils.h"
#include "fileutilities.h"

using namespace java::fileutils;
using namespace java::util;

OS_EXPORT bool FileUtilities::isFile(const std::wstring& aPath)
{
    JELOG2(EJavaFile);
    char* utf8Name = JavaCommonUtils::wstringToUtf8(aPath);
    struct stat fileStat;
    int error = lstat(utf8Name, &fileStat);

    bool retValue = false;

    // If there was no error, then go ahead and retrieve the data out of struct.
    if (0 == error)
    {
        mode_t mode = fileStat.st_mode;

        if (S_ISREG(mode))
        { // If it is regular file, return true.
            retValue = true;
        }
    }
    else
    {
        LOG1(EJavaFile, EInfo,
             "FileUtilities::isFile: Unable to stat file: %s", utf8Name);
    }

    // Deallocate utf8 name
    delete[] utf8Name;
    return retValue;
}

OS_EXPORT bool FileUtilities::isDirectory(const std::wstring& aPath)
{
    JELOG2(EJavaFile);
    char* utf8Name = JavaCommonUtils::wstringToUtf8(aPath);
    struct stat fileStat;
    int error = lstat(utf8Name, &fileStat);
    bool retValue = false;

    // If there was no error, then go ahead and retrieve the data out of struct.
    if (0 == error)
    {
        mode_t mode = fileStat.st_mode;

        if (S_ISDIR(mode))
        {
            // If it is directory, return true.
            retValue = true;
        }
    }
    else
    {
        LOG1(EJavaFile, EInfo,
             "FileUtilities::isDirectory: Unable to stat file: %s", utf8Name);
    }

    // Deallocate utf8 name
    delete[] utf8Name;
    return retValue;
}

OS_EXPORT bool FileUtilities::exists(const std::wstring& aPath)
{
    JELOG2(EJavaFile);
    char* utf8Name = JavaCommonUtils::wstringToUtf8(aPath);
    bool retValue = false;

    struct stat fileStat;

    // We try to stat the file, if it succeeds, then we can say it exists.
    // When we try to stat, the file is not opened. So, even if access restrictions
    // are present, we can still know if it exists.
    int error = lstat(utf8Name, &fileStat);
    if (0 == error)
    {
        retValue = true;
    }
    else
    {
        LOG1(EJavaFile, EInfo, "FileUtilities::exists: Unable to stat file %s",
             utf8Name);
    }

    delete[] utf8Name;
    return retValue;
}

OS_EXPORT bool FileUtilities::canRead(const std::wstring& aPath)
{
    JELOG2(EJavaFile);
    char* utf8Name = JavaCommonUtils::wstringToUtf8(aPath);
    bool retValue = false;

    struct stat fileStat;
    int error = lstat(utf8Name, &fileStat);

    if (0 == error)
    {
        mode_t curMode = fileStat.st_mode;

        if (curMode & (S_IRUSR))
        {
            retValue = true;
        }
    }
    else
    {
        LOG1(EJavaFile, EInfo,
             "FileUtilities::canRead: Unable to check access for %s",
             utf8Name);
    }

    delete[] utf8Name;
    return retValue;
}

OS_EXPORT bool FileUtilities::canWrite(const std::wstring& aPath)
{
    JELOG2(EJavaFile);
    char* utf8Name = JavaCommonUtils::wstringToUtf8(aPath);
    bool retValue = false;

    struct stat fileStat;
    int error = lstat(utf8Name, &fileStat);

    if (0 == error)
    {
        mode_t curMode = fileStat.st_mode;

        if (curMode & (S_IWUSR))
        {
            retValue = true;
        }
    }
    else
    {
        LOG2(
            EJavaFile,
            EInfo,
            "FileUtilities::canWrite: Unable to check access for %s: Error: %d",
            utf8Name, errno);
    }

    delete[] utf8Name;
    return retValue;
}

OS_EXPORT long FileUtilities::fileSize(const std::wstring& aPath)
{
    JELOG2(EJavaFile);
    char* utf8Name = JavaCommonUtils::wstringToUtf8(aPath);

    struct stat fileStat;
    int error = lstat(utf8Name, &fileStat);

    long retValue = 0;
    if (0 == error)
    {
        retValue = fileStat.st_size;
    }
    else
    {
        ELOG2(EJavaFile,
              "FileUtilities::fileSize: Unable to stat file %s: errno: ",
              utf8Name, errno);

        delete[] utf8Name;
        int error = errno;
        throw error;
    }

    delete[] utf8Name;
    return retValue;
}

OS_EXPORT long FileUtilities::lastModified(const std::wstring& aPath)
{
    JELOG2(EJavaFile);
    char* utf8Name = JavaCommonUtils::wstringToUtf8(aPath);

    struct stat fileStat;
    int error = lstat(utf8Name, &fileStat);

    long retValue = 0;
    if (0 == error)
    {
        retValue = fileStat.st_mtime;
    }
    else
    {
        ELOG2(
            EJavaFile,
            "FileUtilities::lastModified: Error stating file. return = 0 %s: errno: ",
            utf8Name, errno);
        retValue = 0;
    }

    delete[] utf8Name;
    return retValue;
}

OS_EXPORT long FileUtilities::getDirSize(const std::wstring& origDirName,
        const bool includeSubDirs)
{
    JELOG2(EJavaFile);

    long totalSize = 0;
    char* multiByteDirName = java::util::JavaCommonUtils::wstringToUtf8(
                                 origDirName);
    std::string dirName(multiByteDirName);
    std::stack<std::string> subDirs;
    subDirs.push(dirName);

    while (!subDirs.empty())
    {
        //while stack not empty, keep stating files
        dirName = subDirs.top();
        subDirs.pop();
        DIR* wDirHandle = opendir(dirName.c_str());
        if (wDirHandle)
        {
            struct dirent* dir = readdir(wDirHandle);
            while (dir)
            {
                //dirName = subDirs.top();
                std::string name(dirName);
                name += "/";
                name += std::string(dir->d_name);

                LOG1(EJavaFile, EInfo,
                     "  FileUtilities::getDirSize(): Stating %s",
                     name.c_str());
                struct stat buf;
                if (stat(name.c_str(), &buf) == 0)
                {
                    totalSize += buf.st_size;

                    if (includeSubDirs && S_ISDIR(buf.st_mode))
                    {
                        //if it is directory, put it onto stack
                        subDirs.push(name);
                    }
                }
                dir = readdir(wDirHandle);
            }
            closedir(wDirHandle);
        }
    }

    delete[] multiByteDirName; //JavaCommonUtils Allocates Memory
    return totalSize;
}

OS_EXPORT std::list<std::wstring> FileUtilities::getDirContentsList(const std::wstring& dirName)
{
    JELOG2(EJavaFile);
    std::list<std::wstring> contentList;
    std::wstring stringContents = getDirContents(dirName);

    wchar_t* delim = L"*";
    wchar_t* last;
    wchar_t* tok;
    wchar_t* stringToTokenize = new wchar_t[ stringContents.length()+1 ];
    wcscpy(stringToTokenize, stringContents.c_str());

    for (tok = wcstok(stringToTokenize,delim,&last); tok!=NULL;
            tok = wcstok(NULL, delim, &last))
    {
        LOG1(EJavaFile, EInfo, "  FileUtilities::getDirContentsList(): Adding %S", tok);
        contentList.push_back(std::wstring(tok));
    }

    delete[] stringToTokenize;
    return contentList;
}

OS_EXPORT int FileUtilities::makeDirAll(const std::wstring& aDirPath)
{
    JELOG2(EJavaFile);
    bool ableToOpen = true;
    std::wstring path;
    wchar_t *last, *tok, *delim =L"/" ;
    wchar_t *stringToTokenize = new wchar_t[aDirPath.length()+ 1];
    wcscpy(stringToTokenize, aDirPath.c_str());

    for (tok = wcstok(stringToTokenize,delim,&last); tok!=NULL;
            tok = wcstok(NULL, delim, &last))
    {
        path += std::wstring(tok);
        path += L"/";
        char *dirName = JavaCommonUtils::wstringToUtf8(path);
        if (ableToOpen)
        {
            if (!opendir(dirName))
            {
                ableToOpen = false;
                if (mkdir(dirName,0666)<0)
                {
                    WLOG1(EJavaFile,"FileUtilities::makeDirAll: Directory Creation Failed : %s",dirName);
                    delete[] dirName;
                    delete[] stringToTokenize;
                    return -1;
                }
            }
        }
        else
        {
            if (mkdir(dirName,0666)<0)
            {
                ELOG1(EJavaFile, "FileUtilities::makeDirAll: Directory Creation Failed : %s",dirName);
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

OS_EXPORT std::wstring FileUtilities::getDirContents(const std::wstring& aDirName)
{
    return getDirContents(aDirName, true);
}
