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
* Description:  MidpRuntime
*
*/


#include "logger.h"
#include "javacommonutils.h"
#include "midpruntimearguments.h"

#include "midpruntime.h"

#include "javaprocessconstants.h"

namespace java
{
namespace captain
{

MidpRuntime::MidpRuntime(const Uid& aUID, CoreInterface* aCore)
        :RtcBasePlugin(aUID, aCore)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
}

MidpRuntime::~MidpRuntime()
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
}

cmdLine_t& MidpRuntime::generateCommandLine(const int& aLaunchType,
        cmdLine_t& params)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    params.push_back(java::runtime::JAVA_PROCESS);
    params.push_back(java::runtime::JAVA_MIDP_STARTER_DLL); //The name of the dll, that runs MIDlets

    if (aLaunchType == RTC_LAUNCH_TYPE_PREWARM_C)
    {
        params.push_back(java::runtime::PREWARM_ARGUMENT);
    }
    else
    {
        params.push_back(java::runtime::APP_UID_ARGUMENT);
        char* appUID = java::util::JavaCommonUtils::wstringToUtf8(mUID.toString());
        params.push_back(appUID);
        delete [] appUID;

        if (aLaunchType == RTC_LAUNCH_TYPE_DEBUG_C ||
                !mRuntimeArguments.empty())
        {
            params.push_back(java::runtime::DEBUG_ARGUMENT);
        }

        if (!mApplicationArguments.empty())
        {
            params.push_back(java::runtime::EXTRA_ARGUMENTS);
        }

        switch (aLaunchType)
        {
        case RTC_LAUNCH_TYPE_PUSH_C:
            params.push_back(java::runtime::PUSH_ARGUMENT);
            break;

        case RTC_LAUNCH_TYPE_AUTO_INVOCATION_C:
            params.push_back(java::runtime::AUTO_INVOCATION_ARGUMENT);
            break;

        case RTC_LAUNCH_TYPE_BACKGROUND_C:
            params.push_back(java::runtime::BG_START_ARGUMENT);
            break;

            // fall through
        case RTC_LAUNCH_TYPE_NORMAL_C:
        case RTC_LAUNCH_TYPE_DEBUG_C:
        default:
            break;
        }
    }

    return params;
}

} // namespace captain
} // namespace java

