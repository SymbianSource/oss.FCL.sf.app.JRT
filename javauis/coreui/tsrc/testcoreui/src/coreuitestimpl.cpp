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
* Description: A CoreUi TEST implementation.
*
*/

#include <string.h>

#include "logger.h"
#include "coreuitestimpl.h"
#include "runtimeexception.h"
#include "javacommonutils.h"

using namespace java::ui;
using namespace java::util;

// ======== STATIC VARIABLES ========

/**
* Singleton
*/
static CoreUiTestImpl* sCoreUiTestImpl = 0;

CoreUiTestImpl* getCoreUi()
{
    if (sCoreUiTestImpl == 0)
    {
        sCoreUiTestImpl = new CoreUiTestImpl(); // codescanner::nonleavenew
    }
    return sCoreUiTestImpl;
}

OS_EXPORT void CoreUiTestImpl::setTestHandler(CoreUiTestHandler* testHandler)
{
    getCoreUi()->mTestHandler = testHandler;
}

#ifndef __SYMBIAN32__
extern "C"
#endif // __SYMBIAN32__
void setTestHandler(CoreUiTestHandler* testHandler)
{
    CoreUiTestImpl::setTestHandler(testHandler);
}

#ifndef __SYMBIAN32__
extern "C"
#endif // __SYMBIAN32__
CoreUi& getUiInstance()
{
    JELOG2(EJavaUI);
    return *getCoreUi();
}

#ifndef __SYMBIAN32__
extern "C"
#endif // __SYMBIAN32__
void releaseUi()
{
    JELOG2(EJavaUI);
    if (getCoreUi()->mTestHandler)
    {
        getCoreUi()->mTestHandler->releaseCalled();
    }
    delete sCoreUiTestImpl;
    sCoreUiTestImpl = 0;
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
    else if (strcmp(funcName, "setTestHandler") == 0)
    {
        ptr = (FuncPtr)CoreUiTestImpl::setTestHandler;
    }
    return ptr;
}
#endif // __SYMBIAN32__

CoreUiTestImpl::CoreUiTestImpl() : mTestHandler(0)
{
    JELOG2(EJavaUI);
}


CoreUiTestImpl::~CoreUiTestImpl()
{
    JELOG2(EJavaUI);
}


void CoreUiTestImpl::start(const java::util::Uid& midletUid,
                           CoreUiParams* uiParams)
{
    JELOG2(EJavaUI);
    if (mTestHandler)
    {
        mTestHandler->startCalled(midletUid, uiParams);
    }
}
