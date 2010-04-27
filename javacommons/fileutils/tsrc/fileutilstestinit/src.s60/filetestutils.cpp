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


#include <f32file.h>
#include <fstream>
#include <iostream>

#include <DRMCommon.h>
#include <DRMMessageParser.h>
#include <DRMRightsClient.h>

#include "filetestutils.h"

using namespace std;
using namespace java::filetest;

void FileTestUtils::createFile(const wstring &aPath, int aFlags, int aMode)
{
    int fd = wopen(aPath.c_str(), aFlags, aMode);
    FileTestUtils::checkError(aPath,fd);
    close(fd);
}



HBufC* stringToDes(const wchar_t* str)
{
    HBufC* resultBuf = 0;
    try
    {
        if (str)
        {
            int len = wcslen(str);
            resultBuf = HBufC::New(len + 1);
            TPtr ptr = resultBuf->Des();

            TPtr16 ptr16((TUint16 *)str, len);
            ptr16.SetLength(len);
            ptr.Copy(ptr16);
            ptr.ZeroTerminate();
        }
    }
    catch (...)
    {

    }
    return resultBuf;
}


/**
 *
 */
void ReplaceCharacters(TPtr& aPtr,TUint8 aReplacedChar,
                       TUint8 aNewChar,TBool aOnlyFirstMatch)
{
    for (TInt i = 0; i < aPtr.Length(); ++i)
    {
        if (aReplacedChar == aPtr[i])
        {
            aPtr[i] = aNewChar;
            if (aOnlyFirstMatch)
                return;
        }
    }
}

void ReplaceCharacters(TPtr& aPtr,TUint8 aReplacedChar,
                       TUint8 aNewChar)
{
    ReplaceCharacters(aPtr,aReplacedChar,aNewChar,EFalse);
}



void FileTestUtils::createSystemFile(const wstring &aPath)
{
    FileTestUtils::createNormalFile(aPath);
    wchar_t* filePtr = const_cast<wchar_t*>(aPath.c_str());
    HBufC* tmpBuf = stringToDes(filePtr);
    TPtr tmpBufPtr(tmpBuf->Des());
    ReplaceCharacters(tmpBufPtr,'/','\\');

    RFs aRFs;
    aRFs.Connect();
    int err = aRFs.SetAtt(*tmpBuf,KEntryAttSystem,0);
    aRFs.Close();
    //Donot know what to do in linux
    checkError(aPath, err);
}

void FileTestUtils::createHiddenDir(const wstring &aPath, const wstring& aFile)
{
    FileTestUtils::createDir(aPath+aFile);

    wstring fileName = (aPath+aFile);

    wchar_t* filePtr = const_cast<wchar_t*>(fileName.c_str());
    HBufC* tmpBuf = stringToDes(filePtr);
    TPtr tmpBufPtr(tmpBuf->Des());
    ReplaceCharacters(tmpBufPtr,'/','\\');

    RFs aRFs;
    aRFs.Connect();
    int error = aRFs.SetAtt(*tmpBuf ,KEntryAttHidden,0);
    aRFs.Close();
    checkError(aPath+aFile, error);
}

void FileTestUtils::createSystemDir(const wstring &aPath)
{
    FileTestUtils::createDir(aPath);
    wchar_t* filePtr = const_cast<wchar_t*>(aPath.c_str());
    HBufC* tmpBuf = stringToDes(filePtr);
    TPtr tmpBufPtr(tmpBuf->Des());
    ReplaceCharacters(tmpBufPtr,'/','\\');

    RFs aRFs;
    aRFs.Connect();
    int err = aRFs.SetAtt(*tmpBuf,KEntryAttSystem,0);
    aRFs.Close();
    //Donot know what to do in linux
    checkError(aPath, err);
}


void FileTestUtils::createDirMode(const wstring& aPath, const int aMode)
{
    // R W X
    // 4 2 1
    int error = wmkdir(aPath.c_str(), aMode);
    FileTestUtils::checkError(aPath, error);
}

void FileTestUtils::writeDataToFile(const wstring& aPath, const string& aData)
{
    //Linux, just use ofstream
    int fd = wopen(aPath.c_str(), O_RDWR);

    checkError(aPath, fd);
    write(fd, aData.c_str(), aData.length());
    close(fd);
}

void FileTestUtils::writeDataInLoop(const wstring& aPath, const int aData, const int aTimes)
{
    char* chr = new char[aPath.size()+1];
    sprintf(chr,"%ls",aPath.c_str());
    wofstream file(chr, ios::out|ios::app|ios::binary);

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
    wofstream file(chr, ios::out|ios::app|ios::binary);
    file<<(char)aData;
    file.close();
}

void FileTestUtils::appendDataToFile(const wstring& aPath, const wstring& aData)
{
    char* chr = new char[aPath.size()+1];
    sprintf(chr,"%ls",aPath.c_str());
    wofstream file(chr, ios::out|ios::app|ios::binary);
    file<<(char*)aData.c_str();
    file.close();
}

void FileTestUtils::setFileHidden(const wstring &aPath, const wstring &aFile)
{
    wstring fileName = (aPath+aFile);

    wchar_t* filePtr = const_cast<wchar_t*>(fileName.c_str());
    HBufC* tmpBuf = stringToDes(filePtr);
    TPtr tmpBufPtr(tmpBuf->Des());
    ReplaceCharacters(tmpBufPtr,'/','\\');

    RFs aRFs;
    aRFs.Connect();
    int error = aRFs.SetAtt(*tmpBuf ,KEntryAttHidden,0);
    aRFs.Close();
    checkError(aPath+aFile, error);
}

void FileTestUtils::setFileReadOnly(const wstring &aPath)
{
    wchmod(aPath.c_str(), 0444);
}

void FileTestUtils::setDirHidden(const wstring &aPath, const wstring &aName)
{
    wstring fileName = (aPath+aName);

    wchar_t* filePtr = const_cast<wchar_t*>(fileName.c_str());
    HBufC* tmpBuf = stringToDes(filePtr);
    TPtr tmpBufPtr(tmpBuf->Des());
    ReplaceCharacters(tmpBufPtr,'/','\\');

    RFs aRFs;
    aRFs.Connect();
    int error = aRFs.SetAtt(*tmpBuf ,KEntryAttHidden,0);
    aRFs.Close();
    checkError(aPath+aName, error);
}


/*******************************************************************************


DRM Methods: Specific to S60

******************************************************************************/

void InitDrFile(RFs& aRfs,const TDesC& aFileName)
{
    //_LIT( KTempFile, "c:\\mainTestDir\\readme.dr");
    TInt size;
    RFile file;
    //CDRMRights* rightsDetail = NULL;
    RPointerArray<CDRMRights> rightsDetails;

    //DRMAuthenticated *drm = DRMAuthenticated::NewL();
    CDRMMessageParser* msgParser = CDRMMessageParser::NewL();
    CleanupStack::PushL(msgParser);

    User::LeaveIfError(file.Open(aRfs,aFileName,EFileRead));
    User::LeaveIfError(file.Size(size));
    HBufC8* buffer = HBufC8::NewLC(size);
    TPtr8 ptr(buffer->Des());

    User::LeaveIfError(file.Read(ptr, size));
    CleanupStack::Pop(buffer);

    User::LeaveIfError(msgParser->ProcessRightsObject(ptr, rightsDetails));

    delete buffer;
    //delete rightsDetail;
    rightsDetails.ResetAndDestroy();

    CleanupStack::PopAndDestroy(); // drm
    file.Close();
}



//We will have to unzip winscw.zip in ..\doc folder to \epoc32\winscw\ for this
//to work
void FileTestUtils::doInitDRMContents()
{
    RFs aRFs;
    aRFs.Connect();
    RDRMRightsClient rightsClient;
    User::LeaveIfError(rightsClient.Connect());
    TInt err = rightsClient.DeleteAll();
    rightsClient.Close();
    User::LeaveIfError(err);

    //InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile002DecExecuteCount2.dr"));
    //InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\readme2.dr"));

    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile001PlayPreview.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile001DisplayPreview.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile001PrintPreview.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile001ExecutePreview.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile002DecDisplayCount2.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile002DecPlayCount2.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile002DecPrintCount2.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile002DecExecuteCount2.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile002EncCount1.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile003_01DecPlayCount2.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile003_02DecPlayCount2.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile003_03DecPlayCount2.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile003_04DecPlayCount50.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile004DecPlayCount1.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile006_6DecPlayCount1.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile006_7DecPlayCount10.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile006DecPlayCount1.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile007DecPlayCount2.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile008DecPlayCount2.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile009DecPlayCount1.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile010DecPlayCount2.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile011DecPlayCount4.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile012DecPlayCount4.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile013DecPlayCount1.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile014DecPlayCount2.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile015DecPlayCount3.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\testDir001\\drmfile015_001DecPlayCount3.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile016DecPlayCount2.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile017DecPlayCount10.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile018DecPlayCount10.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile019DecPlayCount2.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile020DecPlayCount3.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile021DecPlayCount2.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile022DecPlayCount2.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile023DecPlayCount10.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile024DecPlayCount50.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile024DecDisplayCount50.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile024DecPrintCount50.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile024DecExecuteCount50.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Videos\\drmFiles\\drmfile025DecPlayCount3.dr"));
    InitDrFile(aRFs,_L("c:\\data\\Images\\myDir\\drmfile026DecPlayCount50.dr"));

    InitDrFile(aRFs,_L("e:\\drmfile003DecPlayCount1.dr"));
    InitDrFile(aRFs,_L("e:\\drmfile006_61DecPlayCount1.dr"));
    InitDrFile(aRFs,_L("e:\\drmfile025DecPrintCount50.dr"));
    InitDrFile(aRFs,_L("e:\\drmfile003_05DecPlayCount50.dr"));
}
