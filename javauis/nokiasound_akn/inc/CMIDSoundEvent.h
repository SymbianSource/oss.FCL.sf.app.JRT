/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class for sound related events.
*
*/


#ifndef CMIDSOUNDEVENT_H
#define CMIDSOUNDEVENT_H

#include <jni.h>

//  CLASS DEFINITION

/**
 * Class for sound related events.
 */
NONSHARABLE_CLASS(CMIDSoundEvent)
{
public: // constructor
    CMIDSoundEvent(TInt aEvent);

public: // new methods
    /**
     * Setter for listener
     * @param aListener this event will be sent to this listener
     */
    void SetListener(jobject aListener);

    /**
     * Setter for method ID
     * @param aMethodID this event will be sent by calling this method
     */
    void SetMethodID(jmethodID aMethodID);

    /**
     * Adds a global reference to peer object, this reference is
     * removed when the event is dispatched. This prevents peer
     * object from being deleted by garbage collector before the
     * event has been dispatched.
     */
    void AddGlobalRef(JavaVM* javaVM);


private:
    void Dispatch(JNIEnv& aJni);

private:
    jobject iListener;
    jmethodID iMethodID;
    TInt iEvent;
    jobject iGlobalRef;
    // Number of times the global reference has been added.
    // The actual global reference is removed when the counter reaches zero.
    TInt iRefCount;

};


#endif // CMIDSOUNDEVENT_H
