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
* Description: A support class implementation for the eSWT.
*
*/


#include <swtserver.h>
#include "coreuiavkoneswtsupportimpl.h"
#include "logger.h"
#include "javacommonutils.h"
#include "runtimeexception.h"

using namespace java::ui;
using namespace java::util;

CoreUiAvkonEswtSupportImpl::CoreUiAvkonEswtSupportImpl() : mDisplay(0)
{
    JELOG2(EJavaUI);
}

CoreUiAvkonEswtSupportImpl::~CoreUiAvkonEswtSupportImpl()
{
    JELOG2(EJavaUI);
}

void CoreUiAvkonEswtSupportImpl::uiThreadInit()
{
    JELOG2(EJavaUI);
    int error = 0;
#ifdef _DEBUG
    error = mThread.Open(RThread().Id());
    ASSERT(!error);
#endif
    TRAP(error, mServer = CSwtServer::NewL(this));
    if (error)
    {
        std::string errorStr("Server creation failed in eSWT: ");
        errorStr.append(JavaCommonUtils::intToString(error));
        throw java::runtime::RuntimeException(errorStr, __FILE__, __FUNCTION__, __LINE__);
    }
}

// This is called aftr CCoeEnv::ExecuteD() has returned
void CoreUiAvkonEswtSupportImpl::dispose()
{
    JELOG2(EJavaUI);
    mDisplay = 0;
    delete mServer;
    mServer = 0;
#ifdef _DEBUG
    mThread.Close();
#endif
}

#ifdef _DEBUG
RThread& CoreUiAvkonEswtSupportImpl::thread()
{
    JELOG2(EJavaUI);
    return mThread;
}
#endif

CSwtDisplay* CoreUiAvkonEswtSupportImpl::display() const
{
    JELOG2(EJavaUI);
    return mDisplay;
}

void CoreUiAvkonEswtSupportImpl::setDisplay(CSwtDisplay* display)
{
    JELOG2(EJavaUI);
    mDisplay = display;
}

RServer2 CoreUiAvkonEswtSupportImpl::getServerHandle() const
{
    JELOG2(EJavaUI);
    return mServer->Server();
}
