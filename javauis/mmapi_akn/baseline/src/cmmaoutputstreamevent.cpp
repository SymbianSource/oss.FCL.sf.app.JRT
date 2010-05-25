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


#include <jdebug.h>

#include "cmmaoutputstreamevent.h"

CMMAOutputStreamEvent::~CMMAOutputStreamEvent()
	{
		iMutex.Close();
	}
	
CMMAOutputStreamEvent::CMMAOutputStreamEvent(jmethodID aHandleEventMethod,
        jobject aNotifyObject)
        : CMMAEvent(EReusableEvent),
        iHandleEventMethod(aHandleEventMethod),
        iListenerObject(aNotifyObject)
{
    iState = EMMAEventNotActive;
    iMutex.CreateLocal();
}

void CMMAOutputStreamEvent::SetLength(TInt aLength)
{
		iMutex.Wait();
    iLength = aLength;
    iState = EMMAEventActive;
    iMutex.Signal();
}


void CMMAOutputStreamEvent::SetStatus(TInt aStatus)
{
		iMutex.Wait();
    iStatus = aStatus;
    iState = EMMAEventActive;
    iMutex.Signal();
}

void CMMAOutputStreamEvent::SetState(CMMAOutputStreamEvent::TMMAOutputStreamState aState)
{
		iMutex.Wait();
    iState = aState;
    iMutex.Signal();
}

CMMAOutputStreamEvent::TMMAOutputStreamState CMMAOutputStreamEvent::State()
{
    return iState;
}

void CMMAOutputStreamEvent::Dispatch(JNIEnv& aJni)
{
    DEBUG_INT2("CMMAOutputStreamEvent::Dispatch, len=%d status=%d-",
               iLength, iStatus);
		iMutex.Wait();
    if (iState == EMMAEventActive)
    {
        aJni.CallVoidMethod(iListenerObject,
                            iHandleEventMethod,
                            iLength,
                            iStatus);
        iState = EMMAEventNotActive;
    }
    else if (iState == EMMADeleteEvent)
    {
        // This is done because some
        // event may still exist in the event server when
        // the sender has been deleted.

        delete this;
    }
    iMutex.Signal();
    // else event is not active
}

//  END OF FILE
