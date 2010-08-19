/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Publish and Subscribe key watcher
*
*/


#include <e32base.h>
#include "cmmacallstatemonitor.h"
#include "mmmacallstateobserver.h"

// ---------------------------------------------------------------------------
// Default Constructor
// ---------------------------------------------------------------------------
//
CMMACallStateMonitor::CMMACallStateMonitor(  TUid aUid, TInt aKey,
                                     MMMACallStateObserver* aObserver ) :
                                                     CActive( EPriorityHigh ),
                                                     iObserver( aObserver ),
                                                     iUid( aUid ),
                                                     iKey( aKey )
                                                      
    {
    }


// ---------------------------------------------------------------------------
// 2nd phased Constructor
// ---------------------------------------------------------------------------
//
void CMMACallStateMonitor::ConstructL()
    {
    CActiveScheduler::Add(this);
    
    iProperty.Attach( iUid, iKey );
    iProperty.Subscribe( iStatus );
    SetActive();
    }


// ---------------------------------------------------------------------------
// Two-Phased COnstructor
// ---------------------------------------------------------------------------
//
CMMACallStateMonitor* CMMACallStateMonitor::NewL( TUid aUid, TInt aKey,
                                                   MMMACallStateObserver* aObserver )
    {
    CMMACallStateMonitor* self = new(ELeave) CMMACallStateMonitor( aUid, 
                                                           aKey, 
                                                           aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMMACallStateMonitor::~CMMACallStateMonitor()
    {
    Cancel();  // Cancels waiting request
    iProperty.Close();
    }

// ---------------------------------------------------------------------------
// CMMAPSKeyWatcher::RunL
// ---------------------------------------------------------------------------
//
void CMMACallStateMonitor::RunL()
    {
    iProperty.Subscribe( iStatus );
    SetActive();
    
    // Notify Observer and start watching again
    if( iObserver )
        {
        iObserver->HandleCallStateEventL( iUid, iKey );
        }
    }

// ---------------------------------------------------------------------------
// CMMAPSKeyWatcher::DoCancel
// ---------------------------------------------------------------------------
//
void CMMACallStateMonitor::DoCancel()
    {
    iProperty.Cancel();
    }

// ---------------------------------------------------------------------------
// CMMAPSKeyWatcher::GetValue
// ---------------------------------------------------------------------------
//
TInt CMMACallStateMonitor::GetValue( TInt& aValue )
    {
    return iProperty.Get( aValue );
    }

