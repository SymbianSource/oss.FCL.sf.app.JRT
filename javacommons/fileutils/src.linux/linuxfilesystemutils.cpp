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
* Description:  LinuxFileSystemUtils
 *
*/


#include <errno.h>
#include <sys/statvfs.h>
#include <sys/stat.h>
#include <string>
#include <stdlib.h>

#include "javajniutils.h"
#include "javacommonutils.h"
#include "logger.h"

#include "fileutilities.h"
#include "linuxfilesystemutils.h"

using namespace std;
using namespace java::fileutils;
using namespace java::util;

/*
 * Implementation of LinuxFileSystemUtils.h
 *
 */
unsigned long LinuxFileSystemUtils::getAvailableSize(const char* aPath)
{
    JELOG2(EJavaFile);
    struct statvfs fiData;

    int result = statvfs(aPath, &fiData);
    if (result < 0)
    {
        ELOG1(EJavaFile,
              "  LinuxFileSystemUtils::GetAvailableSize(): Error %d ", errno);
        int error = errno;
        throw error;
    }
    return (fiData.f_bfree * fiData.f_bsize);
}

unsigned long LinuxFileSystemUtils::getUsedSize(const char* aPath)
{
    JELOG2(EJavaFile);
    struct statvfs fsData;
    int result = statvfs(aPath, &fsData);
    if (result < 0)
    {
        ELOG1(EJavaFile, "  LinuxFileSystemUtils::GetUsedSize(): Error %d ",
              errno);
        int error = errno;
        throw error;
    }

    return ((fsData.f_blocks - fsData.f_bfree) * fsData.f_bsize);

}

unsigned long LinuxFileSystemUtils::getTotalSize(const char* aPath)
{
    JELOG2(EJavaFile);
    struct statvfs fsData;
    int result = statvfs(aPath, &fsData);
    if (result < 0)
    {
        ELOG1(EJavaFile, "  LinuxFileSystemUtils::GetTotalSize(): Error %d ",
              errno);
        int error = errno;
        throw error;
    }

    return (fsData.f_blocks * fsData.f_frsize);
}

bool LinuxFileSystemUtils::isHidden(const char* aFile)
{
    //Files in Linux are hidden if they have a "." as a prefix.
    //We need to search for "/." in the file name to confirm. Simple. Or is it?
    JELOG2(EJavaFile);
    std::string name(aFile);

    int lastIndex = name.find_last_of('/') ;
    if (name.at(lastIndex+1) == '.')
    {
        return true;
    }
    return false;
}

int LinuxFileSystemUtils::setHidden(const char* /*aFile*/, const bool /*aHide*/)
{
    return 0;
}

string LinuxFileSystemUtils::listRoots()
{
    string retValue(getenv("HOME"));
    return retValue;
}

