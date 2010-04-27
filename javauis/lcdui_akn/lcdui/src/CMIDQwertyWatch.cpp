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


#include "CMIDQwertyWatch.h"
// Internal Central Repository keys, keys constants
#include <AvkonInternalCRKeys.h>

CMIDQwertyWatch* CMIDQwertyWatch::NewL(MMIDQwertyObserver* aObserver)
{
    CMIDQwertyWatch* me=new(ELeave) CMIDQwertyWatch(aObserver);
    CleanupStack::PushL(me);
    me->ConstructL();
    CleanupStack::Pop(me);
    return me;
}

CMIDQwertyWatch::CMIDQwertyWatch(MMIDQwertyObserver* aObserver)
        : CActive(EPriority), iObserver(aObserver)
{
    ASSERT(iObserver);
}

void CMIDQwertyWatch::ConstructL()
{
    User::LeaveIfError(iProperty.Attach(KCRUidAvkon,KAknQwertyInputModeActive));
    CActiveScheduler::Add(this);

    RunL();
}

CMIDQwertyWatch::~CMIDQwertyWatch()
{
    Cancel();
    iProperty.Close();
}

void CMIDQwertyWatch::DoCancel()
{
    iProperty.Cancel();
}

void CMIDQwertyWatch::RunL()
{
    iProperty.Subscribe(iStatus);
    SetActive();

    TInt value;
    TInt err = iProperty.Get(value);
    iObserver->NewQwertyValueL(err == KErrNone ? value : err);
}



