/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Tests for JvmStarter class
*
*/

#include <string>
#include <vector>
#include <memory>

#include "coreuitestimpl.h"
#include "dynamiclibloader.h"

#include "TestHarness.h"
#include "testutils.h"
#include "javaoslayer.h"
#include "logger.h"

using namespace java::util;
using namespace java::ui;


class CoreUiTestHandlerImpl : public CoreUiTestHandler
{
public:
    CoreUiTestHandlerImpl() : mUiParams(0)
    {
        reset();
    }
    ~CoreUiTestHandlerImpl()
    {
        delete mUiParams;
        mUiParams = 0;
    }

    virtual void startCalled(const Uid& midletUid, CoreUiParams* uiParams)
    {
        mMidletUid = midletUid;
        if (mUiParams)
        {
            delete mUiParams;
            mUiParams = 0;
        }
        if (uiParams)
        {
            mUiParams = new CoreUiParams();
            *mUiParams = *uiParams;
        }
        mStartCalledCount++;
    }
    virtual void releaseCalled()
    {
        mReleaseCalledCount++;
    }
    virtual void error(const std::string& info)
    {
        CHECKWITHNOTE(false, info)
    }

    void reset()
    {
        mStartCalledCount = 0;
        mReleaseCalledCount = 0;
    }
    int  mStartCalledCount;
    int  mReleaseCalledCount;
    CoreUiParams* mUiParams;
    Uid  mMidletUid;
};

#define UIPARAMSCHECK(check1, check2, fname, info) \
    CHECKWITHNOTE(check1 == check2, info << ": " << fname << " failed: 1=" << check1 << " 2=" << check2);


void compare(const CoreUiParams* uiParams1, const CoreUiParams* uiParams2, const std::string& info)
{
    bool pass = false;
    if ((uiParams1 == 0 && uiParams2 != 0) || (uiParams1 != 0 && uiParams2 == 0))
    {
        CHECKWITHNOTE(false, info << ": pointer mismatch 1=" << std::hex << uiParams1  << ", 2=" << std::hex << uiParams2);
        return;
    }

    if (uiParams1 == 0 && uiParams2 == 0)
    {
        return;
    }
    else if (uiParams1 != uiParams2)
    {
        pass = true;
    }
    CHECKWITHNOTE(pass, info << ": pointer mismatch 1=" << std::hex << uiParams1  << ", 2=" << std::hex << uiParams2);
    UIPARAMSCHECK(uiParams1->getOrientation(), uiParams2->getOrientation(), "getOrientation", info);
    UIPARAMSCHECK(uiParams1->getScreenMode(), uiParams2->getScreenMode(), "getScreenMode", info);
    UIPARAMSCHECK(uiParams1->getImagePath(), uiParams2->getImagePath(), "getImagePath", info);
    UIPARAMSCHECK(uiParams1->isBackgroundStart(), uiParams2->isBackgroundStart(), "isBackgroundStart", info);

}

void checkCallCounts(const CoreUiTestHandlerImpl& testHandler, int start, int release, const std::string& info)
{
    CHECKWITHNOTE(testHandler.mStartCalledCount == start, info <<". Start call count incorrect: " << testHandler.mStartCalledCount
                  <<", expecting: "<<start);
    CHECKWITHNOTE(testHandler.mReleaseCalledCount == release, info << "Release call count incorrect: " << testHandler.mReleaseCalledCount
                  <<", expecting: "<<release);
}

typedef void (*SetTestHandler)(CoreUiTestHandler*);

// Run these tests with tst.bat. These tests require that the test version of the
// javacoreui.dll is compiled
TEST_GROUP(TestCoreUi)
{
    CoreUiTestHandlerImpl testHandler;
    CoreUiParams uiParams;
    Uid uid;
    std::auto_ptr<java::util::DynamicLibLoader> basicCoreUiLoader;
    TEST_SETUP()
    {
        basicCoreUiLoader.reset(new java::util::DynamicLibLoader(COREUI_LIB_NAME));
        SetTestHandler setTestHandler =
            reinterpret_cast<SetTestHandler>(basicCoreUiLoader->getFunction("setTestHandler",
                                             true));
        setTestHandler(&testHandler);
    }

    TEST_TEARDOWN()
    {
    }
};


// Run these tests with tst2.bat. These tests require that the javacoreui.dll
// doesn't exist at all.
TEST_GROUP(NoDllCoreUiTest)
{
    Uid uid;
    TEST_SETUP()
    {
    }

    TEST_TEARDOWN()
    {
    }
};

TEST(TestCoreUi, testBasic)
{
    std::auto_ptr<java::util::DynamicLibLoader> coreUiLoader;
    // __BREAKPOINT();
    CHECKWITHNOTE(CoreUi::start(coreUiLoader, uid, &uiParams), "testBasic, start");
    checkCallCounts(testHandler, 1, 0, "Before release");
    CHECKWITHNOTE(CoreUi::releaseUi(coreUiLoader), "testBasic release");
    checkCallCounts(testHandler, 1, 1, "After release");
    compare(testHandler.mUiParams, &uiParams, "Final check basic");
}

TEST(TestCoreUi, testNullUiParams)
{
    std::auto_ptr<java::util::DynamicLibLoader> coreUiLoader;
    CHECKWITHNOTE(CoreUi::start(coreUiLoader, uid), "testNullUiParams, start");
    checkCallCounts(testHandler, 1, 0, "Before release");
    CHECKWITHNOTE(CoreUi::releaseUi(coreUiLoader), "testNullUiParams release");
    checkCallCounts(testHandler, 1, 1, "After release");
    compare(testHandler.mUiParams, 0, "Final check Null ui");
}

TEST(TestCoreUi, testModifiedOrientationArgs)
{
    std::auto_ptr<java::util::DynamicLibLoader> coreUiLoader;

    uiParams.setOrientation(UNDEFINED);
    CHECKWITHNOTE(CoreUi::start(coreUiLoader, uid, &uiParams), "testModifiedOrientationArgs, start1");
    checkCallCounts(testHandler, 1, 0, "Before release");
    compare(testHandler.mUiParams, &uiParams, "OrientationArgs UNDEFINED");

    uiParams.setOrientation(LANDSCAPE);
    CHECKWITHNOTE(CoreUi::start(coreUiLoader, uid, &uiParams), "testModifiedOrientationArgs, start2");
    checkCallCounts(testHandler, 2, 0, "Before release");
    compare(testHandler.mUiParams, &uiParams, "OrientationArgs LANDSCAPE");

    uiParams.setOrientation(PORTRAIT);
    CHECKWITHNOTE(CoreUi::start(coreUiLoader, uid, &uiParams), "testModifiedOrientationArgs, start3");
    checkCallCounts(testHandler, 3, 0, "Before release");
    compare(testHandler.mUiParams, &uiParams, "OrientationArgs PORTRAIT");

    CHECKWITHNOTE(CoreUi::releaseUi(coreUiLoader), "testModifiedOrientationArgs release");
    checkCallCounts(testHandler, 3, 1, "After release");
}

TEST(TestCoreUi, testModifiedScreenModeArgs)
{
    std::auto_ptr<java::util::DynamicLibLoader> coreUiLoader;

    uiParams.setScreenMode(NO_START_SCREEN);
    CHECKWITHNOTE(CoreUi::start(coreUiLoader, uid, &uiParams), "testModifiedScreenModeArgs, start1");
    checkCallCounts(testHandler, 1, 0, "Before release");
    compare(testHandler.mUiParams, &uiParams, "ScreenModeArgs NO_START_SCREEN");

    uiParams.setScreenMode(DEFAULT_START_SCREEN);
    CHECKWITHNOTE(CoreUi::start(coreUiLoader, uid, &uiParams), "testModifiedScreenModeArgs, start2");
    checkCallCounts(testHandler, 2, 0, "Before release");
    compare(testHandler.mUiParams, &uiParams, "ScreenModeArgs DEFAULT_START_SCREEN");

    uiParams.setScreenMode(MIDLET_DEFINED_SCREEN);
    CHECKWITHNOTE(CoreUi::start(coreUiLoader, uid, &uiParams), "testModifiedScreenModeArgs, start3");
    checkCallCounts(testHandler, 3, 0, "Before release");
    compare(testHandler.mUiParams, &uiParams, "ScreenModeArgs MIDLET_DEFINED_SCREEN");

    uiParams.setScreenMode(USER_DEFINED_SCREEN);
    CHECKWITHNOTE(CoreUi::start(coreUiLoader, uid, &uiParams), "testModifiedScreenModeArgs, start4");
    checkCallCounts(testHandler, 4, 0, "Before release");
    compare(testHandler.mUiParams, &uiParams, "ScreenModeArgs USER_DEFINED_SCREEN");

    CHECKWITHNOTE(CoreUi::releaseUi(coreUiLoader), "testModifiedScreenModeArgs release");
    checkCallCounts(testHandler, 4, 1, "After release");
}

TEST(TestCoreUi, testModifiedImagePathArgs)
{
    std::auto_ptr<java::util::DynamicLibLoader> coreUiLoader;

    uiParams.setImagePath(L"c:\\logs\\java\\");
    CHECKWITHNOTE(CoreUi::start(coreUiLoader, uid, &uiParams), "testModifiedImagePathArgs start");
    checkCallCounts(testHandler, 1, 0, "Before release");
    compare(testHandler.mUiParams, &uiParams, "ImagePathArgs");

    CHECKWITHNOTE(CoreUi::releaseUi(coreUiLoader), "testModifiedImagePathArgs release");
    checkCallCounts(testHandler, 1, 1, "After release");
}

TEST(TestCoreUi, testModifiedForeGroundArgs)
{
    std::auto_ptr<java::util::DynamicLibLoader> coreUiLoader;

    uiParams.setBackgroundStart(true);
    CHECKWITHNOTE(CoreUi::start(coreUiLoader, uid, &uiParams), "testModifiedForeGroundArgs start1");
    checkCallCounts(testHandler, 1, 0, "Before release");
    compare(testHandler.mUiParams, &uiParams, "ForeGround");

    uiParams.setBackgroundStart(false);
    CHECKWITHNOTE(CoreUi::start(coreUiLoader, uid, &uiParams), "testModifiedForeGroundArgs start2");
    checkCallCounts(testHandler, 2, 0, "Before release");
    compare(testHandler.mUiParams, &uiParams, "ForeGround");

    CHECKWITHNOTE(CoreUi::releaseUi(coreUiLoader), "testModifiedForeGroundArgs release");
    checkCallCounts(testHandler, 2, 1, "After release");
}

TEST(TestCoreUi, testNoStartCall)
{
    std::auto_ptr<java::util::DynamicLibLoader> coreUiLoader;
    CHECKWITHNOTE(CoreUi::releaseUi(coreUiLoader), "testNoStartCall release");
    checkCallCounts(testHandler, 0, 1, "After release");
}

TEST(NoDllCoreUiTest, testBasic)
{
    std::auto_ptr<java::util::DynamicLibLoader> coreUiLoader;
    CHECKWITHNOTE(!CoreUi::start(coreUiLoader, uid), "testBasic NO_DLL start");
    CHECKWITHNOTE(!CoreUi::releaseUi(coreUiLoader), "testBasic NO_DLL release");
}

TEST(NoDllCoreUiTest, testOnlyStart)
{
    std::auto_ptr<java::util::DynamicLibLoader> coreUiLoader;
    CHECKWITHNOTE(!CoreUi::start(coreUiLoader, uid), "testOnlyStart NO_DLL start");
}

TEST(NoDllCoreUiTest, testOnlyRelease)
{
    std::auto_ptr<java::util::DynamicLibLoader> coreUiLoader;
    CHECKWITHNOTE(!CoreUi::releaseUi(coreUiLoader), "testOnlyRelease NO_DLL release");
}
