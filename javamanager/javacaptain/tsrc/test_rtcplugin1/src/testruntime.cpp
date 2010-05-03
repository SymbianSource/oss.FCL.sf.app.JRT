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
* Description:  TestRuntime
*
*/


#include "logger.h"
#include "javacommonutils.h"

#include "testruntime.h"

#include "javaprocessconstants.h"

namespace java
{
namespace captain
{

TestRuntime::TestRuntime(const Uid& aUID, CoreInterface* aCore)
        :RtcBasePlugin(aUID, aCore)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
}

TestRuntime::~TestRuntime()
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
}

const char* const APP_UID_ARGUMENT          = "-appid";
const char* const PUSH_ARGUMENT             = "-push";
const char* const PREWARM_ARGUMENT          = "-prewarm";
const char* const AUTO_INVOCATION_ARGUMENT  = "-autoinvocation";
const char* const DEBUG_ARGUMENT            = "-debug";
const char* const BG_START_ARGUMENT         = "-background";

// Currently only platform/runtime specific part
cmdLine_t& TestRuntime::generateCommandLine(const int& aLaunchType,
        cmdLine_t& params)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    params.push_back("javatestruntime");
    params.push_back(java::runtime::JAVA_MIDP_STARTER_DLL); //The name of the dll, that runs MIDlets
    params.push_back(APP_UID_ARGUMENT);
    char* appUID = java::util::JavaCommonUtils::wstringToUtf8(mUID.toString());
    params.push_back(appUID);
    delete [] appUID;

    switch (aLaunchType)
    {
    case RTC_LAUNCH_TYPE_PUSH_C:
        params.push_back(PUSH_ARGUMENT);
        break;

    case RTC_LAUNCH_TYPE_AUTO_INVOCATION_C:
        params.push_back(AUTO_INVOCATION_ARGUMENT);
        break;

    case RTC_LAUNCH_TYPE_BACKGROUND_C:
        params.push_back(BG_START_ARGUMENT);
        break;

    case RTC_LAUNCH_TYPE_DEBUG_C:
        LOG(EJavaCaptain, EInfo, "launching in debug mode");
        params.push_back(DEBUG_ARGUMENT);
        break;

    case RTC_LAUNCH_TYPE_NORMAL_C: // fall through
    default:
        break;

    }

    return params;
}

} // namespace captain
} // namespace java

