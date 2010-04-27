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
* Description:  Event class for native soft notifications.
*
*/


// INCLUDE FILES
#include "CSoftNotificationEvent.h"
#include <jdebug.h>


// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CSoftNotification::CSoftNotificationEvent
// -----------------------------------------------------------------------------
//
CSoftNotificationEvent::CSoftNotificationEvent(
    jobject aNotifyObject,
    jmethodID aHandleEventMethod,
    TInt aEventData) :
        iListenerObject(aNotifyObject),
        iEventHandlerMethod(aHandleEventMethod),
        iEventData(aEventData)
{
    DEBUG("CSoftNotificationEvent::CSoftNotificationEvent");
}

// -----------------------------------------------------------------------------
// CSoftNotificationEvent::Dispatch
// -----------------------------------------------------------------------------
//
void CSoftNotificationEvent::Dispatch(JNIEnv& aJniEnv)
{
    DEBUG("CSoftNotificationEvent::Dispatch +");

    aJniEnv.CallVoidMethod(
        iListenerObject,
        iEventHandlerMethod,
        iEventData);

    DEBUG("CSoftNotificationEvent::Dispatch -");
}

//  End of File
