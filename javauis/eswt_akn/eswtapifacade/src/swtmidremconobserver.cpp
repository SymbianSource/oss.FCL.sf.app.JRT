/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include <remconcoreapitarget.h>
#include <remconinterfaceselector.h>
#include <aknconsts.h>

#include <javaremconobserver.h>
#include <javaremconmanager.h>

#include "swtmidremconobserver.h"

// Keyboard delays and repeat periods
const TInt KFirstTimerExpiryInterval = KAknKeyboardRepeatInitialDelay;
// For second time onwards ,the duration of the time interval, is below
// Should not be greater than 1 Minute
const TInt KTimerExpiryInterval = KAknStandardKeyboardRepeatRate;


EXPORT_C CSwtMIDRemConObserver* CSwtMIDRemConObserver::NewL()
{
    CSwtMIDRemConObserver* self = new(ELeave) CSwtMIDRemConObserver();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}


EXPORT_C CSwtMIDRemConObserver::~CSwtMIDRemConObserver()
{
    iListeners.Close();
    if (iJavaRemConManager)
    {
        iJavaRemConManager->RemoveObserver(*iJavaRemConObserver);
        delete iJavaRemConObserver;
        iJavaRemConObserver = NULL;
        delete iJavaRemConManager;
        iJavaRemConManager = NULL;
    }

    if (iRepeatTimer)
    {
        iRepeatTimer->Cancel();
        delete iRepeatTimer;
    }
}


EXPORT_C void CSwtMIDRemConObserver::AddMediaKeysListenerL(MSwtMediaKeysListener* aListener)
{
    User::LeaveIfError(iListeners.Append(aListener));
}


EXPORT_C void CSwtMIDRemConObserver::RemoveMediaKeysListener(MSwtMediaKeysListener* aListener)
{
    for (TInt i = 0;  i < iListeners.Count(); i++)
    {
        if (iListeners[i] == aListener)
        {
            iListeners.Remove(i);
        }
    }

}


// ---------------------------------------------------------------------------
// This function handles the events when following buttons are press/click/rel
// - Play, Stop, Forward, Backward
// ---------------------------------------------------------------------------
void CSwtMIDRemConObserver::MrccatoCommand(TRemConCoreApiOperationId aOperationId,
        TRemConCoreApiButtonAction aButtonAct)
{
    MSwtMediaKeysListener::TMediaKey mediaKey = MSwtMediaKeysListener::ESwtMediaKeyNone;

    switch (aOperationId)
    {
    case ERemConCoreApiPausePlayFunction:
        mediaKey = MSwtMediaKeysListener::ESwtMediaKeyPlay;
        break;
    case ERemConCoreApiStop:
        mediaKey = MSwtMediaKeysListener::ESwtMediaKeyStop;
        break;
    case ERemConCoreApiForward:
    case ERemConCoreApiFastForward:
        mediaKey = MSwtMediaKeysListener::ESwtMediaKeyNext;
        break;
    case ERemConCoreApiBackward:
    case ERemConCoreApiRewind:
        mediaKey = MSwtMediaKeysListener::ESwtMediaKeyPrevious;
        break;
    default:
        return;
    }

    TInt eventType = EEventKeyDown;

    // Next/Previous keys produce Click event only if pressed briefly, otherwise they
    // produce Press events followed by Release event when the key is released.

    // Play/Stop keys produce Click event immediately when the key is pressed.

    switch (aButtonAct)
    {
    case ERemConCoreApiButtonPress: // Long key press
        iKeyRepeating = mediaKey;
        break;
    case ERemConCoreApiButtonClick: // Short key press
        eventType = EEventKeyDown;
        break;
    case ERemConCoreApiButtonRelease: // Key released after long key press.
        eventType = EEventKeyUp;
        break;
    default:
        return;
    }


    // Media keys do not provide keyReleased events when a key is pressed shortly.
    // However, keyReleased events are posted after long key press of next and previous keys.

    // Play and Stop does not provide keyRepeated nor keyReleased Java events at all.
    // Reason for these limitation is RemCon that does not provide such events for us.

    iRepeatTimer->Cancel(); //Repeat timer is always canceled if any Media key event occurs.

    if (aButtonAct == ERemConCoreApiButtonPress
            && !(mediaKey == MSwtMediaKeysListener::ESwtMediaKeyPlay || mediaKey == MSwtMediaKeysListener::ESwtMediaKeyStop))
    {
        // keyPressed event is posted first, then timer for key repeats is started.
        PostRawEventToListeners(mediaKey, eventType);
        iRepeatTimer->Start(KFirstTimerExpiryInterval, KTimerExpiryInterval,
                            TCallBack(RepeatTimerCallback, this));
    }
    else if (aButtonAct == ERemConCoreApiButtonRelease
             && !(mediaKey == MSwtMediaKeysListener::ESwtMediaKeyPlay || mediaKey == MSwtMediaKeysListener::ESwtMediaKeyStop))
    {
        // post key release event after holding Next/Previous key.
        PostRawEventToListeners(mediaKey, eventType);
        iKeyRepeating = MSwtMediaKeysListener::ESwtMediaKeyNone;
    }
    else if ((aButtonAct == ERemConCoreApiButtonRelease ||  aButtonAct == ERemConCoreApiButtonPress)
             && (mediaKey == MSwtMediaKeysListener::ESwtMediaKeyPlay || mediaKey == MSwtMediaKeysListener::ESwtMediaKeyStop))
    {
        // If Press or Release event occur for Play or Stop keys we just ignore them.
        // This should not happen normally, but is possible e.g. in certain error situations.
        // Also some media key accessories differ from each other so that some might post these events.
        iKeyRepeating = MSwtMediaKeysListener::ESwtMediaKeyNone;
        return;
    }
    else
    {
        PostClickEventToListeners(mediaKey);
        iKeyRepeating = MSwtMediaKeysListener::ESwtMediaKeyNone;
    }
}


void CSwtMIDRemConObserver::MrccatoPlay(TRemConCoreApiPlaybackSpeed /*aSpeed*/, TRemConCoreApiButtonAction aButtonAct)
{
    switch (aButtonAct)
    {
    case ERemConCoreApiButtonPress:
    case ERemConCoreApiButtonClick:
    {
        PostClickEventToListeners(MSwtMediaKeysListener::ESwtMediaKeyPlay);
        break;
    }
    default:
    {
        break;
    }
    }
}

void CSwtMIDRemConObserver::MrceoError(TInt /*aError*/)
{
    iRepeatTimer->Cancel();
    iKeyRepeating = MSwtMediaKeysListener::ESwtMediaKeyNone;
}


CSwtMIDRemConObserver::CSwtMIDRemConObserver()
{
}


void CSwtMIDRemConObserver::ConstructL()
{
    iJavaRemConObserver = CJavaRemConObserver::NewL(*this);
    iJavaRemConManager = CJavaRemConManager::NewL();
    iJavaRemConManager->SetObserverL(*iJavaRemConObserver);

    // Timer for implementing key repeat feature.
    iRepeatTimer = CPeriodic::NewL(CActive::EPriorityStandard);

    iKeyRepeating = MSwtMediaKeysListener::ESwtMediaKeyNone;
}


void CSwtMIDRemConObserver::PostRawEventToListeners(MSwtMediaKeysListener::TMediaKey aKeyCode, TInt aEventType)
{
    TKeyEvent event;
    event.iCode = 0;
    event.iScanCode = aKeyCode;
    event.iModifiers = 0;
    event.iRepeats = 0; // no repeats

    if (iKeyRepeating != MSwtMediaKeysListener::ESwtMediaKeyNone)
    {
        event.iRepeats = 1; // repeats
    }
    else if (aEventType == EEventKeyUp)
    {
        event.iModifiers |= EModifierKeyUp;
    }

    for (TInt i = 0;  i < iListeners.Count(); i++)
    {
        iListeners[i]->HandleMediaKeyEvent(event, aEventType);
    }
}

void CSwtMIDRemConObserver::PostClickEventToListeners(MSwtMediaKeysListener::TMediaKey aKeyCode)
{
    PostRawEventToListeners(aKeyCode, EEventKeyDown);
    PostRawEventToListeners(aKeyCode, EEventKey);
    PostRawEventToListeners(aKeyCode, EEventKeyUp);
}


TInt CSwtMIDRemConObserver::RepeatTimerCallback(TAny* aThis)
{
    CSwtMIDRemConObserver* observer = static_cast<CSwtMIDRemConObserver*>(aThis);
    observer->HandleRepeatTimerEvent();
    return 0;
}


void CSwtMIDRemConObserver::HandleRepeatTimerEvent()
{
    if (iKeyRepeating != MSwtMediaKeysListener::ESwtMediaKeyNone)
    {
        PostRawEventToListeners(iKeyRepeating, EEventKey);
    }
}

//  End of File
