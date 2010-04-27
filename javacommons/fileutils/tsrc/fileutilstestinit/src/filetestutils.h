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


#ifndef FILETESTUTILS_H
#define FILETESTUTILS_H

#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>

namespace java
{
namespace filetest
{

using namespace std;
class FileTestUtils
{
public:
    //File Operations
    static void createReadOnlyFile(const wstring& aPath);
    static void createWriteOnlyFile(const wstring& aPath);
    static void createNormalFile(const wstring& aPath);
    static void createFile(const wstring &aPath, const int aFlags, int  aMode=0);
    static void createHiddenFile(const wstring &aPath, const wstring &aFile);
    static void createSystemFile(const wstring &aPath);
    static void setFileHidden(const wstring &aPath, const wstring &aFile);
    static void setFileReadOnly(const wstring &aPath);

    //Directory Operations
    static void createDirMode(const wstring &aPath, const int mode);
    static void createDir(const wstring &aPath);
    static void createHiddenDir(const wstring &aPath, const wstring& aDirName);
    static void createSystemDir(const wstring &aPath);
    static void createReadOnlyDir(const wstring &aPath);
    static void setDirHidden(const wstring &aPath, const wstring &aName);

    static void appendDataToFile(const wstring &aPath, const int aData);
    static void appendDataToFile(const wstring &aPath, const wstring& aData);
    static void writeDataToFile(const wstring &aPath, const string &aData);
    static void checkError(const wstring &aString, const int aErr);

    static void writeDataInLoop(const wstring &aPath, const int aData, const int aTimes);
    static void doInitDRMContents();
};

}  //end namespace filetest
}  //end namespace java

#endif // FILETESTUTILS_H
