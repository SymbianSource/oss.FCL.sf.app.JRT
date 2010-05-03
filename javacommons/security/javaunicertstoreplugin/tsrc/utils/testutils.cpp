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

#include <spawn.h>
#include <memory>
#include <stdlib.h>
#include <ccertattributefilter.h>
#include "testutils.h"
#include "testfailedexception.h"
#include "logger.h"

_LIT(KMIDP2TrustRoot, "Java Trust Root");

bool TestUtils::sCaptainRunning = false;

void tbufC8tochar(TDesC8 aSrc, char* aDes)
{
    char *temp    = (char*)aSrc.Ptr();
    const TInt size       = aSrc.Length();
    *(temp + size)    = '\0';
    strcpy(aDes, temp);
}

void CheckCertInfo(std::list<CertMetaData>& aExpected,CCTCertInfo* aInfo)
{
    std::list<CertMetaData>::iterator iter = aExpected.begin();
    bool metaDataObjFound = false;
    for (; iter != aExpected.end(); ++iter)
    {
        int expectedSize = iter->mCertContent.size();
        //'-1' is decreased because Windows file system adds one extra byte to the file size.
        int actualSize = aInfo->Size() - 1;
        if (expectedSize == actualSize)
        {
            metaDataObjFound = true;
            if (EX509Certificate != aInfo->CertificateFormat())
            {
                throw TestFailedException("Incorrect CertificateFormat",
                                          __FILE__,__FUNCTION__,__LINE__);
            }
            if (ECACertificate != aInfo->CertificateOwnerType())
            {
                throw TestFailedException("Incorrect CertificateOwnerType",
                                          __FILE__,__FUNCTION__,__LINE__);
            }
            if (iter->mRemovable != aInfo->IsDeletable())
            {
                throw TestFailedException("Incorrect IsDeletable value",
                                          __FILE__,__FUNCTION__,__LINE__);
            }
            break;
        }//end if(expectedSize == actualSize)
    }//end for
    if (false == metaDataObjFound)
    {
        throw TestFailedException("Corresponding CertMetaData obj was not found",
                                  __FILE__,__FUNCTION__,__LINE__);
    }
}

int TestUtils::StartJavaCaptain()
{
    if (true == sCaptainRunning)
    {
        return 0;
    }
    int rc = 0;
    int pid = 0;
    char* av[2];
    int index = 0;
    av[index++] = "javacaptain";
    av[index] = NULL;

#ifdef __SYMBIAN32__
    rc = posix_spawn(&pid, "javacaptain", NULL, NULL, av, NULL);
#else
    if (!(pid = fork()))
    {
        rc = execvp("javacaptain", av);
        if (rc == -1)
        {
            rc = errno;
        }
    }
#endif // __SYMBIAN32__

    if (rc)
    {
        ELOG3(EJavaSecurity,"%s failed, %s - errno=%d", __PRETTY_FUNCTION__, strerror(rc), rc);
    }
    sCaptainRunning = true;
    return rc;
}

CCertAttributeFilter* TestUtils::GetFilterObj()
{
    std::auto_ptr< CCertAttributeFilter> retObj(CCertAttributeFilter::NewL());
    TUid uid = TUid::Uid(0x101F9B28);
    retObj->SetUid(uid);
    retObj->SetFormat(EX509Certificate);
    retObj->SetOwnerType(ECACertificate);
    retObj->SetLabel(KMIDP2TrustRoot());
    return retObj.release();
}

void TestUtils::CheckResultOfList(std::list<CertMetaData>& aExpected,
                                  RMPointerArray<CCTCertInfo>& aResult)
{
    for (TInt i = 0; i < aResult.Count(); i++)
    {
        CheckCertInfo(aExpected,aResult[i]);
    }
}

void TestUtils::CheckContentOfCert(std::list<CertMetaData>& aExpected,
                                   HBufC8* aContent)
{
    std::list<CertMetaData>::iterator iter = aExpected.begin();
    bool metaDataObjFound = false;
    char* tmpChar = new char[aContent->Length()+1];
    tbufC8tochar(aContent->Des(),tmpChar);
    std::string returnedContent(tmpChar);
    delete [] tmpChar;
    for (; iter != aExpected.end(); ++iter)
    {
        if (returnedContent.size() == iter->mCertContent.size())
        {
            metaDataObjFound = true;
            if (0 == returnedContent.compare(iter->mCertContent))
            {
                throw TestFailedException("Incorrect cert content",
                                          __FILE__,__FUNCTION__,__LINE__);
            }
            break;
        }
    }//end for
    if (false == metaDataObjFound)
    {
        throw TestFailedException("Corresponding CertMetaData obj was not found",
                                  __FILE__,__FUNCTION__,__LINE__);
    }
}

