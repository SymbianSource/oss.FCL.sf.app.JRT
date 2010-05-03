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
* Description:  FileSystemUtils
 *
*/


#include <errno.h>
#include <sys/statvfs.h>
#include <sys/stat.h>
#include <string>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>

#include "javajniutils.h"
#include "javacommonutils.h"
#include "logger.h"

#include "fileutilities.h"
#include "linuxfilesystemutils.h"

using namespace std;
using namespace java::fileutils;
using namespace java::util;

/*
 * Implementation of FileUtilities methods
 *
 */
OS_EXPORT long long FileUtilities::availableSize(const std::wstring& aDrive)
{
    JELOG2(EJavaFile);
    char* mbDrive = JavaCommonUtils::wstringToUtf8(aDrive);
    long retVal = 0;
    try
    {
        retVal = LinuxFileSystemUtils::getAvailableSize(mbDrive);
    }
    catch (...)
    {
        // We need not report this error. If there was some problem, return -1
        retVal = -1;
    }
    delete[] mbDrive;
    return retVal;
}

OS_EXPORT long long FileUtilities::totalSize(const std::wstring& aDrive)
{
    JELOG2(EJavaFile);
    char* mbDrive = JavaCommonUtils::wstringToUtf8(aDrive);
    long retVal = 0;
    try
    {
        retVal = LinuxFileSystemUtils::getTotalSize(mbDrive);
    }
    catch (...)
    {
        // We need not report this error. If there was some problem, return -1
        retVal = -1;
    }
    delete[] mbDrive;
    return retVal;
}

OS_EXPORT long long FileUtilities::usedSize(const std::wstring& aDrive)
{
    JELOG2(EJavaFile);
    char* mbDrive = JavaCommonUtils::wstringToUtf8(aDrive);
    long retVal = 0;
    try
    {
        retVal = LinuxFileSystemUtils::getUsedSize(mbDrive);
    }
    catch (...)
    {
        // We need not report this error. If there was some problem, return -1
        retVal = -1;
    }
    delete[] mbDrive;
    return retVal;
}

OS_EXPORT bool FileUtilities::isHidden(const std::wstring& aFile)
{
    JELOG2(EJavaFile);
    std::wstring file = aFile;
    if (aFile.at(aFile.length()-1) == L'/')
    {
        file = aFile.substr(0, aFile.length()-1);
    }
    char* mbDrive = JavaCommonUtils::wstringToUtf8(file);

    bool retVal = LinuxFileSystemUtils::isHidden(mbDrive);
    delete[] mbDrive;
    return retVal;
}

OS_EXPORT std::wstring FileUtilities::listRoots()
{
    JELOG2(EJavaFile);
    std::string retString = LinuxFileSystemUtils::listRoots();
    std::wstring wcRetString = JavaCommonUtils::utf8ToWstring(retString.c_str());
    return wcRetString;

}

OS_EXPORT int FileUtilities::setReadable(const std::wstring& aFile, bool aReadable)
{
    JELOG2(EJavaFile);
    int result = 0;
    char* utf8Name = JavaCommonUtils::wstringToUtf8(aFile);

    struct stat fileStat;
    int error = lstat(utf8Name, &fileStat);

    if (0!=error)
    {
        return -1;
    }
    mode_t currentMode = fileStat.st_mode;

    if (aReadable)
    {
        currentMode = currentMode | S_IRUSR;
    }
    else
    {
        currentMode = currentMode & (~S_IRUSR);
    }

    result = chmod(utf8Name, currentMode);

    delete[] utf8Name;
    return result;
}

OS_EXPORT int FileUtilities::setWritable(const std::wstring& aFile, bool aWritable)
{
    JELOG2(EJavaFile);
    int result = 0;
    char* utf8Name = JavaCommonUtils::wstringToUtf8(aFile);

    struct stat fileStat;
    int error = lstat(utf8Name, &fileStat);

    if (0!=error)
    {
        return -1;
    }

    mode_t currentMode = fileStat.st_mode;

    if (aWritable)
    {
        currentMode = currentMode | S_IWUSR;
    }
    else
    {
        currentMode = currentMode & (~S_IWUSR);
    }

    result = chmod(utf8Name, currentMode);

    delete[] utf8Name;
    return result;
}

OS_EXPORT std::wstring FileUtilities::getDirContents(const std::wstring& dName,
        bool aIncludeHidden)
{
    JELOG2(EJavaFile);
    std::wstring origDirName = dName;
    if (origDirName.at(origDirName.length() - 1) == L'/')
    {
        int len = origDirName.length();
        std::wstring trimName = origDirName.substr(0, len -1);
        origDirName = trimName;
    }

    //since there are no unicode variants of openDir and readDir in linux, we will use basic strings
    char* multiByteDirName = new char[ origDirName.length()+1 ];
    multiByteDirName = java::util::JavaCommonUtils::wstringToUtf8(origDirName);
    std::string dirName(multiByteDirName);
    std::string origMbsDirName(multiByteDirName);
    std::wstring iFileListWrapper;

    DIR* wDirHandle = opendir(dirName.c_str());
    if (wDirHandle)
    {
        struct dirent* dir = readdir(wDirHandle);
        while (dir)
        {
            //keep reading directory
            dirName = origMbsDirName;

            std::string name(dirName);
            name += "/";
            name += std::string(dir->d_name);

            if (!aIncludeHidden)
            {
                std::wstring wName = java::util::JavaCommonUtils::utf8ToWstring(name.c_str());
                if (isHidden(wName))
                {
                    // If it is hidden, continue.
                    dir = readdir(wDirHandle);
                    continue;
                }
            }

            LOG1(EJavaFile, EInfo, "  FileUtilities::getDirContents(): Trying to Stat :%s: ",name.c_str());
            struct stat buf;
            if (stat(name.c_str() , &buf) == 0)
            {
                iFileListWrapper += java::util::JavaCommonUtils::utf8ToWstring(dir->d_name);
                if (S_ISDIR(buf.st_mode))
                { //it is directory
                    LOG1(EJavaFile, EInfo, "  FileUtilities::getDirContents(): Directory: %s",dir->d_name);
                    iFileListWrapper += std::wstring(L"/*");
                }
                else
                {
                    LOG1(EJavaFile, EInfo, "  FileUtilities::getDirContents(): File: %s",dir->d_name);
                    iFileListWrapper += std::wstring(L"*");
                }
            }
            else
            {
                WLOG1(EJavaFile, "  FileUtilities::getDirContents(): Error in Stat: %d ",errno);
            }
            dir = readdir(wDirHandle);
        }
        closedir(wDirHandle);
    }
    else
    {
        ELOG1(EJavaFile,"  FileUtilities::getDirContents(): Error Opening Dir: %d",errno);
    }

    delete[] multiByteDirName;
    return iFileListWrapper;
}
