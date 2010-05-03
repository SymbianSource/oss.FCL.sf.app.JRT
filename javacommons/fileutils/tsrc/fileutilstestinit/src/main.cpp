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
* Description:  JavaCaptain Main
*
*/


#include <locale.h>

#include "filetestconsts.h"
#include "filetestutils.h"

using namespace std;
using namespace java::filetest;

void doCreateMainDir()
{
    wstring mainDirPath = FileTestConsts::KDefaultTestPath;
    FileTestUtils::createDir(mainDirPath);
}

void doInitTestExists()
{
    wstring mainDirPath = FileTestConsts::KDefaultTestPath;
    mainDirPath+=L"testexists/";
    FileTestUtils::createDir(mainDirPath);

    wstring dir1 = mainDirPath;
    dir1 += L"testExistsDir/";
    FileTestUtils::createDir(dir1);

    wstring file1 = mainDirPath;
    file1 += L"testExistsFile";
    FileTestUtils::createNormalFile(file1);
}

void doInitTestCreate()
{
    wstring mainDirPath = FileTestConsts::KDefaultTestPath;
    mainDirPath+=L"testcreate/";
    FileTestUtils::createDir(mainDirPath);

    // Relates to testFileCreateFailure()
    wstring file1 = mainDirPath;
    file1 += L"FileCreateFailureTest0";
    FileTestUtils::createNormalFile(file1);

    wstring file2 = mainDirPath;
    file2 += L"pää.txt";
    FileTestUtils::createNormalFile(file2);

    wstring dir = mainDirPath;
    dir += L"FileCreateFailureOnDirTest/";
    FileTestUtils::createDir(dir);
}

void doInitTestIsFile()
{
    wstring mainDirPath = FileTestConsts::KDefaultTestPath;
    mainDirPath+=L"testisfile/";
    FileTestUtils::createDir(mainDirPath);

    wstring file1 = mainDirPath;
    file1 += L"testOnFileTest";
    FileTestUtils::createNormalFile(file1);

    wstring dir = mainDirPath;
    dir += L"testOnDirTest/";
    FileTestUtils::createDir(dir);
}

void doInitTestIsDir()
{
    wstring mainDirPath = FileTestConsts::KDefaultTestPath;
    mainDirPath+=L"testisdir/";
    FileTestUtils::createDir(mainDirPath);

    wstring file1 = mainDirPath;
    file1 += L"testOnFileTest";
    FileTestUtils::createNormalFile(file1);

    wstring dir = mainDirPath;
    dir += L"testOnDirTest/";
    FileTestUtils::createDir(dir);
}

void doInitTestCanRead()
{
    wstring mainDirPath = FileTestConsts::KDefaultTestPath;
    mainDirPath+=L"testcanread/";
    FileTestUtils::createDir(mainDirPath);

    wstring file1 = mainDirPath;
    file1 += L"testOnReadOnlyFile";
    FileTestUtils::createReadOnlyFile(file1);

    wstring dir = mainDirPath;
    dir += L"testOnReadOnlyDir/";
    FileTestUtils::createReadOnlyDir(dir);

    wstring file2 = mainDirPath;
    file2 += L"testOnFile";
    FileTestUtils::createNormalFile(file2);
}

void doInitTestCanWrite()
{
    wstring mainDirPath = FileTestConsts::KDefaultTestPath;
    mainDirPath+=L"testcanwrite/";
    FileTestUtils::createDir(mainDirPath);

    wstring file1 = mainDirPath;
    file1 += L"testOnReadOnlyFile";
    FileTestUtils::createReadOnlyFile(file1);

    wstring dir = mainDirPath;
    dir += L"testOnReadOnlyDir/";
    FileTestUtils::createReadOnlyDir(dir);

    wstring file2 = mainDirPath;
    file2 += L"testOnFile";
    FileTestUtils::createNormalFile(file2);
}


void doInitTestFileSize()
{
    wstring mainDirPath = FileTestConsts::KDefaultTestPath;
    mainDirPath+=L"testfilesize/";
    FileTestUtils::createDir(mainDirPath);

    wstring file2 = mainDirPath;
    file2 += L"testOnFile";
    FileTestUtils::createNormalFile(file2);
    FileTestUtils::writeDataInLoop(file2, 1, 1000);

    wstring dir = mainDirPath;
    dir += L"testOnDir/";
    FileTestUtils::createDir(dir);
}

void doInitTestLastModified()
{
    wstring mainDirPath = FileTestConsts::KDefaultTestPath;
    mainDirPath+=L"testlastmodified/";
    FileTestUtils::createDir(mainDirPath);
}

void doInitTestDirCreate()
{
    wstring mainDirPath = FileTestConsts::KDefaultTestPath;
    mainDirPath+=L"testdircreate/";
    FileTestUtils::createDir(mainDirPath);

    wstring dir = mainDirPath;
    dir += L"FileCreateFailureTestDir0/";
    FileTestUtils::createDir(dir);

    wstring dir1 = mainDirPath;
    dir1 += L"pääDirTest/";
    FileTestUtils::createDir(dir1);

    wstring dir2 = mainDirPath;
    dir2 += L"FileCreateFailureOnDirTest/";
    FileTestUtils::createDir(dir2);
}

void doInitTestRename()
{
    wstring mainDirPath = FileTestConsts::KDefaultTestPath;
    mainDirPath+=L"testrename/";
    FileTestUtils::createDir(mainDirPath);

    wstring file2 = mainDirPath;
    file2 += L"FileUtilsTestFileRename.txt";
    FileTestUtils::createNormalFile(file2);

    wstring dir = mainDirPath;
    dir += L"FileUtilsTestFileRename/";
    FileTestUtils::createDir(dir);
}

void doInitTestInputStream()
{
    wstring mainDirPath = FileTestConsts::KDefaultTestPath;
    mainDirPath+=L"testinputstream/";
    FileTestUtils::createDir(mainDirPath);

    wstring file1 = mainDirPath;
    file1 += L"inputFile.txt";
    FileTestUtils::createNormalFile(file1);
    FileTestUtils::writeDataInLoop(file1, 1, 1000);

    wstring file2 = mainDirPath;
    file2 += L"inputFile001.txt";
    FileTestUtils::createNormalFile(file2);
    FileTestUtils::writeDataInLoop(file2, 1, 1000);
    FileTestUtils::writeDataInLoop(file2, 2, 1000);
    FileTestUtils::writeDataInLoop(file2, 3, 1000);
    FileTestUtils::writeDataInLoop(file2, 4, 1000);

    wstring file3 = mainDirPath;
    file3 += L"inputFileMark.txt";
    FileTestUtils::createNormalFile(file3);
    FileTestUtils::writeDataInLoop(file3, 5, 20);
    FileTestUtils::writeDataInLoop(file3, 6, 10);
    FileTestUtils::writeDataInLoop(file3, 7, 15);
}

void doInitTestOutputStream()
{
    wstring mainDirPath = FileTestConsts::KDefaultTestPath;
    mainDirPath+=L"testoutputstream/";
    FileTestUtils::createDir(mainDirPath);
}

void doInitTestDirListing()
{
    wstring mainDirPath = FileTestConsts::KDefaultTestPath;
    mainDirPath+=L"testlist/";
    FileTestUtils::createDir(mainDirPath);

    wstring dir = mainDirPath;
    dir += L"folder1/";
    FileTestUtils::createDir(dir);

    wstring dir1 = mainDirPath;
    dir1 += L"folder2/";
    FileTestUtils::createDir(dir1);

    dir1 = mainDirPath;
    dir1 += L"folder3/";
    FileTestUtils::createDir(dir1);

    dir1 = mainDirPath;
    FileTestUtils::createHiddenDir(dir1, L"folder4/");

    wstring file1 = mainDirPath;
    file1 += L"inputFile.txt";
    FileTestUtils::createNormalFile(file1);

    file1 = mainDirPath;
    file1 += L"inputFile1.txt";
    FileTestUtils::createNormalFile(file1);

    file1 = mainDirPath;
    file1 += L"inputFile2.txt";
    FileTestUtils::createNormalFile(file1);

    file1 = mainDirPath;
    file1 += L"inputFile3.txt";
    FileTestUtils::createNormalFile(file1);

    file1 = mainDirPath;
    file1 += L"pää.txt";
    FileTestUtils::createNormalFile(file1);

    file1 = mainDirPath;
    FileTestUtils::createHiddenFile(file1, L"hidden.txt");

}

void doInitTestDelete()
{
    wstring mainDirPath = FileTestConsts::KDefaultTestPath;
    mainDirPath+=L"testdelete/";
    FileTestUtils::createDir(mainDirPath);

    wstring dir = mainDirPath;
    dir += L"folder1/";
    FileTestUtils::createDir(dir);

    wstring file1 = mainDirPath;
    file1 += L"inputFile.txt";
    FileTestUtils::createNormalFile(file1);

    wstring file3 = mainDirPath;
    file3 += L"inputStreamFile.txt";
    FileTestUtils::createNormalFile(file3);
    FileTestUtils::writeDataInLoop(file3, 5, 20);

    file3 = mainDirPath;
    file3 += L"outputStreamFile.txt";
    FileTestUtils::createNormalFile(file3);
}

void doInitTestMkdirs()
{
    wstring mainDirPath = FileTestConsts::KDefaultTestPath;
    mainDirPath+=L"testmkdirs/";
    FileTestUtils::createDir(mainDirPath);
}

void doInitTestFileCopy()
{
    wstring mainDirPath = FileTestConsts::KDefaultTestPath;
    mainDirPath+=L"testfilecopy/";
    FileTestUtils::createDir(mainDirPath);
}

void doInitTestFileMove()
{
    wstring mainDirPath = FileTestConsts::KDefaultTestPath;
    mainDirPath+=L"testfilemove/";
    FileTestUtils::createDir(mainDirPath);

    mainDirPath += L"folderToMove/";
    FileTestUtils::createDir(mainDirPath);

    wstring dir = mainDirPath;
    dir += L"folder1/";
    FileTestUtils::createDir(dir);

    dir = mainDirPath;
    dir += L"folder2/";
    FileTestUtils::createDir(dir);

    wstring dir1 = dir;
    dir1 += L"nestedfolder1/";
    FileTestUtils::createDir(dir1);

    wstring file = mainDirPath;
    file += L"file1.txt";
    FileTestUtils::createNormalFile(file);
    FileTestUtils::writeDataInLoop(file, 5, 20);

    file = mainDirPath;
    file += L"file2.txt";
    FileTestUtils::createNormalFile(file);

    file = mainDirPath;
    file += L"file3.txt";
    FileTestUtils::createNormalFile(file);
    FileTestUtils::writeDataInLoop(file, 5, 20);

    file = mainDirPath;
    file += L"file4.txt";
    FileTestUtils::createNormalFile(file);
}

void doInitTestFileCopyAll()
{
    wstring mainDirPath = FileTestConsts::KDefaultTestPath;
    mainDirPath+=L"testfilecopyall/";
    FileTestUtils::createDir(mainDirPath);

    mainDirPath += L"source/";
    FileTestUtils::createDir(mainDirPath);

    wstring dir = mainDirPath;
    dir += L"folder1/";
    FileTestUtils::createDir(dir);

    dir = mainDirPath;
    dir += L"folder2/";
    FileTestUtils::createDir(dir);

    wstring dir1 = dir;
    dir1 += L"nestedfolder1/";
    FileTestUtils::createDir(dir1);

    wstring file = mainDirPath;
    file += L"file1.txt";
    FileTestUtils::createNormalFile(file);
    FileTestUtils::writeDataInLoop(file, 5, 20);

    file = mainDirPath;
    file += L"file2.txt";
    FileTestUtils::createNormalFile(file);

    file = mainDirPath;
    file += L"file3.txt";
    FileTestUtils::createNormalFile(file);
    FileTestUtils::writeDataInLoop(file, 5, 20);

    file = mainDirPath;
    file += L"file4.txt";
    FileTestUtils::createNormalFile(file);


    mainDirPath = FileTestConsts::KDefaultTestPath;
    mainDirPath+=L"testfilecopyall/";

    mainDirPath += L"source1/";
    FileTestUtils::createDir(mainDirPath);

    dir = mainDirPath;
    dir += L"folder1/";
    FileTestUtils::createDir(dir);

    dir = mainDirPath;
    dir += L"folder2/";
    FileTestUtils::createDir(dir);

    dir1 = dir;
    dir1 += L"nestedfolder1/";
    FileTestUtils::createDir(dir1);

    file = mainDirPath;
    file += L"file1.txt";
    FileTestUtils::createNormalFile(file);
    FileTestUtils::writeDataInLoop(file, 5, 20);

    file = mainDirPath;
    file += L"file2.txt";
    FileTestUtils::createNormalFile(file);

    file = mainDirPath;
    file += L"file3.txt";
    FileTestUtils::createNormalFile(file);
    FileTestUtils::writeDataInLoop(file, 5, 20);

    file = mainDirPath;
    file += L"file4.txt";
    FileTestUtils::createNormalFile(file);
}

void doInitTestFileDeleteAll()
{
    wstring mainDirPath = FileTestConsts::KDefaultTestPath;
    mainDirPath+=L"testfiledeleteall/";
    FileTestUtils::createDir(mainDirPath);

    mainDirPath += L"source/";
    FileTestUtils::createDir(mainDirPath);

    wstring dir = mainDirPath;
    dir += L"folder1/";
    FileTestUtils::createDir(dir);

    dir = mainDirPath;
    dir += L"folder2/";
    FileTestUtils::createDir(dir);

    wstring file = mainDirPath;
    file += L"file1.txt";
    FileTestUtils::createNormalFile(file);
    FileTestUtils::writeDataInLoop(file, 5, 20);

    file = mainDirPath;
    file += L"file2.txt";
    FileTestUtils::createNormalFile(file);

    file = mainDirPath;
    file += L"file3.txt";
    FileTestUtils::createNormalFile(file);
    FileTestUtils::writeDataInLoop(file, 5, 20);

    file = mainDirPath;
    file += L"file4.txt";
    FileTestUtils::createNormalFile(file);
}


int main(int /*argc*/, char** /*argv*/)
{
    setlocale(LC_ALL, "");

    // Create the main folder within which tests are executed.
    doCreateMainDir();

    doInitTestExists();

    doInitTestCreate();

    doInitTestIsFile();

    doInitTestIsDir();

    doInitTestCanRead();

    doInitTestCanWrite();

    doInitTestFileSize();

    doInitTestLastModified();

    doInitTestDirCreate();

    doInitTestRename();

    doInitTestInputStream();

    doInitTestOutputStream();

    doInitTestDirListing();

    doInitTestDelete();

    doInitTestMkdirs();

    doInitTestFileCopy();

    doInitTestFileMove();

    doInitTestFileCopyAll();

    doInitTestFileDeleteAll();
}
