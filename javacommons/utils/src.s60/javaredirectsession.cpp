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
* Description: CRedirectServer session
*
*/

#include "javaredirectsession.h"
#include "javaredirectconsts.h"
#include "javadiagnostic.h"
using namespace java::util;


CRedirectSession::CRedirectSession(DiagnosticListener& aListener) : mListener(aListener)
{
}

CRedirectSession::~CRedirectSession()
{
}

void CRedirectSession::ServiceL(const RMessage2& aMessage)
{
    TInt function = aMessage.Function();
    switch (function)
    {
    case ESystemOut:
    case ESystemErr:
    case ELog:
        handleMessageL(aMessage);
        break;

    default:
        break;
    }
    aMessage.Complete(KErrNone);
}

void CRedirectSession::handleMessageL(const RMessage2& aMessage)
{
    TInt len = aMessage.GetDesLengthL(0);

    RBuf8 data;
    data.CleanupClosePushL();
    data.CreateL(len);
    aMessage.ReadL(0,data);

    TInt function = aMessage.Function();
    dispatchToListener(function, data);

    CleanupStack::PopAndDestroy();
}

void CRedirectSession::dispatchToListener(TInt aType, const TDesC8& aData)
{
    switch (aType)
    {
    case ESystemOut:
        mListener.systemOut(aData);
        break;
    case ESystemErr:
        mListener.systemErr(aData);
        break;
    case ELog:
        mListener.log(aData);
        break;
    default:
        break;
    }
}

