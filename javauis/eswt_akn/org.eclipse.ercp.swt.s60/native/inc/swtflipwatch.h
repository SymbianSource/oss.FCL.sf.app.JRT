/*******************************************************************************
 * Copyright (c) 2007, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#ifndef SWTFLIPWATCH_H
#define SWTFLIPWATCH_H


#include <e32base.h>
#include <e32property.h>


/**
 * MSwtFlipWatchTimerHandler
 */
class MSwtFlipWatchTimerHandler
{
public:
    virtual void HandleTimeoutL() = 0;
};


/**
 * CSwtFlipWatchTimer
 * Timer used by the flip watch.
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtFlipWatchTimer)
        : public CActive
{
// Own methods
public:
    static CSwtFlipWatchTimer* NewL(MSwtFlipWatchTimerHandler* aHandler);
    virtual ~CSwtFlipWatchTimer();
    void SetTimer(TTimeIntervalMicroSeconds32 aDelay);
private:
    CSwtFlipWatchTimer(MSwtFlipWatchTimerHandler* aHandler);
    void ConstructL();
    void RunL();
    void DoCancel();

// Data
private:
    RTimer iTimer;
    MSwtFlipWatchTimerHandler* iHandler;
};


/**
 * MSwtFlipObserver
 */
class MSwtFlipObserver
{
public:
    /**
     * Flip being changed. In the case of several quick swaps of the flip
     * this notification will be sent continuosly.
     */
    virtual void FlipChangingL() = 0;

    /**
     * End of flip change. In the case of several quick swaps of the flip
     * this notification will be sent once after the quick swap stops.
     */
    virtual void FlipChangedL(TBool aState) = 0;
};


/**
 * CSwtFlipWatchTimer
 * Flip watch. Observers and reports changes in the flip status.
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtFlipWatch)
        : public CActive
        , public MSwtFlipWatchTimerHandler
{
// Own methods
public:
    static CSwtFlipWatch* NewL(MSwtFlipObserver* aObserver);
    virtual ~CSwtFlipWatch();
private:
    CSwtFlipWatch(MSwtFlipObserver* aObserver);
    void ConstructL();
    void DoCancel();
    void RunL();

// From MSwtFlipWatchTimerHandler
public:
    void HandleTimeoutL();

// Data
private:
    RProperty iProperty;
    MSwtFlipObserver* iObserver;
    CSwtFlipWatchTimer* iTimer;
    TBool iFlipOpen;
};


#endif // SWTFLIPWATCH_H
