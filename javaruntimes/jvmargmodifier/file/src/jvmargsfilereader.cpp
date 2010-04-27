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
* Description:  A default empty implementation for JvmArgs modifier.
*
*/

#include <fstream>

#include "logger.h"
#include "javacommonutils.h"
#include "exceptionbase.h"
#include "driveutilities.h"

#include "jvmargsfilereader.h"

namespace java // codescanner::namespace
{
namespace runtime // codescanner::namespace
{

#ifdef __SYMBIAN32__
const wchar_t JVM_ARGS_DIR[] = L"java\\";
#else
const wchar_t JVM_ARGS_DIR[] = L"java/";
#endif

const wchar_t* const IDENTIFIERS[] = {L"Midp", L"Installer", L"TCK_runner", 0};
const wchar_t* const JVM_ARGS_FILENAMES[] = {L"midpargs.txt", L"installerargs.txt", L"tckrunnerargs.txt", 0};
const wchar_t* const APP_ARGS_FILENAMES[] = {L"midpappargs.txt", L"installerappargs.txt", L"tckrunnerappargs.txt", 0};


JvmArgsFileReader::JvmArgsFileReader(const std::wstring& aIdentifier)
        : mIdentifier(aIdentifier)
{
}

JvmArgsFileReader::~JvmArgsFileReader()
{
}

const std::list<std::wstring>& JvmArgsFileReader::getJvmArguments()
{
    std::wstring filename;
    for (int i = 0; IDENTIFIERS[i] != 0; ++i) // codescanner::accessArrayElementWithoutCheck2
    {
        if (mIdentifier.compare(IDENTIFIERS[i]) == 0) // codescanner::accessArrayElementWithoutCheck2
        {
            filename = JVM_ARGS_FILENAMES[i];
            break;
        }
    }

    if (!filename.empty())
    {
        readArgs(filename , mJvmArgs);
    }
    else
    {
        WLOG2(EJavaRuntime, "%s: unknown identifier: %S", __PRETTY_FUNCTION__, mIdentifier.c_str());
    }
    return mJvmArgs;
}

const std::list<std::wstring>& JvmArgsFileReader::getApplicationArguments()
{
    std::wstring filename;
    for (int i = 0; IDENTIFIERS[i] != 0; ++i) // codescanner::accessArrayElementWithoutCheck2
    {
        if (mIdentifier.compare(IDENTIFIERS[i]) == 0) // codescanner::accessArrayElementWithoutCheck2
        {
            filename = APP_ARGS_FILENAMES[i];
            break;
        }
    }

    if (!filename.empty())
    {
        readArgs(filename , mApplicationArgs);
    }
    else
    {
        WLOG2(EJavaRuntime, "%s: unknown identifier: %S", __PRETTY_FUNCTION__, mIdentifier.c_str());
    }
    return mApplicationArgs;
}

void JvmArgsFileReader::readArgs(const std::wstring& aFilename, std::list<std::wstring>& aArgs)
{
    java::fileutils::driveInfos drives;
    java::fileutils::DriveUtilities::getAccesibleDrives(drives);

    for (java::fileutils::driveInfos::const_iterator iter = drives.begin(); iter != drives.end(); iter++)
    {
        std::string path = makeFullFilename((*iter).iRootPath.c_str(), JVM_ARGS_DIR, aFilename);

        std::ifstream file;
        file.open(path.c_str());
        if (file.good())
        {
            LOG1(EJavaRuntime, EInfo, "Using JVM args from %s", path.c_str());
            std::string arg;
            while (file >> arg)
            {
                try
                {
                    std::wstring str = java::util::JavaCommonUtils::utf8ToWstring(arg.c_str());
                    aArgs.push_back(str);
                    LOG1(EJavaRuntime, EInfo, " '%S'", str.c_str());
                }
                catch (java::util::ExceptionBase& e)
                {
                    ELOG2(EJavaRuntime, "%s: utf8ToWstring failed: %s", __PRETTY_FUNCTION__, e.toString().c_str());
                }
            }
            file.close();
            break;
        }
        file.close();
    }
}

std::string JvmArgsFileReader::makeFullFilename(const std::wstring& aDrive, const std::wstring& aDir, const std::wstring& aFilename)
{
    std::wstring path;
    path.append(aDrive).append(aDir).append(aFilename);

    std::string fullFilename;
    try
    {
        char* utf8 = java::util::JavaCommonUtils::wstringToUtf8(path);
        fullFilename.assign(utf8);
        delete[] utf8;
    }
    catch (java::util::ExceptionBase& e)
    {
        ELOG2(EJavaRuntime, "%s: wstringToUtf8 failed: %s", __PRETTY_FUNCTION__, e.toString().c_str());
    }

    LOG1(EJavaRuntime, EInfoHeavyLoad, "makeFullFilename returns %s", fullFilename.c_str());
    return fullFilename;
}



} // end namespace runtime
} // end namespace java

