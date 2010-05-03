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
* Description:  Debug API implementation.
*
*/


#include <string>
#include <list>
#include <unistd.h>

#include "javadebugapi.h"
#include "javaoslayer.h"

#include "logger.h"
#include "javasymbianoslayer.h"
#include "javauid.h"
#include "debugapiimpl.h"

using java::util::Uid;

OS_EXPORT
TBool java::debug::installApp(HBufC& aFilename,
                              TUid& aSuiteUid,
                              RArray<TUid>& aApplicationUid)
{
    LOG1(EDebugApi, EInfo, "-->%s", __PRETTY_FUNCTION__);

    aSuiteUid = TUid::Null();
    aApplicationUid.Reset();

    TPtr16 filePtr(aFilename.Des());
    std::wstring filename(desToWstring(filePtr));
    Uid suiteUid;
    std::list<Uid> appUids;

    DebugApiImpl debug;
    int rc = debug.installApp(filename, suiteUid, appUids);
    if (!rc)
    {
        rc =  uidToTUid(suiteUid, aSuiteUid);

        for (std::list<Uid>::iterator iter = appUids.begin();
                iter != appUids.end() && rc == KErrNone;
                ++iter)
        {
            TUid uid;
            rc = uidToTUid((*iter), uid);
            if (rc == KErrNone)
            {
                rc = aApplicationUid.Append(uid);
            }
        }
    }

    if (rc)
    {
        ELOG2(EDebugApi, "installApp() failed: file=%S, err=%d", filename.c_str(), rc);
        return EFalse;
    }

    LOG1(EDebugApi, EInfo, "<--%s", __PRETTY_FUNCTION__);
    return ETrue;
}


OS_EXPORT
TBool java::debug::uninstallApp(TUid aSuiteUid)
{
    LOG1(EDebugApi, EInfo, "-->%s", __PRETTY_FUNCTION__);

    Uid suiteUid;
    TUidToUid(aSuiteUid, suiteUid);
    DebugApiImpl debug;
    int rc = debug.uninstallApp(suiteUid);
    if (rc)
    {
        ELOG2(EDebugApi, "uninstallApp() failed: uid=%S, err=%d", suiteUid.toString().c_str(), rc);
        return EFalse;
    }

    LOG1(EDebugApi, EInfo, "<--%s", __PRETTY_FUNCTION__);
    return ETrue;
}


OS_EXPORT
TBool java::debug::startApp(TUid aApplicationUid, HBufC& aUeiParameters, TProcessId& aProcessId)
{
    LOG1(EDebugApi, EInfo, "-->%s", __PRETTY_FUNCTION__);

    Uid appUid;
    TUidToUid(aApplicationUid, appUid);
    TPtr16 ueiPtr(aUeiParameters.Des());
    std::wstring cmdLineOpts(desToWstring(ueiPtr));

    DebugApiImpl debug;
    pid_t pid = 0;
    int rc = debug.startApp(appUid, cmdLineOpts, pid);
    aProcessId = pid;
    if (rc)
    {
        ELOG3(EDebugApi, "startApp() failed: uid=%S, cmdLineOpts=%S err=%d",
              appUid.toString().c_str(), cmdLineOpts.c_str(), rc);
        return EFalse;
    }

    LOG1(EDebugApi, EInfo, "<--%s", __PRETTY_FUNCTION__);
    return ETrue;
}


OS_EXPORT
TBool java::debug::stopApp(TUid aApplicationUid)
{
    LOG1(EDebugApi, EInfo, "-->%s", __PRETTY_FUNCTION__);

    Uid uid;
    TUidToUid(aApplicationUid, uid);

    DebugApiImpl debug;
    int rc = debug.stopApp(uid);
    if (rc)
    {
        ELOG2(EDebugApi, "stopApp() failed: uid=%S, err=%d", uid.toString().c_str(), rc);
        return EFalse;
    }

    LOG1(EDebugApi, EInfo, "<--%s", __PRETTY_FUNCTION__);
    return ETrue;
}
