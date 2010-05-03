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
* Description:  Tests for Java Debug API
*
*/

#include <memory>
#include <flogger.h>

#include "TestHarness.h"
#include "javadiagnostic.h"
#include "javaredirector.h"
#include "diagnosticimpl.h"

#include "logger.h"

using namespace java::debug;
using namespace java::util;

_LIT8(KText1,"");
_LIT8(KText2," ");
_LIT8(KText3,"\n");
_LIT8(KText4,"hello world\r\n");
_LIT8(KText5,"this is\r\n multiline\r\n example string with\t tabs\r\n");
_LIT8(KText6,"very long string                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      indeed\r\n");
_LIT8(KText7,"0123456789abcdefghijklmnopqrstyvwxyz\r\n");
_LIT8(KText8,"*-.,/\"%&()=?\r\n");

class TestListener: public DiagnosticListener
{
public:
    TestListener() : mSystemOutCount(0), mSystemErrCount(0), mLogCount(0)
    {
        mData = HBufC8::New(2000);
    };
    ~TestListener()
    {
        delete mData;
    };

    virtual void systemOut(const TDesC8& aData)
    {
        *mData = aData;
        mSystemOutCount++;
    };
    virtual void systemErr(const TDesC8& aData)
    {
        *mData = aData;
        mSystemErrCount++;
    };
    virtual void log(const TDesC8 &aData)
    {
        *mData = aData;
        mLogCount++;
    };

    bool operator==(const TDesC8& aRhs) const
    {
        if (aRhs.Compare(mData->Des()) == 0)
            return true;
        return false;
    };

    TInt mSystemOutCount;
    TInt mSystemErrCount;
    TInt mLogCount;

private:
    HBufC8* mData;
};

class LoggingListener : public TestListener
{
public:
    virtual void systemOut(const TDesC8& aData)
    {
        flog(aData);
        TestListener::systemOut(aData);
    };
    virtual void systemErr(const TDesC8& aData)
    {
        flog(aData);
        TestListener::systemErr(aData);
    };
    virtual void log(const TDesC8& aData)
    {
        flog(aData);
        TestListener::log(aData);
    };

protected:
    virtual void flog(const TDesC8& aData)
    {
        _LIT(KLogDirectory, "debug");   // create e.g. c:\logs\java to get logs
        _LIT(KLogFileName, "javadiagnostic.txt");
        RFileLogger::Write(KLogDirectory, KLogFileName, EFileLoggingModeAppendRaw, aData);
    };
};

class DiagnosticImplTester : public DiagnosticImpl
{
public:
    using DiagnosticImpl::DoCancel;
    using DiagnosticImpl::RunError;
};

TEST_GROUP(TestJavaDiagnostic)
{
    TEST_SETUP()
    {
    }

    TEST_TEARDOWN()
    {
    }
};


TEST(TestJavaDiagnostic, createInstance)
{
    std::auto_ptr<JavaDiagnostic> jd(JavaDiagnostic::createInstance());
    CHECK(jd.get());
}

TEST(TestJavaDiagnostic, setRemoveListener)
{
    std::auto_ptr<JavaDiagnostic> jd(JavaDiagnostic::createInstance());
    CHECK(jd.get());

    TestListener listener;
    TInt rc = jd->setDiagnosticListener(listener);
    CHECK(rc == KErrNone);
    jd->removeDiagnosticListener();
}

TEST(TestJavaDiagnostic, setRemoveListenerTwice)
{
    std::auto_ptr<JavaDiagnostic> jd(JavaDiagnostic::createInstance());
    CHECK(jd.get());

    TestListener listener;
    TInt rc = jd->setDiagnosticListener(listener);
    CHECK(rc == KErrNone);
    TestListener listener2;
    rc = jd->setDiagnosticListener(listener2);
    CHECK(rc == KErrAlreadyExists);

    jd->removeDiagnosticListener();
    jd->removeDiagnosticListener();
}

TEST(TestJavaDiagnostic, noRemoveListener)
{
    std::auto_ptr<JavaDiagnostic> jd(JavaDiagnostic::createInstance());
    CHECK(jd.get());

    TestListener listener;
    jd->setDiagnosticListener(listener);
}

TEST(TestJavaDiagnostic, noSetListener)
{
    std::auto_ptr<JavaDiagnostic> jd(JavaDiagnostic::createInstance());
    CHECK(jd.get());

    jd->removeDiagnosticListener();
}

TEST(TestJavaDiagnostic, serverNotRunning)
{
    Redirector r;
    TInt rc = r.connect();
    CHECK(rc != KErrNone);

    rc = r.systemOut(KText1);
    CHECK(rc != KErrNone);
    rc = r.systemErr(KText1);
    CHECK(rc != KErrNone);
    rc = Redirector::log(KText1);
    CHECK(rc != KErrNone);

    r.close();
}

TEST(TestJavaDiagnostic, redirectorNotConnected)
{
    Redirector r;

    TInt rc = r.systemOut(KText1);
    CHECK(rc != KErrNone);
    rc = r.systemErr(KText1);
    CHECK(rc != KErrNone);
    rc = Redirector::log(KText1);
    CHECK(rc != KErrNone);

    r.close();
}

TEST(TestJavaDiagnostic, alreadyRunning)
{
    std::auto_ptr<JavaDiagnostic> jd1(JavaDiagnostic::createInstance());
    CHECK(jd1.get());

    TestListener listener;
    TInt rc = jd1->setDiagnosticListener(listener);
    CHECK(rc == KErrNone);

    std::auto_ptr<JavaDiagnostic> jd2(JavaDiagnostic::createInstance());
    CHECK(jd2.get());

    rc = jd2->setDiagnosticListener(listener);
    CHECK(rc != KErrNone);
}


TEST(TestJavaDiagnostic, systemOut)
{
    std::auto_ptr<JavaDiagnostic> jd(JavaDiagnostic::createInstance());
    CHECK(jd.get());

    TestListener listener;
    jd->setDiagnosticListener(listener);

    Redirector r;
    TInt rc = r.connect();
    CHECK(rc == KErrNone);

    rc = r.systemOut(KText1);
    CHECK(rc == KErrNone);
    CHECK(listener == KText1);
    CHECK(listener.mSystemOutCount == 1);

    rc = r.systemOut(KText2);
    CHECK(rc == KErrNone);
    CHECK(listener == KText2);
    CHECK(listener.mSystemOutCount == 2);

    rc = r.systemOut(KText3);
    CHECK(rc == KErrNone);
    CHECK(listener == KText3);
    CHECK(listener.mSystemOutCount == 3);

    rc = r.systemOut(KText4);
    CHECK(rc == KErrNone);
    CHECK(listener == KText4);
    CHECK(listener.mSystemOutCount == 4);

    rc = r.systemOut(KText5);
    CHECK(rc == KErrNone);
    CHECK(listener == KText5);
    CHECK(listener.mSystemOutCount == 5);

    rc = r.systemOut(KText6);
    CHECK(rc == KErrNone);
    CHECK(listener == KText6);
    CHECK(listener.mSystemOutCount == 6);

    rc = r.systemOut(KText7);
    CHECK(rc == KErrNone);
    CHECK(listener == KText7);
    CHECK(listener.mSystemOutCount == 7);

    rc = r.systemOut(KText8);
    CHECK(rc == KErrNone);
    CHECK(listener == KText8);
    CHECK(listener.mSystemOutCount == 8);

    r.close();
    jd->removeDiagnosticListener();
}

TEST(TestJavaDiagnostic, systemErr)
{
    std::auto_ptr<JavaDiagnostic> jd(JavaDiagnostic::createInstance());
    CHECK(jd.get());

    TestListener listener;
    jd->setDiagnosticListener(listener);

    Redirector r;
    TInt rc = r.connect();
    CHECK(rc == KErrNone);

    rc = r.systemErr(KText1);
    CHECK(rc == KErrNone);
    CHECK(listener == KText1);
    CHECK(listener.mSystemErrCount == 1);

    rc = r.systemErr(KText2);
    CHECK(rc == KErrNone);
    CHECK(listener == KText2);
    CHECK(listener.mSystemErrCount == 2);

    rc = r.systemErr(KText3);
    CHECK(rc == KErrNone);
    CHECK(listener == KText3);
    CHECK(listener.mSystemErrCount == 3);

    rc = r.systemErr(KText4);
    CHECK(rc == KErrNone);
    CHECK(listener == KText4);
    CHECK(listener.mSystemErrCount == 4);

    rc = r.systemErr(KText5);
    CHECK(rc == KErrNone);
    CHECK(listener == KText5);
    CHECK(listener.mSystemErrCount == 5);

    rc = r.systemErr(KText6);
    CHECK(rc == KErrNone);
    CHECK(listener == KText6);
    CHECK(listener.mSystemErrCount == 6);

    rc = r.systemErr(KText7);
    CHECK(rc == KErrNone);
    CHECK(listener == KText7);
    CHECK(listener.mSystemErrCount == 7);

    rc = r.systemErr(KText8);
    CHECK(rc == KErrNone);
    CHECK(listener == KText8);
    CHECK(listener.mSystemErrCount == 8);

    jd->removeDiagnosticListener();
}

TEST(TestJavaDiagnostic, log)
{
    std::auto_ptr<JavaDiagnostic> jd(JavaDiagnostic::createInstance());
    CHECK(jd.get());

    LoggingListener listener;
    jd->setDiagnosticListener(listener);

    TInt rc = Redirector::log(KText1);
    CHECK(rc == KErrNone);
    CHECK(listener == KText1);
    CHECK(listener.mLogCount == 1);

    rc = Redirector::log(KText2);
    CHECK(rc == KErrNone);
    CHECK(listener == KText2);
    CHECK(listener.mLogCount == 2);

    rc = Redirector::log(KText3);
    CHECK(rc == KErrNone);
    CHECK(listener == KText3);
    CHECK(listener.mLogCount == 3);

    rc = Redirector::log(KText4);
    CHECK(rc == KErrNone);
    CHECK(listener == KText4);
    CHECK(listener.mLogCount == 4);

    rc = Redirector::log(KText5);
    CHECK(rc == KErrNone);
    CHECK(listener == KText5);
    CHECK(listener.mLogCount == 5);

    rc = Redirector::log(KText6);
    CHECK(rc == KErrNone);
    CHECK(listener == KText6);
    CHECK(listener.mLogCount == 6);

    rc = Redirector::log(KText7);
    CHECK(rc == KErrNone);
    CHECK(listener == KText7);
    CHECK(listener.mLogCount == 7);

    rc = Redirector::log(KText8);
    CHECK(rc == KErrNone);
    CHECK(listener == KText8);
    CHECK(listener.mLogCount == 8);

    jd->removeDiagnosticListener();
}


TEST(TestJavaDiagnostic, loadTest)
{
    std::auto_ptr<JavaDiagnostic> jd(JavaDiagnostic::createInstance());
    CHECK(jd.get());

    TestListener listener;
    jd->setDiagnosticListener(listener);

    Redirector r;
    TInt rc = r.connect();
    CHECK(rc == KErrNone);

    for (int i = 0; i< 1000; i++)
    {
        rc = r.systemOut(KText1);
        CHECK(rc == KErrNone);
        rc = r.systemErr(KText1);
        CHECK(rc == KErrNone);
        rc = Redirector::log(KText1);
        CHECK(rc == KErrNone);
    }
    CHECK(listener.mSystemOutCount == 1000);
    CHECK(listener.mSystemErrCount == 1000);
    CHECK(listener.mLogCount == 1000);

    r.close();
    jd->removeDiagnosticListener();
}


TEST(TestJavaDiagnostic, logger)
{
    std::auto_ptr<JavaDiagnostic> jd(JavaDiagnostic::createInstance());
    CHECK(jd.get());

    LoggingListener listener;
    jd->setDiagnosticListener(listener);

    ELOG(EDebugApi, "error level text");
    CHECK(listener.mLogCount == 1);

    WLOG(EDebugApi, "warning level text");
    CHECK(listener.mLogCount == 2);

    LOG(EDebugApi, EInfo, "info level text");
    CHECK(listener.mLogCount == 2);

    jd->removeDiagnosticListener();
}

TEST(TestJavaDiagnostic, unusedMethods)
{
    std::auto_ptr<DiagnosticImplTester> d((DiagnosticImplTester*)JavaDiagnostic::createInstance());

    d->DoCancel();
    TInt rc = d->RunError(KErrGeneral);
    CHECK(rc == KErrNone);
}
