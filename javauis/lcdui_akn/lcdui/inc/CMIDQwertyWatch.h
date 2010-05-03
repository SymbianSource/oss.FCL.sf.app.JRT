/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/


#ifndef CMIDQWERTYWATCH_H
#define CMIDQWERTYWATCH_H

// basic api using (CActive, CleanupStack ...)
#include <e32base.h>
// RProperty iProperty
#include <e32property.h>

/** An observer of CMIDQwertyWatch */
class MMIDQwertyObserver
{
public:
    virtual void NewQwertyValueL(TInt aNewValue) = 0;
};

/**  Active object that tracks changes to the qwerty property */
NONSHARABLE_CLASS(CMIDQwertyWatch) : public CActive
{
    enum {EPriority=0};
public:
    static CMIDQwertyWatch* NewL(MMIDQwertyObserver* aObserver);
    ~CMIDQwertyWatch();

private:
    CMIDQwertyWatch(MMIDQwertyObserver* aObserver);
    void ConstructL();

    void RunL();
    void DoCancel();
private:
    RProperty iProperty;
    MMIDQwertyObserver* iObserver;
};

#endif // CMIDQWERTYWATCH_H
