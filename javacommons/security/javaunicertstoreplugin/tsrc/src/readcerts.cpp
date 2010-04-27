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

#include <stdio.h>
#include <string>
#include <memory>
#include <unistd.h>
#include <e32cmn.h>
#include <e32des8.h>

#include "TestHarness.h"
#include "testexecutor.h"
#include "securitycommsmessagedefs.h"
#include "testdata.h"
#include "testutils.h"
#include "javacertstoretokentype.h"
#include "javacertstoretoken.h"
#include "javacertstoreimpl.h"
#include "testfailedexception.h"
#include "commsmsgsender.h"

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <securitydefs.h>
#else
#include <securitydefs.h>
#include <securitydefsconst.h>
#endif

using namespace std;
using namespace java::security;

_LIT(KMIDP2TrustRoot, "Java Trust Root");

TEST_GROUP(ReadCerts)
{
    TEST_SETUP()
    {
    }

    TEST_TEARDOWN()
    {
    }
};

//
// TEST CASE 1
//
TEST(ReadCerts,readingCerts)
{
    printf("+readingCerts\n");

    try
    {
        //Starting java captain.
        TestUtils::StartJavaCaptain();

        //Stopping javacertstore extension plugin of the java captain.
        CommsMsgSender commsMsgSender;
        commsMsgSender.SendStopJavaCertStorePlugin();

        //Deleting all existing
        auto_ptr<CTestData> testData(CTestData::NewL());
        testData->DeleteTestDataL();

        //# 1
        //Create certificates + metadata.
        list<CertMetaData> listOfCerts;
        {
            CertMetaData tmpObj(STATE_ENABLED,"enabledcert1",false,true,"1","content1");
            listOfCerts.push_back(tmpObj);
        }
        {
            CertMetaData tmpObj(STATE_ENABLED,"enabledcert2",true,true,"2","content12");
            listOfCerts.push_back(tmpObj);
        }
        {
            CertMetaData tmpObj(STATE_DISABLED,"disabledcert1",true,true,"3","content123");
            listOfCerts.push_back(tmpObj);
        }
        {
            CertMetaData tmpObj(STATE_DELETED,"deletedcert1",true,true,"4","content1234");
            listOfCerts.push_back(tmpObj);
        }
        testData->StoreTestData(listOfCerts);

        auto_ptr<CTestExecutor> testExecutor(new CTestExecutor());
        auto_ptr<CJavaCertStoreTokenType> certStoreTokenType
        (static_cast<CJavaCertStoreTokenType*>(CJavaCertStoreTokenType::New2L()));
        auto_ptr<CJavaCertStoreToken> certStoreToken
        (CJavaCertStoreToken::New(0x1234,*certStoreTokenType.get()));
        auto_ptr<CJavaCertStoreImpl> certStoreObj(CJavaCertStoreImpl::NewL(*certStoreToken.get()));

        //# 2
        //Reading certificates.
        {
            RMPointerArray<CCTCertInfo> certInfoArray;
            auto_ptr<CCertAttributeFilter> attrFilter(TestUtils::GetFilterObj());
            certStoreObj->List(certInfoArray,*attrFilter.get(),testExecutor->GetRequestStatus());
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());

            CHECK(3 == certInfoArray.Count());
            TestUtils::CheckResultOfList(listOfCerts,certInfoArray);

            for (TInt i = 0; i < certInfoArray.Count(); i++)
            {
                auto_ptr<HBufC8> certContentBuf(HBufC8::NewL(certInfoArray[i]->Size()));
                TPtr8 certContentPtr(certContentBuf->Des());
                certStoreObj->Retrieve(*certInfoArray[i],certContentPtr,testExecutor->GetRequestStatus());
                testExecutor->Activate();
                CActiveScheduler::Start();
                CHECK(KErrNone == testExecutor->GetErrorCode());
                TestUtils::CheckContentOfCert(listOfCerts,certContentBuf.get());
            }
            certInfoArray.Close();
        }

        //# 3
        //Recall list() operation to check that objects are returned correctly from
        //cache memory.
        {
            RMPointerArray<CCTCertInfo> certInfoArray;
            auto_ptr<CCertAttributeFilter> attrFilter(TestUtils::GetFilterObj());
            certStoreObj->List(certInfoArray,*attrFilter.get(),testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());

            CHECK(3 == certInfoArray.Count());
            TestUtils::CheckResultOfList(listOfCerts,certInfoArray);

            for (TInt i = 0; i < certInfoArray.Count(); i++)
            {
                auto_ptr<HBufC8> certContentBuf(HBufC8::NewL(certInfoArray[i]->Size()));
                TPtr8 certContentPtr(certContentBuf->Des());
                certStoreObj->Retrieve(*certInfoArray[i],certContentPtr,testExecutor->GetRequestStatus());
                testExecutor->Activate();
                CActiveScheduler::Start();
                CHECK(KErrNone == testExecutor->GetErrorCode());
                TestUtils::CheckContentOfCert(listOfCerts,certContentBuf.get());
            }
            certInfoArray.Close();
        }

        //# 4
        //Reading certificates in the situation when certificate folder is empty.
        {
            CommsMsgSender commsMsgSender;
            commsMsgSender.SendStopJavaCertStorePlugin();
            sleep(2);
            testData->DeleteTestDataL();
            RMPointerArray<CCTCertInfo> certInfoArray;
            auto_ptr<CCertAttributeFilter> attrFilter(TestUtils::GetFilterObj());
            auto_ptr<CJavaCertStoreImpl> certStoreObj2(CJavaCertStoreImpl::NewL(*certStoreToken.get()));
            certStoreObj2->List(certInfoArray,*attrFilter.get(),testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            int count = certInfoArray.Count();
            CHECK(0 == count);
            certInfoArray.Close();
        }

        printf("-readingCerts\n");
    }
    catch (TestFailedException& ex)
    {
        printf("readingCerts() ERROR: %s",ex.toString().c_str());
        CHECK(false);
    }
    catch (...)
    {
        printf("readingCerts() Unexpected exception caught");
        CHECK(false);
    }
}

//
// TEST CASE 2
//
TEST(ReadCerts,testDisablingCert)
{
    printf("+testDisablingCert\n");
    try
    {
        //Starting java captain.
        TestUtils::StartJavaCaptain();

        //Stopping javacertstore extension plugin of the java captain.
        CommsMsgSender commsMsgSender;
        commsMsgSender.SendStopJavaCertStorePlugin();
        sleep(2);

        //Deleting all existing
        auto_ptr<CTestData> testData(CTestData::NewL());
        testData->DeleteTestDataL();

        //# 1
        //Create certificates + metadata.
        list<CertMetaData> listOfCerts;
        {
            CertMetaData tmpObj(STATE_ENABLED,"enabledcert2",true,true,"2","content12");
            listOfCerts.push_back(tmpObj);
        }
        testData->StoreTestData(listOfCerts);

        auto_ptr<CTestExecutor> testExecutor(new CTestExecutor());
        auto_ptr<CJavaCertStoreTokenType> certStoreTokenType
        (static_cast<CJavaCertStoreTokenType*>(CJavaCertStoreTokenType::New2L()));
        auto_ptr<CJavaCertStoreToken> certStoreToken
        (CJavaCertStoreToken::New(0x1234,*certStoreTokenType.get()));

        {
            //# 2
            //List certificates.
            auto_ptr<CJavaCertStoreImpl> certStoreObj(CJavaCertStoreImpl::NewL(*certStoreToken.get()));
            RMPointerArray<CCTCertInfo> certInfoArray;
            auto_ptr<CCertAttributeFilter> attrFilter(TestUtils::GetFilterObj());
            certStoreObj->List(certInfoArray,*attrFilter.get(),testExecutor->GetRequestStatus());
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            CHECK(1 == certInfoArray.Count());

            //# 3
            //Getting apps associated to certificate.
            RArray<TUid> uidList;
            certStoreObj->Applications(*certInfoArray[0],uidList,testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            CHECK(1 == uidList.Count());
            CHECK(KMidletInstallApplicabilityUid == uidList[0]);
            uidList.Close();

            //# 4
            //Providing list of active apps to plugin by SetApplicability() operation.
            //List is empty in the disabling case.
            RArray<TUid> uidList2;
            certStoreObj->SetApplicability(*certInfoArray[0],uidList2,testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            uidList2.Close();

            //# 5
            //Disabling certificate in the cert store by SetTrust() operation.
            certStoreObj->SetTrust(*certInfoArray[0],ETrue,testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());

            //# 6
            //Verfying that Applications() operation returns empty list after disabling.
            RArray<TUid> uidList3;
            certStoreObj->Applications(*certInfoArray[0],uidList3,testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            CHECK(0 == uidList3.Count());
            certInfoArray.Close();
            uidList3.Close();
        }

        {
            //# 7
            //Retrieve certificates through a new CJavaCertStoreImpl object.
            auto_ptr<CJavaCertStoreImpl> certStoreObj(CJavaCertStoreImpl::NewL(*certStoreToken.get()));
            RMPointerArray<CCTCertInfo> certInfoArray;
            auto_ptr<CCertAttributeFilter> attrFilter(TestUtils::GetFilterObj());
            certStoreObj->List(certInfoArray,*attrFilter.get(),testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            CHECK(1 == certInfoArray.Count());

            //# 8
            //Check that Applications() operation returns empty list.
            RArray<TUid> uidList;
            certStoreObj->Applications(*certInfoArray[0],uidList,testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            CHECK(0 == uidList.Count());
            certInfoArray.Close();
            uidList.Close();
        }

        printf("-testDisablingCert\n");
    }
    catch (TestFailedException& ex)
    {
        printf("testDisablingCert() ERROR: %s",ex.toString().c_str());
        CHECK(false);
    }
    catch (...)
    {
        printf("testDisablingCert() Unexpected exception caught");
        CHECK(false);
    }
}

//
// TEST CASE 3
//
TEST(ReadCerts,testEnablingCert)
{
    printf("+testEnablingCert\n");
    try
    {
        //Starting java captain.
        TestUtils::StartJavaCaptain();

        //Stopping javacertstore extension plugin of the java captain.
        CommsMsgSender commsMsgSender;
        commsMsgSender.SendStopJavaCertStorePlugin();
        sleep(2);

        //Deleting all existing
        auto_ptr<CTestData> testData(CTestData::NewL());
        testData->DeleteTestDataL();

        //# 1
        //Create certificates + metadata.
        list<CertMetaData> listOfCerts;
        {
            CertMetaData tmpObj(STATE_DISABLED,"enabledcert2",true,true,"2","content12");
            listOfCerts.push_back(tmpObj);
        }
        testData->StoreTestData(listOfCerts);

        auto_ptr<CTestExecutor> testExecutor(new CTestExecutor());
        auto_ptr<CJavaCertStoreTokenType> certStoreTokenType
        (static_cast<CJavaCertStoreTokenType*>(CJavaCertStoreTokenType::New2L()));
        auto_ptr<CJavaCertStoreToken> certStoreToken
        (CJavaCertStoreToken::New(0x1234,*certStoreTokenType.get()));

        {
            //# 2
            //List certificates.
            auto_ptr<CJavaCertStoreImpl> certStoreObj(CJavaCertStoreImpl::NewL(*certStoreToken.get()));
            RMPointerArray<CCTCertInfo> certInfoArray;
            auto_ptr<CCertAttributeFilter> attrFilter(TestUtils::GetFilterObj());
            certStoreObj->List(certInfoArray,*attrFilter.get(),testExecutor->GetRequestStatus());
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            CHECK(1 == certInfoArray.Count());

            //# 3
            //Getting apps associated to certificate. Returned list is empty because only cert
            //has been already disabled.
            RArray<TUid> uidList;
            certStoreObj->Applications(*certInfoArray[0],uidList,testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            CHECK(0 == uidList.Count());
            uidList.Close();

            //# 4
            //Providing list of active apps to plugin by SetApplicability() operation.
            RArray<TUid> uidList2;
            uidList2.Append(KMidletInstallApplicabilityUid);
            certStoreObj->SetApplicability(*certInfoArray[0],uidList2,testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            uidList2.Close();

            //# 5
            //Verifing that Applications() operation appends active app to the list after enabling.
            RArray<TUid> uidList3;
            certStoreObj->Applications(*certInfoArray[0],uidList3,testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            CHECK(1 == uidList3.Count());
            CHECK(KMidletInstallApplicabilityUid == uidList3[0]);
            certInfoArray.Close();
            uidList3.Close();
        }

        {
            //# 6
            //Retrieve certificates through new CJavaCertStoreImpl object.
            auto_ptr<CJavaCertStoreImpl> certStoreObj(CJavaCertStoreImpl::NewL(*certStoreToken.get()));
            RMPointerArray<CCTCertInfo> certInfoArray;
            auto_ptr<CCertAttributeFilter> attrFilter(TestUtils::GetFilterObj());
            certStoreObj->List(certInfoArray,*attrFilter.get(),testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            CHECK(1 == certInfoArray.Count());

            //# 7
            //Check that Applications() operation appends active app to the list.
            RArray<TUid> uidList;
            certStoreObj->Applications(*certInfoArray[0],uidList,testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            CHECK(1 == uidList.Count());
            CHECK(KMidletInstallApplicabilityUid == uidList[0]);
            certInfoArray.Close();
            uidList.Close();
        }

        printf("-testEnablingCert\n");
    }
    catch (TestFailedException& ex)
    {
        printf("testEnablingCert() ERROR: %s",ex.toString().c_str());
        CHECK(false);
    }
    catch (...)
    {
        printf("testEnablingCert() Unexpected exception caught");
        CHECK(false);
    }
}

//
// TEST CASE 4
//
TEST(ReadCerts,testDeletingCert)
{
    printf("+testDeletingCert\n");
    try
    {
        //Starting java captain.
        TestUtils::StartJavaCaptain();

        //Stopping javacertstore extension plugin of the java captain.
        CommsMsgSender commsMsgSender;
        commsMsgSender.SendStopJavaCertStorePlugin();
        sleep(2);

        //Deleting all existing
        auto_ptr<CTestData> testData(CTestData::NewL());
        testData->DeleteTestDataL();

        //# 1
        //Create certificates + metadata.
        list<CertMetaData> listOfCerts;
        {
            CertMetaData tmpObj(STATE_ENABLED,"enabledcert1",true,true,"1","content1");
            listOfCerts.push_back(tmpObj);
        }
        {
            CertMetaData tmpObj(STATE_ENABLED,"enabledcert2",true,true,"2","content12");
            listOfCerts.push_back(tmpObj);
        }
        {
            CertMetaData tmpObj(STATE_DISABLED,"disabledcert1",true,true,"3","content123");
            listOfCerts.push_back(tmpObj);
        }
        testData->StoreTestData(listOfCerts);

        auto_ptr<CTestExecutor> testExecutor(new CTestExecutor());
        auto_ptr<CJavaCertStoreTokenType> certStoreTokenType
        (static_cast<CJavaCertStoreTokenType*>(CJavaCertStoreTokenType::New2L()));
        auto_ptr<CJavaCertStoreToken> certStoreToken
        (CJavaCertStoreToken::New(0x1234,*certStoreTokenType.get()));

        {
            //# 2
            //List certificates.
            auto_ptr<CJavaCertStoreImpl> certStoreObj(CJavaCertStoreImpl::NewL(*certStoreToken.get()));
            RMPointerArray<CCTCertInfo> certInfoArray;
            auto_ptr<CCertAttributeFilter> attrFilter(TestUtils::GetFilterObj());
            certStoreObj->List(certInfoArray,*attrFilter.get(),testExecutor->GetRequestStatus());
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            CHECK(3 == certInfoArray.Count());

            //# 3
            //Remove first cert.
            certStoreObj->Remove(*certInfoArray[0],testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            certInfoArray.Close();

            //# 4
            //List certificates. Only two should be returned by List() operation.
            //certInfoArray.ResetAndDestroy();
            RMPointerArray<CCTCertInfo> certInfoArray2;
            certStoreObj->List(certInfoArray2,*attrFilter.get(),testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            CHECK(2 == certInfoArray2.Count());

            //# 5
            //Remove second cert.
            certStoreObj->Remove(*certInfoArray2[0],testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            certInfoArray2.Close();

            //# 6
            //List certificates. Only one should be returned by List() operation.
            //certInfoArray.ResetAndDestroy();
            RMPointerArray<CCTCertInfo> certInfoArray3;
            certStoreObj->List(certInfoArray3,*attrFilter.get(),testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            CHECK(1 == certInfoArray3.Count());

            //# 7
            //Remove last cert.
            certStoreObj->Remove(*certInfoArray3[0],testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            certInfoArray3.Close();

            //# 8
            //List certificates. Only one should be returned by List() operation.
            //certInfoArray.ResetAndDestroy();
            RMPointerArray<CCTCertInfo> certInfoArray4;
            certStoreObj->List(certInfoArray4,*attrFilter.get(),testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            CHECK(0 == certInfoArray4.Count());
            certInfoArray4.Close();
        }

        printf("-testDeletingCert\n");
    }
    catch (TestFailedException& ex)
    {
        printf("testDeletingCert() ERROR: %s",ex.toString().c_str());
        CHECK(false);
    }
    catch (...)
    {
        printf("testDeletingCert() Unexpected exception caught");
        CHECK(false);
    }
}

//
// TEST CASE 5
//
TEST(ReadCerts,testsForValidateCertAttrFilter)
{
    printf("+testsForValidateCertAttrFilter\n");
    try
    {
        //Starting java captain.
        TestUtils::StartJavaCaptain();

        //Stopping javacertstore extension plugin of the java captain.
        CommsMsgSender commsMsgSender;
        commsMsgSender.SendStopJavaCertStorePlugin();
        sleep(2);

        //Deleting all existing
        auto_ptr<CTestData> testData(CTestData::NewL());
        testData->DeleteTestDataL();

        auto_ptr<CTestExecutor> testExecutor(new CTestExecutor());
        auto_ptr<CJavaCertStoreTokenType> certStoreTokenType
        (static_cast<CJavaCertStoreTokenType*>(CJavaCertStoreTokenType::New2L()));
        auto_ptr<CJavaCertStoreToken> certStoreToken
        (CJavaCertStoreToken::New(0x1234,*certStoreTokenType.get()));
        auto_ptr<CJavaCertStoreImpl> certStoreObj(CJavaCertStoreImpl::NewL(*certStoreToken.get()));

        //# 1
        //Not supported uid(0x101F9B29).
        {
            RMPointerArray<CCTCertInfo> certInfoArray;
            std::auto_ptr< CCertAttributeFilter> filterObj(CCertAttributeFilter::NewL());
            TUid uid = TUid::Uid(0x101F9B29);
            filterObj->SetUid(uid);
            filterObj->SetFormat(EX509Certificate);
            filterObj->SetOwnerType(ECACertificate);
            filterObj->SetLabel(KMIDP2TrustRoot());
            certStoreObj->List(certInfoArray,*filterObj.get(),testExecutor->GetRequestStatus());
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            CHECK(0 == certInfoArray.Count());
            certInfoArray.Close();
        }

        //# 2
        //Not supported certificate format(EWTLSCertificate).
        {
            RMPointerArray<CCTCertInfo> certInfoArray;
            std::auto_ptr< CCertAttributeFilter> filterObj(CCertAttributeFilter::NewL());
            filterObj->SetUid(KMidletInstallApplicabilityUid);
            filterObj->SetFormat(EWTLSCertificate);
            filterObj->SetOwnerType(ECACertificate);
            filterObj->SetLabel(KMIDP2TrustRoot());
            certStoreObj->List(certInfoArray,*filterObj.get(),testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            CHECK(0 == certInfoArray.Count());
            certInfoArray.Close();
        }

        //# 3
        //Not supported owner type(EUserCertificate).
        {
            RMPointerArray<CCTCertInfo> certInfoArray;
            std::auto_ptr< CCertAttributeFilter> filterObj(CCertAttributeFilter::NewL());
            filterObj->SetUid(KMidletInstallApplicabilityUid);
            filterObj->SetFormat(EX509Certificate);
            filterObj->SetOwnerType(EUserCertificate);
            filterObj->SetLabel(KMIDP2TrustRoot());
            certStoreObj->List(certInfoArray,*filterObj.get(),testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            CHECK(0 == certInfoArray.Count());
            certInfoArray.Close();
        }

        //# 4
        //Adding subject key to filter object.
        {
            RMPointerArray<CCTCertInfo> certInfoArray;
            std::auto_ptr< CCertAttributeFilter> filterObj(CCertAttributeFilter::NewL());
            filterObj->SetUid(KMidletInstallApplicabilityUid);
            filterObj->SetFormat(EX509Certificate);
            filterObj->SetOwnerType(ECACertificate);
            filterObj->SetLabel(KMIDP2TrustRoot());
            TKeyIdentifier keyIdent = _L8("blaah");
            filterObj->SetSubjectKeyId(keyIdent);
            certStoreObj->List(certInfoArray,*filterObj.get(),testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            CHECK(0 == certInfoArray.Count());
            certInfoArray.Close();
        }

        //# 5
        //Adding issuer key to filter object.
        {
            RMPointerArray<CCTCertInfo> certInfoArray;
            std::auto_ptr< CCertAttributeFilter> filterObj(CCertAttributeFilter::NewL());
            filterObj->SetUid(KMidletInstallApplicabilityUid);
            filterObj->SetFormat(EX509Certificate);
            filterObj->SetOwnerType(ECACertificate);
            filterObj->SetLabel(KMIDP2TrustRoot());
            TKeyIdentifier keyIdent = _L8("blaah");
            filterObj->SetIssuerKeyId(keyIdent);
            certStoreObj->List(certInfoArray,*filterObj.get(),testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            CHECK(0 == certInfoArray.Count());
            certInfoArray.Close();
        }

        //# 6
        //Not supported label.
        {
            RMPointerArray<CCTCertInfo> certInfoArray;
            std::auto_ptr< CCertAttributeFilter> filterObj(CCertAttributeFilter::NewL());
            filterObj->SetUid(KMidletInstallApplicabilityUid);
            filterObj->SetFormat(EX509Certificate);
            filterObj->SetOwnerType(ECACertificate);
            filterObj->SetLabel(_L("blaah"));
            certStoreObj->List(certInfoArray,*filterObj.get(),testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            CHECK(0 == certInfoArray.Count());
            certInfoArray.Close();
        }

        printf("-testsForValidateCertAttrFilter\n");
    }
    catch (TestFailedException& ex)
    {
        printf("testsForValidateCertAttrFilter() ERROR: %s",ex.toString().c_str());
        CHECK(false);
    }
    catch (...)
    {
        printf("testsForValidateCertAttrFilter() Unexpected exception caught");
        CHECK(false);
    }
}

//
// TEST CASE 6
//
TEST(ReadCerts,errorCasesForList)
{
    printf("+errorCasesForList\n");
    try
    {
        //Starting java captain.
        TestUtils::StartJavaCaptain();

        //Stopping javacertstore extension plugin of the java captain.
        CommsMsgSender commsMsgSender;
        commsMsgSender.SendStopJavaCertStorePlugin();
        sleep(2);

        //Deleting all existing
        auto_ptr<CTestData> testData(CTestData::NewL());
        testData->DeleteTestDataL();

        auto_ptr<CTestExecutor> testExecutor(new CTestExecutor());
        auto_ptr<CJavaCertStoreTokenType> certStoreTokenType
        (static_cast<CJavaCertStoreTokenType*>(CJavaCertStoreTokenType::New2L()));
        auto_ptr<CJavaCertStoreToken> certStoreToken
        (CJavaCertStoreToken::New(0x1234,*certStoreTokenType.get()));
        auto_ptr<CJavaCertStoreImpl> certStoreObj(CJavaCertStoreImpl::NewL(*certStoreToken.get()));
        auto_ptr<CCertAttributeFilter> attrFilter(TestUtils::GetFilterObj());

        {
            //# 1
            //Calling List() operation two times in the row.
            RMPointerArray<CCTCertInfo> certInfoArray1;
            RMPointerArray<CCTCertInfo> certInfoArray2;
            auto_ptr<CTestExecutor> testExecutor2(new CTestExecutor());
            certStoreObj->List(certInfoArray1,*attrFilter.get(),testExecutor2->GetRequestStatus());
            auto_ptr<CTestExecutor> testExecutor3(new CTestExecutor());
            certStoreObj->List(certInfoArray2,*attrFilter.get(),testExecutor3->GetRequestStatus());
            certStoreObj->CancelList();
            certInfoArray1.Close();
            certInfoArray2.Close();
        }

        {
            //# 2
            //Calling CancelList() immediately after List() operation.
            RMPointerArray<CCTCertInfo> certInfoArray1;
            certStoreObj->List(certInfoArray1,*attrFilter.get(),testExecutor->GetRequestStatus());
            certStoreObj->CancelList();
            CActiveScheduler::Start();
            CHECK(KErrCancel == testExecutor->GetErrorCode());
            certInfoArray1.Close();
        }

        printf("-errorCasesForList\n");
    }
    catch (TestFailedException& ex)
    {
        printf("errorCasesForList() ERROR: %s",ex.toString().c_str());
        CHECK(false);
    }
    catch (...)
    {
        printf("errorCasesForList() Unexpected exception caught");
        CHECK(false);
    }
}

//
// TEST CASE 7
//
TEST(ReadCerts,errorCasesForAdd)
{
    printf("+errorCasesForAdd\n");
    try
    {
        //Starting java captain.
        TestUtils::StartJavaCaptain();

        //Stopping javacertstore extension plugin of the java captain.
        CommsMsgSender commsMsgSender;
        commsMsgSender.SendStopJavaCertStorePlugin();
        sleep(2);

        //Deleting all existing
        auto_ptr<CTestData> testData(CTestData::NewL());
        testData->DeleteTestDataL();

        auto_ptr<CTestExecutor> testExecutor(new CTestExecutor());
        auto_ptr<CJavaCertStoreTokenType> certStoreTokenType
        (static_cast<CJavaCertStoreTokenType*>(CJavaCertStoreTokenType::New2L()));
        auto_ptr<CJavaCertStoreToken> certStoreToken
        (CJavaCertStoreToken::New(0x1234,*certStoreTokenType.get()));
        auto_ptr<CJavaCertStoreImpl> certStoreObj(CJavaCertStoreImpl::NewL(*certStoreToken.get()));
        auto_ptr<CCertAttributeFilter> attrFilter(TestUtils::GetFilterObj());

        {
            //# 1
            //Calling not supported Add() operation.
            certStoreObj->Add(_L("someLabel"),EX509Certificate,ECACertificate,NULL,NULL,_L8("some"),
                              testExecutor->GetRequestStatus());
            CActiveScheduler::Start();
            CHECK(KErrNotSupported == testExecutor->GetErrorCode());
        }

        printf("-errorCasesForAdd\n");
    }
    catch (TestFailedException& ex)
    {
        printf("errorCasesForAdd() ERROR: %s",ex.toString().c_str());
        CHECK(false);
    }
    catch (...)
    {
        printf("errorCasesForAdd() Unexpected exception caught");
        CHECK(false);
    }
}

//
// TEST CASE 8
//
TEST(ReadCerts,errorCasesForRemove)
{
    printf("+errorCasesForRemove\n");
    try
    {
        //Starting java captain.
        TestUtils::StartJavaCaptain();

        //Stopping javacertstore extension plugin of the java captain.
        CommsMsgSender commsMsgSender;
        commsMsgSender.SendStopJavaCertStorePlugin();
        sleep(2);

        //Deleting all existing
        auto_ptr<CTestData> testData(CTestData::NewL());
        testData->DeleteTestDataL();

        //# 1
        //Create certificates + metadata.
        list<CertMetaData> listOfCerts;
        {
            CertMetaData tmpObj(STATE_ENABLED,"enabledcert1",false,true,"1","content1");
            listOfCerts.push_back(tmpObj);
        }
        testData->StoreTestData(listOfCerts);

        auto_ptr<CTestExecutor> testExecutor(new CTestExecutor());
        auto_ptr<CJavaCertStoreTokenType> certStoreTokenType
        (static_cast<CJavaCertStoreTokenType*>(CJavaCertStoreTokenType::New2L()));
        auto_ptr<CJavaCertStoreToken> certStoreToken
        (CJavaCertStoreToken::New(0x1234,*certStoreTokenType.get()));
        auto_ptr<CJavaCertStoreImpl> certStoreObj(CJavaCertStoreImpl::NewL(*certStoreToken.get()));
        auto_ptr<CCertAttributeFilter> attrFilter(TestUtils::GetFilterObj());

        RMPointerArray<CCTCertInfo> certInfoArray;
        {
            //# 2
            //List certificates.
            certStoreObj->List(certInfoArray,*attrFilter.get(),testExecutor->GetRequestStatus());
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            CHECK(1 == certInfoArray.Count());
        }

        {
            //# 3
            //Passing CCTCertInfo object, which does not match to any cert, as argument to
            //Remove() operation.
            CCTCertInfo* certInfo(CCTCertInfo::NewL(KMIDP2TrustRoot,EX509Certificate,
                                                    ECACertificate,10,NULL,NULL,
                                                    certStoreObj->Token(),2,true));
            certStoreObj->Remove(*certInfo,testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrArgument == testExecutor->GetErrorCode());
            certInfo->Release();
        }

        {
            //# 4
            //Passing CCTCertInfo object, which points to non-deletable, as argument to
            //Remove() operation.
            certStoreObj->Remove(*certInfoArray[0],testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrArgument == testExecutor->GetErrorCode());
        }
        certInfoArray.Close();

        printf("-errorCasesForRemove\n");
    }
    catch (TestFailedException& ex)
    {
        printf("errorCasesForRemove() ERROR: %s",ex.toString().c_str());
        CHECK(false);
    }
    catch (...)
    {
        printf("errorCasesForRemove() Unexpected exception caught");
        CHECK(false);
    }
}

//
// TEST CASE 9
//
TEST(ReadCerts,testsForSetTrust)
{
    printf("+testsForSetTrust\n");

    try
    {
        //Starting java captain.
        TestUtils::StartJavaCaptain();

        //Stopping javacertstore extension plugin of the java captain.
        CommsMsgSender commsMsgSender;
        commsMsgSender.SendStopJavaCertStorePlugin();
        sleep(2);

        //Deleting all existing
        auto_ptr<CTestData> testData(CTestData::NewL());
        testData->DeleteTestDataL();

        //# 1
        //Create certificates + metadata.
        list<CertMetaData> listOfCerts;
        {
            CertMetaData tmpObj(STATE_ENABLED,"enabledcert1",true,false,"1","content1");
            listOfCerts.push_back(tmpObj);
        }
        testData->StoreTestData(listOfCerts);

        auto_ptr<CTestExecutor> testExecutor(new CTestExecutor());
        auto_ptr<CJavaCertStoreTokenType> certStoreTokenType
        (static_cast<CJavaCertStoreTokenType*>(CJavaCertStoreTokenType::New2L()));
        auto_ptr<CJavaCertStoreToken> certStoreToken
        (CJavaCertStoreToken::New(0x1234,*certStoreTokenType.get()));
        auto_ptr<CJavaCertStoreImpl> certStoreObj(CJavaCertStoreImpl::NewL(*certStoreToken.get()));
        auto_ptr<CCertAttributeFilter> attrFilter(TestUtils::GetFilterObj());

        {
            //# 2
            //Passing CCTCertInfo object, which does not match to any cert, as argument to
            //SetTrust() operation.
            CCTCertInfo* certInfo(CCTCertInfo::NewL(KMIDP2TrustRoot,EX509Certificate,
                                                    ECACertificate,10,NULL,NULL,
                                                    certStoreObj->Token(),2,true));
            certStoreObj->SetTrust(*certInfo,ETrue,testExecutor->GetRequestStatus());
            CActiveScheduler::Start();
            CHECK(KErrArgument == testExecutor->GetErrorCode());
            certInfo->Release();
        }

        {
            //# 3
            //Try to set trusted cert to non-trusted.
            ////Try to disable cert which is not allowed to disable.
            RMPointerArray<CCTCertInfo> certInfoArray;
            certStoreObj->List(certInfoArray,*attrFilter.get(),testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            CHECK(1 == certInfoArray.Count());

            certStoreObj->SetTrust(*certInfoArray[0],EFalse,testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrArgument == testExecutor->GetErrorCode());
            certInfoArray.Close();
        }

        printf("-testsForSetTrust\n");
    }
    catch (TestFailedException& ex)
    {
        printf("testsForSetTrust() ERROR: %s",ex.toString().c_str());
        CHECK(false);
    }
    catch (...)
    {
        printf("testsForSetTrust() Unexpected exception caught");
        CHECK(false);
    }
}

//
// TEST CASE 10
//
TEST(ReadCerts,testsForGetCert)
{
    printf("+testsForGetCert\n");

    try
    {
        //Starting java captain.
        TestUtils::StartJavaCaptain();

        //Stopping javacertstore extension plugin of the java captain.
        CommsMsgSender commsMsgSender;
        commsMsgSender.SendStopJavaCertStorePlugin();
        sleep(2);

        //Deleting all existing
        auto_ptr<CTestData> testData(CTestData::NewL());
        testData->DeleteTestDataL();

        //# 1
        //Create certificates + metadata.
        list<CertMetaData> listOfCerts;
        {
            CertMetaData tmpObj(STATE_ENABLED,"enabledcert1",false,true,"1","content1");
            listOfCerts.push_back(tmpObj);
        }
        testData->StoreTestData(listOfCerts);

        auto_ptr<CTestExecutor> testExecutor(new CTestExecutor());
        auto_ptr<CJavaCertStoreTokenType> certStoreTokenType
        (static_cast<CJavaCertStoreTokenType*>(CJavaCertStoreTokenType::New2L()));
        auto_ptr<CJavaCertStoreToken> certStoreToken
        (CJavaCertStoreToken::New(0x1234,*certStoreTokenType.get()));
        auto_ptr<CJavaCertStoreImpl> certStoreObj(CJavaCertStoreImpl::NewL(*certStoreToken.get()));
        auto_ptr<CCertAttributeFilter> attrFilter(TestUtils::GetFilterObj());

        {
            //# 2
            //Try to retrieve CCTCertInfo using non-existing handle.
            CCTCertInfo* retCert = NULL;
            TCTTokenHandle tmpObj;
            TCTTokenObjectHandle tmpHandle(tmpObj,2);
            certStoreObj->GetCert(retCert,tmpHandle,testExecutor->GetRequestStatus());
            CActiveScheduler::Start();
            CHECK(KErrArgument == testExecutor->GetErrorCode());
        }

        {
            //# 3
            //Get CCTCertInfo object by TCTTokenObjectHandle.
            RMPointerArray<CCTCertInfo> certInfoArray;
            auto_ptr<CCertAttributeFilter> attrFilter(TestUtils::GetFilterObj());
            certStoreObj->List(certInfoArray,*attrFilter.get(),testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            CHECK(1 == certInfoArray.Count());

            CCTCertInfo* retCert = NULL;
            TCTTokenHandle tmpObj;
            TCTTokenObjectHandle tmpHandle(tmpObj,0);
            certStoreObj->GetCert(retCert,tmpHandle,testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            CHECK(8 != retCert->Size());
            certInfoArray.Close();
            retCert->Release();
        }

        printf("-testsForGetCert\n");
    }
    catch (TestFailedException& ex)
    {
        printf("testsForGetCert() ERROR: %s",ex.toString().c_str());
        CHECK(false);
    }
    catch (...)
    {
        printf("testsForGetCert() Unexpected exception caught");
        CHECK(false);
    }
}

//
// TEST CASE 11
//
TEST(ReadCerts,errorCasesForApplications)
{
    printf("+errorCasesForApplications\n");

    try
    {

        //Starting java captain.
        TestUtils::StartJavaCaptain();

        //Stopping javacertstore extension plugin of the java captain.
        CommsMsgSender commsMsgSender;
        commsMsgSender.SendStopJavaCertStorePlugin();
        sleep(2);

        //Deleting all existing
        auto_ptr<CTestData> testData(CTestData::NewL());
        testData->DeleteTestDataL();

        auto_ptr<CTestExecutor> testExecutor(new CTestExecutor());
        auto_ptr<CJavaCertStoreTokenType> certStoreTokenType
        (static_cast<CJavaCertStoreTokenType*>(CJavaCertStoreTokenType::New2L()));
        auto_ptr<CJavaCertStoreToken> certStoreToken
        (CJavaCertStoreToken::New(0x1234,*certStoreTokenType.get()));
        auto_ptr<CJavaCertStoreImpl> certStoreObj(CJavaCertStoreImpl::NewL(*certStoreToken.get()));
        auto_ptr<CCertAttributeFilter> attrFilter(TestUtils::GetFilterObj());

        //# 1
        //Pass CCTCertInfo object, which does not match to any cert, as argument to
        //Applications() operation.
        CCTCertInfo* certInfo(CCTCertInfo::NewL(KMIDP2TrustRoot,EX509Certificate,
                                                ECACertificate,10,NULL,NULL,
                                                certStoreObj->Token(),2,true));
        RArray<TUid> uidList;
        certStoreObj->Applications(*certInfo,uidList,testExecutor->GetRequestStatus());
        CActiveScheduler::Start();
        CHECK(KErrArgument == testExecutor->GetErrorCode());
        certInfo->Release();

        printf("-errorCasesForApplications\n");
    }
    catch (TestFailedException& ex)
    {
        printf("errorCasesForApplications() ERROR: %s",ex.toString().c_str());
        CHECK(false);
    }
    catch (...)
    {
        printf("errorCasesForApplications() Unexpected exception caught");
        CHECK(false);
    }
}

//
// TEST CASE 12
//
TEST(ReadCerts,testCasesForIsApplicable)
{
    printf("+testCasesForIsApplicable\n");

    try
    {
        //Starting java captain.
        TestUtils::StartJavaCaptain();

        //Stopping javacertstore extension plugin of the java captain.
        CommsMsgSender commsMsgSender;
        commsMsgSender.SendStopJavaCertStorePlugin();
        sleep(2);

        //Deleting all existing
        auto_ptr<CTestData> testData(CTestData::NewL());
        testData->DeleteTestDataL();

        //# 1
        //Create certificates + metadata.
        list<CertMetaData> listOfCerts;
        {
            CertMetaData tmpObj(STATE_DISABLED,"disabledcert1",true,true,"1","content1");
            listOfCerts.push_back(tmpObj);
        }
        {
            CertMetaData tmpObj(STATE_ENABLED,"enabledcert1",true,true,"2","content12");
            listOfCerts.push_back(tmpObj);
        }
        testData->StoreTestData(listOfCerts);

        auto_ptr<CTestExecutor> testExecutor(new CTestExecutor());
        auto_ptr<CJavaCertStoreTokenType> certStoreTokenType
        (static_cast<CJavaCertStoreTokenType*>(CJavaCertStoreTokenType::New2L()));
        auto_ptr<CJavaCertStoreToken> certStoreToken
        (CJavaCertStoreToken::New(0x1234,*certStoreTokenType.get()));
        auto_ptr<CJavaCertStoreImpl> certStoreObj(CJavaCertStoreImpl::NewL(*certStoreToken.get()));
        auto_ptr<CCertAttributeFilter> attrFilter(TestUtils::GetFilterObj());

        {
            //# 2
            //Passing incorrect Uid as argument to IsApplicable() operation.
            CCTCertInfo* certInfo(CCTCertInfo::NewL(KMIDP2TrustRoot,EX509Certificate,
                                                    ECACertificate,10,NULL,NULL,
                                                    certStoreObj->Token(),2,true));
            TUid tmpUid = TUid::Uid(0x101F9B);
            TBool isApplicable;
            certStoreObj->IsApplicable(*certInfo,tmpUid,isApplicable,testExecutor->GetRequestStatus());
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            if (isApplicable)
                CHECK(false);
            certInfo->Release();
        }

        {
            //# 3
            //Pass CCTCertInfo object, which does not match to any cert, as argument to
            //IsApplicable() operation.
            CCTCertInfo* certInfo(CCTCertInfo::NewL(KMIDP2TrustRoot,EX509Certificate,
                                                    ECACertificate,10,NULL,NULL,
                                                    certStoreObj->Token(),2,true));
            TBool isApplicable;
            certStoreObj->IsApplicable(*certInfo,KMidletInstallApplicabilityUid,
                                       isApplicable,testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrArgument == testExecutor->GetErrorCode());
            certInfo->Release();
        }

        {
            //# 4
            //Ask applicability of disabled cert.
            RMPointerArray<CCTCertInfo> certInfoArray;
            testExecutor->Activate();
            certStoreObj->List(certInfoArray,*attrFilter.get(),testExecutor->GetRequestStatus());
            //testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            CHECK(2 == certInfoArray.Count());

            TBool isApplicable = ETrue;
            certStoreObj->IsApplicable(*certInfoArray[1],KMidletInstallApplicabilityUid,
                                       isApplicable,testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            if (isApplicable)
                CHECK(false);

            //# 5
            //Delete cert.
            certStoreObj->Remove(*certInfoArray[1],testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());

            //# 6
            //Ask applicability of deleted cert.
            isApplicable = ETrue;
            certStoreObj->IsApplicable(*certInfoArray[1],KMidletInstallApplicabilityUid,
                                       isApplicable,testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            if (isApplicable)
                CHECK(false);

            //# 7
            //Ask applicability of enabled cert.
            isApplicable = EFalse;
            certStoreObj->IsApplicable(*certInfoArray[0],KMidletInstallApplicabilityUid,
                                       isApplicable,testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            if (!isApplicable)
                CHECK(false);
            certInfoArray.Close();
        }

        printf("-testCasesForIsApplicable\n");
    }
    catch (TestFailedException& ex)
    {
        printf("testCasesForIsApplicable() ERROR: %s",ex.toString().c_str());
        CHECK(false);
    }
    catch (...)
    {
        printf("testCasesForIsApplicable() Unexpected exception caught");
        CHECK(false);
    }
}

//
// TEST CASE 13
//
TEST(ReadCerts,testCasesForTrusted)
{
    printf("+testCasesForTrusted\n");

    try
    {
        //Starting java captain.
        TestUtils::StartJavaCaptain();

        //Stopping javacertstore extension plugin of the java captain.
        CommsMsgSender commsMsgSender;
        commsMsgSender.SendStopJavaCertStorePlugin();
        sleep(2);

        //Deleting all existing
        auto_ptr<CTestData> testData(CTestData::NewL());
        testData->DeleteTestDataL();

        //# 1
        //Create certificates + metadata.
        list<CertMetaData> listOfCerts;
        {
            CertMetaData tmpObj(STATE_ENABLED,"enabledcert1",true,true,"1","content1");
            listOfCerts.push_back(tmpObj);
        }
        testData->StoreTestData(listOfCerts);

        auto_ptr<CTestExecutor> testExecutor(new CTestExecutor());
        auto_ptr<CJavaCertStoreTokenType> certStoreTokenType
        (static_cast<CJavaCertStoreTokenType*>(CJavaCertStoreTokenType::New2L()));
        auto_ptr<CJavaCertStoreToken> certStoreToken
        (CJavaCertStoreToken::New(0x1234,*certStoreTokenType.get()));
        auto_ptr<CJavaCertStoreImpl> certStoreObj(CJavaCertStoreImpl::NewL(*certStoreToken.get()));
        auto_ptr<CCertAttributeFilter> attrFilter(TestUtils::GetFilterObj());

        {
            //# 2
            //Passing CCTCertInfo object, which does not point to any cert,
            //as argument to Trusted() operation.
            CCTCertInfo* certInfo(CCTCertInfo::NewL(KMIDP2TrustRoot,EX509Certificate,
                                                    ECACertificate,10,NULL,NULL,
                                                    certStoreObj->Token(),2,true));
            TBool retFlag;
            certStoreObj->Trusted(*certInfo,retFlag,testExecutor->GetRequestStatus());
            CActiveScheduler::Start();
            CHECK(KErrArgument == testExecutor->GetErrorCode());
            if (!retFlag)
            {
                CHECK(false);
            }
            certInfo->Release();
        }

        {
            //# 3
            //Passing valid CCTCertInfo object as argument to Trusted() operation.
            RMPointerArray<CCTCertInfo> certInfoArray;
            auto_ptr<CCertAttributeFilter> attrFilter(TestUtils::GetFilterObj());
            certStoreObj->List(certInfoArray,*attrFilter.get(),testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            CHECK(1 == certInfoArray.Count());

            TBool retFlag;
            certStoreObj->Trusted(*certInfoArray[0],retFlag,testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            CHECK(retFlag);

            //# 4
            //Deleting cert.
            certStoreObj->Remove(*certInfoArray[0],testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());

            //# 5
            //Call Trusted() to deleted cert.
            certStoreObj->Trusted(*certInfoArray[0],retFlag,testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrArgument == testExecutor->GetErrorCode());

            certInfoArray.Close();
        }

        printf("-testCasesForTrusted\n");
    }
    catch (TestFailedException& ex)
    {
        printf("testCasesForTrusted() ERROR: %s",ex.toString().c_str());
        CHECK(false);
    }
    catch (...)
    {
        printf("testCasesForTrusted() Unexpected exception caught");
        CHECK(false);
    }
}

//
// TEST CASE 14
//
TEST(ReadCerts,errorCasesForRetrieve)
{
    printf("+errorCasesForRetrieve\n");

    try
    {
        //Starting java captain.
        TestUtils::StartJavaCaptain();

        //Stopping javacertstore extension plugin of the java captain.
        CommsMsgSender commsMsgSender;
        commsMsgSender.SendStopJavaCertStorePlugin();
        sleep(2);

        //Deleting all existing
        auto_ptr<CTestData> testData(CTestData::NewL());
        testData->DeleteTestDataL();

        //# 1
        //Create certificates + metadata.
        list<CertMetaData> listOfCerts;
        {
            CertMetaData tmpObj(STATE_ENABLED,"enablecert1",true,true,"1","content1");
            listOfCerts.push_back(tmpObj);
        }
        {
            CertMetaData tmpObj(STATE_ENABLED,"enabledcert2",true,true,"2","content12");
            listOfCerts.push_back(tmpObj);
        }
        testData->StoreTestData(listOfCerts);

        auto_ptr<CTestExecutor> testExecutor(new CTestExecutor());
        auto_ptr<CJavaCertStoreTokenType> certStoreTokenType
        (static_cast<CJavaCertStoreTokenType*>(CJavaCertStoreTokenType::New2L()));
        auto_ptr<CJavaCertStoreToken> certStoreToken
        (CJavaCertStoreToken::New(0x1234,*certStoreTokenType.get()));
        auto_ptr<CJavaCertStoreImpl> certStoreObj(CJavaCertStoreImpl::NewL(*certStoreToken.get()));
        auto_ptr<CCertAttributeFilter> attrFilter(TestUtils::GetFilterObj());

        {
            //# 2
            //Call twice Retrieve() operation in the row.
            RMPointerArray<CCTCertInfo> certInfoArray;
            certStoreObj->List(certInfoArray,*attrFilter.get(),testExecutor->GetRequestStatus());
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            CHECK(2 == certInfoArray.Count());

            auto_ptr<HBufC8> certContentBuf(HBufC8::NewL(certInfoArray[0]->Size()));
            TPtr8 certContentPtr(certContentBuf->Des());
            auto_ptr<HBufC8> certContentBuf2(HBufC8::NewL(certInfoArray[1]->Size()));
            TPtr8 certContentPtr2(certContentBuf2->Des());
            auto_ptr<CTestExecutor> testExecutor2(new CTestExecutor());
            certStoreObj->Retrieve(*certInfoArray[0],certContentPtr,testExecutor2->GetRequestStatus());
            auto_ptr<CTestExecutor> testExecutor3(new CTestExecutor());
            certStoreObj->Retrieve(*certInfoArray[1],certContentPtr2,testExecutor3->GetRequestStatus());
            certStoreObj->CancelRetrieve();
            certInfoArray.Close();
            certStoreObj->CancelRetrieve();
        }

        {
            //# 3
            //Pass CCTCertInfo object, which does not match to any cert, as argument to
            //Retrieve() operation.
            CCTCertInfo* certInfo(CCTCertInfo::NewL(KMIDP2TrustRoot,EX509Certificate,
                                                    ECACertificate,10,NULL,NULL,
                                                    certStoreObj->Token(),4,true));
            auto_ptr<HBufC8> certContentBuf(HBufC8::NewL(100));
            TPtr8 certContentPtr(certContentBuf->Des());
            certStoreObj->Retrieve(*certInfo,certContentPtr,testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrArgument == testExecutor->GetErrorCode());
            certInfo->Release();
        }

        printf("-errorCasesForRetrieve\n");
    }
    catch (TestFailedException& ex)
    {
        printf("errorCasesForRetrieve() ERROR: %s",ex.toString().c_str());
        CHECK(false);
    }
    catch (...)
    {
        printf("errorCasesForRetrieve() Unexpected exception caught");
        CHECK(false);
    }
}

//
// TEST CASE 15
//
TEST(ReadCerts,errorCasesForSetApplicability)
{
    printf("+errorCasesForSetApplicability\n");

    try
    {
        //Starting java captain.
        TestUtils::StartJavaCaptain();

        //Stopping javacertstore extension plugin of the java captain.
        CommsMsgSender commsMsgSender;
        commsMsgSender.SendStopJavaCertStorePlugin();
        sleep(2);

        //Deleting all existing
        auto_ptr<CTestData> testData(CTestData::NewL());
        testData->DeleteTestDataL();

        //# 1
        //Create certificates + metadata.
        list<CertMetaData> listOfCerts;
        {
            CertMetaData tmpObj(STATE_ENABLED,"enabledcert1",true,false,"1","content1");
            listOfCerts.push_back(tmpObj);
        }
        testData->StoreTestData(listOfCerts);

        auto_ptr<CTestExecutor> testExecutor(new CTestExecutor());
        auto_ptr<CJavaCertStoreTokenType> certStoreTokenType
        (static_cast<CJavaCertStoreTokenType*>(CJavaCertStoreTokenType::New2L()));
        auto_ptr<CJavaCertStoreToken> certStoreToken
        (CJavaCertStoreToken::New(0x1234,*certStoreTokenType.get()));
        auto_ptr<CJavaCertStoreImpl> certStoreObj(CJavaCertStoreImpl::NewL(*certStoreToken.get()));
        auto_ptr<CCertAttributeFilter> attrFilter(TestUtils::GetFilterObj());

        {
            //# 2
            //CertInfo object does not match to any cert.
            CCTCertInfo* certInfo(CCTCertInfo::NewL(KMIDP2TrustRoot,EX509Certificate,
                                                    ECACertificate,10,NULL,NULL,
                                                    certStoreObj->Token(),2,true));
            RArray<TUid> emptyArray;
            certStoreObj->SetApplicability(*certInfo,emptyArray,testExecutor->GetRequestStatus());
            CActiveScheduler::Start();
            CHECK(KErrArgument == testExecutor->GetErrorCode());
            certInfo->Release();
            emptyArray.Close();
        }

        {
            //# 3
            //Try to disable cert which cannot be disabled.
            RMPointerArray<CCTCertInfo> certInfoArray;
            certStoreObj->List(certInfoArray,*attrFilter.get(),testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            CHECK(1 == certInfoArray.Count());

            RArray<TUid> emptyArray;
            certStoreObj->SetApplicability(*certInfoArray[0],emptyArray,testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrArgument == testExecutor->GetErrorCode());
            certInfoArray.Close();
            emptyArray.Close();
        }

        printf("-errorCasesForSetApplicability\n");
    }
    catch (TestFailedException& ex)
    {
        printf("errorCasesForSetApplicability() ERROR: %s",ex.toString().c_str());
        CHECK(false);
    }
    catch (...)
    {
        printf("errorCasesForSetApplicability() Unexpected exception caught");
        CHECK(false);
    }
}


