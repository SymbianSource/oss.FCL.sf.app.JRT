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
* Description:  Provides a way of sending asynchronous
*                requests from mmapi (JSR-135) to lcdui
*
*/


// Using CCoeControl
#include <coecntrl.h>
// Using MUiEventConsumer
#include <reflcdui.h>
// Using debug macros.
#include <j2me/jdebug.h>

#include "CMIDToLcduiObserver.h"


// Event queue size
const TInt KMaxQueueSlots = 30;


// ---------------------------------------------------------------------------
// CMIDToLcduiObserver::CMIDToLcduiObserver
// Constructor.
// ---------------------------------------------------------------------------
//
CMIDToLcduiObserver::CMIDToLcduiObserver():
        CActive(EPriorityStandard)
{
    // Create mutex
    iEventQueue.CreateLocal(KMaxQueueSlots);

    // Install self to Active scheduler
    CActiveScheduler::Add(this);

    iEventQueue.NotifyDataAvailable(iStatus);
    SetActive();
}


// ---------------------------------------------------------------------------
// CMIDToLcduiObserver::~CMIDToLcduiObserver
// Destructor.
// ---------------------------------------------------------------------------
//
CMIDToLcduiObserver::~CMIDToLcduiObserver()
{
    // Unregister all controls
    iRegisteredControls.Reset();

    // Cancel the active object
    Cancel();

    // Close the mutex
    iEventQueue.Close();
}


// ---------------------------------------------------------------------------
// CMIDToLcduiObserver::RegisterControl
// Allows a control to be used during event processing.
// ---------------------------------------------------------------------------
//
#ifdef RD_JAVA_NGA_ENABLED
void CMIDToLcduiObserver::RegisterControl(
    CCoeControl& aControl, MDirectContainer* /*aCallbackContainer*/)
#else
void CMIDToLcduiObserver::RegisterControl(CCoeControl& aControl)
#endif
{
    TInt index = iRegisteredControls.Find(&aControl);

    // Append the control only if it have not been added yet
    if (index == KErrNotFound)
    {
        if (iRegisteredControls.Append(&aControl) != KErrNone)
        {
            // Appending of control failed
            DEBUG("CMIDToLcduiObserver::RegisterControl - Append failed");
        }
    }
}


// ---------------------------------------------------------------------------
// CMIDToLcduiObserver::UnregisterControl
// Removes a control from the list of controls allowed
// to be used in event processing.
// Events which works with this control will be ignored.
// ---------------------------------------------------------------------------
//
void CMIDToLcduiObserver::UnregisterControl(CCoeControl& aControl)
{
    TInt index = iRegisteredControls.Find(&aControl);

    if (index != KErrNotFound)
    {
        iRegisteredControls.Remove(index);
    }
}


// ---------------------------------------------------------------------------
// CMIDToLcduiObserver::FlushControl
// Asynchronously flushes the control's graphics content on screen.
// The call may origin in other than lcdui thread.
// ---------------------------------------------------------------------------
//
void CMIDToLcduiObserver::FlushControl(
    CCoeControl& aControl,
    const TRect& aRect)
{
    // Can run in non lcdui thread

    // Prepare event data
    TToLcduiEventData data;
    data.iType = EFlushEvent;
    data.iRect = aRect;
    data.iControl = &aControl;

    // Add event to the queue
    iEventQueue.Send(data);
}


// ---------------------------------------------------------------------------
// CMIDToLcduiObserver::InvokeDSAResourcesCallback
// Asynchronously invokes aConsumer->MdcDSAResourcesCallback
// from lcdui thread.
// The call may origin in other than lcdui thread.
// ---------------------------------------------------------------------------
//
void CMIDToLcduiObserver::InvokeDSAResourcesCallback(
    CCoeControl& aControl,
    MUiEventConsumer& aConsumer)
{
    // Can run in non lcdui thread

    // Prepare event data
    TToLcduiEventData data;
    data.iType = EDSAResourcesCallbackEvent;
    data.iControl = &aControl;
    data.iConsumer = (void*)&aConsumer;

    // Add event to the queue
    iEventQueue.Send(data);
}


// ---------------------------------------------------------------------------
// CMIDToLcduiObserver::InvokeUICallback
// Asynchronously invokes aConsumer->MdcUICallback
// from lcdui thread.
// The call may origin in other than lcdui thread.
// ---------------------------------------------------------------------------
//
void CMIDToLcduiObserver::InvokeUICallback(
    MUiEventConsumer& aConsumer,
    TInt aCallbackId)
{
    // Can run in non lcdui thread

    // Prepare event data
    TToLcduiEventData data;
    data.iType = EUICallbackEvent;
    data.iConsumer = (void*)&aConsumer;
    data.iId = aCallbackId;

    // Add event to the queue
    iEventQueue.Send(data);
}


// ---------------------------------------------------------------------------
// CMIDToLcduiObserver::RunL
// Handles an active object’s request completion event.
// Processes the first available event in queue.
// ---------------------------------------------------------------------------
//
void CMIDToLcduiObserver::RunL()
{
    TToLcduiEventData data;

    // Get the event
    iEventQueue.Receive(data);

    // Process the event
    switch (data.iType)
    {
    case EFlushEvent:
        DoFlushControl(data.iControl, data.iRect);
        break;
    case EDSAResourcesCallbackEvent:
        DoInvokeDSAResourcesCallback(data.iControl, (MUiEventConsumer*)data.iConsumer);
        break;
    case EUICallbackEvent:
        DoInvokeUICallback((MUiEventConsumer*)data.iConsumer, data.iId);
        break;
    case ELcduiEvent:
        DoInvokeLcduiEvent((MMIDLcduiEventConsumer*)data.iConsumer,data.iId);
        break;
#ifdef RD_JAVA_NGA_ENABLED
    case ENotifyContentAdded:
        DoInvokeNotifyContentAdded(data.iControl, data.iContainer);
        break;
#endif
    }

    iEventQueue.NotifyDataAvailable(iStatus);
    SetActive();
}


// ---------------------------------------------------------------------------
// CMIDToLcduiObserver::DoCancel
// Implements cancellation of an outstanding request.
// ---------------------------------------------------------------------------
//
void CMIDToLcduiObserver::DoCancel()
{
    iEventQueue.CancelDataAvailable();
}


// ---------------------------------------------------------------------------
// CMIDToLcduiObserver::DoFlushControl
// Processes the event invoked by FlushControl.
// ---------------------------------------------------------------------------
//
void CMIDToLcduiObserver::DoFlushControl(
    CCoeControl* aControl,
    const TRect& aRect)
{
    TInt index = iRegisteredControls.Find(aControl);

    if (index != KErrNotFound)
    {
        TPoint relativePos =
            aControl->Position() - aControl->PositionRelativeToScreen();
        TRect rectToDraw = aRect;
        rectToDraw.Move(relativePos);
        aControl->DrawNow(rectToDraw);
    }
}


// ---------------------------------------------------------------------------
// CMIDToLcduiObserver::DoInvokeDSAResourcesCallback
// Processes the event invoked by InvokeDSAResourcesCallback.
// ---------------------------------------------------------------------------
//
void CMIDToLcduiObserver::DoInvokeDSAResourcesCallback(
    CCoeControl* aControl,
    MUiEventConsumer *aConsumer)
{
    TInt index = iRegisteredControls.Find(aControl);

    if (index != KErrNotFound)
    {
        RWsSession&      session = aControl->ControlEnv()->WsSession();
        CWsScreenDevice* device  = aControl->ControlEnv()->ScreenDevice();
        RDrawableWindow* window  = aControl->DrawableWindow();

        aConsumer->MdcDSAResourcesCallback(session, *device, *window);
    }
}

void CMIDToLcduiObserver::DoInvokeLcduiEvent(
    MMIDLcduiEventConsumer* aConsumer,
    TInt aCallbackId)
{
    if (aConsumer)
    {
        aConsumer->HandleLcduiEvent(aCallbackId);
    }
}

// ---------------------------------------------------------------------------
// CMIDToLcduiObserver::DoInvokeUICallback
// Processes the event invoked by InvokeUICallback.
// ---------------------------------------------------------------------------
//
void CMIDToLcduiObserver::DoInvokeUICallback(
    MUiEventConsumer *aConsumer,
    TInt aCallbackId)
{
    aConsumer->MdcUICallback(aCallbackId);
}


#ifdef RD_JAVA_NGA_ENABLED
// ---------------------------------------------------------------------------
// CMIDToLcduiObserver::DoInvokeNotifyContentAdded
// ---------------------------------------------------------------------------
void CMIDToLcduiObserver::DoInvokeNotifyContentAdded(
    CCoeControl* aControl,
    MDirectContainer *aContainer)
{
    TInt index = iRegisteredControls.Find(aControl);
    if (index != KErrNotFound)
    {
        aContainer->MdcNotifyContentAdded();
    }
}
#endif

// ---------------------------------------------------------------------------
// CSwtDcObserver::InvokeDcEvent
// Asynchronously sends an event into a consumer.
// The call may origin in other than eswt thread.
// ---------------------------------------------------------------------------
//
void CMIDToLcduiObserver::InvokeLcduiEvent(
    MMIDLcduiEventConsumer& aConsumer,
    TInt aCallbackId)
{
    // Can run in non lcdui thread

    // Prepare event data
    TToLcduiEventData data;
    data.iType = ELcduiEvent;
    data.iConsumer = (void*)&aConsumer;
    data.iId = aCallbackId;

    // Add event to the queue
    iEventQueue.Send(data);
}

