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
* Description:  Tests for UeiArgsParser class
*
*/

#include <string>
#include <vector>

#include "ueiargsparser.h"

#include "TestHarness.h"
#include "logger.h"

using java::runtime::UeiArgsParser;
using namespace std;

class ArgsParserTester : public UeiArgsParser
{
public:
    // give access to base class's protected functions
    using UeiArgsParser::getArgs;
    using UeiArgsParser::isVerbose;
    using UeiArgsParser::isRundjwp;
    using UeiArgsParser::convertVerbose;
    using UeiArgsParser::convertRundjwp;
    using UeiArgsParser::hasOption;
    using UeiArgsParser::trimTrailingSpaces;
};

TEST_GROUP(TestUeiArgsParser)
{
    ArgsParserTester parser;
    TEST_SETUP()
    {
    }

    TEST_TEARDOWN()
    {
    }
};



TEST(TestUeiArgsParser, convertUeiArgsToJvmArgs)
{
    EXPECT_N_LEAKS(12);
    wstring uei = L"-classpath midlet.jar"
                  L" -Xverbose:allocation,gc,gcverbose,class,classverbose,verifier,stackmaps,"
                  L"bytecodes,frames,stackchunks,exceptions,events,threading,monitors,networking"
                  L" -Xdebug -Xrunjdwp:server=y,address=localhost:8000"
                  L" -Dproperty=value -Xdescriptor:jad-file -Xdevice:device-name -Xheapsize:2M";
    wstring expectedResult = L"-verbose:gc,class -Xrunjdwp:server=y,address=localhost:8000";

    wstring result = parser.convertUeiArgsToJvmArgs(uei);
    CHECK(result.compare(expectedResult) == 0);
}

TEST(TestUeiArgsParser, getArgs)
{
    EXPECT_N_LEAKS(4);

    std::vector<std::wstring> result = parser.getArgs(L"");
    CHECK(result.size() == 0);

    result = parser.getArgs(L"-Xverbose");
    CHECK(result.size() == 1);

    result = parser.getArgs(L"-Xverbose -Xrunjdwp:server=y,address=localhost:8000");
    CHECK(result.size() == 2);

    result = parser.getArgs(L"-Xdebug -Xverbose -Xrunjdwp:server=y,address=localhost:8000");
    CHECK(result.size() == 3);
}

TEST(TestUeiArgsParser, isVerbose)
{
    wstring str = L"-Xverbose";
    CHECK(parser.isVerbose(str) == true);

    str = L"-Xverbose:all,allocation,gc,gcverbose,class,classverbose,verifier,stackmaps,"
          L"bytecodes,frames,stackchunks,exceptions,events,threading,monitors,networking"
          L"-Xdebug -Xrunjdwp:server=y,address=localhost:8000";
    CHECK(parser.isVerbose(str) == true);

    str = L"-verbose:all,allocation,gc,gcverbose,class,classverbose,verifier,stackmaps,"
          L"bytecodes,frames,stackchunks,exceptions,events,threading,monitors,networking"
          L"-Xdebug -Xrunjdwp:server=y,address=localhost:8000";
    CHECK(parser.isVerbose(str) == false);

    str = L" -Xverbose";
    CHECK(parser.isVerbose(str) == false);
    str = L"-xverbose";
    CHECK(parser.isVerbose(str) == false);
    str = L"";
    CHECK(parser.isVerbose(str) == false);
}

TEST(TestUeiArgsParser, isRundjwp)
{
    wstring str = L"-Xrunjdwp";
    CHECK(parser.isRundjwp(str) == true);

    str = L"-Xrunjdwp:server=y,address=localhost:8000";
    CHECK(parser.isRundjwp(str) == true);

    str = L"-drunjdwp:server=y,address=localhost:8000";
    CHECK(parser.isRundjwp(str) == false);

    str = L" -Xrunjdwp";
    CHECK(parser.isRundjwp(str) == false);
    str = L"-drunjdwp";
    CHECK(parser.isRundjwp(str) == false);
    str = L"";
    CHECK(parser.isRundjwp(str) == false);
}

TEST(TestUeiArgsParser, convertVerbose)
{
    EXPECT_N_LEAKS(7);

    wstring str = L"-Xverbose:all,allocation,gc,gcverbose,class,classverbose,verifier,stackmaps,"
                  L"bytecodes,frames.stackchunks,exceptions,events,threading,monitors,networking";
    wstring result = parser.convertVerbose(str);
    CHECK(result.compare(L"-verbose:gc,class,stack,sizes") == 0);

    // no all
    str = L"-Xverbose:allocation,gc,gcverbose,class,classverbose,verifier,stackmaps,"
          L"bytecodes,frames.stackchunks,exceptions,events,threading,monitors,networking";
    result = parser.convertVerbose(str);
    CHECK(result.compare(L"-verbose:gc,class") == 0);

    // only all
    str = L"-Xverbose:all,allocation,gcverbose,classverbose,verifier,stackmaps,"
          L"bytecodes,frames.stackchunks,exceptions,events,threading,monitors,networking";
    result = parser.convertVerbose(str);
    CHECK(result.compare(L"-verbose:gc,class,stack,sizes") == 0);

    // only gc
    str = L"-Xverbose:allocation,gc,gcverbose,classverbose,verifier,stackmaps,"
          L"bytecodes,frames.stackchunks,exceptions,events,threading,monitors,networking";
    result = parser.convertVerbose(str);
    CHECK(result.compare(L"-verbose:gc") == 0);

    // only class
    str = L"-Xverbose:allocation,gcverbose,class,classverbose,verifier,stackmaps,"
          L"bytecodes,frames.stackchunks,exceptions,events,threading,monitors,networking";
    result = parser.convertVerbose(str);
    CHECK(result.compare(L"-verbose:class") == 0);

    // no supported options
    str = L"-Xverbose:allocation,gcverbose,classverbose,verifier,stackmaps,"
          L"bytecodes,frames.stackchunks,exceptions,events,threading,monitors,networking";
    result = parser.convertVerbose(str);
    CHECK(result.compare(L"-verbose") == 0);

    // only Xverbose
    str = L"-Xverbose";
    result = parser.convertVerbose(str);
    CHECK(result.compare(L"-verbose") == 0);

}

TEST(TestUeiArgsParser, convertRundjwp)
{
    wstring str = L"-Xrunjdwp:server=y,address=localhost:8000";
    wstring result = parser.convertRundjwp(str);
    CHECK(result.compare(str) == 0);
}

TEST(TestUeiArgsParser, hasOption)
{
    wstring str = L"-classpath midlet.jar"
                  L"-Xverbose:all,allocation,gc,gcverbose,class,classverbose,verifier,stackmaps,"
                  L"bytecodes,frames,stackchunks,exceptions,events,threading,monitors,networking"
                  L"-Xdebug -Xrunjdwp:server=y,address=localhost:8000";

    CHECK(parser.hasOption(str, L"all") == true);
    CHECK(parser.hasOption(str, L"class") == true);
    CHECK(parser.hasOption(str, L"gc") == true);
    CHECK(parser.hasOption(str, L"8000") == true);

    CHECK(parser.hasOption(str, L"alloca") == false);
    CHECK(parser.hasOption(str, L"classv") == false);
    CHECK(parser.hasOption(str, L"stack") == false);

    CHECK(parser.hasOption(str, L"ALL") == false);
    CHECK(parser.hasOption(str, L"CLASS") == false);
    CHECK(parser.hasOption(str, L"GC") == false);
}

TEST(TestUeiArgsParser, trimTrailingSpaces)
{
    // string has whitespaces
    wstring str = L"erase trailing white-spaces   \n";
    wstring whitespaces = L" \t\f\v\n\r";
    wstring result = parser.trimTrailingSpaces(str, whitespaces);
    CHECK(result.compare(L"erase trailing white-spaces") == 0);

    str = L"erase trailing white-spaces ";
    result = parser.trimTrailingSpaces(str, L" ");
    CHECK(result.compare(L"erase trailing white-spaces") == 0);

    str = L"-verbose: ";
    result = parser.trimTrailingSpaces(str, L": ");
    CHECK(result.compare(L"-verbose") == 0);

    str = L"-verbose:gc,class,";
    result = parser.trimTrailingSpaces(str, L",");
    CHECK(result.compare(L"-verbose:gc,class") == 0);

    // string does not have whitespaces
    str = L"erase trailing white-spaces";
    result = parser.trimTrailingSpaces(str, whitespaces);
    CHECK(result.compare(L"erase trailing white-spaces") == 0);

    str = L"erase trailing white-spaces";
    result = parser.trimTrailingSpaces(str, L"kjh");
    CHECK(result.compare(L"erase trailing white-spaces") == 0);

    str = L"erase trailing white-spaces";
    result = parser.trimTrailingSpaces(str, L"e");
    CHECK(result.compare(L"erase trailing white-spaces") == 0);

    // empty string
    wstring empty;
    result = parser.trimTrailingSpaces(empty, whitespaces);
    CHECK(result.compare(L"") == 0);
    empty = L"";
    result = parser.trimTrailingSpaces(empty, L" ");
    CHECK(result.compare(L"") == 0);
}
