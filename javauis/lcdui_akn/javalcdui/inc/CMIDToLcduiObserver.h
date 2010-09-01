/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides a way to invoke some functionality
*                which can run in LCDUI ES thread only
*
*/


#ifndef CMIDTOLCDUIOBSERVER_H
#define CMIDTOLCDUIOBSERVER_H

// Using RPointerArray
#include <e32std.h>
// Using RMsgQueue
#include <e32msgqueue.h>

#include "lcdui.h"

// Forward declarations
class MUiEventConsumer;
class CCoeControl;

/**
 * Provides a way to make calls to functions which are able
 * to run in LCDUI Event Server thread only and
 * which may origin in non-lcdui thread (MMAPI for instance).
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
 * @lib    lcdui.dll
 * @since  S60 v5.0
 */
class CMIDToLcduiObserver : public CActive, public MMIDToLcduiObserver
{
public:
    /**
     * Constructor.
     */
    CMIDToLcduiObserver();

    /**
     * Destructor.
     */
    ~CMIDToLcduiObserver();

// from base MMIDToLcduiObserver

    /**
     * From MMIDToLcduiObserver.
     * Allows a control to be used during event processing.
     *
     * @since S60 5.0
     * @param aControl Control to be registered.
     * @param aCallbackContainer MDirectContainer that is nofied in LCDUI thread
     *                           about added MDirectContent. NULL, if notification
     *                           is not needed.
     */
#ifdef RD_JAVA_NGA_ENABLED
    virtual void RegisterControl(CCoeControl& aControl,
                                 MDirectContainer* aCallbackContainer = NULL);
#else
    virtual void RegisterControl(CCoeControl& aControl);
#endif

    /**
     * From MMIDToLcduiObserver.
     * Removes a control from the list of controls allowed
     * to be used in event processing.
     * Events which works with this control will be ignored.
     *
     * @since S60 5.0
     * @param aControl Control to be unregistered.
     */
    virtual void UnregisterControl(CCoeControl& aControl);

    /**
     * From MMIDToLcduiObserver.
     * Flushes the control's graphics content on screen.
     * The call may origin in other than LCDUI ES thread.
     *
     * @since S60 5.0
     * @param aControl Control used to get DSA resources from.
     * @param aRect An area of the control to flush.
     */
    virtual void FlushControl(CCoeControl& aControl, const TRect &aRect);

    /**
     * From MMIDToLcduiObserver.
     * Invokes aConsumer->MdcDSAResourcesCallback from LCDUI ES thread.
     * The call may origin in other than LCDUI ES thread.
     *
     * @since S60 5.0
     * @param aControl Control used to get DSA resources from.
     * @param aConsumer Consumer of the callback.  The instance
     *                  must not be deleted before the event processed.
     */
    void InvokeDSAResourcesCallback(
        CCoeControl& aControl,
        MUiEventConsumer& aConsumer);

    /**
     * From MMIDToLcduiObserver.
     * Invokes aConsumer->MdcUICallback( aCallbackId ) from LCDUI ES thread.
     * The call may origin in other than LCDUI ES thread.
     *
     * @since S60 5.0
     * @param aConsumer Consumer of the callback.
     * @param aCallbackId Id which is provided to the callback
     */
    void InvokeUICallback(MUiEventConsumer& aConsumer, TInt aCallbackId);

    /**
     * Invokes MDirectContainer::InvokeLcduiEvent in UI thread.
     *
     * @since S60 9.2
     * @param aControl CCoeControl pointer needed for checking that registered
     *                 control has not unregistered yet.
     * @param aContainer The container to be notifed.
     */
    void InvokeLcduiEvent(
         MMIDLcduiEventConsumer& aConsumer,
         TInt aCallbackId);

private:
    /** CMIDToLcduiObserver event datatype */
    enum TToLcduiEventType
    {
        EFlushEvent,
        EDSAResourcesCallbackEvent,
        EUICallbackEvent
#ifdef RD_JAVA_NGA_ENABLED
        ,ENotifyContentAdded
#endif
        ,ELcduiEvent
    };

    /** CMIDToLcduiObserver event content datatype */
    struct TToLcduiEventData
    {
        TToLcduiEventType iType;
        CCoeControl* iControl;
        TRect iRect;
        void* iConsumer;
        TInt iId;
#ifdef RD_JAVA_NGA_ENABLED
        MDirectContainer* iContainer;
#endif
    };

    /**
     * Redraws area on component.
     *
     * @since S60 5.0
     * @param aControl Control to redraw.
     *                 Must be registered by RegisterControl.
     * @param aRect Area to redraw.
     */
    void DoFlushControl(CCoeControl* aControl, const TRect& aRect);

    /**
     * Invokes callback aConsumer->MdcDSAResourcesCallback.
     *
     * @since S60 5.0
     * @param aControl Control used to get DSA resources from.
     *                 Must be registered by RegisterControl.
     * @param aConsumer Consumer of the callback.
     */
    void DoInvokeDSAResourcesCallback(
        CCoeControl* aControl,
        MUiEventConsumer *aConsumer);

    /**
     * Invokes callback aConsumer->HandleLcduiEvent(aCallbackId)
     *
     * @since S60 9.2
     * @param aConsumer Consumer of the callback.
     * @param aCallbackId Id which is provided to the callback
     */
    void DoInvokeLcduiEvent(
         MMIDLcduiEventConsumer  *aConsumer,
         TInt aCallbackId);
    
    /**
     * Invokes callback aConsumer->MdcUICallback
     *
     * @since S60 5.0
     * @param aConsumer Consumer of the callback.
     * @param aCallbackId Id which is provided to the callback
     */
    void DoInvokeUICallback(
         MUiEventConsumer *aConsumer,
         TInt aCallbackId);


#ifdef RD_JAVA_NGA_ENABLED
    /**
     * Invokes MDirectContainer::MdcNotifyContentAdded in UI thread.
     *
     * @since S60 9.2
     * @param aControl CCoeControl pointer needed for checking that registered
     *                 control has not unregistered yet.
     * @param aContainer The container to be notifed.
     */
    void DoInvokeNotifyContentAdded(
        CCoeControl* aControl,
        MDirectContainer *aContainer);
#endif

// from base class CActive

    /**
     * From CActive.
     * Handles an active object’s request completion event.
     * Processes the first available event in queue.
     *
     * @since S60 5.0
     */
    virtual void RunL();

    /**
     * From CActive.
     * Implements cancellation of an outstanding request.
     *
     * @since S60 5.0
     */
    virtual void DoCancel();    

private:
    /**
     * Event queue to store the call requests
     * Own.
     */
    RMsgQueue<TToLcduiEventData> iEventQueue;

    /**
     * List of controls registered to allowed its usage
     * during event processing.
     * Controls are not owned.
     * Own.
     */
    RPointerArray<CCoeControl> iRegisteredControls;
};


#endif // CMIDTOLCDUIOBSERVER_H
