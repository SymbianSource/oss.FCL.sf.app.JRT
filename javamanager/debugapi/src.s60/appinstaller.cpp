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
* Description: AppInstaller takes care of installing Java application
*
*/

#include "appinstaller.h"

#include "logger.h"
#include "commsmessage.h"

using namespace java::debug;
using namespace java::comms;

AppInstaller::AppInstaller(const std::wstring& aFilename) : mFilename(aFilename),
        mInstallDone(false), mInstallResult(-1)
{
}

AppInstaller::~AppInstaller()
{
}

int AppInstaller::install()
{
    JELOG2(EDebugApi);

    runJavaInstaller();
    LOG3(EDebugApi, EInfo, "AppInstaller::installApp(): file=%S, result=%d, suiteUid=%S",
         mFilename.c_str(), mInstallResult, mSuiteUid.toString().c_str());
    return mInstallResult;
}

Uid AppInstaller::getSuiteUid() const
{
    return mSuiteUid;
}

std::list<Uid> AppInstaller::getAppUids() const
{
    return mAppUids;
}


CommsMessage AppInstaller::getNextOperation()
{
    if (mInstallDone)
    {
        return InstallOperation::getNextOperation();
    }

    CommsMessage msg;
    msg << INSTALL_OPERATION << mFilename;
    LOG1(EDebugApi, EInfo, "next operation is INSTALL(%S)", mFilename.c_str());
    return msg;
}

void AppInstaller::handleOperationResult(CommsMessage& aMessage)
{
    if (mInstallDone)
    {
        InstallOperation::handleOperationResult(aMessage);
        return;
    }

    int operation = 0;
    aMessage >> operation >> mInstallResult;

    if (!mInstallResult)
    {
        int uidCount = 0;
        aMessage >> uidCount;
        for (int i = 0; i < uidCount; i++)
        {
            Uid appUid;
            aMessage >> appUid;
            mAppUids.push_back(appUid);
        }
        aMessage >> mSuiteUid;
    }

    LOG3(EDebugApi, EInfo, "INSTALL result: result=%d, suiteUid=%S, appUid count=%d",
         mInstallResult, mSuiteUid.toString().c_str(), mAppUids.size());

    mInstallDone = true;
}
