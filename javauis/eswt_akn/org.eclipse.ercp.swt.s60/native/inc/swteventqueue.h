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


#ifndef SWTEVENTQUEUE_H
#define SWTEVENTQUEUE_H


#include <e32base.h>
#include <jni.h>
#include "eswt.h"


class ASwtDisplayBase;
class CSwtEvent;


/**
 * CSwtEventQueue
 * A "first in / first out" queue for storing events until they are dispatched.
 * The queue is synchronised using a mutex to avoid concurrent accesses of
 * the main thread and the App UI's thread.
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtEventQueue)
        : public CBase
{
// Own methods
public:
    /**
     * Two phase constructor.
     * Must be executed from within the main thread.
     * @param aDisplay The display to be notified when an event is received.
     */
    static CSwtEventQueue* NewL(ASwtDisplayBase& aDisplay);

    /**
     * Destructor
     */
    virtual ~CSwtEventQueue();

    /**
     * Tests if the queue is empty.
     */
    TBool IsEmpty() const;

    /**
     * Appends an event at the end of queue and notifies of the reception
     * of an event. The queue takes ownership of the event.
     * Must be executed from within the App UI's thread.
     */
    void PushL(CSwtEvent* aEvent, const TBool& aInFront = EFalse);

    /**
     * Removes the oldest undispatched event from the queue and dispatches it.
     * If the queue contains no undispatched event, no event is dispatched
     * and EFalse is returned. Must be executed from within the main thread.
     * @return EFalse if a message was dispatched, ETrue otherwise.
     */
    TBool PopAndDispatch(JNIEnv& aJniEnv);

    /**
     * Removes all undispatched PaintEvent from the queue and dispatches it.
     * Must be executed from within the main thread. This method is used by
     * Update() method.
     * @param aSwtControl If equals NULL then all paint events of all control
     * are dispatch otherwise only the paint event of the Control correspondent
     * to Peer was dispatched.
     */
    void PopAndDispatchPaintEvents(JNIEnv& aJniEnv, MSwtControl* aSwtControl);

    /**
     * Prepares the queue for destruction. The queue will no longer notify
     * of new events it receives.
     */
    void PrepareForDestruction();

    /**
     * Test whether the queue is about to be destroyed. This is a way for
     * letting events check if the display was disposed and thus stop
     * executing.
     */
    TBool IsDestroying() const;

    /**
     * Destroys all undispatched events the queue still holds.
     * Must be executed from within the App UI's thread.
     */
    void Flush();

    /**
     * Informs the event queue that the specified Java object has been disposed.
     * Must be executed from within the main thread.
     */
    void PeerDisposed(TSwtPeer aPeer);

    /**
     * Turns on/off aggressive paint merging. If the aggressive paint merging
     * is turned on, all the paint events of a control are merged, disregarding
     * non paint events positioned in between them.
     */
    void SetOptimizedPaint(TBool aStatus);

private:
    /**
     * First phase constructor.
     * @param aDisplay The display to be notified when an event is received.
     */
    CSwtEventQueue(ASwtDisplayBase& aDisplay);

    /**
     * Second phase constructor.
     */
    void ConstructL();

    /**
     * PushL helper.
     */
    void DoPushL(CSwtEvent* aEvent, const TBool& aInFront);

    /**
     * Tests if the queue is empty, but does not use the mutex.
     */
    TBool IsEmptyNoMutex() const;

// Data types
private:
    typedef CArrayPtrFlat<CSwtEvent> CEventArray;

// Data
private:
    /**
     * The events, may be NULL. Both the array and the events are on the UI's heap
     * Own.
     */
    CEventArray* iEvents;

    /**
     * Related display reference.
     */
    ASwtDisplayBase& iDisplay;

    /**
     * The mutex.
     * Own.
     */
    mutable RMutex iMutex;

    /**
     * The number of events currently being dispatched
     */
    volatile TInt iDispatchingCount;

    /**
     * Flag indicating whether destruction is ongoing.
     */
    TBool iDestroying;

    /**
     * Flag indicating whether aggressive paint merging is allowed.
     * Set to ETrue when the application registers paint listeners.
     */
    TBool iOptimizedPaint;
};


#endif // SWTEVENTQUEUE_H
