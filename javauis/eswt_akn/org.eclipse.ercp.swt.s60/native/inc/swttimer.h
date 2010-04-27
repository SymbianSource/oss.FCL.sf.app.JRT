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


#ifndef SWTTIMER_H
#define SWTTIMER_H


#include <e32base.h>
#include "eswt.h"


class ASwtDisplayBase;


/**
 * CSwtTimer
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtTimer)
        : public CBase
{
public:
    static CSwtTimer* NewL(ASwtDisplayBase& aDisplay);
    ~CSwtTimer();

public:
    void ExecuteAfterLD(TInt aDelayInMilliSecs, TInt aTimerHandle);

private:
    inline CSwtTimer(ASwtDisplayBase& aDisplay);
    void ConstructL();

private:
    static TInt TimerCallback(TAny* aThis);

    void DoTimerCallbackL();
    void ExecuteCallbackJavaSideL();

private:
    ASwtDisplayBase& iDisplay;
    CPeriodic*       iTimer;
    TInt             iTimerHandle;
};

#endif // SWTTIMER_H
