/*******************************************************************************
 * Copyright (c) 2005, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#include "swttimer.h"
#include "swtdisplaybase.h"
#include "swtevents.h"


// ======== MEMBER FUNCTIONS ========


CSwtTimer* CSwtTimer::NewL(ASwtDisplayBase& aDisplay)
{
    CSwtTimer* self = new(ELeave) CSwtTimer(aDisplay);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

CSwtTimer::~CSwtTimer()
{
    delete iTimer;
}

void CSwtTimer::ExecuteAfterLD(TInt aDelayInMilliSecs, TInt aTimerHandle)
{
    if (iTimer->IsActive())
        User::Leave(KErrInUse);

    if (aDelayInMilliSecs < 0)
        User::Leave(KErrArgument);

    iTimerHandle = aTimerHandle;
    iTimer->Start(aDelayInMilliSecs*1000,0,TCallBack(TimerCallback,this));

}

inline CSwtTimer::CSwtTimer(ASwtDisplayBase& aDisplay)
        : iDisplay(aDisplay)
{
}

void CSwtTimer::ConstructL()
{
    iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
    iDisplay.RegisterTimerL(this);
}

TInt CSwtTimer::TimerCallback(TAny* aThis)
{
    CSwtTimer* self = static_cast<CSwtTimer*>(aThis);
    TInt err = KErrNone;

    if (self)
    {
        TRAP(err, self->DoTimerCallbackL()); //will be destroyed
    }

    return err;
}

void CSwtTimer::DoTimerCallbackL()
{
    iTimer->Cancel();
    ExecuteCallbackJavaSideL();

    iDisplay.UnRegisterTimer(this);
    delete this;
}

void CSwtTimer::ExecuteCallbackJavaSideL()
{
    CSwtTimerCallback* callback = new(ELeave) CSwtTimerCallback(iDisplay.JavaPeer(), iTimerHandle);
    iDisplay.EventQueue()->PushL(callback);
}
