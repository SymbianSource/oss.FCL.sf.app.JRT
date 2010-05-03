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

#include "TestHarness.h"

#include "javacommonutils.h"

using namespace std;
using namespace java::util;

TEST_GROUP(TestBase64)
{
    TEST_SETUP()
    {
    }

    TEST_TEARDOWN()
    {
    }
};

/**
 * Test JavaCommonUtils::base64encode().
 *
 * 1. Test encode string.
 * 2. Test encode empty string.
 * 3. Test encode hard string.
 * 4. Test encode not supported chars. It is tested that encoding does
 *    not panic.
 * 5. Test encode one char string.
 */
TEST(TestBase64, testBase64Encoding)
{
    // 1. Test encode string.
    string str = "PleaseEncodeMe";
    string encodedStr = JavaCommonUtils::base64encode(str);
    string refStr = "UGxlYXNlRW5jb2RlTWU=";
    CHECK(refStr == encodedStr);

    // 2. Test encode empty string.
    str = "";
    encodedStr = JavaCommonUtils::base64encode(str);
    refStr = "";
    CHECK(refStr == encodedStr);

    // 3. Test encode hard string "a. b"c#d&G"
    str = "aAzZ+/";
    encodedStr = JavaCommonUtils::base64encode(str);
    refStr = "YUF6Wisv";
    CHECK(refStr == encodedStr);

    // 4. Test encode not supported chars.
    str = "!\"#¤%&/()=?´´'*äö-.,;:_\\<>|@£$€";
    encodedStr = JavaCommonUtils::base64encode(str);

    // 5. Test encode one char string.
    str = "x";
    encodedStr = JavaCommonUtils::base64encode(str);
    refStr = "eA==";
    CHECK(refStr == encodedStr);
}

/**
 * Test JavaCommonUtils::base64encode().
 *
 * 1. Test decode valid string.
 * 2. Test decode empty string.
 * 3. Test decode hard string.
 * 3. Test decode not supported chars. It is tested that encoding does
 *    not panic.
 * 4. Test decode one char.
 */
TEST(TestBase64, base64decode)
{
    // 1. Test encode string.
    string str = "UGxlYXNlRW5jb2RlTWU=";
    string encodedStr = JavaCommonUtils::base64decode(str);
    string refStr = "PleaseEncodeMe";
    CHECK(refStr == encodedStr);

    // 2. Test encode empty string.
    str = "";
    encodedStr = JavaCommonUtils::base64decode(str);
    refStr = "";
    CHECK(refStr == encodedStr);

    // 3. Test encode hard string.
    str = "YUF6Wisv";
    encodedStr = JavaCommonUtils::base64decode(str);
    refStr = "aAzZ+/";
    CHECK(refStr == encodedStr);

    // 4. Test encode not supported chars.
    str = "!\"#¤%&/()=?´´'*äö-.,;:_\\<>|@£$€";
    encodedStr = JavaCommonUtils::base64decode(str);

    // 5. Test encode one char string.
    str = "eA==";
    encodedStr = JavaCommonUtils::base64decode(str);
    refStr = "x";
    CHECK(refStr == encodedStr);

}
