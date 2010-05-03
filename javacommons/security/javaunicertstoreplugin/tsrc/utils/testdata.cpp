/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include <memory>
#include <fstream>
#include "testdata.h"
#include "javacommonutils.h"

_LIT(KStar,"*");

using namespace std;
using namespace java::util;

CTestData::CTestData()
{
}

CTestData::~CTestData()
{
    mRfs.Close();
}

CTestData* CTestData::NewL()
{
    CTestData* self = new(ELeave) CTestData();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

void CTestData::ConstructL()
{
    User::LeaveIfError(mRfs.Connect());
}

void CTestData::DeleteTestDataL()
{
    std::auto_ptr<CFileMan> fileMan(CFileMan::NewL(mRfs));
    auto_ptr<HBufC> workDirBuf(HBufC::NewL(KCertMetaDataRootDir().Length()+2));
    TPtr workDirPtr(workDirBuf->Des());
    workDirPtr.Append(KCertMetaDataRootDir());
    workDirPtr.Append(KStar());
    TInt err = fileMan->Delete(workDirBuf->Des());
    if ((KErrNone != err) && (KErrNotFound != err) && (KErrPathNotFound != err))
    {
        User::Leave(err);
    }
    auto_ptr<HBufC> workDirBuf1(HBufC::NewL(KCertDataRootDir().Length()+2));
    TPtr workDirPtr1(workDirBuf1->Des());
    workDirPtr1.Append(KCertDataRootDir());
    workDirPtr1.Append(KStar());
    err = fileMan->Delete(workDirBuf1->Des());
    if ((KErrNone != err) && (KErrNotFound != err) && (KErrPathNotFound != err))
    {
        User::Leave(err);
    }
}

void CTestData::StoreTestData(std::list<CertMetaData>& aTestData)
{
    RFs rfs;
    rfs.Connect();
    rfs.MkDirAll(KCertMetaDataRootDir);
    rfs.MkDirAll(KCertDataRootDir);

    std::list<CertMetaData>::iterator iter = aTestData.begin();
    int index = 0;
    for (; iter != aTestData.end(); ++iter)
    {
        std::string fileName(CertDataRootDir);
        fileName.append("cert");
        fileName.append(JavaCommonUtils::intToString(index));
        string certFileNameWithoutSuffix(fileName);
        //string certMetadata(fileName);
        fileName.append(".metadata");
        ofstream outfile(fileName.c_str());
        outfile << "name=" << iter->mNameOfCert.c_str() << endl;
        outfile << "category=ITPD" << endl;
        int removable = 0;
        if (true == iter->mRemovable)
            removable = 1;
        int disablable = 0;
        if (true == iter->mDisablable)
            disablable = 1;
        outfile << "removable=" << removable << endl;
        outfile << "disablable=" << disablable << endl;
        outfile << "hash=" << iter->mHash.c_str() << endl;
        outfile.close();

        std::string certContent(certFileNameWithoutSuffix);
        certContent.append(".der");
        ofstream outfile2(certContent.c_str());
        outfile2 << iter->mCertContent.c_str() << endl;
        outfile2.close();

        //std::string certState(certFileNameWithoutSuffix);
        fileName = CertMetaDataRootDir;
        fileName.append("cert");
        fileName.append(JavaCommonUtils::intToString(index));
        fileName.append(".state");
        int len = fileName.size();
        HBufC* filePath = HBufC::NewL(len);
        CleanupStack::PushL(filePath);
        TPtr ptr = filePath->Des();
        TPtr8 ptr8((unsigned char *)fileName.c_str(),len);
        ptr8.SetLength(len);
        ptr.Copy(ptr8);
        RFile stateFile;
        int err = stateFile.Create(rfs, ptr, EFileWrite);
        if (err == KErrNone)
        {
            CleanupClosePushL(stateFile);
            TPckgBuf<TUint32> state(iter->mState);
            stateFile.Write(0, state);
            CleanupStack::PopAndDestroy();  // state file
        }
        CleanupStack::PopAndDestroy(); // file path
        index++;
    }//end for
    rfs.Close();
}

