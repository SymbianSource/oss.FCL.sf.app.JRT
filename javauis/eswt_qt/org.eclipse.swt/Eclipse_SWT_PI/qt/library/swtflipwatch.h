/*******************************************************************************
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/


#ifndef SWTFLIPWATCH_H
#define SWTFLIPWATCH_H


#include <e32base.h>
#include <e32property.h>

namespace Java { namespace eSWT {

class MSwtFlipWatchTimerHandler
{
public:
    virtual void HandleTimeoutL() = 0;
};


/**
 * CSwtFlipWatchTimer
 * Timer used by the flip watch.
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
 * CSwtFlipWatchTimer
 * Flip watch. Observers and reports changes in the flip status.
 */
NONSHARABLE_CLASS(CSwtFlipWatch)
        : public CActive
        , public MSwtFlipWatchTimerHandler
{
// Own methods
public:
    static CSwtFlipWatch* NewL();
    virtual ~CSwtFlipWatch();
private:
    CSwtFlipWatch();
    void ConstructL();
    void DoCancel();
    void RunL();

// From MSwtFlipWatchTimerHandler
public:
    void HandleTimeoutL();

// Data
private:
    RProperty iProperty;
    CSwtFlipWatchTimer* iTimer;
    bool iFlipChanging;
    bool isInitializationState;
};
}}

#endif // SWTFLIPWATCH_H
