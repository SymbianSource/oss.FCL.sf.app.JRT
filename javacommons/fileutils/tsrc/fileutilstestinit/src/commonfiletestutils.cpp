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
* Description:  FileTestUtils
 *
*/


#include <errno.h>
#include "filetestutils.h"

using namespace std;
using namespace java::filetest;


void FileTestUtils::createReadOnlyFile(const wstring &aPath)
{
    FileTestUtils::createFile(aPath, O_CREAT|O_EXCL|O_RDONLY);
}

void FileTestUtils::createWriteOnlyFile(const wstring &aPath)
{
    FileTestUtils::createFile(aPath, O_CREAT|O_EXCL|O_WRONLY);
}

void FileTestUtils::createNormalFile(const wstring& aPath)
{
    FileTestUtils::createFile(aPath, O_CREAT|O_EXCL);
}

void FileTestUtils::checkError(const wstring& aString,const int aErr)
{
    if (0 > aErr)
    {
        printf("Errno: %d: Path: %S\n", errno,  aString.c_str());
        exit(aErr);
    }
}

void FileTestUtils::createReadOnlyDir(const wstring& aPath)
{
    FileTestUtils::createDirMode(aPath, 0444);
}

void FileTestUtils::createDir(const wstring &aPath)
{
    FileTestUtils::createDirMode(aPath, 0777);
}


void FileTestUtils::createHiddenFile(const wstring &aPath, const wstring &aFile)
{
    //In linux, we just rename aFIle with "."+aFile and then create a file. :-)
    FileTestUtils::createNormalFile(aPath+aFile);
    FileTestUtils::setFileHidden(aPath, aFile);
}

