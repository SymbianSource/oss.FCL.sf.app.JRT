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


#include "utils.h"
#include "swtevents.h"
#include "swteventqueue.h"


const TInt KMergePaintAreaRatio = 2;


// ======== MEMBER FUNCTIONS ========


// -----------------------------------------------------------------------------
// CSwtEventQueue::CSwtEventQueue
// -----------------------------------------------------------------------------
//
inline CSwtEventQueue::CSwtEventQueue(ASwtDisplayBase& aDisplay)
        : iDisplay(aDisplay)
        , iOptimizedPaint(ETrue)
{
}

// -----------------------------------------------------------------------------
// CSwtEventQueue::~CSwtEventQueue
// -----------------------------------------------------------------------------
//
CSwtEventQueue::~CSwtEventQueue()
{
    ASSERT(iDisplay.IsCurrentThreadJavaUi());
    ASSERT(!iEvents);
    // https://bugs.eclipse.org/bugs/show_bug.cgi?id=153012
    // Do not call ASSERT(iDispatchingCount==0); This assert is not valid
    // if a call to Display.dispose() is made on the event listener
    iMutex.Close();
}

// -----------------------------------------------------------------------------
// CSwtEventQueue::ConstructL
// -----------------------------------------------------------------------------
//
inline void CSwtEventQueue::ConstructL()
{
    User::LeaveIfError(iMutex.CreateLocal());
    // The queue itself cannot be created here as it must be located on the
    // UI's heap. It will be created in PushL() when first needed.
}

// -----------------------------------------------------------------------------
// CSwtEventQueue::ConstructL
// -----------------------------------------------------------------------------
//
CSwtEventQueue* CSwtEventQueue::NewL(ASwtDisplayBase& aDisplay)
{
    ASSERT(aDisplay.IsCurrentThreadJavaUi());
    CSwtEventQueue* self = new(ELeave) CSwtEventQueue(aDisplay);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
// CSwtEventQueue::IsEmpty
// -----------------------------------------------------------------------------
//
TBool CSwtEventQueue::IsEmpty() const
{
    iMutex.Wait();
    TBool result = IsEmptyNoMutex();
    iMutex.Signal();
    return result;
}

// -----------------------------------------------------------------------------
// CSwtEventQueue::IsEmptyNoMutex
// -----------------------------------------------------------------------------
//
TBool CSwtEventQueue::IsEmptyNoMutex() const
{
    return (iEvents == NULL || iEvents->Count() == iDispatchingCount) ?
           ETrue : EFalse;
}

// -----------------------------------------------------------------------------
// CSwtEventQueue::PushL
// -----------------------------------------------------------------------------
//
void CSwtEventQueue::PushL(CSwtEvent* aEvent,
                           const TBool& aInFront /*=EFalse */)
{
    ASSERT(iDisplay.IsCurrentThreadNativeUi());
    ASSERT(aEvent);

    iMutex.Wait();
    TRAPD(error, DoPushL(aEvent, aInFront));
    iMutex.Signal();
    if (error)
    {
        delete aEvent;
        aEvent = NULL;
        User::Leave(error);
    }
}

// -----------------------------------------------------------------------------
// CSwtEventQueue::DoPushL
// -----------------------------------------------------------------------------
//
void CSwtEventQueue::DoPushL(CSwtEvent* aEvent, const TBool& aInFront)
{
    if (iDestroying)
    {
        delete aEvent;
        aEvent = NULL;
    }
    else
    {
        // Push onto the queue (create it if needed)
        if (!iEvents)
        {
            iEvents = new(ELeave) CEventArray(4);
        }
        if (aInFront)
        {
            iEvents->InsertL(0, aEvent);
        }
        else
        {
            iEvents->AppendL(aEvent);
        }

        // The event needs to know in which event queue it is proceessed.
        aEvent->PushedToQueue(this);

        // Notify
        iDisplay.HandleNewEvent();
    }
}

// -----------------------------------------------------------------------------
// CSwtEventQueue::PopAndDispatchPaintEvents
// -----------------------------------------------------------------------------
//
void CSwtEventQueue::PopAndDispatchPaintEvents(JNIEnv& aJniEnv,
        MSwtControl* aSwtControl)
{
    ASSERT(iDisplay.IsCurrentThreadJavaUi());

    // Get the Mutex
    iMutex.Wait();

    if (IsEmptyNoMutex())
    {
        iMutex.Signal();
        return;
    }

    // Merge all paint events which are related to given control or to no control
    // at all. All such events in queue are replaced with a single event which is
    // then put behind the currently being dispatched event in queue.
    // The reason of having the paint event in queue is that the event is deleted
    // together with queue in UI thread if some java listener will dospose the
    // disply during dispatching.
    CSwtPaintEvent* paintEventOfAllArea = NULL;

    for (TInt i = iDispatchingCount; i < iEvents->Count(); i++)
    {
        CSwtEvent* event = (*iEvents)[i];

        if (event->Type() == ESwtEventPaint)
        {
            CSwtPaintEvent* paintEvent = static_cast<CSwtPaintEvent*>(event);

            // check owner of the event
            if ((!aSwtControl) ||
                    (aSwtControl->JavaPeer() == paintEvent->iSenderPeer))
            {
                if (!paintEventOfAllArea)
                {
                    // check position of event in queue
                    if (i != iDispatchingCount)
                    {
                        // there shouldnt be any problem with leaving from
                        // "InsertL()" function since "Delete()" doesnt actually
                        // free any memory and thus there should be space for
                        // insertion
                        iEvents->Delete(i);
                        TRAPD(err, iEvents->InsertL(iDispatchingCount, event));

                        if (err != KErrNone)
                        {
                            iMutex.Signal();
                            DeleteInUiHeap(event);
                            return;
                        }
                    }

                    // event to be dispatched
                    iDispatchingCount++;
                    paintEventOfAllArea = paintEvent;
                }
                else
                {
                    // merge event
                    iEvents->Delete(i--);
                    paintEventOfAllArea->iRect.BoundingRect(paintEvent->iRect);
                    DeleteInUiHeap(event);
                }
            }
        }
    }

    // Release the Mutex.
    iMutex.Signal();

    // Dispatch
    if (paintEventOfAllArea)
    {
        paintEventOfAllArea->Dispatch(&aJniEnv);

        // A call to the Display.dispose() has been made in the event listener
        // and event queue with all events is already destroyed. Check this from
        // Tls in order not to access freed memory and abort rest to allow exit.
        if (!CSwtDisplay::CurrentOrNull())
        {
            return;
        }

        // Remove from array
        iMutex.Wait();

        CSwtEvent* event = static_cast<CSwtEvent*>(paintEventOfAllArea);
        ASSERT((*iEvents)[iDispatchingCount - 1] == event);
        iEvents->Delete(--iDispatchingCount);

        iMutex.Signal();

        // Destroy
        DeleteInUiHeap(event);
    }
}

// -----------------------------------------------------------------------------
// CSwtEventQueue::PopAndDispatch
// -----------------------------------------------------------------------------
//
TBool CSwtEventQueue::PopAndDispatch(JNIEnv& aJniEnv)
{
    ASSERT(iDisplay.IsCurrentThreadJavaUi());

    iMutex.Wait();

    if (IsEmptyNoMutex())
    {
        iMutex.Signal();
        return EFalse;
    }

    // Pop
    CSwtEvent* event = (*iEvents)[iDispatchingCount++];
    if (event->Type() == ESwtEventPaint)
    {
        CSwtPaintEvent& paintEvent = static_cast<CSwtPaintEvent&>(*event);
        if (paintEvent.iMergeable)
        {
            TInt i = iDispatchingCount;
            while (i < iEvents->Count())
            {
                CSwtEvent* event2 = (*iEvents)[i];

                // Jump over paint events of other controls.
                if (event2->Peer() != event->Peer())
                {
                    i++;
                    continue;
                }

                // Decide if jumping over non paint events is allowed,
                // depenging on aggressive paint merging status.
                if (event2->Type() != ESwtEventPaint)
                {
                    if (iOptimizedPaint)
                    {
                        i++;
                        continue;
                    }
                    else
                    {
                        break;
                    }
                }

                // Stop merging if the reached paint event is non meargeable.
                CSwtPaintEvent& paintEvent2 = static_cast<CSwtPaintEvent&>(*event2);
                if (!paintEvent2.iMergeable)
                {
                    break;
                }

                // Merge paint rectangles if the area of the bounding rectangle
                // is less than the double of their combined areas.
                TInt areaRectInt1 = paintEvent.iRect.Height() * paintEvent.iRect.Width();
                TInt areaRectInt2 = paintEvent2.iRect.Height() * paintEvent2.iRect.Width();
                TRect bndRect = paintEvent.iRect;
                bndRect.BoundingRect(paintEvent2.iRect);
                TInt areaBndRect = bndRect.Height() * bndRect.Width();
                if (areaBndRect <= KMergePaintAreaRatio * (areaRectInt1 + areaRectInt2))
                {
                    paintEvent.iRect.BoundingRect(paintEvent2.iRect);
                    DeleteInUiHeap(event2);
                    iEvents->Delete(i);
                }
                else
                {
                    i++;
                }
            }
        }
    }

    // Dispatch event
    iMutex.Signal();
    event->Dispatch(&aJniEnv);

    // A call to the Display.dispose() has been made in the event listener
    // and event queue with all events is already destroyed. Check this from
    // Tls in order not to access freed memory and abort rest to allow exit.
    if (!CSwtDisplay::CurrentOrNull())
    {
        return ETrue;
    }

    // Remove from array
    iMutex.Wait();
    ASSERT((*iEvents)[iDispatchingCount - 1] == event);
    iEvents->Delete(--iDispatchingCount);
    iMutex.Signal();

    // Destroy
    DeleteInUiHeap(event);

    return ETrue;
}

// -----------------------------------------------------------------------------
// CSwtEventQueue::PrepareForDestruction
// -----------------------------------------------------------------------------
//
void CSwtEventQueue::PrepareForDestruction()
{
    ASSERT(iDisplay.IsCurrentThreadJavaUi());
    iMutex.Wait();
    iDestroying = ETrue;
    iMutex.Signal();
}

// -----------------------------------------------------------------------------
// CSwtEventQueue::Flush
// -----------------------------------------------------------------------------
//
void CSwtEventQueue::Flush()
{
    ASSERT(iDisplay.IsCurrentThreadNativeUi());
    ASSERT(iDestroying);

    iMutex.Wait();
    if (iEvents)
    {
        iEvents->ResetAndDestroy();
        delete iEvents;
        iEvents = NULL;
    }
    iMutex.Signal();
}

// -----------------------------------------------------------------------------
// CSwtEventQueue::PeerDisposed
// -----------------------------------------------------------------------------
//
void CSwtEventQueue::PeerDisposed(TSwtPeer aPeer)
{
    ASSERT(iDisplay.IsCurrentThreadJavaUi());

    if (!iEvents)
    {
        return;
    }

    iMutex.Wait();

    // Inform the currently dispatching events
    TInt i;
    for (i = 0; i < iDispatchingCount; ++i)
    {
        CSwtEvent* event = (*iEvents)[i];
        if (event->Peer() == aPeer)
        {
            event->PeerDisposed();
        }
    }

    // Remove events originating from the disposed object
    // Going backwards is both faster and easier
    const TInt count = iEvents->Count();
    for (i = count - 1; i >= iDispatchingCount; --i)
    {
        CSwtEvent* event = (*iEvents)[i];
        if (event->Peer() == aPeer)
        {
            iEvents->Delete(i);
            DeleteInUiHeap(event);
        }
    }

    iMutex.Signal();
}

// -----------------------------------------------------------------------------
// CSwtEventQueue::SetOptimizedPaint
// -----------------------------------------------------------------------------
//
void CSwtEventQueue::SetOptimizedPaint(TBool aStatus)
{
    iOptimizedPaint = aStatus;
}

// -----------------------------------------------------------------------------
// CSwtEventQueue::IsDestroying
// -----------------------------------------------------------------------------
//
TBool CSwtEventQueue::IsDestroying() const
{
    return iDestroying;
}
