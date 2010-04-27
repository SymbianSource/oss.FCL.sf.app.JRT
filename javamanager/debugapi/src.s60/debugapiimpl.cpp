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
* Description: DebugApiImpl provides implementation for Degug API methods
*
*/


#include "debugapiimpl.h"

#include "applauncher.h"
#include "appinstaller.h"
#include "appremover.h"

#include "logger.h"
#include "javauid.h"
#include "javaprocessconstants.h"
#include "javasymbianoslayer.h"

using namespace java::debug;
using namespace java::util;


DebugApiImpl::DebugApiImpl()
{
}

DebugApiImpl::~DebugApiImpl()
{
}

int DebugApiImpl::installApp(const std::wstring& aFilename,
                             Uid& aSuiteUid,
                             std::list<Uid>& aAppUids)
{
    JELOG2(EDebugApi);

    AppInstaller installer(aFilename);
    int rc = installer.install();

    if (!rc)
    {
        aSuiteUid = installer.getSuiteUid();
        aAppUids = installer.getAppUids();
    }

    return rc;
}

int DebugApiImpl::uninstallApp(const Uid& aSuiteUid)
{
    JELOG2(EDebugApi);

    AppRemover installer(aSuiteUid);
    int rc = installer.uninstall();

    return rc;
}

int DebugApiImpl::startApp(const Uid& aAppUid, const std::wstring& aUeiParameters, pid_t& aPid)
{
    JELOG2(EDebugApi);

    AppLauncher launcher(aAppUid);
    int rc = launcher.startApp(aUeiParameters);
    aPid = launcher.getPid();
    return rc;
}

int DebugApiImpl::stopApp(const Uid& aAppUid)
{
    JELOG2(EDebugApi);
    AppLauncher launcher(aAppUid);
    return launcher.stopApp();
}

