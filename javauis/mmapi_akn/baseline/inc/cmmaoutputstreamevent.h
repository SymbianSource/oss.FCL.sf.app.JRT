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


#ifndef CMMAOUTPUTSTREAMEVENT_H
#define CMMAOUTPUTSTREAMEVENT_H

#include <e32std.h>
#include "cmmaevent.h"


//  CLASS DECLARATION
/**
*   This class is used to post events to the java.
*
*
*/

NONSHARABLE_CLASS(CMMAOutputStreamEvent): public CMMAEvent
{
public:
    enum TMMAOutputStreamState
    {
        EMMAEventActive,    // event is waiting dispatching
        EMMAEventNotActive, // not added to event server
        EMMADeleteEvent     // dispatch deletes this event
    };
public:
    CMMAOutputStreamEvent(jmethodID aHandleEventMethod,
                          jobject aNotifyObject);
		~CMMAOutputStreamEvent();
    // status of the source stream
    void SetStatus(TInt aStatus);

    // length of the available output stream data
    void SetLength(TInt aLength);

    /**
     * Sets new state.
     * @param aState TMMAOutputStreamState.
     */
    void SetState(TMMAOutputStreamState aState);

    TMMAOutputStreamState State();
private: // from CJavaEvent
    void Dispatch(JNIEnv& aJni);

private:
    jmethodID iHandleEventMethod;
    jobject iListenerObject;
    TInt iLength;
    TInt iStatus;

    /**
     * State of this event.
     */
    TMMAOutputStreamState iState;
    
    RMutex iMutex;
};

#endif // CMMAOUTPUTSTREAMEVENT_H
