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


#ifndef CMMAEVENT_H
#define CMMAEVENT_H

// #include <mevents.h>
#include "mmapiutils.h"

class MMAFunctionServer;

//  CLASS DECLARATION
/**
*   This class is used to post events to the java.
*
*
*/

NONSHARABLE_CLASS(CMMAEvent) // public CJavaEvent< MMAFunctionServer >
{
public:

    enum {EEventPriority = 0, ENotifyPriority = 1, ELastPriority = 1};
    enum TDisposability { EDisposableEvent, EReusableEvent };

    CMMAEvent(jobject aNotifyObject,
    jmethodID aHandleEventMethod,
    TDisposability aDisposable = EDisposableEvent);

protected:
    CMMAEvent(TDisposability aDisposable);

public:
    void SetEventData(TInt aEventData);


public: // from CJavaEvent
    virtual void Dispatch(JNIEnv& aJni);

protected:
    jobject iListenerObject;
    jmethodID iHandleEventMethod;
    TInt iEventData;
};

#endif // CMMAEVENT_H
