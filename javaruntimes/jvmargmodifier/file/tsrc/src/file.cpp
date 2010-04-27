/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Tests for reading JVM and app args from file
*
*/

#include <fstream>
#include <string>
#include <list>
#include <sstream>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>

#include "TestHarness.h"
#include "javacommonutils.h"
#include "exceptionbase.h"
#include "driveutilities.h"

#include "jvmargsmodifier.h"

const wchar_t* IDENTIFIERS[] = {L"Midp", L"Installer", L"TCK_runner", 0};
#ifdef __SYMBIAN32__
const char* JVM_ARGS_FILENAMES[] = {"java\\midpargs.txt", "java\\installerargs.txt", "java\\tckrunnerargs.txt", 0};
const char* APP_ARGS_FILENAMES[] = {"java\\midpappargs.txt", "java\\installerappargs.txt", "java\\tckrunnerappargs.txt", 0};
#else
const char* JVM_ARGS_FILENAMES[] = {"java/midpargs.txt", "java/installerargs.txt", "java/tckrunnerargs.txt", 0};
const char* APP_ARGS_FILENAMES[] = {"java/midpappargs.txt", "java/installerappargs.txt", "java/tckrunnerappargs.txt", 0};
#endif
const char* ARG_DIR = "java";

const char* VM_ARGS =
    "-jcl:cldc11:nokiaextcldc" "\n"\
    "-Xrunjdwp:server=y,address=localhost:8000" "\n"\
    "-Xmos192K" "\n"\
    "-Xmns64K" "\n"\
    "-Xits8k" "\n"\
    "-Xiss1k" "\n"\
    "-Xmco16k" "\n"\
    "-Xmr1k" "\n"\
    "-Xgc:finInc=32,compactMaxRelocationEntries=4096" "\n"\
    "-Xmaxf0.3" "\n"\
    "-Xminf0.1" "\n"\
    "-Xmox16M" "\n"\
    "-Xmx16M" "\n"\
    "-Dmicroedition.connection.pkgs=com.symbian.midp.io.protocol" "\n"\
    "-Dnokia.vm.portclass=com.nokia.mj.impl.vmport.J9VmPortImplCldc" "\n"\
    "-io:localhost:4444";

const char* APP_ARGS = VM_ARGS;

bool initArgsFile(const std::string& aDrive, const std::string& aFilename, const std::string& aValue)
{
    std::string filename = aDrive + aFilename;
    std::fstream argsFile;
    argsFile.open(filename.c_str(), std::fstream::out | std::fstream::trunc);
    bool openOk = argsFile.good();
    argsFile << aValue;
    argsFile.close();
    return openOk;
}

void removeFile(const std::string& aDrive, const std::string& aFilename)
{
    std::string filename = aDrive + aFilename;
    remove(filename.c_str());
}

std::list<std::wstring> argsToList(const std::string& aArgs)
{
    std::stringstream ss;
    ss << aArgs;
    std::string arg;

    std::list<std::wstring> result;
    while (ss >> arg)
    {
        try
        {
            std::wstring vmarg = java::util::JavaCommonUtils::utf8ToWstring(arg.c_str());
            result.push_back(vmarg);
        }
        catch (java::util::ExceptionBase& e) {}
    }
    return result;
}

std::list<std::string> getDrives()
{
    std::list<std::string> result;

    java::fileutils::driveInfos drives;
    java::fileutils::DriveUtilities::getAccesibleDrives(drives);

    for (java::fileutils::driveInfos::const_iterator iter = drives.begin(); iter != drives.end(); iter++)
    {
        try
        {
            std::string drive = java::util::JavaCommonUtils::wstringToUtf8((*iter).iRootPath);
            std::string dir = drive + ARG_DIR;
            mkdir(dir.c_str(), S_IRWXU);
            if (initArgsFile(drive, JVM_ARGS_FILENAMES[0], ""))
            {
                result.push_back(drive);
            }
            removeFile(drive, APP_ARGS_FILENAMES[0]);
        }
        catch (java::util::ExceptionBase& e) {}
    }
    return result;
}

TEST_GROUP(File)
{
    std::string defaultDrive;
    std::list<std::string> roots;
    TEST_SETUP()
    {
        roots = getDrives();
        defaultDrive = roots.front();
        for (std::list<std::string>::const_iterator iter = roots.begin(); iter != roots.end(); ++iter)
        {
            std::string drive = (*iter);
            for (int i = 0; IDENTIFIERS[i] != 0; ++i)
            {
                removeFile(drive, JVM_ARGS_FILENAMES[i]);
                removeFile(drive, APP_ARGS_FILENAMES[i]);
            }
        }
    }

    TEST_TEARDOWN()
    {
        for (std::list<std::string>::const_iterator iter = roots.begin(); iter != roots.end(); ++iter)
        {
            std::string drive = (*iter);
            for (int i = 0; IDENTIFIERS[i] != 0; ++i)
            {
                removeFile(drive, JVM_ARGS_FILENAMES[i]);
                removeFile(drive, APP_ARGS_FILENAMES[i]);
            }
        }
    }
};


/**
 * Test reading of argument files for different identifiers
 * 1. Read JVM and App args from file
 */
TEST(File, ReadArgsFile)
{
#ifdef __SYMBIAN32__
    EXPECT_N_LEAKS(586);
#else
    EXPECT_N_LEAKS(3);
#endif
    // 1. Read JVM and App args from file
    for (std::list<std::string>::const_iterator iter = roots.begin(); iter != roots.end(); ++iter)
    {
        std::string drive = (*iter);
        for (int i = 0; IDENTIFIERS[i] != 0; ++i)
        {
            initArgsFile(drive, JVM_ARGS_FILENAMES[i], VM_ARGS);
            initArgsFile(drive, APP_ARGS_FILENAMES[i], APP_ARGS);

            std::wstring identifier = IDENTIFIERS[i];
            std::list<std::wstring> jvmArgs;
            std::list<std::wstring> applicationAndArgs;

            java::runtime::modifyJvmArguments(identifier, jvmArgs, applicationAndArgs);

            // check VM args
            std::list<std::wstring> expectedJvmResult = argsToList(VM_ARGS);
            CHECK(expectedJvmResult.size() == jvmArgs.size());
            CHECK(std::equal(expectedJvmResult.begin(), expectedJvmResult.end(), jvmArgs.begin()) == true);

            // check app args
            std::list<std::wstring> expectedAppResult = argsToList(APP_ARGS);
            CHECK(expectedAppResult.size() == applicationAndArgs.size());
            CHECK(std::equal(expectedAppResult.begin(), expectedAppResult.end(), applicationAndArgs.begin()) == true);

            removeFile(drive, JVM_ARGS_FILENAMES[i]);
            removeFile(drive, APP_ARGS_FILENAMES[i]);
        }
    }
}

/**
 * Check that arguments are not modified if arguments file does not exist
 * 1. Arguments file does not exist
 * 2. Arguments file is empty
 */
TEST(File, NoArgsFile)
{
#ifdef __SYMBIAN32__
    EXPECT_N_LEAKS(10);
#else
    EXPECT_N_LEAKS(3);
#endif
    // 1. Arguments file does not exist
    for (int i = 0; IDENTIFIERS[i] != 0; ++i)
    {
        std::wstring identifier = IDENTIFIERS[i];
        std::list<std::wstring> jvmArgs = argsToList(VM_ARGS);
        std::list<std::wstring> applicationAndArgs = argsToList(APP_ARGS);

        java::runtime::modifyJvmArguments(identifier, jvmArgs, applicationAndArgs);

        // check VM args
        std::list<std::wstring> expectedJvmResult = argsToList(VM_ARGS);
        CHECK(expectedJvmResult.size() == jvmArgs.size());
        CHECK(std::equal(expectedJvmResult.begin(), expectedJvmResult.end(), jvmArgs.begin()) == true);

        // check app args
        std::list<std::wstring> expectedAppResult = argsToList(APP_ARGS);
        CHECK(expectedAppResult.size() == applicationAndArgs.size());
        CHECK(std::equal(expectedAppResult.begin(), expectedAppResult.end(), applicationAndArgs.begin()) == true);
    }

    // 2. Arguments file is empty
    for (int i = 0; IDENTIFIERS[i] != 0; ++i)
    {
        initArgsFile(defaultDrive, JVM_ARGS_FILENAMES[i], "");
        initArgsFile(defaultDrive, APP_ARGS_FILENAMES[i], "");

        std::wstring identifier = IDENTIFIERS[i];
        std::list<std::wstring> jvmArgs = argsToList(VM_ARGS);
        std::list<std::wstring> applicationAndArgs = argsToList(APP_ARGS);

        java::runtime::modifyJvmArguments(identifier, jvmArgs, applicationAndArgs);

        // check VM args
        std::list<std::wstring> expectedJvmResult = argsToList(VM_ARGS);
        CHECK(expectedJvmResult.size() == jvmArgs.size());
        CHECK(std::equal(expectedJvmResult.begin(), expectedJvmResult.end(), jvmArgs.begin()) == true);

        // check app args
        std::list<std::wstring> expectedAppResult = argsToList(APP_ARGS);
        CHECK(expectedAppResult.size() == applicationAndArgs.size());
        CHECK(std::equal(expectedAppResult.begin(), expectedAppResult.end(), applicationAndArgs.begin()) == true);

        removeFile(defaultDrive, JVM_ARGS_FILENAMES[i]);
        removeFile(defaultDrive, APP_ARGS_FILENAMES[i]);
    }
}


/**
 * Check that arguments are not modified if unknow identifier is used
 * 1. Original arguments are empty
 * 2. Original arguments are not modified
 */
TEST(File, UnknowIdentifier)
{
#ifdef __SYMBIAN32__
    EXPECT_N_LEAKS(14);
#else
    EXPECT_N_LEAKS(3);
#endif
    initArgsFile(defaultDrive, JVM_ARGS_FILENAMES[0], VM_ARGS);
    initArgsFile(defaultDrive, APP_ARGS_FILENAMES[0], VM_ARGS);

    //1. Original arguments are empty
    std::wstring identifier = L"dummy";
    std::list<std::wstring> jvmArgs;
    std::list<std::wstring> applicationAndArgs;

    java::runtime::modifyJvmArguments(identifier, jvmArgs, applicationAndArgs);
    CHECK(jvmArgs.size() == 0);
    CHECK(applicationAndArgs.size() == 0);

    //2. Original arguments are not modified
    jvmArgs = argsToList(VM_ARGS);
    applicationAndArgs = argsToList(APP_ARGS);
    java::runtime::modifyJvmArguments(identifier, jvmArgs, applicationAndArgs);

    std::list<std::wstring> expectedJvmResult = argsToList(VM_ARGS);
    CHECK(expectedJvmResult.size() == jvmArgs.size());
    CHECK(std::equal(expectedJvmResult.begin(), expectedJvmResult.end(), jvmArgs.begin()) == true);

    std::list<std::wstring> expectedAppResult = argsToList(APP_ARGS);
    CHECK(expectedAppResult.size() == applicationAndArgs.size());
    CHECK(std::equal(expectedAppResult.begin(), expectedAppResult.end(), applicationAndArgs.begin()) == true);

    removeFile(defaultDrive, JVM_ARGS_FILENAMES[0]);
    removeFile(defaultDrive, APP_ARGS_FILENAMES[0]);
}


/**
 * Test adding of arguments
 * 1. New arguments are appended to the args list
 */
TEST(File, ArgsAdded)
{
#ifdef __SYMBIAN32__
    EXPECT_N_LEAKS(640);
#else
    EXPECT_N_LEAKS(3);
#endif
    const char* ORIG_VM_ARGS = "-dummy1=100 -dummy2=200 -dummy3=300";
    const char* ORIG_APP_ARGS = ORIG_VM_ARGS;

    for (std::list<std::string>::const_iterator iter = roots.begin(); iter != roots.end(); ++iter)
    {
        std::string drive = (*iter);
        // 1. New arguments are appended to the args list
        for (int i = 0; IDENTIFIERS[i] != 0; ++i)
        {
            initArgsFile(drive, JVM_ARGS_FILENAMES[i], VM_ARGS);
            initArgsFile(drive, APP_ARGS_FILENAMES[i], APP_ARGS);

            std::wstring identifier = IDENTIFIERS[i];
            std::list<std::wstring> jvmArgs = argsToList(ORIG_VM_ARGS);
            std::list<std::wstring> applicationAndArgs = argsToList(ORIG_APP_ARGS);

            java::runtime::modifyJvmArguments(identifier, jvmArgs, applicationAndArgs);

            // check VM args
            std::list<std::wstring> expectedJvmResult = argsToList(ORIG_VM_ARGS);
            std::list<std::wstring> temp = argsToList(VM_ARGS);

            std::copy(temp.begin(),temp.end(), std::back_inserter(expectedJvmResult));
            CHECK(expectedJvmResult.size() == jvmArgs.size());
            CHECK(std::equal(expectedJvmResult.begin(), expectedJvmResult.end(), jvmArgs.begin()) == true);

            // check app args
            std::list<std::wstring> expectedAppResult = argsToList(ORIG_APP_ARGS);
            std::list<std::wstring> temp2 = argsToList(APP_ARGS);

            std::copy(temp2.begin(),temp2.end(), std::back_inserter(expectedAppResult));
            CHECK(expectedAppResult.size() == applicationAndArgs.size());
            CHECK(std::equal(expectedAppResult.begin(), expectedAppResult.end(), applicationAndArgs.begin()) == true);

            removeFile(drive, JVM_ARGS_FILENAMES[i]);
            removeFile(drive, APP_ARGS_FILENAMES[i]);
        }
    }
}

/**
 * Test modifying of arguments
 */
TEST(File, ArgsModified)
{
#ifdef __SYMBIAN32__
    EXPECT_N_LEAKS(10);
#else
    EXPECT_N_LEAKS(3);
#endif
    // for now changed values are just appended so no difference to add args test case
}

/**
 * Test adding and modifying of arguments
 */
TEST(File, ArgsAddedAndModified)
{
#ifdef __SYMBIAN32__
    EXPECT_N_LEAKS(10);
#else
    EXPECT_N_LEAKS(3);
#endif
    // for now new/changed values are just appended so no difference to add args test case
}
