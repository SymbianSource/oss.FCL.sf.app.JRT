/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  THIS FILE IS NOT INCLUDED INTO ECLIPSE CVS DELIVERY
*               Provides a way to invoke some functionality
*               which can run in ESWT thread only
*
*/


#ifndef SWTDCOBSERVER_H
#define SWTDCOBSERVER_H

// Using MSwtDcEventConsumer
#include "swtdceventconsumer.h"
// Using RPointerArray
#include <e32std.h>
// Using RMsgQueue
#include <e32msgqueue.h>

// Forward declarations
class MUiEventConsumer;
class CCoeControl;

/**
 * Provides a way to make calls to functions which are able
 * to run in ESWT thread only and
 * which may origin in non-ESWT thread (MMAPI for instance).
 *
 * The calls are kept in queue and processed serially.
 *
 * If the call works with some CCoeControl instance,
 * ther control must be registered by RegisterControl().
 *
 * Some events may invoke a callback into specified callback interface.
 * The receiver of this callback must not be deleted if there is
 * a possibility of having some events, which will call this, in queue.
 *
 * The callback receiver can for example implement
 * its instance deletion via event using this observer,
 * so this deletion event will be the last one for that receiver.
 *
 * @lib    eswtdirectcontent.dll
 * @since  S60 v5.2
 */
class CSwtDcObserver : public CActive
{
public:
    /**
     * Constructor.
     */
    CSwtDcObserver();

    /**
     * Destructor.
     */
    ~CSwtDcObserver();


    /**
     * Asynchronously sends an event into a consumer.
     * The call may origin in other than ESWT thread.
     *
     * @since S60 5.2
     * @param aConsumer Consumer which gets event asynchronously.
     * @param aType An data indicating event type for consumer.
     */
    virtual void InvokeDcEvent(MSwtDcEventConsumer& aConsumer, TInt aType);

    /**
     * Invokes aConsumer->MdcDSAResourcesCallback from ESWT thread.
     * The call may origin in other than ESWT thread.
     *
     * @since S60 5.2
     * @param aControl Control used to get DSA resources from.
     * @param aConsumer Consumer of the callback.  The instance
     *                  must not be deleted before the event processed.
     */
    void InvokeDSAResourcesCallback(
        CCoeControl& aControl,
        MUiEventConsumer& aConsumer);

    /**
     * Invokes aConsumer->MdcUICallback( aCallbackId ) from ESWT thread.
     * The call may origin in other than ESWT thread.
     *
     * @since S60 5.2
     * @param aConsumer Consumer of the callback.
     * @param aCallbackId Id which is provided to the callback
     */
    void InvokeUICallback(MUiEventConsumer& aConsumer, TInt aCallbackId);
    
private:
    /** CSwtDcObserver event datatype */
    enum TDcEventType
    {
        EDcEvent,
        EDSAResourcesCallbackEvent,
        EUICallbackEvent
    };

    /** CSwtDcObserver event content datatype */
    struct TDcEventData
    {
        TDcEventType iType;
        void* iConsumer;
        void* iParam;
    };

    /**
     * Sends an event into a consumer.
     *
     * @since S60 5.2
     * @param aConsumer Consumer of the event.
     * @param aType Type of event given to the invoke method.
     */
    void DoDcEvent(MSwtDcEventConsumer* aConsumer, TInt aType);

    /**
     * Invokes callback aConsumer->MdcDSAResourcesCallback.
     *
     * @since S60 5.2
     * @param aControl Control used to get DSA resources from.
     * @param aConsumer Consumer of the callback.
     */
    void DoInvokeDSAResourcesCallback(
        CCoeControl* aControl,
        MUiEventConsumer *aConsumer);

    /**
     * Invokes callback aConsumer->MdcUICallback.
     *
     * @since S60 5.2
     * @param aConsumer Consumer of the callback.
     * @param aCallbackId Id which is provided to the callback
     */
    void DoInvokeUICallback(
        MUiEventConsumer *aConsumer,
        TInt aCallbackId);

    
// from base class CActive

    /**
     * From CActive.
     * Handles an active object’s request completion event.
     * Processes the first available event in queue.
     *
     * @since S60 5.2
     */
    virtual void RunL();

    /**
     * From CActive.
     * Implements cancellation of an outstanding request.
     *
     * @since S60 5.2
     */
    virtual void DoCancel();

private:
    /**
     * Event queue to store the call requests
     * Own.
     */
    RMsgQueue<TDcEventData> iEventQueue;
};


#endif // SWTDCOBSERVER_H
