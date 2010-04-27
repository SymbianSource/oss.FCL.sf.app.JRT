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


#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <wchar.h>

#include "filetestconsts.h"
#include "filetestutils.h"


using namespace std;
using namespace java::filetest;

void doCreateMainDir()
{
    wstring mainDirPath = FileTestConsts::KDefaultRootPath;
    mainDirPath +=  L"mainTestDir/";
    FileTestUtils::createDir(mainDirPath);
    wstring file002Ptr = FileTestConsts::KDefaultRootPath;
    file002Ptr += L"mainTestDir/tmp";
    FileTestUtils::createNormalFile(file002Ptr);
}


void doCreate000()
{
    wstring dirPath = FileTestConsts::KDefaultRootPath;
    dirPath += L"mainTestDir/test000/";
    FileTestUtils::createDir(dirPath);

    //file 002
    wstring filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test000/file 002.txt";
    FileTestUtils::createNormalFile(filePath);

    filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test000/p��.txt";
    FileTestUtils::createNormalFile(filePath);

    filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test000/";
    FileTestUtils::createHiddenFile(filePath, L"fileHidden.txt");

    filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test000/fileSystem.txt";
    FileTestUtils::createSystemFile(filePath);

    filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test000/dir 003/";
    FileTestUtils::createDir(filePath);

    filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test000/";
    FileTestUtils::createHiddenDir(filePath, L"dirHidden/");

    filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test000/dirSystem/";
    FileTestUtils::createSystemDir(filePath);

    filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test000/dir 004/";
    FileTestUtils::createReadOnlyDir(filePath);

    filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test000/";
    wstring file(L"file004");
    FileTestUtils::createNormalFile(filePath+file);
    string data("addingSomeDataToFile");
    FileTestUtils::writeDataToFile(filePath+file, data);
    FileTestUtils::setFileReadOnly(filePath+file);

    filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test000/file007.txt";
    FileTestUtils::createNormalFile(filePath);
    string data2("addingSomeDataToFile2");
    FileTestUtils::writeDataToFile(filePath, data2);

    filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test000/binaryFile.dat";
    FileTestUtils::createNormalFile(filePath);
    FileTestUtils::setFileReadOnly(filePath);
}

void doCreate020()
{
    wstring filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test020/";
    FileTestUtils::createDir(filePath);

    wstring fileName = L"binaryFile.dat";
    FileTestUtils::createNormalFile(filePath+fileName);
    FileTestUtils::writeDataInLoop(filePath+fileName, 4, 1025);

    fileName = L"binaryFile2.dat";
    FileTestUtils::createNormalFile(filePath+fileName);
    FileTestUtils::writeDataInLoop(filePath+fileName, 56, 1025);
    FileTestUtils::setFileReadOnly(filePath+fileName);

    fileName = L"textFile.txt";
    FileTestUtils::createNormalFile(filePath+fileName);
    for (int i=0; i<1025; ++i)
    {
        FileTestUtils::appendDataToFile(filePath+fileName, L"a");
    }

    wstring subDir = L"subDir1/";
    FileTestUtils::createDir(filePath+subDir);

    subDir = L"mainTestDir/test027/";
    FileTestUtils::createDir(FileTestConsts::KDefaultRootPath+subDir);

    fileName = L"test027.jar";
    FileTestUtils::createNormalFile(FileTestConsts::KDefaultRootPath+subDir+fileName);
    FileTestUtils::writeDataInLoop(FileTestConsts::KDefaultRootPath+subDir+fileName, 57, 25);
}

void doCreate040()
{
    wstring filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test040/";
    FileTestUtils::createDir(filePath);

    wstring fileName = L"binaryFile.dat";
    FileTestUtils::createNormalFile(filePath+fileName);
    FileTestUtils::writeDataInLoop(filePath+fileName, 56, 100);
}

void doCreate050()
{
    wstring filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test050/";
    FileTestUtils::createDir(filePath);
}

void doCreate060()
{
    wstring filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test060/";
    FileTestUtils::createDir(filePath);

    wstring fileName = L"binaryFile.dat";
    FileTestUtils::createNormalFile(filePath+fileName);
    FileTestUtils::writeDataInLoop(filePath+fileName, 56, 100);

    fileName = L"binaryFile2.dat";
    FileTestUtils::createNormalFile(filePath+fileName);
    FileTestUtils::writeDataInLoop(filePath+fileName, 50, 100);

    fileName = L"binaryFile3.dat";
    FileTestUtils::createNormalFile(filePath+fileName);
    FileTestUtils::writeDataInLoop(filePath+fileName, 50, 98);

    fileName = L"binaryFile4.dat";
    FileTestUtils::createNormalFile(filePath+fileName);
    FileTestUtils::writeDataInLoop(filePath+fileName, 50, 100);

    fileName = L"binaryFile5.dat";
    FileTestUtils::createNormalFile(filePath+fileName);
    FileTestUtils::writeDataInLoop(filePath+fileName, 50, 1000);

}


void doCreate070()
{
    wstring filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test070/";
    FileTestUtils::createDir(filePath);
}


void doCreate100()
{
    wstring filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test100/";
    FileTestUtils::createDir(filePath);

    wstring fileName = L"file100.txt";
    FileTestUtils::createNormalFile(filePath+fileName);
    FileTestUtils::writeDataInLoop(filePath+fileName, 4, 876);

    fileName = L"file100b.txt";
    FileTestUtils::createNormalFile(filePath+fileName);
    FileTestUtils::writeDataInLoop(filePath+fileName, 4, 50);
    FileTestUtils::setFileReadOnly(filePath+fileName);

    fileName = L"file100c.txt";
    FileTestUtils::createNormalFile(filePath+fileName);
    FileTestUtils::writeDataInLoop(filePath+fileName, 5, 400);
    FileTestUtils::setFileHidden(filePath, fileName);

    fileName = L"file100d.txt";
    FileTestUtils::createSystemFile(filePath+fileName);
    FileTestUtils::writeDataInLoop(filePath+fileName, 5, 100);

    wstring subDir = L"hiddenDir/";
    FileTestUtils::createHiddenDir(filePath, subDir);

    subDir = L"subDir/";
    FileTestUtils::createDir(filePath+subDir);

    fileName = L"file100e.txt";
    FileTestUtils::createNormalFile(filePath+subDir+fileName);
    FileTestUtils::writeDataInLoop(filePath+subDir+fileName, 5, 120);
}


void doCreate101()
{
    wstring filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test101/";
    FileTestUtils::createDir(filePath);

    wstring fileName = L"file101.txt";
    FileTestUtils::createNormalFile(filePath + fileName);
    FileTestUtils::writeDataInLoop(filePath+fileName, 4, 201);

    fileName = L"file101b.txt";
    FileTestUtils::createNormalFile(filePath+fileName);

    fileName = L"file101c.txt";
    FileTestUtils::createNormalFile(filePath+fileName);
    FileTestUtils::writeDataInLoop(filePath+fileName, 5, 101);
    FileTestUtils::setFileHidden(filePath, fileName);

    fileName = L"file101d.txt";
    FileTestUtils::createSystemFile(filePath +fileName);
    FileTestUtils::writeDataInLoop(filePath+fileName, 5, 101);
}

void doCreate102()
{
    wstring filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test102/";
    FileTestUtils::createDir(filePath);

    wstring fileName = L"file102.txt";
    FileTestUtils::createNormalFile(filePath + fileName);
    FileTestUtils::writeDataInLoop(filePath+fileName, 4, 101);

    fileName = L"file102b.txt";
    FileTestUtils::createNormalFile(filePath + fileName);
    FileTestUtils::writeDataInLoop(filePath+fileName, 4, 201);

    wstring subDir = L"subDir1/";
    FileTestUtils::createDir(filePath+subDir);

    fileName = L"file102e.txt";
    FileTestUtils::createNormalFile(filePath + subDir + fileName);
    FileTestUtils::writeDataInLoop(filePath+subDir+fileName, 5, 301);

    wstring subSubDir = L"subSubDir1/";
    FileTestUtils::createDir(filePath+subDir+subSubDir);

    fileName = L"file102f.txt";
    FileTestUtils::createNormalFile(filePath + subDir + subSubDir + fileName);
    FileTestUtils::writeDataInLoop(filePath+subDir+subSubDir+fileName, 5, 401);

    subDir = L"subDir2/";
    FileTestUtils::createDir(filePath+subDir);

    fileName = L"file102m.txt";
    FileTestUtils::createNormalFile(filePath+subDir+fileName);
    FileTestUtils::writeDataInLoop(filePath+subDir+fileName, 5, 301);

    subSubDir = L"subSubDir2/";
    FileTestUtils::createDir(filePath+subDir+subSubDir);

    fileName = L"file102k.txt";
    FileTestUtils::createNormalFile(filePath+subDir+subSubDir+fileName);
    FileTestUtils::writeDataInLoop(filePath+subDir+subSubDir+fileName, 5, 401);

    subDir = L"subDir3/";
    FileTestUtils::createDir(filePath+subDir);

    fileName = L"file102r.txt";
    FileTestUtils::createNormalFile(filePath+subDir+fileName);
    FileTestUtils::writeDataInLoop(filePath+subDir+fileName, 5, 601);
}

void doCreate110()
{
    wstring filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test110/";
    FileTestUtils::createDir(filePath);
}

void doCreate160()
{
    wstring filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test160/";
    FileTestUtils::createDir(filePath);

    wstring fileName = L"writable.txt";
    FileTestUtils::createWriteOnlyFile(filePath+fileName);

    fileName = L"writable2.txt";
    FileTestUtils::createWriteOnlyFile(filePath+fileName);

    wstring subDir = L"writableDir/";
    FileTestUtils::createDir(filePath+subDir);

    fileName = L"binaryFile.txt";
    FileTestUtils::createNormalFile(filePath+fileName);
    FileTestUtils::writeDataInLoop(filePath+fileName, 56, 1025);
    FileTestUtils::setFileReadOnly(filePath+fileName);

    fileName = L"binaryFile2.txt";
    FileTestUtils::createNormalFile(filePath+fileName);
    FileTestUtils::writeDataInLoop(filePath+fileName, 56, 1025);
    FileTestUtils::setFileReadOnly(filePath+fileName);
}

void doCreate165()
{
    wstring filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"Data/videos/myDir/";
    FileTestUtils::createDir(filePath);

    wstring fileName = L"file";
    FileTestUtils::createNormalFile(filePath+fileName);
}

void doCreate170()
{
    wstring filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test170/";
    FileTestUtils::createDir(filePath);

    wstring fileName = L"hiddenFile.txt";
    FileTestUtils::createHiddenFile(filePath, fileName);

    fileName = L"notHiddenFile.txt";
    FileTestUtils::createNormalFile(filePath+fileName);

    wstring subDir = L"hiddenDir/";
    FileTestUtils::createHiddenDir(filePath, subDir);

    fileName = L"readOnly.txt";
    FileTestUtils::createNormalFile(filePath+fileName);
    FileTestUtils::writeDataInLoop(filePath+fileName, 56, 1600);
    FileTestUtils::setFileReadOnly(filePath + fileName);
}

void doCreate180()
{
    wstring filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test180/";
    FileTestUtils::createDir(filePath);
}


void doCreate181()
{
    wstring filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test181/";
    FileTestUtils::createDir(filePath);

    wstring fileName;
    fileName = L"päå.ööö";
    FileTestUtils::createNormalFile(filePath+fileName);

    fileName = L"someData";
    FileTestUtils::createNormalFile(filePath+fileName);

    wstring subDir = L"pää/";
    FileTestUtils::createDir(filePath+subDir);
}

void doCreate191()
{
    wstring filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test191/";
    FileTestUtils::createDir(filePath);

    wstring fileName = L"temp.txt";
    FileTestUtils::createSystemFile(filePath+fileName);

    fileName = L"tempo.java";
    FileTestUtils::createNormalFile(filePath+fileName);

    fileName = L"tempor.cxx";
    FileTestUtils::createReadOnlyFile(filePath+fileName);
    FileTestUtils::setFileReadOnly(filePath+fileName);

    fileName = L"txmp.cxx";
    FileTestUtils::createNormalFile(filePath+fileName);

    fileName = L"temp.cpp";
    FileTestUtils::createHiddenFile(filePath,fileName);

    wstring subDir = L"tempora/";
    FileTestUtils::createDir(filePath+subDir);

    subDir = L"txmp";
    FileTestUtils::createDir(filePath+subDir);

    subDir = L"tempg";
    FileTestUtils::createHiddenDir(filePath, subDir);

    subDir = L"temporar";
    FileTestUtils::createDir(filePath+subDir);
}

void doCreate192()
{
    wstring filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test192/";
    FileTestUtils::createDir(filePath);

    wstring fileName = L"temp.txt";
    FileTestUtils::createNormalFile(filePath+fileName);

    fileName = L"tempo.txt";
    FileTestUtils::createNormalFile(filePath+fileName);

    fileName = L"tempor.cxx";
    FileTestUtils::createReadOnlyFile(filePath+fileName);

    fileName = L"txmp.cxx";
    FileTestUtils::createNormalFile(filePath+fileName);

    fileName = L"tmp.txt";
    FileTestUtils::createHiddenFile(filePath,fileName);

    wstring subDir = L"tempora/";
    FileTestUtils::createDir(filePath+subDir);

    subDir = L"txmp";
    FileTestUtils::createDir(filePath+subDir);

    subDir = L"tempg";
    FileTestUtils::createHiddenDir(filePath, subDir);

    subDir = L"temporar";
    FileTestUtils::createDir(filePath+subDir);
}

void doCreate194()
{
    wstring filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"Data/videok/";
    FileTestUtils::createDir(filePath);
}


void doCreate200()
{
    wstring filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test200/";
    FileTestUtils::createDir(filePath);

    wstring subDir = L"readOnlyDir/";
    FileTestUtils::createReadOnlyDir(filePath+subDir);

    wstring fileName = L"ExistFile202.txt";
    FileTestUtils::createNormalFile(filePath+fileName);

    fileName = L"ReadOnlyFile202.txt";
    FileTestUtils::createReadOnlyFile(filePath+fileName);
    FileTestUtils::setFileReadOnly(filePath+fileName);
}

void doCreate210()
{
    wstring filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test210/";
    FileTestUtils::createDir(filePath);
}


void doCreate220()
{
    wstring filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test220/";
    FileTestUtils::createDir(filePath);

    wstring fileName = L"ReadOnlyFile220.txt";
    FileTestUtils::createReadOnlyFile(filePath+fileName);
    FileTestUtils::setFileReadOnly(filePath+fileName);
}

void doCreate240()
{
    wstring filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test240/";
    FileTestUtils::createDir(filePath);

    wstring fileName = L"file";
    FileTestUtils::createNormalFile(filePath+fileName);
    FileTestUtils::writeDataInLoop(filePath+fileName, 65, 1600);

    wstring subDir = L"readOnlyDir";
    FileTestUtils::createReadOnlyDir(filePath+subDir);

    fileName = L"readOnlyFile";
    FileTestUtils::createReadOnlyFile(filePath+fileName);
    FileTestUtils::setFileReadOnly(filePath+fileName);

    subDir = L"systemDir";
    FileTestUtils::createSystemDir(filePath+subDir);

    fileName = L"systemFile";
    FileTestUtils::createSystemFile(filePath+fileName);
}

void doCreate250()
{
    wstring filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test250/";
    FileTestUtils::createDir(filePath);

    wstring fileName = L"normalFile.txt";
    FileTestUtils::createNormalFile(filePath+fileName);
    FileTestUtils::writeDataInLoop(filePath+fileName, 54, 99);

    fileName = L"file253.txt";
    FileTestUtils::createNormalFile(filePath+fileName);
    FileTestUtils::writeDataInLoop(filePath+fileName, 54, 99);

    fileName = L"file253b.txt";
    FileTestUtils::createNormalFile(filePath+fileName);
    FileTestUtils::writeDataInLoop(filePath+fileName, 77, 700);

    wstring subDir = L"temp/";
    FileTestUtils::createDir(filePath+subDir);

    fileName = L"temp.txt";
    FileTestUtils::createNormalFile(filePath+subDir+fileName);

    fileName = L"tempo.txt";
    FileTestUtils::createNormalFile(filePath+subDir+fileName);

    subDir = L"readOnlyDir/";
    FileTestUtils::createReadOnlyDir(filePath+subDir);

    fileName = L"readOnlyFile";
    FileTestUtils::createNormalFile(filePath+fileName);
    FileTestUtils::writeDataInLoop(filePath+fileName, 77, 1200);
    FileTestUtils::setFileReadOnly(filePath+fileName);
}

void doCreate260()
{
    wstring filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test260/";
    FileTestUtils::createDir(filePath);

    wstring fileName = L"temp.txt";
    FileTestUtils::createNormalFile(filePath+fileName);

    fileName = L"file260.txt";
    FileTestUtils::createNormalFile(filePath+fileName);
    FileTestUtils::writeDataInLoop(filePath+fileName, 54, 101);

    fileName = L"file260b.txt";
    FileTestUtils::createNormalFile(filePath+fileName);
    FileTestUtils::writeDataInLoop(filePath+fileName, 56, 101);

    fileName = L"file260c.txt";
    FileTestUtils::createNormalFile(filePath+fileName);
    FileTestUtils::writeDataInLoop(filePath+fileName, 56, 10);

    fileName = L"file260d.txt";
    FileTestUtils::createNormalFile(filePath+fileName);
    FileTestUtils::writeDataInLoop(filePath+fileName, 56, 50);

    fileName = L"file260e.txt";
    FileTestUtils::createNormalFile(filePath+fileName);
    FileTestUtils::writeDataInLoop(filePath+fileName, 56, 50);

    fileName = L"file262.txt";
    FileTestUtils::createNormalFile(filePath+fileName);
    FileTestUtils::writeDataInLoop(filePath+fileName, 56, 2000);
}

void doCreate270()
{
    wstring filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test270/";
    FileTestUtils::createDir(filePath);

    wstring fileName = L"My%20Doc.txt";
    FileTestUtils::createNormalFile(filePath+fileName);
}

void doCreate273()
{
    wstring filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test273/";
    FileTestUtils::createDir(filePath);

    wstring fileName = L"binaryFile";
    FileTestUtils::createNormalFile(filePath+fileName);
    FileTestUtils::writeDataInLoop(filePath+fileName, 56, 100);
    FileTestUtils::setFileReadOnly(filePath+fileName);
}

void doCreate275()
{
    wstring filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"Data/images/myDir/";
    FileTestUtils::createReadOnlyDir(filePath);

    wstring fileName = L"something.jar";
    FileTestUtils::createNormalFile(filePath+fileName);

    fileName = L"something.jad";
    FileTestUtils::createNormalFile(filePath+fileName);

    fileName = L"something.jad.jad";
    FileTestUtils::createNormalFile(filePath+fileName);

    wstring subDir = L"myImages/";
    FileTestUtils::createDir(filePath+subDir);

    subDir = L"myImages2/";
    FileTestUtils::createDir(filePath+subDir);

    fileName = L"myImagesFile.txt";
    FileTestUtils::createNormalFile(filePath+subDir+fileName);
    FileTestUtils::writeDataInLoop(filePath+fileName, 56, 999);

    //Set to hidden at the end cause in linux, dir name will change :-(
    filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"Data/images/";
    FileTestUtils::setDirHidden(filePath, L"myDir/");
}

void doCreate280()
{
    wstring filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test280/";
    FileTestUtils::createDir(filePath);

    wstring subDir = L"new dir/";
    FileTestUtils::createDir(filePath+subDir);

    wstring fileName = L"file.txt";
    FileTestUtils::createNormalFile(filePath+subDir+fileName);

    subDir = L"te mp/";
    FileTestUtils::createDir(filePath+subDir);

    fileName = L"some file";
    FileTestUtils::createNormalFile(filePath+subDir+fileName);

    fileName = L"tääfile";
    FileTestUtils::createNormalFile(filePath+subDir+fileName);
}

void doCreate291()
{
    wstring filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test291/";
    FileTestUtils::createDir(filePath);
}

void doCreate330()
{
    wstring filePath = FileTestConsts::KDefaultRootPath;
    filePath += L"mainTestDir/test330/";
    FileTestUtils::createDir(filePath);

    wstring fileName = L"readFile.txt";
    FileTestUtils::createNormalFile(filePath+fileName);
    FileTestUtils::writeDataInLoop(filePath+fileName, 75, 10);
    FileTestUtils::writeDataInLoop(filePath+fileName, 76, 10);

    fileName = L"readFile2.txt";
    FileTestUtils::createNormalFile(filePath+fileName);
    FileTestUtils::writeDataInLoop(filePath+fileName, 75, 5);
    FileTestUtils::writeDataInLoop(filePath+fileName, 76, 10);

    fileName = L"readFile3.txt";
    FileTestUtils::createNormalFile(filePath+fileName);
    FileTestUtils::writeDataInLoop(filePath+fileName, 75, 700);
    FileTestUtils::writeDataInLoop(filePath+fileName, 76, 300);
}

void doCreateSpecialFiles()
{

}



int main(int argc, char** argv)
{
    doCreateMainDir();
    doCreate000();
    doCreate020();
    doCreate040();
    doCreate050();
    doCreate060();
    doCreate070();
    doCreate100();
    doCreate101();
    doCreate102();
    doCreate110();
    doCreate160();
    doCreate170();
    doCreate180();
    doCreate181();
    doCreate191();
    doCreate192();
    doCreate200();
    doCreate210();
    doCreate220();
    doCreate240();
    doCreate250();
    doCreate260();
    doCreate270();
    doCreate273();
    doCreate280();
    doCreate291();
    doCreate330();

//  if(FileTestConsts::KIncludeDRMInits)
//  {
//      FileTestUtils::doInitDRMContents();
//  }

//Call in the end. Might not work on Linux
    doCreate165();
    doCreate194();
    doCreate275();
    printf("File Tests Init: Successful\n");
}
