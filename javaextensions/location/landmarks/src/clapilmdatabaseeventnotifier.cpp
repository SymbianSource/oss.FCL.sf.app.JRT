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
* Description:  Handles native landmark database events
 *
*/


// INTERNAL INCLUDES
#include    "clapilmdatabaseeventnotifier.h"
#include    "mlapilmdatabaseobserver.h"
#include    "lapipanics.h"
#include    "logger.h"

// EXTERNAL INCLUDES
#include    <EPos_CPosLandmarkDatabase.h>

// ---------------------------------------------------------------------------
// CLAPILmDatabaseEventNotifier::NewLC
// ---------------------------------------------------------------------------
//
CLAPILmDatabaseEventNotifier* CLAPILmDatabaseEventNotifier::NewLC(
    CPosLandmarkDatabase& aDatabase)
{
    JELOG2(EJavaLocation);
    CLAPILmDatabaseEventNotifier* self =
        new(ELeave) CLAPILmDatabaseEventNotifier(aDatabase);
    CleanupStack::PushL(self);
    return self;
}

// ---------------------------------------------------------------------------
// CLAPILmDatabaseEventNotifier::~CLAPILmDatabaseEventNotifier
// ---------------------------------------------------------------------------
//
CLAPILmDatabaseEventNotifier::~CLAPILmDatabaseEventNotifier()
{
    JELOG2(EJavaLocation);
    Cancel();
    iEventObservers.Close();
}

// ---------------------------------------------------------------------------
// CLAPILmDatabaseEventNotifier::AddObserverL
// ---------------------------------------------------------------------------
//
void CLAPILmDatabaseEventNotifier::AddObserverL(
    MLAPILmDatabaseObserver* aObserver)
{
    JELOG2(EJavaLocation);
    __ASSERT_DEBUG(aObserver, LAPIError::Panic(ELAPIPanicNullArgument));
    iEventObservers.AppendL(aObserver);
}

// ---------------------------------------------------------------------------
// CLAPILmDatabaseEventNotifier::RemoveObserver
// ---------------------------------------------------------------------------
//
void CLAPILmDatabaseEventNotifier::RemoveObserver(
    MLAPILmDatabaseObserver* aObserver)
{
    JELOG2(EJavaLocation);
    __ASSERT_DEBUG(aObserver, LAPIError::Panic(ELAPIPanicNullArgument));
    TInt index = iEventObservers.Find(aObserver);
    if (index != KErrNotFound)
    {
        iEventObservers.Remove(index);
    }
}

// ---------------------------------------------------------------------------
// CLAPILmDatabaseEventNotifier::IsEventListeningSupported
// ---------------------------------------------------------------------------
//
TBool CLAPILmDatabaseEventNotifier::IsEventListeningSupported()
{
    JELOG2(EJavaLocation);
    // Event listening is not supported if the first request has returned
    // with KErrNotSupported error code. If there are requests pending, it
    // means that the request is pending and event listening is supported
    return iStatus != KErrNotSupported;
}

// ---------------------------------------------------------------------------
// CLAPILmDatabaseEventNotifier::RunL
// ---------------------------------------------------------------------------
//
void CLAPILmDatabaseEventNotifier::RunL()
{
    JELOG2(EJavaLocation);
    LOG1(EJavaLocation, EInfo, "CLAPILmDatabaseEventNotifier::RunL - event %d",
         iEvent.iEventType);
    // Listening database events is not supported. Stop listening and return
    if (iStatus == KErrNotSupported)
    {
        return;
    }
    else
    {
        TPosLmEvent curEvent(iEvent);
        TRequestStatus curStatus(iStatus);
        // Request a new database event. The operation completes when a change
        // occurs in the native database so the callback does not occur immediately
        // Call this before notifying the observer because events may occur
        // very frequently so subsequent events will not be missed
        iDatabase.NotifyDatabaseEvent(iEvent, iStatus);
        SetActive();

        // Notify observers if the change occured without any errors. All events
        // are informed to the observers which are responsible for filtering
        // incorrect events
        if (curStatus == KErrNone)
        {
            TInt observerCount = iEventObservers.Count();
            for (TInt i = 0; i < observerCount; i++)
            {
                iEventObservers[i]->NotifyEvent(curEvent.iEventType,
                                                curEvent.iLandmarkItemId);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// CLAPILmDatabaseEventNotifier::DoCancel
// ---------------------------------------------------------------------------
//
void CLAPILmDatabaseEventNotifier::DoCancel()
{
    JELOG2(EJavaLocation);
    // Cancel listening database events
    iDatabase.CancelNotifyDatabaseEvent();
}

// ---------------------------------------------------------------------------
// CLAPILmDatabaseEventNotifier::CLAPILmDatabaseEventNotifier
// ---------------------------------------------------------------------------
//
CLAPILmDatabaseEventNotifier::CLAPILmDatabaseEventNotifier(
    CPosLandmarkDatabase& aDatabase) :
        CActive(EPriorityStandard), iDatabase(aDatabase)
{
    JELOG2(EJavaLocation);
    CActiveScheduler::Add(this);
    // Issue database change request. Note that the request completes only
    // when a change occurs in the native database
    iDatabase.NotifyDatabaseEvent(iEvent, iStatus);
    SetActive();
}

// End of file
