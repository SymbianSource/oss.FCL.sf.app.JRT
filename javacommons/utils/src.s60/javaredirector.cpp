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
* Description: Redirector redirects standard outs and log messages
*
*/


#include "javaredirector.h"

#include "javaredirectconsts.h"
using namespace java::util;

OS_EXPORT Redirector::Redirector() : mStatus(KErrNotReady)
{
}

OS_EXPORT Redirector::~Redirector()
{
}

OS_EXPORT TInt Redirector::systemOut(const TDesC8& aData)
{
    return send(ESystemOut, aData);
}

OS_EXPORT TInt Redirector::systemErr(const TDesC8& aData)
{
    return send(ESystemErr, aData);
}

OS_EXPORT TInt Redirector::log(const TDesC8& aData)
{
    Redirector redirector;
    redirector.connect();

    TInt rc = redirector.send(ELog, aData);
    redirector.close();

    return rc;
}

TInt Redirector::send(TInt aFunction, const TDesC8& aData)
{
    if (mStatus == KErrNone)
    {
        mStatus =  SendReceive(aFunction, TIpcArgs(&aData));
    }
    return mStatus;
}

OS_EXPORT TInt Redirector::connect()
{
    TVersion version(REDIRECT_VERSION_MAJOR, REDIRECT_VERSION_MINOR, REDIRECT_VERSION_BUILD);
    mStatus = CreateSession(KJavaRedirectServer, version, -1, EIpcSession_Sharable);
    return mStatus;
}

OS_EXPORT void Redirector::close()
{
    Close();
}
