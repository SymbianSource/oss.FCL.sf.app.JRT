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
#include <string>
#include <list>
#include <algorithm>
#include <memory>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>


#include "TestHarness.h"
#include "logger.h"
#include "javacommonutils.h"
#include "exceptionbase.h"
#include "javasymbianoslayer.h"
#include "javadebugapi.h"

const char INSTALL_DIR[] = "c:\\java\\debugapitest\\";
const char JAD_EXTENSION[] = ".jad";
const char JAR_EXTENSION[] = ".jar";

const char* UEI_PARAMS = "-Xverbose:all,allocation,gc,gcverbose,class,classverbose,verifier,stackmaps,"
                         "bytecodes,frames,stackchunks,exceptions,events,threading,monitors,networking"
                         "-Xdebug -Xrunjdwp:server=y,address=localhost:8000";

void logStrings(std::string aArg)
{
    LOG1(EDebugApi, EInfo, " '%s'", aArg.c_str());
}

std::list<std::string> getFileList(const std::string& aPath, const std::string& aExtension)
{

    std::list<std::string> files;

    DIR *pDIR = opendir(aPath.c_str());
    if (pDIR != NULL)
    {
        struct dirent* pDirEnt = readdir(pDIR);
        while (pDirEnt != NULL)
        {
            //check if file extension matches
            if (strcmp((pDirEnt->d_name + strlen(pDirEnt->d_name) - aExtension.length()), aExtension.c_str()) == 0)
            {
                std::string file;
                file = aPath + pDirEnt->d_name;
                files.push_back(file);
            }
            pDirEnt = readdir(pDIR);
        }
        closedir(pDIR);
    }

    return files;
}



TEST_GROUP(TestJavaDebugApi)
{
    TEST_SETUP()
    {
    }

    TEST_TEARDOWN()
    {
    }
};


/**
 * Tests install, start, stop and uninstall for JADs
 */
TEST(TestJavaDebugApi, runAllJads)
{
    std::list<std::string> files;
    files = getFileList(INSTALL_DIR, JAD_EXTENSION);
    LOG(EDebugApi, EInfo, "JAD files");
    std::for_each(files.begin(), files.end(), logStrings);

    CHECK(files.size() != 0); // fail if no files found

    for (std::list<std::string>::const_iterator iter = files.begin(); iter != files.end(); ++iter)
    {
        // install app
        std::auto_ptr<HBufC> filename(stringToDes((*iter).c_str()));
        TUid suiteUid;
        RArray<TUid> applicationUids;
        int rc = java::debug::installApp(*filename, suiteUid, applicationUids);
        CHECK(rc == (int)ETrue);

        // start and stop each app
        for (int i = 0; i < applicationUids.Count(); i++)
        {
            std::auto_ptr<HBufC> ueiParams(stringToDes(UEI_PARAMS));
            TProcessId pid;
            rc = java::debug::startApp(applicationUids[i], *ueiParams, pid);
            CHECK(rc == (int)ETrue);
            RProcess process;
            rc = process.Open(pid);
            CHECK(rc == KErrNone);
            TRequestStatus status;
            process.Logon(status);
            // wait a while so that runtime has got ueiParams
            usleep(300000); //0.3s
            rc = java::debug::stopApp(applicationUids[i]);
            CHECK(rc == (int)ETrue);
            User::WaitForRequest(status);
            process.Close();
        }

        // uninstall app
        rc = java::debug::uninstallApp(suiteUid);
        CHECK(rc == (int)ETrue);

        applicationUids.Reset();
    }
}

/**
 * Tests install, start, stop and uninstall for JARs
 */
TEST(TestJavaDebugApi, runAllJars)
{
    EXPECT_N_LEAKS(1);
    std::list<std::string> files;
    files = getFileList(INSTALL_DIR, JAR_EXTENSION);
    LOG(EDebugApi, EInfo, "JAR files");
    std::for_each(files.begin(), files.end(), logStrings);

    CHECK(files.size() != 0); // fail if no files found

    for (std::list<std::string>::const_iterator iter = files.begin(); iter != files.end(); ++iter)
    {
        // install app
        std::auto_ptr<HBufC> filename(stringToDes((*iter).c_str()));
        TUid suiteUid;
        RArray<TUid> applicationUids;
        int rc = java::debug::installApp(*filename, suiteUid, applicationUids);
        CHECK(rc == (int)ETrue);

        // start and stop each app
        for (int i = 0; i < applicationUids.Count(); i++)
        {
            std::auto_ptr<HBufC> ueiParams(stringToDes(UEI_PARAMS));
            TProcessId pid;
            rc = java::debug::startApp(applicationUids[i], *ueiParams, pid);
            CHECK(rc == (int)ETrue);
            RProcess process;
            rc = process.Open(pid);
            CHECK(rc == KErrNone);
            TRequestStatus status;
            process.Logon(status);
            // wait a while so that runtime has got ueiParams
            usleep(300000); //0.3s
            rc = java::debug::stopApp(applicationUids[i]);
            CHECK(rc == (int)ETrue);
            User::WaitForRequest(status);
            process.Close();
        }

        // uninstall app
        rc = java::debug::uninstallApp(suiteUid);
        CHECK(rc == (int)ETrue);

        applicationUids.Reset();
    }
}

TEST(TestJavaDebugApi, installError)
{
    std::string file = "thisfiledoesnotexists.jar";
    std::auto_ptr<HBufC> filename(stringToDes(file.c_str()));
    TUid suiteUid;
    RArray<TUid> applicationUids;
    int rc = java::debug::installApp(*filename, suiteUid, applicationUids);
    CHECK(rc == (int)EFalse);
}

TEST(TestJavaDebugApi, uninstallError)
{
    EXPECT_N_LEAKS(1);
    TUid suiteUid = TUid::Null();
    int rc = java::debug::uninstallApp(suiteUid);
    CHECK(rc == (int)EFalse);
}

TEST(TestJavaDebugApi, startError)
{
    TUid appUid = TUid::Null();
    std::auto_ptr<HBufC> ueiParams(stringToDes(UEI_PARAMS));
    TProcessId pid = 0;
    int rc = java::debug::startApp(appUid, *ueiParams, pid);
    CHECK(rc == (int)EFalse);
}

TEST(TestJavaDebugApi, stopError)
{
    EXPECT_N_LEAKS(1);
    TUid appUid = TUid::Null();
    int rc = java::debug::stopApp(appUid);
    CHECK(rc == (int)ETrue);
}
