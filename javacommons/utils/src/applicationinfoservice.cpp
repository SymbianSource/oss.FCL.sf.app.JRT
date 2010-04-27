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
* Description:  ApplicationInfoService provide a storage place for singleton
*                instance of ApplicationInfo object.
*
*/


#include "logger.h"
#include "monitor.h"
#include "javaoslayer.h"
#include "applicationinfo.h"
#include "applicationinfosetter.h"
#include "exceptionbase.h"


// ======== STATIC VARIABLES ========

/**
* Singleton
*/

//OS_NONSHARABLE_CLASS(AppInfoGlobals)
class AppInfoGlobals
{
public:
    AppInfoGlobals() : mAppInfo(0)
    {
    }

public:
    const java::runtime::ApplicationInfo* mAppInfo;
};


#if defined(__WINSCW__)

#include <pls.h>
AppInfoGlobals* getAppInfoGlobals()
{
    // Access the PLS of this process
    AppInfoGlobals* globals  =
        Pls<AppInfoGlobals>(TUid::Uid(0x200211DE));
    return globals;
}

#else

static AppInfoGlobals* sGlobals = 0;

AppInfoGlobals* getAppInfoGlobals()
{
    if (sGlobals == 0)
    {
        sGlobals = new AppInfoGlobals();
    }
    return sGlobals;
}
#endif

OS_EXPORT java::runtime::ApplicationInfo::ApplicationInfo()
{
}

OS_EXPORT java::runtime::ApplicationInfo::~ApplicationInfo()
{
}

OS_EXPORT void java::util::setApplicationInfoProvider(const java::runtime::ApplicationInfo& appInfo)
{
    AppInfoGlobals* globals = getAppInfoGlobals();
    if (globals->mAppInfo != 0)
    {
        delete globals->mAppInfo;
    }
    globals->mAppInfo = &appInfo;
}

OS_EXPORT const java::runtime::ApplicationInfo& java::runtime::ApplicationInfo::getInstance()
{
    AppInfoGlobals* globals = getAppInfoGlobals();
    if (globals->mAppInfo == 0)
    {
        {
            std::string errorStr = "ApplicationInfo service provider not available";
            throw java::util::ExceptionBase(errorStr, __FILE__, __FUNCTION__, __LINE__);
        }
    }
    return *globals->mAppInfo;
}
