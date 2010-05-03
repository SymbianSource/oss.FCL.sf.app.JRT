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


#ifndef CAOSYNCHRONIZER_H
#define CAOSYNCHRONIZER_H

#include <e32base.h>
#include <e32def.h>

#include "timeouttimer.h"



class CAOSynchronizer :
        public CActive,
        public MTimeOutNotify

{
public:
    IMPORT_C static CAOSynchronizer* NewL();
    virtual ~CAOSynchronizer();

    IMPORT_C void ExecuteL(TTimeIntervalMicroSeconds32 aTimeout);

protected:
    inline CAOSynchronizer() : CActive(CActive::EPriorityStandard) {};
    void ConstructL();

    // From CActive
    void DoCancel();

    void RunL();

    // From CTimeoutTimer
    void TimerExpired();

private:
    void Complete();

    // Heap objects that we own
    CActiveScheduler* iCas;
    CTimeOutTimer* iTimer;

    // owned
    CActiveSchedulerWait* iWait;
};


#endif // CAOSYNCHRONIZER_H

//  End of File
