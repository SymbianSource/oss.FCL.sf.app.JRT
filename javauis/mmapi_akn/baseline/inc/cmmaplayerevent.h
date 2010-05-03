/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is used to post events to the java.
*
*/


#ifndef CMMAPLAYEREVENT_H
#define CMMAPLAYEREVENT_H

//  INCLUDES
#include "cmmaevent.h"

//  CLASS DECLARATION
/**
*   This class is used to post events to the java.
*/

NONSHARABLE_CLASS(CMMAPlayerEvent): public CMMAEvent
{
public:
    enum TEventType
    {
        EStarted = 0,
        EStopped = 1,
        EStoppedAtTime = 2,
        EEndOfMedia = 3,
        EDurationUpdated = 4,
        EAudioDeviceUnavailable = 5,
        EVolumeChanged = 6,
        ESizeChanged = 7,
        EError = 8,
        EClosed = 9,
        ERecordStarted = 10,
        ERecordStopped = 11,
        EPrivateDataAvailable = 12,
        EPrivateDataOverwritten = 13,
        EBufferingStarted = 14,
        EBufferingEStopped = 15,
        ERecordError = 16,
        ENOKIA_EXTERNAL_VOLUME_EVENT = 17, // Event ID for Nokia volume key
        EAudioOutputPreferenceChangeEvent =18 // Event ID for Audio Routing Preference Change
    };

private:
    enum TEventParams
    {
        ENormal = 0,    // string and object
        EString,        // int and string
        ELong,          // int and long
        EObject         // int and object
    };

public:
    /**
     * Constructor. One of set methods must be called before sending this
     * event.
     * @param aNotifyObject Java Objects.
     * @param aHandleEventMethod Java method.
     * @param aDisposable Defaults to disposable event.
     */
    CMMAPlayerEvent(jobject aNotifyObject,
                    jmethodID aHandleEventMethod,
                    TDisposability aDisposable = EDisposableEvent);

    /**
     * Destructor.
     */
    ~CMMAPlayerEvent();

    /**
     * Sets event data.
     * @param aEventType Type of the event.
     * @param aEventData Data to be send to Java
     */
    void SetLongEvent(TEventType aEventType, const TInt64& aEventData);

    /**
     * Sets event data.
     * @param aEventType Type of the event.
     * @param aEventData Data to be send to Java
     */
    void SetStringEventL(TEventType aEventType, const TDesC& aEventData);

    /**
     * Sets event data.
     * @param aEventType Type of the event.
     * @param aEventData Data to be send to Java
     */
    void SetObjectEventL(TEventType aEventType, const jobject aEventData);
private: // from CJavaEvent
    void Dispatch(JNIEnv& aJni);

private: // Data
    TEventType iEventType;
    TEventParams iEventParams;

    // Used when sending ELong event.
    TInt64 iLongEventData;

    // Owned event data. Used when sending EString event.
    HBufC* iStringEventData;
    // Owned event type. Used when sending ENormal event.
    HBufC* iStringEventType;

    // Used when sending ENormal or EObject event.
    jobject iObjectEventData;
};

#endif // CMMAPLAYEREVENT_H
