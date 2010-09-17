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
* Description: A CoreUi QT implementation.
*
*/

#include <string.h>
// #include <hbsplashscreen.h>

#include "logger.h"
#include "coreuiqtimpl.h"
#include "runtimeexception.h"
#include "javacommonutils.h"

using namespace java::ui;
using namespace java::util;

// ======== STATIC VARIABLES ========

/**
* Singleton
*/
static CoreUiQtImpl* sCoreUiQtImpl = 0;

CoreUiQtImpl* getCoreUi()
{
    if (sCoreUiQtImpl == 0)
    {
        sCoreUiQtImpl = new CoreUiQtImpl(); // codescanner::nonleavenew
    }
    return sCoreUiQtImpl;
}

#ifndef __SYMBIAN32__
extern_c
#endif // __SYMBIAN32__
CoreUi& getUiInstance()
{
    JELOG2(EJavaUI);
    return *getCoreUi();
}

#ifndef __SYMBIAN32__
extern_c
#endif // __SYMBIAN32__
void releaseUi()
{
    JELOG2(EJavaUI);
    ELOG(EJavaUI, "CoreUiQtImpl::releaseUi -->");
}

#ifdef __SYMBIAN32__
EXPORT_C FuncPtr findDllMethod(const char* funcName)
{
    JELOG2(EJavaUI);
    FuncPtr ptr = 0;
    if (strcmp(funcName, "getUiInstance") == 0)
    {
        ptr = (FuncPtr)getUiInstance;
    }
    else if (strcmp(funcName, "releaseUi") == 0)
    {
        ptr = (FuncPtr)releaseUi;
    }
    return ptr;
}
#endif // __SYMBIAN32__

CoreUiQtImpl::CoreUiQtImpl()
{
    JELOG2(EJavaUI);
}


CoreUiQtImpl::~CoreUiQtImpl()
{
    JELOG2(EJavaUI);
}


void CoreUiQtImpl::start(const java::util::Uid& /*midletUid*/,
                            CoreUiParams* /*uiParams*/)
{
    JELOG2(EJavaUI);
    ELOG(EJavaUI, "CoreUiQtImpl::start -->");

/*
    ELOG(EJavaUI, "HbSplashScreen::start() -->");
    QString uid = "10104305";
    __BREAKPOINT();
    HbSplashScreen::setAppId(uid);
    HbSplashScreen::start();
    User::After(5 * 1000 * 1000); // UID = 10104305
    ELOG(EJavaUI, "HbSplashScreen::start() <--");
*/
}
