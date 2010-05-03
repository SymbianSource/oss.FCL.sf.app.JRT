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
* Description: Converts Universal Emulator Interface (UEI) arguments to
*              JVM arguments
*
*/


#include <sstream>

#include "ueiargsparser.h"

#include "logger.h"

const wchar_t* const SUPPORTED_UEI_ARGS[] = {L"-Xverbose", L"-Xrunjdwp", 0};
const wchar_t* const SUPPORTED_JVM_ARGS[] = {L"-verbose", L"-Xrunjdwp", 0};
const int INDEX_VERBOSE = 0;
const int INDEX_RUNJDWP = 1;

const wchar_t* const ALL = L"all";
const wchar_t* const GC = L"gc";
const wchar_t* const CLASS = L"class";
const wchar_t* const JVM_SUPPORTED_VERBOSE_OPTIONS = L"gc,class,stack,sizes";

const wchar_t* const OPTION_SEPARATOR = L":";
const wchar_t* const SPACE = L" ";
const wchar_t* const COMMA = L",";

using namespace java::runtime;

std::wstring UeiArgsParser::convertUeiArgsToJvmArgs(const std::wstring& aUeiParameters)
{
    std::vector<std::wstring> args = getArgs(aUeiParameters);

    std::wostringstream jvmArgs;
    for (int i = 0; i < args.size(); ++i)
    {
        LOG2(EDebugApi, EInfo,"args(%S): %d", args.at(i).c_str(), i);
        if (isVerbose(args.at(i)))
        {
            jvmArgs << convertVerbose(args.at(i)) << SPACE;
        }
        else if (isRundjwp(args.at(i)))
        {
            jvmArgs << convertRundjwp(args.at(i)) << SPACE;
        }
    }

    std::wstring result = jvmArgs.str();
    trimTrailingSpaces(result, SPACE);

    LOG2(EDebugApi, EInfo,"convertUeiArgsToJvmArgs(%S): %S", aUeiParameters.c_str(), result.c_str());
    return result;
}

std::vector<std::wstring> UeiArgsParser::getArgs(const std::wstring& aUeiParameters)
{
    std::vector<std::wstring> result;
    std::wstringstream stream;
    stream << aUeiParameters; // codescanner::leave
    std::wstring arg;
    while (stream.good())
    {
        stream >> arg; // codescanner::leave
        if (arg.size() != 0)
        {
            result.push_back(arg);
        }
    }

    LOG2(EDebugApi, EInfo,"getArgs(%S): count=%d", aUeiParameters.c_str(), result.size());
    return result;
}

bool UeiArgsParser::isVerbose(const std::wstring& aUeiArg)
{
    std::wstring verbose(SUPPORTED_UEI_ARGS[INDEX_VERBOSE]);
    if (aUeiArg.compare(0, verbose.length(), verbose) == 0)
    {
        return true;
    }
    return false;
}

bool UeiArgsParser::isRundjwp(const std::wstring& aUeiArg)
{
    std::wstring rundjwp(SUPPORTED_UEI_ARGS[INDEX_RUNJDWP]);
    if (aUeiArg.compare(0, rundjwp.length(), rundjwp) == 0)
    {
        return true;
    }
    return false;
}

// hasOption() assumes that options are separated by ','
bool UeiArgsParser::hasOption(const std::wstring& aArg, const std::wstring& aOption)
{
    // option is in the middle: option,zzz
    size_t found = aArg.find(aOption + COMMA);
    if (found != std::wstring::npos)
    {
        return true;
    }

    // option is the last one: zzz,option
    found = aArg.find(aOption);
    if (found != std::wstring::npos)
    {
        // avoid false alarm: zzz,optionz
        int expectedSize = found + aOption.size();
        if (expectedSize == aArg.size())
        {
            return true;
        }
    }

    return false;
}

std::wstring UeiArgsParser::convertVerbose(const std::wstring& aVerbose)
{
    std::wostringstream arg;
    arg << SUPPORTED_JVM_ARGS[INDEX_VERBOSE];

    if (aVerbose.find(OPTION_SEPARATOR) != std::wstring::npos)
    {
        arg << OPTION_SEPARATOR;
        if (hasOption(aVerbose, ALL))
        {
            arg << JVM_SUPPORTED_VERBOSE_OPTIONS;
        }
        else
        {
            if (hasOption(aVerbose, GC))
            {
                arg << GC << COMMA;
            }
            if (hasOption(aVerbose, CLASS))
            {
                arg << CLASS << COMMA;
            }
        }
    }

    std::wstring result = arg.str();
    std::wostringstream whitespaces;
    whitespaces << OPTION_SEPARATOR << COMMA;
    trimTrailingSpaces(result, whitespaces.str());

    LOG2(EDebugApi, EInfo,"convertVerbose(%S): %S", aVerbose.c_str(), result.c_str());
    return result;
}

std::wstring UeiArgsParser::convertRundjwp(const std::wstring& aRunjdwp)
{
    return aRunjdwp;
}

std::wstring UeiArgsParser::trimTrailingSpaces(std::wstring& aString, const std::wstring& aWhitespaces)
{
    size_t trailingSpace = aString.find_last_not_of(aWhitespaces);
    if (trailingSpace != std::wstring::npos)
    {
        aString.erase(trailingSpace + 1);
    }
    return aString;
}

