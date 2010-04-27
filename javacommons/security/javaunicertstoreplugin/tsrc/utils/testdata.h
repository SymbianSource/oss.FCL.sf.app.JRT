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

#ifndef TESTDATA_H
#define TESTDATA_H

#include <f32file.h>
#include <string>
#include <list>

_LIT(KCertMetaDataRootDir,"c:\\private\\200211dc\\security\\trustroots\\device\\state\\");
const char CertMetaDataRootDir[] = "c:\\private\\200211dc\\security\\trustroots\\device\\state\\";

_LIT(KCertDataRootDir,"c:\\private\\200211dc\\security\\trustroots\\device\\certificates\\");
const char CertDataRootDir[] = "c:/private/200211dc/security/trustroots/device/certificates/";

//const int DISABLED      = 0x1;
//const int CAN_DISABLED  = 0x2;
//const int CAN_DELETED   = 0x4;

const int STATE_ENABLED = 3;
const int STATE_DISABLED = 2;
const int STATE_DELETED = 1;

class CertMetaData
{
public:

    CertMetaData(int aState,const std::string& aNameOfCert,TBool aRemovable,TBool aDisablable,
                 const std::string& aHash,const std::string& aCertContent)
            : mState(aState),mNameOfCert(aNameOfCert),mRemovable(aRemovable),mDisablable(aDisablable),
            mHash(aHash),mCertContent(aCertContent) {}

    CertMetaData(const CertMetaData& x)
    {
        *this = x;
    }

    CertMetaData& operator=(const CertMetaData& x)
    {
        mState = x.mState;
        mNameOfCert = x.mNameOfCert;
        mRemovable = x.mRemovable;
        mDisablable = x.mDisablable;
        mHash = x.mHash;
        mCertContent = x.mCertContent;
        return *this;
    }

    int mState;
    std::string mNameOfCert;
    bool mRemovable;
    bool mDisablable;
    std::string mHash;
    std::string mCertContent;
};

class CTestData
{
public:

    static CTestData* NewL();

    virtual ~CTestData();

    void DeleteTestDataL();

    void StoreTestData(std::list<CertMetaData>& aTestData);

private:

    RFs mRfs;

    CTestData();
    void ConstructL();

};


#endif // TESTDATA_H
