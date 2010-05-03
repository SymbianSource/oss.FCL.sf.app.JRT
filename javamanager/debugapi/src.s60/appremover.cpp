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
* Description: AppRemover uninstalls Java application
*
*/

#include "appremover.h"

#include "logger.h"
#include "commsmessage.h"

using namespace java::debug;
using namespace java::comms;

AppRemover::AppRemover(const Uid& aSuiteUid):
        mSuiteUid(aSuiteUid), mUninstallDone(false), mUninstallResult(-1)
{
}

AppRemover::~AppRemover()
{
}

int AppRemover::uninstall()
{
    JELOG2(EDebugApi);

    runJavaInstaller();
    LOG2(EDebugApi, EInfo, "InstallOperation::uninstallApp(): uid=%S, result=%d",
         mSuiteUid.toString().c_str(), mUninstallResult);
    return mUninstallResult;
}

CommsMessage AppRemover::getNextOperation()
{
    if (mUninstallDone)
    {
        return InstallOperation::getNextOperation();
    }

    CommsMessage msg;
    msg << UNINSTALL_OPERATION << mSuiteUid;
    LOG1(EDebugApi, EInfo, "next operation is UNINSTALL(%d)",
         mSuiteUid.toString().c_str());
    return msg;
}

void AppRemover::handleOperationResult(CommsMessage& aMessage)
{
    if (mUninstallDone)
    {
        InstallOperation::handleOperationResult(aMessage);
        return;
    }

    int operation = 0;
    aMessage >> operation >> mUninstallResult;

    LOG1(EDebugApi, EInfo, "UNINSTALL result: result=%d", mUninstallResult);
    mUninstallDone = true;
}


