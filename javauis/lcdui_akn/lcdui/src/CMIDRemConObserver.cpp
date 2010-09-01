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
* Description:  ?Description
*
*/


// CRemConCoreApiTarget* iCoreTarget
#include <remconcoreapitarget.h>
// CRemConInterfaceSelector* iInterfaceSelector;
#include <remconinterfaceselector.h>
// using avkon constants
#include <aknconsts.h>
#include <j2me/jdebug.h>

#include "CMIDRemConObserver.h"
#include "CMIDKeyDecoder.h"


// Keyboard delays and repeat periods
const TInt KFirstTimerExpiryInterval = KAknKeyboardRepeatInitialDelay;
// For second time onwards ,the duration of the time interval, is below
// Should not be greater than 1 Minute
const TInt KTimerExpiryInterval = KAknStandardKeyboardRepeatRate;


CMIDRemConObserver* CMIDRemConObserver::NewL()
{
    CMIDRemConObserver* self = new(ELeave) CMIDRemConObserver();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}


CMIDRemConObserver::~CMIDRemConObserver()
{
    DEBUG("CMIDRemConObserver::~CMIDRemConObserver() +");

    if (iJavaRemConManager != NULL)
    {
        iJavaRemConManager->RemoveObserver(*iJavaRemConObserver);
    }
    if (iJavaRemConObserver != NULL)
    {
        delete iJavaRemConObserver;
        iJavaRemConObserver = NULL;
    }
    if (iJavaRemConManager != NULL)
    {
        delete iJavaRemConManager;
        iJavaRemConManager = NULL;
    }
    iListeners.Close();

    if (iRepeatTimer)
    {
        iRepeatTimer->Cancel();
        delete iRepeatTimer;
    }

    DEBUG("CMIDRemConObserver::~CMIDRemConObserver() -");
}


void CMIDRemConObserver::AddMediaKeysListenerL(MMIDMediaKeysListener* aListener)
{
    User::LeaveIfError(iListeners.Append(aListener));
}


void CMIDRemConObserver::RemoveMediaKeysListener(MMIDMediaKeysListener* aListener)
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
void CMIDRemConObserver::MrccatoCommand(TRemConCoreApiOperationId aOperationId,
                                        TRemConCoreApiButtonAction aButtonAct)
{
    DEBUG_INT2("CMIDRemConObserver::MrccatoCommand - JAVA operationId %d JAVA buttonAct %d",
               aOperationId, aButtonAct);

    MMIDMediaKeysListener::TMediaKey mediaKey = MMIDMediaKeysListener::EMIDMediaKeyNone;

    switch (aOperationId)
    {
    case ERemConCoreApiPausePlayFunction:
        mediaKey = MMIDMediaKeysListener::EMIDMediaKeyPlay;
        break;
    case ERemConCoreApiStop:
        mediaKey = MMIDMediaKeysListener::EMIDMediaKeyStop;
        break;
    case ERemConCoreApiForward:
    case ERemConCoreApiFastForward:
        mediaKey = MMIDMediaKeysListener::EMIDMediaKeyNext;
        break;
    case ERemConCoreApiBackward:
    case ERemConCoreApiRewind:
        mediaKey = MMIDMediaKeysListener::EMIDMediaKeyPrevious;
        break;
    default:
        return;
    }

    TMIDKeyEvent::TEvent keyEvent = TMIDKeyEvent::EPressed;

    switch (aButtonAct)
    {
    case ERemConCoreApiButtonPress: // Long key press
        iKeyRepeating = mediaKey;
        break;
    case ERemConCoreApiButtonClick: // Short key press
        keyEvent = TMIDKeyEvent::EPressed;
        break;
    case ERemConCoreApiButtonRelease: // Key released after long key press.
        keyEvent = TMIDKeyEvent::EReleased;
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
            && !(mediaKey == MMIDMediaKeysListener::EMIDMediaKeyPlay || mediaKey == MMIDMediaKeysListener::EMIDMediaKeyStop))
    {
        // keyPressed event is posted first, then timer for key repeats is started.
        PostEventToListeners(mediaKey, TMIDKeyEvent::EPressed);
        iRepeatTimer->Start(KFirstTimerExpiryInterval, KTimerExpiryInterval,
                            TCallBack(RepeatTimerCallback, this));
    }
    else if ((aButtonAct == ERemConCoreApiButtonRelease ||  aButtonAct == ERemConCoreApiButtonPress)
             && (mediaKey == MMIDMediaKeysListener::EMIDMediaKeyPlay || mediaKey == MMIDMediaKeysListener::EMIDMediaKeyStop))
    {
        // If Press or Release event occur for Play or Stop keys we just ignore them.
        // This should not happen normally, but is possible e.g. in certain error situations.
        // Also some media key accessories differ from each other so that some might post these events.
        return;
    }
    else
    {
        PostEventToListeners(mediaKey, keyEvent);
    }

    DEBUG("CMIDRemConObserver::MrccatoCommand -");
}


void CMIDRemConObserver::MrccatoPlay(TRemConCoreApiPlaybackSpeed aSpeed, TRemConCoreApiButtonAction aButtonAct)
{
    DEBUG("CMIDRemConObserver::MrccatoPlay +");
    DEBUG_INT2("RemConObserver::MrccatoPlay - JAVA speed %d JAVA buttonAct %d", aSpeed, aButtonAct);
    (void)aSpeed; // Just to suppress warning in release build

    switch (aButtonAct)
    {
    case ERemConCoreApiButtonPress:
    case ERemConCoreApiButtonClick:
    {
        PostEventToListeners(MMIDMediaKeysListener::EMIDMediaKeyPlay, TMIDKeyEvent::EPressed);
        break;
    }
    default:
    {
        break;
    }
    }
    DEBUG("CMIDRemConObserver::MrccatoPlay -");
}

void CMIDRemConObserver::MrceoError(TInt /*aError*/)
{
    DEBUG("CMIDRemConObserver::MrceoError +");
    iRepeatTimer->Cancel();
    DEBUG("CMIDRemConObserver::MrceoError -");
}


CMIDRemConObserver::CMIDRemConObserver()
{
}


void CMIDRemConObserver::ConstructL()
{
    DEBUG("CMIDRemConObserver::ConstructL +");
    iJavaRemConObserver = CJavaRemConObserver::NewL(*this);
    iJavaRemConManager = CJavaRemConManager::NewL();
    iJavaRemConManager->SetObserverL(*iJavaRemConObserver);
    // Timer for implementing key repeat feature.
    iRepeatTimer = CPeriodic::NewL(CActive::EPriorityStandard);

    iKeyRepeating = MMIDMediaKeysListener::EMIDMediaKeyNone;

    DEBUG("CMIDRemConObserver::ConstructL -");
}


void CMIDRemConObserver::PostEventToListeners(MMIDMediaKeysListener::TMediaKey aKeyCode, TMIDKeyEvent::TEvent aEventType)
{
    DEBUG("CMIDRemConObserver::PostEventToListeners +");

    TMIDKeyEvent event;
    event.iKeyCode = aKeyCode;
    event.iEvents = aEventType;

    if (event.iEvents == TMIDKeyEvent::ERepeated)
    {
        event.iRepeats = 1; // repeats
    }
    else
    {
        event.iRepeats = 0; // no repeats
    }

    for (TInt i = 0;  i < iListeners.Count(); i++)
    {
        iListeners[i]->HandleMediaKeyEvent(event);
    }

    DEBUG("CMIDRemConObserver::PostEventToListeners -");
}


TInt CMIDRemConObserver::RepeatTimerCallback(TAny* aThis)
{
    CMIDRemConObserver* observer = static_cast<CMIDRemConObserver*>(aThis);
    observer->HandleRepeatTimerEvent();
    return 0;
}


void CMIDRemConObserver::HandleRepeatTimerEvent()
{
    DEBUG("CMIDRemConObserver::HandleTimerEvent +");
    if (iKeyRepeating != MMIDMediaKeysListener::EMIDMediaKeyNone)
    {
        DEBUG("CMIDRemConObserver::HandleTimerEvent calling PosTEventToListeners +");
        PostEventToListeners(iKeyRepeating, TMIDKeyEvent::ERepeated);
        DEBUG("CMIDRemConObserver::HandleTimerEvent calling PostEventToListeners -");
    }
    DEBUG("CMIDRemConObserver::HandleTimerEvent -");
}

//  End of File
