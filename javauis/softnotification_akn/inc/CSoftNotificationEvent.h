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


#ifndef CSOFTNOTIFICATIONEVENT_H
#define CSOFTNOTIFICATIONEVENT_H

// INCLUDES
#include <mevents.h>
#include "CSoftNotificationEventSource.h"


// CLASS DECLARATION
/**
 *  Observer event for soft notification.
 *  This class delegates native events to java implementation.
 *
 *  @since 3.2
 */
NONSHARABLE_CLASS(CSoftNotificationEvent) :
        public CJavaEvent<CSoftNotificationEventSource>
{
public: // Constructors

    /**
     * Constructor.
     *
     * @param aNotifyObject Object to be notified.
     * @param aHandleEventMethod Java method to be called.
     * @param aEventData Data to be posted.
     */
    CSoftNotificationEvent(
        jobject aNotifyObject,
        jmethodID aHandleEventMethod,
        TInt aEventData);

private: // From CJavaEvent

    /**
     * Dispatches the event.
     *
     * @param aJni JNI environment to be used in dispatching.
     */
    void Dispatch(JNIEnv& aJni);

private: // Event data
    jobject   iListenerObject;      // Object to be notified.
    jmethodID iEventHandlerMethod;  // Java method to be called.
    TInt      iEventData;           // Data to be posted.
};

#endif // CSOFTNOTIFICATIONEVENT_H


