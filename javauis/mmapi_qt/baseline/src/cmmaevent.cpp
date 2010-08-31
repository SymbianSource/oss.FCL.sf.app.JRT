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


//  INCLUDE FILES
#include <logger.h>

#include "cmmaevent.h"

CMMAEvent::CMMAEvent(jobject aNotifyObject,
                     jmethodID aHandleEventMethod,
                     TDisposability /*aDisposable*/):
        iListenerObject(aNotifyObject),
        iHandleEventMethod(aHandleEventMethod)
{
}


CMMAEvent::CMMAEvent(TDisposability /*aDisposable*/)
{
}


void CMMAEvent::SetEventData(TInt aEventData)
{
    iEventData = aEventData;
}


// from CJavaEvent
void CMMAEvent::Dispatch(JNIEnv& aJni)
{
    LOG1(EJavaMMAPI, EInfo, "MMA::CMMAEvent::Dispatch iEventData=%d", iEventData);
    LOG1(EJavaMMAPI, EInfo, "MMA::CMMAEvent::Dispatch iListenerObject=%d", iEventData);

    aJni.CallVoidMethod(iListenerObject,
                        iHandleEventMethod,
                        iEventData);
}

//  END OF FILE
