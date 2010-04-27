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
*               (ie. asynchronous requests from mmapi (JSR-135) to eswt
*
*/


// Using CCoeControl
#include <coecntrl.h>
// Using MUiEventConsumer
#include <reflcdui.h>
// Using debug macros.
#include <j2me/jdebug.h>

#include "swtdcobserver.h"


// Event queue size
const TInt KMaxQueueSlots = 30;


// ---------------------------------------------------------------------------
// CSwtDcObserver::CSwtDcObserver
// Constructor.
// ---------------------------------------------------------------------------
//
CSwtDcObserver::CSwtDcObserver():
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
// CSwtDcObserver::~CSwtDcObserver
// Destructor.
// ---------------------------------------------------------------------------
//
CSwtDcObserver::~CSwtDcObserver()
{
    // Cancel the active object
    Cancel();

    // Close the mutex
    iEventQueue.Close();
}


// ---------------------------------------------------------------------------
// CSwtDcObserver::InvokeDcEvent
// Asynchronously sends an event into a consumer.
// The call may origin in other than eswt thread.
// ---------------------------------------------------------------------------
//
void CSwtDcObserver::InvokeDcEvent(MSwtDcEventConsumer& aConsumer, int aType)
{
    // Can run in non eswt thread

    // Prepare event data
    TDcEventData data;
    data.iType = EDcEvent;
    data.iConsumer = (void*)&aConsumer;
    data.iParam = (void*)aType;

    // Add event to the queue
    iEventQueue.Send(data);
}


// ---------------------------------------------------------------------------
// CSwtDcObserver::InvokeDSAResourcesCallback
// Asynchronously invokes aConsumer->MdcDSAResourcesCallback
// from eswt thread.
// The call may origin in other than eswt thread.
// ---------------------------------------------------------------------------
//
void CSwtDcObserver::InvokeDSAResourcesCallback(
    CCoeControl& aControl,
    MUiEventConsumer& aConsumer)
{
    // Can run in non eswt thread

    // Prepare event data
    TDcEventData data;
    data.iType = EDSAResourcesCallbackEvent;
    data.iParam = (void*)&aControl;
    data.iConsumer = (void*)&aConsumer;

    // Add event to the queue
    iEventQueue.Send(data);
}


// ---------------------------------------------------------------------------
// CSwtDcObserver::InvokeUICallback
// Asynchronously invokes aConsumer->MdcUICallback
// from eswt thread.
// The call may origin in other than eswt thread.
// ---------------------------------------------------------------------------
//
void CSwtDcObserver::InvokeUICallback(
    MUiEventConsumer& aConsumer,
    TInt aCallbackId)
{
    // Can run in non eswt thread

    // Prepare event data
    TDcEventData data;
    data.iType = EUICallbackEvent;
    data.iConsumer = (void*)&aConsumer;
    data.iParam = (void*)aCallbackId;

    // Add event to the queue
    iEventQueue.Send(data);
}


// ---------------------------------------------------------------------------
// CSwtDcObserver::RunL
// Handles an active object’s request completion event.
// Processes the first available event in queue.
// ---------------------------------------------------------------------------
//
void CSwtDcObserver::RunL()
{
    TDcEventData data;

    // Get the event
    iEventQueue.Receive(data);

    // Process the event
    switch (data.iType)
    {
    case EDcEvent:
        DoDcEvent((MSwtDcEventConsumer*)data.iConsumer, (TInt)data.iParam);
        break;
    case EDSAResourcesCallbackEvent:
        DoInvokeDSAResourcesCallback((CCoeControl*)data.iParam,
                                     (MUiEventConsumer*)data.iConsumer);
        break;
    case EUICallbackEvent:
        DoInvokeUICallback((MUiEventConsumer*)data.iConsumer,
                           (TInt)data.iParam);
        break;
    }

    iEventQueue.NotifyDataAvailable(iStatus);
    SetActive();
}


// ---------------------------------------------------------------------------
// CSwtDcObserver::DoCancel
// Implements cancellation of an outstanding request.
// ---------------------------------------------------------------------------
//
void CSwtDcObserver::DoCancel()
{
    iEventQueue.CancelDataAvailable();
}


// ---------------------------------------------------------------------------
// CSwtDcObserver::DoDcEvent
// Sends an event into a consumer.
// ---------------------------------------------------------------------------
//
void CSwtDcObserver::DoDcEvent(MSwtDcEventConsumer* aConsumer, TInt aType)
{
    aConsumer->HandleDcEvent(aType);
}


// ---------------------------------------------------------------------------
// CSwtDcObserver::DoInvokeDSAResourcesCallback
// Processes the event invoked by InvokeDSAResourcesCallback.
// ---------------------------------------------------------------------------
//
void CSwtDcObserver::DoInvokeDSAResourcesCallback(
    CCoeControl* aControl,
    MUiEventConsumer *aConsumer)
{
    RWsSession&      session = aControl->ControlEnv()->WsSession();
    CWsScreenDevice* device  = aControl->ControlEnv()->ScreenDevice();
    RDrawableWindow* window  = aControl->DrawableWindow();

    aConsumer->MdcDSAResourcesCallback(session, *device, *window);
}


// ---------------------------------------------------------------------------
// CSwtDcObserver::DoInvokeUICallback
// Processes the event invoked by InvokeUICallback.
// ---------------------------------------------------------------------------
//
void CSwtDcObserver::DoInvokeUICallback(
    MUiEventConsumer *aConsumer,
    TInt aCallbackId)
{
    aConsumer->MdcUICallback(aCallbackId);
}

