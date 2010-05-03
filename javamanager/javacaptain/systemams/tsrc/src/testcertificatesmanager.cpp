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

#include "TestHarness.h"
#include <s32mem.h>
#include "clientsession.h"

TEST_GROUP(TestCertificatesManager)
{
    RClientSession iSession;

    TEST_SETUP()
    {
    }

    TEST_TEARDOWN()
    {
    }
};

TEST(TestCertificatesManager, TestCertificatesManagerL)
{
    // initializations
    TRequestStatus status;
    _LIT(KServerName, "!SystemAMS.SecurityAdmin");
    _LIT(KDummy, "dummy");
    TPckgBuf<TUint32> count;
    HBufC8* buffer = NULL;
    CBufFlat* info = NULL;
    // connect
    CHECK(iSession.Connect(KServerName) == KErrNone);
    iSession.Count(status, KDummy, count);
    User::WaitForRequest(status);
    CHECK(status.Int() == KErrNone && count() != 0);
    // info
    buffer = HBufC8::NewL(4096);
    CleanupStack::PushL(buffer);
    TPtr8 ptr = buffer->Des();
    iSession.Info(status, KDummy, ptr);
    User::WaitForRequest(status);
    CHECK(status.Int() == KErrNone);
    info = CBufFlat::NewL(ptr.Length());
    CleanupStack::PushL(info);
    info->SetReserveL(ptr.Length());
    info->InsertL(0, ptr);
    RBufReadStream reader(*info, 0);
    TInt cnt = reader.ReadUint32L();
    CHECK(cnt > 0);
    for (int i=0; i<cnt; i++)
    {
        TInt id = reader.ReadUint32L();
        TInt flags = reader.ReadUint32L();
        TInt length = reader.ReadUint32L();
        HBufC8* cert = HBufC8::NewL(4096);
        CleanupStack::PushL(cert);
        TPtr8 certPtr = cert->Des();
        iSession.Certificate(status, id, certPtr);
        User::WaitForRequest(status);
        CHECK(length > 0 && certPtr.Length() == length);
        CleanupStack::PopAndDestroy(cert);
        iSession.Disable(status, id);
        User::WaitForRequest(status);
        CHECK(status.Int() == KErrNone || status.Int() == KErrArgument);
        iSession.Delete(status, id);
        User::WaitForRequest(status);
        CHECK(status.Int() == KErrNone || status.Int() == KErrArgument);
        iSession.Enable(status, id);
        User::WaitForRequest(status);
        CHECK(status.Int() == KErrNone || status.Int() == KErrArgument);
    }
    HBufC8* tmpCert = HBufC8::NewL(4096);
    CleanupStack::PushL(tmpCert);
    TPtr8 tmpCertPtr = tmpCert->Des();
    iSession.Certificate(status, -1, tmpCertPtr);
    User::WaitForRequest(status);
    CHECK(status.Int() == KErrNotFound);
    CleanupStack::PopAndDestroy(tmpCert);
    CleanupStack::PopAndDestroy(info);
    CleanupStack::PopAndDestroy(buffer);
}
