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
* Description:
*
*/


#include "timeouttimer.h"




CTimeOutTimer::CTimeOutTimer(const TInt aPriority)
        : CTimer(aPriority)
{
}

CTimeOutTimer::~CTimeOutTimer()
{
    Cancel();
}

EXPORT_C CTimeOutTimer* CTimeOutTimer::NewL(const TInt aPriority, MTimeOutNotify& aTimeOutNotify)
{
    CTimeOutTimer *p = new(ELeave) CTimeOutTimer(aPriority);
    CleanupStack::PushL(p);
    p->ConstructL(aTimeOutNotify);
    CleanupStack::Pop();
    return p;
}

void CTimeOutTimer::ConstructL(MTimeOutNotify &aTimeOutNotify)
{
    iNotify=&aTimeOutNotify;
    CTimer::ConstructL();
    CActiveScheduler::Add(this);
}

void CTimeOutTimer::RunL()
// Timer request has completed, so notify the timer's owner
{
    iNotify->TimerExpired();
}


//  End of File
