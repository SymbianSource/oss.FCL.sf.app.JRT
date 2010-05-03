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
#include "javauid.h"
#include "javasymbianoslayer.h"

using namespace std;
using namespace java::util;

TEST_GROUP(UidTests)
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
// This test verifies that angle bracket version from the UID [12345678] is managed properly.
//
TEST(UidTests,uidtoTuidTests1)
{
    printf("+uidtoTuidTests1\n");

    try
    {
        //# 1
        //Maximum value of the uid.
        {
            java::util::Uid myUid(L"[EFFFFFFF]");
            TUid tUid;
            TInt err = uidToTUid(myUid,tUid);
            CHECK(KErrNone == err);
            CHECK(4026531839 == tUid.iUid);
        }

        //# 2
        {
            java::util::Uid myUid(L" [efffffff] ");
            TUid tUid;
            TInt err = uidToTUid(myUid,tUid);
            CHECK(KErrNone == err);
            CHECK(4026531839 == tUid.iUid);
        }

        //# 3
        {
            java::util::Uid myUid(L"\t[EFFFFFFF]\n");
            TUid tUid;
            TInt err = uidToTUid(myUid,tUid);
            CHECK(KErrNone == err);
            CHECK(4026531839 == tUid.iUid);
        }

        //# 4
        {
            java::util::Uid myUid(L" [00000000] ");
            TUid tUid;
            TInt err = uidToTUid(myUid,tUid);
            CHECK(KErrNone == err);
            CHECK(0 == tUid.iUid);
        }

        //Error cases.

        //# 5
        {
            java::util::Uid myUid(L" [000t0000] ");
            TUid tUid;
            TInt err = uidToTUid(myUid,tUid);
            CHECK(KErrArgument == err);
        }

        //# 6
        //More than 8 digits.
        {
            java::util::Uid myUid(L"[EFFFFFFFF]");
            TUid tUid;
            TInt err = uidToTUid(myUid,tUid);
            CHECK(KErrArgument == err);
        }

        //# 7
        //Less than 8 digits.
        {
            java::util::Uid myUid(L"[EFFFFFF]");
            TUid tUid;
            TInt err = uidToTUid(myUid,tUid);
            CHECK(KErrArgument == err);
        }

        //# 8
        //Empty string.
        {
            java::util::Uid myUid(L"");
            TUid tUid;
            TInt err = uidToTUid(myUid,tUid);
            CHECK(KErrArgument == err);
        }

        //# 9
        //Unsupported brackets.
        {
            java::util::Uid myUid(L"(EFFFFFFF)");
            TUid tUid;
            TInt err = uidToTUid(myUid,tUid);
            CHECK(KErrArgument == err);
        }

        printf("-uidtoTuidTests1\n");
    }
    catch (...)
    {
        printf("uidtoTuidTests1() Unexpected exception caught");
        CHECK(false);
    }
}

//
// TEST CASE 2
// This test verifies that "0x123456ab" version from the UID is managed properly.
//
TEST(UidTests,uidtoTuidTests2)
{
    printf("+uidtoTuidTests2\n");

    try
    {
        //# 1
        //Maximum value of the uid.
        {
            java::util::Uid myUid(L"0xEFFFFFFF");
            TUid tUid;
            TInt err = uidToTUid(myUid,tUid);
            CHECK(KErrNone == err);
            CHECK(4026531839 == tUid.iUid);
        }

        //# 2
        {
            java::util::Uid myUid(L" 0xEFFFFFFF ");
            TUid tUid;
            TInt err = uidToTUid(myUid,tUid);
            CHECK(KErrNone == err);
            CHECK(4026531839 == tUid.iUid);
        }

        //# 3
        {
            java::util::Uid myUid(L"\n 0xEFFFFFFF \t");
            TUid tUid;
            TInt err = uidToTUid(myUid,tUid);
            CHECK(KErrNone == err);
            CHECK(4026531839 == tUid.iUid);
        }

        //# 4
        {
            java::util::Uid myUid(L" 0x00000000 ");
            TUid tUid;
            TInt err = uidToTUid(myUid,tUid);
            CHECK(KErrNone == err);
            CHECK(0 == tUid.iUid);
        }

        //Error cases.

        //# 5
        {
            java::util::Uid myUid(L"0x000t0000");
            TUid tUid;
            TInt err = uidToTUid(myUid,tUid);
            CHECK(KErrArgument == err);
        }

        //# 6
        //More than 8 digits.
        {
            java::util::Uid myUid(L"0xEFFFFFFFF");
            TUid tUid;
            TInt err = uidToTUid(myUid,tUid);
            CHECK(KErrArgument == err);
        }

        //# 7
        //Less than 8 digits.
        {
            java::util::Uid myUid(L"0xEFFFFFF");
            TUid tUid;
            TInt err = uidToTUid(myUid,tUid);
            CHECK(KErrArgument == err);
        }

        printf("-uidtoTuidTests2\n");
    }
    catch (...)
    {
        printf("uidtoTuidTests2() Unexpected exception caught");
        CHECK(false);
    }
}

//
// TEST CASE 3
// This test verifies that "123456ab" version from the UID is managed properly.
//
TEST(UidTests,uidtoTuidTests3)
{
    printf("+uidtoTuidTests3\n");

    try
    {
        //# 1
        //Maximum value of the uid.
        {
            java::util::Uid myUid(L"EFFFFFFF");
            TUid tUid;
            TInt err = uidToTUid(myUid,tUid);
            CHECK(KErrNone == err);
            CHECK(4026531839 == tUid.iUid);
        }

        //# 2
        {
            java::util::Uid myUid(L" EFFFFFFF ");
            TUid tUid;
            TInt err = uidToTUid(myUid,tUid);
            CHECK(KErrNone == err);
            CHECK(4026531839 == tUid.iUid);
        }

        //# 3
        {
            java::util::Uid myUid(L"\n EFFFFFFF \t");
            TUid tUid;
            TInt err = uidToTUid(myUid,tUid);
            CHECK(KErrNone == err);
            CHECK(4026531839 == tUid.iUid);
        }

        //# 4
        {
            java::util::Uid myUid(L" 00000000 ");
            TUid tUid;
            TInt err = uidToTUid(myUid,tUid);
            CHECK(KErrNone == err);
            CHECK(0 == tUid.iUid);
        }

        //Error cases.

        //# 5
        {
            java::util::Uid myUid(L"000t0000");
            TUid tUid;
            TInt err = uidToTUid(myUid,tUid);
            CHECK(KErrArgument == err);
        }

        //# 6
        //More than 8 digits.
        {
            java::util::Uid myUid(L"EFFFFFFFF");
            TUid tUid;
            TInt err = uidToTUid(myUid,tUid);
            CHECK(KErrArgument == err);
        }

        //# 7
        //Less than 8 digits.
        {
            java::util::Uid myUid(L"EFFFFFF");
            TUid tUid;
            TInt err = uidToTUid(myUid,tUid);
            CHECK(KErrArgument == err);
        }

        printf("-uidtoTuidTests3\n");
    }
    catch (...)
    {
        printf("uidtoTuidTests3() Unexpected exception caught");
        CHECK(false);
    }
}

//
// TEST CASE 4
// Tests for TUidToUid(const TUid& aId,java::util::Uid& aOutUid) method.
//
TEST(UidTests,tuidToUidTests1)
{
    printf("+tuidToUidTests1\n");

    try
    {
        //# 1
        {
            TUid tuid = TUid::Uid(4026531839);
            Uid retUid;
            retUid = TUidToUid(tuid,retUid);
            CHECK(false == retUid.isEmpty());
            std::wstring tmpStr(retUid.toString());
            std::string tmpStr2(tmpStr.begin(),tmpStr.end());
            CHECK(0 == retUid.toString().compare(L"[efffffff]"));
        }

        //# 2
        {
            TUid tuid = TUid::Uid(0);
            Uid retUid;
            retUid = TUidToUid(tuid,retUid);
            CHECK(true == retUid.isEmpty());
            std::wstring tmpStr(retUid.toString());
            std::string tmpStr2(tmpStr.begin(),tmpStr.end());
            CHECK(0 == retUid.toString().compare(L""));
        }

        //# 3
        //Null constructed TUid.
        {
            TUid tuid = TUid::Null();
            Uid retUid;
            retUid = TUidToUid(tuid,retUid);
            CHECK(true == retUid.isEmpty());
        }

        printf("-tuidToUidTests1\n");
    }
    catch (...)
    {
        printf("tuidToUidTests1() Unexpected exception caught");
        CHECK(false);
    }
}

