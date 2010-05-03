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

using namespace std;
using namespace java::security;

_LIT(KMIDP2TrustRoot, "Java Trust Root");

TEST_GROUP(TokenTests)
{
    TEST_SETUP()
    {
    }

    TEST_TEARDOWN()
    {
    }
};

//
// TEST CASE 30
//
TEST(TokenTests,testsForList)
{
    printf("+testsForList\n");

    try
    {
        auto_ptr<CTestExecutor> testExecutor(new CTestExecutor());
        auto_ptr<CJavaCertStoreTokenType> certStoreTokenType
        (static_cast<CJavaCertStoreTokenType*>(CJavaCertStoreTokenType::New2L()));

        RCPointerArray<HBufC> outList;
        certStoreTokenType->List(outList,testExecutor->GetRequestStatus());
        CActiveScheduler::Start();
        CHECK(KErrNone == testExecutor->GetErrorCode());
        CHECK(1 == outList.Count());
        CHECK(0 == outList[0]->Compare(_L("Java Trust Roots")));
        outList.Close();

        printf("-testsForList\n");
    }
    catch (TestFailedException& ex)
    {
        printf("testsForList() ERROR: %s",ex.toString().c_str());
        CHECK(false);
    }
    catch (...)
    {
        printf("testsForList() Unexpected exception caught");
        CHECK(false);
    }
}

//
// TEST CASE 31
//
TEST(TokenTests,testsForOpenToken)
{
    printf("+testsForOpenToken\n");

    try
    {
        auto_ptr<CTestExecutor> testExecutor(new CTestExecutor());
        auto_ptr<CJavaCertStoreTokenType> certStoreTokenType
        (static_cast<CJavaCertStoreTokenType*>(CJavaCertStoreTokenType::New2L()));

        //# 1
        //Getting token.
        MCTToken* outToken = NULL;
        certStoreTokenType->OpenToken(_L("Java Trust Roots"),outToken,testExecutor->GetRequestStatus());
        CActiveScheduler::Start();
        CHECK(KErrNone == testExecutor->GetErrorCode());
        TCTTokenHandle tokenHandle = outToken->Handle();
        CHECK(0x1234 == tokenHandle.iTokenId);
        outToken->Release();

        //# 2
        //Try to get token using unsupported token type.
        MCTToken* outToken2 = NULL;
        certStoreTokenType->OpenToken(_L("blaah"),outToken2,testExecutor->GetRequestStatus());
        testExecutor->Activate();
        CActiveScheduler::Start();
        CHECK(KErrNotSupported == testExecutor->GetErrorCode());

        //# 3
        //Calling unsupported version from OpenToken() operation.
        certStoreTokenType->OpenToken(tokenHandle,outToken2,testExecutor->GetRequestStatus());
        testExecutor->Activate();
        CActiveScheduler::Start();
        CHECK(KErrNotSupported == testExecutor->GetErrorCode());

        printf("-testsForOpenToken\n");
    }
    catch (TestFailedException& ex)
    {
        printf("testsForOpenToken() ERROR: %s",ex.toString().c_str());
        CHECK(false);
    }
    catch (...)
    {
        printf("testsForOpenToken() Unexpected exception caught");
        CHECK(false);
    }
}

//
// TEST CASE 32
//
TEST(TokenTests,testsForDoGetInterface)
{
    printf("+testsForDoGetInterface\n");

    try
    {
        //Starting java captain.
        TestUtils::StartJavaCaptain();

        auto_ptr<CTestExecutor> testExecutor(new CTestExecutor());

        {
            //# 1
            //Getting non-writable cert store interface.
            auto_ptr<CJavaCertStoreTokenType> certStoreTokenType
            (static_cast<CJavaCertStoreTokenType*>(CJavaCertStoreTokenType::New2L()));
            MCTToken* outToken = NULL;
            certStoreTokenType->OpenToken(_L("Java Trust Roots"),outToken,testExecutor->GetRequestStatus());
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());

            MCTTokenInterface* outInterface = NULL;
            TUid tmpUid = TUid::Uid(KInterfaceCertStore);
            outToken->GetInterface(tmpUid,outInterface,testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            MCTToken& token = outInterface->Token();
            TCTTokenHandle tokenHandle = outToken->Handle();
            CHECK(0x1234 == tokenHandle.iTokenId);
            outInterface->Release();
            outToken->Release();
            CJavaCertStoreTokenType* tmpObj = certStoreTokenType.release();
            tmpObj->Release();
        }

        {
            //# 2
            //Getting writable cert store interface.
            auto_ptr<CJavaCertStoreTokenType> certStoreTokenType
            (static_cast<CJavaCertStoreTokenType*>(CJavaCertStoreTokenType::New2L()));
            MCTToken* outToken = NULL;
            certStoreTokenType->OpenToken(_L("Java Trust Roots"),outToken,testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());

            MCTTokenInterface* outInterface = NULL;
            TUid tmpUid = TUid::Uid(KInterfaceWritableCertStore);
            outToken->GetInterface(tmpUid,outInterface,testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());
            MCTToken& token = outInterface->Token();
            TCTTokenHandle tokenHandle = outToken->Handle();
            CHECK(0x1234 == tokenHandle.iTokenId);
            outInterface->Release();
            outToken->Release();
            CJavaCertStoreTokenType* tmpObj = certStoreTokenType.release();
            tmpObj->Release();
        }

        {
            //# 3
            //Pass incorrect interface uid as argument to DoGetInterface() operation.
            auto_ptr<CJavaCertStoreTokenType> certStoreTokenType
            (static_cast<CJavaCertStoreTokenType*>(CJavaCertStoreTokenType::New2L()));
            MCTToken* outToken = NULL;
            certStoreTokenType->OpenToken(_L("Java Trust Roots"),outToken,testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNone == testExecutor->GetErrorCode());

            MCTTokenInterface* outInterface = NULL;
            TUid tmpUid = TUid::Uid(0x10);
            outToken->GetInterface(tmpUid,outInterface,testExecutor->GetRequestStatus());
            testExecutor->Activate();
            CActiveScheduler::Start();
            CHECK(KErrNotSupported == testExecutor->GetErrorCode());
            outToken->Release();
            CJavaCertStoreTokenType* tmpObj = certStoreTokenType.release();
            tmpObj->Release();
        }

        printf("-testsForDoGetInterface\n");
    }
    catch (TestFailedException& ex)
    {
        printf("testsForDoGetInterface() ERROR: %s",ex.toString().c_str());
        CHECK(false);
    }
    catch (...)
    {
        printf("testsForDoGetInterface() Unexpected exception caught");
        CHECK(false);
    }
}




