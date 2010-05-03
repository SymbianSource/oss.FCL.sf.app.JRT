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


#include <fstream>
#include <iostream>
#include <stdio.h>
#include <errno.h>
#include "filetestutils.h"

using namespace std;
using namespace java::filetest;

void FileTestUtils::createFile(const wstring &aPath, const int aFlags, int aMode)
{
    char* chr = new char[aPath.size()+1];
    sprintf(chr,"%ls",aPath.c_str());

    if (0 == aMode)
    {
        aMode = 0777;
    }
    int fd = open(chr, aFlags, aMode);
    FileTestUtils::checkError(aPath,fd);
    close(fd);
}


void FileTestUtils::createSystemFile(const wstring &aPath)
{
    FileTestUtils::createFile(aPath, O_CREAT|O_EXCL|O_WRONLY);
    //There is nothing like System File in Linux. So do nothing.
}

void FileTestUtils::createHiddenDir(const wstring &aPath, const wstring& aFile)
{
    wstring name = aPath+L"."+aFile;
    FileTestUtils::createDir(name.c_str());
    //Creating dir which starts with .is as good as hidden :-)
}

void FileTestUtils::createSystemDir(const wstring &aPath)
{
    FileTestUtils::createDirMode(aPath, 0777);
    //There is nothing like System File in Linux. So do nothing.
}

void FileTestUtils::createDirMode(const wstring& aPath, const int aMode)
{
    // R W X
    // 4 2 1
    char* chr = new char[aPath.size()+1];
    sprintf(chr,"%ls",aPath.c_str());
    int error = mkdir(chr, aMode);
    FileTestUtils::checkError(aPath, error);
}

void FileTestUtils::writeDataToFile(const wstring& aPath, const string& aData)
{
    //Linux, just use ofstream
    char* chr = new char[aPath.size()+1];
    sprintf(chr,"%ls",aPath.c_str());
    int fd = open(chr, O_RDWR);
    CheckError(aPath, fd);
    write(fd, aData.c_str(), aData.length());
    close(fd);
}

void FileTestUtils::writeDataInLoop(const wstring& aPath, const int aData, const int aTimes)
{
    char* chr = new char[aPath.size()+1];
    sprintf(chr,"%ls",aPath.c_str());
    ofstream file(chr, ios::out|ios::app|ios::binary);

    for (int i=0 ; i<aTimes; i++)
    {
        file<<(char)aData;
    }
    file.close();

}

void FileTestUtils::appendDataToFile(const wstring& aPath, const int aData)
{
    char* chr = new char[aPath.size()+1];
    sprintf(chr,"%ls",aPath.c_str());
    ofstream file(chr, ios::out|ios::app|ios::binary);
    file<<(char)aData;
    file.close();
}

void FileTestUtils::appendDataToFile(const wstring& aPath, const wstring& aData)
{
    char* chr = new char[aPath.size()+1];
    sprintf(chr,"%ls",aPath.c_str());
    ofstream file(chr, ios::out|ios::app|ios::binary);
    file<<(char*)aData.c_str();
    file.close();
}

void FileTestUtils::setFileHidden(const wstring &aPath, const wstring &aFile)
{
    //File Starting from . is considered hidden
    wstring oldFile = aPath+aFile;
    wstring newName = aPath+L"."+aFile;

    char* chr = new char[aPath.size()+1];
    sprintf(chr,"%ls",aPath.c_str());

    char* chr1 = new char[aFile.size()+1];
    sprintf(chr1,"%ls",aFile.c_str());
    rename(chr,chr1);
}

void FileTestUtils::setFileReadOnly(const wstring &aPath)
{
    char* chr = new char[aPath.size()+1];
    sprintf(chr,"%ls",aPath.c_str());
    chmod(chr, 0444);
}

void FileTestUtils::setDirHidden(const wstring &aPath, const wstring &aName)
{
    wstring oldFile = aPath+aName;
    wstring newName = aPath+L"."+aName;

    char* chr = new char[aPath.size()+1];
    sprintf(chr,"%ls",aPath.c_str());

    char* chr1 = new char[aName.size()+1];
    sprintf(chr1,"%ls",aName.c_str());
    rename(chr,chr1);
}
