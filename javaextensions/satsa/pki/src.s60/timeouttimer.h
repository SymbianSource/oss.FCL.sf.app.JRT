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


#ifndef TIMEOUTTIMER_H
#define TIMEOUTTIMER_H

#include <e32base.h>
#include <e32def.h>




// MTimeOutNotify: used in conjunction with CTimeOutTimer class
class MTimeOutNotify
{
public:
    virtual void TimerExpired() = 0;
};

// CTimeOutTimer: timer for comms time-outs
class CTimeOutTimer : public CTimer
{
public:
    IMPORT_C static CTimeOutTimer* NewL(const TInt aPriority, MTimeOutNotify& aTimeOutNotify);
    ~CTimeOutTimer();

protected:
    CTimeOutTimer(const TInt aPriority);
    void ConstructL(MTimeOutNotify& aTimeOutNotify);
    virtual void RunL();

private:
    MTimeOutNotify* iNotify;
};


#endif // TIMEOUTTIMER_H

//  End of File
